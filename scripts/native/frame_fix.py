#!/usr/bin/env python3
"""Re-place named frame slots at Ghidra's entry-sp offsets: slot sp-offset = frame - 0xXX
for *Stack_XX. Run as a post-pass after transliterate.py + emit_prologue_saves.py; idempotent.
Usage: frame_fix.py <gen dir> [--write]  (prints per-file plan)."""
import re, glob, sys

root = sys.argv[1]; write = '--write' in sys.argv
LINE = re.compile(r'^(\s*)(?:uint32_t\s+(\w+)\s*=\s*)?rtguest::allocFrame\((\w+)\);[^\n]*$', re.M)
changed = 0; skipped = []
for p in sorted(glob.glob(root + '/*.cpp')):
    t = open(p).read()
    fg = re.search(r'FrameGuard rt_fg\((\d+)\)', t)
    if not fg or 'allocFrame' not in t: continue
    N = int(fg.group(1))
    ms = list(LINE.finditer(t))
    if not ms: continue
    # all allocFrame lines must be one contiguous block (only whitespace between)
    seg = t[ms[0].start():ms[-1].end()]
    if len(re.sub(r'\s', '', LINE.sub('', seg))) != 0: skipped.append((p, 'noncontig')); continue
    named = []
    for m in ms:
        if m.group(2):
            mm = re.search(r'Stack_([0-9a-f]+)$', m.group(2))
            if not mm: skipped.append((p, 'nonstack ' + m.group(2))); named = None; break
            named.append((m.group(1), m.group(2), int(m.group(3).rstrip('uU'), 0), N - int(mm.group(1), 16)))
    if not named: continue
    # current placement
    cur = 0; curpos = {}
    for m in ms:
        if m.group(2): curpos[m.group(2)] = cur
        cur += int(m.group(3).rstrip('uU'), 0)
    if all(curpos[n] == e for _, n, _, e in named): continue
    order = sorted(named, key=lambda x: x[3])
    if order[0][3] < 0: skipped.append((p, 'negative ' + order[0][1])); continue
    out = []; c = 0; ok = True
    for ind, n, sz, e in order:
        if e < c: ok = False; break
        if e > c: out.append(f'{ind}rtguest::allocFrame({e - c});  // guest frame gap (sp+0x{c:x}..0x{e - 1:x})')
        out.append(f'{ind}uint32_t {n} = rtguest::allocFrame({sz});  // sp+0x{e:x} (Ghidra entry-sp -0x{N - e:x}, frame 0x{N:x})')
        c = e + sz
    if not ok or c > N: skipped.append((p, 'overlap/overflow')); continue
    changed += 1
    print(p.split('/')[-1], 'frame', hex(N), ' '.join(f'{n}:{curpos[n]:#x}->{e:#x}' for _, n, _, e in order if curpos[n] != e))
    if write:
        nt = t[:ms[0].start()] + '\n'.join(out) + t[ms[-1].end():]
        open(p, 'w').write(nt)
print('changed', changed); [print('SKIP', *s) for s in skipped]
