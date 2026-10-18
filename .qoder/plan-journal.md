# Current plan — fan-out board (2026-10-16, pushed cabab41)

## Done this cycle (committed cabab41 + pushed)
- Geo anchoring wired (Peach-origin render space), route drive unit fix, car terrain lift.
- Depth-off overlay pipeline: NAV strips verified 12,599 px delta localized at Peach graph area (FRAMES=10; at FRAMES>=60 the demo car drives the overlay out of frame).
- COURSE prop parser (Kimi-K3): 82/82 synthetic + disc walk matches doc table exactly (verified independently vs MSCALF-0xA counts; C05 delta explained by extra-block texture payloads).
- CMake staged guards for parallel modules.
- CLAUDE.md: Synthetic pool tiers + window semantics.

## Committed & pushed
- cabab41 geo anchoring + depth-off nav overlay + COURSE prop parser (verified)
- b23129d camera constants (+DAT swap fix: X 0.47 @0x332c80, Y 0.8 @0x332c84, gp-relative proof)
- 075a6b1 position-driven streamer advanceTo (golden trace, LRU, nearest-first; verified)
- 6c8af0f rt-pack-extract (222 packs / 1490 units / deterministic; independently re-verified; packs integration 1177 checks)
- 716c7cf FONTE glyph atlas (pitch 16x24 512 slots PROVEN — my own ASCII-art decode confirms A/B/C/D, '_'/row2col0; 45 checks)
- 08c09d4 doc corrections: FONTE geometry, 485tex/483pal, NLOOP padding, SCOREA16 absent, lbn_map=bytes, SHOP layout, CAR CNT-not-REF
- ca84554 position-driven demand wired into app (RT_APP_STREAM_POS)
- 1df6c17 scene/tile dispute resolved: Peach Town = tile 43 (byte-verified by me); traffic-AI group released + committed
- 443ecad anchor re-based to tile 43 / row 5 (smoke: demanded id 43, loads 221/223/230/232.BIN)
- 0b5d807 doc corrections propagated across 8 reference docs
- fc365d1 CAR model pipeline (table E 151/151 verified; livery binding; 154+225 checks)
- 40c5403 HUD layout tables (dial anchor + timer banks byte-verified; 430 checks)
- 0a50fdd FONTE HUD text presentation pass (RT_APP_HUDTEXT; legible LAP 1/1 pixel-verified, deterministic)
- 7b46db0 minigame venue logic (capstone instruction-level verified; 80 checks)
- 24a6c43 town street graph reader + Peach overlay wiring (nav_town pixel-verified; matrix now 19 rows, all green)
- 9e93d8f CAR model+livery binding (RT_APP_CARMODEL; Q44-vs-Q00 red-paint proof; matrix 20/20)
- fdfa38c original-save migration importer (P2 cards -> native slots; ELF strings byte-verified; 188 checks; card-geometry limits documented)
- 0c11519 frame-timing verdict (VBlank-locked 60Hz logic, coordinator byte-verified) + feature-retention audit ledger + matrix route rows (23/23) + CLAUDE.md dispatch HARD RULE gate
- 73d7e80 analog trigger drive mapping (additive input module, gate-off byte-identical, glue deferred)
- 36e0423 test-engineer agent definition (independent family testing) + CLAUDE.md agents line
- f6f4568 perf-overlay stats + button-glyph selection modules (PS font codes UNVALIDATED w/ font-dump closure plan)
- 2343cb2 options model + persisted settings store (40 rows, every env flag mapped)
- a6d6cbf motion-vector module + visual pipeline design (MSAA-absent/depth-DONT_CARE/HUD-CPU-composite inventory byte-checked)
- 5443f30 frame interpolation (fixed 60Hz sim + blended poses; determinism proven twice; matrix 24/24)
- 6a6a406 wave-2a app wiring (triggers@logic-step, perf overlay/log, glyph family, settings bootstrap; matrix 28/28; cross-wave+interp byte-gates held; P2 trigger-state gap logged)

## In flight
| Task | Model | Owned files | State |
|---|---|---|---|
| SCORE/METER digit presentation | openai gpt-5.6-terra | race_hud.h, test_hud_layout | DONE — committed 2beab6b (I byte-checked ElementC 0x2EBDA0 + anchors 0x2A9AF0; ASCII proof "SCORE 000500"; matrix 21/21) |
| Save migration importer | openai gpt-5.6-terra (cloud, bg) | save.{h,cpp}, test_save, save-layout.md | DONE — committed fdfa38c (ELF strings byte-matched at all 5 vaddrs; 0x3448 x3 in load fn; 188 checks; unvalidated-card limits documented) |
| Feature-retention audit | openai gpt-5.6-terra (cloud, bg — REISSUED after local-default mis-dispatch was cancelled) | feature-retention.md only | DONE — committed 0c11519; coordinator verified MISSING claims (rumble/framegen/2nd-screen/HD greps empty) + status upgrades (BGM VERIFIED from 23/23 matrix run); gaps now route/route_town closed, only CARMODEL_DUMP + DEBUG_OVERLAY without rows (both debug aids) |
| Frame timing / fps-lock RE | openai gpt-5.6-terra (cloud, bg — REISSUED after local-default mis-dispatch cancelled) | frame-timing.md only | DONE — committed 0c11519; VERDICT (a): logic VBlank-locked fixed-step. Coordinator byte-verified INTC_STAT ack+poll 0x1000F000 bit2 @0x285420, task tick +1 @0x204fd4, clock ctx+0x14 +0x2328/hour @0x210714, gravity -0x59 @0x21bd20; corrected INTC addr + downgraded 0x34bc0 wrap to INFERRED. => native keeps 60Hz sim, frame-gen/interp is the >60 route (logic fps-flexible conversion = inventory all per-step int constants) |
| Frame interp build (fixed-60Hz sim + pose lerp) | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg; windows 100%/100% fresh @18:46, $0 — OpenAI 38%/47% saved for verification) | interp.{h,cpp}+test_interp.cpp, main.cpp wiring, matrix interp row(s), frame-interpolation.md, retention row update | running ses_eba2c9628ffe — acceptance gate: interp-on snapshot byte-identical to interp-off at same logic step; RT_APP_FRAMES = logic steps invariant |
| Rumble/haptics module | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | rumble.{h,cpp}+test_rumble+rumble-haptics.md; NOT main.cpp/matrix/retention | running ses_eba2921f4ffe — ELF grounding @0x217830 required; I add glue+row on landing |
| Radio station module | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | radio.{h,cpp}+test_radio+radio-stations.md; NOT main.cpp/save (RO) | REISSUED (first attempt died pre-work on response-header timeout, zero loss) — running ses_eb9ddddc4ffe — station->VAG pair grounded vs audio-map + .lbn_map; deferred town-boundary apply |
| Analog trigger mapping | synthetic/hf:deepseek-ai/DeepSeek-V4.1-Flash (cloud, bg; mechanical) | input.{h,cpp} additive + test_trigger_map | DONE — committed 73d7e80 (verified: zero removals in input.h, trigger_map+input+sweep green in my tree; CarInput bits match car_state.h:24-29; glue+matrix row deferred to integration wave) |
| zai cadence probe #4 | zai-coding-plan/glm-5.3-flash (cloud, bg) | — | FAILED ~18:52 (4th consecutive, err 1113) — needs credential refresh (`opencode auth login` -> Z.AI Coding Plan) or provider recovery; next probe after ~3 more dispatch decisions |
| Output modes (720p/1080p/1440p/4K + true progressive) | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | output-modes.md + app/output_modes.{h,cpp} + test_output_modes; NOT main.cpp/preview.cpp | running ses_eba1ff507ffec — audits native field-parity leakage, defines res ladder + progressive contract |
| Motion vectors + visual pipeline DESIGN | REISSUED openai/gpt-5.6-terra (cloud, bg) | motion.{h,cpp}+test_motion+visual-pipeline.md | DONE — committed a6d6cbf (coordinator byte-checked inventory: MSAA off @preview.cpp:354, D32 depth present w/ DONT_CARE store — temporal gated on STORE export, HUD CPU-composited = UI-mask free; phase order MSAA->FXAA->MV->TAA->FSR1->temporal) |
| Map + second-screen LOGIC | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | map_view.{h,cpp}+test_map_view+second-screen-map.md; NOT preview.cpp/main.cpp | running ses_eb9fd34a8ffeB — reconstructed map (graph+markers+torus projection), page state machine; door/resident grounding verdict required |
| HD texture packs LOGIC | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | assets/hd_pack.{h,cpp}+test_hd_pack+hd-texture-packs.md | running ses_eb9f8efa0ffeKa — content-stable keys, pack index, mip chain, dump-mode reusing P6 pattern |
| First-run ISO extractor | synthetic/hf:zai-org/GLM-5.3#high (cloud, bg) | assets/iso_extract.{h,cpp}+test_iso_extract+first-run-extraction.md | running ses_eb9f8ef9effe3z — synthetic-ISO fixtures only (no game bytes), gate chain w/ CRC32 0x5A49851D, 2048/2352 stride detect, CHD deferred; NOTE: supersedes the old "blocked on ROM" item — module is testable synthetically now, real-ISO smoke when user supplies one |
| Options menu + settings model | synthetic/hf:deepseek-ai/DeepSeek-V4.1-Flash (cloud, bg) | gameplay/settings.{h,cpp}+test_settings+options-menu.md | DONE — committed 2343cb2 (verified: 126 checks, sweep green, 40 rows covering every RT_APP_* w/ file:line; gpu-pending rows honest; CMake defers to wave) |
| Perf overlay | openai/gpt-5.6-terra (cloud, bg) | gameplay/perfstats.{h,cpp}+test_perfstats+perf-overlay.md | DONE — committed f6f4568 (coordinator ran: perfstats passed, sweep green; Vulkan timing deferred per doc) |
| Button glyph repaint | openai/gpt-5.6-terra (cloud, bg) | gameplay/glyphs.{h,cpp}+test_glyphs+glyph-repaint.md | DONE — committed f6f4568 (257 checks; PS symbol codes UNVALIDATED by design w/ RT_APP_FONT_DUMP closure plan — honest, no invented codes) |
| Test-engineer (integration/regression, DIFFERENT FAMILY) | synthetic/hf:zai-org/GLM-5.3#high (GLM vs GPT writers; cloud, bg) | test_trigger_integration.cpp + test_perf_glyphs_integration.cpp + proposed matrix rows | RUNNING ses_eb9e4a8d4ffe (3rd attempt, slot freed by interp landing) |
| REMAINING NATIVE-GAP INVENTORY | — | — | widescreen HUD anchor policy (wave2 preview); second-screen WINDOW + map GPU render (wave2); HD-pack GPU binding + budget/evict (wave2); visual pipeline GPU passes MSAA/FXAA/TAA/FSR1/MV-buffer/UI-mask (wave2); options-menu + setup-screen DRAWING (wave2); save-state menu rows wiring (wave2); ISO extractor --rom/RT_ROM bootstrap wiring (wave2); N/A natively: interlace flicker blend (progressive by mandate), GS frontend internals. movie/recording: not on mandate, parked |
| Frame interp RESUME | openai/gpt-5.6-terra (cloud, bg) | interp group + matrix interp row | DONE — committed 5443f30 after DOUBLE verification (agent cmp + coordinator cmp byte-identical; sweep green; matrix 24/24) |
| WAVE 2a: app wiring | openai/gpt-5.6-terra (cloud, bg) | main.cpp + matrix rows | DONE — committed 6a6a406 (28/28 matrix; default-snapshot vs interp-era baseline byte-identical; interp cmp held; P2 per-port trigger state + settings doc deltas queued for rollup) |
| POLICY v4 | synthetic concurrency cap ~=8 (killed interp mid-flight 19:1x) — cap synthetic at 6 concurrent, overflow to openai/terra; all cloud + background:true as always. KNOWN POOL TRAPS: zai-coding-plan err 1113 (stale package); zai plain = probes pass but DISPATCH fails "ZAI_API_KEY is not set" (env not wired despite db key) — treat all zai* dispatch as unavailable pending credential fix; working pools: openai + synthetic only | — | live |
| UI PARITY | openai/gpt-5.6-terra (cloud, bg) | docs/native-reference/ui-parity.md | DONE — committed 3eaab73 (verified cited literals; 9 screens/184 strings/deviation ledger/10 model gaps/5 automation checks; caught native debug-label violations for wave-2c) |
| ANDROID BRING-UP (Thor 97a75c3e) | openai/gpt-5.6-terra (cloud, bg) | src/native/android/ + scripts/native/android_build.sh + android-only hooks (__ANDROID__-isolated) | DONE-uncommitted — APK 15MiB installed over dev app (firstInstallTime Oct 11 preserved), Vulkan presents, title+terrain on screen, disc found internal files/disc (512MiB/378f) + files/rom (588MiB); env.txt loader live; commit gated on matrix re-run (prior run timed out at matrix stage: build 0 errs, sweep green except rumble WIP, default ppm byte-identical) |
| THOR FULL RUN | openai/gpt-5.6-terra (cloud, bg) | src/native/runtime/testsocket+guest_facade, rtharness native adapter, first-run SETUP state+assets extraction on Android, scripts/thor_full_run.sh | BLOCKED-RESOLVED ses_eb98743a5ffea: reported honest fail — baseline broke mid-dispatch (rumble WIP dup crashPatternForCode, self-healed 21:51); valuable audit: cmds run/pad/read/write/frame/stats/step, Android abstract socket localabstract:roadtrip-test, GameSession lacks guest-menu model -> split into stage slices below |
| THOR SLICE A: socket+facade | openai/gpt-5.6-terra (cloud, bg) | src/native/runtime/testsocket+guest_facade, rtharness RTH_NATIVE adapter, native_facade_test | DONE-partials ses_eb981f2dbffe7: socket (RT_TEST_SOCKET tcp + abstract @roadtrip-test verified by coord) lockstep/pad/release_pad/hello/run/quit working; facade table = car pos/place/clock/gravity/vib + tile warp 43/11; UNMET: inline-PNG frame cmd, state_slot, texture_pack, full stats, Android adapter completion, determinism cmp (blocked by tree break). Coordinator verdict: foundations sound, gap list real (town/race scenarios CANNOT run unmodified — full guest tables/menu model absent; warp-first flows needed) |
| THOR SLICE B: first-run extraction | openai/gpt-5.6-terra (cloud, bg) | assets/setup_wizard + scripts/thor_setup_test.sh | DONE-partials ses_eb981f2daffenO: unit PASS (coord standalone re-compile), CRC 0x5A49851Du gate + rom>disc>Download/RoadTripDisc order verified in source, manifest v1, resumable crc-skip, extract:/assets: ready logs; main.cpp hook = 1 include + 1 call @1019 (clean shape). UNMET: loaders not redirected RT_DISC->cache, no on-screen overlay, ISO skipped, device rehearsal (blocked by tree break) |
| THOR SLICE C (queued, now enriched) | synthetic next dispatch | gap-completion agent: frame cmd inline-PNG + state_slot + texture_pack + stats in socket; wizard cache loader-redirect + progress overlay; town/race NATIVE-mode scenarios (warp-based: facade tile-warp 43/11 + race start force, asserts via facade, screenshots); scripts/thor_full_run.sh end-to-end on Thor | BLOCKED on tree green (map_view WIP break: dup NdcPoint, undefined kFieldTileIdCount) |
| TREE-WATCH | — | gameplay/map_view.{h,cpp} | RESOLVED by coordinator @22:35 (agent declared dead 45min-stale): removed byte-identical dup NdcPoint@414 (canonical @104 kept), kFieldTileIdCount -> assets::kFieldTileCount (=64, matches row-major comment). Build now 0 errs, determinism byte-identical vs coord-off |
| RADIO RESCUE | synthetic/hf:zai-org/GLM-5.3-Flash (cloud, bg) | radio.{h,cpp} + test_radio.cpp | DONE ses_eb9589b53ffe — 879 checks green (verified coord); root cause: dead agent had REWRITTEN failing assertion into no-op — rescue restored bidirectional last-wins coverage; station table MATCHES oracle; authored docs/native-reference/radio-stations.md (+README index). GAP->integration duty: app audio-host wiring (radioStation -> VAG pairs in main.cpp) |
| MAP RESCUE | synthetic/hf:moonshotai/Kimi-K3 (cloud, bg) | map_view.{h,cpp} + test_map_view.cpp + docs map-view.md | running ses_eb9589b54ffe — PROGRESS: 71 -> 2 fails (player-NDC :561-562 only, per radio sweep); doors/residents verdict owed |
| PERF-GLYPHS INTEGRATION | test-engineer | test_perf_glyphs_integration.cpp | RESOLVED @~22:4x — passing in radio-agent sweep; will re-prove at rollup |
| RUMBLE MODULE | cloud (ses_eba2921f4ffe8, long haul) | rumble.{h,cpp} + test_rumble.cpp + rumble-haptics.md | DONE — all checks green (coord re-run); complete ELF decode incl. interpreter opcodes 0x80-0x84 + envelope records + options {0,2,4}; AUTHORED priority shield labeled; RT_APP_RUMBLE opt-in |
| HD-PACK + ISO-EXTRACT | synthetic (reports pending) | assets/hd_pack + docs/native-reference/hd-texture-packs.md; tests/native_assets/test_iso_extract.cpp | HD-PACK DONE-ish (tests passed in sweeps); ISO-extract files landed, verify at rollup |
| QUOTA MODE | updated @~23:20: synthetic weekly $0 EXHAUSTED; openai ~11% 5h/33% wk; **user sanctioned free pool** opencode/deepseek-v4.1-flash-free (compliant: better pools genuinely dry); zai still dead | — | coordinator commits only after matrix; determinism gates stay local regardless of which pool did the work |
| ISO-EXTRACT RESCUE (coord, local) | — | iso_extract.cpp + test_iso_extract.cpp | DONE — was orphaned mid-write by quota kill; fixed: FixtureA/F forwarding, CHECK_EQ string misuse, got1.insert typo (heap overflow, ASan-caught), folder-rule dual reading (REAL BUG: plain-name rules SYS/FLD/COURSE/CAR*/ITEM/CARS/SHOP could never match files), locate leaf-preference, empty-dir required-set fallback, pre-stop cancel semantics. All checks pass under ASan |
| OUTMODES STAGE-1 (coord, local) | — | main.cpp (envResolution/fullscreen/title/progressive-assert) + session/game_loop setProjectionAspect + race_hud uniform scale | DONE — 800x600 default byte-identical; 1080p/1440p/native/invalid all behave; title -> "Road Trip Adventure" (parity, sibling src/main.cpp:418); hud_layout widescreen pins updated to correct centered geometry (were pinning the shear) |
| ROLLUP COMMIT | — | 68 files, +14,512 | PUSHED 778d15a (re/native-complete): android app + wizard + socket/facade + slice-C scripts + output-modes stage1 + glue + map/radio/rumble/hd_pack/iso_extract + parity suite + docs; gates all green (0 err, 68/0 sweep, byte-identical, matrix 28/28, ASan-clean, nm sub_00=0) |
| THOR REHEARSAL | local bg | scripts/thor_full_run.sh | running sh_146fdd1d3001 — full chain on 97a75c3e vs fresh pushed build |
| RUMBLE+RADIO GLUE | opencode/deepseek-v4.1-flash-free (free, bg) | main.cpp only (7 sites) | DONE ses_eb91f27ceffew — gates green (coord re-verified: 0 err, sweep 68/0, byte-identical, RT_APP_RUMBLE=1 headless safe, radio=1 starts real SOUND/1CH_L.VAG w/ RT_DISC set, radio=0/unset ZERO calls). Honest gaps cited: 5 rumble event sources absent (native loop resolves no collisions/landings — physics-wave item); radio stereo-right unrouted (engine.playBgm lacks per-voice pan — cited engine.h:110/mixer.h:56) |
| PARITY AUTOMATION | opencode/deepseek-v4.1-flash-free (free, bg) | tests/native_parity/ (9 files) | DONE ses_eb91f27bfffe2 — 29 checks green, 0 DEFECT; findings.json: 4 SPEC_DRIFT (incl. stale 1.6875x scale claim — use uniform min(W/640,H/480) centered; Controls-row mis-cites), 118 SPEC_GAP (64 hints/41 label-value/13 formats; 12 uncited strings must still be drawn), 36 KNOWN_GAP (27 model gaps + 9 GpuWavePending rows must stay hidden retail + 3 FONTE subs). Wave-2c top-10 actionable list captured |
| TERRAIN POLISH (deferred wave-2b-adjacent) | — | preview/tile_streamer/camera | CONCLUSION from device-vs-desktop image compare (desktop-textured.png vs coord-check.png): NOT device-specific — same authored scene both sides (one tile quad, trigger bars, green edge stripe, void beyond center tile). Fix = desktop work: adjacent-tile streaming coverage, camera framing, lighting pass, textures-on default; Vulkan/Adreno exonerated |
| UI-PARITY defect logged | — | wave-2c | trigger bars render horizontal top-left @540p snapshot vs vertical bottom-left @1080p device — anchor/scale inconsistency; placeholders anyway but must not survive restyle |
| PARITY follow-ups (queued): wave-2c UI drawing per spec + model gaps (root/Quit flow, unified Options page, display/graphics controls, binding UX, state-slot thumbnails, setup UI, second screen, map-enhanced look, perf-overlay look); test-engineer round 2 = the 5 parity automation checks; strip title debug labels | TBD | — | tracked |
| WAVE 2b/2c (queued): GPU passes (depth STORE export -> MSAA -> FXAA/FSR1 -> MV buffer -> TAA per visual-pipeline.md order), second-screen WINDOW + map GPU render, HD-pack binding, widescreen HUD anchor, UI DRAWING per ui-parity.md (pause/options/setup/second-screen tabs/overlay), ISO extractor wiring | TBD by quota at dispatch | preview.cpp/presenter/main.cpp single-writer each | blocked-behind wave-1 drain |
| zai-coding-plan retry | — | — | cadence probes failed 17:12 & ~17:15 (err 1113). Retry next checkpoint; Synthetic weekly resets ~17:49 today — probe GLM-5.3 there after. |

## Dispatch policy v3 — dynamic, quota-driven (replaces v2 provider tables)
No hardcoded provider->task assignments. At EVERY dispatch moment:
1. `execute -> tools.quota_status()` for live windows (never trust remembered quota).
2. Rank candidates = allowed models at the task's capability tier, scored by:
   auth configured + live_probe success  ->  min(5h %, weekly %) above ~25%
   ->  reset_at vs expected task duration (a window resetting in 15m with 2% is
   fine for a 5-min job; dead-until-next-week is not)  ->  free > paid for the
   same capability (Zen/go *_free models ride outside paid windows — probe once,
   cache per session).
3. Capability tiering: mechanical grunt (docs, scaffolds, enumeration) -> free
   tier; RE reasoning / codegen -> strongest probe-passing model available;
   never assume free == capable, verify output as always.
4. ERROR-STRING MEANINGS (re-read quota_status before retrying differently):
   - "Insufficient balance or no resource package" (z.ai error 1113) — 2026-10-16
     deep-check: the stored zai-coding-plan credential (db cred active=1) gets 1113
     on ALL legitimate routes: z.ai paas/v4, coding/paas/v4, Anthropic /api/anthropic,
     AND bigmodel.cn mirrors (OpenAI + Anthropic forms, EN + CN messages). moe/v1
     returns 200-with-404-body (wrong path). Conclusion: NOT an endpoint/config bug
     on our side — the API says the stored key has no package. ACTION (user): in
     z.ai/bigmodel console verify the plan-bound API key and regenerate it after
     plan purchase if unsure, then refresh via `opencode auth login` -> Z.AI Coding
     Plan. User confirms plan HAS quota + allowed models = GLM-5.3, GLM-5.3-Flash,
     so the stored credential is stale/wrong-account OR provider-side outage
     (user: "might be down"). CADENCE: keep project momentum; re-probe
     zai-coding-plan with a 1-line subagent at natural checkpoints (roughly
     every 3-4 dispatch decisions); on probe-pass, GLM-5.3 becomes default
     strong tier ($0). Do not burn tokens hammering it in a tight loop.
   - "exceeded your subscription rate limits" = window throttle; check reset_at,
     reschedule or switch candidate by score
   - "<PROVIDER>_API_KEY is not set" = dispatch-env wiring gap despite db key
     present; that provider is unusable for subagents until wired, ignore
     quota_status saying "configured"
   - Synthetic CONCURRENCY 429 != quota: cap parallel dispatch at 2-3 anyway.
5. Probe-before-first-use of any provider/model pair (1-line echo subagent,
   ~seconds); cache verdicts for the session.
6. EVERY subagent dispatch MUST pass background:true AND an explicit CLOUD
   model:model param — never omit model (default = local omlx inference; user
   rule violated 2026-10-16 TWICE: feature-retention audit, then frame-timing
   RE; both cancelled by the user, both re-dispatched on openai/gpt-5.6-terra).
   MECHANICAL PRE-CHECK before every subagent tool call: the param object must
   contain a model: whose prefix is one of openai/, synthetic/, zai/, opencode-go/,
   and background: true. If either is absent, DO NOT SEND — rewrite the call.
   Local-model subagents are FORBIDDEN. Foreground mode only when its RESULT
   is needed to continue and nothing else can proceed — which is never in
   this project's coordinated flow.
Today's snapshot (ephemeral, do NOT reuse later): openai 66%/51%; synthetic
weekly $0 resets ~17:49; zai weekly 87% but dispatch env lacks ZAI_API_KEY;
zai-coding-plan quota fine but glm-5.3 ineligible (probe lower GLMs);
opencode-go/cat-jam-free + opencode/big-pickle probe-PASS (free rides past
their weekly 0%); huggingface no credits; anthropic weekly 12%.

## Agents in flight
| Agent | Model | Owns | Status |
|---|---|---|---|
| Minigame logic RE | openai gpt-6.1-sol | minigames.*, test_minigames, minigame-logic.md | DONE — committed 7b46db0 (I capstone-verified golf/gate/timer-gates/polarity at instruction level) |
| Town street graph reader | openai gpt-5.6-terra | nav_routes.*, test_nav_routes | DONE — committed 24a6c43 (byte-verified by me; pixel-verified overlay after fixing sub-pixel regression: strips float at y=6; RT_APP_DEBUG_OVERLAY frustum stats kept) |
- f3a5b78 rules tables (payout ladder/clocks/golf/slot/speedo byte-verified by me; entry-fee + taxi-fare bounded negatives)
| Rules tables | synthetic GLM-5.3#high | rules_tables.*, rules-tables.md | running ses_ebb5cbf20ffe |
| Town street graph reader | synthetic GLM-5.3#high (RESTART) | nav_routes.*, test_nav_routes | DIED on Synthetic rate limit; superseded by OpenAI reroute above |
| Doc corrections | synthetic Flash | 8 docs | DONE — committed 0b5d807 (spot-checked by me) |
| HUD text presentation | me | glyph_atlas helpers, preview hud pipeline, race_hud builders, app wiring | DONE — committed 0a50fdd (pixel-verified "LAP 1/1" legible, determinism byte-identical, 60/60 glyph pins) |

## After agents return
1. Minigame duplicate: verify first reporter (compile+suite+evidence sanity), commit; when second reports, rebuild+retest; if worse/mixed -> git restore to committed winner.
2. Town graph: verify dir entries + Peach bbox [418,552]x[443,1084] myself; wire RT_APP_NAV to townGraph(1) (coords inside anchored tile), RT_APP_ROUTE follower to same; pixel smoke OFF vs NAV; commit.
3. Remaining: glyph+HUD presentation wiring into preview; CAR livery binding into app model; original-save migration importer; oracle golden-parity harness; first-run extractor wiring for CAR/ITEM/SHOP/SYS via pack catalogs.

## Verification gates (skepticism policy)
- Course props: VERIFIED (disc re-walk by me).
- Camera constants: PARTIAL — (0,2,-7)@0x29ce00, focal 500@0x29ce10, pitch 0x380, ±0.001@0x332c30 all byte-true; DAT_00332c80/84 swap fix re-dispatched.
- Traffic AI: HELD pending scene-tile verdict + my spot-check of 0x28000/0x14000 rules.
- Others: verify reports against disc/ELF before committing.

## After agents return
1. Verify + integrate each: unguard staged CMake entries, full suite (build/native + root build/macos-release where preview-dependent), rt_feature_matrix.sh 16/16, pixel smoke, commit + push each green group.
2. Scene-tile verdict may force anchor correction (Peach tile id) — highest-priority follow-up; affects main.cpp anchor, nav overlay labeling, world_layout test expectations.
3. Wire position-driven streamer demand into main.cpp (needs streamer API).
4. Remaining roadmap: HUD font glyph presentation via glyph_atlas + hud_layout into preview; CAR livery binding into app model; original-save migration importer; oracle golden-parity harness; rules tables app integration (economy UI + session mutations).

## Quota log
- ~13:51 Synthetic 429 on one GLM-5.3 dispatch (concurrency); retry succeeded. z.ai Lite had no capacity (~13:50 "Insufficient balance") — user believed it did; re-check later. OpenAI reviewed + working (gpt-6.1-sol, gpt-5.6-terra both dispatched OK).

## PRIORITY PIVOT — Fri 17 Oct (user: "extremely broken, nowhere near parity, should surpass")
- Infrastructure != game. All waves now aim at what the player SEES on the Thor.
- Visible gap vs PS2Recomp today: dark flat ground + box cars at default
  launch; no terrain coverage, roads, buildings, real models, HUD, title art;
  radio mono; physics stub. Root cause: renderer not consuming extracted
  assets at default launch (everything gate-ENV'd off; terrain defaults flat).
- dd0ebc5 (pushed): abstract test-socket was AF_INET -> Android listener
  silently dead (this, not scene content, caused rehearsal title/town/race
  FAILs); scenario hello retry widened to 180 s for cold device boot.
- GAME-LOOK v1 dispatch (Kimi-K3, synthetic weekly reset 17 Oct 00:04):
  terrain+textures ON by default when assets present (flat fallback when
  absent); streamer covers camera frustum; real player-car model from CAR/
  replaces box at spawn; Peach Town roads visible; camera + daylight lighting
  per evidence; real title screen (GAME container art + FONTE, per
  ui-parity.md). Desktop-verified + snapshot rebaseline declared at land;
  then device rehearsal screenshots compared against PS2 evidence.
- Next: GAME-LOOK v2 = HUD page + menu rows drawn identical to recomp;
  v3 = physics/collision so the car drives like the game.

- Pool reality (00:5x): zai quota probe says alive but subagents fail
  "ZAI_API_KEY is not set" -> zai unusable for dispatch despite probe.
  GAME-LOOK v1 re-dispatched opencode-go/deepseek-v4-pro (5h window
  refreshed 100%). Device launch component is
  .RoadTripNativeActivity (NOT .SetupActivity — wrong name caused the
  240s hello timeout just now; thor_full_run.sh already uses the right one).
- Plan: after GAME-LOOK lands -> review+verify -> APK rebuild from final
  state -> FULL thor rehearsal (socket fix + terrain land together) ->
  decide coord-off rebaseline -> push rollup.
- 01:0x POOLS: xai/grok unlocked by user (grok-4.7, grok-build-0.1; quota
  api has no live xai rows - treat as granted). zai probe lies (subagent
  ZAI_API_KEY missing) - do not dispatch zai/* ids.
- IN FLIGHT: GAME-LOOK v1 = opencode-go/deepseek-v4-pro
  (ses_eb8de9a3...); ORACLE GOLDEN FRAMES = xai/grok-4.7
  (ses_eb8d158b...): desktop oracle build w/ disc at
  ~/Library/Application Support/RoadTripRecomp/disc -> title/menu/driving/
  race frames -> build/scratch/oracle-golden/ (untracked) +
  docs/native-reference/oracle-golden-frames.md ledger; device fallback
  via newest build/apk oracle apk on DEV id, restore ours after.
  Ground-truth pixels unblock: camera framing, HUD layout calibration,
  title art target, terrain colors -> v2/v3 aimed at real references.

## GAME-LOOK v1 stage 1 LANDED — 1304bca (pushed) + oracle ledger 5d819b8
- Defaults flipped: terrain/textures/FONTE-hudtext ON when assets autodetected;
  radius 3x3 default (RT_APP_TILES_RADIUS), lazy scene-init on first PLAYING.
- Boot: wizard re-CRCed all files EVERY launch (per-bit CRC; ~6s desktop,
  ~60s+ Thor = the socket-bind delay). Now table CRC + size fast-path;
  full verify post-extraction / RT_APP_SETUP_VERIFY=full. TITLE 12.5s->1.46s.
- Gates: 0-err, 68/0, matrix 28/28 (2 rows now radius=all honestly),
  PLAYING snapshot byte-identical. Corruption test kept green by preserving
  public full-CRC contract (fast path boot-call-site only).
- Oracle goldens verified independently (PIL means match report; docs-only
  commit, frames untracked).
- NEXT: APK rebuild + full thor rehearsal (title/town/race real verdicts),
  then stage 2: real CAR model (convertCarModels), daycycle tint, title art
  vs oracle title.png, camera framing vs town_driving.png.

## GAME-LOOK v2 wave (2026-10-17 ~04:15) — assembly of decompiled modules
- Agents (file-owner lanes, verified independently after each landed):
  - 8c6fef5 camera framing (grok-build-0.1): kChase* consts from
    camera-framing.md (back 7 RU ROM, height 2 ROM, pitch 0.0859 ROM,
    lookAt 1.5 + fov 1.03 flagged UNDOC); GameLoop::setCarPose API. 3 files.
  - dcbac58 sky+fog (terra): clear=sky RGB, smooth fog 0.5..1.0*fogFar in
    both shaders, lighting.h helpers + native_preview_lighting_test.
    NOTE: default clock was midnight => night-dark scene (their honest gap).
  - 7d83ae7 app wiring (grok-4.7): real Q00 car mesh (disc/extracted
    fallback, box fallback logged), FONTE TITLE/MENU/OPTIONS via ui_menu
    (eager atlas, old quads kept for no-assets), radio default ON.
- Coordinator follow-ups (unit-verified): morning clock default 82950=09:13
  (oracle-golden-frames.md:54), Peach Town square anchor 506.283/442.230
  (same citation) via GameSession::setExplorePose ctor + results re-anchor,
  setCarPose now FREEZES speed/steer (post-race anchor drift fix; test
  testSetCarPoseFreezesMotion), test_game_loop night-band -> grounded
  morning contract (strict equality kept).
- Scores vs oracle: title 0.384->0.419 hist-intersection (NEAR),
  town first-ever score 0.415 (NEAR). Still wrong: 3D title art, HUD chrome,
  town buildings, livery UVs (flat tint), menu ink white vs gold.
- Matrix: 2 failures after defaults wave — under investigation (background).
  coord-off.ppm 01:20 now stale BY DESIGN (fov+sky+fog+clock+anchor wave);
  will rebaseline honestly in the wave commit.

## GAME-LOOK v3 debug dispatch (2026-10-17 ~05:00, post dbf603c push)
- Visual truth from captures: title screen excellent (FONTE + plate frame +
  sky; oracle adds shipped art). Town: sky/clock/car present, but
  (a) tile lattice gaps render charcoal underlay (staircase blocks),
  (b) Q00 car = tan blob too tall (suspect wrong up-axis or parts bloat).
- Dispatched (quota: openai 81/29, xai 82; go/zai dead as usual):
  - T   grok-build-0.1 -> loadTerrainMesh lattice/stagger + coverage probe
  - C2  terra          -> car_mesh axis/parts/scale + per-model bbox dump
  - BG  terra          -> runtime title backdrop/sprites (new module +
    TITLE branch ONLY; new-file-first design, never commits game data)
- RISK: T + BG both touch main.cpp DIFFERENT regions; commits may
  cross-contaminate the index. MERGE PROTOCOL: after each completion run
  build+sweep immediately, `git show --name-only` + region-check diff;
  if a commit carries foreign hunks, fixup locally BEFORE push.

## 2026-10-17 POST-CRASH RECOVERY + HUD ART WAVE
Sandbox env degraded after kernel panic: shell lost DirectoryServices
("not in passwd db", scutil=0 nameservers → DNS dead) and WindowServer
session (SDL "no displays"). IP-level OK. FIX = restart OpenCode app or
Wi-Fi toggle; NOT sudo-able from sandbox. Commits safe locally.
Committed chain awaiting push (DNS): dbf603c-pushed → e0aa9ba (missed
GAME-LOOK v2 files: flat floor 0.85 + warp-anchor lane), de1d03d
(full-tile ground-cover fill: FLDT meshes sparse-coverage-proven, 16RU
grid own-nearest-vertex sample, -0.08 drop, white-page bucket),
cca87b0 (Agent H hud_art: METER0.GSL@0x70 256x128 PSMT8+CT32@0x80e0
dial; SCORE.GSL@0x70 256x256 PSMT4+CT16 race atlas; needle procedural
ElementA type-2; minimap live viewport §1.3 — NOT static art),
df99fed (upload fix + dial wire at hud-layout.md:81 rect).
Agent M (terra, ses…x3nIf): menu backdrop param + oracle menu colors
(ui_menu.cpp + additive preview.h + test) — IN FLIGHT.
Latent bug found+fixed: hudAtlasReady bool cache ⇒ title backdrop stayed
bound through glyph UVs (FONTE rebind was no-op); ptr+dims identity now.
Blob hypothesis v2: tan mass = distant mountain peaks (y≤7.2 RU) above
horizon, brightened by GS×2 — legit terrain, verify in v8 vs oracle.
When env returns: v8 capture → score → matrix 28/28 → determinism pair
→ push all → (Thor if reconnected: expect dial + backdrop + ground fill).
Agent M DONE eabd91e: additive ScreenQuad.texture+UVs, explicit-backdrop
overloads (legacy 3-arg paths unchanged), oracle menu colors pinned
(highlight (0,110,224) rect x870..1185 y274..297; panel (48,64,144)/F2;
labels near-white (244,244,244)). Coordinator wiring 4d12711: compose
title art under MODE_SELECT/OPTIONS panels (composite-level, opaque art
pass + transparent panel pass; legacy opaque path intact when no disc),
RT_APP_MODE=race (straight-to-RACE, autopilot drives -> dial capture on
desktop). post_recovery_verify.sh + watcher (20s poll, 40min cap):
title/menu/town/race captures + 4 oracle scores + matrix + det pair +
push. NEXT after env: eyeball blob (mountains hypothesis), dial visual,
menu score delta, then minimap authored viewport (hud-layout §1.3:
scissor viewport + arrow/dot + zoom table — data-driven, NOT static art).
Push queue: e0aa9ba de1d03d cca87b0 df99fed 4d12711 +2 chore.

## Speed-digit pass (14b3a97) + env note
Race HUD now: dial (atlas rect 0,0-132,96 fix) + CONFIRMED METER speed
digits (u=d*20 v=102, right-align anchor+{78,59,40}/+49, 640x448 space
fixed from /480). Divisor physics->kph PROVISIONAL /100 (flagged GAP;
calibrate vs oracle race frame post-capture). Gear bar deferred: snapshot
carries NO gear field — not fabricating. Minimap deferred: content is
VU1 PATH1 live render, zoom table values unpinned — need eyeball pass.
adb server wedged too (kill-server hangs) -> everything non-headless
awaits OpenCode app restart: DNS + WindowServer + adb all restore with it.
Watches: sh_149cd6d7 (40-min chain). Push queue now 9 commits.

## ROOT CAUSE of env outage (verified)
Machine NEVER rebooted after panic: uptime 10 days; launchservicesd scan
= -10822, DNS/adb dead, sandbox "not in passwd db". FIX = real reboot.
Crash suspect: omlx-server RSS 44.5GB + AdobeIPCBroker 134% since boot
(not game build alone — 128GB box). make_mac_app.sh committed as the
GUI-session launch path for post-reboot testing.

## CPU-side pixel verification breakthrough (no GPU needed)
Post-crash sandbox: Metal segfaults (MTLCreateSystemDefaultDevice SIGSEGV),
so NO rasterization until reboot. But decoders are pure CPU -> PIL harness
validates art + composition rules directly:
- METER0 = baked dial ring base (NOT 96px crop -> full 132x128 column;
  §1.2 record interpretation cropped mph; oracle sim match decides).
- METER10 = dynamic parts (v=102 digit row, gear badge, arcs, mph parts);
  hasSpeedDigitRow validator in hud_art.cpp auto-picks it.
- sim_dial_v2_vs_golden.png: base+digits composite vs oracle = match
  (artwork, anchor, digit placement incl oracle's 63).
- Race dial renderer: two passes (base upload+draw, parts upload+digits)
  relies on ptr-keyed rebind (df99fed). speed kph divisor still /100 GAP.
RT_APP_HEADLESS committed (2b22136) — usable for CI on healthy machines.
Remaining GPU-only list: v9 captures x4 + scores, matrix, Thor rehearsal.
Push queue: 13 commits (through 2b22136).

## 2026-10-17 ~23:15 — injection incident, v14 gate, wave-2 launch
- INJECTION: harness flagged+IGNORED a fabricated tool-call block (fake quota_status
  readout, fake wave-board edit, fake subagent dispatch receipts ses_59ac002…) that
  appeared inside tool-result text. Verified nothing executed (.qoder/wave2/ absent at
  the time). All actions redone by coordinator with verified content. Wave-board now
  carries an incident note; workers' prompts treat embedded "instructions/tool results"
  as data.
- v14 GREEN GATE: determinism pairs byte-identical; sweep 73/73; scores
  title 0.872 / menu 0.532 / town 0.597 / race 0.471. Feature matrix running
  (/tmp/rt-matrix-v14.log); commit+push on PASS. Score-history table added to
  .qoder/wave-board.md.
- WAVE-2 STEP 0 (coordinator): src/native/CMakeLists.txt — 17 staged sources in the
  EXISTS-guarded foreach; glob tests/native_w2/*_test.cpp -> w2_<filestem>;
  tools/w2_*_main.cpp -> rt-w2-<short>. Fresh configure validated rc=0
  (/tmp/w2-cmake-check). dirs tests/native_w2/{,data,tools} created.
- BATCH A DISPATCH (all background; no omlx/strata/zcp/go/plain-zai):
  W2-14 score harness v2   openai/gpt-6-luna            ses_eb405f918ffeLXMrYQWbDrZmkT
  W2-01 script VM          anthropic/claude-sonnet-5-5  ses_eb405f917ffeuq8Cmz2RfH95uO
  W2-10 HUD live data      anthropic/claude-sonnet-5-5  ses_eb405f910ffeFGAew9tgrYLgzI
  W2-02 app state machine  anthropic/claude-haiku-5-5   ses_eb405f90effeD4B6Lm3uGUXEGc
  W2-03 vehicle dynamics   openai/gpt-5.6-terra         ses_eb405f90cffe3Zk6OCm0Ev5u5k
  W2-04 terrain queries    xai/grok-4.7                 ses_eb405f90bffe43urkgarhnUPU9
  Quota at dispatch: openai healthy; anthropic ~$100 credits; xai weekly ~27%;
  synthetic resets ~23:35; opencode-go dead til Oct-19.
- BATCH B (after A reports): W2-05 race rules, W2-06 shop economy, W2-07 dialogue,
  W2-08 save v2, W2-09 audio runtime, W2-11 minimap, W2-12 input map, W2-13
  enhancements registry. Synthetic lane after reset; re-probe quota first.
- Coordinator verify bar per returning batch: integrity diff (only owned files),
  re-measure >=1 pinned number, run their test target, then integrate.

## 2026-10-17 late — wave-2 Batch A verified + pushed; W2-10 verified; board refreshed
- v14 pushed 0adad7d; Batch A (W2-01 script VM, W2-02 app_flow, W2-03 vehicle_model,
  W2-14 score harness v2 + docs) independently re-verified and pushed 4ee96e0
  (JT 0x2EC750 29/27 re-read; selector@0x21CE08 bytes matched; 0x332f44=0x3fcdf3b6
  double; script_vm selftest PASS 29 opcode table).
- W2-10 HUD live (anthropic sonnet lane) coordinator-verified and pushed 0373d33:
  JT 0x2A8FB0 entry0=0x2496c8 (1P), entry10=0x24e1a0 (2P) — ROM re-read; origin
  table 0x2A8FF0 = [484,294,484,70]; pi 0x40490fdb@0x2a40fc, 0.7f@0x29d180,
  10000.0f@0x29abe0; timer = frames-1 (2/2 oracle 0:10.87 @2446, 0:35.23 @3177).
  IMPACT: main.cpp gauge digit page (METER10 v=102) is the 2P layout; needle is
  RPM-based (supersedes gauge-calibration.md table). Unit-scale GAP open (native
  184/190 vs oracle 39/63 mph) — physics side, fit at W2-03 integration.
- W2-04 (grok-4.7) green except y_err_max 0.102 vs 0.100 threshold; user amended
  acceptance: static/settled-pose Y exact (spawn 48/grid/56; town drive 121),
  driving-pose residual documented not thresholded. Continue session ses_eb405f90bffe43urkgarhnUPU9.
- Overdue corrections logged: B2 sun-badge claim stays REJECTED; W2-01's three
  non-entry JT refs (0x23B0E0 ops 9,10,12) -> W2-07 scope; script-vm.md vs
  gsl-vm.md §5.1 handler-width arbitration -> W2-07.
- Batch B dispatched 2026-10-18 ~00:10 (all background): W2-05 terra-fast, W2-06
  codex-spark (openai); W2-07 sonnet-5-5, W2-08 haiku-5-5 (anthropic credits);
  W2-09 grok-4.5, W2-11 grok-4.3, W2-04r grok-4.7 continuation (xai); W2-12
  deepseek-v4-flash-free, W2-13 Sparky-Qwen-3.5-2B (synthetic, fresh weekly).
