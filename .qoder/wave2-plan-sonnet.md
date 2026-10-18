# Wave-2 batch plan (P2, independent) — native rebuild of Road Trip (SLUS-203.98)

Author: claude-sonnet-5-5 (P2). Date: 2026-10-17. Read-only analysis; this file is the only artifact.
Baseline: GAME-LOOK v13, 73 native tests green, ELF RE ~complete. All evidence below was either
read directly this pass (marked **[R]**) or taken from `docs/native-reference/*.md` (cited by doc).

---------------------------------------------------------------------------------------------------
## 0. Pre-flight: what I re-verified (and where the brief / prior docs are wrong)

| # | Claim | Verdict | How I checked (reproducible) |
|---|---|---|---|
| 1a | `.GSL` = GIF DMA texture-upload chain only, no script bytes | **VERIFIED [R]** | python3 walk of all `**/*.GSL` on the disc: 49 files, 1142 units, every tag byte3 = 0x10 until END (0x70), `qwc == 6 + NLOOP` for 1142/1142, exactly 48 bytes after END in all 49. Matches `gsl-vm.md` §1–2. |
| 1b | `InterpretScriptCommands` @0x23AD00, jump table @0x2EC750, 29 entries ops 1..0x1D | **VERIFIED [R], with 3 corrections** | `struct.unpack('<29I')` at file off `0x2EC750-0x1FF000`: all 29 targets lie in `0x23AD50..0x23B0E0` (inside the function). **Only 27 distinct targets; ops 9, 10, 12 all alias the unknown-op stub `0x23B0E0`** → 26 live handlers, not 29. Dispatcher bytes at 0x23AD28..0x23AD44 (`addiu -1; sltiu 0x1d; lw 0x2EC750+op*4`) confirmed. Decomp (`build/decomp/fn/0023ad00.c`): `if (0x1f < byte) return ptr` → bytes ≥ 0x20 end the control prefix; 0x1E/0x1F fall to the unknown path. |
| 1c | (NEW, answers `gsl-vm.md` open question #5) where the scripts actually live | **FOUND [R]** | Spawners at `0x2105B8` / `0x22AEB0` (decomp) do `gs_tasks_spawn(ctx, 0x23B168, *(u32*)(shopId*4 + *(u32*)(0x2A7ED0 + town*4)))`. `0x2A7ED0` = 22-word table (word 0 = 0, words 1..21 = per-town tables at `0x2A5740…0x2A7EC8`). Level-2 tables (e.g. `0x2A4450`) = `{name-string ptr, entry-script ptr, branch targets…}`; level-3 = script/text bytes in ELF `.data` (e.g. `0x2F2AC0` = `0E 1D 1E 1F 20 21 22 | 01 00 05 …` then English text; `0x2F2C38` = "Q's Factory"). **Scripts + dialogue text are ELF-resident (`.data` ≈ `0x2F1300..0x334378`), not on disc files.** Crude un-pinned census: 21 towns, 330 level-1 slots, 307 distinct level-2 tables, 2757 distinct level-3 pointers (includes pure-text targets). `SHOP/Tnn.BIN` = a 2-unit GSL chain (END @0x3C4E0) + other data; `T16.BIN` tail is zero padding. |
| 2a | Race kinematic anchor 158.4 units / 731 frames | **VERIFIED arithmetic [R]** | `race_hud` (3-D pos 616.684, 568.756) → `race_hud_driving` (747.502, 479.340): chord = **158.4568** units; vblank 4193→4924 = **731**; total-time frames 2447→3178 = **731** (timer +1/frame ✓, `frame-timing.md`). |
| 2b | "dial ≈ 63 mph at the end" | **VERIFIED (visual crop [R])** | `race_hud_driving.png` digits = "63". |
| 2c | `gauge-calibration.md` (B5): `race_hud` shows **99** mph and needle +0.36° | **REFUTED [R]** | Cropped/enlarged the dial: `race_hud.png` reads **"39 mph"** (also `oracle-golden-frames.md` table: "39 mph"). The "+0.36°" component (1166..1202, 353..366) is not the needle (needle in that frame points left/down of the hub ≈ 195°±8°; in `race_hud_driving` ≈ 117°±8°). Implied slope ≈ −3.3°/mph (visual, ±0.5), i.e. B5's slope accidentally ≈ right but the rest angle and the 99-mph point must NOT be committed. Needs ≥4 measured points. |
| 2d | Anchor ⇒ unit scale | **OPEN [R]** | If 1 world unit were 1 m: chord-average = 158.457/731×60 = 13.0 u/s = 46.8 km/h = **29 mph**, below both dial readings (39→63). So path ≫ chord and/or unit ≠ 1 m (`rules-tables.md` §5.1: len×216.0 = km/h for f15 m/frame; native camera converts world→render by ×0.1, `camera-framing.md`). The anchor alone cannot fix the scale; a per-frame trace is required (W2-03). |
| 2e | (NEW) HUD race timer format | **DERIVED [R]** | Decomp of `raceHudElementC` `0x22F750`: digits are nibbles `[f/36000][(f/3600)%10][(f/60%60)/10][(f/60%60)%10][(f%60)/6][((f%60)*100/60)%10]`; `0`/`0x7FFFFFFF` → `--`(0xAAAAAA); `≥360000` → 99'59"99. Oracle HUD shows 00'40"76 / 00'52"95 while manifest total-time = 2447 / 3178 → HUD time = **frames−1** matches 2/2 (2446 → 76, 3177 → 95); frames−0 matches 0/2. (`FormatRaceTimeBCD 0x2495F0` divides by 0x96 = 150 → that is the **town clock**, not the race timer.) |
| 3 | SOUND/*.VAG = streams; TSQ/TVB sequence files | **VERIFIED [R]** | 6 files `VAGp` v0x20 (BE), 12000 Hz, 3603.0 / 3605.6 / 3630.0 s per side; 20 `.TSQ`, 3 `.TVB`. SFX are **not** files (`audio-map.md` §6: ids go via `PlayUiSound 0x258E50`→cmd 0xD; id→sample map OPEN). |
| 4 | "functions.tsv names 1542 functions" | **WRONG [R]** | `docs/native-reference/functions.tsv` has **737** rows (README says 731). **1542** = `build/decomp/fn/*.c` count / the "1542/1542 named" commit (`build/decomp/symbols.tsv`, 1665 lines incl. data). Cite decomp `.c` files for the other ~800. |
| 5 | Score baselines | **REPRODUCED [R]** | `scripts/frame_compare.py` `histogram_intersection`: title-v13 **0.872**, menu-v13 **0.460**, town-v13 **0.532**, race-v13 **0.374**. Caveat: mean-abs-diff on the same pairs is still 75–89 /255 (title "CLOSE" at 0.872 has MAD 89), so histogram intersection is a weak, gameable metric. town-v15 = 0.484 < v13 (regression), race-v16 = 0.406. |
| 6 | Pixel→UI mapping `x/2, y/0.93125−16` | **SCENE-DEPENDENT [R]** | Gauge digits: ELF places them at 448-space y = 336+49 = 385..411 (`hud-layout.md` §1.2) and I observe the glyphs at screenshot y ≈ 388–408, i.e. **shot_y ≈ y448 (×0.998, offset ≈0)**; the 480-line formula would predict 373–398 (off by ~12 px). Menu frames do fit `/0.93125−16` (menu-calibration). Do **not** apply one mapping to every scene; W2-14 builds a per-scene calibration table. |

Consequences for the plan: (i) no GSL "script VM" is needed — the only opcode VM is the town/shop event
interpreter and its bytes are in the ELF; (ii) HUD work must use 448-space ELF coordinates and per-scene
oracle mapping; (iii) the speed/needle calibration must be redone from ≥4 oracle points; (iv) the score
metric must be strengthened before it can steer wave 2.

---------------------------------------------------------------------------------------------------
## 1. Ground rules for all 14 batches

1. **New files only.** Wave-2 workers create the files listed under "Owns" and edit nothing else
   (no edits to existing `src/native/**`, existing tests, `docs/*` outside their own new doc,
   `third_party/`, `build/native/`, any `CMakeLists.txt`, `src/native/app/main.cpp`, `.qoder/*` other than
   their report). Superseding an existing module (e.g. `race.cpp`, `shop.cpp`, `car_physics.cpp`) is a
   *coordinator integration step*. This is what makes the batches conflict-free.
2. **Report file per batch** (counts as owned): `.qoder/wave2/W2-<NN>-report.md` (summary, files, key
   numbers + how to reproduce, confidence, open questions).
3. **Private build dir**: `build/w-W2-<NN>/`. Never `build/native`.
4. **ROM read-only; no game bytes in repo.** Tests take the ELF path via `--elf` / `RT_ELF`
   (default `~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98`, vaddr − 0x1FF000 = file
   offset). Pin only counts, hashes (FNV-1a/CRC32) and derived numbers — never string/table bytes.
   Missing ELF ⇒ test exits **2** (`NO_ELF`), never silent-pass; `--allow-skip` exists only for CI without ROM.
5. **Acceptance protocol** (deterministic, scriptable, numbers pre-registered here; only the coordinator may
   re-register a threshold):
   ```
   cmake -S src/native -B build/w-W2-<NN> -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
   cmake --build build/w-W2-<NN> --target w2_<stem>_test -j
   build/w-W2-<NN>/w2_<stem>_test --elf "$RT_ELF"     # prints one line:  W2-<NN> SCORE k=v k=v …
   echo $?                                            # 0 only if every threshold is met
   ```
   Every test is single-threaded, seed-fixed, runs the whole suite twice and compares a result hash
   (`deterministic=1`). Wall time budget ≤ 60 s per batch.
6. Oracle goldens live in `build/scratch/oracle-golden/` (1280×447, untracked). Oracle recaptures use
   `scripts/oracle_golden_capture.sh` with `RT_GOLDEN_OUT=build/w-W2-<NN>/oracle` and `RT_ONLY=<scene>`
   (one oracle at a time; take `rt-gpu.lock` as `native_sample.py` does).

### Step 0 — coordinator pre-dispatch (NOT a worker batch; ~15 lines of CMake, done once)
`src/native/CMakeLists.txt` is coordinator-owned and registers sources/tests explicitly, so all new
files must be pre-registered or workers cannot build. Do this before dispatching:
* Append to the existing `foreach(_rt_staged_source …)` (EXISTS-guarded, so absent files are harmless):
  `assets/script_image.cpp assets/price_tables.cpp gameplay/script_vm.cpp gameplay/app_flow.cpp
  gameplay/vehicle_model.cpp gameplay/terrain_query.cpp gameplay/race_rules.cpp gameplay/economy.cpp
  gameplay/text_script.cpp gameplay/save_v2.cpp audio/audio_runtime.cpp audio/sfx_table.cpp
  audio/vag_stream.cpp gameplay/hud_live.cpp gameplay/minimap.cpp gameplay/input_map.cpp
  gameplay/enhancements.cpp`
* Inside the `RT_NATIVE_ASSETS_BUILD_TESTS` block add
  `file(GLOB W2_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/*_test.cpp)`
  → for each, `add_executable(w2_<stem> …)` linking `rt_native_assets`, same warning flags (stem keeps its
  `_test` suffix → target `w2_<stem>_test`).
* Add `file(GLOB W2_TOOLS CONFIGURE_DEPENDS tools/w2_*_main.cpp)` → executables `rt-w2-<name>`.
Without Step 0 no batch can run; with it, batches never touch CMake again.

---------------------------------------------------------------------------------------------------
## 2. Batch specs

Lane/escalation use the `wave-board.md` lane matrix (no omlx/strata; ≤2 lanes per provider). "Escalate to"
= next tier up with the failure report embedded.

### W2-01 — Town/shop event-script VM (host interpreter for `InterpretScriptCommands`)
* **Objective**: bit-faithful host VM for the 26 live control ops + an ELF-resident script image reader
  (3-level pointer walk, §0 row 1c). **Not** a GSL interpreter (GSL = textures, row 1a). The VM exposes
  an abstract `IScriptHost` whose methods mirror the named callees in `gsl-vm.md` §5.1
  (`TestFlagBitFc98`, `GetFlaggedSlotCount`, `TestStampFlag`, `TestFlagFc68`, `TestStateBitFc78`,
  `CheckSlotPresence`, `MatchSlotPairState`/`TestFlagFcb8`, `GetOrSetPresidentRaceResult`,
  `TestFlagFca8Clear`, `SetFlagBitFc98`/`ClearFlagBitFc98`, `ClearFlaggedSlotBit`, `RecordTaskResult`,
  `SetStateBitFc88`, `TestProgressFlag*`, `CountBitsFca8Fcb0`, `TestObjectAttribute`, …) so later waves bind
  it to `SaveData`/economy without touching the VM.
* **Evidence**: `InterpretScriptCommands 0x23AD00..0x23B167` **[R]**, jump table `0x2EC750` **[R]**,
  unknown stub `0x23B0E0` **[R]**, caller `dialogueTask 0x23B168` **[R]** (task+0x1c table; `[1]` = entry
  script; branch target = `table[branchIdx]`), spawners `0x2105B8`/`0x22AEB0` + town table `0x2A7ED0` **[R]**;
  opcode/width draft `gsl-vm.md` §5.1; helpers in `functions/script-dialogue.md`; per-op case vaddrs
  `0x23AD50 0x23AD70 0x23AD90 0x23ADA8 0x23ADC8 0x23ADE8 0x23AE08 0x23AE20 0x23AE58 0x23AE78 0x23AEA8
  0x23AEC0 0x23AEE0 0x23AF00 0x23AF38 0x23AF60 0x23AFA0 0x23AFC0 0x23AFF8 0x23B010 0x23B028 0x23B048
  0x23B060 0x23B078 0x23B0B0 0x23B0C8` (from the table **[R]**). Worker must settle widths of ops
  0x08/0x0E/0x18/0x1B (doc marks "STRONG/optional skip") from the decomp.
* **Owns**: `src/native/assets/script_image.{h,cpp}`, `src/native/gameplay/script_vm.{h,cpp}`,
  `tests/native_w2/script_vm_test.cpp`, `docs/native-reference/script-vm.md`, `.qoder/wave2/W2-01-report.md`.
* **Acceptance** (`w2_script_vm_test --elf`):
  `W2-01 SCORE tables=<towns>/<l1slots>/<l2tables> entry_clean=<n>/<n> unknown_ops=0 branch_oob=0 op_vectors=52/52 deterministic=1`
  — (a) worker pins the table census (my un-pinned snapshot: 21/330/307; any different value must be
  justified in the report); (b) **100%** of level-2 `[1]` entry scripts decode with a clean prefix and end on
  a byte ≥ 0x20, zero hits of ops {0,9,10,12,0x1E,0x1F} in a prefix; (c) every branch index < its level-2
  table length (0 out-of-bounds); (d) 26 ops × {taken, not-taken} = **52/52** synthetic vectors vs a stub host;
  (e) ops 9/10/12 and ≥0x1E reproduce the "Unknown pre-control code %d in town T%02d shop %d" report + skip rule.
* **Escalation**: if (b) < 98% or (d) < 52/52 after two attempts → re-run on a stronger tier with a
  mandate to re-read `0023ad00.c` end-to-end for ops 0x08/0x0E/0x18/0x1B (the only ambiguous widths).
  Lane: openai gpt-5.3-codex-spark → escalate gpt-6.1-sol.

### W2-02 — Mode / application state machine (title → menu → town → race → results, + attract)
* **Objective**: pure, table-driven state machine (no SDL/GPU) that replaces `enum AppState{TITLE,MENU,PLAYING}`
  + `SessionState{EXPLORE,RACE,RESULTS}` glue in `main.cpp` (declared at `main.cpp:1301`, transitions ~2001–2047):
  states BOOT, TITLE, ATTRACT, MENU(Adventure/Quick Race/2P/Options/Results), ADVENTURE-TOWN, RACE, RESULTS,
  SHOP/DIALOGUE overlay; each state exports the *guest-equivalent words* so oracle parity is testable.
* **Evidence**: root loop `0x200528` + walker `0x204F98` (`frame-timing.md`); `SelectScene3dHandler 0x21CE08`
  (handlers `0x21CE48` race / `0x21CF80` town); `UpdateTownExplorationTask 0x2105B8`; `UpdateMenuScreenTask
  0x20A580`; harness words `game_mode 0x33590A` (0 Adventure, 1 Quick Race, 8 attract), `scene3d_handler
  0x3356D8`, `drive_flags 0x335928`, `location 0x335923`, scene word = progress+0x538 (6 factory, 2 town
  driving, 10 racing, 3 race over), all from `oracle-golden-frames.md`; attract = 45 s idle, `game_mode=8`,
  `drive_flags=0x10104`.
* **Owns**: `src/native/gameplay/app_flow.{h,cpp}`, `tests/native_w2/app_flow_test.cpp`,
  `docs/native-reference/app-flow.md`, report.
* **Acceptance** (`w2_app_flow_test`): `W2-02 SCORE golden_words=9/9 edges=<n>/<n> illegal_accepted=0 replay_hash_equal=1`
  — (a) for the 9 oracle goldens, driving the machine with the documented input scripts (title: 12 s idle;
  main_menu: +Start; options: Down×3+Cross; attract: 45 s idle; race_hud: Quick Race path; factory/town: Continue
  path) yields exactly the `(game_mode, scene, handler, drive_flags&mask, location)` tuples in
  `oracle-golden-frames.md` (**9/9**); (b) every documented edge has a positive test and every illegal
  (state,event) pair is rejected (0 accepted); (c) 2 replays of a 5000-event fuzz trace give identical hashes.
* **Coordinator integration step (main.cpp)**: swap `AppState`/`SessionState` plumbing for `AppFlow`.
* **Escalation**: if (a) < 9/9 → bigger model re-derives the missing words from the cited task bodies
  (`2105B8`, `20A580`). Lane: anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-03 — Driving dynamics fidelity (kinematics from the real vehicle tick)
* **Objective**: replace the invented constants in `car_physics.cpp` (`kAccel 40`, `kSpeedMax 20000`, …)
  with an ELF-derived fixed-point vehicle model; calibrate the position-unit ↔ speed relation **from a
  per-frame oracle trace** (row 2d), not from the 158.4-unit chord alone.
* **Phase 0 (deliverable, bounded)**: capture a per-frame oracle trace over the same 731-frame hold-gas
  window as `race_hud → race_hud_driving` (pos x/y/z, velocity words at car+0xF0, dial digits sampled every
  30 frames) via the test-socket; store ≤ 800 numeric rows in `tests/native_w2/data/vehicle_trace.csv`
  (telemetry numbers, not ROM bytes).
* **Evidence**: `UpdateVehiclePhysics 0x21BA70` (master tick), gravity `−0x59` @0x21BD20, nitro cap `0xCF69`
  @0x21BE68/0x21BE80, position increment `velocity<<4 / 0x19` @0x21C308..0x21C35C (`frame-timing.md`,
  `rules-tables.md` §5.1); `GetSpeedFromF15Vector 0x21D358` (×216.0), `CalculateSpeedometerValue 0x249458`
  (÷`DAT_00332F44`=1.6089999676, clamp 186) **[R decomp]**; `AdvanceVehicleMotion 0x218EE0`,
  `UpdateSteeringDamping 0x21A088` (±512), `UpdateSuspensionFlex 0x21ACD0`, `LoadCarPerformanceSpecs
  0x2180C0` (part tables `rules-tables.md` §5.2), `AcquireVehicleInput 0x21A990`; anchors in §0 rows 2a–2d.
* **Owns**: `src/native/gameplay/vehicle_model.{h,cpp}`, `tests/native_w2/vehicle_model_test.cpp`,
  `tests/native_w2/data/vehicle_trace.csv`, `docs/native-reference/vehicle-model.md`, report.
* **Acceptance**: `W2-03 SCORE speed_rms_mph=<≤2.0> end_mph=63±1 chord_err_pct=<≤2.0> mph_cells_exact=<≥0.95> unit_scale=<pinned> deterministic=1`
  — feeding the recorded gas/steer script to `vehicle_model` reproduces the trace: RMS speed error ≤ 2.0 mph,
  final dial 63 ±1, 731-frame chord 158.457 ±2% (3.2 units), ≥95% of sampled dial integers exact, the
  speed→dial algebra (`floor(len·216/1.609)`, cap 186) exact on 100% of sampled velocity words. The worker
  must publish the pinned `units-per-(raw speed·frame)` and explain row 2d.
* **Escalation**: RMS > 2.0 after 3 iterations → stronger tier with the failure plot of residual vs frame
  (likely missing drag/gear-torque term in `0x21BA70`). Lane: openai gpt-5.6-terra-fast → gpt-6.1-sol.

### W2-04 — World geometry fidelity (terrain height, surface, tile algebra at the car)
* **Objective**: `terrain_query` — height/surface-material/tile-id at (x,z) over the loaded FLD tile
  meshes using the proven fixed-point world algebra, so car grounding stops being render-guess.
* **Evidence**: `world-geography.md` (scale `S=20971.51953125` @`0x332C0C`, tile `0x02000000`, torus
  `0x10000000`, half-column shear), `scene-tile-map.md` (Peach Town = tile 43 / `FLD/223.BIN`),
  `ComputeGridCellIndex 0x21C8D0` (id formula in `functions.tsv`), mesh queries `FindWorldMeshIntersection
  0x207280`, `IntersectPointWithMesh 0x206F28`, `SelectMeshIntersectionRoutine 0x2080D8`,
  `ResolveGroundContact 0x21B3D0`, `QueryCarSurfaces 0x219ED8` (`functions/collision-terrain.md`);
  existing converter `assets/field_converter.h`, `gameplay/world_layout.h`, `town-road-grounding.md`.
  Oracle ground truth (`oracle-golden-frames.md`): town (506.283, **30.000**, 442.230) and (518.557,
  **30.098**, 481.538); race (616.684, **1.100**, 568.756) and (747.502, **1.100**, 479.340).
* **Owns**: `src/native/gameplay/terrain_query.{h,cpp}`, `tests/native_w2/terrain_query_test.cpp`,
  `docs/native-reference/terrain-query.md`, report.
* **Acceptance** (needs the user's disc; exit 2 otherwise): `W2-04 SCORE oracle_y_err_max=<≤0.05> tile_id_ok=<n>/<n> coverage=<≥0.99> fallback_hits=<pinned> deterministic=1`
  — `heightAt` at the 4 oracle poses within **0.05** units of 30.000 / 30.098 / 1.100 / 1.100;
  `tileIdAt` equals the closed-form id for 1000 random torus points (exact); ≥99% of 10 000 seeded points
  inside the Peach Town tile resolve to a finite height (documented fallback for the rest).
* **Escalation**: y-error > 0.05 → bigger model to resolve the y unit/offset (render ×0.1 vs fixed-point
  `<<4/0x19`). Lane: zai-coding-plan glm-5.3-flash → glm-5.3.

### W2-05 — Race scoring, laps, standings, purses, licence
* **Objective**: `race_rules` — lap/split recording, best-time bookkeeping, `GetRaceTimeRank` thresholds,
  payout ladder, licence award, WGP 7-round/8-driver standings and the result-slot remap, as pure rules
  over `SaveData` (supersedes the ad-hoc award code in `session.cpp` at integration).
* **Evidence**: `RecordLapSplitTimes 0x22D390` (SE 0x2E/0x2F on new bests), `RankCarsByProgress 0x22D7B8`
  (already in `race.cpp`), `GetRaceTimeRank 0x249670`, `UpdateRaceRecords 0x237C48` (per-player bands of 26
  slots ×3 = 78; ids 26/27→24/25), payout ladder `0x29FCB8` u32[5][6], race records `0x2BDDD0` (26×0x10,
  tier byte +3), `SumPlayerPoints 0x236238`, `EnterLicenseScreen 0x237508`, `DrawLicenseResultsScreen
  0x2364B0`, WGP `0x236858` (RAM strip `0x1782A74`, 7 rounds), result channel `GetOrSetPresidentRaceResult
  0x23D8C8` (`minigame-logic.md`); `rules-tables.md` §1.2–1.4; save arrays `0xFF0/0x1040/0x1178`.
* **Owns**: `src/native/gameplay/race_rules.{h,cpp}`, `tests/native_w2/race_rules_test.cpp`,
  `docs/native-reference/race-rules.md`, report.
* **Acceptance**: `W2-05 SCORE payout=30/30 licence_vectors=9/9 records=24+2 wgp_fuzz=1000/1000 rank_fuzz=1000/1000 bands=78/78 deterministic=1`
  — all 30 ladder cells equal the ELF at `0x29FCB8`; licence truth table (3 tiers × {all top-6, one 7th,
  one not-raced}) = +1 only when all tier races have place ≤ 5; record table has 24 licence races
  (tiers 0..2) + 2 WGP (courses 17, 15, tier 4); standings sort matches an independent reference
  implementation (python script in the report) on 1000 seeded fuzz cases; save write/read keeps 3×26 = 78 rows.
* **Escalation**: licence/WGP mismatch → stronger tier with `236858.c`/`237508.c` decomp. Lane: zai glm-5.3-flash → glm-5.3.

### W2-06 — Shop economy (purse funnel, price tables, sell/bank/mileage/slot)
* **Objective**: `economy` — the single money funnel and all 13 documented mutation sites as named
  operations, plus ELF-backed per-category price tables (`price_tables` reader).
* **Evidence**: `SpendTimerBudget 0x23DC58` (spend>money → −1; cap 999 999; negative = credit; SFX 0x305 iff
  amount≠0), seed 1000 @`0x229FFC/0x22A004`, 13 sites (`rules-tables.md` §1.4: `0x261934`, `0x2692B4`,
  `0x2697E4`, `0x269AFC`, `0x256584`, `0x23A850`, `0x269244/0x269284`, `0x2366F0`, `0x236C84`, `0x23A220`,
  `0x23A7E0`, `0x262288`), selector `0x2DC8A8`, price-getter table `0x2F1280` (15 ptrs, idx 0 = flat 500),
  labels `0x2F1240`, sell = price/2 (`0x269284`), 2P transfer `0x268E40`, mileage exchange
  `ShowPrizeScreen 0x23A140` (÷0x649 = 1609 m/mile), bank `HandleAmountAdjuster 0x23A460`, slot
  `0x261638/0x261DE0`, part tables `0x2180C0`; script money gate (op 0x18) in W2-01. Existing
  `shop.cpp`/`rules_tables.cpp` stay untouched.
* **Owns**: `src/native/gameplay/economy.{h,cpp}`, `src/native/assets/price_tables.{h,cpp}`,
  `tests/native_w2/economy_test.cpp`, `docs/native-reference/economy.md`, report.
* **Acceptance**: `W2-06 SCORE funnel_vectors=24/24 sites=13/13 priced=<n>/<n> sell_floor_ok=100% bank_conserve=1000/1000 deterministic=1`
  — 24 funnel vectors (insufficient, exact, cap, credit, zero-amount no SFX); all 13 sites exist as named
  ops and are exercised; every catalog item in categories 1..13 has an ELF-read price (count pinned by
  worker, coverage must be 100%); `sell == floor(price/2)` for every item; 1000 random deposit/withdraw
  sequences conserve `purse + bank`.
* **Escalation**: unpriced items → stronger tier to decode the per-family `GetIntField_*` getters. Lane: opencode
  free pool deepseek-v4.1-flash-free (if up) else anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-07 — Dialogue & text engine (escape codes, wrap, paging, message window)
* **Objective**: `text_script` — `$X` escape expansion, `$N` name substitution, glyph-advance word wrap,
  page/cursor state of the message window, bounded copy; consumes byte spans from W2-01 (no dependency
  at build time).
* **Evidence**: `RenderEscapeText 0x23A8E8` (money/time/course codes), `SubstituteNameTokens 0x264CB0`,
  `CopyDialogueTextBounded 0x24E7D8`, `ExtractDelimitedToken 0x24EA00`, page flow `ShowSequencedMessages
  0x239748` / `DisplayMenuMessage 0x239260` / `dialogueTask 0x23B168` ("next msg #%d", states 0..6) **[R]**,
  glyph advances `uiGlyphWidths 0x29A880` (8..16 px, 20 JP), 12-px line pitch, FONTE atlas (`glyph_atlas.h`),
  message window `(32,328,576,104)`, next-arrow `(304,392)` (`hud-layout.md` §1.4), `ui-parity.md` (184 string
  templates, FONTE punctuation substitutions).
* **Owns**: `src/native/gameplay/text_script.{h,cpp}`, `tests/native_w2/text_script_test.cpp`,
  `docs/native-reference/text-script.md`, report.
* **Acceptance**: `W2-07 SCORE advance_table=<n>/<n> wrap_oracle_within2px=<≥0.90> escapes=<n>/<n> bounded_copy_fuzz=10000/10000 deterministic=1`
  — advance table read from the ELF equals the module's; for ≥ 8 strings visible in `main_menu.png`,
  `options.png`, `factory_menu.png` (list in the doc) the predicted pixel width is within ±2 UI px of the
  measured oracle extent for ≥ 90%; every documented `$` code expands per decomp vectors; bounded copy never
  overruns and keeps 2-byte escapes whole over 10 000 fuzz cases.
* **Escalation**: width score < 0.9 → bigger model to re-measure with the per-scene Y/X mapping from W2-14
  (§0 row 6). Lane: zai-coding-plan glm-5.3-flash → glm-5.3.

### W2-08 — Save format v2 (native container around the 0x3448 block)
* **Objective**: versioned, integrity-checked native save container that wraps the unchanged 13 384-byte
  progress block (PS2 files have no checksum/version, `save-layout.md`), adds a TLV block for native-only
  settings (volumes, rumble, HD pack, widescreen policy — options offsets are OPEN in the ELF), supports
  atomic write, v1 (raw 0x3448) → v2 migration, and keeps lossless P2 import (existing `migrateFromP2Image`).
* **Evidence**: `save-layout.md` (block `0x3448`, money 0x654, remap 0x12E8, no checksum verdict, migration
  spec), card I/O `0x228510` read / `0x228708` write / `0x2283E0` short-read check, new-game init
  `0x2277B0`/`0x229FD8`/`0x229F20`, `save.h` (`SaveData`, `serializeSave`, `parseSave`, `P2` import),
  `memcard.h` (2 slots). `mc0/` `mc1/` in the repo root are empty → tests are synthetic only.
* **Owns**: `src/native/gameplay/save_v2.{h,cpp}`, `tests/native_w2/save_v2_test.cpp`,
  `docs/native-reference/save-v2.md`, report.
* **Acceptance**: `W2-08 SCORE roundtrip_identical=6/6 bitflip_detect=<n>/<n>=100% truncate_detect=100% v1_migrate=6/6 tlv_unknown_preserved=1 atomic_interrupt_safe=200/200 deterministic=1`
  — container `{magic 'RTN2', u16 version, u32 payload size, CRC32, payload, TLV}`; 6 synthetic saves
  (new-game, mid-game, full-stamps, max-money, longest names, all-0xFF results) round-trip byte-identical;
  every single-bit flip over 10 000 sampled positions is rejected; every truncation is rejected; v1 images
  migrate losslessly; unknown TLV tags survive re-save; 200 simulated mid-write interruptions never leave a
  loadable-but-corrupt slot (old file or new file only).
* **Escalation**: low. If atomic-write test flakes on macOS rename semantics → stronger tier. Lane:
  anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-09 — Audio runtime (VAG streams + SFX routing + crossfade)
* **Objective**: layer above `audio/engine`: (1) **streaming VAG decode** for the 60-minute 12 kHz L/R
  radio pairs with bounded RAM (no whole-file decode), seek and loop; (2) **SFX routing** from the
  ELF-proven id set to voices (and a best-effort id→(bank,track) table recovered from `SNDMOD.IRX`);
  (3) **crossfade controller** reproducing the driver's per-channel fade (cmd 0x26) + BGM swap (0xF/0xE).
* **Evidence**: `audio-map.md` (VAG pairs, file table `0x29ADF0` 29×12 B, `gs_music_play_bgm 0x258EC8`,
  id = `0x0A II PP 01`, race-BGM table `0x29F5B0`, bank registration `0x256AF8`), `PlayUiSound 0x258E50` →
  `SndRpcPlayOrStop 0x258308` (cmd 0xD/1), fade `SoundRpcCmd26 0x2588B8`, volume `SoundRpcSetVolume
  0x2585F0`, `SoundInitVolumes 0x258578` (0x1CCC), `SetVolumeDgaRegs 0x2583F0`, tyre `0x217C68` (id =
  table@`0x29CBA8` + surface bit, stop = id+0x8000), horn/boost `0x217F10` (0x10/0x11, 0x8010/0x8011),
  `0x22C630` (0x8035), UI ids list (`audio-map.md` §6), radio `0x2590B0`/`0x2137B0`, `radio-stations.md`,
  `sndmod-irx.md`, `tsq-tvb-format.md`; existing `vag.h`, `engine.h`, `bgm_player.h`.
  `audio-inventory.md`'s claim "no SE assets in ROM" is a *non-finding*: SE come from TVB banks.
* **Owns**: `src/native/audio/audio_runtime.{h,cpp}`, `src/native/audio/sfx_table.{h,cpp}`,
  `src/native/audio/vag_stream.{h,cpp}`, `tests/native_w2/audio_runtime_test.cpp`,
  `tests/native_w2/tools/vag_ref.py` (independent ADPCM reference), `docs/native-reference/audio-runtime.md`, report.
* **Acceptance**: `W2-09 SCORE vag_ref_maxdiff=0 stream_rss_kb=<≤4096> seek_ms=<≤50> xfade_peak_step=<≤0.02> xfade_len_exact=1 sfx_ids=<n>/<n> sfx_mapped=<≥0.80> deterministic=1`
  — first 10 s (120 000 samples) of `1CH_L.VAG` decoded by the module equals `vag_ref.py` output with max
  abs diff **0**; streamer resident memory ≤ 4 MiB for the full 3603 s file; seek to t = 3500 s ≤ 50 ms;
  crossfade between two synthetic sources: exact frame length, no inter-sample step > 0.02 FS, gains sum
  to the documented ramp; all 29 UI ids + vehicle ids route to a voice (100%) and ≥ 80% resolve to a
  recovered (bank,track), the rest explicitly flagged `OPEN` (never silently dropped).
* **Escalation**: `sfx_mapped < 0.8` → stronger tier for the SNDMOD cmd-0xD handler analysis (the only
  OPEN item, `audio-map.md` §7). Lane: openai gpt-5.6-terra-fast → gpt-6.1-sol.

### W2-10 — HUD live data (speed digits, needle, timer, lap/position)
* **Objective**: `hud_live` — `HudLiveInput{speedRaw, gear, lap, rank, totalFrames, countdown…}` →
  ordered draw list `{atlas page, u,v,w,h, x,y, CSA}` in **448-space ELF coordinates**, plus a software
  rasterizer tool for oracle ROI diffs. Includes the recalibrated needle (row 2c) and the race timer
  formatter (row 2e).
* **Evidence**: drive HUD `0x24E1A0` (anchor 1P (492,336) @`0x2A9AF0`, tex table `0x2A9B08`, speed digits
  20×26 u=d·20 v=102 right-aligned at anchor+{78,59,40}, +49; gear bar; odometer) and race HUD builders
  (`raceHudTask 0x22DDD0`, `raceHudDraw 0x233860`, `raceHudElementC 0x22F750` **[R decomp]**, ElementA
  `0x22EE70` type 2 rotated quad `(value+1)%60` folded, atlas `hudTexTable 0x29F5F8`, templates
  `0x2EBD00..0x2EC040`) — `hud-layout.md` §1.1–1.2, `race-hud-atlas.md`; speed: `CalculateSpeedometerValue
  0x249458`, `CalculateSpeedometerDigits 0x2494E8`, `IntToPackedBCD 0x24F4A0`, `CountDecimalDigits 0x24F4E0`;
  art decode `assets/hud_art.h`. Oracle readings: speed 39 (`race_hud`) and 63 (`race_hud_driving`), time
  00'40"76 / 00'52"95, digit cells at shot x = 1064/1102/1140 (`gauge-calibration.md`, X values only).
* **Phase 0 (deliverable)**: capture ≥ 2 more oracle frames at low speed (≈ 0–25 mph) so the needle fit has
  ≥ 4 points (39, 63 + two new); store derived angles in the doc (not the images).
* **Owns**: `src/native/gameplay/hud_live.{h,cpp}`, `src/native/tools/w2_hud_raster_main.cpp`,
  `tests/native_w2/hud_live_test.cpp`, `tests/native_w2/hud_oracle_diff.py`,
  `docs/native-reference/hud-live.md`, report.
* **Acceptance**: `W2-10 SCORE rect_err_px=0 timer_vectors=14/14 timer_oracle=2/2 speed_oracle=2/2 needle_resid_deg_max=<≤2.0> roi_mad=<≤20> roi_hist=<≥0.85> deterministic=1`
  — draw-list rects equal the ELF/`hud-layout.md` numbers exactly (digit x = anchor+{40,59,78}, y = anchor+49,
  gear/odometer/dial rects); timer formatter: 12 boundary vectors (0, 59, 60, 3599, 3600, 35999, 36000,
  359999, 360000, 0x7FFFFFFF, …) + **2/2 oracle readouts** using `frames−1` (4076/5295 cs); speed formatter
  reproduces 39 and 63; needle angle fit over ≥4 points with max residual ≤ 2.0°; rasterized dial+digits
  vs the oracle crop (ROI ≈ shot x 1056..1188, y 380..415, measured in shot px, no 480-line mapping) mean-abs-
  diff ≤ 20/255 and 32-bin histogram intersection ≥ 0.85.
* **Coordinator integration step (main.cpp ~2440–2497)**: replace the hand-placed METER0/METER10 quads and
  the provisional `/100` divisor with `hud_live` output.
* **Escalation**: needle residual > 2° or ROI MAD > 20 → stronger tier with the 4-point angle table and the
  ElementA type-2 fold decomp. Lane: anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-11 — Minimap top-down projection (town + race HUD map)
* **Objective**: `minimap` — exact view-block math, scissor windows, marker set and clipping for the HUD
  map (north-up town, placement-entry race map), independent of the second-screen `map_view`.
* **Evidence**: `docs/native-map.md` §1 (town: `fx = zoom.x0+1728 − px·s/200`, `fy = zoom.y0+1936 +
  pz·s/200·0.47`; zoom table `0x2A42C0` mode 0 `{302,89,s50}` / mode 1 `{202,136,s25}`; car at field px
  (102,183)), `mapViewMatrix 0x21F920`, `townMapTask 0x242FE8`, bezel `0x249208` (30,304,144,120), scissors
  `0x2A43F0` (45..158 × 156..207 half-lines), `0x2A4410`, `0x2A4430`, race placements `raceMapPlacements
  0x2A4050` (24 × {x,y,scale,rot}), state packet `0x2A43B0` (TEST_2 0x3380B), arrow chain `0x290870`, dot
  `0x290940`, colour sets `0x2A4330/40/50/70/90/A0`, coin table `0x2A3970` (100 entries); `hud-layout.md` §1.3.
* **Owns**: `src/native/gameplay/minimap.{h,cpp}`, `tests/native_w2/minimap_test.cpp`,
  `tests/native_w2/minimap_oracle_shift.py`, `docs/native-reference/minimap-projection.md`, report.
* **Acceptance**: `W2-11 SCORE car_px=(102,183) clip_ok=100% oracle_dx=<−6.14±1.5> oracle_dy=<8.6..9.2±1.0> mapping_verdict=<448|480> deterministic=1`
  — the player lands at field px (102,183) for 1000 random poses (exact); every point outside the scissor is
  clipped, inside never; **oracle shift test**: phase-correlate the minimap crops of `town_driving.png` and
  `town_driving_street.png` (car Δ = (+12.274, +39.308) world units) — predicted feature shift =
  (−12.274·50/200, +39.308·0.47·50/200) field px = (−3.07, +4.62) → ×2 UI = (−6.14, +9.24) 448-space →
  shot px (−6.14, +8.60 under the 480 mapping / +9.22 under ≈448 mapping). The measured shift must match
  within ±1.5 px (x) and the y result **decides** which mapping holds (§0 row 6) — publish it.
* **Escalation**: shift mismatch > 1.5 px → bigger model to re-derive the `0.47`/`s/200` constants and the
  `+1728/+1936` offsets from `mapViewMatrix 0x21F920`. Lane: zai-coding-plan glm-5.3 → huggingface (reserve, if credits) / glm-5.3#max.

### W2-12 — Input mapping (device → PS2 action masks, remap, menu nav)
* **Objective**: `input_map` — SDL-agnostic mapper from keyboard/gamepad/touch events to the original
  per-port action masks (10 driving actions in `action_map` order: gas, brake, reverse, jet, wing up/down,
  horn, view, lights(=horn), navigator) and DualShock button bits, with remap persistence and menu-nav
  (confirm/cancel/up/down/L/R) semantics, deterministic last-write-wins per tick.
* **Evidence**: `action_map 0x1780A48` (+0 gas … +0x12 navigator; `config/game_state.toml` [controls]),
  save remap `0x12E8` (10×u16, defaults written by `FUN_00229F20` at `0x12E8..0x12FA`, `save-layout.md`),
  `UpdateControllerInput 0x20AB10`, `ReadPadState 0x20A8A8`, `UpdateMenuInputState 0x20A660`,
  `AcquireVehicleInput 0x21A990`, spinner `HandleNumberSpinner 0x23A320` (up/down = 0x4000/0x1000), race
  placement-editor bit table (0x2000/0x8000 x, 0x1000/0x4000 y — `native-map.md` §1.1);
  existing `input.h`/`menu.h` (read-only) and `tests/input_test.cpp`.
* **Owns**: `src/native/gameplay/input_map.{h,cpp}`, `tests/native_w2/input_map_test.cpp`,
  `docs/native-reference/input-map.md`, report.
* **Acceptance**: `W2-12 SCORE default_remap=10/10 conflicts=0 bits_mapped=16/16 roundtrip_save=1000/1000 order_deterministic=1`
  — default masks equal the ELF immediates at `0x229F20` (10/10); keyboard and gamepad profiles each map all
  16 pad bits with 0 conflicts; 1000 random remaps survive `SaveData.controllerRemap` round-trip; 10 000
  shuffled same-tick event orderings produce one canonical mask.
* **Coordinator integration step (main.cpp)**: route SDL events through `input_map`.
* **Escalation**: default-remap mismatch → stronger tier to read `0x229F20` instruction stream. Lane:
  anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-13 — Enhancement retention (feature registry + regression gate)
* **Objective**: make every mandated enhancement a registered, flag-addressable, automatically-gated
  feature so rebuild waves cannot silently drop it: widescreen, HD packs, second screen, frame generation,
  rumble, radio, BGM, analog triggers, save states, original-save migration, HUD, traffic AI, chase camera,
  economy, world streaming, task scheduler, mini-games (**17 ledger rows**, `feature-retention.md`).
* **Evidence**: `feature-retention.md` (statuses + matrix mapping; matrix = 23/23 PASS), `output-modes.md`,
  `hd-texture-packs.md`, `rumble-haptics.md`, `radio-stations.md`, `frame-interpolation.md`,
  `frame-timing.md` (logic locked 60 Hz ⇒ interpolation only for >60 Hz visuals), `perf-overlay.md`;
  matrix script `scripts/rt_feature_matrix.sh` (**read-only here: it has uncommitted edits in the tree**).
* **Owns**: `src/native/gameplay/enhancements.{h,cpp}` (registry: id, env flag, default, persisted key,
  owning module), `scripts/w2_retention_gate.py`, `tests/native_w2/enhancements_test.cpp`,
  `docs/native-reference/enhancement-retention-gate.md` (coordinator merges into `feature-retention.md`), report.
* **Acceptance**: `W2-13 SCORE registry=17/17 flags_resolve=17/17 matrix=23/23 determinism_pair=PASS new_regressions=0`
  — each ledger row has a registry entry whose env flag/setting key exists in the sources (`rg`-verified by
  the script); gate runs the feature matrix (must stay 23/23 PASS) and a determinism pair (two `rt-app`
  `RT_APP_SNAPSHOT` runs, byte-identical PPM); a deliberately removed flag makes the gate fail (self-test).
* **Coordinator integration step**: add gate to the green bar next to the 73 native tests.
* **Escalation**: matrix < 23/23 is a *regression report*, not a worker failure → coordinator bisects.
  Lane: opencode-go cheap tier (if revived) else anthropic claude-haiku-5-5 → claude-sonnet-5-5.

### W2-14 — Oracle score harness v2 (what the other batches are measured against)
* **Objective**: replace histogram-intersection-only steering with a region-aware, per-scene-calibrated
  score so wave-2 integration can claim real progress: for title/menu/town/race compute per-ROI
  mean-abs-diff, 8×8 block SSIM and edge-density ratio on the 320×180 canvas **and** at full resolution
  for named ROIs (sky, ground, HUD-dial, HUD-timer, minimap, text), plus a **per-scene Y/X calibration table**
  (scale, offset) fitted from ≥ 3 landmark pairs per scene (§0 row 6).
* **Evidence**: `frame-compare.md`, `scripts/frame_compare.py`, `oracle-golden-frames.md`,
  `sky-ground-calibration.md`, `menu-calibration.md`, `town-road-grounding.md`, `camera-framing.md`,
  snapshots `build/scratch/gamelook/*-v13.ppm`; baselines reproduced in §0 row 5.
* **Owns**: `scripts/w2_score.py`, `tests/native_w2/score_selftest.py`,
  `docs/native-reference/score-harness.md`, report.
* **Acceptance** (`python3 tests/native_w2/score_selftest.py`): `W2-14 SCORE baseline_hist=0.872/0.460/0.532/0.374 (±0.001) region_scores=<n> ymap_fits=<n>/<n> self_noise=<≤0.01> oracle_vs_oracle=1.000`
  — reproduces the four v13 histogram baselines; oracle vs itself = 1.000 on every metric; oracle vs a
  1-px-shifted copy of itself drops block-SSIM measurably (sensitivity test); metric noise across
  re-runs ≤ 0.01; per-scene mapping table pinned with fit residual ≤ 1.0 px.
* **Escalation**: low. Lane: openai gpt-5.3-codex-spark or anthropic claude-haiku-5-5 → claude-sonnet-5-5.

---------------------------------------------------------------------------------------------------
## 3. Conflict matrix (zero overlap by construction)

| Batch | New source files | New test / tool / script | New doc |
|---|---|---|---|
| 01 | assets/script_image.*, gameplay/script_vm.* | tests/native_w2/script_vm_test.cpp | script-vm.md |
| 02 | gameplay/app_flow.* | tests/native_w2/app_flow_test.cpp | app-flow.md |
| 03 | gameplay/vehicle_model.* | …/vehicle_model_test.cpp, tests/native_w2/data/vehicle_trace.csv | vehicle-model.md |
| 04 | gameplay/terrain_query.* | …/terrain_query_test.cpp | terrain-query.md |
| 05 | gameplay/race_rules.* | …/race_rules_test.cpp | race-rules.md |
| 06 | gameplay/economy.*, assets/price_tables.* | …/economy_test.cpp | economy.md |
| 07 | gameplay/text_script.* | …/text_script_test.cpp | text-script.md |
| 08 | gameplay/save_v2.* | …/save_v2_test.cpp | save-v2.md |
| 09 | audio/audio_runtime.*, audio/sfx_table.*, audio/vag_stream.* | …/audio_runtime_test.cpp, tests/native_w2/tools/vag_ref.py | audio-runtime.md |
| 10 | gameplay/hud_live.*, tools/w2_hud_raster_main.cpp | …/hud_live_test.cpp, hud_oracle_diff.py | hud-live.md |
| 11 | gameplay/minimap.* | …/minimap_test.cpp, minimap_oracle_shift.py | minimap-projection.md |
| 12 | gameplay/input_map.* | …/input_map_test.cpp | input-map.md |
| 13 | gameplay/enhancements.* | …/enhancements_test.cpp, scripts/w2_retention_gate.py | enhancement-retention-gate.md |
| 14 | — | scripts/w2_score.py, tests/native_w2/score_selftest.py | score-harness.md |

All paths unique; no batch edits an existing file; no worker owns `main.cpp` or a `CMakeLists.txt`.
Cross-batch references are by documented interface only (W2-01 `IScriptHost` ↔ W2-05/06 later; W2-10 owns
the timer formatter so W2-05 stays pure rules; W2-07 consumes byte spans, not W2-01 types).

## 4. Run order and coordinator integration (sequential, after workers land)

Parallel: all 14 can dispatch at once after Step 0 (≤ 2 oracle runs concurrently: W2-03, W2-10, W2-11 need
oracle captures — serialize them behind `rt-gpu.lock`). Suggested integration order (each a separate
coordinator commit, green bar after each: build + 73 native tests + feature matrix + determinism pair + the
new `w2_*_test` binaries):
1. **I-0** Step 0 CMake staging; **W2-14** score harness first (so later steps are measured with it).
2. **I-1** W2-02 `app_flow` into `main.cpp` (state plumbing only; no visual change; score unchanged ±0).
3. **I-2** W2-10 `hud_live` into `main.cpp` race/drive HUD (expect race ROI improvement, race hist ≥ 0.42).
4. **I-3** W2-11 `minimap` into `main.cpp` (town ROI; with W2-04 height query → town hist ≥ 0.55).
5. **I-4** W2-03 `vehicle_model` + W2-04 `terrain_query` into `game_loop.cpp`/`car_physics.cpp`
   (needs a wave-3 batch that *owns* those two files; not part of this wave).
6. **I-5** W2-12 `input_map`, W2-08 `save_v2`, W2-09 audio runtime, W2-13 gate into `main.cpp`.
7. **I-6** W2-01/05/06/07 bound together as the shop/dialogue/economy stack (a wave-3 "town interior"
   batch owns the glue; the VM + economy + text engine exist but have no UI consumer yet).

Exit targets for wave 2 (proposal, measured with W2-14, not the histogram alone): race HUD ROI MAD ≤ 20/255,
minimap shift ±1.5 px, town hist ≥ 0.55 (v13 0.532; v15 regressed to 0.484 — investigate before reuse),
race hist ≥ 0.45 (v13 0.374), title hist ≥ 0.87 (hold), menu hist ≥ 0.55 (needs wave-1 M1 `ui_menu` constants;
no wave-2 batch owns menu rendering — flagged gap).

## 5. Open risks / things I could not close

* **Script entry mechanics**: how `+0x21/+0x23` (shop/town) are set per door is in the spawners'
  callers (not traced); W2-01 only needs the table, W2-13/wave-3 binds it.
* **Position-unit scale** (§0 row 2d) is the largest numerical unknown; W2-03's trace is the fix.
* **Y-mapping** (§0 row 6) can silently bias every HUD/text ROI; W2-14 + W2-11's oracle shift test settle it.
* **SFX id→sample** remains driver-internal (`audio-map.md` §7); W2-09 acceptance tolerates ≤ 20% `OPEN`.
* **Menu fidelity (0.46)** has no wave-2 owner beyond in-flight M1; recommend a wave-3 batch after W2-14.
* `functions.tsv` (737 rows) is not the full name list; cite `build/decomp/fn/*.c` (1542) for ELF evidence.
