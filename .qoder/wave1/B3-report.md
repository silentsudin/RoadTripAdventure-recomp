# B3 report — main-menu render audit vs oracle (menu-v13)

Worker: B3 · Date: 2026-10-17 · Repo: RoadTripAdventure-decomp
Deliverable doc: `docs/native-reference/menu-calibration.md`
Artifacts: `build/scratch/hud-art/{menu-sxs.png,menu-diff.png,b3_menu_*.py}`
Code touched: **none** (read-only audit; constants proposed, not applied).

## Baseline

ours `menu-v13.ppm` (800x600) resized to 1280x447 vs oracle `main_menu.png`:
mean |ΔRGB| **63.4**, **67.3 %** of pixels >48 off.

## Root cause (highest leverage)

The `ui_menu.h` scanout table converts `ui_y = scan_y` (T1). The 447-row
scanout actually covers the full 480-line display:
`display_y = scan_y / 0.93125`, `placement_y = display_y − 16`. Dropping the
×1.0738 makes every vertical constant ~3 px too high and every height/pitch
~7 % too short. Verified with three independent anchors (panel rim −3.4/−12.6
scan px top/bottom, text pitch drift −5→−12 across rows, copyright −12); x
mapping (`scan/2`) is correct to ±1 px. Detail + proofs: calibration doc §1.

## What differs structurally (side-by-side + amplified diff)

1. **Missing full-width backdrop bands** (oracle scan y 169–207 navy (7,13,93),
   210–227 gold stripe (244,193,10), left grass (2,125,42) down to y 328) — 5 of
   the 10 worst 8×5 cells; region weight ≈ 29 of the 63.4 total MAD.
2. **Panel geometry**: oracle panel = placement (433, **272**) 162×**143**
   (ours 268/133); row pitch **25.8** UI (ours 24); bar y 278, h 26.5.
3. **Text**: oracle cap height **17.2 UI** vs ours 12.9 (FONTE scaleY must be
   1.0; scaleX 0.77); pitch 25.8; selected label drawn 6 UI left of the others
   (inset 23 vs 29). Diff shows every label double-ghosted at growing offset.
4. **Selection bar**: starts at placement x **457** (panel-left + 24, ours 435),
   w **138.5** (right-aligned to panel rim), body is a **vertical blue
   gradient** (0,110,224 → ~10,60,150), 1 px gold+orange edge lines top/bottom,
   cap block only **8 UI** wide (ours 14.6) with mid-bar cream shine.
5. **Text lines misplaced**: `Published by…` is NOT centered — oracle ink at
   placement x 321..622.5, y 212.7 (ours centered @156, scale 0.40 renders
   dark-edged); copyright ink top placement **436** (ours 422, −14 error).
6. **Horn**: correct size, but 26.4 UI too far left / 6.8 too high (tip 461).
7. **`button Enter` box** (documented GAP but 2.9 % of frame, MAD 83):
   placement (423, 37.7) 193.5×39.7, body (48,64,144), rim (17,159,232),
   label ink (220,114,88).
8. Confirmed-good already: sky (exact), panel body (48,64,144) (exact), rim
   inner (±4), road gray (±4), plate x/w (exact), row inset 6, text inset 29.

## Fix list (ordered by expected score impact — full tables in calibration doc §4–5)

1. Backdrop band quads (navy/stripe/grass) — new, exact colors measured.
2. `kMainY` 268→272, `kMainH` 133→143, `kRowPitch` 24→25.8, `kRowH` 24→26.5.
3. Text metrics: glyph scale (0.75 uniform) → **x 0.77 / y 1.0**, text cell top
   = rowY + 4.3, selected inset 23, publisher (0.45, 0.55) @ (321, 212),
   copyright y 422→436 (scaleY 0.44).
4. Selection bar: x 457, w 138.5, 3-band gradient, gold+orange 1 px edges,
   cap 8 UI + shine h 6.
5. `button Enter` box (optional GAP closure), horn tipX 461, plate y 25 h 194,
   `kRoad` → (125,125,125).

Expected: fixes 1–2 alone address ≈ 33 of 63.4 MAD weight; realistic floor
with the 3D-scene GAP (plate art, cars, town) is overall MAD ≈ 40–45.

## Caveats

- FONTE is monospace, oracle font is proportional: per-glyph widths can't all
  match; the x/y scale split matches cap height AND advance simultaneously.
- Oracle `uiY = scan/0.931` (task brief) is the *display* space; `ui_menu.h`
  constants are placement = display − 16. Both stated in every table.
- Same remap must later be applied to `buildOptionsScreen` (`kOptY`/`kOptH`)
  by measuring `options.png` — out of B3 scope, flagged in the doc.
- No git state changed; writes limited to the two permitted markdown files and
  `build/scratch/hud-art/*.png` + analysis scripts.
