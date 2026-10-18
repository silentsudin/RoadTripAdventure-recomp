# HUD layout — screen placements, font slots, and colour conventions

Updated 2026-10-16. Extracted from the shipped ELF (SLUS-20398) in the Ghidra
session on `/SLUS_203.98` (ELF file offset = vaddr − 0x1ff000). Authored data
module: `src/native/gameplay/hud_layout.{h,cpp}`; contract test:
`tests/native_assets/test_hud_layout.cpp` (`native_assets_hud_layout_test`,
430 checks). Complements [`native-2d.md`](../native-2d.md) (the drawing model:
§1 frame/space, §3.1 UiBatch, §3.2 arena engine) and
[`sys-gsl-packs.md`](sys-gsl-packs.md) (the atlas containers).

**Coordinate space (CONFIRMED, native-2d.md §1):** all UI is authored in a
**640 x 448** space (NTSC; `X = (x << 4) + 0x6C00`, `Y = (y << 3) + 0x7900`;
PAL scales x by 0.8 to 512). Every rect below is quoted in that space,
top-left origin. Normalized-to-640x480 values (for widescreen re-anchoring)
are `x/640` and `(y + 16)/480` — the 448-line field centred in the 480-line
frame. The `+16` centring is the **one INFERRED mapping step**; everything
else is read from builder decompilations and shipped ROM bytes.

**Element record grammar (CONFIRMED, disassembly of 0x22F750):** arena sprite
records are 32 B `{x, y, u, v, w, h, z, flags}` (word 0 = x is what the digit
loops patch, word 2 = u gets `digit << 4`, word 3 = v gets the glyph row);
`uiArenaSprites(arena, recs, n, dx, dy, dz)` adds the element origin
`(dx, dy)` to each record's `x, y`. Race-HUD records are copied verbatim from
ROM template tables at `0x2EBD00..0x2EC040` (e.g. `ldl/sdr` pairs from
`0x2EBD40`); the tables below quote those bytes.

---

## 1. Element placement table

Normalized values are `(x/640, (y+16)/480, w/640, h/480)`. "vaddr" names the
builder that emits the element (decompiled this pass) unless noted.

### 1.1 Race HUD (task `raceHudTask` 0x22DDD0; arena drawn by `raceHudDraw` 0x233860)

Atlas: `hudTexTable` **0x29F5F8**, 3 x 16 B entries `{TEX2/clamp template,
TEX0}` — decoded TEX0: entry 0 TBP0 **0x2C36** (PSMT8 256x256, CLUT CT16(S)
CBP 0x2CB6), entry 1 TBP0 **0x2CB8** (PSMT8 256x256, CBP 0x2D38), entry 2
TBP0 **0x2D3A** (PSMT8 128x128, CBP 0x2D4A); CSM1, CLD 2, TFX DECAL,
TEX1 0x60 (linear). These are the `SYS/SCORE.GSL` upload pages (DBP 0x2c36
family, sys-gsl-packs.md §2). Element states live in the 0x8CB0 object at
+0x8000..; per-player blocks stride **0x408** (P0 base +0x8414, P1 +0x881C).

| element | 640x448 rect | normalized (640x480) | confidence | evidence |
|---|---|---|---|---|
| Top banner (READY/GO pair) | (239, 5, 162, 30) | (.373, .044, .253, .063) | CONFIRMED | group record +0x8078 `{dx=320, dy=20, recs=0x29F628, n=2}` set by 0x233F10; recs 32x30 atlas(0,112) + 130x18 atlas(32,81), centred |
| Lap/time panel (1P) | (60, 52, 132, 24) | (.094, .142, .206, .050) | CONFIRMED | ElementC 0x22F750 origin (60,52) bank 1 written by 0x233F10 (+0x808C..+0x80A0); panel = 18 cap + 12x8 middle + 18 cap (templates 0x2EBD40/60/80) |
| Lap/time panels (2P defaults) | (494, 52/76/100, 132, 24) | (.772, .142/.192/.242, .206, .050) | CONFIRMED (positions) / INFERRED (mutual exclusion) | `raceHudInit` 0x2321B8 writes (0x1EE, 0x34/0x4C/0x64); flag words are 0 until an updater enables |
| Time digits MM:SS.cc | (64, 56, 114, 16) | (.100, .150, .178, .033) | CONFIRMED | six 16x16 digits at x+{4,18,46,60,88,102}, y+4, v=64 row, u=digit*16; separators 16x16 at x+32 (u=160,v=48) and x+74 (u=176,v=48); templates 0x2EBDA0/C0/E0 |
| "LAP" badge | (34, 52, 24, 24) | (.053, .142, .038, .050) | CONFIRMED | template 0x2EBE00 `{x-26, y, u=232, v=168, 24x24}` |
| Lap count | (38, 57, 16, 16) | (.059, .152, .033, .033) | CONFIRMED | 8x16 digits u=lap*8, v=200 at (x-18, y+5) (1 digit) or (x-22 & x-14, y+5) (2); lap==0 -> 32x16 flag glyph atlas(80,200) at (x-30, y+5) (template 0x2EBE20) |
| Position display, small | (54, 88, 38, 16) | (.084, .217, .059, .033) | CONFIRMED | ElementD 0x22FE50 origin (40,88): digit 16x16 u=(rank+1)*16, v=64 at +14; ordinal suffix 22x16 u=k*22+32, v=31 at +30 (k=3 default "th") |
| Position display, large | (128, 88, 83, 32) | (.200, .217, .130, .067) | CONFIRMED | ElementD origin (128,88): 32x32 panel atlas(160,105) at +51 (template 0x2EBE40); 28x30 digits u=(d%9)*28, v=(d/9)*30 at +23 / +0; 2P P1 origin (40,400) |
| Position banner (ElementJ) | (258, 20, 124, 20) | (.403, .075, .194, .042) | CONFIRMED (banner) | ElementJ 0x231F28: 124x20 atlas(0,193) at (258,20); rank digits are **rotated 40x48-cell quads** centred from (288,40) stepping -30 — quad half-extents (36,30) mean the cell is magnified; bounds not pinned |
| Progress panel (ElementG) | (32, 24, 112, 32) | (.050, .083, .175, .067) | CONFIRMED (panel) | ElementG 0x231208, fixed positions (dx=0): panel 112x32 atlas(0,216) at (32,24) (template 0x2EBF20); label 32x20 atlas(160,194) at (98,65); 16x20 digits u=d*16, v=0xC2 right-aligned from (82,65) step -15; minimap-arrow rotated quad from template 0x2EBF60 at (575,140); 3 icons 24x24 atlas(112+24i, 216) at (458, 38+36i) |
| Status icon stack (ElementH/I) | (453, 24, 163, 58) | (.708, .083, .255, .121) | CONFIRMED | ElementH 0x2318B0 / I 0x231C90: 48x48 atlas(200,104) at (562,24); 56x38 atlas(160,104) at (453,40); 22x20 atlas(216,104) at (512,48); 40x48 rank digits u=(d%6)*40, v=(d/6)*48+56 right-aligned from (522|576, 24|34) step -37 |
| Item roulette panel (ElementB) | size (168, 88), position runtime-set | — | CONFIRMED (size) / OPEN (anchor) | ElementB 0x22F338: caps 16x88 atlas(152,168)/(176,168), middle 8x88 atlas(168,168) x17; icons 40x36 atlas(112,184) pitch (40,36) from +8,+8; origin word +0x800C defaults 0, set by an untraced updater |
| "READY" overlay | (198, 115, 244, 40) | (.309, .273, .381, .083) | CONFIRMED | ElementA idx0 @0x8538: recs 0x29F768 {244x40 atlas(0,56)} centred (320,135) (0x232C18: y=135 P0 / 359 P1-bottom-half 316) |
| "GO" overlay | (277, 69, 86, 46) | (.433, .177, .134, .096) | CONFIRMED | ElementA idx1 @0x8564: recs 0x29F788 {86x46 atlas(163,96)} centred (320,92) / (320,316) |
| Start countdown bars | (215, 84, 210, 56) max | (.336, .208, .328, .117) | CONFIRMED | ElementA idx2/5/6 @0x8590/0x8614/0x8640: 210x56 atlas(0,0) csa0, 160x24 atlas(0,128) csa3, 136x48 atlas(0,152) csa10, all centred (320,112); type-3 bars fill value 0..max over 30 frames (scale 1.0/2.0) |
| TOTALTIME banner | (20, 20, 162, 30) | (.031, .075, .253, .063) | CONFIRMED | ROM group `0x002EC1A8` = `{1, 20, 20, recs=0x29F628, n=2}`: origin is **uncentred** (not a centred `(320,20)` banner) |
| Per-player LAPTIME badge | (490, 20 | 234, 130, 32) | (.766, .075 | .521, .203, .067) | CONFIRMED (position) / OPEN (enable path) | ROM group `0x002EC070` = `{0, 490, 20, recs=0x29F668, n=2}` (P0; P1 y=234): 32x32 atlas(0,80) + 98x17 atlas(32,159). The label is **LAPTIME**, not TOTALTIME. |

ElementA (0x22EE70) generic records, 20 per player @+0x8538, stride 44 B:
`{type, x, y, texEntry, csa, recs, count, value, -, max, scale}` — type 1
draws the batch **centred** on (x,y) (bounding-box centring), type 2 emits a
rotated quad (needle) whose angle is `(value+1) % 60` folded
(`>=45 -> -60`, `>=15 -> 30-value`) and scaled by `DAT_00332E34`, type 3 is a
proportional fill (`value/max`, animating +1/frame to max).

### 1.2 Drive HUD (task 0x2490A0 dispatcher, jump table 0x2A8FB0, default handler 0x24E1A0)

Per-player state blocks 0x1757BC0 stride 0x1010, arena limit 0x1000. Atlas:
tex table **0x2A9B08** — entry 0 TBP0 **0x237A** (PSMT8 256x128, CLUT CBP
0x23FA), entry 1 TBP0 0x23FE (128x32), entry 2 TBP0 0x240C (128x64) — the
`ITEM/METER*.GSL` gauge pages (DBP rebase +0x237a, sys-gsl-packs.md §2).
Viewport anchors **0x2A9AF0**: 1P **(492, 336)**, 2P (492, 112) / (492, 336).
Normalized values below are the 1P anchor.

| element | 640x448 rect (1P) | normalized | confidence | evidence |
|---|---|---|---|---|
| Gauge dial face | (492, 336, 132, 96) | (.769, .733, .206, .200) | CONFIRMED | 0x24E1A0 `{0,0,132,96}` atlas(0,0) tex entry 0 at anchor |
| Speed segments | (526, 336, 58, 60) for 10 | (.822, .733, .091, .125) | CONFIRMED (geometry) / OPEN (count) | 4x60 bars at anchor+(34+6i, 0), atlas u=0x88+6i, v=0; count from FUN_00249520 |
| Speed digits (mph) | badge (556, 388, 48, 28) = x 556..604 y 388..416 | (.869, .866, .075, .063) | CONFIRMED (coordinator ruling 2026-10-18; supersedes the earlier 20x26 / step -19 row) | 16x23 quads, RIGHT-aligned, ones at (573, 388), step -17 px/digit; `V = floor(speed_word / 55.07)` clamped 0..186; u=(140+16*(d%7))/256, v=(23*(d/7))/128 on METER0 (256x128, `hudArt.speedDial`); white; `mph` text baked into METER0; see `gauge-calibration.md` |
| Gear bar | (499, 412, 100, 18) | (.780, .892, .156, .038) | CONFIRMED | anchor+(7,76); 100x18 atlas(0,0); gear overlay = same rect, tex entry 1, **TEX2 CSA = 15 − gear**; gear 0 draws a 58x8 sliver atlas(0,24) at anchor+(30,82) |
| Prompt icon | (501, 348, 16, 16) | (.783, .758, .025, .033) | CONFIRMED (free-run/0x22 scenes) | anchor+(9,12) or (9,36); 16x16 atlas(240, 0|24) |
| Odometer digit | (502, 398, 16, 10) | (.784, .863, .025, .021) | CONFIRMED | anchor+(10,62); 16x10 atlas(u=(n+(n>0))*16, v=32) |

The other ten handlers (0x2496C8..0x24DA50, mini-game/betting HUD variants)
use their own origin tables (0x2A8FF0 (484,294), 0x2A9038 (484,291),
0x2A9060 (474,304), 0x2A9218, 0x2A9260) and the same record grammar; audited
this pass — **none of them draws money** (see §3).

### 1.3 Town map / clock (`townMapTask` 0x242FE8, bezel 0x249208)

| element | 640x448 rect | normalized | confidence | evidence |
|---|---|---|---|---|
| Minimap bezel | (30, 304, 144, 120) | (.047, .667, .225, .250) | CONFIRMED | 0x249208 (arena 0x1759BE0, tex table 0x2A8FE0 entry 0, csa 10 or 11): left cap 16x120 atlas(0,192), right cap 16x120 atlas(216,0), middle 112-wide tile of an 8px cell atlas(208,0) via `uiArenaSpritesWrap` (dz=1) |
| Map window (SCISSOR_2) | (45, 312, 113, 102) | (.070, .683, .176, .213) | CONFIRMED | shipped packets: SCAX0=45, SCAX1=158, SCAY0=156, SCAY1=207 half-lines (0x2A43D8 region; native-2d.md §2 table) — also 36..126 x 156..207 and 48..208 x 68..200 variants |
| "TIME" label | (530, 34, ~64, 24) | (.828, .104) | CONFIRMED | `uiDrawText(530, 17)` (0x203FC8 doubles y) with RAM string "TIME" @0x333930; gated by flag & 0x1000 |
| Clock "%2d:%02d" | (530, 58, ~80, 24) | (.828, .154) | CONFIRMED | format @0x2ECA58; `DAT_00335730` ticks, 9000/hour, 150/minute (0x384 / 0x96) |
| "DESTINATION" | (30, 58) | (.047, .154) | CONFIRMED | `uiDrawText(30, 29)` @0x2ECA48 |
| Location name | (30, 82) | (.047, .204) | CONFIRMED | `uiDrawText(30, 41)`, `gs_locations_table[DAT_00335734*2]` |
| "+++ Q's Rally +++" | (230, 34) | (.359, .104) | CONFIRMED | `uiDrawText(230, 17)` @0x2ECA30 |

Map content is PATH1 (VU1): player arrow model 0x290870 (colour sets
0x2A4370 red / 0x2A4380 teal), rival dot model 0x290940 (0x2A4390, 0.5
alpha), zoom `50.0 / mapZoomTable[id]` (0x2A4058 race / 0x2A42C8 town,
16 B stride).

### 1.4 Prompt text

| element | 640x448 rect | normalized | confidence | evidence |
|---|---|---|---|---|
| Hint bar panel | (418, 46, 204, 48) | (.653, .129, .319, .100) | CONFIRMED | `uiDrawHintBar` 0x203240 -> `uiDrawPanel9(430, 29, 180, 12|24, style 0)` 0x202350; panel9 y doubles: corners 12x12 at (x-12, y*2-12) |
| Hint bar text | (438, 58, ~180, 12) | (.684, .154) | CONFIRMED | `uiDrawGlyph(438, 58)` glyph walk; 5 languages via PTR tables 0x29A8E0..0x29A8EC, pad-glyph icons inline |
| Message window | (32, 328, 576, 104) | (.050, .717, .900, .217) | CONFIRMED | `uiDrawDialogueFrameA` 0x2028B0 called with y=0x158 (344) from dialogue task 0x23B168: frame x 32..608, y 344-16..344+88; corners 40x56/40x48 atlas(0|40|160|200, 256), edges 496 wide atlas(80|120, 256) |
| Next-message arrow | (304, 392, 32, 24) | (.475, .850, .050, .050) | CONFIRMED | 7th sprite atlas(88,256) at (304, y+48) |
| Drive prompt icon | see §1.2 | | CONFIRMED | 16x16 icon in the dial |

### 1.5 Money readouts

The live money counter is `DAT_0177FDB4` (working mirror of the save money
u32 at progress+0x654; same 999999 cap, save-layout.md). It is **not drawn
while driving** — audited: all 11 drive-HUD handlers, the town map band, and
the race HUD builders contain no money draw (CONFIRMED absence this pass).

| element | 640x448 rect | normalized | confidence | evidence |
|---|---|---|---|---|
| Shop money plate | (435, 26, 154, 32) | (.680, .088, .241, .067) | CONFIRMED | `DrawScoreCounter` 0x22E7C0 -> `DrawDialogTextLine(440, 26)` 0x202D10; plate slabs (x-5 w5 | x w96 | x+96 w64 | x+160 w5) at y..y+32; callers: shopScene 0x22AEB0, 0x261AC8 |
| Shop money digits | (440, 32, 96, 20) (6 digits) | (.688, .100, .150, .042) | CONFIRMED | `uiDrawNumber16` 0x201FD0 is RIGHT-aligned (x steps -16 from right edge x+96=536), drawn at y+8-2=32; cells 16x20: digit<4 -> u=d*16+0x1B8, v=200; digit>=4 -> u=d*16-0x40, v=220 |
| Bet screen money | (435, 338, 154, 32) | (.680, .738, .241, .067) | CONFIRMED | 0x22E7F0 `DrawDialogTextLine(440, 0x152)` |
| Bet after amount | (435, 386, 154, 32) | (.680, .838, .241, .067) | CONFIRMED | 0x22E7F0 `DrawDialogTextLine(440, 0x182)` (money − bet, clamped) |
| 2P dual scores | (320, 42) / (440, 74) | (.500, .121) / (.688, .188) | CONFIRMED | 0x22EDB8 (mini-game HUD) |

---

## 2. Font-slot mapping summary

Three text systems; the HUD references these cells (module:
`kHudGlyphSets`):

* **FONTE glyph atlas** (`SYS/FONTE.GSL`, live font; owned by
  `src/native/assets/glyph_atlas.h` — cross-referenced, not duplicated):
  512x384 PSMT4, 16x24 cells, 32 columns; slot i at
  `(col = i%32, row = i/32)`, ASCII c -> slot `(g/31)*32 + (g%31)`,
  g = c − 0x21. Layout metrics the HUD assumes: advance from
  `uiGlyphWidths` **0x29A880** (8..16 px per ASCII char, 20 px JP; = `DAT_0029A880`, u8 table indexed by
  ASCII 0x20..0x7F; ELF-verified values: `'A'`=15, `'a'`=14, `' '`=8, `'.'`=12, digits `0..9`=12; coordinator ruling 2026-10-18),
  12 px line pitch on the `x = col*20 + 10, y = (row*12 + 5)*2` cell grid
  (native-2d.md §3.1).
* **UiBatch font atlas** (TEX0 `0x2013780E65321A40`: TBP0 **0x1A40**, PSMT8
  512x512, CBP **0x1BC0** CT16, CSM1, 16 palette banks via TEX2 CSA;
  banks seen 0,1,2,5,7,8,0xB,0xD,0xE): money/counter digits **16x20** at
  u = d*16+0x1B8 (d 0..3, v=200) / u = d*16−0x40 (d 4..9, v=220); small
  **10x20** set at v = 0xB4 + 20*row; text glyphs are FONTE cells re-blitted
  into this atlas by the loaders.
* **Race/SCORE atlas** (hudTexTable 0x29F5F8 entry 0, TBP0 0x2C36): time
  digits **16x16** u=d*16, v=64; separators 16x16 (160,48) and (176,48);
  small position digits 16x16 (same row as time); ordinal suffix **22x16**
  u=k*22+32, v=31 (k 0..3); lap digits **8x16** u=lap*8, v=200; lap flag
  glyph 32x16 (80,200); large position digits **28x30** u=(d%9)*28,
  v=(d/9)*30; big digits **40x48** u=(d%6)*40, v=(d/6)*48+56; lap digits in
  large display **12x16** u=d*12+32, v=48; ElementG digits **16x20** u=d*16,
  v=0xC2; banners/icons: READY 244x40 (0,56), GO 86x46 (163,96), LAP badge
  24x24 (232,168), progress panel 112x32 (0,216), label 32x20 (160,194).
* **Drive/METER atlas** (0x2A9B08 entry 0, TBP0 0x237A): speed digits
  **16x23** u=(140+16*(d%7))/256, v=(23*(d/7))/128 (mph law, ruling 2026-10-18; replaces the older 20x26 / v=102 reading);
  speed segments 4x60 u=0x88+6i, v=0; odometer 16x10 u=(n+(n>0))*16, v=32;
  prompt icons 16x16 (240,0)/(240,24); gear cell = the 100x18 bar rect
  re-sampled from tex entry 1 with CSA = 15 − gear.

## 3. Colour / alpha conventions

* **Arena batches** (race HUD, drive HUD, bezel): batch header
  `gifBeginSpriteBatch` = TEST_1 0x5001B (ATE GEQUAL aref 1, ZTST GEQUAL),
  ALPHA_1 0x44 ((Cs−Cd)·As+Cd), TEX1 0x60, CLAMP REGION_CLAMP = the atlas
  cell; sprite depth `0xFFFFFF − (z + base)` — UI depth is real, layers
  order by Z. Per-draw colour via `uiArenaSetColour` (RGBAQ, MODULATE).
* **UiBatch** (text/plates/windows): first entry rewrites ALPHA_1 to
  **0x8000000044** (As = 0.5); TEXA (TA0 0x80, AEM 1, TA1 0x40) supplies the
  alpha the all-zero-alpha CLUTs lack; `uiSetTexa` 0x203990 fades text by
  driving TA0.
* **Item flash** (ElementB): 50x50 white cell (atlas 0,0) MODULATE-tinted,
  colour word `alpha << 24 | 0x4334EC` (top item row) / `0x50E800` (bottom
  row); alpha ramps +0x40/frame to 0x80 on land, −0x20/frame to 0 on use.
* **Timer dimming**: the packed MM:SS.cc BCD saturates to **0xAAAAAA**
  (underflow) / **0x995999** (overflow ≥ 360000 frames) and the palette bank
  comes from **0x29FCA8[bank]** = {0, 1, 12, 0} — bank 12 is the dimmed
  variant observed in the bank table.
* **Fades**: `uiFade` 0x2080F8 flat quad (0,0)-(640,224), ALPHA_1 0x44,
  strength in the top byte, TEST 0x3180A (ZTST ALWAYS).
* **Money-change flash**: not found — no flashing colour path is attached to
  `DAT_0177FDB4` draws (they use the static plate + palette bank 9). OPEN.

## 4. Ledger

**CONFIRMED** (decompilation + shipped ROM bytes, this pass unless noted):
- 640x448 UI space and the A+D vertex formulas (native-2d.md §1).
- Arena sprite record `{x,y,u,v,w,h,z,flags}` and the element-origin
  `(dx,dy)` add — from 0x22F750 disassembly (sp+0 patched with running x,
  sp+8 with `digit << 4`, sp+0xc with the glyph row).
- Race-HUD sprite templates 0x2EBD00..0x2EC040 (ElementC set 0x2EBD40..E20,
  ElementD panel 0x2EBE40, ElementE divider 0x2EBE80, ElementG panel/label
  0x2EBF20/F0, arrow quad 0x2EBF60 + vertex floats, ElementH/I icons
  0x2EBFE0..0x2EC040) and static tables 0x29F628/68/E8/708/768/788/A8/C8.
- hudTexTable 0x29F5F8 TEX0 decode (TBP0 0x2C36/0x2CB8/0x2D3A, PSMT8, CSM1,
  CT16(S) CLUTs, CLD 2) and the drive tex table 0x2A9B08 (0x237A/0x23FE/
  0x240C = the METER.GSL pages).
- raceHudInit 0x2321B8 element origins (ElementC (494,52/76/100), ElementD
  (40,88)/(128,88)/(128,104) P0 and (40,400) P1, badges dx=490 dy=20/234);
  0x233F10 single-player values (banner (320,20), ElementC (60,52) bank 1,
  lap −1); 0x232C18 READY/GO centres (320,135/92; 2P 359/316); updater
  origins 0x234128/1F8/378/450 (320,112 / 336 2P).
- Element builder geometry for A/B/C/D/E/F/G/H/I/J (all sprite rects above).
- Drive-HUD dispatch (0x2490A0, jump table 0x2A8FB0, 11 handlers), anchors
  0x2A9AF0, and default-handler rects (dial/segments/digits/gear/prompt/
  odometer); the mini-game handlers audited for the money draw (absent).
- Town bezel 0x249208 (frame 144x120 at (30,304), wrap middle) and the
  shipped SCISSOR_2 minimap windows (45..158 x 156..207 half-lines etc.).
- Clock: "%2d:%02d" @0x2ECA58, ticks 9000/hour / 150/minute; text band
  positions via `uiDrawText` (y doubled inside).
- Hint bar (panel9 y-doubling `(y-6)*2`), message window (0x2028B0(0x158)),
  next arrow.
- Money plate/digits: DrawDialogTextLine 0x202D10 slab layout, right-aligned
  uiDrawNumber16 0x201FD0 with the v=200/v=220 digit-row split; bet-screen
  positions; DAT_0177FDB4 = live money (cap 999999, save +0x654).

**INFERRED:**
- The +16 px centring when normalizing 448 -> 480 (mapping convention only;
  the native rects are exact).
- Element semantics (which banner says "READY" vs "GO", badge contents) from
  shape/size and call context; the art itself is not decoded here.
- 2P split arrangement (top/bottom halves) from the 0x408 stride and the
  y=112/336 anchor pair.

**OPEN / unknowns:**
- The updater that enables the item panel (+0x800C) and the per-player badge
  group (+0x84A0) — their runtime anchors are untraced.
- Which of the three ElementC default panels the updaters select per race
  mode (flag words stay 0 in the shipped init).
- Rotated-quad vertex semantics (the 96-byte record + 4x16 B vertex layout
  at 0x2EBF80) — bounds of ElementJ/F digit quads not pinned.
- The drive-HUD reverse-indicator sprite (58x8 sliver, atlas (0,24)) — h
  restored from a 4-byte store; verify against a capture.
- Speed-segment count source (FUN_00249520) and the meter page each entry
  maps to (METER0..10.GSL per-race-type gauge sets).
- Sun/moon or day/night *icon*: none found in any audited HUD path; the
  day/night state surfaces only through lighting (updateTimeOfDayLight
  0x225988) and the digital clock.
- CLUT alpha-bit semantics (sys-gsl-packs.md OPEN item) — TEXA assumed.
