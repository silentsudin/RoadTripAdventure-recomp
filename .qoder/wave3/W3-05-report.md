# W3-05 worker handoff

## Outcome and limits

Implemented `src/native/gameplay/event_bridge.{h,cpp}`, deterministic test
`tests/native_w2/event_bridge_test.cpp`, read-only grounding reproducer
`tests/native_w2/event_bridge_grounding.py`, and
`docs/native-reference/event-bridge.md` (exact coordinator integration).
One isolated appended CMake block registers the module and
`w3_script_bridge_test`; no main.cpp edits, commits, or destructive git actions.

**Do not claim the original W3-05 premise is fulfilled.** ROM evidence rejects
0x8A=radio, 0x8B=cutscene, and the nominated countdown region. The town prefix
VM stops at >=0x20; pad-pattern 0x85..0x8B are one-byte hold values. The bridge
has explicit native radio/countdown/camera callbacks but never synthesizes
those callbacks from pad bytes. Countdown ElementA timing and camera-table
grounding are unresolved. No new gameplay integration has been performed.

The tested bridge has a 64-record owned FIFO, max 256-byte payloads, resumable
cursor/backpressure, bounded pumping, dispatch-once draining, callback-enqueue
deferral, nonrecursive drains, and log-once diagnostic indications. Unknown
town widths stay UNKNOWN (zero), not guessed. Literal rumble width 0x82=2;
0x80/81/83/84/85..8B=1. Raw opcode/payload/address/consumed width are retained.
The implicit runtime strength of 0x84 has `valueKnown=false`.

Live tests replay all eight pointer-selected ROM patterns, starts
0x2EA3D0/3D8/3E0/3E8/3F0/3F8/400/408, ending no later than 0x2EA40B: 36
records. No extracted streams or ROM assets added to tracked source files.

## Required final verification (verbatim commands and outputs)

All following commands ran from repo root, except the specifically marked
missing-ROM test.

```
cmake -S src/native -B build/w-W3-05 -DCMAKE_BUILD_TYPE=Release -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
```
```
-- rt_native_assets: Vulkan found (optional; used by rt-asset-preview)
-- rt_native_assets: SDL3 found (optional; not linked by any target here)
-- Configuring done (0.1s)
-- Generating done (0.3s)
-- Build files have been written to: /Users/holzr/github/RoadTripAdventure-decomp/build/w-W3-05
```

```
cmake --build build/w-W3-05 --target w3_script_bridge_test rt-app -j4
```
```
[  0%] Building CXX object CMakeFiles/rt_native_assets.dir/gameplay/event_bridge.cpp.o
[  0%] Linking CXX static library librt_native_assets.a
[ 96%] Built target rt_native_assets
[100%] Building CXX object CMakeFiles/w3_script_bridge_test.dir/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w2/event_bridge_test.cpp.o
[100%] Linking CXX executable w3_script_bridge_test
[100%] Built target w3_script_bridge_test
[ 96%] Built target rt_native_assets
[ 96%] Linking CXX executable rt-app
[100%] Built target rt-app
```

```
./build/w-W3-05/w3_script_bridge_test
```
```
SCORE w3_script_bridge_test checks=303 patterns=8 records=36 vaddrs=0x2EA3D0..0x2EA40B deterministic=1 PASS
```

```
./build/w-W3-05/w3_script_bridge_test
```
```
SCORE w3_script_bridge_test checks=303 patterns=8 records=36 vaddrs=0x2EA3D0..0x2EA40B deterministic=1 PASS
```

All four required targets remain; concurrently added collision/save targets also
remain. Read after the edit and again at handoff using the grep tool:
`add_executable\(w3_` in `src/native/CMakeLists.txt` returned
`w3_enhancements_test`, `w3_minimap_track_test`, `w3_text_metrics_test`,
`w3_collision_grid_test`, `w3_save_slots_test`, `w3_script_bridge_test`.
Read-only `git diff -- src/native/CMakeLists.txt` confirms this worker's sole
edit is the isolated final block. Other workers' staged-source edits preserved.

## Additional executed checks

Missing-ROM test, cwd `/Users/holzr/github/RoadTripAdventure-decomp/build/w-W3-05`:
```
HOME="$PWD" RT_ELF="$PWD/missing-ROM" ./w3_script_bridge_test
```
```
SCORE w3_script_bridge_test synthetic=PASS ROM=NO_ELF SKIP
```
Exit 0. Canonical-path + scratch-candidate lookup works without requiring CLI
arguments. An earlier attempt to mask HOME from repo root still found a scratch
ROM and ran the ROM checks; the isolated-directory run above proves skip.

Related regressions were built before the final additive valueKnown-field check:
```
cmake --build build/w-W3-05 --target w2_script_vm_test w2_text_script_test w2_audio_graph_test -j4
```
```
[ 96%] Built target rt_native_assets
[100%] Building CXX object CMakeFiles/w2_script_vm_test.dir/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w2/script_vm_test.cpp.o
[100%] Linking CXX executable w2_script_vm_test
[100%] Built target w2_script_vm_test
[ 96%] Built target rt_native_assets
[100%] Building CXX object CMakeFiles/w2_text_script_test.dir/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w2/text_script_test.cpp.o
[100%] Linking CXX executable w2_text_script_test
[100%] Built target w2_text_script_test
[ 96%] Built target rt_native_assets
[ 96%] Building CXX object CMakeFiles/w2_audio_graph_test.dir/Users/holzr/github/RoadTripAdventure-decomp/tests/native_w2/audio_graph_test.cpp.o
[100%] Linking CXX executable w2_audio_graph_test
[100%] Built target w2_audio_graph_test
```
```
./build/w-W3-05/w2_script_vm_test
```
```
W2-01 SCORE tables=21/330/307 entry_clean=307/307 unknown_ops=0 branch_oob=0 op_vectors=52/52 deterministic=1 branch_bleed=1 extra_vectors=17/17 PASS
```
```
./build/w-W3-05/w2_text_script_test
```
```
W2-07 SCORE advance_table=96/96 wrap_oracle_within2px=1.00(16/16) escapes=63/63 bounded_copy_fuzz=10000/10000 deterministic=1 oracle_vectors=13306/13306 elf_msgs=2476/2476 positions=2/2 fullwidth=96/96 font_glyphs=94/94 font_lower=26/26 font_slots=91/91
```
```
RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" ./build/w-W3-05/w2_audio_graph_test
```
```
W2-09 SCORE seq_vectors=24/24 rumble_vectors=23/23 pan_vectors=18/18 sndmod_math=20/20 ids=13/13 graph=49/49 alloc_ref=1995/1995 alloc_hash=720fe023f3c23092 elf_pins=15/15 irx_pins=5/5 tsq_files=20 tsq_tracks=622 tsq_ff=99 tsq_unknown=0 tsq_oob=0 tsq_runaway=0 tsq_alive=523 tsq_forks=27 tsq_stalls=0 action_phrase=1 deterministic=1 PASS
```
An initial audio test without RT_ELF printed NO_ELF and ran synthetic vectors
only; the rerun above includes ROM/IRX pins. No entire native-suite sweep claimed.

## Smoke coverage and earlier failures

Executed the plan's environment spelling, with build-private output paths:
```
RT_APP_HEADLESS=1 RT_APP_NO_AUDIO=1 RT_APP_STATE=town RT_APP_MODE=explore RT_APP_FRAMES=60 RT_APP_SNAPSHOT=build/w-W3-05/smoke.json RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" RT_PAK=build/w-W3-05/smoke.pak ./build/w-W3-05/rt-app
```
Verbatim relevant output (excerpt):
```
rt-app state=TITLE
rt-app frames=60 simulation_frame=0
```
This did **not** exercise gameplay: actual main.cpp expects PLAYING and
RT_APP_NOAUDIO, not `town`/`RT_APP_NO_AUDIO`. Corrected command executed:
```
RT_APP_HEADLESS=1 RT_APP_NOAUDIO=1 RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_FRAMES=60 RT_ELF="$HOME/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98" RT_PAK=build/w-W3-05/smoke.pak ./build/w-W3-05/rt-app
```
Verbatim relevant output (excerpt):
```
app: radio=off
rt-app hudtext: FONTE decoded 512x384 non-empty slots=438
rt-app state=PLAYING
rt-app session state=EXPLORE lap=0 finished=0 money=1000
```
```
rt-app minimap: town street segments=84
rt-app frames=60 simulation_frame=1
```
Exit 0; 60 render frames, only one simulation step in this fast headless run.
It is not a claim of 60 logic steps or runtime bridge integration. Smoke was
before the final additive valueKnown-field check; rt-app was rebuilt afterward.

Initial test-first compile command:
`c++ -std=c++20 -Isrc/native -fsyntax-only tests/native_w2/event_bridge_test.cpp`
failed because the new header did not yet exist. First executable run failed:
`FAIL 0x8a is hold, not radio` due to a test index typo (index 6 selected 8B;
fixed to index 5). Subsequent final runs above pass. Initial full build emitted
pre-existing unused warnings in assets/hud_art.cpp and runtime/guest_facade.h;
final incremental build above emitted none.

## Integration

Use `docs/native-reference/event-bridge.md` for exact loop anchors, callbacks,
width/fault rules, existing rumble-timing integration, and Android registration
limitation. App and GameLoop are untouched. Notably, do not directly dispatch
an entire decoded pad pattern into motor changes on the same frame; its hold
records belong to the existing `audio_graph::RumbleInterpreter` scheduler.
