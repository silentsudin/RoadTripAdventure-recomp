# HANDOFF W4-06: race HUD element layout

Branch `re/native-complete`, worker W4-06. No git operations were run.

## Summary

The race HUD layout is resolved for the 1P Quick Race. Each element has a ROM record,
a pixel box measured on both race goldens, and a test.

- All of these are ROM-confirmed: the TOTALTIME banner and timer, the LAPTIME banner,
  timer and lap disc, the rank badge, the gauge dial face, the baked mph badge and gear
  plate, and the gear glyph.
- Predicted boxes come from the ROM record cropped to inked texels. They match the PNG
  measurements within 2 design px on every edge, and most edges match within 1 px.
- The timer digits come from the SCORE.GSL page, not FONTE and not METER0. The measured
  pitch is 14 design px; FONTE advances digits by 12 and METER0 by 17.
- The capsule behind each timer is the CSA 1 texel `(32,24,64)` drawn at alpha 0.509.
  It is not an opaque `(87,109,159)`.
- Speedometer needle: **GAP, the needle law is unresolved.** Only the observed
  geometry is recorded. Nothing in this module draws a needle.

## Files (mine)

| file | status |
|---|---|
| `src/native/gameplay/race_hud_layout.h` | new. `namespace roadtrip::gameplay`, header-only constexpr data. Every constant carries its citation. |
| `src/native/preview/race_hud.h` | extended. The whole old API is unchanged; new builders are appended (see below). |
| `tests/native_w4/race_hud_layout_test.cpp` | new. Target `w4_race_hud_layout_test`. |
| `tests/native_w4/data/race_hud_fixture.json` | new. Authored numbers only: boxes, windows, colour classes, RGB samples, readings. |
| `tests/native_w4/CMakeLists.txt` | new. Globs `*_test.cpp` into `w4_<stem>`. zlib is optional. |
| `src/native/CMakeLists.txt` | **one line**, inside the `RT_NATIVE_ASSETS_BUILD_TESTS` block next to the native_w8 line: `add_subdirectory(.../tests/native_w4 ...)  # W4-06 race_hud_layout_test` |

Nothing under `build/` is referenced from tracked code. Scratch work is in
`build/scratch/w4-06/` (scripts, crops, composites; all gitignored).

## Coordinate law and tolerance

- `design_x = png_x / 2` and `design_y = png_y * 448 / 447`, the verified law. The
  helpers are `raceHudDesignX` and `raceHudDesignY`.
- `race_hud.png` matches the ROM rows exactly for static parts. For example the banner
  starts at y 20, the panel at 52, the dial at 294 and the badge at 382, as the ROM says.
- `race_hud_driving.png` sits one PNG row higher for every static element: banner 19,
  panel 51, dial 293, badge 381. I read this as field phase, because the goldens are
  captured with `RT_PROGRESSIVE_FIELDS=0`.
- Linear filtering (TEX1 0x60) shrinks one-texel fills by about 1 px.
- The tolerance is therefore `kRaceHudTolerancePx = 2.0`.

## Element constants and grounding

**Notation.** All boxes are design px, half-open `[x0,x1) x [y0,y1)`. "Ink" means the ROM
sprite rect cropped to the inked texels of the decoded page. "PNG" means the measured
box on `race_hud_driving.png` / `race_hud.png`, as x0,x1,y0,y1. The ELF addresses are
re-read by the test whenever the ROM is present.

| element | ROM record / ELF | ink or cell box | PNG (drv / start) | presence |
|---|---|---|---|---|
| TOTALTIME banner | group `{1,20,20,0x29F628,2}` @0x2EC1A8. InitHudConfig 0x233F10 copies it to +0x8078. raceHudDraw 0x2338C8..0x23390C draws it at (dx,dy), not centred, with tex entry 0, csa 1. recs 0x29F628 `{0,0,u0,v112,32x30}` + 0x29F648 `{32,12,u32,v81,130x18}` | 20,20,182,50 | 40,364,19,50 / 40,364,20,50 | race |
| T medallion fill | texels rel (5..25, 6..27) | 25,26,45,47 | 52,88,26,45 / 52,88,26,46 | race |
| OTALTIME fill | medallion texels u29..31 are lavender; word rec 0x29F648 | 49,33,179,47 | 98,356,33,45 / 98,356,32,46 | race |
| TOTAL panel | ElementC 0x22F750 at (60,52) (`addiu a2,zero,0x3c` @0x233F18, `addiu a3,zero,0x34` @0x233F1C). Caps 0x2EBD40/60 18x24, middle 0x2EBD80 8x24 x12. csa 1, translucent | 60,52,185,76 | 120,370,51,75 / 120,370,52,76 | race |
| TOTAL digits | slots x+{4,18,46,60,88,102}, y+4. 16x16 cells, u=d*16, v=64 (template 0x2EBDE0, v patched at 0x22FB6C). Loop 0x22FB40..0x22FBAC. Separators 0x2EBDA0 (32,4,u160,v48) and 0x2EBDC0 (74,4,u176,v48). Bank 0x29FCA8 = {0,1,12,0} | cells 64..178 x 56..72 | 136,350,57,68 / 136,348,58,68 | race |
| LAPTIME banner | group `{0,490,20,0x29F668,2}` @0x2EC070. Enable is 0 at init. raceHudInit 0x2321B8 copies it to +0x84A0; drawn 0x2339C4..0x233A04 with csa 1. recs 0x29F668 `{0,0,u0,v80,32x32}` + 0x29F688 `{32,12,u32,v159,98x17}` | 490,21,620,50 | 980,1240,20,50 (both) | race, conditional |
| L medallion fill | rec 0x29F668 | 497,26,517,47 | 996,1032,26,45 / 996,1032,26,46 | race, conditional |
| APTIME fill | medallion u31 + rec 0x29F688 | 521,33,617,46 | 1042,1232,33,45 / 1042,1232,34,46 | race, conditional |
| LAP panel | ElementC P0 slot 0 `{0,494,52}` @0x2EC088 | 494,52,619,76 | n/a (billboard behind) / 988,1238,52,76 | race, conditional |
| LAP digits | same ElementC law at (494,52) | cells 498..612 x 56..72 | 1004,1216,57,68 / 1002,1214,58,68 | race, conditional |
| lap disc ("L 1" badge) | 0x2EBE00 `{-26,0,u232,v168,24x24}`, **tex entry 1** (SCORE.GSL unit @0x8350), csa 8 (0x22FBC4/D0), translucent | 468,52,492,76 | 936,984,51,75 / 936,984,52,76 | race, conditional |
| lap digit | 8x16, u=lap*8, v=200, entry 1, csa 7, at (-18,+5); 2 digits at -22/-14; lap 0 = flag 0x2EBE20 | 476,58,482,73 (cell) | 952,962,58,70 (both) | race, conditional |
| rank digits "24" | ElementD 0x22FE50 at (40,88) (`addiu v1,zero,0x28` @0x232388, `addiu v0,zero,0x58` @0x23238C). 28x30, u=(d%9)*28, v=(d/9)*30, ones +23, tens +0, csa 0 | 43,95,85,114 | 88,168,94,112 (both) | race, conditional |
| rank oval | 0x2EBE40 `{51,0,u160,v105,32x32}` csa 0 | 91,88,123,120 | 182,246,87,119 / 182,246,88,120 | race, conditional |
| rank suffix "th" | 22x16 at +51, u=32+22k, v=31, csa 0 (0x22FF34..0x22FFD0) | 94,91,109,101 | 190,216,90,99 / 190,216,90,100 | race, conditional |
| rank total "24" | 12x16, u=32+12d, v=48, csa 1; ones (+71,+16), tens (+61,+16) | 105,106,120,118 | 212,240,106,116 (both) | race, conditional |
| dial face | handler 0 @0x2496C8, sprite (0,0,140x128), entry 0 csa 0, 1P origin table 0x2A8FF0 = (484,294) | 484,294,624,422 | 968,1248,293,421 / 968,1248,294,422 | race + town |
| mph badge plate | baked into METER0 texels u42..138 v88..128, colour (50,129,62) | 526,382,622,422 | 1052,1240,381,421 / 1052,1240,382,422 | race + town |
| mph digits | coordinator law (not emitted here). Ones (573,388) = origin + (0x59,0x5e): `addiu s2,zero,0x59` @0x2498FC, `addiu v0,zero,0x5e` @0x249AB8; step `addiu s2,s2,-0x11` @0x249AE8 | cells 556..589 x 388..411 | 1112,1172,387,407 / 1114,1172,388,406 | race + town |
| "mph" label | baked into METER0 texels u111..133 v109..122 | 595,403,617,416 | 1190,1232,402,415 / 1190,1232,402,416 | race + town |
| gear plate (lozenge) | baked into METER0 texels u95..117 v56..76, colour (74,95,196) | 579,350,601,370 | 1158,1200,349,369 / 1158,1200,350,370 | race + town |
| gear digit | 20x14 at origin + (97,60) (`addiu v0,zero,0x61` @0x249A08, `addiu s1,zero,0x3c` @0x2498D0). u=(g%5)*20+140, v=(g/5)*16+80, entry 0 csa 0. g = car+0x1FF. Cells: R,1,2,3,4 / 5,6 | cell 581,354,601,368 | 1168,1201,353,365 / 1174,1190,354,366 | race + town |
| boost arc | two rotated 20x60 quads about (554,364), template 0x327820, Meter0Sub page | measured only | 1204,1236,328,380 (both) | race + town |

**Gear readings.** Glyph-mask IoU picks gear 4 on race_hud_driving.png (0.809) and
gear 1 on race_hud.png (0.765, the italic "1").

**Golden readings.** These are cross-checked against the ElementC law in
`hud_live::raceTimerBcd`, with the TOTAL value taken as frames−1 (hud-live.md):

| golden | TOTAL | LAP | place / total | lap |
|---|---|---|---|---|
| race_hud_driving.png | `00'52"95` (3178 frames) | `00'13"56` (814 frames) | 24 / 24 | 1 |
| race_hud.png | `00'40"76` (2447 frames) | `00'01"38` (83 frames) | 24 / 24 | 1 |

The lap frame counts are inferred, not logged. They give the same offset on both
goldens: 3177 − 814 = 2446 − 83 = 2363.

**Draw order.** raceHudDraw 0x233860 runs in this order:

1. arena begin 0x24F558.
2. item panel 0x22F338.
3. TOTALTIME group (+0x8078).
4. ElementC +0x808C.
5. ElementA (×20).
6. Elements E, F, G, H, I, J.
7. Per player, stride 0x408: LAPTIME group (+0x84A0), ElementC +0x84B4 (×3 slots), ElementD +0x84FC (×3), ElementA +0x8538.
8. submit 0x24F578.

The gauge is a separate task (UpdateRaceHud 0x2490A0, handler 0). Within handler 0 the
order is: arc quads, dial, gear glyph, mph digits, (town clock), (transmission icon),
needle, hub cap.

**Colours.** The test pins 26 exact RGB samples (`kRaceHudColorSamples`). Highlights:

| what | value | where |
|---|---|---|
| medallion yellow | (208,248,48) | drv (54,28) |
| medallion orange | (232,176,16) | drv (62,42) |
| banner navy | (8,8,48) | drv (68,20) |
| word lavender | (208,200,248) | drv (100,34) |
| timer digit fill | (201,239,48) | drv (144,58) |
| timer digit fill | (205,244,48) | start (144,58) |
| capsule | (81,105,158) | drv (238,52) — translucent |
| capsule | (76,102,158) | start (214,52) — translucent |
| lap disc | (45,85,159) | drv (960,52) — translucent |
| lap disc | (77,103,158) | start (966,58) — translucent |
| lap digit | (199,238,48) | drv (960,60) |
| rank pink | (248,40,200) | drv (94,102) |
| oval lilac | (176,120,168) | drv (226,92) |
| dial green | (38,86,29) | drv (1100,294) |
| mph badge green | (50,129,62) | drv (1080,382) |
| gear plate blue | (74,95,196) | drv (1162,352) |
| gear digit white | (247,250,249) | drv (1170,354) |
| "mph" label blue | (1,95,183) | drv (1196,407) |

**Uniform-white draw is exact.** TEX0 TFX decodes to DECAL for hudTexTable 0x29F5F8
entries 0 and 1 and for gauge table 0x2A9008 entry 0.

### Which atlas the timer digits use

| measured on the goldens | timer digits | FONTE | METER0 |
|---|---|---|---|
| digit pitch | 28 png px = 14 design (fill left edges 136→164, LAP 1004→1032) | 12 (0x29A880) | 17 |
| fill height | 10 rows (y 58..68) inside a 16-row cell | 24-px cells | 23-px cells |
| fill colour | (208,248,48) | — | — |

The fill colour is SCORE CSA 1, so the digits are the **SYS/SCORE.GSL unit @0x70 v=64
row** (hud-layout.md §2, race-hud-atlas.md §1). Per-cell glyph classification by IoU
against the CSA 1 page reads `00'52"95` and `00'13"56` on the driving golden and
`00'40"76` on the start golden. On the start golden's LAP cells the hundredths are low
contrast. The reading `01"38` is fixed by the constant 2363-frame offset; `01"48` would
give 2357.

### Translucent capsule

- The CSA 1 texel at SCORE (0,176) is `(32,24,64)`, from `build/scratch/hud-art/score-banks/palette-variants/711681979ef6457b_256x256_psm14.png`.
- A least-squares fit of `C = a*S + (1-a)*B` used 1803 capsule pixel-channels on both goldens. The sky B was interpolated from png rows 49 and 78.
- The fit gives a = 0.509, S = (29.3, 25.0, 67.7), rms 1.64 levels. That fits ALPHA_1 0x44 with As = 0x40.
- The lap disc (entry 1, csa 8) shows the same blend.
- The native decoder returns these texels opaque, so `raceHudSpriteQuad` puts `kRaceHudCapsuleAlpha` in the vertex alpha. `preview_hud.frag` multiplies `vColor.a * tex.a`, so that applies the blend.

## Needle (GAP: needle law unresolved)

The flag is `kRaceHudNeedleLawResolved = false`. There is no mph→angle formula, and the
module never emits a Meter0Sub sprite. The test checks both.

**ROM geometry (handler 0).**
- Rotated quad about origin + (70,68) = **(554,362)**: `addiu v0,zero,0x46` @0x249E54, `addiu v1,zero,0x44` @0x249E24.
- Sprite u3 v53 8x59 (0x249E40/44/48, 0x249E30) on gauge entry 1, csa 12.
- Strip vertices (−4,−59),(4,−59),(−4,0),(4,0), so the strip is 59 px long.
- Hub cap: 12x12 u16 v80 at (548,358).

**Measured.**
- The two race goldens were differenced, grey texels kept, then PCA in design space. Angles are 0 = right, + = counter-clockwise.
- The two axes intersect at **(554.67, 362.58)** and miss the ROM pivot by 0.51 / 0.86 px.

| golden | mph | tip angle | visible tip length |
|---|---|---|---|
| race_hud_driving.png | 63 | **+104.09°** | 52.38 px |
| race_hud.png (no throttle) | 39 | **−146.11°** | 49.09 px |

**Dial art.** The outer white ticks are static between the goldens. Measured from (554,362):

| numeral | 0 | 2 | 3 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|
| tick angle | −142.9° | −178.4° | 163.5° | 127.0° | 108.6° | 90.0° | 71.6° | 53.3° |

- The step is about 18.2° per numeral, clockwise. Numerals 0..10 span about 180°.
- The needle on race_hud.png rests at the 0 tick while the display reads 39 mph. **The needle does not track the mph display.**
- This fits the dial being a 0..10 tachometer, and fits the ELF rotation constants in hud_live.h. That is still not a ruled law, so nothing was wired.

**Discrepancy, for the gauge-calibration.md owner.**
- That doc gives race_hud as +0.36° with component bbox (1166,353)-(1202,366) in shot px.
- That box is the gear plate / arc border, not the needle.
- On race_hud.png the needle points to the lower left, toward "0".

The town goldens' needle sits near rest and overlaps race_hud's needle, so my difference
mask cannot separate it. See `hud_live::kNeedleOraclePoints` for those points.

## Integration notes for main.cpp (coordinator)

All new API is in `rt::native::preview` (race_hud.h):

```cpp
using namespace rt::native::preview;
RaceHudLayoutState hs = raceHudLayoutStateFor(snapshot, lapFrames, place0, totalCars, gear);
//  or fill it directly: totalTime = hud_live::raceHudTimerValue(totalFrames), lapTime, lap (1-based), rank (0-based), totalCars, gear
const RaceHudViewport vp = raceHudStretchViewport(W, H);   // same sx=W/640, sy=H/448 as the current race passes
// pass by texture page; upload each page once, render, compositeOver:
if (hudArt.raceAtlas)  render(*hudArt.raceAtlas,  raceHudLayoutQuads(hs, RaceHudPass::ScoreCsa0, vp)); // rank digits/suffix/oval
if (hudArt.raceBanner) render(*hudArt.raceBanner, raceHudLayoutQuads(hs, RaceHudPass::ScoreCsa1, vp)); // banners, capsules, timers, rank total
// SCORE.GSL unit @0x8350 is not decoded yet -> stand-ins:
renderFlat(raceHudLapDiscFallback(hs, vp));                                                         // translucent disc
if (hudArt.raceBanner) render(*hudArt.raceBanner, raceHudLapDigitFallbackQuads(hs, vp));             // AUTHORED lap digit
if (hudArt.speedDial)  render(*hudArt.speedDial,  raceHudLayoutQuads(hs, RaceHudPass::Meter0, vp));  // dial face + gear glyph
// then the existing mph digit pass (unchanged law) on top of the dial
```

What changes against the current main.cpp race block (main.cpp:2485-2614):

1. **Dial face.** main.cpp blits METER0 `(0,0,132,128)` at (492,336). The 1P handler draws `(0,0,140,128)` at **(484,294)**: origin table 0x2A8FF0, golden dial box 484..624 x 294..422. (492,336) is the 2P handler-10 anchor (0x2A9AF0). The mph digits at (573,388) already assume the (484,294) origin. The Meter0 pass replaces the dial blit.
2. **TOTALTIME and timer.** Replace the 480-line dest rects (18.3, 33.3, 59.1, widths 14.5 / 36.5) with the ROM records. These are now emitted in the ScoreCsa1 pass:
   - medallion (0,112,32,30) at (20,20)
   - word (32,81,130,18) at (52,32)
   - digits 16x16 at y 56, x 64/78/106/120/148/162
   - separators at (92,56) and (134,56)
3. **Timer capsule.** The hand-drawn opaque `(87,109,159)` capsule (main.cpp:2517-2534) should go. The panel caps and middle tiles are in the CSA 1 pass with alpha 0.509. If the flat path is kept, use `(32,24,64)` at alpha 0.509 over 60..185 x 52..76.
4. **New parts.** These were not drawn before: the LAPTIME block, the lap disc and number, the rank badge, and the gear glyph.
   - Gear needs car+0x1FF; the native gear source is still to be wired.
   - Place needs the race position. `FrameSnapshot::rank` is the time rank, not the place.
   - Lap time needs a per-lap frame counter.
5. **Unchanged.** Keep the mph digit pass exactly as it is. Do not add a needle (GAP).
6. **Placeholder bars.** `raceHudQuads` and the FONTE text helpers are untouched for the `!hudArt.raceBanner` path.

**race_hud.h API.**
- Preserved, byte-identical: `raceHudFraction`, `raceHudQuads`, `fonteTextQuads`, `appendFonteText`, `raceHudTextQuads`, `ScoreMeterPage`, `scorePageTextQuads`, `meterPageTextQuads`, `scoreMeterPageTextQuads`, `townClockTextQuads`.
- New: `RaceHudLayoutState`, `raceHudLayoutStateFor`, `RaceHudPass`, `raceHudPassFor`, `raceHudLayoutSprites`, `RaceHudViewport`, `raceHudStretchViewport`, `raceHudSpriteQuad`, `raceHudLayoutQuads`, `raceHudLapDiscFallback`, `raceHudLapDigitFallbackQuads`.
- race_hud.h now also includes `gameplay/hud_live.h` and `gameplay/race_hud_layout.h`.
- Builds verified: `rt-app` (main.cpp includer), `native_assets_hud_layout_test` (463 checks passed) and `w4_race_hud_layout_test`.

## Build and test

```sh
cd /Users/holzr/github/RoadTripAdventure-decomp
RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w406 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/native-w406 -j8
./build/native-w406/native_w4/w4_race_hud_layout_test            # from the repo root
```

**Source dir.** The brief named `-S .`, but the root `CMakeLists.txt` has no
`add_subdirectory(src/native)`: `grep -c src/native CMakeLists.txt` prints 0. The native
tests only exist in the standalone `src/native` project, and that is how build/native
and the earlier W3/W4 worker dirs are configured (`CMAKE_HOME_DIRECTORY=.../src/native`).
I used `-S src/native` with my own dir.

Results:

| run | result |
|---|---|
| all inputs | `W4-06 SCORE checks=463 failed=0 elements=23 png=pass elf=pass needle=GAP PASS` |
| `RT_ELF=/nonexistent RT_ORACLE_GOLDEN_DIR=/nonexistent` | `checks=343 failed=0 png=skip elf=skip ... PASS`, exit 0 |
| same, run from `/tmp` (fixture found through the configure-time path) | PASS, exit 0 |
| negative control: goldens swapped (scratch copies, since deleted) | `failed=65 png=fail`, exit 1 |
| negative control: ELF copy with one byte flipped at 0x2A8FF0 | `FAIL 0x2A8FF0 1P gauge origin`, exit 1 |

The test runs in three tiers:
1. **Numeric:** boxes in bounds, ink ±2 px or containment, monotonic timer slots, pitch 14/28, separators between groups, TOTAL/LAP shift 434, mph parity with `hud_live` (573,388 / −17 / 16x23 / u0 140 / 7 cols / badge 556..604 x 388..416), gauge nesting, ordinal law, needle GAP flag, builder sprite count and positions, pass split, viewport scaling.
2. **Fixture drift:** the JSON is checked against the header.
3. **Optional:**
   - PNG (100 checks, needs zlib): every element box re-measured with the same colour classes and windows, all 12 timer cells, 26 exact RGB samples, presence (town_driving has the gauge and no race HUD; attract has neither).
   - ELF (20 checks): 19 record, immediate and table pins, plus TFX.

## Corrections for doc owners (not edited; docs are owned by others)

- **hud-layout.md §1.1 "Top banner (READY/GO pair) (239,5,162,30)".** In 1P race, group +0x8078 is rewritten by 0x233F10 from 0x2EC1A8 to (20,20), drawn uncentred. It is the TOTALTIME banner. The "(320,20) centred" reading does not hold for the shipped 1P race.
- **hud-layout.md "Per-player badge (490,20|234)".** This is the LAPTIME banner (recs 0x29F668/0x29F688); position confirmed on pixels.
- **race-hud-atlas.md, rank "drawn smaller than 1:1".** In 448-line design space the 28x30 digits are 1:1 at (40,88)/(63,88). The scaling was an artifact of the /0.93125 conversion. Also, the capsule colour is CSA 1 `(32,24,64)` at 50% alpha, which explains "no bank matched (87,109,159)".
- **gauge-calibration.md needle table.** See the needle section above; the race_hud component it uses is not the needle.

## Open gaps

1. **Needle law** — unresolved, not wired (ruling).
2. **SCORE.GSL unit @0x8350** (hudTexTable entry 1) is not decoded by `HudArt`. The lap disc (csa 8) and the 8x16 lap digits (csa 7) use stand-ins (`raceHudLapDiscFallback`, `raceHudLapDigitFallbackQuads`, marked AUTHORED). Decode with CSA 7/8, then drop the stand-ins.
3. **METER0 entry 1** (gauge page TBP0 0x23FE, `ITEM/METER0.GSL` unit @0x8550) is not decoded. The needle, hub cap and boost arc cannot be drawn natively yet. The arc rotation law lives in hud_live; its input is unresolved.
4. **LAP ElementC digit bank.** The updater that sets it is untraced. The goldens show CSA 1, and the builder uses 1.
5. **Enable updaters** for the LAPTIME group, LAP ElementC and ElementD are untraced (hud-layout.md §4 OPEN). Presence is recorded as `RaceConditional`: all three show on both Quick Race goldens.
6. **Native inputs** are not yet in `FrameSnapshot`: race place, per-lap frames and gear (car+0x1FF).
7. **Field-phase row offset** (one row on race_hud_driving.png) is covered by the tolerance. It is not modelled.

## Reproduction (scratch scripts, condensed)

Full scripts: `build/scratch/w4-06/gen_fixture2.py` and `gen_final.py` (measurements),
`iou.py` (1:1 blit IoU), `measure_needle2.py`, `composite.py` / `dump_sprites.cpp`
(module output pasted over the goldens: `composite_race_hud.png`, `sxs_race_hud.png`).
The colour classes and PNG windows are stored per element in the fixture JSON.

```python
from PIL import Image; import numpy as np, json
fx = json.load(open("tests/native_w4/data/race_hud_fixture.json"))
def load(n): return np.asarray(Image.open(f"build/scratch/oracle-golden/{n}.png").convert("RGB")).astype(int)
def cls(a, name):
    r, g, b = a[..., 0], a[..., 1], a[..., 2]; navy = (r <= 40) & (g <= 40) & (b <= 80)
    return {"nonsky": ~((b >= 235) & ((g - r) >= 12)), "yellow": (g >= 200) & (r >= 150) & (b <= 110),
            "medal": ((g >= 200) & (r >= 150) & (b <= 110)) | ((r >= 200) & (g >= 140) & (g <= 200) & (b <= 60)),
            "lavender": (r >= 190) & (b >= 230) & (r > g), "pink": (r >= 200) & (g <= 130) & (b >= 150),
            "lilac_navy": ((abs(r-176) <= 24) & (abs(g-120) <= 24) & (abs(b-168) <= 24)) | navy,
            "green": (g >= 60) & (g > r + 15) & (g > b + 15),
            "badge_green": (abs(r-50) <= 10) & (abs(g-129) <= 12) & (abs(b-62) <= 10),
            "cyan": (b >= 180) & (g >= 150) & (r <= 150), "label_blue": (b >= 150) & (r <= 120) & (g <= 160) & (b > r + 60),
            "plate_blue": (b >= 170) & (r <= 120) & (g <= 140), "white": (r >= 200) & (g >= 200) & (b >= 200),
            "arc": (r >= 150) & (g >= 180) & (b <= 120)}[name]
for e in fx["elements"]:
    for n in ("race_hud_driving", "race_hud"):
        x0, x1, y0, y1 = e["window_png"]; m = cls(load(n), e["class"])[y0:y1, x0:x1]
        ys, xs = np.nonzero(m)
        print(e["id"], n, [xs.min()+x0, xs.max()+1+x0, ys.min()+y0, ys.max()+1+y0] if len(xs) else None)
```
