# W2-11 report — Minimap top-down projection (town + race HUD map)

Date 2026-10-18. Worker W2-11. New files only; private build dir `build/w-W2-11`. No git state changes.

## Result

```
W2-11 SCORE car_px=(102,183) clip_ok=100% oracle_dx=-6.15 oracle_dy=9.20 mapping_verdict=448 deterministic=1 elf_pins=36/36 golden=64/64 zoom_levels=8 car_poses=4001/4001 PASS
```

Exit 0; two consecutive runs produce byte-identical stdout (`cmp` identical). The plan keys appear first, in
the plan's order and names; the extra keys follow.

| plan acceptance | result |
|---|---|
| car at field px (102,183) for 1000 random poses | 4001/4001 trials (1000 poses x 2 zoom x NTSC + PAL (81.6,183) + marker fn); NTSC exact at GS 1/16 px |
| clip: outside scissor clipped, inside never | 6048/6048 (2 windows x 1000 random points on the raw predicate; 2 windows x 10 poses x 2 zoom modes x 100 coins through the blip path; 7 boundary points; clip-kind flags) |
| `oracle_dx` -6.14 +-1.5 | **-6.15** (prediction -6.137; error 0.01 px) |
| `oracle_dy` 8.6..9.2 +-1.0 | **9.20** (448 prediction 9.217; 480 prediction 8.602) |
| `mapping_verdict` | **448** (3/3 votes: shift dy, two absolute marker residuals 1.4/1.6 px vs 10.7/10.6 px under 480) |
| deterministic | 1 |

## Oracle offsets (measured, not forced)

* Content shift town_driving -> town_driving_street (C++ in the test, scipy in `minimap_oracle_shift.py shift`, both
  -6.15 / +9.20): **oracle_dx = -6.15, oracle_dy = 9.20** shot px. Prediction from the ELF law with car delta
  (+12.274, +39.308): (-6.137, +9.217) under the 448 mapping, (-6.137, +8.602) under the 480 mapping. dy discriminates
  at 0.02 vs 0.60 px, the absolute marker position at 1.4 vs 10.7 px => 448.
* Marker centroid vs law-predicted position (shot px, includes arrow-shape bias): town_driving (+0.80, +1.18),
  town_driving_street (-1.17, +1.04); race_hud (+1.88, +1.53), race_hud_driving (+0.52, +1.52). Race marker frame
  delta measured (43.35, -30.69) vs predicted (44.71, -30.67).

## What was derived (see `docs/native-reference/minimap.md`)

* The law comes from the handler (0x242438 op sequence asserted from the ELF), not from a fit:
  `fx = x0 - 4s + (wx-px)s/200`, `fy = y0 + 1.88s - 0.47 s (wz-pz)/200` (NTSC). The car point (102,183) falls out
  of both zoom entries ({302,89,50}, {202,136,25}) as a consistency check; PAL gives (81.6, 183).
* **Sign correction to the plan text:** `DAT_00332CAC` is **-0.47** (view y scale `[5] = -0.47 s`); the plan's
  "+pz*s/200*0.47" is only the translation term.
* **Table corrections:** `raceMapPlacements` has **39** entries (0x2A4050..0x2A42BF, not 24);
  `townMapZoomTable` has 2 entries; `scissoredMapPlacements` 0x2A42E0 has **5** entries (not 3).
* Race/split law (new): rotate (world + (-800,-800)) by RotY(rot) (the offset row is rotated), then scale
  (s/200, -0.47 s/200); view translation `(entry.x + 1728, entry.y)`; verified on the two race oracle frames.
* Coin rule: drawn when the bit in 0x177FCA8/FCB0 is SET (`TestFlagFca8Clear == 0`; pickup clears it).
* Course map is unclipped (no scissor packet in 0x2427D0); only town (0x2A43F0 / PAL 0x2A4410) and split
  (0x2A4430) clip.

## Files touched (all new)

* `src/native/gameplay/minimap.h`, `src/native/gameplay/minimap.cpp`
* `tests/native_w2/minimap_test.cpp`
* `tests/native_w2/minimap_oracle_shift.py` (crop export + scipy shift reference)
* `tests/native_w2/minimap_golden_gen.py`, `tests/native_w2/data/minimap_golden.tsv` (64 vectors, derived numbers)
* `docs/native-reference/minimap.md` (the plan names it `minimap-projection.md`; the brief asked for `minimap.md`)
* `.qoder/wave2/W2-11-report.md`

CMake: **no edit needed.** `gameplay/minimap.cpp` is already in the Step-0 staged-source list and
`tests/native_w2/*_test.cpp` is globbed (`w2_minimap_test`). Scratch output only under `build/w-W2-11/`.

## Repro

```
cmake -S src/native -B build/w-W2-11 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build build/w-W2-11 --target w2_minimap_test -j8
python3 tests/native_w2/minimap_oracle_shift.py export          # -> build/w-W2-11/oracle/*.ppm (needs build/scratch/oracle-golden)
build/w-W2-11/w2_minimap_test > run1.txt; build/w-W2-11/w2_minimap_test > run2.txt; cmp run1.txt run2.txt
python3 tests/native_w2/minimap_oracle_shift.py shift           # scipy cross-check: oracle_dx=-6.15 oracle_dy=9.20
python3 tests/native_w2/minimap_golden_gen.py                   # regenerate the golden TSV from the ELF
build/w-W2-11/w2_minimap_test --elf /nonexistent                # prints NO_ELF, exit 0
```
Missing oracle crops: prints `NO_ORACLE` on stderr, exit 2 (never a silent pass).

## Open items

1. **Placement table embedded.** 39x4 + 5x4 floats (704 bytes of authored numeric constants with cited vaddrs) are
   in `minimap.cpp` because the module is pure; the rule-4 spirit says tables should be loaded from the ELF at
   runtime. The test asserts bitwise equality with the ELF, so the coordinator can swap in a loader or approve it.
2. **Marker drawn size** not derived: oracle arrow is about 0.62 of the `50/s x s/200` size; the VU1 scale path
   (0x1752740 block, s/768, s/512) was not decoded. The module returns position, yaw, `modelScale = 50/s` only.
3. Race arrow screen yaw (heading composed with course rot) and the `vu0RotMatrixY` sign for object matrices were not
   checked through the arrow tip (only through positions).
4. Teal second arrow in town frames: not produced by the code read here; owner unknown (blobs doc GAP 3).
5. PAL race `entry.y` not rebased (literal handler behaviour), no PAL oracle to verify. `DAT_00333288` wide flag
   (always 0) not modelled.
6. Town-car world positions come from the tile-local block + quadrant origin (0x29D080); the quadrant byte table
   0x3335B8 and scene tile ids 0x2BE43C are caller inputs (`TileSlots`, `TownCar::tileCode`), not read here.
7. Coin gating (`GetFlaggedSlotCount(0xF,0x2D,0)` and `task+8 & 0x20`) and the race pickup count are caller
   decisions; the module only implements the per-blip rules.
8. Only 4 oracle frames exist (2 town, 2 race; same course, one town); the absolute residuals include ~3 px of
   arrow-shape bias. A zoom-mode-1 (far) oracle frame would pin s = 25 directly; both modes are only ELF-pinned.
