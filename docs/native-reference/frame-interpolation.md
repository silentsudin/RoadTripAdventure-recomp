# Native render-frame interpolation

## Decision and source

[`frame-timing.md`](frame-timing.md)'s verdict **(a)** establishes that the
retail root loop runs one VBlank-paced, nominal 60-Hz fixed simulation step.
The native host therefore retains that quantum rather than scaling recovered
physics, task counters, or clocks by host elapsed time.  `RT_APP_INTERP=1`
only makes the renderer consume interpolated poses between completed steps.
The default (`RT_APP_INTERP=0`, or unset) retains the original one logic step
per host iteration path.

## Accumulator and capture contract

`gameplay::FixedStepClock` banks host elapsed time in `1/60`-second quanta.
Each host iteration drains no more than `RT_APP_INTERP_CAP` quanta (default
`2`), so a stalled frame cannot create an unbounded simulation burst.  Any
remaining backlog yields a clamped render fraction; rendering never
extrapolates past the newest completed pose.

The accumulator is strictly a pacing mechanism: `RT_APP_FRAMES` remains a
count of **logic** steps.  The interpolation exit line records both totals:
`rt-app interp: steps=N renders=M`; with a finite `RT_APP_FRAMES`, `N` equals
that value exactly.  The simulation and renderer run on the host main thread,
so a capture cannot race an update.

An `RT_APP_SNAPSHOT` capture is made at a logic-step boundary (`t=0`): it uses
the completed `FrameSnapshot` directly, not the fractional render pose.  This
is intentionally different from ordinary presentation and is the determinism
contract for regression captures.

## Interpolated state

The renderer currently consumes only these moving scene poses:

* player-car `FrameSnapshot::carPosition` and `heading`;
* chase-camera `cameraEye` and `cameraTarget` (the view matrix is rebuilt from
  them);
* the optional route-following car override, folded into the same pose before
  blending.

`previewRenderScene()` currently draws no AI traffic instances, so there is no
traffic pose to interpolate yet.  HUD, lighting/fog, game clock, streaming,
collectibles, and all game state remain at the completed logic step.

Positions and camera vectors use component-wise linear interpolation. Heading
uses the wrapped shortest arc (`wrapAngle(next - previous)`), avoiding a full
turn when crossing the `-pi`/`+pi` seam.  Render fractions are clamped to
`[0, 1]` defensively.

## Environment flags

* `RT_APP_INTERP=1` enables split fixed-step simulation/render pacing; unset or
  `0` preserves the legacy path.
* `RT_APP_INTERP_CAP=N` sets the positive per-render catch-up limit (default
  `2`; non-positive values become `1`).
* `RT_APP_INTERP_RATE=N` optionally limits host render pacing to `N` fps.
  Without it, the queried display refresh is reported and used when available;
  otherwise presentation/vsync supplies pacing.

Startup reports `rt-app interp: fixed-step sim 60Hz render-rate=<queried>`.

## Determinism proof

The boundary round-trip is tested by `native_assets_interp_test`: extracting a
render pose then rebuilding its snapshot leaves the view matrix and all
non-pose fields bit-identical.  The end-to-end P6 proof was run on 2026-10-16:

```text
RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 \
  RT_APP_SNAPSHOT=/.../interp-off.ppm build/interp/rt-app
RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_INTERP=1 RT_APP_FRAMES=60 \
  RT_APP_SNAPSHOT=/.../interp-on.ppm build/interp/rt-app
cmp interp-off.ppm interp-on.ppm
```

Result: `cmp` exited `0` (byte-identical P6 files).  The interpolated run also
reported `steps=60`, preserving the requested logic-step count.
