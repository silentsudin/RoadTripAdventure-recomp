# HUD minimap projection (W2-11)

Updated 2026-10-18. Module `src/native/gameplay/minimap.{h,cpp}` (pure, deterministic, no rendering),
test `tests/native_w2/minimap_test.cpp` (`w2_minimap_test`), golden generator
`tests/native_w2/minimap_golden_gen.py`, oracle helper `tests/native_w2/minimap_oracle_shift.py`.
Addresses are SLUS_203.98 (file offset = vaddr - 0x1FF000). Every number below was read from the
ELF this pass (struct/capstone) unless marked otherwise. No game bytes are committed: the golden file
holds derived numbers, the tests compare module constants to the ELF at run time.

Supersedes the projection paragraph of `docs/native-map.md` §1.1 and `map_view.h` (`fx = 102 + ...`):
same law, now derived from the handler instead of fitted, plus the PAL branch, the race/split
law and the blip rules.

## 1. Projection law

All handlers build a view block with `mapViewMatrix` 0x21F920 and let VU1 draw the map model (and the
blips, as objects) through it. The block fields that reach the screen: translation `[8],[9]`,
x scale `[0] = s * (1.0 | 0.8 PAL)` (`fVar4 = DAT_00332CA8`, forced to 1 when `videoMode == 0`) and
y scale `[5] = s * DAT_00332CAC` where `DAT_00332CAC = -0.47` (**negative**; `DAT_00332EFC/EF8 = +0.47`).
The 0x2A43E0 vector `(-800, -200, -800, 1)` is the model offset (the -200 is depth after `RotX(-pi/2)`
and never reaches the screen). View space origin = GS `XYOFFSET`: NTSC (1728, 1936) = 2048 - (320, 112),
PAL (1792, 1920) = 2048 - (256, 128). **field = view - XYOFFSET** (x px = UI x px, one field line = 2 UI lines).

### Town / field area (`gs_map_draw_functions_town` 0x242438, north-up)

NTSC (`videoMode == 0`, instructions 0x2425B8..0x242614 in this exact float order):

    tx = (x0 + 1728) - (px*s)/200            ty = (y0 + 1936) + ((pz*s)*0.47)/200
    field.x = tx + sx*((wx - 800)/200) - 1728     field.y = ty + sy*((wz - 800)/200) - 1936

which collapses to the closed form

    fx = x0 - 4*s + (wx - px)*s/200
    fy = y0 + 1.88*s - 0.47*s*(wz - pz)/200       (1.88 = 4 * 0.47)

PAL (0x242518..0x242574): `tx = (x0*0.8 + 1792) - ((px*s)*0.8)/200`, `ty = (y0 + 1920) + ((pz*s)*0.47)/200`,
`sx = 0.8 s`, XYOFFSET (1792, 1920): `fx = 0.8*(x0 - 4 s + (wx-px) s/200)`, `fy` as NTSC.

(`px`, `pz` = subject block 0x177ACE0 +0 / +8. The plan's "`+ pz*s/200*0.47`" is the *translation* sign only;
the model's y scale is `-0.47 s`, so on screen `fy` falls as `wz` rises: moving the car to +z scrolls the map
content down, which is the oracle's (+9.2 px) shift.)

The `200` divisor is the `lui 0x4348` literal at 0x242518 / 0x2425BC; it is what turns world units into the
model scale `s` per 200 units. It is verified by the car-fixed property below (the translation's `-px*s/200`
must cancel the model's `+px*s/200`).

**Car marker (derived, not fitted).** With wx = px, wz = pz: `fx = x0 - 4s`, `fy = y0 + 1.88 s`.
Both zoom entries give the same point, a two-parameter consistency check on the table and the offset:

| mode | {x0, y0, s} | x0 - 4s | y0 + 1.88s |
|---|---|---|---|
| 0 near | {302, 89, 50} | **102** | **183** |
| 1 far | {202, 136, 25} | **102** | **183** |

PAL: x = 0.8*102 = 81.6, y = 183 (scissor centre x = (36+126)/2 = 81). The test runs 1000 random poses
x 2 modes x NTSC/PAL: NTSC lands at (102.000, 183.000) at the GS 1/16-px resolution every time (float
residual < 5e-3).

### Zoom table `townMapZoomTable` 0x2A42C0 (2 entries, 16 B stride `{x0, y0, scale, 0}`)

| mode | vaddr | x0 | y0 | scale |
|---|---|---|---|---|
| 0 (near) | 0x2A42C0 | 302 | 89 | 50 |
| 1 (far) | 0x2A42D0 | 202 | 136 | 25 |

The mode byte (`task+0x25`) is toggled by `townMapTask` 0x24302C (`^= 1` when `task+0x30 & DAT_01780A5A`,
i.e. the Select mask). The handler indexes the table with the raw byte (no clamp); the module clamps. Marker
model scale (`objectMatrix [0]` and `[10]`) = `50 / scale` (1.0 near, 2.0 far).

### Course map (`gs_map_draw_functions_race` 0x2427D0) and split map (`..._scissored` 0x242A88)

`mapViewMatrix(entry.x + 1728 | entry.x*0.8 + 1792, entry.y, entry.scale)` (entry.y is not offset; no
subject term). The aux matrix is `RotY(entry.rot)` then `RotX(-pi/2)` applied to the unit matrix **whose
translation row is already the (-800,-200,-800) vector**, so the offset is rotated too. Result:

    a = (wx - 800, wz - 800)
    x1 =  a.x*cos(rot) + a.z*sin(rot)          z1 = -a.x*sin(rot) + a.z*cos(rot)
    fx = entry.x + sx*x1/200                    fy = (entry.y - 1936) - 0.47*s*z1/200     (NTSC)

`raceMapPlacements` 0x2A4050 is **39** entries (`{x, y, scale, rot}`, 0x2A4050..0x2A42BF; the symbol note
"24" is stale; the course byte `task+0x22` indexes it; entries 24, 27..36 are the default
`{320, 2048, 50, 0}` filler). Rotations in use are 0, +-pi/2, pi; scales 33.5, 40, 50, 70, 75, 105, 133.5.
`scissoredMapPlacements` 0x2A42E0 has 5 entries (`{442|127|-188|284|-31, 1910|2030, 105, 0}`), picked by the
split index when the course byte is 0x19. Race tasks edit the entry live with pad bits (a leftover layout
editor, `DrawMapCoordinateValues`); the module ignores that.

### Scissor windows (SCISSOR_2, GS packet qword 1 of each vifDirect; SCAX0 0..10, SCAX1 16..26, SCAY0 32..42, SCAY1 48..58, inclusive)

| packet | used by | x | y (field lines) | UI rect (x, 2y) |
|---|---|---|---|---|
| 0x2A43F0 | town, `videoMode == 0` | 45..158 | 156..207 | (45, 312, 114x104) |
| 0x2A4410 | town, PAL | 36..126 | 156..207 | |
| 0x2A4430 | split-screen / mini-game | 48..208 | 68..200 | |
| (none) | course map 0x2427D0 | - | - | unclipped: the track's own box |

(`hud-layout.md` §1.3 lists the NTSC window as 113x102: that is the exclusive-edge rect (45, 312, 113, 102)
from SCAX1/SCAY1 as exclusive coordinates; the GS rule is inclusive, so the covered pixels are 114 x 52 field
= 114 x 104 UI. The oracle fill measured by `minimap-blobs.md` SCR (x 90..319, y 311..414 scan) agrees to 1 px.)
Module rule: a continuous field point is visible iff `x0 <= x < x1+1 && y0 <= y < y1+1`.

## 2. Marker (blip) rules

Colour sets are uploaded with `vifUploadColours` (`f32 r,g,b,a`): red 0x2A4370 (1,0,0,1), teal 0x2A4380
(0,.7,.7,1), green 0x2A4390 (0,1,0,.5), yellow 0x2A43A0 (1,1,0,1); map fill sets 0x2A4330 white,
0x2A4340 grey (day/night by `task+0x14 - 0xE484 < 0x1944D`), 0x2A4350 `(.2,.2,.6,.7)` track.
Models: arrow chain 0x290870, dot chain 0x290940, object matrix scale `50/s`, translation (x, z).

**Town** (`townMapTask` 0x242FE8; drawn only if `task+0x28 & 0x10000 == 0` and `task+0x22 == 0xFF`):
1. player arrow at the subject (red), `RotY(DAT_0177AE24 * pi * 2^-15)`: always at field (102, 183);
2. 23 town drivers (`psVar6 = 0x177B058 + k*0x270`, k = 0..22): drawn iff the u16 at +0 != 0, the tile id
   `u16[0x2BE43C + 8*byte(+0xAF)]` is loaded (`tileSlotLoaded` 0x221258: `task[0x60 + 4*(id&3)] == id` and
   quadrant byte `0x3335B8[id&3] >= 0`), position = car (+0x90, +0x98 of its block) **+ quadrant origin**
   (`tileQuadrantOrigins` 0x29D080 `{x,0,z,0}`: (-800,1600) (800,1600) (-1600,0) (0,0) (1600,0) (-800,-1600));
   green half-alpha;
3. coins (yellow) only if `GetFlaggedSlotCount(0xF,0x2D,0) != 0 && (task+8 & 0x20)`: coin id i = 1..100,
   `gs_map_coin_table` 0x2A3970 `{x,y,z,tile code}`: drawn iff bit (i-1) of the 128-bit word pair
   0x177FCA8/0x177FCB0 is **set** (`townMapCoins` 0x241E90 draws when `TestFlagFca8Clear == 0`;
   `UpdateMapCoinCollection` 0x241BA8 *clears* the bit on pickup, so set = still there; this corrects
   `native-map.md`'s "not collected" wording only in how it is phrased, the behaviour is the same), tile loaded,
   origin added;
4. clock strip text if `task+0x28 & 0x1000` (not part of the projection).

**Race** (`raceMapMarkers` 0x242D20; cars are 0x270-byte blocks from 0x177ACE0, position +0 / +8 of the
block, status u16 for slot i >= 1 at 0x177B058 + (i-1)*0x270):
1. player 1 arrow (red) at slot 0, yaw `heading * pi * 2^-15` (0x177AE24);
2. if `task+10 & 2` player 2 arrow (teal, slot 1, heading 0x177B094) and rivals start at slot 2, else slot 1;
3. rivals: slots first..23 with `status != 0 && (status & 0x30) == 0` (green dot);
4. finished / special: slots 1..6 with `status & 0x30` (red dot, teal if `status & 0x40`);
5. pickups (`raceMapPickups` 0x243398): yellow dots at 16-byte entries from 0x177EB40, `{x, ?, z, ?}`.
The course map has no scissor (plan §2.11's "clipped" claim applies to the town and split windows).

## 3. Oracle validation (`build/scratch/oracle-golden`, 1280x447 scanout)

Mapping: shot x = 2 * field x; shot y = UI y * 447/448 (UI y = 2 * field y). That is the **448 mapping**.

* **Content shift** town_driving -> town_driving_street, car world delta (+12.274, +39.308) from the manifest
  `car_xyz` (506.283, 442.23) -> (518.557, 481.538), zoom mode 0 (s = 50). Road + building feature masks, sigma 1.5,
  marker disc excluded, 0.05 px refinement (C++ in the test, scipy in `minimap_oracle_shift.py shift`):
  **oracle_dx = -6.15, oracle_dy = 9.20** vs prediction (-6.137, +9.217 under 448, +8.602 under the 480 mapping).
  dx matches to 0.01 px; dy is 0.02 px from the 448 prediction and 0.60 px from the 480 one.
* **Absolute marker**: red arrow centroid vs law-predicted car marker (field (102,183) -> UI (102, 366) ->
  shot (204, 365.18)): town_driving measured (204.80, 366.36), street (202.83, 366.22): residual 1.43 / 1.56 px
  (448) vs 10.66 / 10.55 px (480; the 480 mapping puts the car at shot y 355.7). The arrow's own shape biases the
  centroid by up to ~3 px (it is a 30x40-unit triangle, heading-dependent).
* **Race law** on course 0 (`{-5, 2014, 50, -pi/2}`), Peach Raceway, manifest poses (616.684, 568.756) and
  (747.502, 479.340): predicted shot (105.62, 198.63) / (150.33, 167.96), measured red-arrow centroid
  (107.50, 200.17) / (150.85, 169.48): residual (1.88, 1.53) / (0.52, 1.52); frame-to-frame marker delta measured
  (43.35, -30.69) vs predicted (44.71, -30.67). This pins the rotation sign convention (`rot = -pi/2` maps world
  (x, z) to field (-z, x)), the rotated offset (+4 s on x, -4 s on z) and the y squash.

**mapping_verdict=448.** The 480-line formula (`y/0.93125 - 16`) is refuted for this HUD; all three votes
(shift dy, two absolute residuals) pick 448. (`hud-layout.md` §0 row 6 / `minimap-blobs.md` §5 already
suspected this; the shift test closes it.)

## 4. Constants and ELF pins (all checked by `w2_minimap_test`)

zoom table (4 x {x0,y0,s,0}); placements 39 x 4 and 5 x 4 (bitwise); scissor packets 0x2A43F0/0x2A4410/0x2A4430
decoded + header qword; `DAT_00332CAC`, `0x332EFC`, `0x332EF8` (0.47), PAL 0.8 at 0x332CA8/EF4/F14/F2C; offset
vector 0x2A43E0; `-pi/2` at 0x332F00/F18/F30, pi at 0x332F34/F38; 7 `lui` float literals in 0x242438
(200, 1728, 1936, 1792, 1920, 768) and the 9-op NTSC translation sequence; quadrant origins; four colour sets.
36 pins in total. Golden vectors: 64 hand-traced tuples (town z0/z1 NTSC+PAL, 8 course entries, 5 split entries,
8 distinct zoom scales) generated by `minimap_golden_gen.py` from the ELF, independent of the module.

## 5. Open / not derived

* **Marker pixel size.** Naively `50/s` object scale x `s/200` would make the 30x40 arrow 7.5 x 10 field px; the
  oracle arrow is about 4.8 x 6.2 field px (ratio ~0.62). The VU1 vertex path (chain 0x290870 scale handling in
  the `0x1752740` projection block: `x = s/768`, `y = s/512`) is not decoded, so the module exposes position, yaw
  and `modelScale` but no drawn radius.
* **Screen yaw of the race arrow** = `heading` composed with the course `rot`; the sign convention of
  `vu0RotMatrixY` for the object matrix was only checked through the translation row, not through the arrow tip.
* **Teal arrow** at the player in town frames (blobs doc GAP 3) is not produced by the town handler code read
  here (only the red arrow and green drivers); owner unknown.
* PAL race maps: `entry.y` is not rebased for the 16-line PAL difference (literal in the handler), so a PAL race
  map sits 16 lines lower than an NTSC one; unverified (no PAL oracle).
* `DAT_00333288` (the game's wide flag, x scale) is 0 in every configuration and is not modelled.
* The placement table is embedded as authored numeric constants (39x4 + 5x4 floats, 704 bytes, cited vaddrs)
  because a pure module needs it; the coordinator may replace it with a load from the runtime ELF
  (`w2_minimap_test` already asserts equality).
