# Native feature-retention audit — `re/native-complete`

Static audit, 2026-10-16.  This is an evidence ledger, not a completion claim.
Statuses mean: **IMPLEMENTED-VERIFIED** = implementation plus a completed
recorded check; **IMPLEMENTED-UNVERIFIED** = implementation found but no
completed applicable check in this audit; **PARTIAL** = a bounded subset exists;
**DOCUMENTED-ONLY** = reference evidence exists but no native implementation was
found; **MISSING** = no implementation was found.  Source line numbers below
refer to this checkout.

The audit's own matrix invocation was interrupted by its 120-second harness
timeout after `title`, `menu`, `playing`, and `terrain` printed `PASS`.  A
coordinator matrix run the same day completed **23/23 PASS** (including the
newly added `route`/`route_town` rows), so every matrix row referenced below is
verified.  Matrix rows pin observable logs/behaviour, not pixel-level fidelity;
statuses below still distinguish those.

| Feature | Original-behavior evidence (doc / vaddr) | Native status | Native evidence (file:line) | Next step |
|---|---|---|---|---|
| Widescreen presentation | Original HUD is authored in 640x448 and widescreen re-anchoring is only an inferred normalized mapping (`hud-layout.md:11-17`); original projection X/Y scales are at `0x0021f500/0x0021f50c` (`camera-constants.md:17-18`). | **PARTIAL** | Resizable SDL/Vulkan window: `src/native/app/main.cpp:768-777`; resize recreates the swapchain/renderer: `src/native/app/main.cpp:910-917`; preview uses current render aspect: `src/native/preview/preview.cpp:599-600`. No explicit wide-HUD re-anchor mode was found. | Add a defined widescreen UI/projection policy and visual checks at target aspects. |
| HD texture packs | The reference documents shipped GS texture upload packs, not an HD replacement-pack format (`sys-gsl-packs.md:12-16`, `sys-gsl-packs.md:222-238`). | **MISSING** | The only terrain texture path decodes shipped tile uploads to RGBA8: `src/native/app/main.cpp:355-362`, `src/native/app/main.cpp:594-606`; no HD/pack lookup was found under `src/native`. | Specify replacement-pack discovery, precedence, and validation. |
| Second screen / split presentation | Original 2P HUD has separate top/bottom anchors (`hud-layout.md:48`, `hud-layout.md:76-77`). | **MISSING** | One SDL window and one `Presenter` are created: `src/native/app/main.cpp:768-777`; one scene render and one HUD composite path are used: `src/native/app/main.cpp:1204-1215`. | Implement a second output/view contract and test both displays. |
| Frame generation | No original frame-generation behavior is documented in the native-reference index; but `frame-timing.md` proves the original logic is VBlank-locked fixed-step (verdict a — one task pass per `sceGsSyncV` VBlank wait, root loop `0x00200528..0x002005a8`), so >60 visuals require interpolation, not a faster logic tick. | **MISSING** | The render loop renders and presents one image per iteration: `src/native/app/main.cpp:1200-1275`; no frame-generation/interpolation implementation was found. | Implement interpolated snapshots between fixed 60-Hz logic steps (see `frame-timing.md` "Higher refresh visuals"); verify motion smoothness and that logic rates are unchanged. |
| Rumble / haptics | Collision response includes collision rumble at `0x00217830` (`README.md:132`); save-option rumble offset is explicitly OPEN (`save-layout.md:74-80`). | **MISSING** | No rumble/haptic/vibration implementation was found under `src/native`. | Add output-device haptics tied to documented collision events and settings. |
| Radio (station selection and VAG streams) | Radio is `progress+0x12DD`; town code invokes cmd `0x33/0x34` for VAG streams (`audio-map.md:241-265`). | **PARTIAL** | The save model parses/stores `radioStation`: `src/native/gameplay/save.h:64-67`, `src/native/gameplay/save.cpp:301-304`, `src/native/gameplay/save.cpp:370-373`. The host audio path instead selects a single TSQ track or one supplied VAG BGM path: `src/native/app/main.cpp:125-159`; it never consumes `radioStation`. | Wire station 0/1/2 to the documented stereo VAG pairs and town-state lifecycle. |
| Music / BGM host playback | Original BGM uses TSQ/TVB and radio uses VAG (`audio-map.md:45-93`, `audio-map.md:241-255`); TSQ parser/renderer scope is documented (`tsq-tvb-format.md:104-114`). | **IMPLEMENTED-VERIFIED** (playback) / station selection PARTIAL | `AudioHost` pumps TSQ or `SoundEngine`: `src/native/app/main.cpp:73-123`; `initAudio` loads TSQ or VAG and opens SDL audio: `src/native/app/main.cpp:125-175`. Matrix `bgm` + `bgm_tsq` rows PASS (coordinator run 2026-10-16). | Test context/station selection (radio station wiring remains PARTIAL above). |
| Analog triggers | Original controller mapping/remaps are at save `+0x12E8` (`save-layout.md:68-70`). | **PARTIAL** | Trigger fields and threshold mapping exist: `src/native/gameplay/input.h:8-18`, `src/native/gameplay/input.cpp:11-17`. App input populates keyboard steering/buttons only, leaves triggers zero, and discards `input`: `src/native/app/main.cpp:1082-1095`. | Add SDL gamepad trigger polling and feed player input into the session/physics tick. |
| Save states (host save/load) | Original persistent record is `0x3448`, read/write at `0x00228510`/`0x00228708` (`save-layout.md:9-17`, `save-layout.md:22-37`). | **PARTIAL** | Two host-slot save/load methods exist: `src/native/gameplay/session.cpp:67-73`; host storage is described as two memory-card slots in `src/native/CMakeLists.txt:76`. No app command/menu path for saving/loading was found. | Expose save/load state controls and verify slot persistence/reload in `rt-app`. |
| Original-save migration | Lossless `BASLUS-20398` import rules are specified (`save-layout.md:95-114`); importer scope and synthetic-only caveat are explicit (`save-layout.md:151-166`, `save-layout.md:203-212`). | **IMPLEMENTED-UNVERIFIED** | P2 scan and both migration APIs are declared: `src/native/gameplay/save.h:209-223`; size/title/slot validation and import are implemented: `src/native/gameplay/save.cpp:456-510`. The docs state fixture coverage is synthetic and no original card/save was available. | Test with an actual supported card image and a real extracted `BASLUS-20398` save. |
| HUD | ROM HUD geometry/atlas layout is recovered (`hud-layout.md:34-118`, `hud-layout.md:137-169`). | **PARTIAL** | Layout lookup is compiled: `src/native/gameplay/hud_layout.cpp:1-28`; app has basic race HUD and optional FONTE text overlay: `src/native/app/main.cpp:870-907`, `src/native/app/main.cpp:1210-1251`. The SCORE label block is explicitly authored: `src/native/app/main.cpp:876-880`, `src/native/app/main.cpp:1239-1241`. | Complete original HUD state/builders (the `hudtext`/`scorehud` rows already PASS — remaining builders lack pinned observables). |
| Traffic AI | Original resident spawn, route selection, speed law, and known omissions are bounded in `traffic-ai.md:9-19`, `traffic-ai.md:26-41`, `traffic-ai.md:64-97`. | **PARTIAL** | Verified integer speed and branch functions: `src/native/gameplay/ai.cpp:33-59`; default waypoint steering is explicitly authored: `src/native/gameplay/ai.cpp:75-139`. | Implement/validate full graph localization, steering-state, spawn, and traffic behavior. |
| Chase camera | Original chase constants and collision/preset evidence are at `0x0021dc18`, `0x0021e070`, `0x0021e690`, `0x0021f698` (`README.md:152-156`; `camera-constants.md:11-22`). | **PARTIAL** | Host look-at chase camera and slew are implemented: `src/native/gameplay/camera.cpp:19-117`; it states a host model and has no terrain-collision path. | Add proven preset transitions and terrain collision; compare captures. |
| Economy / progression | Original money funnel is `0x0023dc58`, with cap/seed/table evidence (`rules-tables.md:30-65`, `rules-tables.md:66-109`). | **PARTIAL** | Documented rules-table module is compiled: `src/native/CMakeLists.txt:73-79`; session uses a simplified authored reward ladder: `src/native/gameplay/session.cpp:16-21`, `src/native/gameplay/session.cpp:75-81`. | Route all rewards, purchases, unlocks, and race records through original tables/rules. |
| World terrain and streaming | Original fixed grid/position streamer evidence is `0x002201c0` and `0x0021ba70` (`world-geography.md:23-49`); loader has four field slots (`task-system.md:181-195`). | **PARTIAL** | 64-tile terrain load and authentic placement: `src/native/app/main.cpp:632-692`; four-slot streamer and position demand: `src/native/app/main.cpp:460-516`, `src/native/app/main.cpp:1179-1187`. Code comments state synchronous loading and incomplete authentic selector use: `src/native/app/main.cpp:464-468`. Matrix `stream`, `stream_row`, `stream_pos` rows PASS (coordinator run 2026-10-16). | Implement the async/authentic selection path and pin the authentic selector's tile choices. |
| Task scheduler | Original is a 48-record scheduler with serialized loader (`task-system.md:26-75`). | **IMPLEMENTED-UNVERIFIED** | Scheduler is included as the native task implementation: `src/native/CMakeLists.txt:51-54`; no completed scheduler-specific runtime evidence was gathered here. | Run its contract executable and compare scheduling/asset-service cases. |
| Mini-game rules | Original scoring/sensor evidence is documented by task/vaddr (`minigame-logic.md:124-158`, `minigame-logic.md:206-253`). | **PARTIAL** | Native scope deliberately covers helpers, not a complete task loop/physics/prize dialog: `minigame-logic.md:384-402`; it is compiled in `src/native/CMakeLists.txt:61-64`. | Implement remaining entry, task-loop, physics, prize, and persistence flows. |

## `rt_feature_matrix.sh` mapping and coverage gaps

The matrix pins these app features/flags (row lines in
`scripts/rt_feature_matrix.sh`):

| Matrix row(s) | Flags exercised | Pinned observable |
|---|---|---|
| `title`, `menu`, `playing` (48-50) | `RT_APP_STATE`, `RT_APP_NOAUDIO`, harness `RT_APP_FRAMES`/`RT_APP_SNAPSHOT` | state/session log |
| `terrain`, `textured` (53-56) | `RT_APP_TERRAIN`, `RT_APP_TEXTURES` | tile count; texture page/unresolved log |
| `stream`, `stream_row`, `stream_pos` (57-62) | `RT_APP_STREAM`, `RT_APP_STREAM_STEP_FRAMES`, `RT_APP_STREAM_ROW`, `RT_APP_STREAM_POS` | four-slot/demand/load logs |
| `nav`, `nav_town` (63-66) | `RT_APP_NAV`, `RT_APP_NAV_TOWN` | static/town graph segment logs |
| `hudtext`, `scorehud` (67-70) | `RT_APP_HUDTEXT`, `RT_APP_SCOREHUD` | FONTE/SCORE log |
| `carmodel` (71-72) | `RT_APP_CARMODEL` | Q0 load log |
| `route`, `route_town` (73-76) | `RT_APP_ROUTE`, `RT_APP_ROUTE_TOWN` | nav/town lap-drive route logs |
| `norender`, `snapshot` (79-81) | `RT_APP_NORENDER`, `RT_APP_SNAPSHOT` | state/snapshot output |
| `single_tile`, `disc_terrain`, `disc_nav` (86-95) | `RT_APP_TERRAIN_FILE`, `RT_DISC` plus terrain/nav flags | disc-dependent loader logs |
| `bgm`, `bgm_tsq` (89-92) | `RT_APP_BGM`, `RT_APP_BGM_TSQ`, `RT_DISC` | BGM load log |

`RT_APP_ROUTE` and `RT_APP_ROUTE_TOWN` gained matrix rows (`route`,
`route_town`) on 2026-10-16 and PASS in the coordinator run.  The remaining
`RT_APP_*` flag referenced by `src/native/app/main.cpp` but **without a matrix
row** is **`RT_APP_CARMODEL_DUMP`** (a dump/debug aid).  The matrix's `row()`
helper supplies `RT_APP_FRAMES` and `RT_APP_SNAPSHOT` to every row
(`scripts/rt_feature_matrix.sh:19-27`), so those harness flags have coverage;
`RT_APP_NOAUDIO` appears in individual rows, and the BGM rows intentionally omit
it.  `RT_APP_DEBUG_OVERLAY` lives in `src/native/preview/preview.cpp:1027`, not
`main.cpp`, and is a debug aid without a row by design.
