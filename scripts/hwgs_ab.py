#!/usr/bin/env python3
"""A/B the hardware GS against paraLLEl-GS on the regression suite's frames.

Runs the regression suite (tests/regression, the same movies and checkpoints) once per GS backend,
capturing every frame a test hands to its golden check instead of checking it, then compares the
two picture by picture: a heatmap per scene, SSIM and the share of pixels over a threshold, and a
summary ranked by difference.

    python3 scripts/hwgs_ab.py run --out build/scratch/hwgs_ab/before [-n 4] [-k EXPR]
            [--app APP] [--base-data DIR] [--work DIR] [--ssaa 1] [--backends pgs,hw]
    python3 scripts/hwgs_ab.py compare --out build/scratch/hwgs_ab/before      # metrics again
    python3 scripts/hwgs_ab.py versus build/scratch/hwgs_ab/before build/scratch/hwgs_ab/after

Both backends render at the same scale (--ssaa 1: paraLLEl-GS without supersampling, the hardware
GS at 1x; 4 -> 2x, 16 -> 4x) with the original interlaced fields (RT_PROGRESSIVE_FIELDS=0, as the
suite). paraLLEl-GS is the reference. Everything written is game output: keep --out under build/.

Run as a pytest plugin (-p hwgs_ab, from `run`), this module patches the harness: golden checks
save the frame to $HWGS_AB_CAPTURE/<name>.png and pass, and every game gets $HWGS_AB_ENV.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SUITE = ROOT / "tests/regression"
VENV_PY = ROOT / "build/regress-venv/bin/python"
DEFAULT_APP = ROOT / "build/macos-release/RoadTrip.app/Contents/MacOS/RoadTrip"
# Tests that change the scanout (progressive fields, packs), measure time or take no pictures (save
# states): not image parity.
DEFAULT_DESELECT = "not progressive and not texture_pack and not perf and not save_states"
THRESHOLD = 24  # a pixel "differs" when a channel is off by more than this (0..255)


def safe(name: str) -> str:
    return "".join(c if c.isalnum() or c in "-_." else "_" for c in name)


# ---------------------------------------------------------------------------------------- plugin
def pytest_configure(config):
    capture = os.environ.get("HWGS_AB_CAPTURE")
    if not capture:
        return
    sys.path.insert(0, str(SUITE))
    import rtharness
    from rtharness import goldens

    out = Path(capture)
    out.mkdir(parents=True, exist_ok=True)
    inject = json.loads(os.environ.get("HWGS_AB_ENV", "{}"))

    def check(name, frame, test_data, update=False):
        frame.image().save(out / f"{safe(name)}.png")
        with open(out / "names.tsv", "a") as f:
            f.write(f"{safe(name)}\t{name}\t{frame.width}x{frame.height}\n")

    goldens.check = check
    goldens.check_audio = lambda *a, **k: None

    start = rtharness.Game.start

    def start_with_backend(self):
        self.extra_env = {**self.extra_env, **inject}
        return start(self)

    rtharness.Game.start = start_with_backend


# ---------------------------------------------------------------------------------------- run
def prepare_base(app: Path, base: Path):
    """A short launch, so the game library is (re)built for this app once, not by parallel tests."""
    env = dict(os.environ, RT_DATA_DIR=str(base), RT_HEADLESS="1", RT_EXIT_AT_VBLANK="2",
               RT_TIME="virtual", RT_SPEED="max")
    print(f"[ab] preparing {base} for {app} ...", flush=True)
    subprocess.run([str(app)], env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=3600)


def run_backend(args, backend: str) -> float:
    out = Path(args.out)
    capture = out / backend
    if capture.exists():
        shutil.rmtree(capture)
    capture.mkdir(parents=True)
    work = Path(args.work) / backend
    if work.exists():
        shutil.rmtree(work)
    work.mkdir(parents=True)
    if args.seed_checkpoints and Path(args.seed_checkpoints).is_dir():
        subprocess.run(["cp", "-c", "-R", str(args.seed_checkpoints), str(work / "checkpoints")], check=False)
    scale = {1: 1, 4: 2, 16: 4}.get(args.ssaa, 1)
    inject = {"RT_GS_BACKEND": backend, "RT_GS_SSAA": str(args.ssaa), "RT_PROGRESSIVE_FIELDS": "0"}
    if backend == "hw":
        inject["RT_GS_SCALE"] = str(scale)
    env = dict(os.environ, HWGS_AB_CAPTURE=str(capture), HWGS_AB_ENV=json.dumps(inject),
               PYTHONPATH=os.pathsep.join([str(ROOT / "scripts"), os.environ.get("PYTHONPATH", "")]))
    env.pop("RT_TEST_RENDER_ALWAYS", None)
    k = f"({DEFAULT_DESELECT})" + (f" and ({args.k})" if args.k else "")
    cmd = [str(args.python), "-m", "pytest", ".", "-p", "hwgs_ab", "-q", "-rfE", f"-n{args.n}", "-k", k,
           f"--app={args.app}", f"--base-data={args.base_data}", f"--test-data={work}"]
    print(f"[ab] {backend}: {' '.join(cmd)}", flush=True)
    t0 = time.time()
    with open(out / f"{backend}.pytest.log", "w") as log:
        subprocess.run(cmd, cwd=SUITE, env=env, stdout=log, stderr=subprocess.STDOUT)
    took = time.time() - t0
    tail = (out / f"{backend}.pytest.log").read_text(errors="replace").strip().splitlines()[-1:]
    print(f"[ab] {backend}: {took / 60:.1f} min, {len(list(capture.glob('*.png')))} frames; {tail[0] if tail else ''}",
          flush=True)
    return took


# ---------------------------------------------------------------------------------------- metrics
def box(img, r: int):
    """Mean over a (2r+1)^2 window (edges: the window inside the image)."""
    import numpy as np

    pad = np.pad(img, r, mode="edge")
    c = pad.cumsum(0).cumsum(1)
    c = np.pad(c, ((1, 0), (1, 0)))
    n = 2 * r + 1
    s = c[n:, n:] - c[:-n, n:] - c[n:, :-n] + c[:-n, :-n]
    return s / (n * n)


def ssim(a, b) -> float:
    """SSIM of the luma of two RGB images (7x7 uniform window)."""
    import numpy as np

    w = np.array([0.299, 0.587, 0.114])
    x = (a.astype(np.float64) @ w)
    y = (b.astype(np.float64) @ w)
    c1, c2 = (0.01 * 255) ** 2, (0.03 * 255) ** 2
    mx, my = box(x, 3), box(y, 3)
    sxx = box(x * x, 3) - mx * mx
    syy = box(y * y, 3) - my * my
    sxy = box(x * y, 3) - mx * my
    m = ((2 * mx * my + c1) * (2 * sxy + c2)) / ((mx * mx + my * my + c1) * (sxx + syy + c2))
    return float(m.mean())


def heat_colors(d):
    """0..255 difference -> black, blue, red, yellow, white."""
    import numpy as np

    stops = np.array([[0, 0, 0], [40, 40, 200], [220, 30, 30], [255, 220, 0], [255, 255, 255]], dtype=np.float64)
    t = np.clip(d.astype(np.float64) / 128.0, 0, 1) * (len(stops) - 1)
    i = np.minimum(t.astype(int), len(stops) - 2)
    f = (t - i)[..., None]
    return (stops[i] * (1 - f) + stops[i + 1] * f).astype(np.uint8)


def compare_one(ref_path: Path, hw_path: Path, out_dir: Path, name: str) -> dict:
    import numpy as np
    from PIL import Image

    ra, ha = Image.open(ref_path).convert("RGB"), Image.open(hw_path).convert("RGB")
    sizes = f"{ra.width}x{ra.height} / {ha.width}x{ha.height}"
    if ra.width == ha.width and ra.height == 2 * ha.height:
        # paraLLEl-GS scans interlaced fields out as a 448-line frame: the field drawn this frame
        # on one parity of lines (the other holds the previous field). Compare with that field.
        full = np.asarray(ra, dtype=np.int16)
        hw = np.asarray(ha, dtype=np.int16)
        parity = min((0, 1), key=lambda p: np.abs(full[p::2] - hw).mean())
        ra = Image.fromarray(full[parity::2].astype(np.uint8))
        sizes += f" (field {parity})"
    elif ra.size != ha.size:
        # Compare at the smaller size (box-filtered down).
        size = (min(ra.width, ha.width), min(ra.height, ha.height))
        ra, ha = ra.resize(size, Image.BOX), ha.resize(size, Image.BOX)
    a = np.asarray(ra, dtype=np.int16)
    b = np.asarray(ha, dtype=np.int16)
    diff = np.abs(a - b).max(axis=2)
    over = float((diff > THRESHOLD).mean()) * 100
    over8 = float((diff > 8).mean()) * 100
    mean = float(np.abs(a - b).mean())
    s = ssim(a, b)
    heat = heat_colors(diff)
    Image.fromarray(heat).save(out_dir / f"{name}.heat.png")
    gap = np.full((a.shape[0], 4, 3), 64, np.uint8)
    Image.fromarray(np.concatenate([a.astype(np.uint8), gap, b.astype(np.uint8), gap, heat], axis=1)).save(
        out_dir / f"{name}.ab.png")
    # Where the difference is: the bounding box of pixels over the threshold.
    ys, xs = np.nonzero(diff > THRESHOLD)
    bbox = [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None
    return {"name": name, "over": round(over, 3), "over8": round(over8, 3), "mean": round(mean, 3),
            "ssim": round(s, 5), "max": int(diff.max()), "sizes": sizes, "bbox": bbox}


def compare(out: Path) -> list[dict]:
    ref, hw = out / "pgs", out / "hw"
    diffs = out / "diff"
    diffs.mkdir(exist_ok=True)
    names = sorted({p.stem for p in ref.glob("*.png")} | {p.stem for p in hw.glob("*.png")})
    rows = []
    for n in names:
        if not (ref / f"{n}.png").exists() or not (hw / f"{n}.png").exists():
            rows.append({"name": n, "missing": "pgs" if not (ref / f"{n}.png").exists() else "hw"})
            continue
        rows.append(compare_one(ref / f"{n}.png", hw / f"{n}.png", diffs, n))
    rows.sort(key=lambda r: (-r.get("over", 101), r.get("ssim", 0)))
    (out / "summary.json").write_text(json.dumps(rows, indent=1) + "\n")
    lines = [f"# Hardware GS vs paraLLEl-GS ({out.name})", "",
             f"{len([r for r in rows if 'over' in r])} scenes; ranked by % of pixels with a channel off by more "
             f"than {THRESHOLD}. Heatmaps: diff/<scene>.ab.png (paraLLEl-GS | hardware GS | difference).", "",
             f"| # | scene | % > {THRESHOLD} | % > 8 | mean | SSIM | max | sizes (ref / hw) |",
             "|---|---|---|---|---|---|---|---|"]
    for i, r in enumerate(rows, 1):
        if "missing" in r:
            lines.append(f"| {i} | {r['name']} | missing on {r['missing']} | | | | | |")
        else:
            lines.append(f"| {i} | {r['name']} | {r['over']:.2f} | {r['over8']:.2f} | {r['mean']:.2f} | "
                         f"{r['ssim']:.4f} | {r['max']} | {r['sizes']} |")
    ok = [r for r in rows if "over" in r]
    if ok:
        lines += ["", f"Mean % > {THRESHOLD}: {sum(r['over'] for r in ok) / len(ok):.2f}; "
                      f"mean SSIM {sum(r['ssim'] for r in ok) / len(ok):.4f}"]
    (out / "summary.md").write_text("\n".join(lines) + "\n")
    html = ["<!doctype html><meta charset=utf-8><title>hwgs A/B</title><style>body{font:13px sans-serif;"
            "background:#111;color:#ddd}img{max-width:100%;image-rendering:pixelated}td{padding:2px 6px}</style>",
            f"<h1>Hardware GS vs paraLLEl-GS: {out.name}</h1><table>"]
    for r in ok:
        html.append(f"<tr><td>{r['name']}<br>{r['over']:.2f}% &gt; {THRESHOLD}<br>SSIM {r['ssim']:.4f}</td>"
                    f"<td><a href='diff/{r['name']}.ab.png'><img src='diff/{r['name']}.ab.png' width=960></a></td></tr>")
    html.append("</table>")
    (out / "index.html").write_text("\n".join(html))
    return rows


def versus(before: Path, after: Path):
    a = {r["name"]: r for r in json.loads((before / "summary.json").read_text())}
    b = {r["name"]: r for r in json.loads((after / "summary.json").read_text())}
    rows = []
    for n in sorted(set(a) | set(b)):
        ra, rb = a.get(n, {}), b.get(n, {})
        rows.append((n, ra.get("over"), rb.get("over"), ra.get("ssim"), rb.get("ssim")))
    rows.sort(key=lambda r: -(r[1] if r[1] is not None else 0))
    print(f"| scene | % > {THRESHOLD} before | after | SSIM before | after |")
    print("|---|---|---|---|---|")
    fmt = lambda v, p: "-" if v is None else f"{v:.{p}f}"
    for n, oa, ob, sa, sb in rows:
        print(f"| {n} | {fmt(oa, 2)} | {fmt(ob, 2)} | {fmt(sa, 4)} | {fmt(sb, 4)} |")
    both = [r for r in rows if None not in r[1:]]
    if both:
        print(f"\nmean % > {THRESHOLD}: {sum(r[1] for r in both) / len(both):.2f} -> {sum(r[2] for r in both) / len(both):.2f}; "
              f"mean SSIM {sum(r[3] for r in both) / len(both):.4f} -> {sum(r[4] for r in both) / len(both):.4f} "
              f"({len(both)} scenes)")


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("--out", required=True)
    r.add_argument("--app", default=str(DEFAULT_APP))
    r.add_argument("--base-data", default=os.environ.get("RT_TEST_BASE", str(ROOT / "build/regression/base")))
    r.add_argument("--work", default=None, help="test data (runs, checkpoints) per backend; default <out>/work")
    r.add_argument("--seed-checkpoints", default=str(ROOT / "build/regression/checkpoints"))
    r.add_argument("--python", default=str(VENV_PY if VENV_PY.exists() else sys.executable))
    r.add_argument("--backends", default="pgs,hw")
    r.add_argument("--ssaa", type=int, default=1, choices=[1, 4, 16])
    r.add_argument("-n", type=int, default=4)
    r.add_argument("-k", default="")
    r.add_argument("--no-prepare", action="store_true")
    c = sub.add_parser("compare")
    c.add_argument("--out", required=True)
    v = sub.add_parser("versus")
    v.add_argument("before")
    v.add_argument("after")
    args = p.parse_args()

    if args.cmd == "run":
        args.out = str(Path(args.out).resolve())
        args.app = str(Path(args.app).resolve())
        args.base_data = str(Path(args.base_data).resolve())
        out = Path(args.out)
        out.mkdir(parents=True, exist_ok=True)
        args.work = str(Path(args.work).resolve()) if args.work else str(out / "work")
        if args.seed_checkpoints:
            args.seed_checkpoints = str(Path(args.seed_checkpoints).resolve())
        if not args.no_prepare:
            prepare_base(Path(args.app), Path(args.base_data))
        for backend in args.backends.split(","):
            run_backend(args, backend)
        rows = compare(out)
    elif args.cmd == "compare":
        rows = compare(Path(args.out))
    else:
        versus(Path(args.before), Path(args.after))
        return 0
    print((Path(args.out) / "summary.md").read_text())
    return 0


if __name__ == "__main__":
    sys.exit(main())
