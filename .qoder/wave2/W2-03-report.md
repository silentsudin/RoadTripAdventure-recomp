# W2-03 — vehicle model report

## Delivered

* `src/native/gameplay/vehicle_model.{h,cpp}`: deterministic f15 velocity,
  speedometer, airborne position commit, and steering-command kernel.
* `tests/native_w2/vehicle_model_test.cpp`: ROM scalar/function provenance,
  exact speed/position/steering vectors, real 731-row telemetry checks, and a
  two-run deterministic replay hash.
* `tests/native_w2/data/vehicle_trace.csv`: 731 numeric rows captured from a
  single locked PS2Recomp Quick Race hold-gas run, with the exact guest dial
  algebra recorded every 30th frame (no ROM bytes).
* `docs/native-reference/vehicle-model.md`: cited algebra and integration
  boundary.

## Measured/pinned values

The read-only ELF at `~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98`
was measured again using `vaddr - 0x1ff000`:

* `0x00332f44 = 0x3fcdf3b6 = 1.6089999675750732`.
* Function FNV-1a-64: `0x21d358..0x21d387 = 078c393c7af6d8d1`,
  `0x249458..0x2494e7 = 0c0a7123de6d7f6a`, and
  `0x21ba70..0x21c69b = c92f40e0033aa59a`.
* Pinned scale: `32768*16/(25*20971.51953125*216) = 0.004623...` road units
  per `(GetSpeedFromF15Vector raw unit × frame)`.
* The captured vblank range is 4182..4912. Its final f15 vector
  `(15051,0,-2993)` yields 62 mph; it is within the golden's 63±1 interval.
  Captured position chord is 159.495776 RU, 0.656% from the independently
  documented 158.4568 RU anchor. The latter golden has vblank 4193..4924.

The documented `race_hud` → `race_hud_driving` chord remains 158.457 road
units over 731 frames (`.qoder/wave2-plan-sonnet.md:15-18`).  It cannot alone
calibrate acceleration because it is a chord rather than travelled arc length.

## Reproduce

```sh
cmake -S src/native -B build/w-W2-03 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-03 --target w2_vehicle_model_test -j
build/w-W2-03/w2_vehicle_model_test --elf "$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98"
```

Output: `SCORE: W2-03=PASS PASS`.

## Confidence / open questions

High confidence for scalar conversion, signed position division, gravity, and
the isolated steering command (direct decomp citations in the reference).
Medium confidence for the ground-contact boundary: the model deliberately
does not replace `ResolveGroundContact`.

The captured trace records car position and `car+0xf0` velocity every vblank;
the test checks every velocity cell against the exact speed algebra and checks
the final 62 mph and 0.656% chord result. It does **not** yet reproduce the
trace from a gas command alone: the force term in `gs_controls_car_update` and
selected `LoadCarPerformanceSpecs` records still need their integration
binding. The native host `/100` versus 107.5269 display-divisor GAP remains
intentionally unresolved.
