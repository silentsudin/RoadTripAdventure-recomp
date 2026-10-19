#!/usr/bin/env python3
"""Rewrite guest_decls.inc prototypes from the actual gen TU definitions.

Workers widen params/returns to uint64_t per pseudocode; the generated loose
decls (uint32_t q0, ...) then form a second overload, which makes
&name ambiguous in RT_GUEST_FN and truncates direct calls. Run after every
port wave; keeps decls exactly matching compiled definitions.
"""
import re
import sys
from pathlib import Path

GEN = Path(sys.argv[1] if len(sys.argv) > 1 else "src/native/guest/gen")
INC = Path(sys.argv[2] if len(sys.argv) > 2 else "build/decomp/guest_decls.inc")

sig_re = re.compile(
    r"^(uint\d+_t|int\d+_t|void) ([A-Za-z_]\w*)\((.*?)\) \{$")

defs = {}
for f in sorted(GEN.glob("*.cpp")):
    for line in f.read_text().splitlines():
        m = sig_re.match(line)
        if m:
            ret, name, params = m.groups()
            plist = params.split(",")
            vari = any(p.strip() == "..." for p in plist)
            types = [p.strip().rsplit(" ", 1)[0] for p in plist
                     if p.strip() and p.strip() != "..."]
            defs.setdefault(name, (ret, types, vari))
            break

lines = INC.read_text().splitlines(keepends=True)
decl_re = re.compile(r"^(\s*)(?:inline\s+)?(uint\d+_t|int\d+_t|void) ([A-Za-z_]\w*)\(.*\;(?:\s*)$")
out, changed = [], 0
for line in lines:
    m = decl_re.match(line.rstrip("\n"))
    if m and m.group(3) in defs:
        ret, types, vari = defs[m.group(3)]
        ps = ", ".join(f"{t} q{i} = 0" for i, t in enumerate(types))
        if vari:
            ps += ", ..."
        new = f"{m.group(1)}{ret} {m.group(3)}({ps});\n"
        if new != line:
            changed += 1
        out.append(new)
    else:
        out.append(line)
INC.write_text("".join(out))
print(f"defs={len(defs)} rewrote={changed}")

# Strip worker #define NAME NAME_decl / #undef NAME guards once decls match defs
# (the guard existed only to dodge the stale-decl overload we just fixed).
stripped = 0
for f in sorted(GEN.glob("*.cpp")):
    txt = f.read_text()
    if "_decl" not in txt:
        continue
    new = re.sub(r"#define ([A-Za-z_]\w*) \1_decl\n#include \"guest_decls.inc\"\n#undef \1\n",
                  lambda m: '#include "guest_decls.inc"\n' if m.group(1) in defs else m.group(0),
                  txt)
    if new != txt:
        if "--apply" in sys.argv:
            f.write_text(new)
        stripped += 1
print(f"guards={'stripped' if '--apply' in sys.argv else 'found'}: {stripped}")
