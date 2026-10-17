# Main-menu calibration — oracle `main_menu.png` vs preview `menu-v13`

Audit of the main-menu render score. Inputs: oracle
`build/scratch/oracle-golden/main_menu.png` (1280x447 GS scanout) and our preview
`build/scratch/gamelook/menu-v13.ppm` (800x600, built by
`src/native/preview/ui_menu.cpp` `buildMainMenu`). Code was **not** modified;
every number below is measured from those two images (scripts in §7, copies in
`build/scratch/hud-art/b3_menu_*.py`; comparison art in
`build/scratch/hud-art/menu-sxs.png` / `menu-diff.png`).

Baseline score: mean |ΔRGB| = **63.4** / 255, **67.3 %** of pixels off by >48.

## 1. Coordinate mapping — root cause of the vertical misfit (F1)

Horizontal (verified, keep): `placement_x = scan_x / 2`. The oracle panel rim
measures scan x 866..1189 → placement 433.0..594.5; the code places it at
433..595 (rim re-measured in our own render at 432.8..594.4). Aligns to ±1 scan px.

Vertical: the 447-row scanout covers the **full 480-line display**, so

```
display_y (0..480) = scan_y / 0.93125        (0.93125 = 447/480)
placement_y        = display_y - 16          (kFrameYOffset, hud_layout.h:41)
```

The `ui_menu.h` header table records correct scanout pixels but converts them
with `ui_y = scan_y` (T1). That drops the ×1.0738 and is the single largest
fixable error: every vertical constant is ~3 px too high and every height/pitch
~7 % too short. Three independent proofs (all in our own scored space, i.e. our
800x600 render resized to 447 rows, factor 0.93125 = (placement+16)):

| anchor | oracle scan | ours today (placement 268..401) resized | fixed (placement 272..415) resized |
|---|---|---|---|
| panel rim top | 268 | 264.6 (−3.4) | 268.2 ✓ |
| panel rim bottom | 400 | 387.4 (−12.6) | 401.4 ✓ |
| text row 0 ink top | 279 | 274 (−5) | 279.0 ✓ |
| text row 4 ink top | 375 | 363 (−12) | 375.0 ✓ |
| copyright ink top | 421 | ~409 (−12) | 420.9 ✓ |

Measured ours text band tops (resized): 274/296/318/341/363 → pitch 22 scan =
24 placement. Oracle: 279/303/327/351/375 → pitch **24.0 scan = 25.77 UI**. The
linear drift (−5 → −12) is the pitch deficit; the constant −5 is the panel-y
deficit. Both disappear under the mapping above.

## 2. Oracle element inventory

All boxes: scanout px (1280x447), then UI640 display (`x/2`, `y/0.93125`) and
`placement` (= display, y−16) which is what `ui_menu.h` constants set.

### 2.1 Full-width backdrop bands (flat, quad-fixable)

| band | scan y | color (median) | placement y | note |
|---|---|---|---|---|
| sky | 0..~60 top edge | (43,154,216) | — | ours `kSky` exact match |
| navy band | 169..207 | **(7,13,93)** | 165..206 | full width, passes behind plate; missing in ours |
| dark seam | 206..209 | (26,22,26) | 206..209 | thin, optional |
| gold stripe | 210..227 | **(244,193,10)**, edge rows (211,166,10)/(161,126,8) | 209..228 | full width; missing in ours |
| dark shoulder | 228..240 | (87,76,50)..(134,114,75) | 229..242 | muddy, optional flat (95,88,55) |
| grass | 238..328, x 0..277 | **(2,125,42)** | x 0..139, y 239..336 | left field edge; missing in ours |
| road | 320+ center | (125,125,125) | y ≥ 247 | ours (129,128,128) ✓ close |

### 2.2 Text elements

Ink mask: min channel ≥ 195..198, max−min ≤ 25 (excludes plate cream/gold).

| element | scan box | display UI | placement | ink RGB | backdrop |
|---|---|---|---|---|---|
| `Adventure` (selected) | 912..1147 × 279..294 | x 456..573.5, y 299.6..315.7 | ink top **283.6**, x **456** | (226,225,223) | selection bar |
| `Quick Race` | 924..1169 × 303..321 | x 462..584.5, y 325.4..344.7 | ink top 309.4, x 462 | (223,223,223) | panel body (48,64,144) |
| `2 Player` | 924..1103 × 327..346 | x 462..551.5, y 351.1..371.5 | ink top 335.1, x 462 | (222,223,224) | panel body |
| `Options` | 924..1089 × 351..370 | x 462..544.5, y 376.9..397.3 | ink top 360.9, x 462 | (221,222,223) | panel body |
| `Results` | 924..1083 × 375..390 | x 462..541.5, y 402.7..418.8 | ink top 386.7, x 462 | (224,224,225) | panel body |
| `Published by Conspiracy Entertainment Inc.` | 642..1245 × 213..221 | x 321..622.5, y 228.7..237.3 | ink top **212.7**, x **321** | (237,236,234) | gold stripe + road — **not centered**, right-weighted |
| `(C) Takara Co., Ltd. 2002` | 904..1227 × 421..428 | x 452..613.5, y 452.1..459.6 | ink top **436.0**, x 452 | ~(230,230,230) | road (125,125,125) |
| `button Enter` (GAP: harness keyboard prompt, oracle-golden-frames.md:107) | label ~920..1225 × 62..80 | ink (220,114,88) | see §4.10 | (220,114,88) | box body (48,64,144) |

Font metrics (menu bitmap font, proportional — FONTE is mono, so x/y scales
must be set independently):

- cap height 16 scan = **17.2 UI** (no-descender rows 279..294, 375..390);
  descender rows 19..20 scan. FONTE cap ≈ 17.2·scaleY → **scaleY = 1.0**
  (today 0.75 renders cap 12.9 — measured on our render, 32 % short).
- advance ≈ 122.5 UI / 10 cells (`Quick Race`) = **12.25 UI** → FONTE
  **scaleX = 12.3/16 ≈ 0.77** (today 0.75, close).
- row pitch **25.77 UI** (24.0 scan, exact ×4).
- unselected inset from panel left: 462−433 = **29** (current `kTextInsetX` ✓).
- selected label is drawn **6 UI further left**: x 456, inset **23**.
- text cell top = ink top − 1.3 (FONTE ink bearing ≈ 1.27·scaleY) → cell top =
  rowY + **4.3** (today `(kRowH−18)/2` = 3 at old metrics; explicit offset needed).

### 2.3 Focus highlight (selected row 0)

Bar scan box x 914..1191, y 274..298 (25 rows = **26.8 UI tall**, one full pitch):

| part | measurement | placement |
|---|---|---|
| top edge | 1 scan px gold (232,146,11) + 1 px orange (211,93,32), x 914..1187 | y 278 |
| body | **vertical gradient**, not two flat tones: (0,110,224) top → (2,88,190) mid → (~10,60,150) bottom | y 280..304 |
| bottom edge | 1 px orange (211,93,32) + 1 px gold (232,146,11) | y 304 |
| side edges | 2 scan px orange (211,93,32) at x 918..919 and 1190..1191 | x 459 / 595.5 |
| cap block | scan x 1176..1191 (gold 240,218,70 → cream 246,242,224) | x 588..595.5, **w ≈ 8** (today 13.2) |
| cap shine | near-white band rows 283..288 (246,242,224) | y 288..293, h ≈ 6 |

Bar left edge is at scan 918 (placement 459; top line from 914 = 457): the bar
starts **24 UI right of the panel edge**, right-aligned to the panel's right rim
(595.5), width **138.5** — today it is x 435, w 158 (panels-left + 2).

Horn (megaphone left of bar): yellow ink scan 848..912 × 276..298 → placement
x **424..456** (w 32.4, h 23.6), body median (211,202,15). It **overlaps the
panel rim**; today ours sits at x 397.6..429.6 — same size, **26.4 UI too far
left, 6.8 too high** (auto-fixed half by the rowY/kRowH changes).

### 2.4 Panel, plate, chrome

- Panel rim scan 866..1189 × 268..400 → placement **(433, 271.8) 162 × 142.8**
  → constants (433, 272, 162, 143). Rim inner median (24,159,231) ≈
  `kUiPromptRimInner` (28,161,230) ✓; body (48,64,144) = `kUiPrompt` exact ✓;
  `button Enter` box body is the same (48,64,144).
- Plate cream+glow AABB scan 114..1099 × 38..219 → placement x 57..549.5
  (w 493 ✓ = current), y **24.8..219.1** (h **194.3** vs current 181, top
  25 vs current 38). Cream tone (205,196,174) ≈ `kPlate` (212,201,177) ✓.

## 3. Worst grid regions (8 × 5 cells, 160×89.4 scan px)

Mean |ΔRGB| per cell, oracle vs ours resized; dominant color = 32-level mode.

| # | cell | scan rect | UI rect (x/2, y/0.931) | MAD | oracle dom | ours dom | hypothesis |
|---|---|---|---|---|---|---|---|
| 1 | (6,1) | 960..1120 × 89..179 | 480,96..560,192 | 115.0 | (16,16,80) | (208,208,176) | oracle plate lettering "TRIP" (navy) + glow vs our flat cream plate |
| 2 | (6,2) | 960..1120 × 179..268 | 480,192..560,288 | 105.8 | (16,16,80) | (208,208,176) | navy band + stripe + "Published by" tail vs our plate extending 8 UI too low |
| 3 | (0,2) | 0..160 × 179..268 | 0,192..80,288 | 99.3 | (16,16,80) | (48,144,208) | navy band + stripe + town edge vs our flat sky |
| 4 | (7,2) | 1120..1280 × 179..268 | 560,192..640,288 | 95.3 | (16,16,80) | (48,144,208) | navy band + stripe vs flat sky |
| 5 | (5,2) | 800..960 × 179..268 | 400,192..480,288 | 94.7 | (16,16,80) | (208,208,176) | plate bottom + bands + publisher text vs cream/sky |
| 6 | (0,1) | 0..160 × 89..179 | 0,96..80,192 | 90.5 | (16,16,80) | (48,144,208) | town rooftops/trees (3D) vs flat sky; plate top edge 13 UI too low |
| 7 | (5,1) | 800..960 × 89..179 | 400,96..480,192 | 89.9 | (16,16,80) | (208,208,176) | plate letters "D T" vs flat cream |
| 8 | (4,4) | 640..800 × 358..447 | 320,384..400,480 | 75.5 | (112,112,112) | (144,144,144) | road with car shadows (oracle darker, structured) vs flat gray |
| 9 | (2,1) | 320..480 × 89..179 | 160,96..240,192 | 75.1 | (16,16,80) | (208,208,176) | plate letters "OAD" vs flat cream |
| 10 | (2,3) | 320..480 × 268..358 | 160,288..240,384 | 74.7 | (48,80,80) | (144,144,144) | road + blue car / shadows vs flat gray |

Read: (16,16,80) is the navy band + plate letter ink — five of ten worst cells
are the missing full-width bands / plate art, three are 3D scene content
(cars, town — GAP for quads), one is road shading. `menu-diff.png` also shows
every label twice (our text + oracle text at growing offset) — the §1 drift.

## 4. Proposed constants (UI640 placement space, the space `ui_menu.h` sets)

RGBA as 0..255; quads drawn in order sky → bands → road → grass → plate → panel.

### 4.1 Backdrop bands (new; expected score impact #1 — region wt 28.8 of 63.4)

| constant | value | evidence |
|---|---|---|
| `kBandNavy` | (7,13,93) rect (0, 165, 640, 41) | §2.1 |
| `kBandStripe` | (244,193,10) rect (0, 209, 640, 19) | §2.1 |
| `kBandStripeEdge` (optional) | (211,166,10) 1..2 px top/bottom of stripe | §2.1 |
| `kBandGrass` | (2,125,42) rect (0, 239, 139, 97) | §2.1 |
| `kBandShoulder` (optional) | (95,88,55) rect (0, 228, 640, 11) | §2.1 |
| `kRoad` | 129,128,128 → **(125,125,125)** | §2.1 |
| `kSky` | keep (43,154,216) | exact |

### 4.2 Panel geometry (#2 — panel region MAD 52.8, wt 4.1)

| constant | current → proposed |
|---|---|
| `kMainX` | 433 → 433 (keep) |
| `kMainY` | 268 → **272** |
| `kMainW` | 162 → 162 (keep) |
| `kMainH` | 133 → **143** |
| `kRim` | 2 → 2 (keep) |
| `kUiPrompt` / rims | keep (exact / within 4) |

### 4.3 Row pitch (#2 cont.)

| constant | current → proposed |
|---|---|
| `kRowPitch` | 24 → **25.8** |
| `kRowInsetY` | 6 → 6 (keep; bar0 lands 278 = measured 278.2) |
| `kRowH` (bar height) | 24 → **26.5** |

### 4.4 Text metrics (#2 cont. — every label lands on the oracle ink)

| constant | current → proposed |
|---|---|
| `kMenuGlyphScale` | 0.75 uniform → **scaleX 0.77, scaleY 1.0** (needs split x/y in `appendText`) |
| text y offset | `(kRowH − 18·s)/2 = 3` → **rowY + 4.3** (cell top; ink top = 283.6 + 25.8·i) |
| `kTextInsetX` | 29 → 29 unselected (keep); selected row **23** |
| `kMenuInk` / `kMenuSelectedInk` | keep white (oracle medians 221..226 incl. antialias) |
| `kSmallGlyphScale` (publisher) | 0.40 uniform → **scaleX 0.45, scaleY 0.55** |
| publisher position | centered @ y 156 → **x 321, y 212** (right-weighted, ink 321..622.5 × 212.7..221.3) |
| publisher ink | keep white (oracle 237,236,234; ours currently renders dark-edged mush at 0.40 — larger cells fix the stroke width) |
| copyright | x 452 keep, y 422 → **436**, scale (0.40, 0.40) → **(0.40, 0.44)** |

### 4.5 Selection bar (#2 cont.)

| part | current → proposed |
|---|---|
| bar x | `kMainX + kRim` = 435 → **457** (`kMainX + 24`) |
| bar w | `kMainW − 2·kRim` = 158 → **138.5** (right edge = panel right rim 595.5) |
| frame | full orange rect `kMenuSelectionFrame` → 1 px **gold (232,146,11)** + 1 px **orange (211,93,32)** top and bottom lines; orange side edges 1 UI |
| body | two-tone (0,110,224)/(0,102,224) at 50 % → **gradient 3 bands**: (0,110,224) 0..30 %, (2,88,190) 30..65 %, (10,60,150) 65..100 % |
| cap | `h·0.55` = 14.6 → **8 UI** wide at right end, gold (232,146,11) |
| shine | keep `kUiCapShine`, band y +2, h 0.3 → **h 6, mid-bar** (246,242,224) |

### 4.6 Horn (#8 by weight but 2-line fix)

| constant | current → proposed |
|---|---|
| `addHorn` tipX | `barX` (435) → **461** (yellow ink spans 424..456, touches new bar edge) |
| centerY | rowY + kRowH/2 = 286 → **291.3** (falls out of 4.3/4.5) |
| size/colors | keep (ink 32×23.2 vs oracle 32.4×23.6 ✓) |

### 4.7 Plate strip (#9 — cheap strip inside the big GAP region)

| constant | current → proposed |
|---|---|
| `plateY` | 38 → **25** |
| `plateH` | 181 → **194** |
| `plateX`/`plateW`, border, colors | keep (AABB x/w exact; cream tone within 7) |

### 4.8 Copyright — see 4.4 (y 422 → 436; wt 0.29).

### 4.9 Road tone — see 4.1 (`kRoad` 125,125,125).

### 4.10 `button Enter` box (optional; documented GAP but 2.9 % of frame, MAD 83)

| part | proposed |
|---|---|
| box rect | placement (423, 37.7) 193.5 × 39.7 |
| body | (48,64,144) (same `addPanel` body) |
| rim | (17,159,232) + 1 px outer (158,213,230) |
| label ink | (220,114,88), text cell top ≈ y 54, scaleX 0.45 / scaleY 0.5 |
| icon stand-in | dark rect ≈ (432, 47) 21 × 24 (30,30,40) — GAP: icon art |

## 5. Fix order (expected score impact, from region MAD × area weight)

| rank | fix (§) | region wt of overall MAD 63.4 | note |
|---|---|---|---|
| 1 | backdrop bands + grass (4.1) | 28.8 | pure quad adds, exact colors measured |
| 2 | panel y/h + row pitch + text metrics + bar geometry (4.2–4.5) | 4.1 | also fixes title/options text paths |
| 3 | `button Enter` box (4.10) | 2.4 | GAP feature; box = one `addPanel` call |
| 4 | publisher line position/scale (4.4) | 1.6 | |
| 5 | copyright y (4.8) | 0.3 | |
| 6 | horn shift (4.6) | 0.3 | |
| 7 | plate strip (4.7) | (region 27.3, mostly un-fixable plate art) | |
| 8 | road tone (4.9) | <0.2 | |

Remaining (not constant-fixable): plate lettering/glow art, 3D cars, town
backdrop, road perspective shading — GAP items, marked in `ui_menu.h:39-40`.
The same ×1.0738 remap must be applied to `buildOptionsScreen` (`kOptY` 168,
`kOptH` 109) by measuring `options.png` with §1's mapping — out of scope here.

## 6. Verification plan

After applying: re-render menu at 800x600, resize to 1280x447, re-run §7
scripts. Acceptance: panel rim box within ±2 scan px of 866..1189 × 268..400;
text band tops within ±2 scan px of 279/303/327/351/375; overall MAD ≤ ~45
(bands+panel fixed; scene GAP floors the score); no double-ghosting of labels
in the diff image.

## 7. Python used

Written to `build/scratch/hud-art/b3_menu_*.py`; consolidated below.

```python
#!/usr/bin/env python3
"""B3 menu audit: oracle main_menu.png (1280x447) vs menu-v13.ppm (800x600).
Mapping: uiX = scan/2; display_y = scan/0.93125; placement_y = display_y - 16."""
import numpy as np
from PIL import Image

ROOT = "/Users/holzr/github/RoadTripAdventure-decomp"
o = np.asarray(Image.open(f"{ROOT}/build/scratch/oracle-golden/main_menu.png").convert("RGB"), dtype=np.int16)
r_img = Image.open(f"{ROOT}/build/scratch/gamelook/menu-v13.ppm").convert("RGB")
r = np.asarray(r_img.resize((1280, 447), Image.LANCZOS), dtype=np.int16)   # scored space
rn = np.asarray(r_img, dtype=np.int16)                                     # native 800x600
X = np.arange(1280)[None, :]; Y = np.arange(447)[:, None]

# --- 1. side-by-side + diff-amplified (menu-sxs.png / menu-diff.png) ---
H = 447
sxs = Image.new("RGB", (1280, H * 2 + 6), (255, 0, 255))
sxs.paste(Image.open(f"{ROOT}/build/scratch/oracle-golden/main_menu.png").convert("RGB"), (0, 0))
sxs.paste(r_img.resize((1280, H), Image.LANCZOS), (0, H + 6))
sxs.save(f"{ROOT}/build/scratch/hud-art/menu-sxs.png")
d = np.abs(o - r).max(axis=2)
Image.fromarray(np.clip(d * 3, 0, 255).astype(np.uint8)).convert("RGB").save(f"{ROOT}/build/scratch/hud-art/menu-diff.png")

def near(img, color, tol): return (np.abs(img - np.array(color, dtype=np.int16)).max(axis=2) <= tol)
def med(img, mask): return None if mask.sum() == 0 else tuple(int(v) for v in np.median(img[mask], axis=0))
def place(s): return s / 0.93125 - 16.0
def whiteink(img, thr, sat):
    mx, mn = img.max(axis=2), img.min(axis=2)
    return (mn >= thr) & ((mx - mn) <= sat)

# --- 2. 8x5 grid, worst 10 (dominant = 32-level quantized mode) ---
def dominant(a):
    q = (a.astype(np.int64).reshape(-1, 3) // 32) * 32 + 16
    keys = q[:, 0] * 65536 + q[:, 1] * 256 + q[:, 2]
    v, c = np.unique(keys, return_counts=True)
    k = int(v[c.argmax()])
    return (k >> 16 & 255, k >> 8 & 255, k & 255)
cells = []
for gy in range(5):
    y0, y1 = round(gy * 447 / 5), round((gy + 1) * 447 / 5)
    for gx in range(8):
        x0, x1 = gx * 160, (gx + 1) * 160
        cells.append((float(np.abs(o[y0:y1, x0:x1] - r[y0:y1, x0:x1]).mean()), gx, gy,
                      dominant(o[y0:y1, x0:x1]), dominant(r[y0:y1, x0:x1])))
cells.sort(reverse=True)

# --- 3. element measurements (masks as in §2; full prints in b3_menu_measure*.py) ---
rim = (near(o, (26, 162, 231), 12) | near(o, (145, 210, 232), 14)) & (X >= 800) & (Y >= 230)
ys, xs = np.nonzero(rim)                      # -> panel rim scan box 866..1189 x 268..400
bar = near(o, (0, 108, 224), 30) & (X > 905) & (Y > 265) & (Y < 310)
col = o[272:302, 940]                         # bar column dump: gold/orange/blue-gradient/orange/gold rows
ink = whiteink(o, 198, 25) & (X >= 900) & (X <= 1160)   # per-row label ink boxes + pitch 24 scan
pub = whiteink(o, 195, 18) & (Y >= 205) & (Y <= 232) & (X >= 620)  # publisher 642..1245 x 213..221
cpr = whiteink(o, 180, 20) & (Y >= 410) & (Y <= 435) & (X >= 880)  # copyright rows 421..428
cap = o[278:296, 1164:1184]                   # blue-vs-gold per column -> cap left edge scan 1176
grass = (o[:, :, 1] > 90) & (o[:, :, 0] < 60) & (o[:, :, 2] < 90)  # rows 238..328, cols 0..277
ours_rim = (near(rn, (26, 162, 231), 12) | near(rn, (145, 210, 232), 14))
ys2, xs2 = np.nonzero(ours_rim & (np.arange(800)[None, :] >= 500) & (np.arange(600)[:, None] >= 345))
# placement check: xs2/1.25, ys2/1.25-16 == constants (433..595, 268..400) -- pipeline verified
```

(`b3_menu_impact.py` computes the per-region MAD weights in §5: region mean
|ΔRGB| × area share; `b3_menu_measure2..6.py` are the literal dumps quoted in §2.)
