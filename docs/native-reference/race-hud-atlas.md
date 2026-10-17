# Race HUD atlas — TOTAL TIME banner, timer, rank

Measured 2026-10-17 against `build/scratch/oracle-golden/race_hud_driving.png`
(1280×447) and the decoded SCORE pages in `build/scratch/hud-art/`. Every
rectangle below was read off pixels (PIL); nothing here is copied from the
ROM UV tables in [`hud-layout.md`](hud-layout.md). Those tables are cited
only as a cross-check.

Validation paste: `build/scratch/hud-art/race_hud_mock.py` →
`race_hud_mock.png`. Oracle crop: `oracle_race_topleft.png` (px 0..440 ×
0..200). Overlay comparison: `race_hud_side_by_side.png`.

## 1. Which page

**The banner glyphs and the timer digits are the same texels.**

| | |
|---|---|
| File | `SYS/SCORE.GSL` |
| Unit | first PSMT4 upload, 256×256, image at file offset **0x70** |
| Pre-extracted PNG | `SCORE.GSL_00000070_256x256_psm14.png` — this is **CLUT bank 0** |
| Colour match | **CSA 1** of that page's 16×16 CT16 CLUT (256 entries, 16 banks) |

Bank 0 is what the existing dump shows: pink digits `(248, 88, 200)` and
pinkish letters `(232, 200, 208)`. The oracle does not use those colours
for the timer or the banner. Re-decoding the same indices with CSA 1
(`build/scratch/hud-art/score-banks/palette-variants/711681979ef6457b_256x256_psm14.png`,
from `rt_texture_extract` on `SCORE.GSL`) hits the oracle colours:

| sample | oracle dominant | CSA 1 | CSA 0 |
|---|---|---|---|
| Banner letter interior | `(208, 200, 248)` (1062 px in the fill) | `(208, 200, 248)` distance **0** | `(232, 200, 208)` distance 64 |
| Yellow T fill | `(208, 248, 48)` | `(208, 248, 48)` | pink, not yellow |
| Yellow T foot | `(232, 176, 16)` | `(232, 176, 16)` | — |
| Timer digit fill | `(200, 240, 48)`, `(192, 232, 48)`, `(208, 248, 48)` | `(208, 248, 48)` (51 px in digit 0); mid-shade `(152, 184, 56)` matches oracle `(152, 184, 48)` | `(248, 88, 200)` pink — rejected |
| Rank "24" fill | `(248, 56, 200)` | yellow-green (wrong) | `(248, 56, 200)` / `(248, 40, 200)` |
| Rank oval | `(176, 120, 168)` + `(8, 8, 48)` | recolour of the same indices | `(176, 120, 168)` n=417 and `(8, 8, 48)` n=395 inside the 32×32 disc |

So one page, two banks: CSA 1 draws the banner and the timer; CSA 0 draws
the pink rank digits, the `th`, and the split oval. That matches the
shipped bank table in hud-layout.md (`0x29FCA8 = {0, 1, 12, 0}`, ElementC
bank 1) but the colours were checked on the pixels, not assumed from it.

Bank-0 of this page is byte-identical in `SCOREA00/01/04`, `SCOREA06`
(`0008ca30`), `SCOREA07` (`0002a610`), `SCOREA09/13/15/17/19/20` page 0.
Those twins were hashed, not re-measured for CSA 1. The CSA 1 colour match
was measured on `SCORE.GSL` itself.

### Ruled out

- **`SCORE.GSL` page 1** (`00008350`): `FINISH` / `WRONGWAY` / `REPLAY` /
  small white `0123456789 BEST`. Yellow-green is the banner-letter fill of
  that page, not a TOTALTIME word and not the timer digit shapes.
- **`SCOREA12` page 0**: `TIME UP`, `NEW RECORD`, blue-fill / yellow-outline
  digits. No TOTALTIME. Digit colour is an outline, not the solid
  `(208, 248, 48)` fill.
- **`SCOREA02/03/11` page 0**: same glyph mask as SCORE page 0 plus a
  `521 GO!` strip. Digit row is still the pink bank-0 colour.
- **`SCOREA10` page 0**: same opaque mask, different CLUT (olive digits
  `(152, 192, 112)`, letters `(232, 248, 232)`). Colour reject.
- **`SCOREA08` page 0**, **SCOREA05 256×256 pages**, **SCORE page-1 twins
  (`SCOREA02/04/11/20` `00008350`)**: yellow pixels exist, but they are not
  the TOTALTIME word plus the 16×16 timer row. No other decoded page
  contains that word.

The banner **is** in the decoded page. It is not a missing asset.

## 2. Coordinate convention

Oracle px → UI 640×480, as specified: `UI_x = px / 2`,
`UI_y = px / (447/480) = px / 0.93125` (the brief's 0.931).
A 1:1 UI blit therefore lands at about 2 screen px per texel in X and
1 screen px per texel in Y. Dest rects below are that conversion of the
placement used in the mock (the placement whose centroid residual is
reported in §4).

hud-layout.md's RaceTimeDigits anchor `(64, 56)` is the 640×448 ElementC
clock, not this strip. This timer's first digit cell is at UI
`(65.0, 59.1)`, i.e. higher on the 480 frame than `(64, 56)` recentred by
+16. Same digit **cells**, different screen anchor.

## 3. Element table

Source rects are atlas pixels on `SCORE.GSL` unit 0 / `00000070`.
`bank` is the CLUT CSA whose colours match the oracle. Dest is UI 640×480.

### 3.1 Banner

The yellow mark is a **T inside a circle**, not the L-circle packed next
to the word. Profile of the T glyph (CSA 1, x=0..36):

- top bar y=118..122, x=5..24, w=20, colour `(208, 248, 48)`
- stem y=123..133, x=9..20, w=12
- orange foot y=134..138, x=9..20, w=12, colour `(232, 176, 16)`

Oracle T, divided by the 2× horizontal scale: top ≈19×4, stem ≈11×11,
orange ≈12×4. Same colours, same structure. The L-circle at y=78..111
(wide orange foot, no top bar) is a different badge and is **not** used.

The lavender word is **OTALTIME** (8 runs), not a T plus a separate
OTALTIME baked as one sprite. The T-circle is a second quad.

| element | bank | source u,v,w,h | notes | dest x,y,w,h |
|---|---|---|---|---|
| T-circle | 1 | **0, 110, 36, 34** | opaque bbox of the circle; yellow/orange ink y=118..138 | **20.0, 18.3, 36.0, 36.5** |
| OTALTIME | 1 | **30, 80, 130, 18** | sample used by the mock (2 px of outline around the fill). Fill itself is **31, 82, 128, 14** | **47.5, 33.3, 131.0, 17.2** |

Letter runs inside the fill (measured, y=82..95):

| letter | x | w |
|---|---|---|
| O | 31..41 | 11 |
| T | 47..59 | 13 |
| A | 64..76 | 13 |
| L | 82..94 | 13 |
| T | 98..110 | 13 |
| I | 115..121 | 7 |
| M | 126..140 | 15 |
| E | 146..158 | 13 |

There is a second copy of the same circle+word pattern under it
(`T` + `ESTTIME` at y≈112..141 / y≈101..114, and `P` + `LAPTIME`). Those
are not the driving-HUD banner.

### 3.2 Timer digits and marks

16×16 cells, v=64, u = digit×16. Ink of every digit falls inside its cell
(digit 0 yellow ink `(2, 66, 11, 12)` inside `(0, 64, 16, 16)`). This is
the row hud-layout.md calls the time-digit row; the colour that matches
the oracle is CSA 1, not the pink bank-0 dump.

Oracle shows `00'52"95` (hundredths, not tenths). Dest is given only for
glyphs actually on that frame. Pitch between the two `0` cells is 28
screen px = 14 UI, which is the shipped advance (16-wide cells, step 14).

| element | bank | source u,v,w,h | dest x,y,w,h |
|---|---|---|---|
| 0 | 1 | 0, 64, 16, 16 | 65.0, 59.1, 16.0, 17.2 |
| 0 | 1 | 0, 64, 16, 16 | 79.0, 59.1, 16.0, 17.2 |
| 5 | 1 | 80, 64, 16, 16 | 106.0, 59.1, 16.0, 17.2 |
| 2 | 1 | 32, 64, 16, 16 | 121.0, 59.1, 16.0, 17.2 |
| 9 | 1 | 144, 64, 16, 16 | 148.5, 59.1, 16.0, 17.2 |
| 5 | 1 | 80, 64, 16, 16 | 162.0, 59.1, 16.0, 17.2 |
| `'` | 1 | 160, 48, 16, 16 (ink 164, 48, 6, 7) | 94.0, 54.8, 16.0, 17.2 |
| `"` | 1 | 176, 48, 16, 16 (ink 178, 48, 10, 7) | 135.0, 54.8, 16.0, 17.2 |

Source cells for the digits **not** on this frame (same row, ink measured,
no dest invented):

| digit | source u,v,w,h | yellow-ink bbox |
|---|---|---|
| 1 | 16, 64, 16, 16 | 20, 66, 8, 12 |
| 3 | 48, 64, 16, 16 | 50, 66, 11, 12 |
| 4 | 64, 64, 16, 16 | 68, 66, 9, 12 |
| 6 | 96, 64, 16, 16 | 98, 66, 11, 12 |
| 7 | 112, 64, 16, 16 | 114, 66, 11, 12 |
| 8 | 128, 64, 16, 16 | 130, 66, 11, 12 |

A second, smaller yellow digit row sits at v=48, u = 32 + d×12, 12×16
(the "lap digits in the large display" row). It was shape-tested against
the rank-badge `24` and lost (IoU 0.22 vs 0.28 for the 16×16 row). It is
not the timer.

The lavender timer **capsule** behind the digits is not a single atlas
cell with a matching colour. Oracle interior is `(87, 109, 159)`
(n=3164, bbox screen 96,48,274×27). No CSA bank of this page has a colour
within distance 50 of that, and the horizontal pill at `(0, 208, 87, 48)`
is the wrong aspect (87×48 vs a 10:1 capsule) and the wrong colour (pink
on CSA 0, yellow-green on CSA 1). Not reported as a cell.

### 3.3 Rank

| element | bank | source u,v,w,h | dest x,y,w,h |
|---|---|---|---|
| rank oval (split disc) | 0 | **160, 105, 32, 32** | **90.0, 92.3, 32.0, 34.4** |
| pink `2` | 0 | **56, 0, 28, 30** | **43.5, 96.6, 21.0, 25.8** |
| pink `4` | 0 | **112, 0, 28, 30** | **66.5, 96.6, 21.0, 25.8** |
| `th` | 0 | **98, 31, 22, 16** (ink 100, 32, 19, 15) | **93.0, 93.4, 16.0, 17.2** |
| badge `2` (yellow) | 1 | 32, 64, 16, 16 | 104.0, 112.8, 8.0, 12.9 |
| badge `4` (yellow) | 1 | 64, 64, 16, 16 | 111.0, 112.8, 8.0, 12.9 |

Pink rank digits are the **28×30** row (u = (d%9)×28, v = (d/9)×30), not
the 16×16 row. Resized onto the oracle `2` ink box (screen 88,94,38×22),
IoU was 0.550 for 28×30, 0.378 for the v=64 16×16 `2`, 0.294 for the v=48
12×16 `2`. They are drawn smaller than a 1:1 blit (a 1:1 28×30 would be
56×30 screen px; the ink is 38×17). The dest above is the box the mock
scales them into.

Ordinal source cells (CSA 0 pink; only `th` is on this frame):

| suffix | source u,v,w,h | ink |
|---|---|---|
| st | 32, 31, 22, 16 | 32, 32, 21, 15 |
| nd | 54, 31, 22, 16 | 55, 32, 21, 15 |
| rd | 76, 31, 22, 16 | 78, 32, 19, 15 |
| th | 98, 31, 22, 16 | 100, 32, 19, 15 |

The other eight big digits, for a consumer placing a different rank
(source only, CSA 0): u = (d%9)×28, v = (d/9)×30, 28×30. Digit 9 is
`(0, 30, 28, 30)`.

## 4. Validation

`race_hud_mock.py` pastes the cells onto an 800×600 canvas at
`dest × 1.25` (the 640×480 frame scaled to 800×600) and also builds a
screenshot-space overlay against the oracle crop.

Verdict: **the composite matches letter order, digit order, and colours.**
`T` + `OTALTIME` reads TOTALTIME. Timer reads `00'52"95` in the CSA 1
yellow-green, with `'` and `"` in the gaps. Rank reads pink `24` + `th`
over the split oval, with a small yellow `24` on the dark half. It is not
a pixel-perfect overlay.

Residual of the mock relative to the oracle, in **screenshot pixels**
(mock − oracle; 2 px ≈ 1 UI x). Positive X means the mock sits right of
the oracle.

| element | residual | how |
|---|---|---|
| T-circle | **(−0.4, −0.5)** | yellow centroid. IoU of the yellow mask 0.795 at shift (0, 0) |
| OTALTIME | ends **(−1.0, −0.3)** and **(−0.9, −0.6)**; middle letters **−3 to −6 X**, Y within 1 | tight `(208,200,248)` centroids of the 8 letters. The packed cell is slightly tighter between O–T and L–T than the oracle word. A single quad cannot absorb that. |
| timer digits | X **−0.3, −0.3, +1.8, 0, −1.1, +2.6** across `0 0 5 2 9 5`; Y within 0.5 | yellow centroids. Last `5` is the worst. |
| rank oval | about **−2 X, −1 Y** vs the lavender+dark bbox (screen 182,87,64×32) | 32×32 disc placed at a 2×/1× blit |
| pink `24` | best mask shift **(+1, 0)**; shape IoU 0.65 against the oracle pink | 28×30 scaled into the ink box, not 1:1 |
| `th` | mask shift **(0, +1)**, IoU 0.41 | small glyph, outline-dominated |
| badge `24` | mask shift **(0, +1)**, IoU 0.28 | same 16×16 digits, drawn at about half size; shape beats the 12×16 row but the fit is the weakest element |

The timer capsule is absent from the paste (no matching cell), so the
digits sit on the oracle's own oval in the overlay and on flat fill in
`race_hud_mock.png`.

## 5. What was not measured

- Per-letter dest rects. The oracle word is one run; only the whole-word
  dest is given. Individual letter source runs are in §3.1.
- Dest of digits 1, 3, 4, 6, 7, 8 and of `st`/`nd`/`rd`. They are not on
  this frame. Source cells are measured; dest is not extrapolated.
- The timer capsule's caps. hud-layout.md describes an 18+middle+18 panel
  for ElementC; that construction was not confirmed on these pixels, and
  no capsule cell matched the oval colour `(87, 109, 159)`.
