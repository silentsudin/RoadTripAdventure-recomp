# W15 cost ledger — native vs the recomp/oracle

**Status:** CPU-path row published (2026-10-28); GPU-path row open.
Machine: Apple M3 Max, macOS 27.0.1, otherwise idle. 3 runs per row, median reported,
1,800 guest frames per run. Raw logs and the full tables:
`build/scratch/perf/w15/LEDGER2.md` (the earlier `LEDGER.md` is marked superseded as
mode-confounded).

## What is measured

- **Native** `build/w-G0-verify/rt-app`, headless, no audio; the GS scanout runs on the
  CPU sink. CPU time is the process's utime+stime.
- **Recomp/oracle** the harness build `build/macos-release/.../RoadTrip` with
  `RT_GS_BACKEND=cpu` — the same backend the scanout goldens use — so the GS runs on the
  CPU on both sides and the same guest work is compared.

## Row A — CPU GS on both sides (the comparable row)

| Scene | Native CPU-ms/frame | Recomp CPU-ms/frame | Native / Recomp |
|---|---:|---:|---:|
| race (1800 frames) | 9.483 ± 0.027 | 12.144 ± 0.064 | **0.78×** (−21.9%) |
| town (1800 frames) | 11.324 ± 0.342 | 25.839 ± 0.175 | **0.44×** (−56.2%) |

The translated C++ is cheaper per guest frame than the recompiled build on the same scenes
and backend. This is the row the goal's "native cost ledger cheaper than the recomp" refers
to; it is measured per guest frame, not by fps (real-time runs are paced).

## Row B — GPU path (open, not yet a verdict)

Native windowed Vulkan 10.33 (race) / 12.13 (town) CPU-ms/frame against the recomp's
offscreen hardware-GS 1.50 / 1.17. **This is not a comparison**: on the recomp side the GS
work is on the GPU and therefore does not appear in process CPU time at all, while the
native preview renderer still runs on the main thread and the sample includes terrain
startup (`loadTerrainMesh`/`appendTexturedTile` dominate the early samples). A GPU-path
row needs both sides uncapped, warmed (startup excluded) and attributed by GPU time, not
CPU time.

## Native hotspots found (optimization targets)

1. `rt::native::preview::*` — 40.5% of main-thread self samples: the preview renderer has
   no thread of its own; it renders on the game thread.
2. `previewRenderScene` — 12.2% self: scene submission.
3. Terrain build at startup (`loadTerrainMesh`/`appendTexturedTile`) — 98.7% of the early
   samples; a finite-process measurement must warm up before it is meaningful.

## How to reproduce

See `build/scratch/perf/w15/LEDGER2.md` — it records the exact env for each row
(`RT_APP_STATE/MODE`, the town pad script and `RT_DATA_DIR`, `RT_GS_BACKEND=cpu` on the
oracle side) and the raw `time -l` outputs.
