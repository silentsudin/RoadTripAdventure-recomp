#!/usr/bin/env python3
"""Model machine prologue register-save stores in gen TUs (v2: + $ra).

The oracle's function prologues store $16-$23/$ra/$fp and float regs into
frame slots (sd $16,0x60($sp) ...); other frames later read those slots as
residue. Ghidra drops the stores, so native leaves stale bytes. This pass
re-emits every prologue store whose value is derivable from the machine
prologue alone: incoming int args ($4..$9 -> param_k), move/lui/ori/li
constants, propagated through `move` chains. $ra saves go through the
link-register model: direct calls to registered guest functions become
rtguest::callRA(0xJALpc, guest::name, args) at unique one-to-one call
sites, and the callee's FrameGuard captured that PC. float (swc1) saves
and old-register ($16-$23/$fp not provable in the prologue) saves stay a
documented residue class (values are the caller's old registers; no
shadow register file).

Writes run at frame entry — the same point the machine stores them, so a
slot both saved and later written by pseudocode keeps the pseudocode value
(last write wins, identical order).

Idempotent: existing PROLOGUE-SAVES blocks are stripped and re-emitted;
call sites already wrapped for a given (jal pc, callee) are skipped.

Usage: python3 scripts/native/emit_prologue_saves.py [--dry]
"""
import json, re, subprocess, sys, glob, os, collections

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ELF = os.path.join(ROOT, "build/rom/SLUS_203.98")
OBJDUMP = "/opt/homebrew/opt/llvm/bin/llvm-objdump"
GEN = os.path.join(ROOT, "src/native/guest/gen")
FRAMESIZES = os.path.join(ROOT, "build/scratch/w12-parity/framesizes.json")

INS = re.compile(r'^\s+([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*(\S+)\s*(.*)$')
REG_LINE = re.compile(r'RT_GUEST_FN\((0x[0-9a-f]+), guest::(\w+)\)')

def disasm():
    out = subprocess.run([OBJDUMP, "-d", "--triple=mipsel", ELF],
                         capture_output=True, text=True).stdout
    prog = []
    for line in out.splitlines():
        m = INS.match(line)
        if m:
            prog.append((int(m.group(1), 16), m.group(2), m.group(3)))
    return prog

def split_regs(rest):
    return [t.strip() for t in rest.split(',')]

STOP_OPS = {"jal", "jalr", "jr", "j", "beq", "bne", "bgtz", "blez", "bltz",
            "bgezal", "bgtzal", "bltzal", "blezal", "beql", "bnel"}
SAVE_OPS = {"sd", "sw"}
# Callee-saved shadow-file slots: sd/sw $16..$23 (and $fp) spill gS[k],
# swc1 $f20..$f31 spills gFS[k] (k = regnum - 16).
SREGS = {f"${r}": r - 16 for r in range(16, 24)}
SREGS["$fp"] = 8
FREGS = {f"$f{r}": r - 16 for r in range(20, 32)}

def parse_tu(path):
    """Return (guard_size, {int param names}) for a gen TU, or None."""
    s = open(path).read()
    m = re.search(r'FrameGuard rt_fg\((\d+)\)', s)
    size = int(m.group(1)) if m else 0
    sig = re.search(r'^[^\n]*guest::?\w*\(([^)]*)\)\s*\{', s, re.M)
    if not sig:
        sig = re.search(r'^\w[\w:]*\s+\w+\(([^)]*)\)\s*\{', s, re.M)
    if not sig:
        return None
    params = []
    for p in sig.group(1).split(','):
        p = p.strip()
        pm = re.match(r'(uint32_t|int32_t|undefined4|int|uint\w*)\s+(\w+)$', p)
        if pm:
            params.append(pm.group(2))
    return size, set(params)

BLOCK_START = "  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES"
def strip_block(src):
    i = src.find(BLOCK_START)
    if i < 0:
        return src, False
    j = src.find("\n  }\n", i)
    if j < 0:
        return src, False
    return src[:i] + src[j + len("\n  }\n"):], True

def prologue_emits(prog, idx, addr, size, params, stats):
    """Symbolic prologue walk; returns [(pc, op, off, kind, val)]."""
    consts, pmap, spk = {}, {}, {}
    snap = None  # fold state frozen at the first call/load (see seeds below)
    for r in range(4, 10):
        pmap[f"${r}"] = r - 3
    emits = []
    def try_save(pc, op, rest):
        m = re.match(r'\$(\w+), (0x[0-9a-f]+|-0x[0-9a-f]+)\(\$sp\)$', rest)
        if not m:
            return
        reg, off = "$" + m.group(1), int(m.group(2), 0)
        if not (0 <= off < size):
            stats["off-out"] += 1
            return
        if reg in pmap and f"param_{pmap[reg]}" in params:
            emits.append((pc, op, off, "P", f"param_{pmap[reg]}"))
        elif reg in consts:
            emits.append((pc, op, off, "C", consts[reg]))
        elif reg in spk:
            emits.append((pc, op, off, "VS", spk[reg]))
        elif reg == "$ra":
            emits.append((pc, op, off, "RA", 0))
        elif reg in SREGS:
            emits.append((pc, op, off, "S", SREGS[reg]))
            stats["s-spill"] += 1
        else:
            stats["opaque"] += 1
    def try_save_f(pc, rest):
        m = re.match(r'\$(\w+), (0x[0-9a-f]+|-0x[0-9a-f]+)\(\$sp\)$', rest)
        if m:
            reg, off = "$" + m.group(1), int(m.group(2), 0)
            if reg in FREGS and 0 <= off < size:
                emits.append((pc, "swc1", off, "F", FREGS[reg]))
                stats["f-spill"] += 1
                return
        stats["swc1"] += 1

    MASK64 = 0xFFFFFFFFFFFFFFFF

    def apply_const(op, ops):
        """One instruction of the symbolic const/pmap walk; True if consumed."""
        if op in ("move", "daddu") and len(ops) == 2 and ops[1] == "$zero":
            consts[ops[0]] = 0
        elif op == "move" and len(ops) == 2:
            s, d = ops[1], ops[0]
            consts.pop(d, None); pmap.pop(d, None); spk.pop(d, None)
            if s in consts: consts[d] = consts[s]
            if s in pmap: pmap[d] = pmap[s]
            if s in spk: spk[d] = spk[s]
        elif op in ("addiu", "daddiu") and len(ops) == 3 and ops[1] == "$zero":
            consts[ops[0]] = int(ops[2], 0) & MASK64
        elif op in ("addiu", "daddiu") and len(ops) == 3 and ops[1] == "$sp":
            spk[ops[0]] = int(ops[2], 0)  # frame-relative value (camera s7=sp+0x10)
            consts.pop(ops[0], None); pmap.pop(ops[0], None)
        elif op in ("addiu", "daddiu") and len(ops) == 3 and ops[1] in consts:
            consts[ops[0]] = (consts[ops[1]] + int(ops[2], 0)) & MASK64
            pmap.pop(ops[0], None); spk.pop(ops[0], None)
        elif op == "lui" and len(ops) == 2:
            consts[ops[0]] = (int(ops[1], 0) << 16) & 0xFFFFFFFF
            pmap.pop(ops[0], None); spk.pop(ops[0], None)
        elif op == "ori" and len(ops) == 3 and ops[1] in consts:
            consts[ops[0]] = consts[ops[1]] | int(ops[2], 0)
            pmap.pop(ops[0], None); spk.pop(ops[0], None)
        elif ops and ops[0].startswith('$'):  # unknown def
            consts.pop(ops[0], None); pmap.pop(ops[0], None); spk.pop(ops[0], None)
        else:
            return False
        return True
    i = idx[addr]
    end = min(len(prog), i + 64)
    while i < end:
        pc, op, rest = prog[i]
        if pc - addr >= 0x100:
            break
        ops = split_regs(rest)
        if op in SAVE_OPS:
            try_save(pc, op, rest)
            i += 1
            continue
        if op == "swc1":
            try_save_f(pc, rest)
            i += 1
            continue
        if op == "sqc2":
            stats["swc1"] += 1
            i += 1
            continue
        if op in STOP_OPS:
            # Seeds freeze at the first call: only defs in the straight-line
            # window before it are provably live for the callee's spills. A
            # folded value is visible there, so the delay slot itself is not
            # folded (gameMainLoop's loop folds would seed stale values).
            snap = (dict(consts), dict(pmap), dict(spk))
            i += 1
            if i < end:  # delay slot executes with pre-callee state, BEFORE the
                pc2, op2, rest2 = prog[i]  # callee's first spill
                if op2 in SAVE_OPS:
                    try_save(pc2, op2, rest2)
                elif op2 == "swc1":
                    try_save_f(pc2, rest2)
                elif op2 == "sqc2":
                    stats["swc1"] += 1
            break
        if op.startswith("l") and op != "lui":  # any load: value unknowable
            snap = (dict(consts), dict(pmap), dict(spk))
            break
        if apply_const(op, ops):
            i += 1
            continue
        i += 1
    if snap is None:  # window exhausted: everything folded is live straight-line
        snap = (consts, pmap, spk)
    c_s, p_s, k_s = snap
    # Seeds: callee-saved regs the walk proved constant/param/frame-relative in
    # the pre-call window. The ambient shadow file propagates these to every
    # callee's spill (and jalr-dispatched task fns) until this frame's epilogue
    # restores what its prologue spilled.
    seeds = []
    for reg, k in SREGS.items():
        if reg in c_s:
            seeds.append((k, "C", c_s[reg]))
        elif reg in p_s and f"param_{p_s[reg]}" in params:
            seeds.append((k, "P", f"param_{p_s[reg]}"))
        elif reg in k_s:
            seeds.append((k, "SP", k_s[reg]))
    return emits, seeds

def emit_block(emits, seeds, has_guard):
    lines = [BLOCK_START]
    for pc, op, off, kind, val in emits:
        if kind in ("RA", "S", "F") and not has_guard:
            continue
        if kind == "S":
            w = 8 if op == "sd" else 4
            lines.append(f"    rtguest::rtSpillS(rt_fg, {val}, rt_sv, {off:#x}, {w});")
        elif kind == "F":
            lines.append(f"    rtguest::rtSpillF(rt_fg, {val}, rt_sv, {off:#x});")
        elif kind == "VS":
            if op == "sd":
                lines.append(f"    *(uint64_t*)rtguest::G(rt_sv+{off:#x}) = rt_sv + {val:#x}ull;")
            else:
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = rt_sv + {val:#x}u;")
        elif op == "sw":
            if kind == "P":
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = (uint32_t)(int32_t){val};")
            elif kind == "RA":
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = rt_fg.rtRA;")
            else:
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = {val & 0xFFFFFFFF:#010x}u;")
        else:  # sd: sign-extended 64-bit
            if kind == "P":
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = (uint32_t)(int32_t){val};")
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off+4:#x}) = (uint32_t)((int64_t)(int32_t){val} >> 32);")
            elif kind == "RA":
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = rt_fg.rtRA;")
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off+4:#x}) = 0x00000000u;")
            else:
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = {val & 0xFFFFFFFF:#010x}u;")
                lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off+4:#x}) = {(val >> 32) & 0xFFFFFFFF:#010x}u;")
    for k, kind, val in seeds:
        if kind == "C":
            lines.append(f"    rtguest::gS[{k}] = {val & 0xFFFFFFFFFFFFFFFF:#018x}ull;  // walk const")
        elif kind == "SP":
            lines.append(f"    rtguest::gS[{k}] = rt_sv + {val:#x}ull;  // walk: sp+{val:#x}")
        else:
            lines.append(f"    rtguest::setS32({k}, (uint32_t){val});  // walk: = {val}")
    lines.append("  }")
    return "\n".join(lines) + "\n"

def wrap_group(src, name, pcs, stats):
    """Wrap direct calls Name(args) -> callRA(0xJALpc, guest::Name, args).

    Pair the caller's jal sites to source call expressions in program order;
    only rewrite when the counts match exactly (otherwise leave the stale
    pending value — mis-pairing a PC is no better than nothing)."""
    pat = re.compile(r'(?<![\w:>.])(' + name + r')\s*\(')
    hits = []
    type_start = re.compile(r'(void|u?int(8|16|32|64)_t|uint|int|float|undefined8?|undefined)$')
    for m in pat.finditer(src):
        line_start = src.rfind('\n', 0, m.start()) + 1
        if '//' in src[line_start:m.start()]:  # inside a line comment
            continue
        # declarations/definitions: only a type keyword precedes the name on the statement
        seg_start = max(src.rfind(';', 0, m.start()) + 1, src.rfind('{', 0, m.start()) + 1, line_start)
        if type_start.search(src[seg_start:m.start()].strip()):
            continue
        # not a function definition: closing paren of the declarator followed by {
        di = src.index('(', m.end() - 1)
        depth, dj = 0, di
        while dj < len(src):
            if src[dj] == '(': depth += 1
            elif src[dj] == ')':
                depth -= 1
                if depth == 0: break
            dj += 1
        tail = src[dj + 1:src.find('\n', dj + 1)]
        if '{' in tail or src[dj + 1:].lstrip().startswith('{'):
            continue
        hits.append(m)
    if len(hits) != len(pcs):
        stats["wrap-multi" if hits else "wrap-none"] += 1
        return src
    for m, pc in sorted(zip(hits, pcs), key=lambda t: -t[0].start()):
        i = src.index('(', m.end() - 1)
        depth, j = 0, i
        while j < len(src):
            if src[j] == '(': depth += 1
            elif src[j] == ')':
                depth -= 1
                if depth == 0: break
            j += 1
        args = src[i + 1:j]
        repl = f"RT_CALLRA(0x{pc:x}, guest::{name}({args}))"
        src = src[:m.start()] + repl + src[j + 1:]
        stats["wrap"] += 1
    return src

def main():
    dry = "--dry" in sys.argv
    prog = disasm()
    idx = {pc: i for i, (pc, _, _) in enumerate(prog)}
    # addr -> name from every TU's registration line
    names = {}
    for f in glob.glob(os.path.join(GEN, "*.cpp")):
        m = REG_LINE.search(open(f).read())
        if m:
            names[int(m.group(1), 16)] = m.group(2)
    stats = collections.Counter()
    starts = sorted(names.keys())
    nxt = {a: (starts[i + 1] if i + 1 < len(starts) else 0x29A848)
           for i, a in enumerate(starts)}
    for f in sorted(glob.glob(os.path.join(GEN, "*.cpp"))):
        addr = int(os.path.basename(f)[:8], 16)
        if addr not in idx:
            continue
        src = open(f).read()
        src, _ = strip_block(src)
        parsed = parse_tu(f)
        if parsed is None:
            stats["no-sig"] += 1
            continue
        size, params = parsed
        if size:
            emits, seeds = prologue_emits(prog, idx, addr, size, params, stats)
            if emits or seeds:
                anchor_re = re.compile(r'^[ ]*(rtguest::FrameGuard[^\n]*|[a-zA-Z0-9_ ]*rtguest::allocFrame\([^\n]*)\n', re.M)
                last, ran = None, 0
                for m in anchor_re.finditer(src):
                    if ran and m.start() > (last.end() if last else 0):
                        break
                    last, ran = m, ran + 1
                if last is None:
                    stats["no-anchor"] += 1
                else:
                    has_guard = "FrameGuard rt_fg(" in src
                    src = src[:last.end()] + emit_block(emits, seeds, has_guard) + src[last.end():]
                    stats["touched"] += 1
                    stats["stores"] += sum(1 for e in emits if e[3] != "RA")
                    stats["ra-stores"] += sum(1 for e in emits if e[3] == "RA")
                    stats["seeds"] += len(seeds)
        # caller rewrite: jal sites -> source calls (ordered, exact count)
        jals = collections.defaultdict(list)
        for pc, op, rest in prog[idx[addr]:]:
            if pc >= nxt.get(addr, addr + 4):
                break
            if op == "jal":
                m = re.match(r'0x([0-9a-f]+)', rest)
                if m and int(m.group(1), 16) in names:
                    jals[names[int(m.group(1), 16)]].append(pc)
        for nm, pcs in jals.items():
            src = wrap_group(src, nm, pcs, stats)
        if not dry:
            open(f, "w").write(src)
    print(dict(stats))

if __name__ == "__main__":
    main()
