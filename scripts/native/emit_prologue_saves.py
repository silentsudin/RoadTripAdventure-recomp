#!/usr/bin/env python3
"""Model machine prologue register-save stores in gen TUs.

The oracle's function prologues store $16-$23/$ra/$fp and float regs into
frame slots (sd $16,0x60($sp) ...); other frames later read those slots as
residue. Ghidra drops the stores, so native leaves stale bytes. This pass
re-emits every prologue store whose value is derivable from the machine
prologue alone: incoming int args ($4..$9 -> param_k), move/lui/ori/li
constants, propagated through `move` chains. $ra/$fp and float (swc1)
saves stay a documented residue class (values are the caller's old
registers; no shadow register file yet).

Writes run at frame entry — the same point the machine stores them, so a
slot both saved and later written by pseudocode keeps the pseudocode value
(last write wins, identical order).

Idempotent: files already carrying `// PROLOGUE-SAVES` are skipped.

Usage: python3 scripts/native/emit_prologue_saves.py [--dry]
"""
import re, subprocess, sys, glob, os, collections

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ELF = os.path.join(ROOT, "build/rom/SLUS_203.98")
OBJDUMP = "/opt/homebrew/opt/llvm/bin/llvm-objdump"
GEN = os.path.join(ROOT, "src/native/guest/gen")

INS = re.compile(r'^\s+([0-9a-f]+):\s+(?:[0-9a-f]{2} )+\s*(\S+)\s*(.*)$')

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
LOAD_OPS = ("lw", "lh", "lhu", "lb", "lbu", "lwl", "lwr", "ld", "ldl", "ldr",
            "lwc1", "lqc2", "lui")  # lui treated as mapping op below
SAVE_OPS = {"sd", "sw"}

def parse_tu(path):
    """Return (guard_size, [int param names]) for a gen TU, or None."""
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

def main():
    dry = "--dry" in sys.argv
    prog = disasm()
    idx = {pc: i for i, (pc, _, _) in enumerate(prog)}
    stats = collections.Counter()
    for f in sorted(glob.glob(os.path.join(GEN, "*.cpp"))):
        addr = int(os.path.basename(f)[:8], 16)
        if addr not in idx:
            continue
        src = open(f).read()
        if "// PROLOGUE-SAVES" in src:
            stats["skip-done"] += 1
            continue
        parsed = parse_tu(f)
        if parsed is None:
            stats["no-sig"] += 1
            continue
        size, params = parsed
        if size == 0:
            stats["frameless"] += 1
            continue
        # symbolic prologue walk
        consts = {}   # reg -> int64
        pmap = {}     # reg -> param index (1..6 from $4..$9)
        for r in range(4, 10):
            pmap[f"${r}"] = r - 3
        emits = []
        i = idx[addr]
        end = min(len(prog), i + 64)
        stop_after = 0
        while i < end:
            pc, op, rest = prog[i]
            if pc - addr >= 0x100:
                break
            ops = split_regs(rest)
            def dst_reg():
                return ops[0] if ops and ops[0].startswith('$') else None
            if op in SAVE_OPS:
                m = re.match(r'\$(\w+), (0x[0-9a-f]+|-0x[0-9a-f]+)\(\$sp\)$', rest)
                if m:
                    reg, off = "$" + m.group(1), int(m.group(2), 0)
                    if not (0 <= off < size):
                        stats["off-out"] += 1
                    elif reg in pmap and f"param_{pmap[reg]}" in params:
                        pn = f"param_{pmap[reg]}" 
                        emits.append((pc, op, off, f"P:{pn}", pn))
                    elif reg in consts:
                        emits.append((pc, op, off, f"C:{consts[reg]:#x}", consts[reg]))
                    elif reg in ("$ra", "$fp"):
                        stats["ra/fp"] += 1
                    else:
                        stats["opaque"] += 1
                i += 1
                if stop_after:
                    break
                continue
            if op == "swc1" or op == "sqc2":
                stats["swc1"] += 1
                i += 1
                if stop_after:
                    break
                continue
            if op in ("move", "daddu") and len(ops) == 2 and ops[1] == "$zero":
                consts[ops[0]] = 0
                i += 1; continue
            if op == "move" and len(ops) == 2:
                s, d = ops[1], ops[0]
                consts.pop(d, None); pmap.pop(d, None)
                if s in consts: consts[d] = consts[s]
                if s in pmap: pmap[d] = pmap[s]
                i += 1; continue
            if op in ("addiu", "daddiu") and len(ops) == 3 and ops[1] == "$zero":
                consts[ops[0]] = int(ops[2], 0) & 0xFFFFFFFFFFFFFFFF
                i += 1; continue
            if op in ("addiu", "daddiu") and len(ops) == 3 and ops[1] in consts:
                consts[ops[0]] = (consts[ops[1]] + int(ops[2], 0)) & 0xFFFFFFFFFFFFFFFF
                pmap.pop(ops[0], None)
                i += 1; continue
            if op == "lui" and len(ops) == 2:
                consts[ops[0]] = (int(ops[1], 0) << 16) & 0xFFFFFFFF
                pmap.pop(ops[0], None)
                i += 1; continue
            if op == "ori" and len(ops) == 3 and ops[1] in consts:
                consts[ops[0]] = consts[ops[1]] | int(ops[2], 0)
                pmap.pop(ops[0], None)
                i += 1; continue
            if op in STOP_OPS:
                i += 1
                if i < end:  # delay slot executes with pre-callee state
                    pc2, op2, rest2 = prog[i]
                    if op2 in SAVE_OPS:
                        m = re.match(r'\$(\w+), (0x[0-9a-f]+|-0x[0-9a-f]+)\(\$sp\)$', rest2)
                        if m:
                            reg, off = "$" + m.group(1), int(m.group(2), 0)
                            if 0 <= off < size and reg in pmap and f"param_{pmap[reg]}" in params:
                                emits.append((pc2, op2, off, f"P:param_{pmap[reg]}", f"param_{pmap[reg]}"))
                            elif 0 <= off < size and reg in consts:
                                emits.append((pc2, op2, off, f"C:{consts[reg]:#x}", consts[reg]))
                            elif 0 <= off < size:
                                stats["opaque"] += 1
                    elif op2 == "swc1":
                        stats["swc1"] += 1
                break
            if op.startswith("l") and op not in ("lui",):  # any load: value unknowable
                break
            d = dst_reg()
            if d:  # unknown def
                consts.pop(d, None); pmap.pop(d, None)
            i += 1
        if not emits:
            stats["no-emit"] += 1
            continue
        # build insertion block; writes in machine order (program order = addr order)
        lines = ["  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES"]
        for pc, op, off, tag, val in emits:
            if op == "sw":
                if tag.startswith("P:"):
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = (uint32_t)(int32_t){val};")
                else:
                    v = val & 0xFFFFFFFF
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = {v:#010x}u;")
            else:  # sd: sign-extended 64-bit
                if tag.startswith("P:"):
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = (uint32_t)(int32_t){val};")
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off+4:#x}) = (uint32_t)((int64_t)(int32_t){val} >> 32);")
                else:
                    lo = val & 0xFFFFFFFF
                    hi = (val >> 32) & 0xFFFFFFFF
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off:#x}) = {lo:#010x}u;")
                    lines.append(f"    *(uint32_t*)rtguest::G(rt_sv+{off+4:#x}) = {hi:#010x}u;")
        lines.append("  }")
        block = "\n".join(lines) + "\n"
        # anchor: after the last FrameGuard/allocFrame line in the opening run
        anchor_re = re.compile(r'^[ ]*(rtguest::FrameGuard[^\n]*|[a-zA-Z0-9_ ]*rtguest::allocFrame\([^\n]*)\n', re.M)
        last = None
        ran = 0
        for m in anchor_re.finditer(src):
            if ran and m.start() > (last.end() if last else 0):
                break
            last = m; ran += 1
        if last is None:
            stats["no-anchor"] += 1
            continue
        out_src = src[:last.end()] + block + src[last.end():]
        stats["touched"] += 1
        stats["stores"] += len(emits)
        if not dry:
            open(f, "w").write(out_src)
    print(dict(stats))

if __name__ == "__main__":
    main()
