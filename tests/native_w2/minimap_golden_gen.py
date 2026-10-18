#!/usr/bin/env python3
"""W2-11 golden-vector generator: hand-traces the minimap view-block handlers from the ELF.

  minimap_golden_gen.py [--elf PATH] [--out tests/native_w2/data/minimap_golden.tsv]

Independent of src/native/gameplay/minimap.cpp.  Reads only numbers (floats of the zoom table,
placement tables, vu0 constants) from the user's SLUS_203.98, asserts the MIPS operation order of
the handlers it traces (capstone), then evaluates the float32 operation sequence literally:

  town NTSC  0x242438 @0x2425B8..0x242614  tx = (x0 + 1728) - (px*s)/200 ; ty = (y0 + 1936) + (pz*s*K)/200
  town PAL   0x242438 @0x242518..0x242574  tx = (x0*0.8 + 1792) - ((px*s)*0.8)/200 ; ty = (y0 + 1920) + ...
  race/split 0x2427D0 / 0x242A88           mapViewMatrix(entry.x + 1728 | entry.x*0.8 + 1792, entry.y, entry.s)
  mapViewMatrix 0x21F920                   [8]=tx [9]=ty [0]=s*(1|0.8) [5]=s*DAT_00332CAC (-0.47)
  aux offset 0x2A43E0                      (-800,-200,-800) rotated by RotY(rot) (race) / RotX(-pi/2) (all)

then maps view -> field (view - XYOFFSET).  The output file holds derived numbers only.
"""
import argparse
import math
import struct
import sys
from pathlib import Path

import numpy as np

F = np.float32
BASE = 0x1FF000


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--elf", default=str(Path.home() / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"))
    ap.add_argument("--out", default=str(Path(__file__).parent / "data/minimap_golden.tsv"))
    a = ap.parse_args()
    d = Path(a.elf).read_bytes()

    def f32(addr):
        return F(struct.unpack("<f", d[addr - BASE : addr - BASE + 4])[0])

    def w32(addr):
        return struct.unpack("<I", d[addr - BASE : addr - BASE + 4])[0]

    # --- structural asserts on the traced handler bytes (lui immediates = float literals) ---
    lui = {0x242518: 0x4348, 0x242530: 0x44F0, 0x24253C: 0x44E0, 0x2425BC: 0x4348, 0x2425D4: 0x44D8,
           0x2425E4: 0x44F2, 0x24261C: 0x4440}
    for addr, imm in lui.items():
        word = w32(addr)
        assert word >> 16 == 0x3C01 and (word & 0xFFFF) == imm, hex(addr)
    ops = {  # opcode words (COP1 fmt S): mul.s 2, div.s 3, add.s 0, sub.s 1  (function field)
        0x242508: 2, 0x242510: 2, 0x242524: 2, 0x24252C: 2, 0x242538: 2, 0x242548: 0, 0x242550: 0, 0x24255C: 3,
        0x242568: 3, 0x24256C: 1, 0x242574: 0,
        0x2425B8: 2, 0x2425C4: 2, 0x2425E0: 2, 0x2425F4: 3, 0x2425F8: 0, 0x2425FC: 0, 0x242608: 3, 0x24260C: 1,
        0x242614: 0}
    for addr, fn in ops.items():
        word = w32(addr)
        assert word >> 26 == 0x11 and (word & 0x3F) == fn, hex(addr)

    zoom = [(f32(0x2A42C0 + 16 * i), f32(0x2A42C4 + 16 * i), f32(0x2A42C8 + 16 * i)) for i in range(2)]
    race = [tuple(f32(0x2A4050 + 16 * i + 4 * k) for k in range(4)) for i in range(39)]
    split = [tuple(f32(0x2A42E0 + 16 * i + 4 * k) for k in range(4)) for i in range(5)]
    kdiv = F(200.0)
    k_sq_neg = f32(0x332CAC)  # -0.47
    k_sq = f32(0x332EFC)  # +0.47 (town y translation)
    k_pal = f32(0x332CA8)  # 0.8
    off = (f32(0x2A43E0), f32(0x2A43E4), f32(0x2A43E8))
    assert off[0] == F(-800) and off[2] == F(-800) and k_sq == -k_sq_neg and f32(0x332EF4) == k_pal

    def view_block(tx, ty, s, pal):
        kx = k_pal if pal else F(1.0)
        return tx, ty, F(s * kx), F(s * k_sq_neg)

    def town(mode, px, pz, wx, wz, pal):
        x0, y0, s = zoom[mode]
        px, pz, wx, wz = F(px), F(pz), F(wx), F(wz)
        if not pal:
            tx = F(F(x0 + F(1728)) - F(F(px * s) / kdiv))
            ty = F(F(y0 + F(1936)) + F(F(F(pz * s) * k_sq) / kdiv))
            xo, yo = F(1728), F(1936)
        else:
            tx = F(F(F(x0 * k_pal) + F(1792)) - F(F(F(px * s) * k_pal) / kdiv))
            ty = F(F(y0 + F(1920)) + F(F(F(pz * s) * k_sq) / kdiv))
            xo, yo = F(1792), F(1920)
        tx, ty, sx, sy = view_block(tx, ty, s, pal)
        fx = F(F(tx + F(sx * F(F(wx + off[0]) / kdiv))) - xo)
        fy = F(F(ty + F(sy * F(F(wz + off[2]) / kdiv))) - yo)
        return tx, ty, fx, fy

    def entry(e, wx, wz, pal):
        ex, ey, s, rot = e
        wx, wz = F(wx), F(wz)
        if not pal:
            tx = F(ex + F(1728))
            xo, yo = F(1728), F(1936)
        else:
            tx = F(F(ex * k_pal) + F(1792))
            xo, yo = F(1792), F(1920)
        tx, ty, sx, sy = view_block(tx, ey, s, pal)
        ax, az = F(wx + off[0]), F(wz + off[2])
        c, sn = F(math.cos(float(rot))), F(math.sin(float(rot)))
        x1 = F(F(ax * c) + F(az * sn))
        z1 = F(F(-ax * sn) + F(az * c))
        fx = F(F(tx + F(sx * F(x1 / kdiv))) - xo)
        fy = F(F(ty + F(sy * F(z1 / kdiv))) - yo)
        return tx, ty, fx, fy

    # deterministic inputs (LCG), multiples of 0.25 within a 0..1600 tile
    state = 0x2545F491

    def rnd(n):
        nonlocal state
        state = (state * 1103515245 + 12345) & 0x7FFFFFFF
        return (state % (n * 4)) / 4.0

    rows = []
    for pal in (0, 1):
        for mode in (0, 1):
            for _ in range(8 if not pal else 3):
                px, pz, wx, wz = rnd(1600), rnd(1600), rnd(1600), rnd(1600)
                if _ % 3 == 0:  # include in-window points near the subject
                    wx, wz = px + rnd(300) - 150, pz + rnd(300) - 150
                tx, ty, fx, fy = town(mode, px, pz, wx, wz, pal)
                rows.append(("town", pal, mode, px, pz, wx, wz, tx, ty, fx, fy))
    for pal in (0, 1):
        for idx in (0, 2, 7, 15, 20, 23, 26, 38):  # scales 50, 50, 50, 40, 70, 33.5, 133.5, 50
            for _ in range(3 if not pal else 1):
                wx, wz = rnd(1600), rnd(1600)
                tx, ty, fx, fy = entry(race[idx], wx, wz, pal)
                rows.append(("race", pal, idx, 0.0, 0.0, wx, wz, tx, ty, fx, fy))
        for idx in range(5):
            wx, wz = rnd(1600), rnd(1600)
            tx, ty, fx, fy = entry(split[idx], wx, wz, pal)
            rows.append(("split", pal, idx, 0.0, 0.0, wx, wz, tx, ty, fx, fy))

    out = Path(a.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("w") as fh:
        fh.write("# W2-11 minimap golden vectors (derived numbers, minimap_golden_gen.py). "
                 "kind pal idx px pz wx wz view_tx view_ty field_x field_y\n")
        for r in rows:
            fh.write("%s %d %d %.2f %.2f %.2f %.2f %.4f %.4f %.4f %.4f\n" % tuple(
                (r[0], r[1], r[2], r[3], r[4], r[5], r[6], float(r[7]), float(r[8]), float(r[9]), float(r[10]))))
    print("wrote %d vectors -> %s" % (len(rows), out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
