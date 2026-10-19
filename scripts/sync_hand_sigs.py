#!/usr/bin/env python3
"""W12: rewrite guest_libs_sif.cpp host def signatures to the authoritative
parameter TYPES (scripts/gen_guest_libs.py rule: .inc line wins over the gen
def), keeping the hand-written parameter NAMES and bodies intact. Reports
any param-count mismatch for manual attention."""
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from gen_guest_libs import CFG, HAND, def_name, signature  # noqa: E402

SRC = pathlib.Path(__file__).resolve().parents[1] / "src/native/guest"
HAND_FILES = ["guest_libs_sif.cpp", "guest_libs_pad.cpp"]

sigs = {}
for line in CFG.read_text().splitlines():
    line = line.strip()
    if not line or line.startswith("#"):
        continue
    addr, name = line.split()
    if name in HAND:
        dname = def_name(addr)
        sigs[dname] = signature(addr, dname)


def split_params(paramstr: str) -> list[str]:
    return [p.strip() for p in paramstr.split(",") if p.strip()]


report = []
state = {"fixed": 0}

DEF_RE = re.compile(
    r"^(uint32_t|uint64_t|int32_t|void) (\w+)\(((?:[^\n]|\n[ \t]+)*?)\) \{", re.M)


def fix_def(m: re.Match) -> str:
    ret, name, params = m.group(1), m.group(2), m.group(3)
    if name not in sigs:
        return m.group(0)
    sig = sigs[name]
    m2 = re.match(r"^(\S+) \w+\((.*)\)$", sig)
    want = split_params(m2.group(2))
    have = split_params(re.sub(r"\s+", " ", params))
    want_named = [p for p in want if p != "..."]
    have_named = [p for p in have if p != "..."]
    if len(have_named) != len(want_named):
        report.append(f"COUNT {name}: have={len(have_named)} want={len(want_named)} "
                      f"want={want_named}")
        return m.group(0)
    new_params = []
    changed = ret != m2.group(1)
    for w, h in zip(want_named, have_named):
        wtype = w.rsplit(" ", 1)[0]
        if wtype != h.rsplit(" ", 1)[0]:
            changed = True
        new_params.append(f"{wtype} {h.rsplit(' ', 1)[1]}")
    if "..." in want:
        new_params.append("...")
    if not changed:
        return m.group(0)
    state["fixed"] += 1
    report.append(f"retyped {name}")
    return f"{m2.group(1)} {name}({', '.join(new_params)}) {{"


for fname in HAND_FILES:
    path = SRC / fname
    text = DEF_RE.sub(fix_def, path.read_text())
    path.write_text(text)
print(f"synced {state['fixed']} signatures; {len(sigs)} hand defs tracked")
for r in report:
    print(" ", r)
