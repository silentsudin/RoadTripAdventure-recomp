# Authored AudioGraph mixer (W6-01)

`src/native/audio/audio_graph.{h,cpp}` is the deterministic host-side final
PCM mixer for the native rebuild. It does **not** emulate guest or IOP code:
Road Trip's shipped stack sends sequence/voice work to `SNDMOD.IRX`, while this
module accepts already-decoded S16 PCM and produces the host device's stereo
S16 stream.

## Graph and voice policy

The fixed graph is `music`, `ambience`, `sfx`, and `radio`, each feeding one
implicit master sum. Every bus has an enabled flag and a Q16.16 gain. Every
voice selects exactly one bus and has its own Q16.16 gain and Q1.15 linear pan:

* pan `-1` is left only, `0` sends full level to both channels, and `+1` is
  right only;
* mono input is duplicated before that pan law; stereo input retains its left
  and right source samples before the same per-side pan scaling;
* `startVoice` uses the first idle pool entry. When full it replaces the
  oldest active voice; equal serials resolve to the lower index. This is an
  authored, documented policy, not a claim about SNDMOD's driver-internal
  allocation.

The constructor preallocates a fixed array of at most 32 voices; rendering has
no heap allocation. Source views are non-owning, so their PCM storage must stay
alive while a voice is active.

## Deterministic render laws

All caller float gains/pans are quantized once at setter/start time. Rendering
then uses signed integer arithmetic only. A source phase is Q16.16 with
`step = floor(sourceRate * 65536 / outputRate)`. Each output frame reads source
frame `floor(phase / 65536)` (nearest-lower / no interpolation), then advances
the phase. This deliberately simple fixed policy gives exact copies at equal
rates and deterministic repeat/skip behavior at all other rates. Looping
voices reduce phase modulo `sourceFrames * 65536`; one-shots retire at end.

Gain changes have a fixed linear law. On a ramp of `N` output frames, render
frame `i` uses `start + (target - start) * i / N`, for `i = 0..N-1`; the gain
is exactly `target` after the Nth interval. Retargeting begins at the currently
quantized gain. Bus and voice ramps advance once per output frame even when a
bus is disabled. Per-voice scaling is applied before bus scaling; each signed
division truncates toward zero. The four bus outputs are summed at master and
saturated to `[-32768, 32767]` for interleaved stereo S16.

## ROM grounding and boundary

The replacement's *host architecture* is authored, but the separation of
music/voice control from PCM output is grounded in the reconstructed shipped
audio path:

* `docs/native-reference/functions.tsv` records `SndRpcPlayOrStop` at
  `0x00258308` as the RPC cmd-`0x0D` play/stop wrapper, and `PlayUiSound` at
  `0x00258E50` as its UI-sound caller.
* Its `ApplySoundVolumes` row at `0x002590B0` records the original global
  amplitude scaling path; it motivates separate bus and voice gains here.
* `SoundStopAllChannels` at `0x00258FF0` is the game-side teardown boundary.
* `audio-map.md` documents the original `SNDMOD.IRX` as the TSQ/voice, radio,
  and synthesized-engine driver, while its VAG inventory establishes the
  decoded sample-source material this host mixer consumes.

Thus sequence interpretation, VAG/TVB decoding, and authentic SNDMOD voice
allocation remain upstream concerns. `AudioGraph` is only the authored,
allocation-free final mixer needed once those sources have produced PCM.

## Standalone check

```sh
mkdir -p build/scratch/w6-01
c++ -std=c++20 -I src/native -o build/scratch/w6-01/w15_audio_graph_test \
  tests/native_w15/audio_graph_test.cpp src/native/audio/audio_graph.cpp
build/scratch/w6-01/w15_audio_graph_test
```

The test covers mono/stereo sources, Q16 resampling, gains, pan, ramps,
saturation, byte-identical replay, deterministic stealing, and a deliberately
perturbed pan-law negative control.
