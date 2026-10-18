# W2-10 report — HUD live data (speed digits, needle, timer, lap/position)

Date 2026-10-17. Batch W2-10. New files only; private build dir `build/w-W2-10`.

## Result

```
W2-10 SCORE rect_err_px=0 timer_vectors=14/14 timer_oracle=2/2 speed_oracle=2/2 elf_pins=69/69 needle_points=4 needle_independent=3 needle_resid_deg_max=DEFERRED needle_rest_resid_deg=0.84 roi_mad=8.715 roi_hist=0.8720 deterministic=1 PASS
```

Exit 0. Every §W2-10 threshold is met except `needle_resid_deg_max`, which is **not computed**. The
coordinator rule is no fit from fewer than 4 independent measured points, and RPM is unknown at all
4 frames. See "Needle" below.

## Headline finding: the oracle 1P gauge is handler 0 @0x2496C8, not FUN_0024E1A0

`UpdateRaceHud` (0x2490A0, `build/decomp/fn/002490a0.c:34-37`) calls
`(&PTR_LAB_002A8FB0)[uVar3]`, where `uVar3 = 10; if (!twoPlayer) uVar3 = modeByte`. So
`0x24E1A0` is jump-table index **10**, the 2-player layout. It uses table 0x2A9B08 → `METER10.GSL`
and 20x26 digits at v=102. The 1P gauge in the oracle frames is **handler 0** (table entry 0 =
`0x2496C8`):

* origin table `0x2A8FF0`: `(484,294)` for 1P (ELF-pinned)
* tex table `0x2A9008`: entry 0 = TBP 0x237A PSMT8 256x128 (= METER0 unit 0), entries 1/2 = TBP 0x23FE PSMT4 128x128 (= METER0 unit 2) with a CT16 CLUT (unit 3) selected by CSA
* it has no `build/decomp/fn/*.c` file. I transcribed it from the capstone MIPS disassembly of `0x2496C8..0x249FA0`, with line-by-line address cites in `hud_live.cpp`, and pinned its FNV-1a code hash in the test

Handler 0 draw order (UI coords = origin + record):

| # | what | geometry / source |
|---|---|---|
| 1 | lime arc track (rot quad, entry 1, csa `blink?3:0`) | pivot (70,70); sprite u40 v48 20x60; strip verts (46,-30)(66,-30)(46,30)(66,30); template `0x327820` |
| 2 | arc overlay (rot quad, entry 1, csa `m==0?5:(blink?4:2)`, u64 v48) | z-rotation `-(m/0x40000)*0.27778*pi` |
| 3 | baked dial ring, entry 0 | (0,0) 140x128 → abs (484,294) |
| 4 | gear glyph | (97,60) 20x14, u=`(g%5)*20+140`, v=`(g/5)*16+80` |
| 5 | speed digits (≤3, right-aligned) | x=0x59,0x48,0x37; y=0x5E; 16x23, u=`(d%7)*16+140`, v=`(d/7)*23` |
| 6 | town only (`param_7`): plate (20,100) 94x38 (`0x327890`), sun/dusk/moon icon (22,113) 24x24 u=`24*phase+140` v49, 5 clock glyphs 8x8 at x=91-9i, y=122, v=40 | phase = `GetRaceTimeRank` 0x249670; BCD = `FormatRaceTimeBCD` 0x2495F0 |
| 7 | `param_6` shift-mode icon (29,59) 16x27 u16 v48, csa 12/14/13 for mode 0/1/2 | template `0x3278B0` |
| 8 | **needle** (rot quad, entry 1 csa 12) | pivot (70,68) → abs **(554,362)**; sprite u3 v53 8x59; strip verts (-4,-59)(4,-59)(-4,0)(4,0); z-rot = `pi*(rpm/10000 - 0.7)` |
| 9 | hub cap (64,64) 12x12 u16 v80 | template `0x3278D0` |

Consequences for the coordinator:

* `main.cpp:2440-2497` (METER0 dial 132x128 at (492,336) plus METER10 20x26 digits) models the **2P**
  layout. For 1P, use `buildGaugeHud` (ring at (484,294) 140x128, 16x23 digits from METER0 at
  `u=(d%7)*16+140`, v=`(d/7)*23`). The existing 2P path is preserved as `buildDriveHud2P`.
* `gauge-calibration.md`'s digit cells (UI x 532/551/570) and hud-layout §1.2's "1P anchor (492,336)"
  come from handler 10. The 1P handler-0 digit left edges are UI 539/556/573, y 388.
* The lime curved shape right of the hub is **not** baked art. It is the boost meter: the
  rotated arc quads (rows 1-2), seen through a window in the ring.
* In all oracle frames the boost meter (`car+0x23C`, clamped 0x40000) is full (best arc match at
  m = 0x40000, MAD 23 over a window that includes outside-ring background; visually a pixel-level match
  including the cyan cap line).

Rasterized handler 0 against all three oracle gauges looks near-pixel-identical (ring, needle, hub,
gear glyph, 16x23 digits, mph label, town plate/sun/clock "09:15"). Side-by-side PNGs are in
`build/w-W2-10/work/{cmp63,cmp_town,arc_cmp}.png` (untracked).

## Speed digits and the divisor GAP

* Shipped chain: `GetSpeedFromF15Vector` (0x21D358, `|v_f15|*216.0`) → `CalculateSpeedometerDigits`
  (0x2494E8) `(int)(raw / DAT_00332F48)` → `IntToPackedBCD` (0x24F4A0). `DAT_00332F44` and
  `DAT_00332F48` are **both 1.6089999676f** (ELF-pinned bit-exact). The digit path has no 186 clamp
  (only `CalculateSpeedometerValue` clamps). The digit loop always draws ≥1 cell, so speed 0 draws
  "0", and it draws at most 3 cells (`0024e1a0.c:91-106`; same loop in handler 0 at
  `0x249A90..0x249B48`).
* **GAP stays open.** `kSpeedDivisor` is a parameter (`GaugeInput::speedDivisor`, default
  `kDefaultSpeedDivisor = 100`, alternative `kAltSpeedDivisor = 20000/186 = 107.527`).
* Formatter test `speed_oracle=2/2`: for oracle 39 and 63, the shipped chain, host/100, host/107.527 and
  the cell layout all reproduce the digits whenever the raw value sits in the digit's bin (39 needs
  host raw [3900,4000) @100 or [4193.5,4301.1) @107.527; 63 needs [6300,6400) / [6774.2,6881.7)).
* **Which divisor reproduces the oracle at the paired frames? Neither.** The native AI car at race
  timer 2446 / 3177 (the oracle frames' timer values) has host speed **18461 / 19012**, so /100 → **184 / 190**
  and /107.527 → **171 / 176**, against oracle 39 / 63 (`native_pair` INFO line, from `GameLoop`).
  The mismatch is the physics speed scale: the native car runs near the 20000 cap, while the oracle
  car was at 39 → 63 mph. The divisor can't fix that. (Native motion is also ~31 world units/frame
  at 19000 speed units — `unitsPerSpeedTick 4/20000 × 8 ticks` — versus the oracle's 158 units chord over
  731 frames from plan row 2a; the unit scale is unresolved.) This needs the W2-03 vehicle model; the
  divisor is secondary.
* `hostMph` truncates (shipped `(int)` cast). `main.cpp` uses `lround`, which differs by ≤1.

## Needle (measure-only, DEFERRED fit)

ELF-derived law (no fitting): `theta = pi * (min(rpm,10000)/10000 - 0.7)`; 0 = up, positive =
clockwise; rpm → -126° … +54° (a 180° sweep, matching the dial's 0 → 10 labels). The constants are
ELF-pinned (`0x332F50` = 10000.0, `0x332F54` = -0.7, `0x332F84` = pi). The pivot is UI (554,362). The needle
follows **engine RPM** (`short @car+0x1D0` via `CalculateTachometerValue` 0x249520), not speed.

Measured oracle needle positions (alpha-over-black PNGs; frame-diff mask vs. the greenest of the 4 frames,
component touching the hub, PCA axis; hub = LS intersection of the 3 distinct axes, residuals
0.24/0.00/0.24 px, hub (1109.25, 361.06) shot px vs ELF pivot (1108, 361.2)):

| frame | digits | mask px | shot-space angle (0=right, +90=up) | UI theta (0=up, cw+) | implied rpm (PCA) | rpm by image-match (sweep, ROI x985..1135, y295..400) |
|---|---:|---:|---:|---:|---:|---:|
| race_hud | 39 | 504 | 198.29° | -123.47° | 141 | ~20-40 |
| race_hud_driving | 63 | 586 | 117.80° | -14.77° | 6180 | ~6240 |
| town_driving | 0 | 518 | 199.40° | -125.16° | 47 | 0 |
| town_driving_street | 0 | 518 | 199.40° | -125.16° | 47 | 0 |

* The shot is anisotropic (x = 2 x UI x, y = UI y), so shot angles are converted to UI angles with
  `tan(phi_ui) = 2 tan(phi_shot)` before inverting the law (`shotAngleToNeedleThetaDeg`).
* town_driving and town_driving_street have the **identical** needle mask, so only **3**
  independent positions exist. RPM is not recorded at any of them, so the plan's
  `needle_resid_deg_max ≤ 2.0` over ≥4 points cannot be computed. Not fitting, per the coordinator rule.
* Weak check only: stationary town car ⇒ rpm 0 ⇒ -126°; measured -125.16° (**0.84°**,
  `needle_rest_resid_deg`). The rendered needle at the image-matched rpm shows the right length (59 UI px,
  tip length 65/103 shot px along the anisotropic directions), width and hub.
* Open: race_hud shows 39 mph with the needle at rest (rpm ≈ 0-140). Either a neutral/coasting/launch
  frame, or the displayed speed and rpm decouple there. To close the fit: capture ≥ 4 oracle frames
  with RPM (read `car+0x1D0`) logged at the same instant.
* `gauge-calibration.md` Needle table is superseded (needle isn't a function of mph; the "+0.36°/99 mph" point is
  not the needle, and `race_hud` reads 39).

## Timer / lap / position

* `raceTimerBcd` transcribes `raceHudElementC` 0x22F750 (`0022f750.c:49-62`). The 14 boundary vectors
  (0, 1, 59, 60, 3599, 3600, 35999, 36000, 359999, 360000, 0x7FFFFFFF, 100, 12345, 215999) are checked
  against an independent Python divmod decomposition, 14/14.
* Oracle: HUD value = total-time frames − 1: 2447 → 00'40"76 (BCD 0x4076), 3178 → 00'52"95 (0x5295), 2/2;
  the frames-0 reading matches 0/2 (it is the discriminator).
* Panel/lap/position builders (`buildRaceTimePanel`, `buildRacePosition`) are transcribed
  from `0022f750.c` / `0022fe50.c` (digit x = origin+{102,88,60,46,18,4}, y+4, 16x16, v=64; LAP badge
  24x24 at (x-26,y); lap digits 8x16 v=200; suffix k = rank%10 if <3 and not 10..19 else 3; large
  variant panel (51) + 28x30 digits (23 / 0) + 12x16 "/total" digits (71 / 61)). Rects equal the
  hud-layout §1.1 numbers exactly (`rect_err_px=0`, 114 rects). Not rasterized (SCORE atlas decode isn't
  wired here), so no oracle ROI for them. ElementA (countdown/READY/GO + type-2 fold) is **not**
  implemented (out of this pass).

## Raster / ROI

* `rt-w2-hud_raster` rasterizes `buildGaugeHud` into oracle shot space. `toShotSpace` is the shared
  resolve model: shot row sy shows UI row sy+1 (**measured**: best row offset −1, MAD 14.1 → 6.8), x = linear 2x
  upsample, then a separable Gaussian σ = 0.5 (a candidate set {0.5, 0.7, 1.0} was tried against the ROI;
  σ=0.5 gives hist 0.906 vs 0.836 without blur; mild tuning, not ELF data).
* ROI (plan: shot x 1056..1188, y 380..415): race_hud (39) **MAD 8.715 / hist 0.8720**,
  race_hud_driving (63) **5.717 / 0.9063**; town_driving_street (informational) 4.487 / 0.8505.
  Without the resolve model: nearest-neighbour MAD 14.1 / hist 0.79-0.81 (fails the hist threshold).
  Per-frame rpm/gear for the ROI renders: the needle rpm is the measured implied rpm (circular for the shaft
  only); the gear glyph and arc are outside the ROI or constant.
* Needle/ring/digit colours decode at 1:1 against the oracle (e.g. pill pixel (50,129,62) identical).

## Files (all new)

* `src/native/gameplay/hud_live.{h,cpp}` — draw lists (handler 0, handler 10, race timer/lap/position), speed/BCD/clock/needle laws, GSL gauge decoder, rasterizer, shot resolve. Auto-picked up by the staged-source list (`gameplay/hud_live.cpp`).
* `src/native/tools/w2_hud_raster_main.cpp` → `rt-w2-hud_raster`.
* `tests/native_w2/hud_live_test.cpp` → `w2_hud_live_test`; `tests/native_w2/hud_oracle_diff.py` (export ROI crops / diff a raster).
* `.qoder/wave2/W2-10-report.md`.
* Not written: `docs/native-reference/hud-live.md` (this report carries the same content).

Template numbers: the handler's memcpy'd sprite records (`0x327820/0x327890/0x3278B0/0x3278D0`) are
quoted as layout ints (x,y,u,v,w,h; verts) in `hud_live.cpp`, consistent with `hud_layout`. The test
reads those ELF words and requires they equal the builder output; no byte tables or art are committed.

## Reproduce

```
cmake -S src/native -B build/w-W2-10 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-10 --target w2_hud_live_test rt-w2-hud_raster -j
python3 tests/native_w2/hud_oracle_diff.py export          # build/w-W2-10/oracle_crops/*.ppm (alpha over black)
build/w-W2-10/w2_hud_live_test --elf "$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"; echo $?
build/w-W2-10/rt-w2-hud_raster --mph 63 --rpm 6180 --gear 4 --meter 262144 --out /tmp/r63.ppm
python3 tests/native_w2/hud_oracle_diff.py diff /tmp/r63.ppm race_hud_driving
```

Flags: `--disc DIR` (ITEM/METER0.GSL), `--oracle-crops DIR`, `--allow-skip`. Missing ELF / disc / crops ⇒
exit 2 (NO_ELF / NO_DISC / NO_ORACLE). Wall time ≈ 0.2 s. A second full pass in-process confirms the hash
(`deterministic=1`).

## Confidence

* High: handler-0 geometry and draw order (pixel-level oracle match, ELF-pinned templates and hash), timer
  formatter (14/14 + 2/2), digit/BCD logic, needle law and pivot (ELF constants; rest angle within 0.84°).
* Medium: csa semantics for the CT16 pages (CSM1-swizzled `csa*16+nibble`), which reproduce the oracle colours
  visually (lime/white/teal/orange variants) but are not asserted numerically. Alpha rule for CT16 follows
  `hud_art.cpp` (`p16 == 0` ⇒ transparent).
* Open / not done: needle quantitative fit (needs ≥ 4 RPM-known oracle frames); raw → mph divisor (the native
  physics speed is ~3-5x the oracle speed at paired frames); ElementA countdown/type-2 needle fold; SCORE-atlas
  rasterization of the timer/lap/position panels and their oracle ROI; why race_hud shows 39 mph at
  rpm ≈ 0; the semantics of the town flag byte (`param_2+0x22 == -1/0x22`) and the viewport flag (`word bit 12`)
  for the shift-mode icon; the second handler-10 quirks (`0x24E1A0` strips draw 4x60 slices, unobserved in the oracle).

## Integration notes (coordinator, `main.cpp` ~2440-2497)

Replace the hand-placed dial/digit quads with `buildGaugeHud` for 1P (fed `speedRaw` = host speed,
`speedDivisor` = chosen divisor, `rpm` = `CarState::rpm`, `meterField` = 0x40000 until a boost model exists,
`gear` = glyph index, `townMode` for EXPLORE, `clockTicks` = `snapshot.clock`). Draw the lists with the
`ITEM/METER0.GSL` pages (entry 0 PSMT8/CT32; entry 1 PSMT4/CT16, CSA bank = `csa`).
