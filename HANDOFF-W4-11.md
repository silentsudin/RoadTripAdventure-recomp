# HANDOFF-W4-11 — METER0 needle input law

## Result

Resolved.  The 1P gauge needle is a tachometer driven by the signed short at
`car + 0x1D0`, not by the MPH display or its raw speed word.

ROM chain:

* Handler 0 (`0x2496C8`), at `0x249DC0..0x249EDC`, loads the car pointer,
  calls `CalculateTachometerValue(car, 0, 1)`, converts its result to float,
  divides by `10000.0f @ 0x332F50`, adds `-0.7f @ 0x332F54`, and stores the
  half-turn z rotation at the RotQuads record offset `+0x88`.
* `CalculateTachometerValue` (`0x249520`) begins `lh v0,0x1D0(a0)`, and with
  the handler arguments only caps values above `10000`; it does not scale.
* There is no handler-local sine/cosine lookup: `0x249EDC` stores the scalar
  half-turn rotation in the generic `uiArenaRotQuads` record.  That generic
  renderer applies the `pi` convention while rotating the supplied strip.
* `AdvanceVehicleMotion` (`0x218EE0`) produces this field and clamps it to
  `[0,10000]`; its gear-ratio use confirms the engine-RPM semantic.

Law for real vehicle state:

```text
needle_theta_rad = pi * (rpm / 10000 - 0.7)
```

`theta=0` points up and positive is clockwise.  Inverting it against supplied
design-space measurements yields ~117 RPM for `race_hud.png` (39 mph,
no-throttle) and ~6217 RPM for `race_hud_driving.png` (63 mph).  This is a
sanity check only, not an MPH fit.

## Files changed/added

* Added `src/native/gameplay/gauge_needle.h`: header-only ROM-law constants,
  `angleRadForRpm`, and METER0 rotated-needle `DrawQuad` builder.  It documents
  that the coordinator, not this worker, flips `kRaceHudNeedleLawResolved`.
* Added `tests/native_w11/CMakeLists.txt` and
  `tests/native_w11/gauge_needle_test.cpp`: ROM instruction/data pins, law and
  geometry assertions, ROM-less exit-0 skip, and an in-memory one-byte-flip
  negative control.
* Updated `src/native/CMakeLists.txt`: one W4-11 test subdirectory line inside
  `RT_NATIVE_ASSETS_BUILD_TESTS`.
* Updated `docs/native-reference/gauge-calibration.md`: resolved tachometer
  law and corrected the former gear-plate bbox misidentification.

## Verbatim build/test transcript

```text
$ RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w4-11 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
-- The CXX compiler identification is AppleClang 21.0.0.21000334
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- rt_native_assets: Vulkan found (optional; used by rt-asset-preview)
-- rt_native_assets: SDL3 found (optional; not linked by any target here)
-- Configuring done (0.8s)
-- Generating done (0.4s)
-- Build files have been written to: /Users/holzr/github/RoadTripAdventure-decomp/build/native-w4-11

$ cmake --build build/native-w4-11 -j8 --target w11_gauge_needle_test 2>&1 | tee build/scratch/w4-11/build.log | grep -E 'error|FAILED' | head -30

$ cat build/scratch/w4-11/build.log
[100%] Built target rt_native_assets
[100%] Building CXX object native_w11/CMakeFiles/w11_gauge_needle_test.dir/gauge_needle_test.cpp.o
[100%] Linking CXX executable w11_gauge_needle_test
[100%] Built target w11_gauge_needle_test

$ RT_ELF="$PWD/build/rom/SLUS_203.98" ./build/native-w4-11/native_w11/w11_gauge_needle_test
W4-11 SCORE checks=8 failed=0 negative_control=CAUGHT

$ RT_ELF=/nonexistent ./build/native-w4-11/native_w11/w11_gauge_needle_test
SKIP gauge needle: no ELF (set RT_ELF or provide build/rom/SLUS_203.98)
```

The `negative_control=CAUGHT` row is produced after the test flips one byte at
the pinned `0x249DC0` instruction in an in-memory ELF copy; the ROM-pin matcher
then rejects it.
