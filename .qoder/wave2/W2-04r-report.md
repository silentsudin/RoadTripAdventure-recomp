# W2-04r3 report — screen-space terrain-Y law

```
W2-04 SCORE static_px_err_max=0.011824 static_gate=PASS terrain_y_px_written=731 driving_px_err_max=1.904815 driving_px_err_mean=0.271588 ymap_agreement_max=0.000007 deterministic=1
```

**PASS on the stated gate, with one scope change and one regression to fix (below).** The test binary exits 1 overall
because the pre-existing game-unit line (`oracle_y_err_max=0.102001`, street pose) still fails; it is kept unchanged.
The full test output (two lines) is byte-identical across two runs. Full second line:

```
W2-04 SCORE static_px_err_max=0.011824 static_gate=PASS terrain_y_px_written=731 driving_px_err_max=1.904815 driving_px_err_mean=0.271588 ymap_agreement_max=0.000007 deterministic=1 law_pins=12/12 mesh_pins=9/9 trace_rows=731 png_tire_edge_res=2.438..2.438 png_gated=0 car_y_contact_err=0.000070(n=5) PASS
```

## Read this first

1. **The brief's static poses do not exist in the tree.** `(429,48,148)`, `y=56`, `(518,121,369)` are not in
   `terrain-query.md` "Oracle poses" nor anywhere else. I captured three static, flat poses from the oracle instead
   (same boot/driving scripts as `oracle_golden_capture.sh`): town spawn, race grid (countdown), town coasted to rest.
2. **The 0.100 px gate cannot be measured against an oracle PNG.** The only sharp known-geometry landmark under the
   car is the rear-tyre bottom edge; it is one GS field row (2 shot rows), reads 378 in three poses and 377 in a
   fourth (field parity), and the drawn tyre sits 1.6–2.4 px below the predicted contact point (tyre geometry). So the gate
   is evaluated against the **live engine camera block** (`cameraBlocks` `0x177E6D0` read from the running oracle,
   world->clip matrix), which is independent of the law (law derived from ELF constants + game-unit poses). The PNG
   residual is printed (`png_tire_edge_res=2.438`, not gated) and documented. If you require a PNG gate, the
   threshold has to be re-registered at about 3 px.
3. **P0 integration item: the appended CSV column breaks `w2_vehicle_model_test`.** Its loader
   (`tests/native_w2/vehicle_model_test.cpp:75`) does `if (column.size() != 9) return {};`. With the new column it
   reports `W2-03=FAIL`; with the original CSV it passes (checked both ways, CSV restored). One-line fix, not mine to
   make: change `!= 9` to `< 9`. The new column is appended exactly as instructed (existing text is a byte-identical
   prefix of every line, CRLF preserved; checked).
4. `NO_ELF` exits **2** (0 with `--allow-skip`), as plan §1.4 and the existing test do. The brief said exit 0; I kept
   the plan's rule. The Python module prints `NO_ELF` and exits 0.

## What was derived (details and vaddrs: `terrain-query.md` §6)

* Projection (`buildCamera` `0x21F698`): `sy_gs = 2048 - 235*Y/Z`, `235 = 500*0.47` (`0x332C88`), X scale 500 (NTSC;
  `0x332C84`=0.8 is PAL). `camera-constants.md` has the X/Y labels swapped; the live matrices settle it.
* Camera: rigid in the car frame (car matrix rows = right/up/forward), preset 0 `(0,2,-7)` `@0x29CE00`, pitch `0x380`
  `@0x29CE14`, focal `@0x29CE10`. Eye error vs the live matrix 5e-5 on flat poses. The engine builds the camera from
  the previous tick's car state (measured lag 1).
* Pixel map: `ui448 = 2*(sy_gs-1936)` (XYOFFSET y, `lui 0x44f2 @0x2425E4`), `shot = ui448*447/448` (`minimap.md` §3).
* Car height law found on the way (`ResolveGroundContact` `0x21B3D0`): `car.y = (h0*0x21 + (h1+h2)/2*0x22)/0x43` over
  front/rear wheel contacts (`0x29CA80..`). It explains the 30.098 vs 30.200 street "miss": contacts read 30.2/30.0/30.0
  → 30.0985 vs harness 30.0984 (`car_y_contact_err=7.0e-5`). That is a unit-scale-free, load-offset-free reason.

## Static-pose validation (|err| ≤ 0.100 px, 448-space; origin + both rear contacts)

| pose | law origin / engine | law rear / engine | max abs err |
|---|---|---|---:|
| town spawn (506.283, 30.000, 442.230) | 358.2838 / 358.2913 | 376.4023 / 376.4104 | 0.0082 |
| race grid (457.200, 1.100, 568.800) | 358.2836 / 358.2944 | 376.4020 / 376.4138 | 0.0118 |
| town settled (504.297, 30.000, 450.594) | 358.2838 / 358.2852 | 376.4023 / 376.4036 | 0.0014 |

PNG (informational): rear-tyre edge observed row 378.0 ×3 vs predicted shot 375.562 → +2.438 px.

## Driving (informational, not thresholded)

16 poses × 3 ground points (2 tilted static dumps, 10 race + 4 town consecutive-vblank samples, gas held):
max 1.905 px, mean 0.272 px (tilted statics within 0.14). The residual is the camera's slewed tilt (not modelled).
The old "4 px at frame 2446" gap is the one-tick camera lag: pairing camera and car floats of the same vblank gives ≈12 px
on race (sampling artefact), so use the lag-1 pairing.

## Deliverables

| # | item | status |
|---|---|---|
| 1 | law + citations | `terrain-query.md` §6, `scripts/w2_terrain_ymap.py` |
| 2 | test amendment | `tests/native_w2/terrain_query_test.cpp` (`namespace screen`; old game-unit checks untouched) |
| 3 | `terrain_y_px` column | 731/731 rows, range 357.40..362.07; existing bytes identical |
| 4 | `scripts/w2_terrain_ymap.py` | `project(y_game, camera)`, `YMAP_ENTRY`; self-test 24 rows vs CSV 0.000000, C++ vs CSV 7e-6 |
| 5 | doc §6 | done |
| 6 | this report | done |

`YMAP_ENTRY = {'status': 'ELEMENT', 'scale': 447/448, 'offset': 0.0, 'citation': ...}`. I did **not** claim `FIT`:
the registry demands ≥3 landmark pairs at ≤1 px and the PNG edges carry the 1.6–2.4 px tyre offset plus 2-row
quantisation. Fold it into `YMAP_REGISTRY['ground']` as ELEMENT unless you accept a different residual rule.

## Files touched

* `tests/native_w2/terrain_query_test.cpp` (amended)
* `tests/native_w2/data/vehicle_trace.csv` (column appended)
* `scripts/w2_terrain_ymap.py` (new)
* `docs/native-reference/terrain-query.md` (§6 appended)
* `.qoder/wave2/W2-04r-report.md`

Not touched: `src/native/CMakeLists.txt` (the target already existed; no staged-source line needed), `scripts/w2_score.py`.
Scratch (untracked): `build/w-W2-04r3/` (capture script `capture_cam.py`, oracle dumps, PNGs).

## Repro

```
cmake -S src/native -B build/w-W2-04r3 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON && cmake --build build/w-W2-04r3 -j8
build/w-W2-04r3/w2_terrain_query_test > a.txt; build/w-W2-04r3/w2_terrain_query_test > b.txt; cmp a.txt b.txt
python3 scripts/w2_terrain_ymap.py --verify-elf
python3 scripts/w2_terrain_ymap.py --write-csv      # idempotent
python3 scripts/w2_terrain_ymap.py --selftest
# oracle captures (needs build/macos-release app, takes /private/tmp/rt-gpu.lock):
build/regress-venv/bin/python build/w-W2-04r3/capture_cam.py town 12   # town_driving/flat1/flat2 + sweep
build/regress-venv/bin/python build/w-W2-04r3/capture_cam.py race 12   # race_grid_a/b, race_hud + sweep
```

The pinned pose numbers in the test are derived values (positions, car matrix rows, mesh heights, engine px); no game
bytes. They come from those captures (`build/w-W2-04r3/oracle/*.json`, stride-1 sweeps for the driving poses).

## Open items

1. Fix `vehicle_model_test.cpp:75` (`!= 9` → `< 9`) — otherwise W2-03 regresses.
2. Decide the PNG-gate policy (see "Read this first" 2).
3. Slewed camera tilt (`ComputeChaseCameraBasis` x/z slew) is not modelled; it is the whole driving residual (≤1.9 px).
4. `camera-constants.md` should swap the X/Y scale labels (0.47 is vertical, NTSC horizontal is 500).
5. A real `FIT` needs sub-row landmarks: ground-feature edges from the *render* mesh (not the collision mesh) or
   a recapture at a different field parity; the collision mesh gave no usable edges.
6. `capture_cam.py` lives in untracked `build/`; promote it if the coordinator wants the pins regenerable.
