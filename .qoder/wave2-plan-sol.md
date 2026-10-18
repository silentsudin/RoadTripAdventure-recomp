# Wave 2 — playable native vertical slice implementation plan (P1 / Sol)

> **For agentic workers:** implement one assigned batch with test-first steps; use the executing-plans skill. Smaller workers execute, the coordinator independently verifies and integrates. No worker dispatches further agents or performs git operations.

**Goal:** turn the current visual demonstration into a controller-driven native loop: title → Adventure → Peach Town → resident/shop → race → results → town → save → restart/Continue.

**Architecture:** retain the existing authored C++ modules; replace placeholder behavior at their boundaries with evidence-backed content readers and deterministic controllers. One 60 Hz authoritative simulation owns progress, tasks and race state; rendering consumes snapshots and interpolates poses only. Original content is runtime input from the user's extraction, never executable guest code.

**Tech stack:** C++20, existing native libraries, SDL3 platform input/audio, Vulkan preview renderer, synthetic native unit fixtures, optional local-disc/oracle differential tests.

**Spec:** user phase-2 brief; `CLAUDE.md`; `.qoder/plan-journal.md`; `.qoder/wave-board.md`; source-grounding documents named per batch below. This is a proposal for coordinator review, not authorization to execute beyond the assigned batch.

## Findings that determine scope

- Existing implementations already include `TaskPool`, `GameSession`, `RaceTracker`, `RaceRoute`, a caller-authored-line `Dialogue`, `Shop`, `MemoryCard`, `SaveData`, input/trigger mapping, chase camera, VAG decoder/mixer/engine, radio and enhanced-map logic. Rebuilding these from scratch wastes the wave. `RaceRoute` already enforces immediately-next zone progression but is not connected to player race completion; audit/reuse it rather than creating a competing route model.
- `src/native/app/main.cpp:2093-2098` discards ordinary driving input and passes it to `GameSession` only in test-socket lockstep. `session.cpp:43` makes any Explore confirm start a race. A screenshot improvement cannot fix either usability blocker.
- `game_loop.cpp:139-159` calls physics eight times per VBlank by default, with authored movement scaling. Player ticks have **no lap/checkpoint detection**; AI waypoint wrap alone completes laps. `FrameSnapshot::rank` currently comes from a time bracket, not opponents. `session.cpp` also owns a second race timer and a fabricated 1000/500/250 payout. Fix these before declaring a playable race.
- `sys-gsl-packs.md` proves 49/49 `.GSL` files are serialized texture-upload containers. Neither that extension nor SCORE palette cells establish a scripting language. Real event/dialogue consumers are `InterpretScriptCommands` at `0x23AD00` and `dialogueTask` at `0x23B168`. **Do not build a fictional GSL instruction VM.** B4 must resolve the naming conflict; implement a bounded native interpreter only for the independently established event grammar. Parsing upload records into host textures is data conversion, not GS execution.
- `map-view.md` already provides torus/shear projection, routes and enhanced-map logic; the retail minimap is a live viewport, not a missing static GSL image. B8 blobs require classification before use.
- `save.h` already preserves unknown bytes of a 0x3448 image, and `memcard.cpp` already loads/writes two raw slots. Missing pieces are safe writes and usable Continue/save flow, not a new guessed PS2 format.
- VAGs are hour-long **radio** pairs at 12 kHz; title/room/race music is TSQ/TVB. `BgmPlayer` already exists but loops a bounded pre-render. Do not relabel radio playback as complete BGM reconstruction.
- Manifest cautions matter: goldens are 1280×447 field scanouts, not 1280×720; `progress_scene` can be stale at title/race; race fields in town are invalid union data (e.g. `1144128939` is not a town finishing place). Race frames show `2447` and `3178` elapsed ticks, a difference of **731**, not the nominal 720 in the prose recipe. Compare exact ticks/context, not wall-clock labels.
- The brief reports 73 tests and parity title 0.872 / menu 0.46 / town 0.532 / race 0.374. These are **supplied baselines, not independently re-run here**. Journal entries describe older 68-test runs and stale GPU-environment failures. Coordinator takes a fresh baseline before dispatch.

## Mandatory contract — copy into EVERY batch dispatch

1. Fully native authored C++ only: **ZERO MIPS/guest execution, ZERO GS/VU execution**, no PS2 runtime linkage. `third_party/PS2Recomp` and oracle executable remain read-only, evidence-only, never native runtime dependencies. Do not mechanically translate guest functions. CPU parsing of user-supplied asset records into host data is allowed; hardware command execution is not.
2. No ROM bytes, extracted/decoded assets, game pseudocode or derived code in tracked files. Read-only evidence root: `~/Library/Application Support/RoadTripRecomp/disc`. Runtime caches/results under the worker's ignored `build/w-<ID>/`; synthetic fixtures only in tests. Constants may be authored with citations. For ELF reads verify PT_LOAD mappings; observed SLUS delta is `vaddr - 0x1FF000`.
3. No git commands; no edits to **any** `main.cpp`, **any** `CMakeLists.txt`, `third_party/`, shared build directories, journal, board, other workers' files, or wave-1 reports. Write only the enumerated owned files and your own report. Private build directory **`build/w-<ID>/`**. Missing dependency or ownership change → ask coordinator, do not repair a neighbor.
4. Exactly fixed **60 Hz VBlank logic**. No extra authoritative steps for rendering, audio callbacks, camera interpolation or screenshot capture. Render interpolation is read-only. Android application ID **`io.github.silentsudin.roadtrip.dev`**, label **Road Trip DEV**; never touch release app/data. Only coordinator performs device deployment.
5. Every gameplay/render calibration constant cites an ELF address+consumer or golden file+pixel measurement; host policy constants explicitly say AUTHORED and cite the policy/design. Unverified behavior remains **GAP** (or existing INFERRED/OPEN/UNVALIDATED with a GAP mapping). Never silently turn a guessed name into proof. Never weaken tests or widen thresholds to pass.
6. Test first: add failing synthetic regression, establish failure, implement, rerun assigned and existing adjacent tests. New CMake targets are registered by coordinator, not worker. If unavailable, compile the focused test directly into your private build directory; report exact compiler/source list. Do not run concurrent shared feature-matrix captures.
7. Deliver `.qoder/wave2/<ID>-report.md`: files, API/glue recipe, commands+exit results, evidence snippets/measurement commands, confidence, remaining GAPs. No raw game payloads. An unmet real-content gate means **partial/blocked**, not complete.

These seven rules apply verbatim to W2-01 through W2-12; each batch below explicitly incorporates them.

## Dispatch gates and exact wave-1 inputs

At planning time the following promised files were **not present** in the inspected tree. Coordinator must wait for their landing, independently verify a sample, then freeze their reviewed revision for workers:

| Wave-1 lane | Exact output path | Wave-2 consumers |
|---|---|---|
| B1 | `config/game_symbols.toml` (upgraded notes) | all RE-sensitive batches |
| B2 | `docs/native-reference/sky-ground-calibration.md` | W2-04, W2-10, W2-12 |
| B3 | `docs/native-reference/menu-calibration.md` | W2-02, W2-06, W2-07 |
| B4 | `docs/native-reference/gsl-vm.md` | W2-01, W2-07; grammar contradiction gate |
| B5 | `docs/native-reference/gauge-calibration.md` | W2-04, W2-05, W2-12 |
| B6 | `docs/native-reference/audio-inventory.md` | W2-09 |
| B8 | `docs/native-reference/minimap-blobs.md` | W2-11 |

B7 changes under `scripts/` must finish before the coordinator freezes capture commands. Do not guess the filenames or wait by polling. Existing source/docs support synthetic scaffolds, but missing reviewed evidence blocks corresponding authenticity claims.

## Coordinator-owned integration and test protocol

- Reserve `src/native/app/main.cpp`, `src/native/CMakeLists.txt`, root CMake, Android/platform glue, `src/native/runtime/guest_facade.*`, shared capture scripts and feature matrix to the coordinator. No batch owns them. Keep the legacy facade a read-only diagnostic adapter over typed native state, never an alternate gameplay authority.
- Coordinator registers the **executable target names** specified below. Build/run contract once registered: `cmake --build build/w-<ID> --target <native_target> -j2` then `build/w-<ID>/<native_target>` (resolve executable location from the generated build if the generator nests it). Native-only configure root is `src/native/`; use current known-good native configure options, not the oracle root build.
- All workers may read shared headers but freeze dependencies before consuming them. APIs below are proposed host interfaces, **not recovered game ABI**. Coordinator approves interfaces before parallel work. Owners may add fields to their private result types without adding fields to another owner's headers.
- After each landing: clean coordinator build, all baseline native tests, new tests, feature matrix, then determinism pair at identical input/tick state. An intentional physics/state change needs an explained baseline update, not byte equality against obsolete mock behavior. Interpolation ON/OFF at equal logic tick must still agree on authoritative state.
- Whole-frame histogram scores are secondary: freeze comparator, crop/resize policy and scene inputs. Record regional masks and geometry residuals too. Do not alter comparator/goldens to improve a score. Worker estimates below are test requirements or proposed policy, never newly measured facts.

## Review focus

1. Physical controllers must drive normal runtime, not just the socket; disconnect/focus loss must release controls (W2-03).
2. A human must finish a race without AI waypoint counters, shortcut/line-jitter exploits or a second timer (W2-05).
3. Resident/shop cancellation and unsupported event commands must not grant money, inventory or flags (W2-01/06/07).
4. Failed saves/loads must preserve the previous slot and active session, including unknown save bytes (W2-08/02).
5. Asset loss, stream underruns, projection seams and render interpolation must not corrupt authoritative simulation (W2-04/09/10/11/12).

---

## W2-01 | Bounded native event-program reader and execution core

**Goal:** implement the evidenced gameplay script grammar, not a GSL/GS emulator.

**Why now:** B4 recon and B1 improved names unlock resident/event behavior shared by dialogue, shops and progression. Existing GSL/container evidence forces a format-identification gate first.

**Owned files:** new `src/native/gameplay/event_program.{h,cpp}`, new `tests/native_assets/test_event_program.cpp`, new `docs/native-reference/event-runtime.md`, own W2-01 report. No existing asset decoder changes.

**Required inputs:** `docs/native-reference/gsl-vm.md`, `docs/native-reference/sys-gsl-packs.md`, `docs/native-reference/task-system.md`, `config/game_symbols.toml`, `config/game_state.toml`, `src/native/gameplay/{task.h,save.h,progression.h}`, local `build/decomp/fn/0023ad00.c` and `0023b168.c` if exported. If filenames differ, locate with `scripts/ghidra/fn.py 0x23ad00` / `0x23b168` rather than invent a path.

**Interface:** own `EventProgram` stores validated commands/string references as host offsets, never function pointers. `decodeEventProgram(span<const uint8_t>) -> EventDecodeResult`; `EventMachine::step(const EventReply&) -> EventYield`. Yield discriminants: Text, Choice, Effect, Wait, Finished, Unsupported, Error. A yield has a monotonically increasing token; only the matching reply resumes it. Repeated polling never reapplies an effect. Effect payloads remain typed requests; W2-07 owns their dispatch, not this parser.

**Steps (global contract 1–7 applies):**
1. [ ] Resolve B4 against `sys-gsl-packs.md`; document actual script origin, opcode widths/terminators, branch/address representation and consumers. If no script grammar is proven, explicitly reject GSL-as-script and scope this batch to the proven ELF event stream; no invented registers/stack/opcodes.
2. [ ] Write failing tests for every opcode admitted to the initial supported set, missing operands, bad string/branch offsets and unrecognized instructions. Initial required semantic subset: proven termination, conditional selection, text/choice handoff and yielding; unsupported semantics fail closed.
3. [ ] Implement bounded decoding and the yielding machine with injected progress queries; any native instruction budget is AUTHORED safety policy and must not change proven waits.
4. [ ] Add replay/token tests so wait/choice/effect responses are consumed once; validate loops cannot hang a VBlank and malformed streams never mutate progress.
5. [ ] Add an optional local-ELF script traversal using an explicitly cited resident pointer. Compare its command/event transcript with an oracle capture; leave all unimplemented handlers GAP.

**Acceptance:** `native_event_program_test` asserts operand/branch bounds, exact decoded synthetic command counts, deterministic yields, stale reply rejection, wait boundary, unknown-command nonmutation and instruction-budget termination. Real-content closure requires one cited Peach resident program decoded without guessing and a matching oracle event transcript; a synthetic-only pass is not closure.

**Tier:** medium; escalate to senior-RE if B4 conflates DMA records and event opcodes, pointer-base semantics conflict, or one opcode's width/side effects remain ambiguous. Coordinator independently reads `0x23AD00`/`0x23B168` and one operand/string in the ELF; does not trust the worker's decoder as its own oracle.

## W2-02 | Native mode/task orchestration and reachable game flow

**Goal:** replace confirm-to-race/demo flow with explicit modes, pause/loading/modal ownership, Continue and real subsystem handoffs.

**Why now:** B1 task names and B3 menu calibration plus W2-01's yield contract make behavior reachable. The existing `TaskPool` must be audited, not replaced.

**Owned files:** `src/native/gameplay/{session.h,session.cpp,task.h,task.cpp,menu.h,menu.cpp}`, `src/native/preview/{ui_menu.h,ui_menu.cpp}`, new `tests/native_assets/test_mode_flow.cpp`, new `docs/native-reference/native-mode-flow.md`, own W2-02 report. Existing tests remain read-only.

**Required inputs:** `docs/native-reference/{task-system.md,menu-calibration.md,ui-parity.md,scene-tile-map.md}`, `docs/game-mechanics/task-scheduler.md`, `config/game_symbols.toml`, `src/native/gameplay/{session.*,task.*,menu.*,game_loop.h,save.h}`, `build/scratch/oracle-golden/manifest.json`; reviewed public interfaces of W2-05/06/07/08 before final wiring.

**Interface:** `SessionCommand` typed requests (NewGame, Continue, EnterDoor, StartRace, AcceptResults, Pause, Resume, ReturnTitle); `GameSession::dispatch(const SessionCommand&) -> bool`; retain `update(MenuInput, const CarInput*)` compatibility but null driving input means idle, not implicit AI. Own a stable `SessionView` for scene/modal/menu state; other workers consume data, not mutable `GameSession` internals. Coordinator routes top-level menu events to dispatch.

**Steps (global contract 1–7 applies):**
1. [ ] Write a state-transition table and failing tests: title/menu, new/continue, factory/town, door/modal, race setup/countdown/racing/results, paused and return-to-title. Unsupported 2-player/ending paths remain explicitly GAP, never masquerade as Adventure.
2. [ ] Remove Explore-confirm race start; require a validated race-entry command, and route menu choices to explicit requests. Introduce transition/loading completion events; a failed load cannot leave a half-switched scene.
3. [ ] Audit deferred task killing, cleanup order and 48-slot exhaustion against `task-system.md`. Preserve/document next-pass spawned-task scheduling as AUTHORED unless oracle proves exact same-frame order; do not assert current scheduler is fully recovered.
4. [ ] Establish pause/modal ownership: input reaches exactly one consumer, driving/timers freeze where evidence requires, opening a menu does not repeatedly advance world state.
5. [ ] Apply B3 reviewed menu constants in `ui_menu` and preserve sibling pause/options hiding rules. Expose view models usable by coordinator's FONTE draw path; do not add debug-only retail labels.
6. [ ] After dependent APIs freeze, wire race result commit, event/modal replies, shop transaction and transactional Continue. Keep only one race timer owner (W2-05), with progress committed once in the correct result phase.

**Acceptance:** `native_mode_flow_test` asserts an ordinary confirm in town does not race; no-input is idle; New Game and Continue reach correct factory/town context; canceled/failed transitions preserve state; 120 paused updates do not advance race/world; modal consumes confirm once; one result yields one record/award application; task cleanup runs once with siblings surviving. Coordinator also rehearses title→town→race→town without environment warps and compares main-menu regional geometry to B3 (no score regression from frozen baseline).

**Tier:** medium; escalate senior-RE only for disputed task lifecycle/timing or scene table semantics. Coordinator replays the same command sequence independently and checks task counts and `SessionView` at every boundary, then verifies visible menu selections route to those commands.

## W2-03 | Real-time player input and deterministic routing

**Goal:** identical keyboard/gamepad/socket actions in ordinary play, with correct edges, pause capture and per-port trigger state.

**Why now:** B1 controls notes and B5 speed units enable meaningful trigger mapping; existing helpers are already present, but ordinary app glue throws their output away. This is the fastest usability win.

**Owned files:** `src/native/gameplay/{input.h,input.cpp}`, new `src/native/gameplay/input_router.{h,cpp}`, new `tests/native_assets/test_input_router.cpp`, new `docs/native-reference/native-input-routing.md`, own W2-03 report.

**Required inputs:** `src/native/gameplay/{input.*,menu.h,car_state.h,settings.h}`, `src/game/{DriveFeel.h,DriveFeel.cpp}` (read-only sibling behavior), `docs/native-reference/{gauge-calibration.md,frame-timing.md}`, `src/native/app/main.cpp:1850-2260` (read-only glue audit), `config/game_state.toml` controls.

**Interface:** `InputRouter::sample(const RawInput&, InputContext) -> RoutedInput`, once per logic step; `releaseAll(port)` on focus/disconnect. Result contains held `CarInput`, edge `MenuInput`, per-port trigger output and camera/map/pause actions. Coordinator must pass `&routed.drive` to `session.update` in **both** ordinary and lockstep modes; deterministic autopilot requires an explicit separate mode.

**Steps (global contract 1–7 applies):**
1. [ ] Add failing tests for held gas, confirm edges, per-port reverse latches, focus loss and menu-vs-driving consumption; pin existing trigger-map defaults as sibling AUTHORED policy, not ELF facts.
2. [ ] Unify raw keyboard/controller/injected button mapping, clearing disconnected controls and sanitizing nonfinite/out-of-range axes.
3. [ ] Sample/advance pulse and reverse state only at logic steps; store held input through extra interpolation draws without generating new menu edges.
4. [ ] Make simultaneous confirm/gas context-sensitive; closing a dialogue cannot fire a second action with the same press. Surface trigger ramp/cap outputs for W2-04 instead of calculating and ignoring them.
5. [ ] Deliver exact main-loop replacement recipe and a runtime smoke input sequence; coordinator owns SDL event and app wiring.

**Acceptance:** `native_input_router_test`: 60 held ticks remain held, one press yields one confirm, release/repress yields another; two ports cannot alter each other's reverse state; disconnect/focus loss yields zero controls; socket/keyboard/pad equivalent synthetic histories produce equal routed actions; 30/60/144 render schedules with equal 600 logic steps yield equal input-state hashes. Coordinator physically drives town and race outside lockstep and verifies input is no longer discarded.

**Tier:** small-paid; free only when live quota/user policy permits. Escalate medium for timing drift or disagreement with DriveFeel behavior. Independent recheck: feed identical raw histories through a small reference edge detector and inspect ordinary main-loop pointer routing, not just socket test results.

## W2-04 | Authoritative drive simulation, contact geometry and real course binding

**Goal:** drive over actual town/course geometry with grounded speed/displacement, wall/ground contact and consistent snapshots.

**Why now:** B2 ground classification and B5 speed calibration distinguish physical truth from visual fill. W2-03 supplies real input; without contacts/calibration a playable car is still an eight-subtick square-track mock.

**Owned files:** `src/native/gameplay/{game_loop.h,game_loop.cpp,car_state.h,car_physics.h,car_physics.cpp,world.h,world.cpp}`, new `src/native/gameplay/drive_surface.{h,cpp}`, new `tests/native_assets/test_native_drive.cpp`, new `docs/native-reference/native-drive-grounding.md`, own W2-04 report.

**Required inputs:** `docs/native-reference/{gauge-calibration.md,sky-ground-calibration.md,world-geography.md,course-routes.md,course-data-status.md,nav-tables.md,frame-timing.md}`, `docs/game-mechanics/car-physics.md`, `src/native/assets/{course_data.h,field_converter.h,native_model.h,nav_routes.h}`, `src/native/gameplay/{nav_route_bridge.h,rules_tables.h,interp.h}`, `config/game_symbols.toml`, `build/scratch/oracle-golden/manifest.json` town/race entries.

**Interface:** injected `DriveSurface` triangle/surface query and scene route, separate from visual hole-fill. `GameLoop::configureScene(const DriveScene&)`; `tickPlayer` accepts W2-03's drive limits via an overload, preserving old callers. Snapshot exposes authoritative position including elevation, gear, raw speed, velocity-derived display speed and contact/event telemetry; W2-05 consumes pose samples, W2-12 camera consumes pose/speed. Do not put race rules in this batch.

**Steps (global contract 1–7 applies):**
1. [ ] Add failing traces around idle, throttle, brake/reverse, steering, slope and wall fixtures; independently derive the relation among `CarState::speed`, displacement and displayed speed from B5/ELF. Never adopt the HUD `/100` placeholder as physical truth.
2. [ ] Audit the eight `tickCar` calls per VBlank, gravity 88 vs journal's `-0x59` evidence, heading integration and odometer increments. Retain/remove substeps only with proven per-VBlank equivalence; record unresolved differences GAP.
3. [ ] Inject real scene routes/world data rather than default square coordinates; use existing runtime parsers and world-layout rebase, with explicit game/render/fixed unit conversion.
4. [ ] Build bounded ground-height/material and swept wall/contact queries over runtime geometry. Surface labels with unproven material mapping stay GAP; never use cosmetic nearest-vertex hole-fill as a road collision floor.
5. [ ] Apply trigger cap/ramp limits to authoritative car evolution; expose landing/scrape telemetry for existing rumble. Keep reset/warp snapshot refresh free of hidden extra simulation ticks.
6. [ ] Record exact-tick oracle drive traces (idle, straight gas, braking, steering) and a user-driven course/town smoke. Include initial car/parts/pose/heading, not only screenshots.

**Acceptance:** `native_drive_test` asserts finite state, no idle drift, correct displacement sign under reverse, no synthetic wall tunneling, slope height continuity, airborne/landing transition, cap/ramp application and zero frame advance on pure snapshot/warp read. Same 600 logic inputs across render schedules produce equal authoritative hashes. Local-content gate: no below-ground pose on a coordinator-chosen Peach route, and matched starter-car oracle straight-line displacement and display speed within a **proposed 5% engineering tolerance** at exact ticks; if miss, report GAP, do not tune unexplained constants. Physics internals must match cited integer step expectations exactly where proven.

**Tier:** medium; senior-RE escalation for unit/substep/contact ambiguity or trace miss. Coordinator independently integrates measured speed/displacement, re-reads critical loads/multipliers, and checks a non-worker-selected geometry segment. A timer-correct autopilot screenshot is not driving evidence.

## W2-05 | Human-completable races, opponents, records and live HUD

**Goal:** checkpoint-validated laps, one timer, positional standings and a real finish/result; display live state with the proven SCORE/METER art.

**Why now:** B5 gauge calibration and `race-hud-atlas.md` provide presentation grounding; W2-04 supplies actual routes/pose. CSA palette cells supply graphics, **not scoring logic**.

**Owned files:** `src/native/gameplay/{race.h,race.cpp,results.h,results.cpp,route.h,route.cpp}`, new `src/native/gameplay/race_controller.{h,cpp}`, `src/native/preview/race_hud.h`, `src/native/assets/{hud_art.h,hud_art.cpp}`, new `tests/native_assets/test_race_controller.cpp`, new `docs/native-reference/native-race-flow.md`, own W2-05 report.

**Required inputs:** `docs/native-reference/{race-hud-atlas.md,hud-layout.md,gauge-calibration.md,rules-tables.md,course-routes.md,traffic-ai.md}`, `src/native/gameplay/{race.*,results.*,route.*,nav_route_bridge.h,rules_tables.h,ai.h,save.h}`, `src/native/assets/{nav_routes.h,course_data.h}`, `config/game_symbols.toml`, `build/scratch/oracle-golden/manifest.json` race entries.

**Interface:** `RaceController::step(span<const RacerPose>) -> RaceView`, one call per VBlank; `RaceOutcome` contains course/result id, finishing order, lap/best/total ticks and immutable finish token. Countdown/running/finished phase controls the sole `RaceTracker`. `RaceView` supplies HUD rank/lap/time/entrant poses; `RaceOutcome` does **not** directly grant money. W2-02 owns committing outcomes; W2-06 validates eligibility/award phase.

**Steps (global contract 1–7 applies):**
1. [ ] Reconstruct checkpoint/start-line ordering, countdown gating and record index mapping from named ELF consumers; create synthetic directional line-crossing and missed-checkpoint tests before implementation. Do not equate navigation nodes with checkpoints without evidence.
2. [ ] Audit/reuse `RaceRoute` and the route bridge for consecutive checkpoints, adding swept crossing and finish phase for human and AI equally where evidence supports them. Backward line crossing, line jitter, teleport or shortcut cannot award a lap; exact unsupported checkpoint data blocks the relevant real course rather than inventing it. Any stricter anti-cheat behavior beyond the recovered game is labelled AUTHORED, not parity.
3. [ ] Rank active entrants by proven progress ordering, not `getRaceTimeRank`; retain that helper only where its time-grade semantics actually apply. Feed existing AI with the same real course/physics adapter, with entrant count/config read from evidenced course/race metadata.
4. [ ] Centralize total/lap ticks and finish freeze; expose deterministic entrant order/tie policy. Reuse results/record layout; no second session timer and no course-0-only writes.
5. [ ] Build HUD from live `RaceView` and grounded speed/gear. Preserve SCORE bank distinction: CSA 1 timer/banner, CSA 0 rank; source/dest measurements stay labelled to their actual coordinate convention. Audit existing `clutBaseEntry=56` claim against true palette swizzle instead of blindly replacing it with 16.
6. [ ] Expose finish/record update API and precise W2-02/W2-04 glue; coordinator draws opponent poses through the existing car mesh path. Local oracle transcript must include countdown, one lap, finish and result confirmation.

**Acceptance:** `native_race_controller_test` asserts no pre-GO elapsed ticks; exactly one increment per running VBlank; required ordered checkpoints before lap; reverse/jitter/missed gates reject; human path finishes without AI waypoint counters; finished clock freezes; inactive entrants rank -1; stable positional standings; record bands/aliases match documented 26×3 mapping. For timer presentation, elapsed **3177** ticks truncates to `00'52"95` if the recovered display rule confirms truncation; `3178` is a separate case, not forced to match the screenshot (capture can lag state). Unsupported truncation/rounding remains a gate. Coordinator independently replays a human input race and checks per-tick line crossings plus HUD masks against atlas measurements.

**Tier:** medium; senior-RE for checkpoint geometry, race-record mapping, timer rounding or eligibility ambiguity. Coordinator verifies rank from entrant world positions and rereads record/timer consumers; cannot accept a race won by hidden autopilot or hardcoded displayed place.

## W2-06 | Persistent shop inventory, equipment and honest economy

**Goal:** resident shop entry → browse → confirm buy/sell/equip → persistent car/progress change, without invented stock or payouts.

**Why now:** B1 shop getters and B3 presentation calibration provide the missing bridge from the existing generic `Shop` to game categories and saved ownership.

**Owned files:** `src/native/gameplay/{shop.h,shop.cpp}`, new `src/native/gameplay/shop_catalog.{h,cpp}`, new `src/native/preview/shop_screen.{h,cpp}`, new `tests/native_assets/test_shop_transactions.cpp`, new `docs/native-reference/native-shop-flow.md`, own W2-06 report. `save.*`, `rules_tables.*` remain read-only.

**Required inputs:** `docs/native-reference/{rules-tables.md,save-layout.md,menu-calibration.md,ui-parity.md,car-data-status.md}`, `docs/game-mechanics/menus-and-save.md`, `src/native/gameplay/{shop.*,save.h,progression.h,rules_tables.h}`, `config/game_symbols.toml` (`0x243670`, `0x2F1280`, `0x269568`, `0x268E40` notes), `src/native/preview/ui_menu.h`.

**Interface:** `ShopCatalog` loads runtime price/category/availability references with provenance; `ShopTransaction::apply(SaveData&, const ShopRequest&) -> ShopResult` atomically validates and changes ownership/equipment/money. Inventory bits not yet typed in SaveData are accessed through a bounded shop-owned codec over the retained image, with documented disjoint ranges; W2-08 serializer must preserve them. `ShopView` produces UI quads/text, not shared menu mutations.

**Steps (global contract 1–7 applies):**
1. [ ] Audit existing buy/sell semantics (catalog quantity is not proof of player ownership); prove category ownership/equipment ranges and price getters before admitting a real part.
2. [ ] Add failing fixtures for sufficient/insufficient cash, odd-price half refund, already-owned, not-owned, equipped-part sell restriction if proven, canceled confirmation and cap overflow.
3. [ ] Implement transaction validation before mutation; expose cash SFX request once on successful nonzero spend/credit. Reject duplicate confirmation tokens and invalid category/id, with no partial changes.
4. [ ] Bind one evidenced Peach Parts Shop catalog and usable browse/confirm/back view; implement price/ownership/equipment effects for its validated subset. Preserve unknown category behavior as GAP.
5. [ ] Provide result-award policy using reviewed payout tiers only for proven licence/WGP contexts; Quick Race cannot grant fabricated 1000/500/250 or guessed entry fees. W2-02 invokes awards in the evidenced result phase, once.

**Acceptance:** `native_shop_transactions_test` asserts atomic failure/cancel, inventory gain/loss, exact funds/refund and cap, stale token rejection, equip persistence across serialize/parse, invalid range no mutation and no unearned Quick Race payout. At least one runtime-catalog price independently matches its ELF getter field. Coordinator re-reads one non-flat price and ownership bit, performs buy→save→restart→sell, then compares money and serialized changed-byte ranges.

**Tier:** small-paid after proven schema; medium/senior-RE escalation if ownership or category getter semantics are unresolved. Do not let a small worker invent item tables to fill a screen.

## W2-07 | Resident/door dialogue and effect interpreter

**Goal:** approach/interact with a Peach resident or shop door, see real dialogue/choices, execute validated effects and resume driving.

**Why now:** B4 actual event grammar + B1 command notes + B3 UI constants; consumes W2-01 rather than introducing a second parser.

**Owned files:** existing `src/native/gameplay/dialogue.{h,cpp}` (extend, preserve legacy caller-authored-line API), new `src/native/assets/resident_events.{h,cpp}`, new `src/native/preview/dialogue_screen.{h,cpp}`, new `tests/native_assets/test_dialogue_flow.cpp`, new `docs/native-reference/native-dialogue-flow.md`, own W2-07 report. `progression.*`/`collectibles.*` read-only; request coordinator expansion if genuinely necessary.

**Required inputs:** `docs/native-reference/{gsl-vm.md,map-view.md,scene-tile-map.md,menu-calibration.md,ui-parity.md}`, `config/{game_symbols.toml,game_state.toml}`, `src/native/gameplay/{dialogue.*,event_program.h,save.h,progression.h,collectibles.h}`, `src/native/assets/glyph_atlas.h`, `src/native/preview/ui_menu.h`; local ELF resident table `0x2C22C8`, door table `0x2C0698`, interpreter consumers `0x23AD00` and `0x23B168`.

**Interface:** resident reader emits `ResidentEventSource` (location, door, text/event references, provenance) using PT_LOAD-validated spans. `DialogueController::begin(source)`, `step(MenuInput) -> DialogueView`, and `replyEffect(token,result)` wrap W2-01 yields. Typed effect bridge invokes existing photo/flag/mail/shop helpers; unknown effects return Unsupported with no progress write. A shared input press is consumed once.

**Steps (global contract 1–7 applies):**
1. [ ] Add bounded ELF table/string/event readers for Peach's door/resident associations; separate 28 door residents from 11 street drivers. Names classify shops, not fabricated door→photo mappings.
2. [ ] Write failing multiline/choice/cancel/wait/effect tests and truncated escape/string tests; supported effects include proven shop handoff and at least one verified progress mutation.
3. [ ] Implement modal dialogue controller over W2-01, preserving choice result mapping and task completion semantics. Typewriter/repeat rates must be evidence-cited or labelled host policy GAP, not guessed retail behavior.
4. [ ] Build FONTE text/frame/choice draw data; runtime strings only. Command bytes/escapes must not appear as printed glyphs.
5. [ ] Bind door interaction geometry and dispatch effects through injected services; update visited-house/photo state only at the proved script phase. Take Quick-Pic number from command `0x11 n`, never door ordinal.
6. [ ] Verify one resident conversation and one Parts Shop handoff against an oracle action transcript; deliver door trigger and modal-return integration instructions to W2-02/coordinator.

**Acceptance:** `native_dialogue_flow_test` asserts text/choice order, mapped choice result, no duplicate advance on held confirm, canceled/unknown effect nonmutation, exact supported flag/photo effect, no drive input while modal, resume afterward, invalid ELF pointers fail closed. Optional-disc test asserts Peach has 28 door records/11 street records and expected staff names without embedding those payload bytes in fixtures. Coordinator chooses a second resident, traces operands independently and confirms post-dialogue save delta.

**Tier:** medium; senior-RE if control bytes vs text escapes or script-side effects differ. Coordinator must verify actual text source pointer and one effect operand; a handwritten replacement conversation is not acceptance.

## W2-08 | Reliable native progress saves and transactional Continue

**Goal:** save actual earned progress safely and resume it through visible Continue, retaining unknown original fields.

**Why now:** B1 upgraded save notes and existing migration importer eliminate format invention. W2-06/07 will now modify real persistent state; direct truncating writes are unacceptable.

**Owned files:** `src/native/gameplay/{save.h,save.cpp,memcard.h,memcard.cpp}`, new `src/native/gameplay/progress_store.{h,cpp}`, new `tests/native_assets/test_progress_store.cpp`, new `docs/native-reference/native-progress-store.md`, own W2-08 report. No session or settings edits.

**Required inputs:** `docs/native-reference/save-layout.md`, `docs/game-mechanics/menus-and-save.md` (cross-check older contradictions), `config/{game_symbols.toml,game_state.toml}`, `src/native/gameplay/{save.*,memcard.*,session.h}`, `build/scratch/oracle-golden/manifest.json` factory/town context.

**Interface:** `ProgressStore::save(slot,const SaveData&) -> StoreResult`, `load(slot) -> validated candidate`, and `listSlots()`. Do not mutate live session on a load attempt. Save files remain raw 0x3448 bytes; optional native integrity metadata must be a separate sidecar, never a guessed in-block game checksum/version. W2-02 applies successful candidate and rebuilds scene/car from saved location/equipment.

**Steps (global contract 1–7 applies):**
1. [ ] Add patterned synthetic-image roundtrip, failed-write and failed-load tests. Correct misleading `save.h` commentary equating resident table `0x2C22C8` with progress RAM `0x177F760`; use corrected evidence, not the old brief's inferred alias.
2. [ ] Replace destination truncation with same-directory temporary write, complete-write/close verification and atomic replacement; document platform durability guarantees and inject failure points for testing. Cleanup only the store's own temporary files.
3. [ ] Preserve unknown bytes through load, migration and shop-owned raw field mutations; validate host acceptance separately from lossless parse. No raw game data fixtures.
4. [ ] Expose occupancy/error/cancel/overwrite status suitable for visible save/Continue flow and isolate slot 0/1 under native data directory. Do not confuse Adventure slots with legacy emulator save states.
5. [ ] Provide transactional Continue/apply recipe, rejection semantics and local original-payload import rehearsal. Mid-race full native save states are deferred, not emulated with progress-only saves.

**Acceptance:** `native_progress_store_test` asserts exact 13384-byte output, byte-identical unknown ranges, slot isolation, invalid/truncated image does not replace current state, failed write preserves prior slot, successful replacement leaves parseable new image, canceled overwrite leaves bytes unchanged, hostile slot/path rejection, and sequential buy/flag/record progress survives restart. Coordinator reads resulting files independently with `struct.unpack_from` at `0x654`, `0xFF0`, `0x1040` and `0x1178`; verifies Continue scene, not just parser equality.

**Tier:** small-paid; escalate medium for filesystem failure semantics or contradictory save ranges. Native crash recovery policy is AUTHORED, not recovered PS2 behavior.

## W2-09 | Bounded stereo radio streaming and correct music-context routing

**Goal:** audible, rate-correct stereo radio and scene music without decoding an hour of PCM into memory or pretending VAG is race BGM.

**Why now:** B6 inventory and existing audio-map establish assets/remaining sequencer gaps; W2-02 will expose real context transitions.

**Owned files:** `src/native/audio/{vag.h,vag.cpp,vag_loader.h,vag_loader.cpp,engine.h,engine.cpp,mixer.h,mixer.cpp,music_library.h,music_library.cpp,bgm_player.h,bgm_player.cpp}`, new `src/native/audio/stream_player.{h,cpp}`, new `tests/native_assets/test_audio_streaming.cpp`, new `docs/native-reference/native-audio-routing.md`, own W2-09 report. Existing assets TSQ/TVB parsers and radio gameplay module remain read-only.

**Required inputs:** `docs/native-reference/{audio-inventory.md,audio-map.md,radio-stations.md,tsq-tvb-format.md,sndmod-irx.md}`, `src/native/audio/*.h`, `src/native/assets/{music_seq.h,tsq_sequencer.h}`, `src/native/gameplay/radio.h`, `src/native/app/main.cpp:282-325` plus audio init/radio sites (read-only).

**Interface:** `StreamPlayer::prepare(pairPaths,outputRate)`, nonblocking `pull(span<int16_t>)`, `stop/reset`; preparation/IO never runs in the audio callback. `MusicRouter::setContext(MusicContext)` chooses VAG radio vs existing TSQ BGM and returns explicit unsupported status. Audio time derives from sample cursor; gameplay context changes are posted at logic boundaries, never by invoking simulation from a callback.

**Steps (global contract 1–7 applies):**
1. [ ] Audit ADPCM loop/end flags vs decoder/extractor; write synthetic predictor/flag/resample tests before changing semantics. Distinguish radio full-file looping from TVB sample loops.
2. [ ] Implement bounded chunk decode, separate L/R predictor state, native-rate metadata and continuous resampling (12 kHz source to device rate), with fixed-capacity buffering and underrun silence.
3. [ ] Route stereo pairs correctly through engine/mixer without `fileIndex % 4` inventing scene slot semantics; preserve mono/volume options. No heap allocation, file IO or blocking locks in steady-state pull.
4. [ ] Map title/select/town/shop/race/result contexts through audio-map and existing radio state; use existing TSQ BGM player only where supported, reporting truncated pre-render/scheduling limitations honestly.
5. [ ] Implement stop/transition/reset queue flush and deterministic offline PCM capture; keep decoder buffers bounded across repeated station/context changes.
6. [ ] Deliver coordinator AudioHost lifetime/command-queue wiring recipe and a local disc playback audit. TSQ driver fidelity beyond proven inventory is a separate GAP, never a substitute looping radio track.

**Acceptance:** `native_audio_streaming_test` asserts separate stereo channels, exact source/output duration ratio on synthetic fixtures, chunk-invariant PCM, continuous predictor/resampler state, underrun silence, reset no stale samples, gain/mono correctness and bounded resident buffers independent of stream duration. Buffer ceiling **8 MiB** is proposed AUTHORED engineering budget, not original-game fact. Coordinator inspects allocation/IO instrumentation, independently decodes a short VAG excerpt and cross-correlates channel/timing output; device audible check covers radio off/on plus scene change.

**Tier:** medium; senior-RE for TSQ event interpretation or disputed loop markers; do not broaden into SNDMOD/IOP emulation. Free tier is appropriate only for mechanical inventory follow-up, not DSP correctness.

## W2-10 | Sky/ground render parity without cosmetic collision hacks

**Goal:** implement B2 calibration in the actual scene renderer, reducing charcoal gaps/incorrect sky/fog without hiding geometry or moving the car to fake a match.

**Why now:** B2 golden masks and B1 render notes separate sky layers, daylight, terrain texture/lighting and visual hole fill. W2-12 independently fixes camera calibration.

**Owned files:** `src/native/preview/{preview.cpp,lighting.h,preview.vert,preview.frag,preview_textured.vert,preview_textured.frag,preview_shaders.h}`, new `src/native/preview/sky_ground.{h,cpp}`, new `tests/native_assets/test_sky_ground.cpp`, new `docs/native-reference/native-sky-ground.md`, own W2-10 report. `preview.h` and HUD/UI shaders stay read-only; coordinator resolves any additive renderer API needs.

**Required inputs:** `docs/native-reference/{sky-ground-calibration.md,field-texture-rendering.md,field-topology-status.md,camera-framing.md}`, `docs/game-mechanics/rendering-pipeline.md`, `src/native/preview/{preview.h,lighting.h}`, `src/native/assets/field_converter.h`, `build/scratch/oracle-golden/{manifest.json,town_driving.png,race_hud_driving.png}`.

**Interface:** pure `SkyGroundParams deriveSkyGround(lighting,scene)` and scene draw implementation using existing snapshot/terrain inputs. Optional native sky geometry is host-generated from runtime asset records, never an executed GIF/VIF/VU chain. Main.cpp ground mesh assembly changes are a coordinator glue recipe.

**Steps (global contract 1–7 applies):**
1. [ ] Turn B2 measured sky/ground/fog regions into parameter/colour tests; distinguish time-of-day and projection changes from shader errors.
2. [ ] Implement confirmed sky/layer gradient or runtime texture sampling, fog and ground modulation with reviewed color-space/GS-intensity interpretation expressed as native shader math.
3. [ ] Remove/contain visual underlay artifacts using data-backed tile coverage; visual fill is explicitly non-collidable and cannot conceal missing model/UV parsing.
4. [ ] Regenerate native shader header with the existing build procedure; test text/texture/alpha regressions and missing-assets safe fallback.
5. [ ] Produce fixed-pose town/race before/after captures and regional metrics through coordinator's serialized capture lane, separately from W2-12 camera changes.

**Acceptance:** `native_sky_ground_test` asserts B2 endpoint/interpolation values, finite fog at degenerate range, unchanged source geometry and bounded/missing-asset behavior. Additional integrated score goal: **town +0.05 absolute** against frozen 0.532 baseline, or if B2 proves a different bottleneck, report no gain and keep GAP rather than alter scoring. Sky/ground RGB residuals must satisfy B2's reviewed tolerances and title/menu must not regress. Target delta is a proposed acceptance goal, not a predicted measured result.

**Tier:** small-paid for grounded shader/constants work; medium if asset-layer projection or color math is unresolved. Coordinator reruns B2 PIL masks, checks unchanged camera/pose and compares regional improvements; reject a whole-frame color wash that raises histograms while destroying geometry.

## W2-11 | Retail minimap top-down content and clipped presentation

**Goal:** moving, correctly projected town/race minimap driven by real world/route content, without confusing the enhanced map with the retail viewport.

**Why now:** B8 blob classification and existing `MapCamera`/map-view contracts provide content and transform groundwork; no second-screen window work is necessary for a useful in-game map.

**Owned files:** new `src/native/assets/minimap_content.{h,cpp}`, new `src/native/preview/minimap.{h,cpp}`, new `tests/native_assets/test_minimap_projection.cpp`, new `docs/native-reference/native-minimap.md`, own W2-11 report. Existing `map_view.*`, `preview.*` and `race_hud.h` remain read-only.

**Required inputs:** `docs/native-reference/{minimap-blobs.md,map-view.md,hud-layout.md,nav-tables.md,world-geography.md}`, `docs/native-map.md`, `src/native/gameplay/{map_view.h,world_layout.h}`, `src/native/assets/{nav_routes.h,native_model.h}`, `src/native/preview/preview.h`, `build/scratch/oracle-golden/{manifest.json,town_driving.png,race_hud_driving.png}`.

**Interface:** `MinimapContent` contains bounded host polygons/lines with source provenance; `buildMinimap(content,pose,zoom,viewport) -> MinimapFrame`. Output includes a CPU RGBA viewport or clipped triangle list plus marker geometry, chosen once with coordinator. Existing `MapCamera` stays north-up enhanced-map authority; a separate retail projection uses the evidenced anisotropic viewport formula, not silently modified enhanced-map math.

**Steps (global contract 1–7 applies):**
1. [ ] Classify B8 blobs as geometry, symbols, encoded records or uncertain; write bounded synthetic decode tests. If none are map content, report it and use proven runtime map chains/route data, never turn blobs into invented houses.
2. [ ] Decode supported runtime map content to host primitives; use native math, not GS/VU replay. Preserve graph-only fallback as explicitly incomplete retail content.
3. [ ] Apply one world-to-map/retail transform to terrain/roads/player/markers; handle torus seams, tile-local offsets, Y flattening, heading and zoom source with citations.
4. [ ] Clip every primitive to evidenced minimap scissor, compose bezel/player icon using reviewed art/constants, and preserve HUD draw order. No triangles leak outside viewport.
5. [ ] Provide coordinator overlay glue plus projection debug metadata; enhanced-map marker overlays remain an explicit opt-in sibling feature, not assumed retail behavior.

**Acceptance:** `native_minimap_projection_test` asserts player-centered invariant, shortest seam displacement, non-sheared odd-row tile projection, correct heading axes, zoom scaling, clipping and identical transform for road/player landmarks. Existing enhanced-map tests remain unchanged. Optional local-disc check reproduces Peach 84 segments and **7056 ordered routes**, but that proves route logic, not minimap artwork. Golden landmark residual goal **≤1.5 px in the documented reference coordinate system**, matching map-view's established projection evidence; rescale correctly for 1280×447, do not use 1.5 output pixels blindly. Coordinator reprojects independently chosen landmarks and compares map motion across a driven sequence, not one static frame.

**Tier:** small-paid after B8 verifies format; medium/senior-RE for unresolved blob/zoom/projection semantics. A missing exact retail map source remains GAP even if enhanced roads look plausible.

## W2-12 | Evidence-backed chase camera, projection and view actions

**Goal:** remove ungrounded FOV/look-at assumptions, stabilize the driving camera through elevation/turns, and make view input functional without coupling render rate to simulation.

**Why now:** B2 horizon calibration and B5 speed/unit correction expose projection and speed-scaling errors independently of sky shading; existing camera constants include proven preset offsets but GameLoop still uses assumed `fovY=1.03`.

**Owned files:** `src/native/gameplay/{camera.h,camera.cpp}`, new `src/native/gameplay/camera_projection.{h,cpp}`, new `tests/native_assets/test_camera_projection.cpp`, new `docs/native-reference/native-camera-projection.md`, own W2-12 report. `game_loop.*` and `interp.*` read-only; W2-04/coordinator inserts approved helper calls.

**Required inputs:** `docs/native-reference/{camera-constants.md,camera-framing.md,sky-ground-calibration.md,gauge-calibration.md,frame-interpolation.md}`, `docs/game-mechanics/car-ai-and-camera.md`, `config/game_symbols.toml` (`0x21F698`, `0x21E690`, `0x21DC18`, preset `0x29CE00`), `src/native/gameplay/{camera.*,game_loop.h,interp.h}`, `build/scratch/oracle-golden/manifest.json` and town/race frames.

**Interface:** `buildNativeProjection(const ProjectionCalibration&,width,height) -> Mat4` with explicit field/progressive reference conversion; `CameraController` consumes authoritative pose/view-edge once per VBlank and emits eye/target/projection. New projection code does not reinterpret `viewNear=viewFar=2048` comments as valid Vulkan clipping planes. Preserve camera-state interpolation read-only in render path.

**Steps (global contract 1–7 applies):**
1. [ ] Re-read projection consumer instructions for focal 500, horizontal 0.47, vertical 0.8, origin/field scaling and preset pitch; derive host perspective mapping with explicit units. Do not equate GS projection constants directly with vertical FOV.
2. [ ] Write failing synthetic matrix/landmark tests plus cited preset/slew step tests, including ±π yaw wrap and elevated car pose.
3. [ ] Implement pure calibrated projection and chase/view preset selection. Verify actual number/order of view modes; unsupported modes remain GAP rather than invented first-person functionality.
4. [ ] Separate car world elevation, camera render-scale and snapshot/interpolation conversion; warp resets camera intentionally, normal driving never resets spring each frame.
5. [ ] Deliver helper/API call changes for W2-04/coordinator; compare matched-pose landmark/horizon captures before changing sky/ground parameters, and preserve aspect-ratio/widescreen policy.

**Acceptance:** `native_camera_projection_test` asserts finite invertible projection, correct near/far depth convention, no X/Y constant swap, expected synthetic landmark placement at 4:3 and wide output, shortest-arc yaw, verified pitch slew and no extra camera logic at 144 Hz render. Equal fixed-tick histories yield equal camera authority; interpolated view cannot feed back. Real-content closure requires at least three independently selected landmarks per matched town/race pose and a calibrated pixel residual table; unresolved oracle yaw must be solved from evidence or reported GAP, not fit away by moving scene geometry. Coordinator independently reads preset bytes, projects test points with a small Python matrix implementation and rechecks both scenes.

**Tier:** medium; senior-RE for field-scaling/projection basis ambiguity or irreconcilable landmark residuals. No arbitrary FOV tuning labelled PROVEN.

---

## Coordinator integration checklist — the definition of playable

These are coordinator work items, **not extra worker file ownership**. Reserve capacity for them; twelve green leaf tests alone do not satisfy the goal.

1. Register native targets and freeze public interfaces. Verify no native target links `ps2_runtime`, interpreter, GS, VU or generated guest objects; inspect both link command/dependency graph and binary symbols. `nm ... | grep sub_00` alone is insufficient proof of zero guest execution.
2. Wire W2-03 input to ordinary `session.update`, W2-02 commands/menu views, W2-04 real DriveScene/contact surface and W2-12 camera. Keep an explicit debug autopilot route, never the normal player path. Decide one source of truth for active scene/clock/pose and refresh facade from it.
3. Wire W2-05 pose→checkpoint→timer/rank→HUD/outcome; remove duplicate session timer and placeholder award path. Configure actual course/laps/entrant metadata and render opponent snapshots. A single-player time trial is a useful intermediate but does not close opponent-race parity.
4. Wire resident trigger→W2-07/W2-01 modal→W2-06 shop/effects→session return, and W2-08 save/Continue. Respect progress vs runtime-state separation: no promise of mid-race native save states from a progress-only image.
5. Wire W2-09 callback-safe sources/context queue, W2-10 native scene draw and W2-11 clipped minimap overlay. Cache runtime decoding at scene/load boundaries, not each draw. Keep missing-content errors visible to diagnostics without adding invented retail UI rows.
6. Add coordinator-owned `native_playable_flow_test` plus an ordinary-runtime input/socket scenario: New Game/Continue → leave factory → drive with steering/brake/reverse → enter a real resident/shop → buy/equip/cancel → complete an actual checkpoint race → results/return → save/restart/Continue; assert progress and geometry, not screenshots alone. Shop→race can be a licence-eligible Adventure route; Quick Race must not mint ungrounded purse rewards.
7. Run full baseline + all twelve new executable tests; feature matrix; equal-tick determinism with interpolation on/off; local-disc optional tests marked RUN/SKIP explicitly. Run synthetic parsers/store/contact tests under ASan/UBSan where supported. Missing oracle/graphics environment yields a blocked visual gate, not a green claim.
8. Capture title/menu/town/race at exact states under frozen comparator, include regional residuals and honest rebaseline reasons. Do not demand byte-identical snapshots against intentionally replaced mock simulation; demand deterministic new behavior and no unrelated menu/title regression.
9. Only coordinator rebuilds/deploys Android DEV, checks identifier/signing/data isolation, and performs a real-time physical-controller smoke (not lockstep), pause/resume, audio/station change and save/relaunch. Keep oracle runs isolated; native shipping target must not load an oracle dylib at first launch.

## Recommended execution order and parallel lanes

**Gate 0:** drain/freeze B1–B8, verify samples, take clean 73-test/score baseline (actual count if changed). Specifically resolve B4's format claim before W2-01. Approve input/scene/event/race/save contracts and coordinator CMake stubs so workers can compile privately without shared edits.

**Stage A — maximum independent foundational work:** W2-03 (input), W2-08 (save), W2-09 (audio), W2-01 (event runtime), W2-12 (camera), W2-10 (sky), W2-11 (minimap) may run in parallel within live provider/concurrency limits; no provider more than two lanes per board. File-disjoint does not justify oversubscribing heavy compiles/captures. Prioritize 03/08/01, then 12/09, then visual leaves when slots free. W2-10 CPU tests can proceed while coordinator serializes GPU captures.

**Stage B — first playable movement:** W2-04 starts after 03/12 contracts freeze and B2/B5 evidence is reviewed; 03/12 may still finish their tests independently. W2-06 starts once shop inventory/raw save-range contract is approved with 08; W2-07 starts after 01's public grammar/yield contract freezes. Those three are file-disjoint and may run in parallel. W2-02 can begin its isolated task/menu/state tests concurrently, but its final session wiring waits for subsystem closure.

**Stage C — race closure:** W2-05 starts after W2-04's pose/route/contact and W2-06's award policy interfaces freeze. It can implement synthetic race/HUD tests earlier, but real course closure waits for 04. W2-02 integrates the finished controllers after 05/06/07/08 settle; W2-04 and W2-02 must not invent competing race/session authorities.

**Stage D — coordinator assembly:** integrate in small verified slices: input+movement+camera → race+results → dialogue+shop+save → audio → scene/minimap. Run the complete real-time vertical slice and Android rehearsal. Coordinator alone changes app/CMake/platform and applies intentional snapshot rebaselines.

**Single highest-leverage batch: W2-04, authoritative drive simulation and real scene/contact binding.** W2-03's one-line app wiring is the fastest initial win, but W2-04 converts controls, town exploration, race checkpoints, camera, HUD speed and rumble from mutually inconsistent demos into one playable world. A beautiful sky, a generic event VM or green persistence tests cannot compensate for a car that moves on the wrong units through walls and cannot traverse the actual course.
