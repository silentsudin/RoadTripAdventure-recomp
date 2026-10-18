# Worker B2 report — sky/ground color calibration

## Summary
Produced `docs/native-reference/sky-ground-calibration.md` with per-screen sky sampling, horizon uiY, bright blobs, native-vs-oracle row diffs, and proposed gradient-stop arrays. Scratch PNGs saved under `build/scratch/hud-art/`.

## Key numbers
- Oracle UI mapping: uiX=px/2, uiY=px/0.931 (1280x447).
- Chosen sky columns: title/main_menu x=40; town x=320; race x=1240; attract none.
- Horizons (uiY): title 89.2, main_menu 89.2, town 87.0, race 87.0, attract n/a.
- Sky gradients (max residual): title 6.86 (1 stop), main_menu 1.98 (2 stops), town 5.53 (3 stops), race 3.60 (3 stops).
- Sun flare badge: px (645,36) -> ui (322,39), colour (255,248,18), ~90x4 px.
- Native overall MAE: town 53.54, race 49.16, title 30.85 (800x600 resized to 1280x447 bilinear).

## Confidence
- High for title/main_menu (flat sky, clean column).
- Medium for town/race (single-column gradient; horizontal variation not captured).
- Low for attract (no identifiable horizon).

## Open questions
- Native worst rows are ground rows (y~160-178), not sky; sky calibration alone won't fix them.
- Attract sky handling deferred.
- Column choices assume HUD-free margins; verify with full HUD mask if needed.
