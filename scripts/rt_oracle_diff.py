#!/usr/bin/env python3
"""Native rt-app vs the emulator (oracle) picture accuracy, per scene.

The golden for every scene is the EMULATOR: PS2Recomp running the guest ELF on paraLLEl-GS,
driven headless in lockstep through the same UI a player would use. The native app runs its
own scene at the closest equivalent moment. Both capture at progressive 640x448 so pixels
line up without resampling.

Metrics per scene (docs/native-reference/accuracy-vs-oracle.md):
  mad          mean absolute channel difference (0..255)
  pct16/48     % of pixels any channel differs by >16 / >48
  tolerant     the regression suite's own cross-renderer rule (goldens.py): 3x3 neighbourhood
               both ways at level 48; fail above 3% of pixels or 30% of any 32px block

Usage: RT_DISC=<disc dir> python3 scripts/rt_oracle_diff.py [--tags title,menu,race,town]
                                                [--native-bin build/native/rt-app]
                                                [--out build/scratch/oracle-diff]
"""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPO / "tests" / "regression"))

import numpy as np  # noqa: E402
from PIL import Image  # noqa: E402

from rtharness import Game, seconds  # noqa: E402
from rtharness.adventure import boot_to_main_menu  # noqa: E402
from rtharness import quick_race  # noqa: E402

BASE_DATA = REPO / "build/regression/base"
CHECKPOINT = REPO / "build/regression/checkpoints/adventure_first_save"
# Match the native scanout: progressive full frames; 2x supersample makes the scanout 640x448.
ORACLE_ENV = {"RT_PROGRESSIVE_FIELDS": "1", "RT_GS_SSAA": "2"}


def oracle_capture(tags: set[str], out: Path) -> dict[str, Path]:
    """Frames from the emulator, taken at the same narrative moment per scene."""
    made: dict[str, Path] = {}

    def save(tag: str, frame) -> None:
        p = out / f"oracle_{tag}.png"
        frame.image().save(p)
        made[tag] = p
        print(f"oracle {tag}: {frame.width}x{frame.height}")

    if {"title", "menu"} & tags:
        g = Game(BASE_DATA, out / "run_titlemenu", env=dict(ORACLE_ENV))
        g.start()
        try:
            if "title" in tags:
                g.run(seconds(9))  # boot + logos + the title screen under "press start"
                save("title", g.frame())
            if "menu" in tags:
                if "title" not in tags:
                    g.run(seconds(10))
                else:
                    g.run(seconds(1))
                g.press("start")
                g.run(seconds(3))
                save("menu", g.frame())
        finally:
            g.close()

    if "race" in tags:
        g = Game(BASE_DATA, out / "run_race", env=dict(ORACLE_ENV))
        g.start()
        try:
            quick_race.open_course(g, 0)
            quick_race.start_race(g)
            g.run(seconds(1))  # onto the grid, countdown lights coming up
            save("race", g.frame())
        finally:
            g.close()

    if "town" in tags:
        g = Game(BASE_DATA, out / "run_town", checkpoint=CHECKPOINT, env=dict(ORACLE_ENV))
        g.start()
        try:
            g.run(seconds(14))  # load animation, greeting, the town settles
            g.press("cross")
            g.run(seconds(2))
            save("town", g.frame())
        finally:
            g.close()

    return made


def native_capture(tag: str, out: Path, binary: Path, disc: str) -> Path | None:
    state = {"title": "TITLE", "menu": "MENU", "race": "PLAYING", "town": "PLAYING"}[tag]
    frames = {"title": 8, "menu": 8, "race": 110, "town": 8}[tag]
    # Static states park simulation_frame and explore starts at 1: no RT_APP_SNAPSHOT_AT,
    # every rendered frame dumps and the last one wins.
    snap = out / f"native_{tag}.ppm"
    env = dict(os.environ)
    env.update({
        "RT_DISC": disc,
        "RT_TIME": "virtual",
        "RT_APP_STATE": state,
        "RT_APP_NOAUDIO": "1",
        "RT_APP_HUDTEXT": "1",
        "RT_APP_FRAMES": str(frames),
        "RT_APP_SNAPSHOT": str(snap),
    })
    if tag == "race":
        env["RT_APP_MODE"] = "race"
    elif tag == "town":
        env["RT_APP_MODE"] = "explore"
    t0 = time.monotonic()
    r = subprocess.run([str(binary)], env=env, cwd=str(REPO), timeout=180,
                      stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
    wall = time.monotonic() - t0
    print(f"native {tag}: rc={r.returncode} wall={wall:.1f}s for {frames} frames")
    (out / f"native_{tag}.wall").write_text(f"{wall:.3f} {frames}\n")
    return snap if snap.exists() else None


def compare(oracle_png: Path, native_ppm: Path) -> dict:
    oa = Image.open(oracle_png).convert("RGB")
    nb = Image.open(native_ppm).convert("RGB")
    extent = f"oracle {oa.size[0]}x{oa.size[1]} native {nb.size[0]}x{nb.size[1]}"
    if nb.size != oa.size:
        # Oracle (emulator) pixels are the ground truth geometry: resample native onto it.
        nb = nb.resize(oa.size, Image.BILINEAR)
    a = np.asarray(oa, dtype=np.int16)
    b = np.asarray(nb, dtype=np.int16)
    h, w = a.shape[0], a.shape[1]
    diff = np.abs(a - b)
    n = h * w
    res = {
        "size": extent,
        "mad": round(float(diff.mean()), 2),
        "pct16": round(float((diff.max(axis=2) > 16).mean()) * 100, 2),
        "pct48": round(float((diff.max(axis=2) > 48).mean()) * 100, 2),
    }
    # The suite's cross-renderer rule: each pixel vs the other's 3x3 neighbourhood, both ways.
    def best_match(x, y):
        out = np.full(y.shape[:2], 255, dtype=np.int16)
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                ys, xs = slice(max(0, dy), h + min(0, dy)), slice(max(0, dx), w + min(0, dx))
                d = np.abs(y[ys, xs] - x[max(0, -dy):h - max(0, dy), max(0, -dx):w - max(0, dx)]).max(axis=2)
                out[ys, xs] = np.minimum(out[ys, xs], d)
        return out

    tol = np.maximum(best_match(a, b), best_match(b, a))
    pct = float((tol > 48).mean()) * 100
    block = 32
    worst_block = 0.0
    for by in range(0, h - block + 1, block):
        for bx in range(0, w - block + 1, block):
            worst_block = max(worst_block, float((tol[by:by + block, bx:bx + block] > 48).mean()) * 100)
    res["tolerant_pct"] = round(pct, 2)
    res["worst_block_pct"] = round(worst_block, 1)
    res["pass"] = pct <= 3.0 and worst_block <= 30.0
    return res


def side_by_side(oracle_png: Path, native_ppm: Path, out_png: Path, tag: str, res: dict) -> None:
    a = Image.open(oracle_png).convert("RGB")
    b = Image.open(native_ppm).convert("RGB")
    if b.size != a.size:
        b = b.resize(a.size, Image.BILINEAR)
    w, h = a.size
    canvas = Image.new("RGB", (w * 2 + 8, h + 20), (20, 20, 20))
    canvas.paste(a, (0, 20))
    canvas.paste(b, (w + 8, 20))
    from PIL import ImageDraw

    d = ImageDraw.Draw(canvas)
    d.text((4, 4), f"{tag}: ORACLE (emulator, left) vs NATIVE (right)  mad {res['mad']}  "
                  f"pct>16 {res['pct16']}%  tolerant {res['tolerant_pct']}%  {'PASS' if res['pass'] else 'FAIL'}",
           fill=(240, 240, 240))
    canvas.save(out_png)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--tags", default="title,menu,race,town")
    ap.add_argument("--native-bin", default=str(REPO / "build/native/rt-app"))
    ap.add_argument("--out", default=str(REPO / "build/scratch/oracle-diff"))
    ap.add_argument("--skip-oracle", action="store_true", help="reuse oracle PNGs already in --out")
    args = ap.parse_args()

    disc = os.environ.get("RT_DISC") or str(Path.home() / "Library/Application Support/RoadTripRecomp/disc")
    if not Path(disc, "SLUS_203.98").exists():
        print(f"disc not found at {disc}", file=sys.stderr)
        return 2

    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    tags = {t.strip() for t in args.tags.split(",") if t.strip()}

    if args.skip_oracle:
        made = {t: out / f"oracle_{t}.png" for t in tags if (out / f"oracle_{t}.png").exists()}
    else:
        made = oracle_capture(tags, out)

    report = {}
    for tag in sorted(tags):
        op = made.get(tag)
        np_ = native_capture(tag, out, Path(args.native_bin), disc)
        if not op or not op.exists() or not np_:
            report[tag] = {"error": f"missing capture (oracle={bool(op and op.exists())}, native={bool(np_)})"}
            continue
        res = compare(op, np_)
        side_by_side(op, np_, out / f"diff_{tag}.png", tag, res)
        report[tag] = res
        print(f"{tag:6s} {res['size']} mad={res['mad']:6.2f} pct16={res['pct16']:5.1f}% "
              f"pct48={res['pct48']:5.1f}% tolerant={res['tolerant_pct']:5.1f}% "
              f"worstBlock={res['worst_block_pct']:5.1f}% -> {'PASS' if res['pass'] else 'FAIL'}")

    (out / "accuracy.json").write_text(json.dumps(report, indent=2) + "\n")
    print(f"\nwrote {out}/accuracy.json and diff_<tag>.png side-by-sides")
    return 0


if __name__ == "__main__":
    sys.exit(main())
