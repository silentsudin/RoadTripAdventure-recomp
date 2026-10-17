#!/usr/bin/env python3
"""W2-10 oracle ROI helper.

  hud_oracle_diff.py export  [--golden DIR] [--out DIR]
      Alpha-composite the oracle goldens over black (their RGB under alpha is
      garbage) and write the gauge ROI of each as binary PPM (<frame>_roi.ppm).
      The C++ test (w2_hud_live_test --oracle-crops DIR) reads these.
  hud_oracle_diff.py diff RASTER.ppm FRAME [--golden DIR]
      Mean-abs-diff (0..255) and 32-bin histogram intersection of the ROI of a
      rt-w2-hud_raster shot-space PPM against the oracle FRAME.

ROI (shot px, no 480-line mapping): x 1056..1188, y 380..415.
"""
import argparse
import sys
from pathlib import Path

import numpy as np
from PIL import Image

ROI = (1056, 380, 1188, 415)
FRAMES = ["race_hud", "race_hud_driving", "town_driving", "town_driving_street"]


def oracle_roi(golden: Path, frame: str) -> np.ndarray:
    im = Image.open(golden / f"{frame}.png").convert("RGBA")
    bg = Image.new("RGBA", im.size, (0, 0, 0, 255))
    bg.alpha_composite(im)
    return np.asarray(bg.convert("RGB").crop(ROI)).astype(np.int32)


def read_ppm(path: Path) -> np.ndarray:
    data = path.read_bytes()
    parts = data.split(b"\n", 3)
    assert parts[0] == b"P6"
    w, h = map(int, parts[1].split())
    return np.frombuffer(parts[3], dtype=np.uint8).reshape(h, w, 3).astype(np.int32)


def hist_intersection(a: np.ndarray, b: np.ndarray) -> float:
    s = 0.0
    for c in range(3):
        ha, _ = np.histogram(a[..., c], bins=32, range=(0, 256))
        hb, _ = np.histogram(b[..., c], bins=32, range=(0, 256))
        s += np.minimum(ha, hb).sum() / max(ha.sum(), 1)
    return s / 3.0


def main() -> int:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    e = sub.add_parser("export")
    e.add_argument("--golden", default="build/scratch/oracle-golden")
    e.add_argument("--out", default="build/w-W2-10/oracle_crops")
    d = sub.add_parser("diff")
    d.add_argument("raster")
    d.add_argument("frame")
    d.add_argument("--golden", default="build/scratch/oracle-golden")
    a = ap.parse_args()
    if a.cmd == "export":
        out = Path(a.out)
        out.mkdir(parents=True, exist_ok=True)
        for f in FRAMES:
            roi = oracle_roi(Path(a.golden), f).astype(np.uint8)
            with open(out / f"{f}_roi.ppm", "wb") as fh:
                fh.write(f"P6\n{roi.shape[1]} {roi.shape[0]}\n255\n".encode())
                fh.write(roi.tobytes())
            print("wrote", out / f"{f}_roi.ppm", roi.shape)
        return 0
    x0, y0, x1, y1 = ROI
    ours = read_ppm(Path(a.raster))[y0:y1, x0:x1]
    ref = oracle_roi(Path(a.golden), a.frame)
    mad = float(np.abs(ours - ref).mean())
    print(f"ROI {a.frame}: mad={mad:.3f} hist={hist_intersection(ours, ref):.4f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
