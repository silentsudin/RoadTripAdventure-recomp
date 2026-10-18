# W5-01 handoff — tachometer RPM source

## Result

* Recovered the selected-gear coupling projection: `trunc(longitudinal_motion * 128 / gear_ratio)`, with six ROM gearbox rows, redline 10000, and both selector policies.
* `src/native/gameplay/gauge_rpm.h` deliberately accepts longitudinal motion, not the MPH display word. The RPM writer is an input/stateful integrator; the direct projection is the coupling law used by its selectors and is the native gauge input primitive.
* Calibration-domain checks using ratio 116: `106 -> 116` (vs ~117, residual -1) and `5634 -> 6216` (vs ~6217, residual -1). These are **not** MPH conversions.
* **GAP:** screenshots do not supply the longitudinal motion, gearbox, throttle/surface state, or previous RPM. A live state trace is required for a frame-exact transient replay or an MPH-to-RPM mapping.

## Verbatim configure transcript

Command:

```sh
RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w5-01 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
```

Output:

```text
-- The CXX compiler identification is AppleClang 21.0.0.21000334
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- rt_native_assets: Vulkan found (optional; used by rt-asset-preview)
-- rt_native_assets: SDL3 found (optional; not linked by any target here)
-- Configuring done (1.1s)
-- Generating done (0.3s)
-- Build files have been written to: /Users/holzr/github/RoadTripAdventure-decomp/build/native-w5-01
```

## Verbatim build + test transcript

Command:

```sh
cmake --build build/native-w5-01 --target w13_gauge_rpm_test -j4 2>&1 | tee build/scratch/w5-01/build-final.log | grep -E 'error|FAILED' | head -30
./build/native-w5-01/native_w13/w13_gauge_rpm_test
```

Build pipeline output (empty because grep found no `error` or `FAILED` lines):

```text
```

Captured build log:

```text
[ 96%] Built target rt_native_assets
[100%] Building CXX object native_w13/CMakeFiles/w13_gauge_rpm_test.dir/gauge_rpm_test.cpp.o
[100%] Linking CXX executable w13_gauge_rpm_test
[100%] Built target w13_gauge_rpm_test
```

Test output:

```text
W5-01 SCORE checks=9 failed=0 negative_control=CAUGHT
```
