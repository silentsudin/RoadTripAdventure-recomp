# Road Trip recomp: where things stand and what's next

Written 2026-10-05 at the end of a long session, to pick up from after a reboot. Branches: app
`android-port`, fork `third_party/PS2Recomp` `roadtrip`. Test device: AYN Thor (Snapdragon 8 Gen 2,
Adreno 740, two screens). CLAUDE.md has the technical detail for everything named here.

## Picking up

1. Check where the commits are: `git log --oneline -3` in the app and in `third_party/PS2Recomp`.
   The last session ended by committing the lifecycle work (below) after a Mac suite run, then
   release-guard and a push (fork first, then the app).
2. If anything is uncommitted: `git status` in both repos. Run the Mac suite before committing
   runtime changes: `python3 scripts/regress.py -n 4` (about 20 min, expect 108 passed, 1 skipped,
   3 xfailed). Then use the `release-guard` agent, push the fork's `roadtrip`, then the app's
   `android-port`.
3. Android build and install: `scripts/android/build_apk.sh --install`. The first launch after a
   runtime header change rebuilds the game on the device (about 2.5 min, with the progress screen).
   RT_* switches go in `files/env.txt` (`adb shell run-as io.github.roadtrip.recomp ...`).

## Done (most recent first)

### Phase 3, in progress: Android lifecycle and performance (task #17)
- Background (home, another activity, screen off): the game pauses (unless the in-game menu already
  had) and the audio device closes. This is `src/platform/Lifecycle.cpp`, an SDL event watch,
  because SDL blocks the app's own loop while paused. Measured: 0% CPU while away (it used to keep
  running at about 70% of a core with audio playing); picture, audio and immersive mode come back.
- raylib's `CloseAudioDevice` destroyed its mutex before stopping the device, and that crashed the
  app at screen off. The fork patches it through FetchContent (`cmake/patch_raylib_audio.cmake`).
  Worth reporting upstream.
- The activity re-hides the status and navigation bars when it regains focus
  (`RoadTripActivity.java`).
- ADPF performance hints (`src/platform/android/PerformanceHint.cpp`, `RT_PERF_HINT=0` turns them
  off): the VU1, GS, game and presenter threads in one session with a 16.7 ms target.
- Ten-minute endurance run: 60 fps with no thermal throttling (CPU ≤ 70 °C, GPU at its lowest
  clock). Remaining dips to 54-58 fps in about 8% of 2 s samples come from the course-select
  carousel, where VU1 program `entry4` keeps the VU1 thread 80% busy (task #29). ADPF didn't change
  the dips; it may save power (unmeasured).
- `GifArbiter::sortQueue` no longer allocates: `std::stable_sort`'s buffer after every VU1 program
  was 12.6% of the VU1 thread on Android.

### Phase 2, done: first run on the device (task #16)
- Welcome screen, then Android's document picker. Images are read through `content://` URIs, with
  no copy.
- Disc images: .cue/.bin, .iso and .chd (libchdr, CD and DVD). A picked image is checked whole
  against Redump disc 27678: .bin SHA-1 `236b902a…`, .iso form `62a9b61f…`, CHD data
  `3db60fb0…`. A CHD is also checked against its own header record: damaged, or intact but made
  another way ("unverified", defaults to "Use it"). The boot ELF hash stays the hard gate.
- Setup screens in the game's style, reviewed by both UI critics:
  - a drawn sunny-road background;
  - the step ("Step 1 of 5"), a gold eased bar, a percentage, and time left from measured per-worker
    throughput;
  - D-pad focus with a horn cursor, plus tap;
  - "Try again" on failure;
  - a rumble and a fade into the game.
- Compiling carries on in the background.
- First-run compile on the Thor: 5:39 → 2:15, because the VU1 recompiler's `--split` emits one
  function and file per microprogram. A Mac fresh build takes 18 s.
- Licences: `scripts/collect_licenses.py` ships every component's licence in the Mac bundle and the
  APK. LGPL parts (paraLLEl-GS LGPL 3, libdwarf LGPL 2.1) are accepted because the project will be
  fully open source.

### Earlier (same day)
- Hardware GS (`RT_GS_BACKEND=hw`, the Android default): texture dumps and HD packs work and name
  textures exactly as paraLLEl-GS does (281/281 in a race).
- Performance, against ARMSX2 on the Thor in the same race:

  | | Recomp | ARMSX2 |
  |---|---|---|
  | CPU (GHz-equivalent) | 4.0 | 5.7 |
  | GPU (MHz-equivalent) | 90 | 108 |

  These are clock-weighted figures from `scripts/android/measure.py`. They come from lean VU1
  code, the cheaper GIF/GS hand-off and hardware GS batching.

## Next steps, in order

### 0. Bugs reported by the user (Android, Quick Race), fix first
- [ ] **Game speed drifts in 120 Hz mode (the audio too).** The game speeds up and slows down when
      the display runs at 120 Hz. Lead: guest vblanks (60 Hz) are paced by host presents, or by the
      present-at-time / display-lock path, instead of a steady 60 Hz clock, so a 120 Hz FIFO
      swapchain or present grid drifts. Check the EE scheduler's vblank source on Android, how
      `presentAtTimeWanted`, display lock and frame-gen settings behave there, and whether
      `settings.toml` sets Frame rate 120 on the Thor. Measure with `RT_SHOW_FPS=1`: vblank/s
      should be exactly 60.0. Fix: a wall-clock 60 Hz guest vblank, independent of the display
      rate; presents repeat or pace to the screen.
- [ ] **The Options page jumps between one row and the full list while scrolling.** Cause:
      picture rows are marked `preview` (`src/ui/PauseMenu.cpp`, `const bool preview = n &&
      rows[sel].preview;`, then `visible = preview ? 1 : ...`), so focusing one collapses the
      panel to a one-row strip at the bottom. **Decision (user): the menu never collapses.** Remove
      the one-row preview mode: the panel keeps its full list and position on every row. If seeing
      the picture change matters, the most that is allowed is lifting the scrim a little while a
      picture row is focused, with no change to the panel's size or position. Re-shoot with
      `RT_MENU_SHOT ... RT_MENU_PAGE=graphics` and have both UI critics check it.
- [ ] **Audio crackles.** Leads: the ring buffer in `ps2_audio_out.cpp` drops the oldest frames
      beyond 200 ms and plays silence on underrun; both click. Unsteady guest timing (above) makes
      both happen. raylib/miniaudio on AAudio uses `SetAudioStreamBufferSizeDefault(1024)`, which
      may be short for AAudio bursts on the Thor. Measure underruns and drops (add counters, log
      them every 5 s), try a larger device buffer and a gentle rate correction (resample by ±0.5%
      on ring fill level) instead of dropping or zero-filling. Re-test after the 120 Hz fix, since
      the two are likely linked.

### 1. Finish lifecycle and performance (#17)
- [ ] Commit and push the lifecycle work if the last session didn't (see "Picking up").
- [ ] Check pause and resume while the in-game menu is open, and during a loading screen.
- [ ] Android back button: open our in-game menu instead of quitting (it currently goes to SDL's
      default).
- [ ] Measure battery drain over Wi-Fi adb (`adb tcpip 5555`, then unplug; `current_now` reads 0
      while charging). Compare in-game, background, and ADPF on/off.
- [ ] Optional: suspend the GS/VU1 worker threads explicitly while away. They idle at 0% already,
      because the game is paused.

### 2. Android features
- [ ] **#18 Performance overlay:** an Options toggle showing FPS, frame time, CPU and GPU load on
      screen, drawn with the theme through the Vulkan presenter. The data exists (`[fps]` lines,
      `measure.py` sampling).
- [ ] **#19 Controller-aware button icons:** glyphs and wording for the Thor's built-in pad,
      Xbox, PlayStation and Nintendo layouts. The theme's `prompt()` reads the device family; the
      setup screens have no glyphs because they run before the input layer. Also show glyphs in
      the game's own prompts where we can.
- [ ] **#20 Dual screen:** use the Thor's second display (a minimap or HUD, an options panel, or
      the game itself). The HUD classifier can already separate HUD from 3D.
- [ ] **#21 Replace the game's Options menu** with ours.

### 3. Hardware GS completion (#24, #25, #27)
- [ ] Parity features: the UI mask (post-processing skips the HUD), motion vectors for TAA and
      MetalFX temporal, frame generation, and the minimap mask for dual screen.
- [ ] Image parity with paraLLEl-GS: the hw suite runs in 6 min against 21; its golden mismatches
      are expected and should shrink. A/B tool (`scripts/hwgs_ab.py`, planned).
- [ ] Texture packs on the hardware GS already pass `test_texture_pack*` with
      `RT_GS_BACKEND=hw`. Tidy and close #27.

### 4. Performance follow-ups
- [ ] **#29 VU1 code generation for the course carousel (`entry4`):** profile by node (vu1_replay
      on a carousel capture on the Mac, or simpleperf with debug info) and improve the generator.
      Verify with vu1_replay bit-exactness.
- [ ] **#23 Front-end batching:** partly done through draw-state serials.
- [ ] IOP/SPU2 costs on the game thread (interpreter): move them to their own thread, or recompile.

### 5. Save states (#28), feasibility first
A static recomp keeps guest thread state in native call stacks, so whole-machine snapshots aren't
free (the regression "checkpoints" are memory-card copies, not snapshots).
- [ ] Study: where Road Trip's threads idle (vsync waits, the kernel scheduler), and whether every
      live native frame at such points can be rebuilt from guest state.
- [ ] Study: what must be serialized: EE RAM, scratchpad, VU0/VU1 memory and registers, GS memory
      and registers, IOP RAM and SPU2, DMA and timers, the kernel's thread table, file handles.
- [ ] If feasible: save and load at quiescent points, slots with thumbnails in the in-game menu,
      and suspend-to-disk on Android background.

## Useful commands and facts
- Thor measurement: `scripts/android/measure.py io.github.roadtrip.recomp 12` gives clock-weighted
  CPU and GPU.
- Frame-rate log: `RT_SHOW_FPS=1`, an `[fps]` line every 2 s with VU1 and GS busy %.
- Quick Race by script: `RT_INPUT_SCRIPT=10:start,13:down,15:cross,...` (see the agent memory
  `game-navigation`).
- Profiling on the Thor: `adb shell simpleperf record --app io.github.roadtrip.recomp -t <tid> ...`
  (add `--call-graph dwarf` to unwind through libc).
- The Thor's own CHD: `/storage/3639-3330/roms/ps2/Road Trip (USA).chd` (the SD card is labelled
  "ayn_thor" in the picker).
- Test hooks: `RT_REBUILD_GAME=1`, `RT_SETUP_TEST_FAIL=1`, `RT_PERF_HINT=0`.
