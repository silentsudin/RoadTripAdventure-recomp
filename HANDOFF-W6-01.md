# W6-01 handoff — native AudioGraph

## Delivered

* `src/native/audio/audio_graph.{h,cpp}` — fixed four-bus (`music`,
  `ambience`, `sfx`, `radio`) deterministic S16 mixer with preallocated voice
  pool, Q16.16 nearest-lower resampling, looping/start offsets, Q16 ramps,
  clipping, and oldest-voice stealing.
* `tests/native_w15/audio_graph_test.cpp` — direct standalone behavior test.
* `docs/native-reference/audio-mixer.md` — authored model, arithmetic laws,
  and ROM-grounding boundary.

## API summary

`rt::native::audio::AudioGraph(outputRate, voiceCapacity)` uses at most 32
fixed slots. Start PCM via `startVoice(Pcm16Source, AudioBus, startOffset,
loop, gain, pan)`. Adjust it with `setVoiceGain`, `setVoicePan`, or
`stopVoice`; adjust buses with `setBusGain` and `setBusEnabled`; write stereo
PCM with `renderFrames(int16_t*, size_t)`. `Pcm16Source` is a non-owning S16
mono/stereo interleaved source view.

## CMake coordinator registration

Add this exact directory line to `src/native/CMakeLists.txt`:

```cmake
add_subdirectory(tests/native_w15)
```

Create `src/native/tests/native_w15/CMakeLists.txt` with this target snippet:

```cmake
add_executable(w15_audio_graph_test
    ../../../../tests/native_w15/audio_graph_test.cpp
    ../../audio/audio_graph.cpp)
target_include_directories(w15_audio_graph_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../..)
target_compile_features(w15_audio_graph_test PRIVATE cxx_std_20)
```

## Test command and observed output

```sh
mkdir -p build/scratch/w6-01
c++ -std=c++20 -I src/native -o build/scratch/w6-01/w15_audio_graph_test \
  tests/native_w15/audio_graph_test.cpp src/native/audio/audio_graph.cpp
build/scratch/w6-01/w15_audio_graph_test
```

Observed output:

```text
W6-01 checks=9 failed=0 negative_control=CAUGHT
```

## Open questions

* This module is intentionally separate from existing `audio/mixer.*` and
  `audio/engine.*`; the coordinator should decide which host path owns final
  device rendering and wire only one path at a time.
* SNDMOD's real pool/priority behavior remains driver-internal; oldest-voice
  stealing is an authored deterministic policy, not ROM parity.
