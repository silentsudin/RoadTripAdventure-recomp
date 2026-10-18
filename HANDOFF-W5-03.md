# W5-03 handoff — race HUD state probes

## Delivered

* `docs/native-reference/race-state-inputs.md`: ROM-backed field, writer,
  cadence, reader, and uncertainty ledger.
* `src/native/gameplay/race_state_probe.h`: small cited constants only.
* `tests/native_w14/race_state_probe_test.cpp`: parses ELF `PT_LOAD` headers,
  pins all cited instruction words, SKIPs without `RT_ELF`, and proves a
  one-byte mutation is caught.
* `tests/native_w14/CMakeLists.txt` plus the one W14 `add_subdirectory` entry
  under `RT_NATIVE_ASSETS_BUILD_TESTS`.

## Coordinator wiring result

All three values are in the `0x270`-stride live car record:

| Input | Offset, representation | Writer evidence | Cadence |
|---|---|---|---|
| Running place | `+0x247`, byte; `0..N-1`, `0xff` inactive sentinel | `RankCarsByProgress` `0x0022d8a0`, `sb t3,0x247(a2)` | Every active `raceHudTask` update; call at `0x0022e138` |
| Current lap frames | `+0x260`, `uint32_t` | increment `0x0022e100..108`; reset `RecordLapSplitTimes` `0x0022d4c0` | Per eligible car frame; reset on lap boundary |
| Gear | `+0x1ff`, byte (`0=R`, `1..6`) | controls update `0x0021a38c`, `sb v0,0x1ff(s0)` | Per active car-control update; reset/spawn writes `1` |

The native snapshot needs separate fields for running place, current-lap
frames, and gear.  Do **not** reuse `FrameSnapshot::rank`: that is the
`GetRaceTimeRank` elapsed-time bucket, not `car+0x247`.

### Remaining GAP

`+0x247`'s writer and semantics are directly proven, but the ROM's
indirect/split `UpdateRaceHud` handler dispatch did not yield a direct
handler-0 `+0x247` load in the available static decompilation.  The direct
display-side `+0x260` load is the result/lap-row path (`0x0022daa4`), not
conclusively the live driving overlay.  Details are deliberately retained in
the reference document rather than overstated here.

## Verbatim command transcripts

```console
$ RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w5-03 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
-- The CXX compiler identification is AppleClang 21.0.0.21000334
-- The CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- rt_native_assets: Vulkan found (optional; used by rt-asset-preview)
-- rt_native_assets: SDL3 found (optional; not linked by any target here)
-- Configuring done (0.6s)
-- Generating done (0.3s)
-- Build files have been written to: /Users/holzr/github/RoadTripAdventure-decomp/build/native-w5-03
```

```console
$ cmake --build build/native-w5-03 --target w14_race_state_probe_test -j4 2>&1 | tee /private/var/folders/66/v2t7gtq135l3g5jlkbrp72_80000gn/T/opencode/rt-w5-03-build.log | grep -E 'error|FAILED' | head -30
```

The tee log (the grep intentionally emitted nothing) was:

```console
[ 96%] Built target rt_native_assets
[ 96%] Building CXX object native_w14/CMakeFiles/w14_race_state_probe_test.dir/race_state_probe_test.cpp.o
[100%] Linking CXX executable w14_race_state_probe_test
[100%] Built target w14_race_state_probe_test
```

```console
$ RT_ELF="$PWD/build/rom/SLUS_203.98" ./build/native-w5-03/native_w14/w14_race_state_probe_test
W5-03 checks=6 failed=0 negative_control=CAUGHT
```
