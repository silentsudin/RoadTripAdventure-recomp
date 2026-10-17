# Native frame comparison

`scripts/frame_compare.py` measures a native `rt-app` snapshot against a PS2 oracle golden. It is the rebuild program's standing visual-parity **calibration** gate: record the result for each rehearsal wave and use the trend to decide what to rebuild next. Its `FAR`, `NEAR`, and `CLOSE` labels are not release pass/fail results; native scenes are not yet 1:1 with the oracle.

The oracle PNGs and their guest-state manifest are untracked under `build/scratch/oracle-golden/`. See [oracle-golden-frames.md](oracle-golden-frames.md) for the capture route and exact state represented by each golden.

## Rehearsal loop

Capture the native state that corresponds as closely as possible to the oracle state, then compare it. `RT_APP_SNAPSHOT` writes P6 PPM snapshots; the tool accepts those and PNGs.

```sh
# The desktop title path is windowed; no RT_NOWINDOW/headless-title switch exists.
RT_APP_STATE=TITLE RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 \
  RT_APP_SNAPSHOT=/tmp/title-native.ppm \
  ./build/native/rt-app

python3 scripts/frame_compare.py \
  --oracle build/scratch/oracle-golden/title.png \
  --native /tmp/title-native.ppm \
  --label title-native \
  --json build/scratch/frame-compare/title-native.json
```

For a HUD-focused comparison, select its rectangle **after normalization**, in 320×180 letterboxed-canvas pixels:

```sh
python3 scripts/frame_compare.py \
  --oracle build/scratch/oracle-golden/race_hud.png \
  --native /tmp/race-native.ppm \
  --roi 220,0,100,70 --label race-hud \
  --json build/scratch/frame-compare/race-hud.json
```

The tool prints the complete JSON report and, when `--json` is supplied, writes that same report. This makes the rehearsal artifact easy to diff without making the result a binary test decision.

## Metrics

Before comparison, each RGB image is resized with Pillow bilinear filtering to fit within a 320×180 canvas while preserving aspect ratio. Unused canvas pixels are black (the JSON `resize_mode` is `letterbox-fit-bilinear`). An optional ROI selects from that common canvas, so it is meaningful even when the source images have different resolutions or aspect ratios.

* **`mean_abs_diff_per_channel`** — mean absolute RGB difference on the normalized canvas/ROI; lower is more alike.
* **`channel_stats`** — mean and standard deviation of each RGB channel for each image.
* **`histogram_intersection`** — mean of three normalized, 32-bin RGB histogram intersections (and the per-channel values). `1.0` is an identical color distribution.
* **`non_black_fraction`** — fraction of pixels with any non-zero channel. This includes the effect of letterbox padding.
* **`edge_density`** — mean Sobel magnitude of luminance, plus native/oracle ratio. A ratio below one usually indicates a flatter or less detailed native frame. If the oracle density is zero the ratio is `null` rather than a fabricated infinity.
* **`verdict`** — histogram-only band: `FAR` below `0.35`, `NEAR` from `0.35` through `0.70`, and `CLOSE` above `0.70`.

## Worked title example

On 2026-10-17, the title command above produced an 800×600 native PPM and was measured against the 1280×447 oracle `title.png`. It reported:

| Metric | Value |
|---|---:|
| Mean absolute difference (R, G, B) | 67.0638, 73.4130, 64.9505 |
| Histogram intersection (R, G, B) | 0.4928, 0.3322, 0.3264 |
| Histogram intersection | 0.3838 (`NEAR`) |
| Non-black fraction (oracle / native) | 0.6222 / 0.7500 |
| Edge density (oracle / native) | 101.0087 / 15.8429 |
| Native/oracle edge ratio | 0.1568 |

This `NEAR` result does **not** imply visual title parity: the present native title is intentionally coarse, and a global color histogram ignores object placement, text correctness, animation timing, and camera pose. Conversely, letterbox padding, interpolation, capture timing, and a mismatched guest state can move these numbers without a meaningful rendering regression. Pair each report with the oracle manifest state and visual inspection; use the bands and individual metrics to track progress, not to gate the program to zero.
