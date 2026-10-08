#!/usr/bin/env python3
"""Which GS features Road Trip uses: the spec for the hardware GS renderer.

Runs the regression suite (or a -k subset) with RT_GS_STATE_SURVEY on and every frame drawn, then
merges the per-process survey files into a readable report:

    python3 scripts/hwgs_survey.py [--rom IMAGE] [-k EXPR] [--out build/scratch/hwgs]
    python3 scripts/hwgs_survey.py --report-only --out build/scratch/hwgs

The survey files (one per game process) hold one line per distinct draw state or transfer:
kind, count, first vblank, last vblank, key (space-separated name=value fields).
"""

from __future__ import annotations

import argparse
import collections
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

PSM = {0x00: "CT32", 0x01: "CT24", 0x02: "CT16", 0x0A: "CT16S", 0x13: "T8", 0x14: "T4", 0x1B: "T8H",
       0x24: "T4HL", 0x2C: "T4HH", 0x30: "Z32", 0x31: "Z24", 0x32: "Z16", 0x3A: "Z16S"}
PRIM = ["point", "line", "linestrip", "tri", "tristrip", "trifan", "sprite", "prim7"]
ATST = ["NEVER", "ALWAYS", "LESS", "LEQUAL", "EQUAL", "GEQUAL", "GREATER", "NOTEQUAL"]
AFAIL = ["KEEP", "FB_ONLY", "ZB_ONLY", "RGB_ONLY"]
ZTST = ["NEVER", "ALWAYS", "GEQUAL", "GREATER"]
TFX = ["MODULATE", "DECAL", "HIGHLIGHT", "HIGHLIGHT2"]
WM = ["REPEAT", "CLAMP", "REGION_CLAMP", "REGION_REPEAT"]
FILTER = ["NEAREST", "LINEAR", "NEAREST_MIP_NEAREST", "NEAREST_MIP_LINEAR", "LINEAR_MIP_NEAREST",
          "LINEAR_MIP_LINEAR", "?6", "?7"]
ABC = ["Cs", "Cd", "0", "?"]
C_ = ["As", "Ad", "FIX", "?"]


def psm(v: str) -> str:
    n = int(v, 16)
    return PSM.get(n, f"psm{n:02x}")


def parse_key(key: str) -> dict:
    return dict(f.split("=", 1) for f in key.split(" ") if "=" in f)


def decode_test(v: str) -> str:
    t = int(v, 16)
    parts = []
    if t & 1:
        parts.append(f"ATE {ATST[(t >> 1) & 7]} aref={(t >> 4) & 0xFF} afail={AFAIL[(t >> 12) & 3]}")
    if (t >> 14) & 1:
        parts.append(f"DATE datm={(t >> 15) & 1}")
    parts.append(f"Z {ZTST[(t >> 17) & 3]}" if (t >> 16) & 1 else "Z off")
    return ", ".join(parts)


def decode_alpha(v: str) -> str:
    a = int(v, 16)
    eq = f"({ABC[a & 3]} - {ABC[(a >> 2) & 3]}) * {C_[(a >> 4) & 3]} + {ABC[(a >> 6) & 3]}"
    if (a >> 4) & 3 == 2:
        eq += f"  FIX={(a >> 32) & 0xFF}"
    return eq


def decode_tex1(v: str) -> str:
    t = int(v, 16)
    lcm, mxl, mmag, mmin = t & 1, (t >> 2) & 7, (t >> 5) & 1, (t >> 6) & 7
    mtba, l, k = (t >> 9) & 1, (t >> 19) & 3, (t >> 32) & 0xFFF
    k = k - 0x1000 if k & 0x800 else k
    return (f"MXL={mxl} MMAG={'LINEAR' if mmag else 'NEAREST'} MMIN={FILTER[mmin]} "
            f"LCM={'fixed' if lcm else 'Q'} L={l} K={k / 16:g} MTBA={mtba}")


def load(out: Path):
    draws: dict[str, list] = {}
    xfers: dict[str, list] = {}
    files = sorted(out.glob("survey-*.tsv"))
    for f in files:
        for line in f.read_text(errors="replace").splitlines():
            parts = line.split("\t")
            if len(parts) != 5:
                continue
            kind, count, _first, _last, key = parts
            m = draws if kind == "draw" else xfers
            e = m.setdefault(key, [0, 0])
            e[0] += int(count)
            e[1] += 1
    return draws, xfers, len(files)


def table(title: str, counter: collections.Counter) -> str:
    total = sum(counter.values()) or 1
    lines = [f"\n## {title}\n", "| count | % | value |", "|---:|---:|---|"]
    for value, n in counter.most_common():
        lines.append(f"| {n} | {100 * n / total:.2f} | {value} |")
    return "\n".join(lines)


def report(out: Path) -> str:
    draws, xfers, nfiles = load(out)
    c = collections.defaultdict(collections.Counter)
    for key, (n, _) in draws.items():
        k = parse_key(key)
        tex = k.get("tex")
        prim = PRIM[int(k["prim"])]
        c["Primitives (path / prim / fst)"][f"path{int(k['path']) + 1} {prim} fst={k['fst']} ctx={int(k['ctx']) + 1}"] += n
        fr = dict((x[:3], x[3:]) for x in k["frame"].split(","))  # psm.., msk.., fbp.., fbw..
        c["Frame buffers (PSM, FBMSK, FBP, FBW)"][
            f"{psm(fr['psm'])} fbmsk={fr['msk']} fbp={fr['fbp']} fbw={fr['fbw']}"] += n
        zb = k["zbuf"].split(",")
        c["Z buffers"][f"{psm(zb[0][3:])} zmsk={zb[1][4:]} zbp={zb[2][3:]}"] += n
        c["Tests (alpha, destination alpha, Z)"][decode_test(k["test"])] += n
        if k["abe"] == "1":
            c["Blend equations (ABE=1)"][decode_alpha(k["alpha"]) + f"  pabe={k['pabe']} fba={k['fba']}"] += n
        c["Other draw state"][f"abe={k['abe']} fge={k['fge']} aa1={k['aa1']} iip={k['iip']} dthe={k['dthe']} "
                              f"colclamp={k['colclamp']} fba={k['fba']} pabe={k['pabe']}"] += n
        if tex:
            t = tex.split(",")
            fmt, cpsm, csm, tfx, tcc, size, fbarea, self_ = t
            c["Texture formats"][f"{psm(fmt[3:])} clut={psm(cpsm[4:])} {csm.upper()}"] += n
            c["Texture functions"][f"{TFX[int(tfx[3:])]} {tcc.upper()}"] += n
            c["Texture sizes"][size] += n
            c["Texture filtering (TEX1)"][decode_tex1(k["tex1"]) + f" miptbp={k['mip']}"] += n
            cl = k["clamp"].split(",")
            c["Wrap modes"][f"S={WM[int(cl[0][3:])]} T={WM[int(cl[1][3:])]}"] += n
            c["TEXA (ta0, aem, ta1)"][k["texa"]] += n
            if fbarea.endswith("1"):
                c["Textures read from the frame buffer / Z area"][
                    f"{psm(fmt[3:])} self={self_[4:]} {path_prim(k)} {decode_alpha(k['alpha']) if k['abe'] == '1' else 'no blend'}"
                    f" fbmsk={fr['msk']}"] += n
    xc = collections.Counter()
    for key, (n, _) in xfers.items():
        k = parse_key(key)
        d = {"0": "host->local", "1": "local->host", "2": "local->local", "3": "off"}.get(k["dir"], k["dir"])
        extra = " ".join(f"{a}={k[a]}" for a in ("sbp", "dbp") if a in k)
        xc[f"{d} {psm(k['spsm'])}->{psm(k['dpsm'])} {k['size']} dst_fbarea={k['dst_fbarea']} {extra}".strip()] += n
    body = [f"# GS state survey\n\n{nfiles} game processes, {sum(v[0] for v in draws.values())} vertex kicks, "
            f"{len(draws)} distinct draw states, {sum(v[0] for v in xfers.values())} transfers.\n"]
    for title, counter in c.items():
        body.append(table(title, counter))
    body.append(table("Transfers (direction, formats, size)", xc))
    return "\n".join(body) + "\n"


def path_prim(k: dict) -> str:
    return f"path{int(k['path']) + 1} {PRIM[int(k['prim'])]}"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", type=Path, default=ROOT / "build/scratch/hwgs")
    ap.add_argument("--rom", default=os.environ.get("RT_ROM"))
    ap.add_argument("-k", dest="select")
    ap.add_argument("-n", default="4")
    ap.add_argument("--report-only", action="store_true")
    a = ap.parse_args()
    a.out.mkdir(parents=True, exist_ok=True)
    if not a.report_only:
        for f in a.out.glob("survey-*.tsv"):
            f.unlink()
        env = dict(os.environ, RT_GS_STATE_SURVEY=str(a.out / "survey-%p.tsv"), RT_TEST_RENDER_ALWAYS="1")
        if a.rom:
            env["RT_ROM"] = a.rom
        cmd = [sys.executable, str(ROOT / "scripts/regress.py"), "-n", a.n]
        if a.select:
            cmd += ["-k", a.select]
        subprocess.run(cmd, cwd=ROOT, env=env)  # failures don't matter here: goldens are skipped
    text = report(a.out)
    (a.out / "states.md").write_text(text)
    print(text[:3000])
    print(f"... full report: {a.out / 'states.md'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
