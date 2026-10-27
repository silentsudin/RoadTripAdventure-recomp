#!/usr/bin/env python3
"""Verify omitted call arguments against the ELF; write derived callargs.tsv.

Only incoming integer parameters survive as fill candidates. Branches, unknown
instructions, call clobbers, constants and stack addresses fail closed.
"""
from __future__ import annotations
import os
import re
import subprocess
from collections import Counter
from pathlib import Path

from transliterate import FN, ROOT, mask_calls, match_bal, sig_name_args, split_args

OUT = ROOT / "build/decomp/callargs.tsv"
OBJDUMP = "/opt/homebrew/opt/llvm/bin/llvm-objdump"


def signature(text):
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)
    name, args = sig_name_args(text.split("{", 1)[0])
    return name.replace("::", "_"), ([] if args.strip() in ("", "void") else
                                    [p for p in split_args(args) if p != "..."])


def calls(text, names):
    text = mask_calls(text)
    text = re.sub(r"\bswitchD_([0-9a-fA-F]+)::", r"switchD_\1_", text)
    for m in re.finditer(r"\b(\w+)\s*\(", text):
        if m.group(1) in names:
            end = match_bal(text, m.end() - 1)
            if end > 0:
                yield m.group(1), split_args(text[m.end():end - 1])


def integer_params(params):
    # Ghidra's leading float parameters are in f12/f13, not a0/a1.
    return [(re.search(r"\bparam_\d+\b", p)[0] if re.search(r"\bparam_\d+\b", p) else "?")
            for p in params if not re.match(r"^(?:float|double)\s+\w+$", p)]


def reassigned(body, name):
    return re.search(r"\b" + name + r"\s*(?:[+*/%&|^<>-]*=(?!=)|\+\+|--)|"
                     r"(?:\+\+|--)\s*\b" + name + r"\b", mask_calls(body)) is not None


def reg(s):
    aliases = {"zero": 0, "at": 1, "v0": 2, "v1": 3, "gp": 28,
               "sp": 29, "fp": 30, "ra": 31}
    s = s.strip().removeprefix("$")
    if s.isdigit():
        return int(s)
    if s in aliases:
        return aliases[s]
    if re.fullmatch(r"a[0-3]", s):
        return 4 + int(s[1])
    return None


def step(state, op, args):
    """Keep only exact incoming-param provenance; never infer arbitrary values."""
    ps = [p.strip() for p in args.split(",")]
    dst = reg(ps[0]) if ps else None
    src = reg(ps[1]) if len(ps) > 1 else None
    if op in ("move", "or", "addu", "daddu") and src is not None:
        zero = op == "move" or (len(ps) == 3 and reg(ps[2]) == 0)
        value = state[src] if zero else None
    else:
        value = None
    if op in ("nop", "sync", "mult", "multu", "div", "divu"):
        return
    if op in ("sb", "sh", "sw", "sd", "sq", "swc1", "sdc1", "sqc2",
              "mtc1", "dmtc1", "ctc1", "mtc0", "ctc2", "cache", "pref"):
        return
    if op.endswith((".s", ".d")) or (ps and re.fullmatch(r"\$f\d+", ps[0])):
        return
    if op in ("move", "or", "and", "xor", "nor", "addu", "daddu", "addiu",
              "daddiu", "subu", "dsubu", "lui", "ori", "andi", "xori", "li",
              "sll", "srl", "sra", "sllv", "srlv", "srav", "dsll", "dsrl",
              "dsra", "dsll32", "dsrl32", "dsra32", "slt", "sltu", "slti",
              "sltiu", "lb", "lbu", "lh", "lhu", "lw", "lwu", "ld", "lq",
              "lwl", "lwr", "ldl", "ldr", "mflo", "mfhi", "mfc1", "mfc0"):
        if dst is not None and dst != 0:
            state[dst] = value
    else:
        # R5900/MMI instructions llvm cannot decode must not preserve stale tags.
        state[:] = [None] * 32


def call_states(insns, incoming):
    state = [None] * 32
    for i, name in enumerate(incoming[:4]):
        state[i + 4] = name
    found = []
    delay = False
    for i, (pc, op, args) in enumerate(insns):
        if delay:
            delay = False
            continue
        if op in ("jal", "jalr"):
            live = state.copy()
            if i + 1 < len(insns) and insns[i + 1][0] == pc + 4:
                step(live, *insns[i + 1][1:])
            else:
                live = [None] * 32
            target = re.match(r"0x([0-9a-fA-F]+)", args) if op == "jal" else None
            found.append((pc, int(target[1], 16) if target else None, live[4:8]))
            state = live
            for r in list(range(1, 16)) + [24, 25, 31]:
                state[r] = None
            delay = True
        elif op.startswith("b") or op in ("j", "jr"):
            # No CFG guesses: any control-flow split invalidates provenance.
            state[:] = [None] * 32
        else:
            step(state, op, args)
    return found


def main():
    raw = {p.stem: p.read_text() for p in FN.glob("*.c")}
    sigs = {addr: signature(text) for addr, text in raw.items()}
    defs = {}
    gen = ROOT / "src/native/guest/gen"
    for p in sorted(gen.glob("*.cpp")):
        text = p.read_text()
        for m in re.finditer(r"RT_GUEST_FN\(\s*0x([0-9a-fA-F]+),\s*(?:guest::)?(\w+)\)", text):
            name = m[2]
            definition = re.search(r"\b(?:void|u?int(?:8|16|32|64)_t|int|float|double|long)\s+" +
                                   re.escape(name) + r"\s*\(([^)]*)\)\s*\{", text)
            if definition:
                params = [a for a in split_args(definition[1]) if a != "..."]
                defs[name] = (int(m[1], 16), len(params),
                              bool(params and re.search(r"\bq0\b", params[0]) and
                                   not re.search(r"\bq0\b", mask_calls(text[definition.end():]))))
    candidates = []
    for addr, text in sorted(raw.items()):
        if not (gen / (addr + ".cpp")).exists():
            continue
        body = text.split("{", 1)[1] if "{" in text else ""
        sites = list(calls(body, defs))
        totals = Counter(name for name, _ in sites)
        seen = Counter()
        for name, args in sites:
            ordinal = seen[name]
            seen[name] += 1
            target, arity, placeholder = defs[name]
            if len(args) < arity:
                candidates.append((addr, name, target, ordinal, len(args), arity, totals[name], placeholder))

    elf = Path(os.environ.get("RT_ELF", ROOT / "build/rom/SLUS_203.98"))
    disasm = subprocess.check_output([OBJDUMP, "-d", str(elf)], text=True)
    insns = []
    for line in disasm.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+(?:[0-9a-f]{2}\s+){4}\s*(\S+)(?:\s+(.*))?$", line)
        if m:
            insns.append((int(m[1], 16), m[2], m[3] or ""))
    rows = []
    for addr, name, target, ordinal, argc, arity, count, placeholder in candidates:
        header = re.search(r"@ 0x[0-9a-f]+ \.\. 0x([0-9a-f]+)", raw[addr])
        stop = int(header[1], 16) + 1 if header else int(addr, 16)
        machine = [x for x in insns if int(addr, 16) <= x[0] < stop]
        incoming = integer_params(sigs[addr][1])
        states = call_states(machine, incoming)
        sites = [(pc, live) for pc, dest, live in states if dest == target]
        indirect = [pc for pc, dest, _ in states if dest is None]
        evidence = ",".join(f"0x{pc:08x}" for pc, _ in sites)
        if not evidence:
            evidence = ",".join(f"0x{pc:08x}" for pc in indirect) or "-"
        reason = ""
        if placeholder:
            reason = "synthetic-q0-param"
        elif f"{target:08x}" not in sigs:
            reason = "callee-signature-unavailable"
        elif not sites:
            reason = "unresolved-jalr-target" if indirect else "no-matching-jal"
        elif count != 1 or len(sites) != 1:
            reason = "ambiguous-or-indirect-site"
        else:
            declared = sigs.get(f"{target:08x}", ("", []))[1]
            # Existing pseudocode params establish integer vs FPR positions;
            # hand-authored trailing params consume the next a-registers.
            nint = len(integer_params(declared[:argc]))
            missing = arity - argc
            live = sites[0][1][nint:nint + missing]
            if any(re.match(r"^(?:float|double)\s+\w+$", p) for p in declared[argc:arity]):
                reason = "missing-float-param"
            elif nint + missing > 4:
                reason = "beyond-a3"
            elif len(live) != missing or any(v is None for v in live):
                reason = "written-clobbered-or-control-flow"
            elif any(not re.fullmatch(r"param_[1-9]\d*", v) for v in live):
                reason = "nonstandard-caller-param"
            else:
                # Reassigned pseudocode params no longer denote entry values.
                body = raw[addr].split("{", 1)[1]
                if any(reassigned(body, v) for v in live):
                    reason = "caller-param-reassigned"
        verdict = "skip=" + reason if reason else "fill=" + ",".join(live)
        rows.append(f"{addr}\t{name}\t{ordinal}\t{argc}\t{arity}\t{evidence}\t{verdict}")
    OUT.parent.mkdir(parents=True, exist_ok=True)
    temporary = OUT.with_suffix(".tsv.tmp")
    temporary.write_text("caller\tcallee\toccurrence\targc\tdeclared\tjal\tverdict\n" + "\n".join(rows) + "\n")
    temporary.replace(OUT)
    filled = sum("\tfill=" in row for row in rows)
    print(f"{OUT}: {filled} fills, {len(rows) - filled} skips")


if __name__ == "__main__":
    main()
