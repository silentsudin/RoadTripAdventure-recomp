# W6-04 handoff: native RPM/gear session exposure

## Delivered

* `src/native/gameplay/game_loop.{h,cpp}` now owns an integer-only
  `DrivelineState` and `advanceDriveline` model using `gauge_rpm.h`.
* `FrameSnapshot` appends (without moving old fields) `int rpm` and `int gear`.
* `tests/native_w18/driveline_test.cpp` covers calibration goldens, shifts,
  clamps/reverse, a 600-frame sequence, byte-identical replay, snapshot
  exposure, and a negative control.
* `docs/native-reference/driveline-exposure.md` documents calibration, policy,
  API, and unresolved ROM gaps.

## Coordinator action required

I did **not** edit `src/native/CMakeLists.txt` per scope. Register the test
inside its `if(RT_NATIVE_ASSETS_BUILD_TESTS)` block:

```cmake
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w18" "${CMAKE_CURRENT_BINARY_DIR}/native_w18")  # W6-04 driveline exposure
```

For HUD wiring in `src/native/app/main.cpp`, consume the already-populated
`snapshot.rpm` and `snapshot.gear`; no new game-loop API call is needed.

## Verification run

* `W6-04 checks=13 failed=0 negative_control=CAUGHT first_shift_frame=114`
* Existing `native_assets_game_loop_test`: passed.
* App determinism gate: two independent 60-frame explore captures from
  `build/w-W6-04/rt-app` compared byte-identically with `cmp`.
