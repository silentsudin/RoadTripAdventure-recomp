# VRC-style agentic pipeline — wave board (user: keep going autonomously, commit+push at green)

## Loop (per wave)
1. Planner dispatch (opus-class, 1): verify premises vs repo+ROM, produce batch plan with
   disjoint file ownership, thresholds, scoring commands.
2. Worker dispatch (parallel, background): each worker = 1 batch, writes ONLY its files,
   NO git operations, evidence per claim (memory addresses, repro commands, numbers).
3. Coordinator verification (me): every worker claim re-checked independently — re-run their
   scripts/measurements; integration + commit only after my own checks pass.
4. Green = clean build + full native test sweep + feature matrix + determinism pair, then commit+push.

## Provider policy (user-mandated)
- Spread lanes across providers with quota. **NO omlx, NO strata** (local inference competes
  with my GPU). **opencode-go weekly exhausted (0%, resets Oct-19) — do not lane there.**
  zcp EXHAUSTED. zai weekly 67% but 5h window refills ~hourly; probe before dispatch.
  anthropic via AI credits (~$100 left, Oct 17) — prefer for RE-heavy lanes; synthetic
  weekly $24 resets ~23:35 daily.
- Refresh live health with the quota_status MCP tool (via execute) before every dispatch round.
- **HARD RULE (user, repeated): EVERY subagent tool call MUST pass an explicit `model:` from a
  REMOTE provider (anthropic/openai/xai/synthetic/…). Omitting `model:` silently inherits the
  coordinator's LOCAL omlx model — this happened 2026-10-18 ~01:20 (W2-04r2, W2-08b launched on
  omlx, user-cancelled). NEVER repeat. EVERY dispatch also MUST set `background: true` (user).**
- zai: dispatch FAILS "ZAI_API_KEY is not set" (2026-10-18 01:30) despite quota probe saying
  configured — treat zai as DEAD; do not lane there until a dispatch actually succeeds.
- xai: quota probe shows api_key_configured:false (2026-10-18 01:28) — same failure mode expected;
  treat xai as DEAD for dispatch too. As of 01:30 the ONLY usable remote provider is anthropic
  (openai 5h opens ~02:02 UTC, synthetic weekly ~02:56 UTC).

## Wave 1 — DONE (all 10 batches verified except B2 sun-badge claim REJECTED; integrated, pushed 0adad7d v14)
Scores v14: title 0.872 / menu 0.532 / town 0.597 / race 0.471. Full sweep PASS=73 FAIL=0.
Key artifacts: config/game_symbols.toml (B1), S1 sky bands + ambient floor 0.85 + XMODEM
font upload cache (M1), wave-1 RE docs (B2-B8, P1, P3). Wave-1 raw reports: .qoder/wave1/.

## Wave 2 — sonnet plan at .qoder/wave2-plan-sonnet.md (14 batches; Step 0 plumbing done)
Step 0 done: CMake EXISTS-guards for src/native/gameplay + tests/native_w2/*_test.cpp ->
w2_<stem> + tools/w2_*_main.cpp -> rt-w2-<name>. Workers still never edit CMakeLists;
Step-0 globs auto-pick new files. Workers build ONLY in private dirs build/w-W2-<NN>/.
Reports: .qoder/wave2/W2-<NN>-report.md.

### Batch A results (integrated+pushed 4ee96e0 for W2-01/02/03/14; W2-10 pushed 0373d33; W2-04 pending)
| ID | Task | Lane | Status |
|----|------|------|--------|
| W2-01 | script VM (InterpretScriptCommands) | anthropic sonnet-5 | VERIFIED (JT 0x2EC750 29 entries/27 distinct re-read; 0x23B0E0 ops 9/10/12 shared; branch_bleed=1 slot raw-read reported separately) + pushed |
| W2-02 | app_flow state machine | anthropic haiku-5 | VERIFIED (selector bytes 0x21CE08; 35/35 vectors; design-choice edges in app-flow.md; attract-timeout trigger NOT found) + pushed |
| W2-03 | vehicle engine-force model | xai grok-4.5 | VERIFIED (0x332f44=0x3fcdf3b6=1.609 double re-unpacked) + pushed; main.cpp binding I-3/I-4 pending |
| W2-04 | terrain/world Y | xai grok-4.7 | r2 FAILED: brief error (told worker to reuse w2_score_v2.py + stale 0.40 threshold — neither exists; worker correctly stopped). Superseded by W2-04r3 on anthropic sonnet-5-5: derive screen-space law fresh, gate 3 static poses <=0.100 px, driving residual informational, terrain_y_px column + w2_terrain_ymap.py. ACCEPTED AMENDED (user ruling): static/settled-pose Y exact; driving residual documented not thresholded |
| W2-10 | HUD live data | anthropic sonnet-5 | VERIFIED + pushed 0373d33 + **integrated into main.cpp race pass 2** (2P speed digits METER0 u=140+16(d%7) v=23(d/7), 16x23 cells @UI(573-17i,388); timer=frames-1; needle theta=pi(rpm/10000-0.7); rtRaceClockFrames counter) |

### Batch B results (delivered 2026-10-18; commit queue pending sweep)
| ID | Task | Lane | Model | Status |
|----|------|------|-------|--------|
| W2-05 | race scoring/laps/standings | openai (87% wk) | gpt-5.6-terra | VERIFIED (coordinator re-run): payout=30/30 licence_vectors=9/9 records=24+2 wgp_fuzz=1000/1000 rank_fuzz=1000/1000 bands=78/78 deterministic=1 |
| W2-06 | shop economy | openai | gpt-5.3-codex-spark | VERIFIED pins=125/125 traces=6/6 invariant_runs=1000 catalog=58 deterministic=1; cap grounding CLOSED: 999999 @0x23DC88/8C = lui v1,0xF + **OR-imm (opcode 0x0D)** 0x3463423F, not ORI 0x0C — constant scans must check both opcodes |
| W2-07 | dialogue/text engine (+vm-width arbitration) | anthropic credits | claude-sonnet-5-5 | DELIVERED, commit after sweep; ELF arbitration: script-vm.md right, gsl-vm.md §5.1 WRONG (op 0x03 width 4 / op 0x08 width 3; ops 9/0x0A/0x0C share stub 0x23B0E0). Fixes pending: gsl-vm.md §5.1, no-auto-wrap, arrow tab (304,416), escapes $A/$G/$N/$R/$S/$T/$U only, bound the hang/overflow paths |
| W2-08 | save format v2 | anthropic | claude-haiku-5-5 | r1 brief WRONG (v1 is raw 0x3448 progress block, not 6x2048+FNV). W2-08b: v2 layout accepted (32B hdr + crc table + 0x3448 image + TLV). Pins VERIFIED: 15/15 ROM words + save-path crc 3A7AD250 re-checked; roundtrip=6/6 migration=8/8 fuzz=1000/0-viol. Open: race-results default 0xFF×78 vs save.h 35 |
| W2-09 | audio runtime | anthropic (re-dispatched; synthetic key dead) | claude-sonnet-5-5 | DELIVERED, commit after sweep; corrections: EA=bend mult, E5=abs pitch+retrigger (supersedes sndmod-irx.md), delta 0 stalls 65535 ticks, F9 forks track; radio pan via playRadioPair; rumble 0x80–0x84 still unwired in main.cpp |
| W2-11 | minimap projection | anthropic (re-dispatched) | claude-sonnet-5-5 | VERIFIED (coordinator): scipy shift repro −6.15/+9.20; red-arrow centroid (205.1,366.6) 1.3px from 448-pred vs 10.9px from 480 → **mapping_verdict=448**; ROM re-reads: DAT_00332CAC=−0.47 (plan sign wrong), zoom=2 entries, race=39 (plan said 24), scissor=5; car (102,183) derived from both zoom rows |
| W2-12 | input mapping | xai (22% wk) | grok-build-0.1 | VERIFIED default_remap=10/10 conflicts=0 bits=16/16 roundtrip=1000/1000; ROM defaults save+0x12E8..=[40,80,20,02,04,01,08,10,08,100] matches kDefaultRemap |
| W2-13 | enhancement registry | anthropic (re-dispatched; hf id rejected) | claude-sonnet-5-5 | VERIFIED registry=50 coverage=51/51 ledger=17/17; scripts/w2_retention_gate.py PASS independently |
| W2-04r | terrain Y amended acceptance | xai continue | grok-4.7 | r2 BLOCKED correctly (brief cited w2_score_v2.py/project()/px law not in tree; worker stopped instead of fabricating — postmortem in plan-journal). Superseded by W2-04r3 (anthropic sonnet-5-5, fresh derivation authorized) |

## Wave 3 — DONE (integrated + pushed 7fecaa7)
Order W3-01 -> 06 -> 04 -> 02 -> 03 -> 05. Six workers delivered; all clean-room verified.
| ID | Task | Status |
|----|------|--------|
| W3-01 | proportional text metrics (pen) | WIRED — src/native/preview/text_metrics.h |
| W3-02 | race graphs by course byte | WIRED — directory stride 8 @0x002bd578 (no deref), 24B nodes, id11->6/id16->15 remaps @0x24FDC8; staticGraph(courseByte) |
| W3-03 | town collision grid | MODULE VERIFIED (collision_grid.h), NOT INTEGRATED — needs setTownCollisionGrid on GameLoop + resolve in integrateCar + build at NavRouteTables::open via townGraph(1) |
| W3-04 | save/load slot env seams | WIRED — RT_APP_LOAD_SLOT/RT_APP_SAVE_SLOT default OFF (matrix determinism); title save-menu UI remains GAP |
| W3-05 | event bridge | WIRED — 0x85..0x8B are rumble-hold bytes (NOT radio/cutscene); unknown-width records stop walk; drain before session.update |
| W3-06 | enhancements app registry | WIRED — RT_ENHANCE_* at env-read points, legacy RT_APP_* precedence; ssaa + physics_hz registered, NO application site -> logged GAP, not fake-wired |
Green sweep at 7fecaa7: 171 native + 25 w2/w3 binaries, 0 failures. NOTE: ctest is EMPTY by design — sweep = run build/native/*_test binaries directly + w2/w3 from repo root.

## Clobber incidents (journal)
- #1+2 (commits b01a7bc/a31b1d7): parallel session carried a clobbered main.cpp — mph-law /
  radio-move / town-golden-truth / gauge-note blocks absent from blobs; LOST from tree AND history.
  Recovered from coordinator constants; re-applied 2026-10-18 on top of 7fecaa7 (mph law Pass 2,
  town golden-truth constants, gauge-discrepancy note). Mitigation: commit every green increment
  IMMEDIATELY; workers never touch main.cpp.
- Radio-panel MOVE constants were lost and are NOT reconstructable from summary -> backlog: re-measure
  radio panel position vs oracle goldens before re-applying (never committed, low risk).

## Minimap static-window ruling RESOLVED (was wrong)
Fresh mask fit 2026-10-18 (coordinator, town_driving vs town_driving_street, navy mask L2<=30 over
field): residual 3276 @shift0 -> 496 @(+6,-9) png px; car dot at field centre in both frames.
=> MAP SCROLLS WITH CAR (north-up, car-centred) — earlier "byte-static" claim disproved.
Native car-centred projection is correct. Zoom: per ELF minimap.md §1 (mapViewMatrix 0x21F920):
x scale s=0.20 working estimate, y scale s*0.47 squash (DAT_00332CAC=-0.47) — equal-axis kKy=0.20
came from the disproved static reading; squash restored. minimap.md §0.1 needs this same update.

## Wave 4 — DISPATCHED 2026-10-18 ~11:50 (anthropic lanes until a dispatch ERRORS = credits out, user ruling)
| ID | Task | Lane | Status |
|----|------|------|--------|
| W4-05 | dashcam footage pipeline (AVP research + dashcam_source.{h,cpp}) | anthropic opus-5-5 high | DONE — src/native/media/dashcam_source.{h,cpp} + tests/native_w5 (234 checks ROM+disc+ffmpeg / 220 ROM-less); env dirs RT_DASHCAM_DIR>android-dev>RT_SAVE_DIR/dashcam>app-local>./dashcam; ffmpeg/ffprobe subprocess-only; registry rows dashcam_dir/ffmpeg_binary/ffprobe_binary added to enhancements.cpp table (DocOnly) + csv/json regenerated via rt-w2-enh_registry. KEY FINDING: ROM ships NO video (0 IPU/MPEG accesses; in-game replay = input re-render, AcquireVehicleInput 0x21A990, SaveControlStateForReplay 0x21CBC8, buffer 0x100000..0x1FFFF8 12B stride). Panel units RESOLVED: fit is oracle-1280-space -> source px: crop_from_design(x,y,w,h)=(3x, 1.5y+317, 3w, 1.5h); clip naming <C-stem>[_n].<ext> UNPROVEN. Not consumed by main.cpp yet |
| W4-06 | race HUD element layout (race_hud_layout.h + race_hud.h + fixtures) | anthropic opus-5-5 xhigh | DONE + coord-verified (463 checks pass vs real goldens+ELF, negative controls genuine) — 23 elements ROM+PNG confirmed within 2 design px; total/lap banners (20,20)/(490,20), ElementC panels y52, rank badge (40,88) digits 1:1, dial 140x128 @(484,294) [0x2A8FF0; (492,336) w132 = 2P anchor 0x2A9AF0], gear digit 20x14 @(581,354) u=(g%5)*20+140 v=(g/5)*16+80 g=car+0x1FF. Timer digits = SCORE.GSL page v=64 row pitch 14 (NOT FONTE 12 / METER0 17); capsule = CSA1 texel (32,24,64) @ alpha 0.509 (not opaque 87,109,159). NEEDLE: dial is 0..10 scale ~18.2deg/numeral, rests at 0 while display reads 39 mph -> NOT mph; law still GAP, nothing draws needle. PENDING coord: main.cpp integration, 4 doc corrections, SCORE.GSL @0x8350 + METER0 @0x8550 decode (stand-ins in place) |
| W4-08 | script VM opcode canon + ElementA F3/countdown (script_ops.{h,cpp}) | anthropic sonnet-5-5 high | DONE — script_ops.{h,cpp} 3 dialects TownPrefix/PadPattern/TsqTrack, 256-row width tables (op 0x03 w4, 0x08 w3, 0x0E w7), walkRecord; 1514 checks ROM / 617 ROM-less; rumble 0x85..0x8B = 133..139-frame holds via pad interp default arm 0x20A7E0; pad holds ALL bytes except 0x80..0x84 and FF (incl 0x00); delta-0 delay=65536; F3 unproven (falls to 0x5640 terminate arm); EA/E5/F9 = SNDMOD play 0x5294, not road sections. script_ops.cpp test-compile only — add to rt_native_assets when event-drain consumer lands |
| W4-09 | docs corrections (7 rulings) | anthropic sonnet-5-5 medium | DONE — all applied; its 7(a) static-window challenge RESOLVED above (scrolls with car); tsq-tvb-format.md:64/65/112 E5 text still stale (follow-up) |
| W4-10 | speed-law constant harmonization | anthropic haiku-5-5 high | DONE — hud_live.h + hud_layout.h + new w2_hud_layout_test + patches A(hostMph clamp)/B(hud_live_test:327)/C(test_hud_layout rect+origin) applied by coordinator. OPEN: shippedMphFromRaw (216/1.609 chain) contradicts 55.07 law — unreconciled, not displayed path; 2P METER10 geometry unverified |
Provider snapshot 11:48: openai 80% 5h / 73% wk; anthropic credits confirmed WORKING (5 dispatches accepted despite probe 429); zai probe "configured" but dispatches failed on key — still dead until a dispatch succeeds; opencode-go wk 0% banned.

## Wave 5 — DISPATCHED 2026-10-18 ~12:45 (anthropic lanes until a dispatch ERRORS, user ruling)
| ID | Task | Lane | Status |
|----|------|------|--------|
| W4-11 | gauge needle input law (tach hypothesis: needle angle source in UpdateRaceHud 0x2490A0 handler 0) | openai gpt-5.6-terra | DONE + coord-verified (fresh build, checks=8 failed=0, negative_control=CAUGHT, ROM-less SKIP ok; ROM chain disassembled independently: handler 0x249DC0 jal 0x249520 lh car+0x1D0 cap 10000, div.s 10000.0f @0x332F50, add.s -0.7f @0x332F54). LAW: theta=pi*(rpm/10000-0.7), 0=up CW; goldens consistent (117 RPM coast 39mph / 6217 RPM @63mph under CCW=90deg-theta_cw). Native rpm input still GAP -> needle stays undrawn; kRaceHudNeedleLawResolved flip = coordinator call at integration commit |
| W4-12 | decode SCORE.GSL unit @0x8350 + METER0.GSL @0x8550 gauge page into HudArt (kill W4-06 stand-ins) | openai gpt-5.6-terra | DONE + coord-verified (fresh w12_hud_pages_test passed + native_hud_art_test green). HudArt += lapDigits (CSA7 cells u=lap*8 v=200 8x16), lapDisc (CSA8 u=232 v=168 24x24), gaugePage (CSA12 128x120 needle 3,53,8,59 hub 16,80,12,12 arc 40,48,20,60). Swap into W4-06 integration |
| W4-06i | integrate race HUD into main.cpp (coordinator) | coordinator | DONE 2026-10-18: race block draws via raceHudLayoutSprites grouped by page/CLUT (Score0 csa0 -> raceAtlas [empty, rank off], csa1 -> raceBanner, Score1 csa7 -> lapDigits, csa8 -> lapDisc, authored fallbacks retained); dial Pass1 now Meter0 pass @ (484,294) w140 (ROM 0x2A8FF0; was 492,336 w132 = 2P anchor); textured capsule alpha 0.509 replaces flat pill; kRaceHudNeedleLawResolved flipped TRUE (+w4 test expectation) = law-resolved, still undrawn pending native rpm; showRank=false (no rival cars); lap block real via snapshot.lap + coordinator lap-frame reset. Gates: build 0err, sweep 106/106, matrix 32/0/0, determinism PASS |
| W5-01 | engine RPM law from ROM (car+0x1D0 source) | openai gpt-5.6-terra | DONE + coord-verified (w13 checks=9 failed=0, negative_control=CAUGHT, in sweep). rpm = trunc(longitudinal_motion*128/gear_ratio) cap 0..10000; two auto-shift policies + boost targets 3000/9000 in gauge-rpm-source.md; calibration residual -1 RPM both goldens (106->116 vs ~117, 5634->6216 vs ~6217). GAP: mph alone cannot yield rpm — needs gearbox + longitudinal motion state |
| W5-02 | docs corrections batch (4 rulings) | openai gpt-5.6-terra | DONE + coord-verified (rumble-haptics: 0x85..0x8B = 133..139-frame holds, all bytes except 0x80..0x84+FF incl 0x00 via default arm 0x0020A7E0; tsq-tvb: EA/E5/F9 = SNDMOD play 0x5294 dispatches not road sections; hud-layout: TOTALTIME (20,20) uncentred 0x002EC1A8 + LAPTIME badge (490,20) 0x002EC070; race-hud-atlas: rank digits 1:1 28x30 + capsule (32,24,64) alpha 0.509). Shipped in f760280 |
| W5-03 | race-state input ROM sources (place / lap frames / gear) | openai gpt-5.6-terra | DONE + coord-verified (w14 checks=6 failed=0, negative_control=CAUGHT, in sweep). place = car+0x247 byte (RankCarsByProgress write 0x0022d8a0); lap frames = car+0x260 u32 (inc 0x0022e100..108, reset at boundary 0x0022d4c0); gear = car+0x1FF byte (write 0x0021a38c, GetVehicleGear 0x002495bc). Native: lap done via snapshot; place needs rival grid; gear needs sim gearbox |
Provider switch ~12:57: Anthropic dispatches now ERROR "x-api-key header is required"; USER RULING 2026-10-18: **Anthropic is no longer an agent provider** — dispatch openai/gpt-5.6-terra (or other remote providers) only from now on. Never opencode-go, never local (strata/omlx). W4-11/12 running on openai (ses_eb11ecce5ffeYDraODK1bFn31V, ses_eb11e82b0ffey4yWVPwk0GF9L4). Patch D APPLIED by coordinator (race_hud.h stale (532,385) comment -> mph law). Matrix settings-row hang under investigation (stack capture watchdog armed).

## Open GAPs / arbitration (coordinator-owned)
- WAVE-6 LANDED 2026-10-18 (mine=2 verified per worker; all four re-run green on canonical build):
  W6-01 audio bus mixer RESOLVED-RETIRED 2026-10-18 (coordinator): parked
  src/native/audio/audio_graph.{h,cpp} + tests/native_w15 DELETED — zero consumers, voice
  machinery duplicated by committed gameplay/audio_graph.{h,cpp} (ROM-grounded: voice
  classes, stealing, SPU gain, radio pairs); the only unique idea (named per-bus gain +
  Q16.16 ramps, no consumer today) is documented in committed HANDOFF-W6-01.md and lands
  in gameplay/audio_graph.h when a feature needs bus controls (e.g. separate music/SFX
  sliders). Deletion beats addition. W6-02 title save/load UI integrated
  (preview/title_ui.{h,cpp} + w16 registered, checks=13 CAUGHT; main.cpp wiring pending).
  W6-03 mph/gear/minimap reconciliation (gear_glyph.h + w17 checks=15 CAUGHT: g==0=R, mph
  55.07 confirmed, kKy=kKx*0.47 via ROM float 0x332CAC). W6-04 driveline rpm/gear exposure
  (game_loop.{h,cpp}, w18 checks=13 CAUGHT first_shift_frame=114): FrameSnapshot.gear/rpm now
  exist; main.cpp gear-glyph wire pending, needle still withheld pending adjudicated art.
- STATUS 2026-10-18 late: commit A = 3891052 pushed (green: sweep 109, matrix 32/32 rc=0,
  det pair). Commit B = gear-source wiring in main.cpp (raceHudLayoutStateFor +
  dialState take snapshot.gear; race_hud.h comment). B GATES GREEN: matrixB 32/32 rc=0;
  det pair race@FRAMES=110 byte-identical rc=0 sim=110 both runs; gear digit "1" pixel-
  verified in race snapshot frames 110+116 (serif stem + flag at design ~(581,354));
  sweep 110/110 (incl. new w19). Live gear-VALUE tracking stays unit-covered only
  (w17 cells, w18 evolution first_shift=114): the headless autopilot race finishes at
  frame 117 before any shift reaches the dial — documented shortcut in build/scratch/wB.
- FREEZES RESOLVED (W7-02, build/scratch/w7-02/REPORT.md): race "freeze at 117" = the
  1-lap session FINISHED (GameSession(1)); headless RESULTS is terminal (only Return/
  input.confirm leaves it, main.cpp:2134). "Explore freeze sim=1" = by-design contract:
  EXPLORE ticks only when socket-driven (session.cpp:52, pinned test_session.cpp:97-102).
  Repro-footgun: `rt-app RT_APP_FRAMES=N` puts vars in argv which main ignores — env
  must come BEFORE the binary. Backlog P1: headless autopilot driver (auto-continue
  RESULTS + EXPLORE idle-tick, gated on RT_APP_MODE) to unblock live HUD tracking.
  Earlier explore det pairs at sim=1 were trivially identical — future det gates must
  assert rc=0 AND simulation_frame==FRAMES.
- W7-03 DELIVERED (w20, checks=22 green, verified by re-run): collision grid reuses
  W3-03 OccupancyGrid as-is; resolves the 84-vs-43 nav-entry discrepancy (84 = Peach
  Town dir 0x2d98a8, 43 = static dir 0x2bd578 — both docs correct for their directory).
  WIRING DEFERRED (ruling): cellUnits=1600/84≈19 but road half-width=60 → resolve()
  as-is = invisible walls on tarmac; must scale cell to road width (or per-town extents)
  before integrateCar wire. w20 REGISTERED (coordinator): checks=22 green via CMake build,
  true-SKIP verified (env -i HOME=<empty> → exit 0, checks=0); collision wire = backlog.
- WAVE-8 DISPATCHED 2026-10-18 16:3xZ (openai/gpt-5.6-terra, bg; openai 5h=100% wk=61%):
  W8-01 title save/load UI main.cpp wiring + RT_APP_STATE=LOAD smoke (ses_eb05d398...qla4Z2);
  W8-02 P1 headless autopilot driver RT_APP_AUTOPILOT (new Autopilot module + w22 unit test;
  gates incl. ON-explore sim=300 and ON-race passes-117-terminates; ses_eb05d398...t3Bjia);
  W8-03 collision sizing repair — free-corridor covers road ribbon (half-width 60 + margin),
  w23 test, integrateCar wire proposal only (ses_eb05d398...W5V9n8m);
  W8-04 rotated-quad preview path + drawGaugeNeedle (preview layer only, NO main.cpp; w21
  test; coordinator wires dial after; ses_eb05d398...yBNqN3xf). Lanes: W8-01/02 both touch
  main.cpp in different regions (title/menu vs session-loop) — clobber watch at integration.
- WAVE-7 dispatched 2026-10-18: W7-01 needle draw procurement (openai, gauge_needle.h +
  w19 test + docs — needle bitmap/cell/pivot/rotation from handler 0x249DC0 path);
  W7-02 explore-freeze diagnosis (synthetic GLM-Flash, report-only); W7-03 town collision
  grid evidence + integrateCar wiring proposal (synthetic DeepSeek-Flash, tests/native_w20).
  All new-files-only, no main.cpp/CMakeLists touches.
- W7-01 VERIFIED + LANDED (commit C): coordinator re-ran test (checks=12 failed=0, CAUGHT),
  ROM-checked cited floats (10000.0/-0.7/pi at 0x332F50/54/84) and cell immediate
  (addiu 0x35 at 0x249E44); helpers uiArenaRotQuads/uiSetSpriteClamp/uiArenaBeginBatch
  already named in game_symbols.toml. Fixed worker CMakeLists to house convention
  (w19_gauge_needle_test target, rt_native_assets link) and converted assert() to
  NDEBUG-proof check() accumulator. Spec: cell (3,53,8,59) in METER0 CSA12 page @0x8550,
  pivot dial+(70,68). GAP: preview textured API is axis-aligned — needle stays UNDRAWN
  until a 4-vertex rotated-quad draw path exists (backlog).
- HUD PIXEL FALSE ALARM CLOSED 2026-10-18: "race HUD zero pixels" was my PPM parser's wrong
  header offset (14 vs 16 bytes), not the app. composite diagnostics prove dial ~24k px,
  banner ~6.9k, digits ~17k blend per frame into the 800x600 buffer. Evidence
  build/scratch/w406int/{race_hud4.png,race4.log}. RULE: parse PPMs with PIL, never by hand.
- ~~Speed unit scale~~ RESOLVED: SPEED LAW V=floor(word/55.07) clamp 0..186 mph, METER0 digits
  (573,388) step -17 16x23 u=140+16(d%7) v=23(d/7), white; mph baked in METER0; badge 556..604x389..416.
- ~~Digit page 1P vs 2P~~ RESOLVED: METER0 (speedDial) for 1P; METER10 kept only for 2P handler-10.
- ~~Needle law~~ RESOLVED by W4-11 (coord ROM-verified): theta=pi*(clamp(rpm,0,10000)/10000-0.7),
  0=up positive CW; input = signed car+0x1D0 engine RPM (NOT mph). Goldens consistent: tip -146.11 deg
  (CCW atan2) = 90-(-123.9) -> 117 RPM coast @39mph; +104.09 -> 6217 RPM @63mph. ROM: handler 0x249DC0
  -> CalculateTachometerValue 0x249520 (lh +0x1D0, cap 10000 args (car,0,1)), /10000.0f -0.7f, half-turn
  into uiArenaRotQuads +0x88. REMAINING GAP: native has no rpm source (AdvanceVehicleMotion 0x218EE0
  gear-ratio chain; W5-01 derivation: rpm = trunc(motion*128/gear_ratio), residual
  -1 RPM at both goldens — driveline wiring pending) -> needle undrawn until physics
  exposes rpm. kRaceHudNeedleLawResolved flipped TRUE 2026-10-18 (law-resolved, not wired).
- ~~W4-06 pending coord~~ DONE 2026-10-18: main.cpp race block integrated (builders +
  W4-12 textures + Meter0 dial pass at (484,294) + textured capsule + real lap block;
  mph Pass 2 untouched, no needle). 4 doc corrections applied (W5-02; the
  gauge-calibration needle row was W4-11's own update).
- W4-06 native inputs: lap DONE (snapshot.lap + coordinator lap-frame reset in
  main.cpp); place (car+0x247 per race-state-inputs.md) needs a rival grid —
  showRank=false meanwhile; gear: W6-04 exposes snapshot.gear (0=R,1..5) — the
  main.cpp dial passes still feed provisional 1 until the wire lands (next commit).
- ~~W4-10 item: shippedMphFromRaw vs 55.07~~ RESOLVED by W6-03 (driveline-exposure.md +
  w17): ROM carries TWO independent velocity paths — the HUD display law is
  floor(word/55.07) (goldens match, re-pinned); the separate *216/1.609 path at
  0x00249508 is another velocity consumer, not HUD mph. hud_live::shippedMphFromRaw
  models the latter and must not be reused for the HUD.
- W3-03 collision grid integration (see Wave 3 table).
- script-vm 0x85..0x8B rumble wiring into main.cpp event drain.
- Title/save-menu UI; ElementA countdown + F3; ssaa/physics_hz application sites.
- Dashcam fit refinement (73.3% IoU at scale 1.5, yoff ~317).

## Score history
v9 0.655/0.703/0.478/0.481 · v10 0.872/0.706/0.597/0.471 · v12 same+car span 2.2 RU ·
v14 0.872/0.532/0.597/0.471 (menu honest lighting).
