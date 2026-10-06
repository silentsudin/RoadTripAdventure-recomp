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

### 0. Bugs reported by the user (Android, Quick Race): fixed 2026-10-06
- [x] **Game speed drifts in 120 Hz mode.** The guest clock was steady (`RT_VBLANK_JITTER=1`: 300
      vblanks per 5 s, none bunched). The picture wasn't: Frame rate 120 asks for frame generation,
      which only paraLLEl-GS renders. On the hardware GS (Android) the extra presents repeated the
      last picture unevenly on a 120 Hz FIFO swapchain, so the game looked as if it sped up and
      slowed down. Frame rates above 60 now need `Capabilities::frameGeneration` (set for
      paraLLEl-GS only), so the row is hidden and the setting ignored on the hardware GS.
      Re-enable when the hardware GS gets frame generation (#25).
- [x] **Audio crackles.** The SPU2 runs on the guest clock and the device on its own (about 0.02%
      apart on the Thor), so the queue grew until the 200 ms cap dropped frames. The callback now
      plays the queue slightly faster or slower (linear interpolation, -0.5% to +1%) to hold it
      near 50 ms: no underruns or drops in a race, and 120 ms less audio lag. `RT_AUDIO_STATS=1`
      logs underruns, drops and the queue every 5 s. User to confirm by ear.
- [x] **The Options page jumped between one row and the full list.** The one-row preview mode is
      gone; the panel keeps its size and position, and picture rows only lift the scrim a little.

### 1. Finish lifecycle and performance (#17)
- [ ] Commit and push the lifecycle work if the last session didn't (see "Picking up").
- [x] Pause and resume with the in-game menu open: the game stays paused under the menu.
- [x] Android back button: opens our in-game menu (the manifest traps it; AC_BACK toggles the menu).
- [x] Pause and resume during a loading screen (checked on the Thor: the race starts normally after).
- [ ] Measure battery drain over Wi-Fi adb (`adb tcpip 5555`, then unplug; `current_now` reads 0
      while charging). Compare in-game, background, and ADPF on/off.
- [ ] Optional: suspend the GS/VU1 worker threads explicitly while away. They idle at 0% already,
      because the game is paused.

### 2. Android features
- [x] **#18 Performance overlay:** Options → Performance overlay: Off / Frame rate / Detailed
      (`src/ui/PerfOverlay.cpp`, numbers from `src/debug/PerfStats.cpp`): one row along the bottom
      edge, clear of the race HUD: fps, worst frame, VU1/GS load, app CPU and, on Android, GPU load
      and clock and battery temperature.
- [x] **#19 Controller-aware button icons:** pads are grouped by family (`padFamily` in
      `src/platform/input/Mapping.cpp`): PlayStation keeps the game's layout; Xbox-style pads
      (including the Thor's built-in pad) and Nintendo pads confirm with A and go back with B
      (`familyProfile` moves the face buttons; menus follow). Our prompts use the family's letters,
      and on the hardware GS the game's own glyphs (the font atlas's ○ ✕ □ △, L1/L2/R1/R2, the
      BACK/OK buttons) are repainted with the pad's letters as the atlas is decoded
      (`src/ui/ButtonGlyphs.cpp`, the backend's decode hook). Checked on the Thor with a virtual
      Xbox pad: A enters, B goes back. paraLLEl-GS keeps the PlayStation glyphs (no CPU decode).
- [x] **#20 Dual screen:** the Thor's lower display shows the game's map (moved off the top
      screen by the hardware GS), a race column, and the stamp notebook with coins, towns and
      money (see CLAUDE.md, "Second screen"). Options → Second screen turns it off.
  - [x] Now playing (town radio): PEACH FM named in full; E-RADIO named where matched against
        Michael Walthius's MIDIs (more pending the dongrays.com check).
  - [x] Stamp earned / new best lap moments; map layer at 4x; clock at 150 vblanks a minute.
  - [x] Town map marks: Q's Factory, shops, Quick-Pic Shops not used, houses not visited; photos counters.
  - [ ] Field areas between towns (locations 10..21): check the minimap projection there.
- [x] **#21 Replace the game's Options menu** with ours: Title > Options opens our menu at its
      Sound rows (Volume, Speaker, Vibration), saved in settings.toml (the game never saved them).

### 3. Hardware GS completion (#24, #25, #27)
- [x] UI mask on the hardware GS (post-processing spares the HUD); the map layer for dual screen.
- [ ] Parity features still to do: motion vectors and depth (TAA, MetalFX temporal), frame generation.
  - [x] Motion vectors on the hardware GS (TAA): per-vertex motion on GSVertex, an RG16F attachment
        written by unblended and As/1-As draws; matches paraLLEl-GS's field (`RT_SHOW_MOTION=1`).
  - [ ] Depth on the hardware GS (MetalFX temporal, GSR 2, Arm ASR).
- [ ] Mobile upscalers (shown greyed "later" in Options today):
  - [x] Snapdragon GSR 1 (spatial, single pass): Options > Upscaling on Android (either GS).
  - [ ] Snapdragon GSR 2 and Arm ASR (temporal): colour, depth, motion vectors and jitter; after the
        hardware GS's motion vectors (paraLLEl-GS has them already).
  - [ ] Arm NSS: only on Mali GPUs with neural accelerators (not testable on the Thor).
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

### 5. Save states (#28)
- [x] Feasibility study: doable with restrictions, no redesign (guest threads are resumable
      contexts, not native stacks). Plan, risks, effort (20-25 agent-days; Mac MVP 12-15) and
      tests in `docs/save-states.md`.
- [ ] Spike A: how often a vblank can be saved (`canSnapshot()` reasons over race/town movies).
- [ ] Spike B: same-process save -> run -> restore -> run, hash equality over 600 vblanks.
- [ ] Then the MVP (Mac, CPU GS + paraLLEl-GS), the hardware GS, Android, the menu slots.
