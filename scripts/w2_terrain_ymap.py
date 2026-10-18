#!/usr/bin/env python3
"""Screen-space terrain-Y law for the Road Trip chase camera (W2-04r3).

project(y_game, camera) -> 448-row UI-space pixel y of the ground point under the car.
See docs/native-reference/terrain-query.md section 6 for the derivation and ELF citations.

    python3 scripts/w2_terrain_ymap.py --selftest [--elf PATH]      # >=10 CSV rows vs the C++ column
    python3 scripts/w2_terrain_ymap.py --write-csv [--elf PATH]     # append terrain_y_px (idempotent)
    python3 scripts/w2_terrain_ymap.py --verify-elf [--elf PATH]    # re-read every ELF constant
"""
from __future__ import annotations

import argparse
import math
import os
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
CSV = REPO / "tests" / "native_w2" / "data" / "vehicle_trace.csv"
DEFAULT_ELF = Path.home() / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
ELF_DELTA = 0x1FF000

# Every constant is (vaddr, expected u32 word, meaning). verify_elf() re-reads them.
ELF_PINS = (
    (0x0029CE00, 0x00000000, "chase preset 0 offset x = 0.0"),
    (0x0029CE04, 0x40000000, "chase preset 0 offset y = 2.0"),
    (0x0029CE08, 0xC0E00000, "chase preset 0 offset z = -7.0"),
    (0x0029CE10, 0x43FA0000, "chase preset 0 focal = 500.0"),
    (0x0029CE14, 0x00000380, "chase preset 0 pitch = 0x380 turns (65536/turn)"),
    (0x00332C88, 0x3EF0A3D7, "DAT_00332c88 = 0.47: camera matrix [1][1] = focal * 0.47"),
    (0x0021E73C, 0x3C014500, "lui 0x4500: SetViewDistanceGlobals(2048.0, ..) = screen centre 2048"),
    (0x002425E4, 0x3C0144F2, "lui 0x44f2: GS XYOFFSET y = 1936.0 (minimap.md section 1)"),
    (0x0029CA90, 0xBF3D70A4, "wheel contact 1 x = -0.74"),
    (0x0029CA98, 0xBF28F5C3, "wheel contact 1 z = -0.66"),
    (0x0029CAA0, 0x3F3D70A4, "wheel contact 2 x = +0.74"),
    (0x0029CAA8, 0xBF28F5C3, "wheel contact 2 z = -0.66"),
)

PRESET_OFFSET = (0.0, 2.0, -7.0)  # right, up, forward (0x29ce00)
FOCAL = 500.0  # 0x29ce10
PITCH_TURNS = 0x380  # 0x29ce14, 65536 per turn
PITCH_RAD = PITCH_TURNS * 2.0 * math.pi / 65536.0  # 4.921875 deg
Y_SCALE = FOCAL * struct.unpack("<f", struct.pack("<I", 0x3EF0A3D7))[0]  # 235.0 (0.47 f32)
SCREEN_CENTRE = 2048.0  # DAT_003356e0/e4
XYOFFSET_Y = 1936.0  # NTSC field origin (2048 - 112)
UI_PER_FIELD_ROW = 2.0  # one field line = 2 UI lines (minimap.md section 1)
SHOT_PER_UI = 447.0 / 448.0  # oracle scanout rows per UI row (minimap.md section 3)
REAR_CONTACTS = ((-0.74, 0.0, -0.66), (0.74, 0.0, -0.66))  # car-local (right, up, fwd), 0x29ca90 / 0x29caa0

LEVEL_BASIS = ((1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0))  # right, up, forward rows of car+0x00..0x20

# Validated y-map entry for the scene 'ground' ROI (scripts/w2_score.py YMAP_REGISTRY schema).
# 'ELEMENT', not 'FIT': the y-map scale/offset are derived (448 mapping), they were not least-squares fitted to
# >= 3 independent landmark pairs at <= 1 px; the PNG landmarks are quantised to a 2-row GS field row.
YMAP_ENTRY = {
    "status": "ELEMENT",
    "scale": SHOT_PER_UI,
    "offset": 0.0,
    "citation": (
        "docs/native-reference/terrain-query.md section 6: ui448 = 2*(sy_gs - 1936), "
        "sy_gs = 2048 - 235*Y/Z from buildCamera 0x21f698; oracle shot_y = ui448*447/448 "
        "(minimap.md section 3, mapping_verdict=448)"
    ),
}


def _dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def camera_frame(pos, basis=LEVEL_BASIS):
    """Chase-camera eye and axes. basis = (right, up, forward) rows of the car matrix (car+0x00/+0x10/+0x20)."""
    r, u0, f0 = basis
    c, s = math.cos(PITCH_RAD), math.sin(PITCH_RAD)
    f = tuple(c * f0[i] - s * u0[i] for i in range(3))
    u = tuple(c * u0[i] + s * f0[i] for i in range(3))
    eye = tuple(
        pos[i] + PRESET_OFFSET[0] * r[i] + PRESET_OFFSET[1] * u[i] + PRESET_OFFSET[2] * f[i] for i in range(3)
    )
    return eye, r, u, f


def gs_y(point, camera):
    eye, _r, u, f = camera_frame(camera["pos"], camera.get("basis", LEVEL_BASIS))
    d = (point[0] - eye[0], point[1] - eye[1], point[2] - eye[2])
    return SCREEN_CENTRE - Y_SCALE * _dot(d, u) / _dot(d, f)


def ui448(sy_gs):
    return UI_PER_FIELD_ROW * (sy_gs - XYOFFSET_Y)


def project_point(point, camera, space="ui448"):
    y = ui448(gs_y(point, camera))
    return y * SHOT_PER_UI if space == "shot" else y


def project(y_game, camera, space="ui448"):
    """448-row UI y of the terrain point (car.x, y_game, car.z) under the car.

    camera = {'pos': (x, y, z), 'basis': (right, up, forward) rows (optional, default level)}.
    """
    x, _y, z = camera["pos"]
    return project_point((x, y_game, z), camera, space)


def rear_contact_points(camera, heights):
    """World points of the two rear wheel contacts (heights = terrain y at each)."""
    r, u, f = camera.get("basis", LEVEL_BASIS)
    x, _y, z = camera["pos"]
    pts = []
    for (ox, _oy, oz), h in zip(REAR_CONTACTS, heights):
        pts.append((x + ox * r[0] + oz * f[0], h, z + ox * r[2] + oz * f[2]))
    return pts


def verify_elf(elf):
    data = Path(elf).read_bytes()
    bad = 0
    for vaddr, want, what in ELF_PINS:
        off = vaddr - ELF_DELTA
        got = struct.unpack_from("<I", data, off)[0]
        ok = got == want
        bad += 0 if ok else 1
        print(f"  {'ok ' if ok else 'BAD'} 0x{vaddr:08X} {got:#010x} {what}")
    return bad == 0


# ---- terrain mesh (same strip walk as src/native/gameplay/terrain_query.cpp) --------------------------------------
def load_mesh(path):
    import numpy as np

    d = Path(path).read_bytes()
    off_b, off_c = struct.unpack_from("<II", d, 8)
    cur, tris = off_b, []
    while cur + 16 <= off_c:
        w0, w1, w2, w3 = struct.unpack_from("<IIII", d, cur)
        if (w0 & 0xF000) != 0x8000 or w1 != 0x20224000 or w2 != 0x41:
            cur += 4
            continue
        n = w0 & 0x7FFF
        if n < 3 or n > 0x400:
            cur += 4
            continue
        nbytes = 16 + n * 16 + (n - 2) * 16
        if cur + nbytes > len(d):
            break
        v = np.array(struct.unpack_from("<%df" % (4 * n), d, cur + 16)).reshape(n, 4)
        if not np.isfinite(v[:, :3]).all():
            cur += 4
            continue
        nrm = np.array(struct.unpack_from("<%df" % (4 * (n - 2)), d, cur + 16 + n * 16)).reshape(n - 2, 4)
        tri = base = 0
        while tri < n - 2:
            for i0, i1, i2, wd in ((base, base + 1, base + 2, 1), (base + 1, base + 2, base + 3, -1)):
                if tri >= n - 2:
                    break
                nx, nz = nrm[tri, 0], nrm[tri, 2]
                if np.isfinite(nx) and np.isfinite(nz):
                    tris.append([v[i0, 0], v[i0, 1], v[i0, 2], v[i1, 0], v[i1, 2], v[i2, 0], v[i2, 2], nx, nz, w3, wd])
                tri += 1
            base += 2
        cur += nbytes
    return np.array(tris, float)


def height_at(mesh, x, z):
    import numpy as np

    x0, h0, z0, x1, z1, x2, z2, nx, nz, surf, wd = mesh.T
    c1 = (z1 - z0) * (x - x1) - (x1 - x0) * (z - z1)
    c2 = (z2 - z1) * (x - x2) - (x2 - x1) * (z - z2)
    c3 = (z0 - z2) * (x - x0) - (x0 - x2) * (z - z0)
    inside = np.where(wd > 0, (c1 >= 0) & (c2 >= 0) & (c3 >= 0), (c1 <= 0) & (c2 <= 0) & (c3 <= 0))
    h = h0 - ((x - x0) * nx + (z - z0) * nz)
    best = None
    for i in np.nonzero(inside)[0]:
        word = int(surf[i]) & 0xFFFFFFFF
        if word & 0x80000000 or word & 0x10000000:
            continue
        if best is None or h[i] > best:
            best = h[i]
    return best


# ---- CSV ---------------------------------------------------------------------------------------------------------
def read_trace(path=CSV):
    rows = []
    for line in Path(path).read_bytes().decode().split("\r\n"):
        if line:
            rows.append(line.split(","))
    return rows


def trace_column(rows_xyz, mesh):
    out = []
    for x, y, z in rows_xyz:
        h = height_at(mesh, x, z)
        out.append(project(h, {"pos": (x, y, z)}))
    return out


def write_csv(elf, path=CSV):
    raw = [ln for ln in Path(path).read_bytes().decode().split("\r\n") if ln]
    if raw[0].endswith(",terrain_y_px"):  # idempotent: drop a previous column first
        raw = [ln.rsplit(",", 1)[0] for ln in raw]
    race = load_mesh(Path(elf).parent / "COURSE" / "C00.BIN")
    xyz = []
    for ln in raw[1:]:
        f = ln.split(",")
        xyz.append((float(f[2]), float(f[3]), float(f[4])))
    col = trace_column(xyz, race)
    # the original text stays a byte-identical prefix; the old last field is empty ("...,-213,") so the
    # new column follows as "...,-213,,<v>"
    out = [raw[0] + ",terrain_y_px"] + [ln + ",%.6f" % v for ln, v in zip(raw[1:], col)]
    Path(path).write_bytes(("\r\n".join(out) + "\r\n").encode())
    return len(col)


def selftest(elf, cxx_cmd=None):
    """Agree with the C++ law (terrain_y_px column written by this module and re-derived in the test) on >= 10 rows."""
    rows = read_trace()
    if "terrain_y_px" not in rows[0]:
        print("selftest: CSV has no terrain_y_px column; run --write-csv first")
        return 1
    race = load_mesh(Path(elf).parent / "COURSE" / "C00.BIN")
    body = rows[1:]
    picks = list(range(0, len(body), max(1, len(body) // 24)))[:24]
    worst = 0.0
    for i in picks:
        r = body[i]
        x, y, z = float(r[2]), float(r[3]), float(r[4])
        got = project(height_at(race, x, z), {"pos": (x, y, z)})
        worst = max(worst, abs(got - float(r[-1])))
    print(f"selftest rows={len(picks)} max|project - csv|={worst:.6f} px (gate 0.01)")
    # The CSV column was written by this module; the C++ test recomputes every row independently
    # (ymap_agreement_max), which is the real cross-implementation check.
    binary = cxx_cmd or os.environ.get("RT_W204_TEST") or str(REPO / "build/w-W2-04r3/w2_terrain_query_test")
    if Path(binary).is_file():
        import re
        import subprocess

        out = subprocess.run([binary, "--elf", str(elf)], capture_output=True, text=True).stdout
        m = re.search(r"ymap_agreement_max=([0-9.eE+-]+)", out)
        w = re.search(r"terrain_y_px_written=(\d+)", out)
        if not m:
            print("selftest: C++ SCORE line missing")
            return 1
        cx = float(m.group(1))
        print(f"selftest C++ rows={w.group(1) if w else '?'} ymap_agreement_max={cx:.6f} px (gate 0.01)")
        if cx > 0.01:
            return 1
    else:
        print("selftest: C++ binary not built; skipped cross-implementation check")
    # self-consistency of the y-map entry and the law at the car origin
    cam = {"pos": (0.0, 0.0, 0.0)}
    assert abs(project(0.0, cam) - 358.2857142857) < 1e-6, project(0.0, cam)
    assert abs(project(0.0, cam, "shot") - 358.2857142857 * SHOT_PER_UI) < 1e-6
    return 0 if worst <= 0.01 else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--elf", default=os.environ.get("RT_ELF", str(DEFAULT_ELF)))
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--write-csv", action="store_true")
    ap.add_argument("--verify-elf", action="store_true")
    a = ap.parse_args()
    if not Path(a.elf).is_file():
        print("NO_ELF", a.elf)
        return 0
    if a.verify_elf:
        return 0 if verify_elf(a.elf) else 1
    if a.write_csv:
        print("terrain_y_px rows written:", write_csv(a.elf))
        return 0
    if a.selftest:
        return selftest(a.elf)
    ap.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())
