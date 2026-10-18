# HANDOFF W4-05: dashcam footage source pipeline

Worker W4-05, branch `re/native-complete`. Nothing was committed or staged.

## Headline finding: the task premise is refuted

**Road Trip (SLUS-203.98) ships no recorded video footage.** There is nothing on the disc to extract and no
route-to-footage table in the ELF. The in-game "replay" re-simulates recorded controller input in real time. It is not a
video. In this project, "dashcam footage" means video the developers record of the oracle on their own machines, such as
`build/scratch/drive/drive_top.mp4`: a 1920x1080 H.264 screen capture of the PS2Recomp app on an Android device, whose
`log.txt` says `RECORDING`. Its 4:3 picture sits pillarboxed at x 240..1680. So the pipeline now indexes dev-supplied
clips in the app files dir; it does not pull anything from the ROM. The route key is proven from the ELF. The file
naming is an **AUTHORED convention, UNPROVEN by ROM**, because the ROM has nothing to prove it against.

## Evidence (all re-runnable; scratch under `build/scratch/w4-05/`)

| claim | status | evidence |
|---|---|---|
| No movie container is named in the ELF | PROVEN | `elf_strings.py` scans both PT_LOAD segments (0x200000 filesz 0x9a848, 0x29a880 filesz 0x9adfb) and finds 16,399 strings. The only disc paths are `cdrom0:\{IOPRP234.IMG,SIO2MAN,MCMAN,MCSERV,PADMAN,LIBSD,SDRDRV,SNDMOD}.IRX;1` at 0x2E9A58..0x2E9B00, plus the memory-card `/BASLUS-20398/icon.sys` and `choroQ.ico`. No .PSS/.IPU/.M2V/.AVI/.AVP/.STR/.MPG name appears, and no `sceMpeg`. Pack files are opened by LBA through the descriptor table at 0x29BB50, never by name. The test re-checks this: `movie_names=0 cdrom_names=8`. |
| The EE never touches the IPU (MPEG decoder) | PROVEN | `ipu_scan.py` and the test's `countMmioAccess` find 0 `lui`-based accesses to IPU regs 0x10002000..3F, IPU_FROM DMA 0x1000B000 or IPU_TO DMA 0x1000B400. Positive control: the same scanner finds 23 VIF1-DMA (0x10009000) and 27 GIF-DMA (0x1000A000) accesses, for example 0x0022720C and 0x00207B90. |
| The disc has no video files | PROVEN | `.lbn_map.tsv` lists 376 entries with extensions BIN 283, GSL 49, TSQ 20, IRX 8, VAG 6, TVB 3, E3D 3, plus ICO/IMG/CNF/ELF. |
| The in-game replay is input re-simulation held in RAM | PROVEN (decomp) | `SaveControlStateForReplay` 0x21CBC8 copies `gs_controls_car_object` 0x177AC50..`cameraBlocks` 0x177E6D0 (0x3A80 bytes) to EE RAM 0x100000. It then sets stream cursors at task+0x44/+0x48/+0x4C to 0x103A84/0x103A88/0x103A8C. `AcquireVehicleInput` 0x21A990 records when flag +0x28 bit 0x20 is set and plays back when bit 0x40 is set. Each stream is a run-length list of `{u16 run, u16 input}` with a 12-byte stride. The three player streams interleave at +0/+4/+8, and recording stops at 0x1FFFF8. `RestoreControlStateFromReplay` 0x21CCB8 prints `Replay Size[0..2] %d` (0x2EAE90). `UpdateRaceGameTask` 0x20F3B8 starts playback after a race: it calls `gs_music_play_bgm(0x0A180201)` (= `SOUND/REPLAY.TSQ`, audio-map.md §4a) and sets flags to 0x10044. Bit 0x40 mutes rumble and tire SFX during replay (0x217830, 0x217C68, 0x217F10). |
| The replay is never saved to the memory card | PROVEN (size) / INFERRED (no writer traced) | The replay arena is about 1 MiB (0x100000..0x1FFFF8), while the save block is 13,384 bytes (save.cpp). No save-path reference to 0x100000 was found. |
| Route key = course pack-type byte (game state +0x22) | PROVEN | `StartRaceModeTask` 0x20F950 sets `param_2[0x22] = *(u8*)raceRecord`. Values 0..17 are COURSE packs C00..C14, C18, C19, C20 (nav-index-binding.md). |
| Race slot to route tables | PROVEN | Byte 0 of each 16-byte record: adventure table 0x2BDDD0 (35 records, index < 0x23) and quick-race table 0x2BE158 (8 records). These are `kAdventureRaceRoute` and `kQuickRaceRoute`; the test reads them back from the ELF (35/35 and 8/8). |
| Every route stem names a real disc pack | PROVEN (disc) | The test finds `COURSE/<stem>.BIN` 18/18 on the user's disc. |
| Footage file naming `<STEM>[_n].<ext>` | **AUTHORED / UNPROVEN** | The ROM has no mapping. The convention reuses the disc pack stem so it stays tied to the proven key. |

### Probes tried for a ROM route-to-footage mapping (all negative)

1. ELF strings in both segments: searched for media extensions and the keywords movie, mpeg, ipu, pss, replay, record,
   ghost and demo. The only hits are dialogue ("movie theater" NPC text at 0x307E4A..0x329D18), the debug strings
   `REPLAY CAM #%d` (0x32FB98), ` RECORD` (0x333818) and the `Replay Size` printf.
2. Scanned .text for IPU MMIO and DMA access, with a positive control. Result: 0 hits.
3. Searched `functions.tsv`, `docs/native-reference/*.md` and `build/decomp/symbols.tsv` (1,665 symbols) for
   video/movie/avi/avp/ffmpeg/footage/replay/record/cinema. The only hits are the replay save/restore pair and the
   record-table functions (best times).
4. Disc file inventory: no video container.

### Next probes, if anyone still suspects on-disc footage

- Trace the attract demo (`game_mode=8`, flags 0x10104, `DEMO.TSQ`) to see whether it plays a pre-authored input stream
  from a pack, using the same 12-byte RLE format. If so, it is still re-simulation, not video. The attract trigger is
  untraced (app-flow.md open question 1).
- Look for an `.lbn_map.tsv` gap that no descriptor covers (asset-map.md already accounts for all of them; the only
  orphans are C16 and FONT.GSL).
- Read the movie-theater script path (dialogue at 0x321410) to confirm the "movie" is drawn in-engine.

## Delivered

- `src/native/media/dashcam_source.h` and `src/native/media/dashcam_source.cpp` (C++17, no dependencies;
  ffmpeg/ffprobe are run only as subprocesses through `popen`).
  - Fit constants: `kDashcamSrcW/H` = 1920/1080, `kDashcamScale` 1.5, `kDashcamXOff` 0, `kDashcamYOff` 317,
    `kDashcamFitIoU` 0.733. The comment says the legacy mappings (2.25 + 54 px, and 2.4107 + xoff 188) are WRONG and
    must not be used, and the test asserts they are not. Law: `src = panel * 1.5 + (0, 317)`, with helpers
    `dashcamSrcX/Y`, `dashcamPanelX/Y` and `dashcamSourceCrop`.
  - Route table: `kDashcamRoutes` (18 rows, id to pack stem), `kAdventureRaceRoute[35]`, `kQuickRaceRoute[8]`,
    `routeForAdventureRace`, `routeForQuickRace`, `routeForStem`, `packStemFor`, `parseClipName`, `canonicalClipName`.
  - Directory resolution through `resolveDashcamDir(DashcamEnv)`, in this order:
    1. `RT_DASHCAM_DIR`.
    2. On Android, `<filesDir>/dashcam`. The default is `/data/data/io.github.silentsudin.roadtrip.dev/files/dashcam`.
       Any path that has `io.github.silentsudin.roadtrip` (the release id) as a path component is refused.
    3. `$RT_SAVE_DIR/dashcam`.
    4. The app-local data dir + `/dashcam`: `RT_DATA_DIR`, or the same macOS default as `appDataDirectory()` in main.cpp,
       or `%APPDATA%` / XDG on other platforms.
    5. `./dashcam`.
  - `DashcamLibrary`: `rescan`, `clipsFor`, `availability` (NotACourse/Missing/Present), `pathFor` (lowest take, then
    extension priority .mp4 > .mov > .mkv > .webm > .m4v), and `importClip`. `importClip` is the first-run "extraction":
    it copies a dev recording to its canonical name, refuses the release dir, and does not overwrite unless asked.
  - `FfmpegFrameProvider`: `isRuntimeAvailable()` requires both executables, found through `RT_FFMPEG`/`RT_FFPROBE` or
    PATH. It also provides `probe`, `frameAt`, `panelFrameAt` (an `iw`/`ih`-relative crop, so the same window applies to
    clips that are not 1920x1080) and `presentRoute`.
  - Fallback policy (`decidePresentation`):
    - not a course, or no clip: **Skip** (do not draw the panel);
    - clip present but no runtime, or decode fails: **PlaceholderStill** (solid `kPlaceholderRgba` {24,24,28,255}
      panel with no text, which keeps the layout and frame diffs stable);
    - otherwise: **Footage**.
- `tests/native_w5/dashcam_source_test.cpp` builds as the target `w5_dashcam_source_test`.
- `tests/native_w5/CMakeLists.txt` adds the media source to `rt_native_assets` (so Android `libmain` gets it too) and
  globs `tests/native_w5/*_test.cpp` into `w5_*` executables.
- Registration: one line in `src/native/CMakeLists.txt`, right after `target_compile_options(rt_native_assets ...)`:
  `add_subdirectory(.../tests/native_w5 .../native_w5)`.

## Integration notes for the coordinator (main.cpp is untouched)

```cpp
#include "media/dashcam_source.h"
namespace md = rt::native::media;
md::DashcamEnv denv = md::DashcamEnv::fromProcess();
#ifdef __ANDROID__
if (const char* p = SDL_GetAndroidInternalStoragePath()) denv.androidFilesDir = p; // dev pkg files dir
#endif
md::DashcamLibrary dashcam(md::resolveDashcamDir(denv));
md::FfmpegFrameProvider dashcamFrames;          // probes PATH once
// per course entry: routeId = gameState+0x22 byte, or md::routeForQuickRace(i) / routeForAdventureRace(i)
md::Presentation how{};
md::RgbaFrame f = dashcamFrames.presentRoute(dashcam, routeId, tSec, panelW, panelH, &how);
if (how != md::Presentation::Skip) { /* upload f.pixels (RGBA8, top-down) into the panel quad */ }
```

- `frameAt` and `panelFrameAt` start one ffmpeg process per call, which costs tens of milliseconds. Call them from a
  worker thread, or only on menu entry and scrubbing, never once per frame on the render thread. A streaming
  `-f rawvideo` pipe reader is the natural next step if continuous playback is needed.
- Android has no ffmpeg on PATH, so `isRuntimeAvailable()` will be false there and the panel shows the placeholder. That
  is the intended graceful path. `RT_FFMPEG`/`RT_FFPROBE` can point at bundled binaries.
- **Open question (fit space):** the constants are stored as given, using the convention
  `src = panel * scale + off`. Under that reading the legacy forms are self-consistent: 448 × 2.4107 = 1080, and
  (1920 − 640 × 2.4107) / 2 = 188.6. I could not determine from the constants alone which unit "panel" is in.
  - In 640x448 design px, a full panel samples source (0, 317, 960, 672).
  - In 1280x447 oracle shot px, it samples (0, 317, 1920, 670.5).
  - In `drive_top.mp4` the game picture is pillarboxed at x 240..1680, so both readings include the black bar. See
    `build/scratch/w4-05/panel_t60.png`.

  Please confirm the coordinate space against the 73.3% IoU measurement before wiring.
- `build/scratch/w4-05/dashcam/C00.mp4` is a scratch symlink to `drive_top.mp4` (town and menu footage, not course
  C00), used only to prove the real-clip probe path.

## Build / test transcript

The given `cmake -S .` configures the PS2Recomp root project, which never adds `src/native`, so no native target would
be built. I used the native tree in my own dir instead:

```
$ RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/native-w405 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
-- Configuring done (0.7s) / -- Generating done (0.3s)
$ cmake --build build/native-w405 -j8          # full tree, all targets
[100%] Built target w5_dashcam_source_test      # no warnings from media/ or tests/native_w5/

$ ./build/native-w405/native_w5/w5_dashcam_source_test
ROM: race_tables=35/35+8/8 movie_names=0 cdrom_names=8 ipu=0/0/0 control_vif1=23 control_gif=27
DISC: course_packs=18/18
FFMPEG: probe=1920x1080 fps=30 panel=160x112
SKIP: RT_DASHCAM_DIR unset; no real dashcam footage probed
SCORE dashcam_source checks=234 failures=0 skips=1 PASS        exit=0

$ env -i HOME=/nonexistent-home PATH=/nonexistent RT_ELF=/nonexistent/SLUS_203.98 RT_DISC=/nonexistent \
    ./build/native-w405/native_w5/w5_dashcam_source_test
SKIP: ROM grounding (no ELF at /nonexistent/SLUS_203.98; NO_ELF rule)
SKIP: disc grounding (no COURSE/ under /nonexistent)
SKIP: ffmpeg/ffprobe not on PATH; footage decode path untested (placeholder policy verified above)
SKIP: RT_DASHCAM_DIR unset; no real dashcam footage probed
SCORE dashcam_source checks=220 failures=0 skips=4 PASS        exit=0

$ RT_DASHCAM_DIR=build/scratch/w4-05/dashcam ./build/native-w405/native_w5/w5_dashcam_source_test
...
REAL: clips=1 probed=1
SCORE dashcam_source checks=235 failures=0 skips=0 PASS        exit=0
```

Research probes: `python3 build/scratch/w4-05/elf_strings.py` (output in `elf_strings.txt`),
`python3 build/scratch/w4-05/ipu_scan.py` (0/0/0) and `ipu_scan_control.py` (VIF1 23 / GIF 27).

## Files

- `src/native/media/dashcam_source.h` (new)
- `src/native/media/dashcam_source.cpp` (new)
- `tests/native_w5/CMakeLists.txt` (new)
- `tests/native_w5/dashcam_source_test.cpp` (new)
- `src/native/CMakeLists.txt` (+1 line: the `add_subdirectory` registration)
- `HANDOFF-W4-05.md` (this file)
