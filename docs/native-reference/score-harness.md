# W2 oracle score harness

`scripts/w2_score.py` compares an oracle PNG and native PNG at both the 320×180 letterboxed canvas and full oracle resolution. Alpha PNGs are composited over black before statistics because transparent pixels may contain unrelated RGB. Metrics are mean absolute RGB difference, 8×8-block SSIM, Sobel edge-density ratio, histogram intersection, and RGB mean delta. `--scene title|menu|town|race` selects named ROIs; ROI coordinates are in 1280×447 oracle space, scaled to the 320×180 canvas for canvas metrics.

```sh
python3 scripts/w2_score.py --scene race \
  --oracle build/scratch/oracle-golden/race_hud_driving.png \
  --native build/scratch/gamelook/race-v13.ppm
python3 tests/native_w2/score_selftest.py
```

The self-test imports the existing `scripts/frame_compare.py` for compatibility, not for implementation, and checks §1's registered v13 continuity values (0.872/0.460/0.532/0.374). Source rows are `oracle-golden-frames.md:45-55`; screen dimensions and comparison context are `sky-ground-calibration.md:3-5` and `frame-compare.md:38-45`.

## Named-ROI y-map registry (coordinator amendment)

`YMAP_REGISTRY` contains exactly the required ROIs: `sky`, `ground`, `hud_dial`, `hud_timer`, `hud_speed_digits`, `minimap`, `menu_panel`, `title_logo`. Every entry is explicitly `FIT`, `ELEMENT`, or `GAP`; the test rejects missing keys, invalid entries, FITs with fewer than three cited coordinate pairs or residual above 1 px, ELEMENTs without a citation/scale/offset, and GAPs without a reason. Current status is `0+2+6/8`: no FIT claim, two evidence-grounded ELEMENT mappings, and six explicit gaps.

- `menu_panel` is `ELEMENT`, scale `1/0.93125`, offset `−16`: `menu-calibration.md:15-24` gives `placement_y = scan_y / 0.93125 − 16`.
- `hud_speed_digits` is `ELEMENT`, scale `1`, offset `0`: `gauge-calibration.md:3-8` identifies the dial as a 448-line HUD element and describes its screenshot-space measurement. The scene-dependent confirmation is `.qoder/wave2-plan-sonnet.md:23-27`, which records glyph y approximately equal to 448-space y (scale ≈0.998, offset ≈0).
- The other six entries are `GAP` with a per-ROI rationale. They do not inherit a scene-wide or neighboring-element transform.

Original acceptance required “a per-scene mapping table pinned with fit residual ≤ 1.0 px” and `ymap_fits=<n>/<n>` (`.qoder/wave2-plan-sonnet.md:375-389`). Coordinator amendment replaces that criterion with the named-ROI FIT/ELEMENT/GAP registry; a single per-scene y-map would contradict the element-specific evidence. The final test line uses the amended `ymap_status=<fit>+<element>+<gap>/<total>` form and passes only when every required ROI is classified with the required evidence/reason, plus the continuity/identity/shift/noise checks.

Named ROI rectangles are initial broad diagnostic windows, not semantically segmented masks. Review them against the indicated screenshot and tune after integration; full-resolution metrics compare pixels in oracle coordinates (native coordinates proportionally mapped when dimensions differ). ROI metrics must not be interpreted as pixel-perfect object alignment on differently framed inputs. All constants defining canvas dimensions and histogram bins retain the existing harness values (`scripts/frame_compare.py:20-21`); the 1280×447 dimensions are cited above.
