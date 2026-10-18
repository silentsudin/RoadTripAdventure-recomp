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
| W2-10 | HUD live data | anthropic sonnet-5 | VERIFIED + pushed 0373d33: JT split 1P=0x2496c8 / 2P=0x24e1a0 re-read; origin 0x2A8FF0=(484,294); timer=frames-1 (2/2 oracle); needle tracks RPM not mph (supersedes gauge-calibration.md); METER10 in main.cpp = 2P layout -> A/B rescore at integration |

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

## Open GAPs / arbitration (coordinator-owned)
- Speed unit scale: native speed/100 = 184/190 vs oracle 39/63 mph at race frames
  2446/3177. Divisor matches rounding (0.625px); native physics ~4.7x guest raw
  units. Fit from vehicle_trace.csv at W2-03 integration.
- Digit page 1P vs 2P: A/B rescore at integration (METER0 1P vs current METER10).
- gauge-calibration.md needle table superseded by hud-live.md (rpm-based).
- script-vm.md §5.1 vs gsl-vm.md op-handler width: arbitration folded into W2-07.
- Needle fit: needs >=4 oracle frames with rpm -> extend capture contract
  (RT_CAPTURE_EXTRAS rpm logging).
- Menu edge density 0.27, race 0.29 vs oracle (missing detail) — wave-2 modules target.

## Score history
v9 0.655/0.703/0.478/0.481 · v10 0.872/0.706/0.597/0.471 · v12 same+car span 2.2 RU ·
v14 0.872/0.532/0.597/0.471 (menu honest lighting).
