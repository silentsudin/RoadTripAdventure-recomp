# Native performance overlay

`src/native/gameplay/perfstats.{h,cpp}` is a pure-C++ accounting and formatting
module for the authored host. It deliberately has no SDL, Vulkan, renderer, or
environment dependency: the application supplies a monotonic wall clock and a
cumulative process CPU-time source, and tests inject both sources.

## Window statistics

The default window is **0.5 seconds** (configurable in the `PerfStats`
constructor). The completed window reports:

| overlay label | definition |
|---|---|
| `FPS` | successful host presents / wall seconds; this is the pictures-on-screen rate |
| `GAME` | completed fixed simulation steps / wall seconds; shown only when its rounded value differs from `FPS` |
| `GEN` | presents / logic steps; shown when materially different from 1.0 |
| `WORST` | maximum elapsed time between consecutive `notePresent()` calls, in milliseconds; an interval straddling a window boundary belongs to the newer window |
| `CPU` | `(getrusage user + system CPU delta) / wall delta * 100`; this is process CPU and can exceed 100% on multi-core work |

`GAME` must read 60 in a healthy interpolated run even when the render rate is
120 Hz: call `noteLogicStep()` exactly once for each drained
`FixedStepClock::takeStep()` quantum, rather than once per rendered image.
The module retains the last four non-warmup windows by default, oldest first,
as well as since-start present and logic-step totals. `resetWindows()` clears
partial/trend data after pause or resume and suppresses the next completed
window, avoiding fake long-frame and CPU spikes.

`OverlayModel` emits an ordered ASCII-only FONTE row (normally 40--47 bytes),
for example `FPS 120 GAME 60 GEN 2.0 WORST 14.2 CPU 34`. Rates and CPU use
integer half-up rounding; `GEN` and `WORST` use one decimal with half-up
rounding. Displayed numeric values cap at 999. An inactive model returns an
empty row/string.

## Host glue (not installed by this module)

The owning app integration belongs in `src/native/app/main.cpp`:

1. Construct `PerfStats` near the interpolation state at
   [`main.cpp:1073-1085`](../../src/native/app/main.cpp#L1073-L1085), with
   `steady_clock` seconds and a lambda that sums `ru_utime`/`ru_stime` from
   `getrusage(RUSAGE_SELF, ...)`.
2. In the `stepLogic` lambda, after every completed logic operation, call
   `perf.noteLogicStep()` at [`main.cpp:1185-1283`](../../src/native/app/main.cpp#L1185-L1283).
   In split pacing this is inside the drain at
   [`main.cpp:1290-1305`](../../src/native/app/main.cpp#L1290-L1305), not the
   render branch.
3. After a successful `present(...)` call in the host render loop
   ([`main.cpp:1389-1392`](../../src/native/app/main.cpp#L1389-L1392)), call
   `perf.notePresent()`. The actual swapchain present is
   [`main.cpp:317-345`](../../src/native/app/main.cpp#L317-L345), specifically
   `vkQueuePresentKHR` at line 342.
4. At the end of every host-loop iteration, after its logic/present work, call
   `perf.now()`. If it completed a non-warmup window, optionally print it and
   convert `OverlayModel::text()` into the existing FONTE glyph quads alongside
   the HUD text pass at [`main.cpp:1346-1382`](../../src/native/app/main.cpp#L1346-L1382).
   Pause/resume and swapchain-recreation recovery should call
   `perf.resetWindows()`.

Pseudocode:

```cpp
PerfStats perf(monotonicSeconds, processCpuSeconds);
OverlayModel overlay(envPerfOverlay());
// each completed fixed step: perf.noteLogicStep();
// after successful vkQueuePresentKHR path: perf.notePresent();
perf.now();
if (const auto w = perf.lastWindow(); w && !w->warmup) {
    if (envPerfLog() && (++windowCount % envPerfLogEvery()) == 0)
        std::cout << "[perf] " << overlay.text(*w) << '\n';
    const std::string row = overlay.text(*w);  // feed existing FONTE HUD builder
}
```

The FONTE renderer already accepts ASCII glyph quads (`preview/preview.h:96-120`)
and is composited after the scene (`main.cpp:1373-1377`). Use the bottom-edge
slot math and the 640x448 HUD coordinate conventions in
[`hud-layout.md`](hud-layout.md), keeping clear of the race HUD.

## Environment contract

* `RT_APP_PERFOVERLAY=0|1` controls the on-screen row; default `0`.
* `RT_APP_PERFLOG=1` prints completed eligible windows using the sibling-style
  `[perf]` prefix; default off.
* `RT_APP_PERFLOG_EVERY=<windows>` prints every Nth eligible window; default
  `1`, and invalid/non-positive values resolve to `1`.

## Renderer counters and GPU timing deferral

There is **no exported preview draw-call counter today**, so the overlay does
not invent one. `preview.cpp` submits several independent passes, including
scene work (`vkQueueSubmit` at lines 1172-1173) and the FONTE HUD pass (lines
1242-1243); its current public API returns pixels/errors only. A cheap future
CPU-side draw-call counter would require a new preview stats result/API and
incrementing it beside each `vkCmdDraw*` call.

GPU busy percentage, GPU frame time, and per-pass GPU timings require Vulkan
timestamp query pools (`VK_QUERY_TYPE_TIMESTAMP`), command-buffer timestamp
writes around the render passes, fence/readback handling, and timestamp-period
conversion. They are deliberately deferred; `vkQueueWaitIdle` duration is not
GPU timing and must not be reported as such.
