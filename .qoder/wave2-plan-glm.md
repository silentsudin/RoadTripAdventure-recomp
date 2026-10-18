# Wave 2 plan — from look-parity mock to a PLAYABLE native game

Planner: P3 (zai-coding-plan / glm-5.3), 2026-10-17. Inputs read: `.qoder/plan-journal.md`,
`.qoder/wave-board.md`, `CLAUDE.md`, `docs/native-reference/{map-view,race-hud-atlas,task-system,
sys-gsl-packs,course-routes,audio-map,hud-layout}.md`, `docs/native-reference/functions/{script-dialogue,
race-mode}.md`, `docs/game-mechanics/menus-and-save.md`, `config/game_symbols.toml` (structure),
`build/scratch/oracle-golden/manifest.json`, `src/native/{app,gameplay,assets,audio,preview,runtime}`
tree + headers.

## 0. Where we are, what wave 2 is for

Naming is 100% (1542/1542; 52 tags still soft — B1 hardens them). Look-parity is a mock:
title 0.872 CLOSE, menu 0.46, town 0.532, race 0.374 (hist-intersection, `scripts/frame_compare.py`
vs `build/scratch/oracle-golden/*.png`). 73 native tests green. What does NOT exist is *game*:
modes are env-forced (`RT_APP_STATE`), race timing is a frame counter, checkpoints/rank are authored,
town interaction (doors/dialogue/shops/photos) is absent, audio is one BGM path, menu/sky/minimap
constants are uncalibrated. Wave 2 = the systems that make the cartridge *play*: script VM, mode
flow, race rules, drive feel, economy, dialogue, saves, audio runtime, and the four wave-1
calibration applications (sky B2, menu B3, gauge B5, minimap B8).

## 0.1 Hard constraints — paste into EVERY batch prompt

1. **Zero guest execution.** Pure C++ modules only: no MIPS, no GS/VU, no IOP. The oracle
   (PS2Recomp desktop build + `build/scratch/oracle-golden/`) is evidence, never shipped logic.
2. **No ROM bytes/assets in the repo.** Constants allowed only with cited grounding (ELF vaddr
   [file offset = vaddr − 0x1FF000], golden-pixel measurement, doc path). Unverified behavior
   stays tagged `GAP` in code comments + report. Never fabricate a number.
3. **Workers: no git; no edits to `src/native/app/main.cpp`, any `CMakeLists.txt`, `third_party/`,
   `build/native/`, `.qoder/plan-journal.md`, `.qoder/wave-board.md`.** Private build dir
   `build/w-<ID>/` only. Deliver the CMake `add_executable` snippet for each new test in the
   report; the coordinator does all integration.
4. **Fixed 60 Hz VBlank logic; rendering interpolates** (`gameplay/interp.{h,cpp}` contract:
   logic steps invariant under interp-on). All new sim code ticks in whole frames.
5. Android target id `io.github.silentsudin.roadtrip.dev` ("Road Trip DEV") — modules must stay
   platform-pure (no Cocoa/Win32; Vulkan/SDL only behind existing preview layer).
6. **Every number reproducible** with a python3/PIL or struct-unpack snippet naming its inputs;
   coordinator re-runs ≥1 measurement per batch before merge.
7. End each batch with `.qoder/wave2/<ID>-report.md` (summary, files, key numbers, confidence,
   open questions, CMake snippet, proposed `config/game_symbols.toml` additions as a diff-in-report
   — never edit the toml directly).
8. Read anything; write ONLY the files listed under "owned files" for that batch.

## 0.2 Parallel-safety rule

Owned file sets below are mutually disjoint. Shared headers (`save.h`, `race.h`, `task.h`,
`session.h`, `menu.h`, `rules_tables.h`) are READ-ONLY for every batch except their owner.
If a batch needs a new field in a header it does not own, it declares the need in its report;
the coordinator applies it at integration. Symbol discoveries go to the report, never to
`config/game_symbols.toml` (B1/coordinator merge).

---

## 1. Batch summary

| ID | Goal | Tier | Depends on wave-1 |
|---|---|---|---|
| W2-01 | Event-script VM (opcode interpreter, variables, control flow) | senior-RE | B4 gsl-vm.md, B1 |
| W2-02 | Game-mode state machine + task-graph mode flow | medium | B1 |
| W2-03 | Race scoring: countdown/zones/laps/timer/rank | senior-RE | B1 |
| W2-04 | Car physics + camera calibration vs oracle kinematics | medium | B5 |
| W2-05 | Shop economy + parts inventory + prices | small-paid | B1 |
| W2-06 | Dialogue text interpreter (escape codes, pages, portraits) | medium | B4 |
| W2-07 | Native save format v2 + full round-trip | medium | B1 |
| W2-08 | Audio runtime: scene BGM table, radio stereo, SE map | medium | B6 |
| W2-09 | Sky/ground render polish module | small-paid | B2 |
| W2-10 | Minimap top-down projection + blob draw-list | small-paid | B8 |
| W2-11 | Menu/title parity constants application | small-paid | B3 |
| W2-12 | HUD live-data contract (kph divisor, timer/lap/rank formats) | small-paid | B5 |

---

## 2. Batch specifications

### W2-01 — Event-script VM core (`gsl_vm`)

- **Goal**: an authored interpreter for the town/shop event scripts the ELF runs through
  `InterpretScriptCommands` (0x23AD00, control bytes 1–0x1d) and its 25+ callee cluster — the
  system that drives doors, residents, Quick-Pic photos, shop entry, minigame entry, prizes.
- **Why now**: wave-1 B4 (`docs/native-reference/gsl-vm.md`) lands the container/opcode recon
  draft; B1 hardens the callee names (`0x23C7B0…0x23DE50`). Everything interactive in town
  (W2-05 shop flows, W2-06 dialogue pages, photo bits at save 0x508 via setter 0x23C5C0)
  hangs off this VM. **Pivot clause**: if B4 proves `.GSL` files are texture-only (per
  `sys-gsl-packs.md`, likely) and the scripts live in ELF-pointed tables, implement the
  ELF-side opcode table instead and rename the doc to `event-script-vm-impl.md`; the module
  name `gsl_vm` stays (wave-board continuity).
- **Owned files**: `src/native/gameplay/gsl_vm.{h,cpp}` (new),
  `tests/native_assets/test_gsl_vm.cpp` (new), `docs/native-reference/gsl-vm-impl.md` (new),
  `.qoder/wave2/W2-01-report.md`.
- **Required inputs**: `docs/native-reference/gsl-vm.md` (B4), `docs/native-reference/functions/
  script-dialogue.md` (opcode cluster), `docs/native-reference/functions/minigame-events.md`,
  `docs/native-reference/map-view.md` §doors/residents (photo op `0x11 n`), `config/game_symbols.toml`
  [functions] 0x23Axxx/0x23Bxxx rows (read), `build/decomp/fn/<addr>.c` via
  `scripts/ghidra/fn.py` (read-only ELF evidence), disc at
  `~/Library/Application Support/RoadTripRecomp/disc` (evidence only).
- **Steps**:
  1. Freeze an opcode table from B4 + your own `fn.py` re-reads of 0x23AD00 and ≥10 callees;
     each opcode gets: byte, operand layout, effect, evidence line (vaddr or B4 §).
  2. Model VM state: script pointer, per-event variables, message sub-state, wait-for-input
     suspension, result code to parent task (mirrors task `+0x4` result convention,
     `task-system.md`).
  3. Implement `step()` as one 60 Hz frame-tick (or immediate for pure ops) — deterministic,
     no wall clock.
  4. Suspend/resume contract for message ops that block on W2-06's dialogue (define a
     `MessageHost` interface *in gsl_vm.h*; W2-06 implements it — keeps batches link-independent).
  5. Photo-bit setter (op `0x11 n` → bit n−1 of the 16-byte bitmap, save offset 0x508,
     setter 0x23C5C0 — map-view.md PROVEN) as the first verified side-effect op.
  6. Synthetic fixtures only in tests; optional `RT_DISC`-gated parse-walk of one real script
     region (skip cleanly when absent).
- **ACCEPTANCE**: new `native_assets_gsl_vm_test` (coordinator registers from report snippet):
  (a) opcode dispatch covers every byte 1–0x1d — unknown byte ⇒ halt-with-code, never UB;
  (b) jump/conditional-jump reproduce authored traces exactly (≥12 scripted cases);
  (c) photo op sets bit n−1 and no other bit; (d) suspend/resume: a message op freezes the PC,
     host completion resumes at the next opcode, deterministic across 1000 ticks;
  (e) RT_DISC case: real script region parses to N ops with zero unknown opcodes or the test
  reports the unknown-byte histogram (GAP-tagged, not failed) — count must be reproducible
  from the report's python snippet.
- **Tier / escalation**: senior-RE — `openai/gpt-6.1-sol`; escalate to `xai/grok-4.7` if the
  opcode table has <60% ELF-verified coverage after step 1.
- **Coordinator verification**: re-run the report's snippet against the disc; spot-decompile
  3 opcodes with `python3 scripts/ghidra/fn.py <addr>` and diff semantics against gsl_vm.cpp
  comments; run `build/w-W2-01/native_assets_gsl_vm_test`.

### W2-02 — Game-mode state machine + mode flow (`mode_flow`)

- **Goal**: authored top-level flow: BOOT → TITLE (attract timeout) → MAIN_MENU
  (Adventure / Quick Race / 2P / Options / Results) → save-select → EXPLORE ⇄ RACE → RESULTS →
  EXPLORE, plus PAUSE, replacing env-forced `RT_APP_STATE`.
- **Why now**: task-system.md is complete (48-slot pool, walker semantics PROVEN) and
  `gameplay/task.{h,cpp}` already mirrors it; B1's hardened `StartMenuFlow 0x2052D0` /
  `UpdateMenuFlowTask 0x205088` / `DispatchMenuTask 0x20A2B0` notes give the real flow.
  `GameSession` (EXPLORE/RACE/RESULTS only) is too thin for playable flow; this batch supplies
  the spine the coordinator wires into `session.cpp` + `main.cpp`.
- **Owned files**: `src/native/gameplay/mode_flow.{h,cpp}` (new),
  `tests/native_assets/test_mode_flow.cpp` (new), `docs/native-reference/mode-flow.md` (new),
  `.qoder/wave2/W2-02-report.md`.
- **Required inputs**: `docs/native-reference/task-system.md`, `docs/game-mechanics/menus-and-save.md`
  (mode dispatch chain), `src/native/gameplay/task.h` (read), `src/native/gameplay/session.h` (read),
  `src/native/gameplay/menu.h` (read), `build/scratch/oracle-golden/manifest.json` (state words:
  title game_mode 0, attract game_mode 8 @ ~45 s idle, race game_mode 1, progress_scene
  2 = town / 6 = factory_menu), `docs/native-reference/oracle-golden-frames.md`.
- **Steps**:
  1. Enum the mode graph from menus-and-save.md dispatch chain; each edge cites vaddr.
  2. Implement `ModeFlow` as a task-tree client: spawn/kill mode tasks on the existing
     `TaskPool` (title task, menu task, race task 0x20F3B8-equivalent, town task 0x2105B8-equiv).
  3. Attract: title idle timeout → attract demo → any-input returns to title. Timeout constant
     GAP-tagged (oracle reached attract by 45 s; exact trigger unpinned) — pick 40 s, tag it.
  4. Pause overlay + resume/quit edges (game's own pause list semantics from CLAUDE.md
     town-Start-menu row: Warp/Notebook/Radio/Items/Settings/Map).
  5. Emit a `ModeSnapshot {state, gameMode, progressScene, location}` the facade can serve —
     must reproduce manifest.json's state words for the four scripted routes.
- **ACCEPTANCE**: new `native_assets_mode_flow_test`: (a) scripted input script
  boot→title→menu→quick-race→race→results→explore drives the exact state sequence; (b) attract
  fires at the tagged timeout and cancels on input; (c) state words match manifest.json values
  (game_mode 0/8/1, progress_scene 2/6) at the checkpoint steps — constants cited, not copied;
  (d) task-pool invariants hold across every transition (48-slot cap respected, no orphan
  children — reuse `native_assets_task_test` helpers read-only).
- **Tier / escalation**: medium — `zai-coding-plan/glm-5.3-highspeed` or
  `openai/gpt-5.6-terra-fast`; escalate to gpt-6.1-sol if the dispatch chain needs fresh ELF
  decompilation beyond B1 notes.
- **Coordinator verification**: run the new test in `build/w-W2-02/`; grep the report for
  vaddr citations and re-read 2 of them via `fn.py`; confirm no `session.cpp` edits in the diff.

### W2-03 — Race scoring engine (upgrade `race`)

- **Goal**: replace the frame-counter `RaceTracker` with the real rules: start countdown,
  checkpoint zones, lap counting with split records, live rank, total/lap timers, finish +
  eligibility, feeding the SCORE.GSL HUD cells (LAP badge, time digits, rank oval).
- **Why now**: race-mode.md + race-hud-scoreboard.md are fully mapped with B1-hardened names
  (`UpdateZoneFromRects 0x22D620` — 3-rect classification storing zone 1–4 at +0x19a;
  `RecordLapSplitTimes 0x22D390` — SE 0x2e/0x2f on new best; `RankCarsByProgress 0x22D7B8`;
  `RunCountdownTimerTask 0x22DAE8` — fade 64 ticks, SE 0x2d @60, flag @240, end @300;
  `ResetRecordTables 0x22D288` INT_MAX seed); race-hud-atlas.md pins display format
  (timer `00'52"95`, hundredths; pink 28×30 rank digits). Oracle manifest gives two timer
  anchors (race_total_time_frames 2447 @ vblank 4193; 3178 @ 4924 → exactly 1 frame/tick).
- **Owned files**: `src/native/gameplay/race.{h,cpp}` (sole owner; keep existing public API
  compatible where tests use it), `tests/native_assets/test_race.cpp`,
  `docs/native-reference/race-scoring.md` (new), `.qoder/wave2/W2-03-report.md`.
- **Required inputs**: `docs/native-reference/functions/race-mode.md`,
  `docs/native-reference/functions/race-hud-scoreboard.md`, `docs/native-reference/race-hud-atlas.md`,
  `docs/native-reference/hud-layout.md` §1.1, `docs/native-reference/course-routes.md`
  (negative result — no COURSE checkpoint record demonstrated),
  `docs/native-reference/map-view.md` §route coverage (C00: 35 segment records, PROVEN),
  `build/scratch/oracle-golden/manifest.json`, `src/native/gameplay/save.h` (read),
  `config/game_state.toml` [race.live] (read).
- **Steps**:
  1. Zone model: 3-rect classifier per 0x22D620 semantics; checkpoint route source =
     static-directory pair-0 C00 segments (map-view.md) as ordered centerline gates —
     tag the exact in-game rect-source GAP (course-routes.md says no COURSE record exists).
  2. Countdown state machine: 64/60/240/300 tick events (cited), drive-flag gating, GO release.
  3. Lap/split recording: per-category record tables seeded INT_MAX (0x22D288), new-best
     flags, lap counter advance (0x22D390).
  4. Rank: keep the proven sort key (lap desc with −1 when zone<2, zone asc, flag asc,
     progress asc — existing `rankCarsByProgress`) and re-verify against 0x22D7B8 pseudocode.
  5. Finish: `FinishOnCarApproach 0x22DC68` 110-unit rule + elimination rank bits
     (`ResetRaceRanks 0x20F330`).
  6. Timer: 1 frame = 1 tick after GO; expose `formatTimer()` producing `MM'SS"cc` per atlas.
- **ACCEPTANCE**: extended `native_assets_race_test`: (a) countdown emits SE-0x2d tick at t=60
  and GO at t=300 exactly; (b) timer ticks 1/frame post-GO (assert elapsed == vblanks − start);
  (c) zone-transition order 1→2→3→4→(lap++, reset) and out-of-order crossings rejected;
  (d) synthetic 24-car field ranks match the authored-key reference implementation (≥5
  adversarial cases: ties, inactive, zone<2); (e) new-best-lap fires only on strict improvement;
  (f) formatTimer("00'52\"95") cell mapping matches race-hud-atlas.md §3.2 dest row.
- **Tier / escalation**: senior-RE — `openai/gpt-5.6-terra-fast` first (docs are rich);
  escalate to `gpt-6.1-sol` the moment 0x22D620 rect semantics need fresh disassembly.
- **Coordinator verification**: re-derive the two timer anchors from manifest.json (2447/4193,
  3178/4924) and check the test cites them; `fn.py 0x22dae8` spot-check of the 300-tick claim;
  run the test binary from `build/w-W2-03/`.

### W2-04 — Car physics + camera calibration vs oracle kinematics

- **Goal**: make the car *drive like the game*: acceleration/brake/steer constants calibrated
  against oracle golden kinematics, plus camera modes (chase, look-back) pinned to
  camera-framing constants.
- **Why now**: B5 (`gauge-calibration.md`) pins the speed digit scale; the oracle manifest
  carries two golden kinematic pairs usable as ground truth:
  - race (Cross held 12 s): (616.684, 568.756)→(747.502, 479.340) over frames 2447→3178
    (731 frames) ⇒ 158.4 units / 731 frames ≈ 0.217 u/frame ≈ 13.0 u/s at the oracle's dial
    reading (~63) — the units→kph anchor.
  - town (Cross from standstill): (506.283, 442.230)→(518.557, 481.538) over 372 frames ⇒
    41.2 units ⇒ launch acceleration profile.
  Camera framing constants already landed (8c6fef5: back 7 RU, height 2, pitch 0.0859,
  lookAt 1.5, fov 1.03 UNDOC-flagged).
- **Owned files**: `src/native/gameplay/car_physics.{h,cpp}`,
  `src/native/gameplay/camera.{h,cpp}`, `tests/native_assets/test_car_physics.cpp`,
  `tests/native_assets/test_camera.cpp`, `docs/native-reference/car-physics-calibration.md`
  (new), `.qoder/wave2/W2-04-report.md`.
- **Required inputs**: `docs/native-reference/functions/car-physics-steering.md`,
  `docs/native-reference/functions/chase-camera.md`, `docs/native-reference/camera-framing.md`,
  `docs/native-reference/camera-constants.md`, `docs/game-mechanics/car-physics.md`,
  `docs/native-reference/gauge-calibration.md` (B5), `build/scratch/oracle-golden/manifest.json`,
  `src/native/gameplay/car_state.h`, `src/native/gameplay/geometry.h` (height sampler),
  `src/native/gameplay/input.h` (CarInput bits, read).
- **Steps**:
  1. Extract the game's accel/brake/steer/gravity constants with vaddr citations
     (gravity −0x59 @0x21BD20 already byte-verified in plan-journal).
  2. Closed-form integrate-at-60 Hz; assert distance curves within tolerance of both golden
     pairs (report the python re-derivation).
  3. Speed pipeline: internal u/frame → kph via the B5-calibrated divisor (replace the
     provisional /100 GAP in the HUD path — coordinate with W2-12's formatter).
  4. Camera: chase constants + look-back + photo-mode framing from chase-camera.md; expose
     per-frame camera pose consumed by interp (pose lerp contract).
  5. Surface coupling through geometry height sampler (asphalt/dirt/grass from car+0x1A8
     contact words — CLAUDE.md Driving notes) affecting accel, GAP-tagged where unpinned.
- **ACCEPTANCE**: extended `native_assets_car_physics_test` + `native_assets_camera_test`:
  (a) 731-frame WOT sim lands within ±2 units of (747.502, 479.340) from
  (616.684, 568.756) — else document per-axis residual and tag GAP;
  (b) 372-frame town-launch distance 41.2 ± 2 units; (c) kph conversion reproduces the
  oracle dial reading for both runs (cited B5 divisor); (d) camera pose determinism:
  same input script ⇒ byte-identical pose log (interp-on == interp-off at logic steps);
  (e) gravity constant sign/scale cited to 0x21BD20.
- **Tier / escalation**: medium — `synthetic/hf:moonshotai/Kimi-K3` or terra-fast; escalate
  to senior-RE if the accel curves need EE float-mode-aware re-derivation.
- **Coordinator verification**: recompute both kinematic anchors from manifest.json with a
  5-line python snippet (numbers must match the report's); run both test binaries.

### W2-05 — Shop economy + parts inventory

- **Goal**: real shop: price tables, stock, buy/sell, money, part ownership + equip flags,
  photo/sprint/minigame entry fees — the Adventure's economic loop.
- **Why now**: rules tables are byte-verified and committed (f3a5b78: payout ladder, clocks,
  golf/slot/speedo, entry fees, taxi fares); menu-shop-ui.md + PARTS.GSL icon inventory
  (122 units, sys-gsl-packs.md) and save-layout inventory fields exist. Shop.cpp today is a
  generic buy/sell shell with no catalog.
- **Owned files**: `src/native/gameplay/shop.{h,cpp}`, `tests/native_assets/test_shop.cpp`,
  `docs/native-reference/shop-economy.md` (new), `.qoder/wave2/W2-05-report.md`.
- **Required inputs**: `docs/native-reference/rules-tables.md`,
  `docs/native-reference/functions/menu-shop-ui.md`, `docs/native-reference/sys-gsl-packs.md`
  (PARTS.GSL), `docs/native-reference/save-layout.md`, `src/native/gameplay/rules_tables.h`
  (read), `src/native/gameplay/save.h` (read), `src/native/gameplay/minigames.h` (read).
- **Steps**:
  1. Catalog: parts/items with prices from the ELF tables behind menu-shop-ui.md (each price
     cited by vaddr); stock rules per shop kind (Parts/Body/Paint door ids from map-view.md
     residents: doors 1/2/3).
  2. Transactions: buy/sell (sell = price/2 truncating — existing), money cap kMoneyCap,
     insufficient-funds refusal, stock decrement/increment.
  3. Ownership + equip: part flags in the save layout (offsets cited from save-layout.md);
     equip validation (body/paint compatibility) GAP-tagged where the ELF rule is unread.
  4. Entry fees: minigame/Quick-Pic fees from rules_tables (read-only include).
  5. Shop UI state: rows/pages cursor model matching UiMenuWindow semantics (0x205350) for
     the coordinator's renderer glue.
- **ACCEPTANCE**: extended `native_assets_shop_test`: (a) every catalog price equals its cited
  ELF table value (coordinator re-reads 3 at the cited offsets); (b) buy→equip→save→load→
  catalog shows owned; (c) money floor/cap + odd-price sell truncation; (d) entry-fee
  deduction matches rules_tables values; (e) stock exhaustion blocks purchase.
- **Tier / escalation**: small-paid — `zai-coding-plan/glm-5.3-flash` /
  `opencode-go/qwen3.8-flash`; escalate to medium if price-table extraction needs disassembly.
- **Coordinator verification**: byte-check 3 prices at vaddr−0x1FF000 in SLUS_203.98; re-run
  the test binary.

### W2-06 — Dialogue text interpreter

- **Goal**: authored text presentation engine: escape-code expansion (`$X` money/time/name
  codes per `RenderEscapeText 0x23A8E8`, `$N` name substitution 0x264CB0), word-wrap,
  page sequencing, speaker windows, bounded copy (0x24E7D8 semantics — exists), serving
  W2-01's `MessageHost`.
- **Why now**: B4's opcode draft includes the message ops; script-dialogue.md maps the whole
  display-task family. Without this, W2-01's VM cannot show anything.
- **Owned files**: `src/native/gameplay/dialogue.{h,cpp}`, `tests/native_assets/test_dialogue.cpp`,
  `docs/native-reference/dialogue-interpreter.md` (new), `.qoder/wave2/W2-06-report.md`.
- **Required inputs**: `docs/native-reference/functions/script-dialogue.md`,
  `docs/native-reference/gsl-vm.md` (B4 — message-op interface), `docs/native-reference/
  sys-gsl-packs.md` (S02.GSL: 9× T8 220×156 dialog portraits), `src/native/gameplay/gsl_vm.h`
  (read — MessageHost interface), `src/native/gameplay/menu.h` (MenuInput, read),
  `src/native/gameplay/save.h` (read — name/money expansion sources).
- **Steps**:
  1. Escape table: `$`-codes with cited semantics (money, time, course name, player name…);
     unknown code ⇒ literal pass-through (never drop bytes — bounded-copy contract).
  2. Token substitution via a `TokenSource` interface (save supplies name/money) — no
     direct save dependency at link time.
  3. Word-wrap in the 640×448 UI space using FONTE metrics (glyph atlas pitch 16×24 —
     FONTE geometry PROVEN, 716c7cf).
  4. Page machine: advance/confirm/cancel from MenuInput; letterbox-typewriter pacing
     constant GAP-tagged (unmeasured) — default instant, tagged.
  5. Portrait binding: speaker id → S02.GSL portrait slot table (metadata only, no bytes).
  6. Implement `gsl_vm::MessageHost` — this file is the *only* place that includes gsl_vm.h
     besides tests (adapter direction: dialogue → VM, keeping W2-01 link-independent).
- **ACCEPTANCE**: extended `native_assets_dialogue_test`: (a) escape expansion snapshot
  table (≥15 codes, incl. 2-byte escapes) byte-exact; (b) wrap: no line exceeds the cited
  dialog-window width, breaks only at spaces, two-byte escapes never split;
  (c) page sequencing + input cancel; (d) MessageHost round-trip with a mock VM:
  show-message → advance → resume fires exactly once; (e) bounded copy max-length edge
  (existing tests stay green).
- **Tier / escalation**: medium — `openai/gpt-5.3-codex-spark` or glm-5.3-highspeed;
  escalate to terra-fast if the escape table needs fresh ELF reads.
- **Coordinator verification**: re-derive 3 escape-code semantics from `fn.py 0x23a8e8`
  output; run the test binary; confirm gsl_vm.h include direction.

### W2-07 — Native save format v2 + round-trip

- **Goal**: one serialized native save that carries ALL wave-2 state (race records, shop/
  parts, photo bits, house masks, settings, stamp/progress), with the original-save importer
  (fd6a17c, 188 checks) still green and a stable on-disk version the coordinator can wire
  to slots.
- **Why now**: multiple batches mutate SaveData-shaped state; without one owner, save.h
  drifts and the importer rots. Field offsets for houses/photos are already PROVEN
  (map-view.md: 0xBD8 masks, 0x508 bitmap).
- **Owned files**: `src/native/gameplay/save.{h,cpp}`, `src/native/gameplay/memcard.{h,cpp}`,
  `tests/native_assets/test_save.cpp`, `tests/native_assets/test_memcard.cpp`,
  `docs/native-reference/native-save-format.md` (new; append-only edits to
  `docs/native-reference/save-layout.md` allowed), `.qoder/wave2/W2-07-report.md`.
  **Runs LAST in the wave** (see §3) so it can freeze final field shapes.
- **Required inputs**: `docs/native-reference/save-layout.md`,
  `docs/native-reference/functions/memory-card-save.md`, `src/native/gameplay/save.h`,
  `src/native/gameplay/progression.h` (read), `src/native/gameplay/race.h` (read),
  `src/native/gameplay/shop.h` (read), `config/game_state.toml` [progress] (read).
- **Steps**:
  1. Inventory every state field the wave-2 batches read/write (from their reports) →
     field map with original-offset analogues where they exist.
  2) Versioned container: magic + version + CRC32 (reuse the 0x5A49851D toolchain pattern),
     field-by-field serialize (structs refused at compile time — archive style).
  3. Round-trip: save→load→save byte-identical; default-construct→load-v0 migration.
  4. Importer regression: the 188 original-save checks untouched and green.
  5. Card geometry limits documented (existing notes) + slot index semantics.
- **ACCEPTANCE**: extended `native_assets_save_test` + `native_assets_memcard_test`:
  (a) round-trip byte-identity across 6 scenario saves (fresh, mid-adventure, post-race,
  full-inventory, all-photos, maxed money); (b) CRC flips on single-bit corruption and load
  refuses with reason; (c) house-mask/photo-bit offsets still match 0xBD8/0x508 citations;
  (d) all pre-existing importer checks pass unchanged (no weakened assertions — diff must
  show zero deletions in test_save.cpp's legacy blocks).
- **Tier / escalation**: medium — glm-5.3-highspeed / terra-fast; escalate rarely.
- **Coordinator verification**: run both test binaries; flip one byte in a fixture save with
  the report's snippet and confirm refusal; `git diff` shows no deletions of legacy test blocks.

### W2-08 — Audio runtime (scene BGM table, radio stereo, SE map)

- **Goal**: the audio *behavior* layer on the existing engine (24 voices, TSQ sequencer,
  VAG decoder, BgmPlayer): which BGM id plays per scene/mode, radio station streaming with
  proper L/R stereo routing, SE ids on game events, fades.
- **Why now**: B6 (`audio-inventory.md`) lands the asset/module gap list; audio-map.md
  already PROVEN-maps gs_music_play_bgm call sites → SOUND/ files; the known defect list is
  concrete (radio right channel unrouted — engine.h playBgm lacks per-voice pan, cited
  engine.h:110/mixer.h:56; 1CH L/R identical so mono-safe).
- **Owned files**: ALL of `src/native/audio/` (`engine.{h,cpp}`, `mixer.{h,cpp}`,
  `vag.{h,cpp}`, `vag_loader.{h,cpp}`, `bgm_player.{h,cpp}`, `music_seq.{h,cpp}`,
  `music_library.{h,cpp}`), `tests/native_assets/test_audio.cpp`, `test_engine.cpp`,
  `test_bgm_player.cpp`, `test_music_seq.cpp`, `test_music_library.cpp`,
  `docs/native-reference/audio-runtime.md` (new), `.qoder/wave2/W2-08-report.md`.
- **Required inputs**: `docs/native-reference/audio-inventory.md` (B6),
  `docs/native-reference/audio-map.md` (call-site → id table, PROVEN),
  `docs/native-reference/tsq-tvb-format.md`, `docs/native-reference/sndmod-irx.md`,
  `docs/native-reference/functions/sound-rpc.md`,
  `docs/game-mechanics/sound-and-daynight.md`, `docs/native-reference/radio-stations.md`,
  `src/native/gameplay/radio.h` (read).
- **Steps**:
  1. Scene→BGM table: mode/location → packed BgmId (unpackBgmId exists) from audio-map.md
     call sites — every row cites its call-site vaddr.
  2. Per-voice pan: add pan to the voice/mixer path; route radio `_L`/`_R` VAG pairs as a
     stereo pair (fixes the cited mono gap); keep 1CH (identical channels) mono-safe.
  3. SE map: event → SE id (0x2d countdown tick, 0x2e/0x2f best-lap, menu confirm/cancel,
     horn) from sound-rpc.md + race-mode.md citations; expose `playSe(id)` with the
     one-shot release path.
  4. Fade semantics per SNDMOD cmd 0x26 (engine already models linear ramps) — verify
     frame counts against sndmod-irx.md, GAP-tag if unpinned.
  5. 12 kHz → device rate: deterministic resample contract (block-exact for tests).
- **ACCEPTANCE**: extended `native_assets_*audio/engine/bgm/music*_test`: (a) scene table
  asserts ≥12 rows match audio-map.md call-site ids exactly; (b) radio pair: L and R buffers
  reach distinct channels (interleave assertion on a synthetic stereo pair; 1CH asserts
  identical); (c) SE one-shots retire; fade reaches target in the cited frame count;
  (d) 60 Hz tick determinism: same script ⇒ identical 4096-frame PCM hash.
- **Tier / escalation**: medium — Kimi-K3 or terra-fast; escalate if TSQ event-stream
  semantics need RE beyond tsq-tvb-format.md.
- **Coordinator verification**: pick 3 scene-table rows, re-derive the id from the call-site
  vaddr via `fn.py`; re-run one PCM-hash test; confirm mixer pan assertion actually inverts
  (anti-tamper: temporarily swap L/R in a copy and watch it fail).

### W2-09 — Sky/ground render polish module

- **Goal**: sky gradient + ground color/fog authored as data (not ad-hoc shader hacks) from
  B2's calibration, lifting town/race look scores.
- **Why now**: B2 (`sky-ground-calibration.md`) lands golden-measured gradients; town is
  0.532 with "dark flat ground" and charcoal lattice underlay as known defects; the fog
  model (0.5..1.0 × fogFar) and time-of-day weights exist in lighting.h but uncalibrated.
- **Owned files**: `src/native/preview/sky_ground.{h,cpp}` (new),
  `src/native/preview/lighting.h` (edits; sole wave-2 owner),
  `tests/native_assets/native_preview_sky_ground_test.cpp` (new) +
  `native_preview_lighting_test.cpp` (extend), `docs/native-reference/sky-ground-impl.md`
  (new), `.qoder/wave2/W2-09-report.md`. **No preview.cpp edits** — the coordinator binds
  the module (snippet in report).
- **Required inputs**: `docs/native-reference/sky-ground-calibration.md` (B2),
  `docs/native-reference/oracle-golden-frames.md`,
  `build/scratch/oracle-golden/{town_driving,race_hud_driving,title}.png` (evidence),
  `src/native/preview/lighting.h`, `scripts/frame_compare.py`.
- **Steps**:
  1. Encode B2's measured sky ramp (elevation → RGB stops, per time-of-day band) as a
     cited constant table; ground albedo per surface class.
  2. Fog: distance ramp parameters from B2 + existing 0.5..1.0×fogFar contract; horizon
     blend that kills the charcoal lattice seam (sampled evidence).
  3. Time-of-day: clock → weight table binding (updateTimeOfDayLight 0x225988 semantics,
     weights at 0x177F720 cited).
  4. Unit test renders the ramp/fog to a small CPU raster (pure functions) — golden values
     from B2's PIL measurements.
- **ACCEPTANCE**: new `native_preview_sky_ground_test` asserting (a) ramp stop table equals
  B2's measured values (cited); (b) fog monotonic + full-cover at fogFar; (c) time-band
  selection for ≥6 clock values incl. oracle default 82950 (09:13). Plus measurable delta
  after coordinator wiring: town 0.532 → ≥0.60 and race 0.374 → ≥0.45 hist-intersection
  (`python3 scripts/frame_compare.py build/scratch/oracle-golden/town_driving.png <native
  snapshot>`) — the report must include the exact capture command.
- **Tier / escalation**: small-paid — glm-5.3-flash / DeepSeek-V4.1-Flash; escalate to
  medium if B2 is thin and fresh golden measurements are needed.
- **Coordinator verification**: re-measure 3 ramp stops from the oracle PNGs with PIL;
  re-run the score command from the report.

### W2-10 — Minimap top-down projection + blob draw-list

- **Goal**: the in-HUD live minimap as data: legacy projection (PROVEN ≤1.5 px), zoom table,
  road strips from the town graph, blob set from B8, player arrow — a `MinimapFrame`
  draw-list the preview layer renders in the scissor viewport.
- **Why now**: B8 (`minimap-blobs.md`) lands the blob extraction; hud-layout §1.3 + map-view
  legacy formula (`fx = 102 + (x−px)·s/200`, `fy = 183 − (z−pz)·0.47·s/200`, zoom table
  0x2A42C0, scissor [45,156,158,207], GS template 0x3380B) are CONFIRMED; the live street
  graph (24a6c43) supplies roads. The GAME-LOOK v13 note says the minimap is a live
  viewport, not static art — this batch makes it data-driven.
- **Owned files**: `src/native/gameplay/minimap.{h,cpp}` (new),
  `tests/native_assets/test_minimap.cpp` (new), `docs/native-reference/minimap-projection.md`
  (new), `.qoder/wave2/W2-10-report.md`.
- **Required inputs**: `docs/native-reference/minimap-blobs.md` (B8),
  `docs/native-reference/map-view.md` (legacy formula + graph),
  `docs/native-reference/hud-layout.md` §1.3, `src/native/assets/nav_routes.h` (read),
  `src/native/gameplay/map_view.h` (read), `config/game_state.toml` [map] (read).
- **Steps**:
  1. Projection functions (exact legacy formula, both directions for hit-testing later).
  2. Zoom table values from 0x2A42C0 with file-offset citations; zoom-switch rule GAP-tagged.
  3. Content: road polylines (town graph segments), blob categories from B8 (houses/shops/
     grip/dash), player arrow rotation = heading; seam-safe via map-space wrap (map-view).
  4. Scissor-space clipping of the draw list; `MinimapFrame` pure struct.
- **ACCEPTANCE**: new `native_assets_minimap_test`: (a) projection reproduces the ≤1.5 px
  legacy check set (the doc's Quick-Pic icon anchors) on synthetic inputs; (b) zoom table
  equals cited values; (c) every element clipped to scissor [45,156,158,207]; (d) player
  arrow at map centre pointing heading for 8 compass cases; (e) blob categorization counts
  match B8's extraction table.
- **Tier / escalation**: small-paid — deepseek-v4.1-flash-free is acceptable only per quota
  policy; prefer glm-5.3-flash / qwen3.8-flash.
- **Coordinator verification**: recompute 3 projection anchors by hand from the formula;
  re-count one blob category from B8's snippet; run the test.

### W2-11 — Menu/title parity constants application

- **Goal**: apply B3's region-diff audit to the authored menu/title: ink gold vs white,
  row geometry, highlight rect, panel palette, title-plate framing — closing the menu 0.46 gap.
- **Why now**: B3 (`menu-calibration.md`) lands exact fix constants; prior pins exist
  (highlight (0,110,224) rect x870..1185 y274..297; panel (48,64,144)/F2; labels (244,244,244)
  — commit eabd91e) and ui-parity.md's deviation ledger lists the known drift.
- **Owned files**: `src/native/preview/ui_menu.{h,cpp}`, `src/native/assets/title_screen.{h,cpp}`,
  `tests/native_assets/native_preview_ui_menu_test.cpp`,
  `tests/native_assets/native_title_screen_test.cpp`, `.qoder/wave2/W2-11-report.md`.
- **Required inputs**: `docs/native-reference/menu-calibration.md` (B3),
  `docs/native-reference/ui-parity.md`,
  `build/scratch/oracle-golden/{main_menu,options,title}.png`, `scripts/frame_compare.py`,
  `src/native/assets/glyph_atlas.h` (read).
- **Steps**:
  1. Diff current menu draw vs B3 constants; fix color/geometry table (all cited).
  2. Title plate + publisher line + three-car silhouette framing from B3.
  3. Focus/blink cadence if B3 pins it; otherwise GAP.
  4. Update the parity tests' pinned constants (with citations — any changed pin must cite
     B3's measurement; never weaken an assertion).
- **ACCEPTANCE**: extended `native_preview_ui_menu_test` + `native_title_screen_test`
  asserting the constant tables match B3 citations; measurable delta after wiring:
  menu 0.46 → ≥0.60 and title 0.872 → ≥0.88 (no regression) via frame_compare.py on
  main_menu.png/options.png snapshots (capture command in report).
- **Tier / escalation**: small-paid — glm-5.3-flash; escalate to medium if B3 lacks pixel
  rects and fresh PIL measurements are needed.
- **Coordinator verification**: re-measure the highlight rect and one row's ink from
  main_menu.png with PIL; re-run the score command.

### W2-12 — HUD live-data contract (kph divisor, timer/lap/rank formats)

- **Goal**: pure formatting/binding layer feeding the already-decoded SCORE.GSL/METER
  atlas cells: speed digits + needle angle from calibrated kph, timer digits `MM'SS"cc`,
  LAP badge count, rank + ordinal, gear badge deferred (snapshot has no gear field — do
  NOT fabricate).
- **Why now**: B5 pins the physics→kph divisor (killing the provisional /100 GAP from
  14b3a97); W2-03 defines the race state; the atlas dest rects are fully measured
  (race-hud-atlas.md §3, hud-layout.md §1.1/§1.2). This batch makes the HUD show *game*
  numbers instead of artwork.
- **Owned files**: `src/native/gameplay/hud_layout.{h,cpp}`,
  `tests/native_assets/test_hud_layout.cpp`, `docs/native-reference/hud-layout.md`
  (append-only §1.2/§1.3 updates), `.qoder/wave2/W2-12-report.md`.
- **Required inputs**: `docs/native-reference/gauge-calibration.md` (B5),
  `docs/native-reference/race-hud-atlas.md`, `docs/native-reference/hud-layout.md`,
  `build/scratch/oracle-golden/manifest.json` (race numbers: place 24th, timer anchors),
  `src/native/assets/hud_art.h` (read), `src/native/preview/race_hud.h` (read — coordinator
  binds; no edits).
- **Steps**:
  1. `formatSpeed(kph)` → digit cells (METER10 v=102 row, right-align anchor+{78,59,40}/+49,
     cited) + needle angle curve (B5; GAP-tag beyond pinned points).
  2. `formatTimer(frames)` → `MM'SS"cc` cells per atlas §3.2 (hundredths, `'`/`"` gaps).
  3. `formatLap(current,total)` → LAP badge 8×16 digits (u=lap*8, v=200) + 0-lap flag glyph.
  4. `formatRank(n)` → 28×30 pink digits (u=(d%9)*28, v=(d/9)*30) + ordinal suffix cells
     (st/nd/rd/th) + split-oval overlay rect.
  5. All functions take primitives — no dependency on race.h/car_physics.h at link time
     (parallel-safe); the coordinator's glue maps W2-03/W2-04 outputs in.
- **ACCEPTANCE**: extended `native_assets_hud_layout_test`: (a) speed digits for the oracle
  dial reading reproduce the measured dest cells; (b) timer 3178 frames → `00'52"98`-style
  cell layout matching atlas §3.2 exactly (digits, separators, pitch 14 UI); (c) rank 24 →
  `2`,`4`,`th` cells at §3.3 dests; (d) lap formats incl. the lap==0 flag glyph branch;
  (e) divisor constant carries its B5 citation — grep-checkable `// cite:` comment.
- **Tier / escalation**: small-paid — glm-5.3-flash / qwen3.8-flash.
- **Coordinator verification**: recompute 3178 frames → the timer string by hand; verify
  the divisor citation resolves to a B5 measurement snippet; run the test binary.

---

## 3. Execution order & parallelism

Quota policy caps ~6 concurrent workers. File ownership is disjoint everywhere, so the only
true sequencing is information flow.

**Round 1 (immediately after wave-1 docs merge; 6 lanes):**
`W2-01` (senior-RE), `W2-03` (senior-RE), `W2-04` (medium), `W2-09` (small), `W2-11` (small),
`W2-12` (small). — The two RE-heavy systems start first; the three calibration appliers and
the HUD contract are cheap and unblock visible-score movement early.

**Round 2 (as round-1 lanes free; 4–5 lanes):**
`W2-08` (medium; needs B6 only), `W2-10` (small; B8), `W2-02` (medium; needs B1 + task
notes only), `W2-05` (small; independent). W2-06 can start the moment W2-01's *header*
lands (MessageHost interface), typically mid-round-2.

**Round 3 (last; 2 lanes):**
`W2-06` if not already started (needs W2-01's gsl_vm.h), then `W2-07` LAST — it freezes the
final SaveData field map once W2-03/05/06 reports are in.

**Coordinator integration beats (not batches):** after each landing — build in
`build/native`, register test snippets, wire glue (mode_flow→session, race state→race_hud,
minimap frame→preview, scene BGM→audio engine), run the 73+new sweep, feature matrix,
determinism pair, capture + `frame_compare.py` scores, commit+push. The look-score deltas
(W2-09/11/12 acceptance) are only claimable *after* this wiring; workers report module-level
proof + the exact capture command, coordinator reports the score.

**Recommended single highest-leverage batch: W2-01 (event-script VM).**
Rationale: the town/adventure loop — doors, residents, photos, shop entry, minigame entry,
prizes — is script-interpreted in the original; W2-05/W2-06 are its consumers and W2-02's
mode flow is a shell without it. It is also the highest-risk RE (opaque opcodes), so starting
it in round 1 with the strongest tier maximizes the chance the whole wave lands. Second:
W2-03 (a playable race is the demo that proves the rebuild).

## 4. Risks & fallbacks

- **B4 finds GSL = texture-only**: W2-01 pivots to the ELF-side script tables (pivot clause
  in its spec); no other batch is affected.
- **Checkpoint rect source stays OPEN** (course-routes.md negative result): W2-03 ships
  centerline-gate zones GAP-tagged; oracle behavioral check (wrong-way/short-cut) deferred
  to wave 3.
- **Attract timeout / typewriter pacing unpinned**: tagged GAP constants, never presented
  as authentic.
- **Save field churn** from late batches: W2-07 runs last precisely to absorb it; if a
  round-3 field lands after W2-07, coordinator appends with the same archive pattern.
- **Score-delta acceptance depends on coordinator wiring**: split acceptance into
  module-test (worker-verifiable) + score-delta (coordinator-verifiable post-glue), as
  written above; a worker never claims the score delta itself.
- **Quota**: dispatch per CLAUDE.md live-quota gate; free models only as last resort;
  background:true + explicit cloud model on every dispatch.
