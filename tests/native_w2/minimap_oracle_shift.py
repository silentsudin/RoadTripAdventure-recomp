#!/usr/bin/env python3
"""W2-11 oracle helper for the minimap projection test.

  minimap_oracle_shift.py export [--golden DIR] [--out DIR]
      Alpha-composite the oracle goldens over black and write binary PPM crops that
      w2_minimap_test reads:  town_driving_town.ppm / town_driving_street_town.ppm
      (shot x 80..330, y 300..425) and race_hud_race.ppm / race_hud_driving_race.ppm
      (shot x 60..300, y 110..290).  Crop origins are constants in the C++ test.
  minimap_oracle_shift.py shift [--golden DIR]
      Reference (scipy) measurement of the content shift between town_driving and
      town_driving_street (road + building feature masks, gaussian sigma 1.5, 0.05 px grid
      refinement); prints oracle_dx / oracle_dy in 1280x447 shot px.  The C++ test repeats the
      same measurement with no scipy; the two agree to ~0.1 px.
"""
import argparse
import sys
from pathlib import Path

import numpy as np
from PIL import Image

TOWN_CROP = (80, 300, 330, 425)
RACE_CROP = (60, 110, 300, 290)
CROPS = {
    "town_driving_town": ("town_driving", TOWN_CROP),
    "town_driving_street_town": ("town_driving_street", TOWN_CROP),
    "race_hud_race": ("race_hud", RACE_CROP),
    "race_hud_driving_race": ("race_hud_driving", RACE_CROP),
}


def load(golden: Path, frame: str) -> np.ndarray:
    im = Image.open(golden / f"{frame}.png").convert("RGBA")
    bg = Image.new("RGBA", im.size, (0, 0, 0, 255))
    bg.alpha_composite(im)
    return np.asarray(bg.convert("RGB")).astype(np.int32)


def export(golden: Path, out: Path) -> None:
    out.mkdir(parents=True, exist_ok=True)
    for name, (frame, (x0, y0, x1, y1)) in CROPS.items():
        a = load(golden, frame)[y0:y1, x0:x1].astype(np.uint8)
        with (out / f"{name}.ppm").open("wb") as fh:
            fh.write(b"P6\n%d %d\n255\n" % (a.shape[1], a.shape[0]))
            fh.write(a.tobytes())
        print("wrote", out / f"{name}.ppm", a.shape[1], "x", a.shape[0])


def features(a: np.ndarray) -> np.ndarray:
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    road = ((b - r) > 40) & (r < 80)
    bld = ((r - b) > 15) & (r < 110) & (g < 90)
    return road.astype(float) + bld.astype(float)


def shift(golden: Path) -> tuple[float, float]:
    from scipy import ndimage

    A, B = load(golden, "town_driving"), load(golden, "town_driving_street")
    x0, x1, y0, y1 = 91, 319, 312, 414  # SCISSOR_2 45..158 x 156..207 in shot px
    yy, xx = np.mgrid[:447, :1280]
    excl = ((xx - 204) ** 2 + (yy - 366) ** 2) < 24**2
    win = np.zeros((447, 1280), bool)
    win[y0 + 2 : y1 - 2, x0 + 2 : x1 - 2] = True
    sa = ndimage.gaussian_filter(features(A), 1.5)
    sb = ndimage.gaussian_filter(features(B), 1.5)
    valid = (win & ~excl).astype(float)

    def err(dx: float, dy: float) -> float:
        sh = ndimage.shift(sa, (dy, dx), order=1, mode="nearest")
        m = win & ~excl & (ndimage.shift(valid, (dy, dx), order=1) > 0.99)
        return float(((sh - sb)[m] ** 2).mean())

    best = min(((err(dx, dy), dx, dy) for dy in np.arange(-4, 16, 0.25) for dx in np.arange(-16, 4, 0.25)))
    _, bx, by = best
    best = min(((err(dx, dy), dx, dy) for dy in np.arange(by - 0.5, by + 0.51, 0.05)
                for dx in np.arange(bx - 0.5, bx + 0.51, 0.05)))
    return best[1], best[2]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("cmd", choices=["export", "shift"])
    ap.add_argument("--golden", default="build/scratch/oracle-golden")
    ap.add_argument("--out", default="build/w-W2-11/oracle")
    a = ap.parse_args()
    if a.cmd == "export":
        export(Path(a.golden), Path(a.out))
    else:
        dx, dy = shift(Path(a.golden))
        print("oracle_dx=%.2f oracle_dy=%.2f" % (dx, dy))
    return 0


if __name__ == "__main__":
    sys.exit(main())
