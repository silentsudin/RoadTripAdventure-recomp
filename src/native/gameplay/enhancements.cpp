// Enhancement retention registry (W2-13) — table + gate implementation.
// See enhancements.h for the contract. Evidence for every row was gathered
// 2026-10-18 by grepping src/native for RT_* literals and cross-checking
// docs/native-reference/*.md, config keys (gameplay/settings.cpp
// OptionsModel), and scripts/rt_feature_matrix.sh rows.
#include "enhancements.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <set>
#include <sstream>

namespace rt::native::gameplay {
namespace {

constexpr char kCsvHeader[] =
    "id,ledger,env_vars,persisted_keys,default,gating,owners,docs,status,notes";

// Field separator inside list-valued columns. '|' never appears in the
// values themselves (asserted implicitly by the data-file round-trip).
[[maybe_unused]] constexpr char kSep[] = "|";

const std::vector<EnhancementSpec>& registryTable() {
    static const std::vector<EnhancementSpec> kRegistry = {
        // ---- Headless / deterministic capture contract ---------------------
        {"headless_mode", "-",
         "RT_APP_HEADLESS", "", "off (windowed Vulkan presenter)", "headless/CI",
         "src/native/app/main.cpp",
         "code:src/native/app/main.cpp|plan",
         RetentionStatus::Live,
         "main.cpp:1428-1434: SDL dummy video driver + null presenter; logic + offscreen scene/HUD + RT_APP_SNAPSHOT capture without a WindowServer session"},
        {"noaudio", "-",
         "RT_APP_NOAUDIO", "sound.audio", "audio device opened", "always",
         "src/native/app/main.cpp|src/native/gameplay/settings.cpp",
         "docs/native-reference/options-menu.md",
         RetentionStatus::Live,
         "main.cpp:1643 skips initAudio when present; settings key sound.audio off sets RT_APP_NOAUDIO=1 (main.cpp:228); matrix rows pin it"},
        {"start_state", "-",
         "RT_APP_STATE", "display.start_state", "TITLE", "always",
         "src/native/app/main.cpp|src/native/gameplay/app_flow.cpp",
         "docs/native-reference/app-flow.md|docs/native-reference/options-menu.md",
         RetentionStatus::Live,
         "main.cpp:1340-1345 TITLE|MENU|PLAYING; settings enum {title,menu,playing} maps to the env (main.cpp:239-244); matrix rows title/menu/playing"},
        {"mode_gate", "-",
         "RT_APP_MODE", "", "unset: PLAYING direct-launch auto-starts the first AI race after one EXPLORE frame", "always",
         "src/native/app/main.cpp|src/native/gameplay/app_flow.cpp",
         "docs/native-reference/app-flow.md",
         RetentionStatus::Live,
         "main.cpp:1832-1849: RT_APP_MODE=race forces SessionState::RACE; =explore holds EXPLORE indefinitely (town/drive captures); matrix rows explore/mode_race"},
        {"frame_budget", "-",
         "RT_APP_FRAMES", "display.frame_budget", "0 (unlimited)", "always",
         "src/native/app/main.cpp|src/native/gameplay/interp.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/frame-interpolation.md",
         RetentionStatus::Live,
         "main.cpp:629 envFrames(); harness/deterministic run length; interp pacing keeps RT_APP_FRAMES a logic-step count (frame-interpolation.md:21-23)"},
        {"snapshot_capture", "-",
         "RT_APP_SNAPSHOT", "display.snapshot_path", "off", "headless/device",
         "src/native/app/main.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/frame-interpolation.md",
         RetentionStatus::Live,
         "main.cpp:2787-2798 writes one P6 PPM per presented frame; main.cpp:2294-2301 pins the logic-step-boundary (t=0) pose contract under interp; matrix row snapshot"},
        {"snapshot_at", "-",
         "RT_APP_SNAPSHOT_AT", "", "unset (every presented frame)", "headless/device",
         "src/native/app/main.cpp",
         "code:src/native/app/main.cpp|plan",
         RetentionStatus::Live,
         "RT_APP_SNAPSHOT_AT=<simframe> narrows the PPM dump to the frame where renderSnapshot.frame==N (one-shot mid-race adjudication, W3); unset keeps the every-frame dump (last frame wins)"},
        {"norender", "-",
         "RT_APP_NORENDER", "display.render", "render enabled", "always",
         "src/native/app/main.cpp|src/native/gameplay/settings.cpp",
         "docs/native-reference/options-menu.md",
         RetentionStatus::Live,
         "main.cpp:1485 skips render+present when present; settings display.render off sets it (inverted key); matrix row norender"},
        {"test_socket", "-",
         "RT_TEST_SOCKET", "", "off", "oracle/harness",
         "src/native/runtime/testsocket.cpp",
         "docs/native-reference/oracle-golden-frames.md",
         RetentionStatus::OracleOnly,
         "runtime/testsocket.cpp:57 binds the deterministic drive socket used by oracle golden captures"},

        // ---- QoL enhancement flags ----------------------------------------
        {"frame_interpolation", "frame_generation",
         "RT_APP_INTERP|RT_APP_INTERP_CAP|RT_APP_INTERP_RATE",
         "display.interp|display.interp_cap|display.interp_rate",
         "off (one logic step per host frame, byte-identical loop); cap 2; rate 0 (vsync)",
         "device (render pacing), logic locked 60 Hz",
         "src/native/app/main.cpp|src/native/gameplay/interp.cpp|src/native/gameplay/interp.h",
         "docs/native-reference/frame-interpolation.md|docs/native-reference/frame-timing.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:636-654; frame-timing.md verdict a: 60 Hz logic locked, only render poses interpolate; matrix row interp; RT_APP_SNAPSHOT stays a t=0 capture"},
        {"rumble", "rumble",
         "RT_APP_RUMBLE", "controllers.rumble", "disabled (no SDL rumble call)", "device (gamepad)",
         "src/native/app/main.cpp|src/native/gameplay/rumble.cpp|src/native/gameplay/rumble.h",
         "docs/native-reference/rumble-haptics.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:1881-1900: opt-in, env must be EXACTLY \"1\"; gate byte (player&master)!=0 per rumble-haptics.md; strength key not consumed yet (no global gain)"},
        {"autopilot", "-",
         "RT_APP_AUTOPILOT|RT_APP_AUTOPILOT_HOLD", "", "off (no steering input)", "headless (test affordance)",
         "src/native/app/main.cpp|src/native/app/Autopilot.cpp|src/native/app/Autopilot.h",
         "code:src/native/app/main.cpp|code:src/native/app/Autopilot.cpp",
         RetentionStatus::Wired,
         "main.cpp:2065-2072: both opt-in, env EXACTLY \"1\" (autopilot) / seconds (hold); pure-pursuit steering on the AI-car trail for race, heading-hold toward road waypoints in explore; w22_autopilot_test covers the decision law, ON-run gates tracked in wave-8 journal"},
        {"radio_station_follow", "radio",
         "RT_APP_RADIO", "sound.radio_station", "unset: Peach FM when its VAG is reachable; explicit 0 disables", "disc + audio",
         "src/native/app/main.cpp|src/native/gameplay/radio.cpp|src/native/gameplay/radio.h|src/native/gameplay/save.cpp",
         "docs/native-reference/radio-stations.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:376-396: explicit byte wins over everything; explicit RT_APP_BGM/RT_APP_BGM_TSQ beat the IMPLICIT default station (2026-10-17 regression); station owns the BGM path (main.cpp:398-411)"},
        {"bgm_playback", "bgm",
         "RT_APP_BGM|RT_APP_BGM_TSQ", "sound.bgm|sound.bgm_tsq",
         "TSQ track BGM_01 (VAG fallback); TSQ tried first", "disc + audio",
         "src/native/app/main.cpp|src/native/audio/bgm_player.cpp|src/native/audio/music_library.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/radio-stations.md|docs/native-reference/audio-map.md",
         RetentionStatus::Wired,
         "main.cpp:405-419: RT_APP_BGM = TSQ track name or VAG path; RT_APP_BGM_TSQ=0 forces VAG; matrix rows bgm/bgm_tsq"},
        {"analog_triggers", "analog_triggers",
         "RT_APP_TRIGGERS", "controllers.triggers", "off (zero trigger fields)", "device (gamepad)",
         "src/native/app/main.cpp|src/native/gameplay/input.cpp|src/native/gameplay/input.h",
         "docs/native-reference/options-menu.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "main.cpp:1486-1490: exactly \"1\" enables; mapping module input.{h,cpp} mapTriggersToDrive; matrix row triggers"},
        {"output_modes", "widescreen",
         "RT_APP_RESOLUTION|RT_APP_FULLSCREEN", "",
         "800x600 windowed (kInitialWidth/Height, main.cpp:1417)", "device (display)",
         "src/native/app/main.cpp|src/native/app/output_modes.cpp",
         "docs/native-reference/output-modes.md",
         RetentionStatus::Wired,
         "main.cpp:1438-1470: preset|WxH ladder + borderless fullscreen; unset stays byte-identical legacy path (matrix quiet); covers the widescreen-presentation ledger row together with the resizable swapchain"},
        {"glyph_family", "-",
         "RT_APP_GLYPH_FAMILY", "", "auto from SDL pad type", "device (gamepad)",
         "src/native/app/main.cpp|src/native/gameplay/glyphs.cpp|src/native/gameplay/glyphs.h",
         "docs/native-reference/glyph-repaint.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:92-100 forces ps|xbox|nintendo button glyphs; logged at startup so the matrix row does not depend on pad attachment (row glyphs)"},
        {"minimap_pill", "hud",
         "RT_APP_MINIMAP", "", "on (pill + town street roads drawn)", "disc",
         "src/native/app/main.cpp",
         "code:src/native/app/main.cpp|plan",
         RetentionStatus::Live,
         "main.cpp:1583 and 2570: default-on HUD map pill; =0 kills pill and roads; matrix rows minimap / minimap_off (disc-required)"},
        {"perf_overlay", "-",
         "RT_APP_PERFOVERLAY|RT_APP_PERFLOG", "",
         "overlay off; perf log off", "device (present path)",
         "src/native/app/main.cpp|src/native/gameplay/perfstats.cpp",
         "docs/native-reference/perf-overlay.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:1491-1497: both exactly \"1\"; perf-overlay.md:82-85 contract; matrix row perflog (with interp+rate 120); RT_APP_PERFLOG_EVERY is documented but not implemented (gap)"},

        // ---- World / graphics pipeline (GAME-LOOK v1: default-on with disc)
        {"disc_terrain", "world_streaming",
         "RT_APP_TERRAIN", "graphics.terrain", "ON when disc FLD/ exists (GAME-LOOK v1)", "disc",
         "src/native/app/main.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Live,
         "main.cpp:762-769 featureDefaultOn + 938; \"=0\" opts out; matrix row terrain pins tiles=64"},
        {"field_textures", "world_streaming",
         "RT_APP_TEXTURES", "graphics.textures", "ON when disc FLD/ exists", "disc",
         "src/native/app/main.cpp|src/native/assets/field_converter.cpp|src/native/assets/hd_pack.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/hd-texture-packs.md",
         RetentionStatus::Live,
         "main.cpp:939/1535 decodeFieldTextures RGBA8 + page dedup by content hash (main.cpp:705-752); the decode path is also the HD-pack key source (hd-texture-packs.md:44)"},
        {"single_tile_override", "world_streaming",
         "RT_APP_TERRAIN_FILE", "graphics.terrain_file", "unset (64-tile grid)", "disc",
         "src/native/app/main.cpp",
         "docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:954-967: one explicit FLD tile; ignored under RT_APP_STREAM; matrix row single_tile"},
        {"tiles_radius", "world_streaming",
         "RT_APP_TILES_RADIUS", "", "1 (5-tile neighbourhood around Peach Town)", "disc",
         "src/native/app/main.cpp",
         "code:src/native/app/main.cpp|plan",
         RetentionStatus::Live,
         "main.cpp:940-950: radius R lattice steps; \"all\" = full grid; matrix terrain row uses all"},
        {"tile_streaming", "world_streaming",
         "RT_APP_STREAM|RT_APP_STREAM_STEP_FRAMES|RT_APP_STREAM_ROW|RT_APP_STREAM_POS",
         "graphics.stream|graphics.stream_step_frames|graphics.stream_row|graphics.stream_pos",
         "stream off (eager 64-tile load); step 10 frames; row 1; position demand off", "disc",
         "src/native/app/main.cpp|src/native/gameplay/tile_streamer.cpp|src/native/gameplay/tile_streamer.h",
         "docs/native-reference/options-menu.md|docs/native-reference/field-topology-status.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Live,
         "main.cpp:804-812 + 1524-1545: hardware-style 4-slot window (slot = id & 3), serpentine sweep, advanceTo demand; matrix rows stream/stream_row/stream_pos"},
        {"nav_overlay", "-",
         "RT_APP_NAV|RT_APP_NAV_TOWN", "graphics.nav|graphics.nav_town",
         "off", "disc (graph data)",
         "src/native/app/main.cpp|src/native/preview/preview.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/nav-tables.md",
         RetentionStatus::Wired,
         "main.cpp:1555-1582: ROM nav-graph ribbons via the depth-off overlay pipeline (preview.cpp:175); NAV_TOWN swaps the Peach Town street graph; matrix rows nav/nav_town"},
        {"route_follower", "-",
         "RT_APP_ROUTE|RT_APP_ROUTE_TOWN", "graphics.route|graphics.route_town",
         "off", "disc (graph data)",
         "src/native/app/main.cpp|src/native/gameplay/route.cpp|src/native/gameplay/route.h",
         "docs/native-reference/nav-drive.md|docs/native-reference/options-menu.md",
         RetentionStatus::Wired,
         "main.cpp:1598-1632: drives the render-visible car along graph 0 (or the town graph); matrix rows route/route_town"},
        {"hud_text_fonte", "hud",
         "RT_APP_HUDTEXT", "graphics.hudtext", "ON when disc FLD/ exists (FONTE.GSL)", "disc",
         "src/native/app/main.cpp|src/native/assets/glyph_atlas.cpp|src/native/preview/race_hud.h",
         "docs/native-reference/options-menu.md|docs/native-reference/hud-layout.md",
         RetentionStatus::Live,
         "main.cpp:774-802 loadFonteAtlas + 1627 featureDefaultOn; FONTE decode logged (\"rt-app hudtext: FONTE decoded\"); matrix rows hudtext/scorehud"},
        {"score_hud_page", "hud",
         "RT_APP_SCOREHUD", "graphics.scorehud", "off", "always",
         "src/native/app/main.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/hud-layout.md",
         RetentionStatus::Wired,
         "main.cpp:1628-1635: off|score|meter overlay page; SCORE label block AUTHORED (hud-layout anchors CONFIRMED); matrix row scorehud"},
        {"car_model", "-",
         "RT_APP_CARMODEL", "graphics.carmodel", "unset: demo box", "disc (CARn/Qnn.BIN)",
         "src/native/app/main.cpp|src/native/assets/car_models.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/hd-texture-packs.md",
         RetentionStatus::Wired,
         "main.cpp:1659/1696: Q{N}.BIN through the CAR pipeline; matrix row carmodel pins Q0"},
        {"car_livery_dump", "-",
         "RT_APP_CARMODEL_DUMP", "graphics.carmodel_dump", "off", "always",
         "src/native/app/main.cpp|src/native/assets/car_models.cpp|src/native/assets/hd_pack.h",
         "docs/native-reference/options-menu.md|docs/native-reference/hd-texture-packs.md",
         RetentionStatus::OracleOnly,
         "main.cpp:1747: P6 PPM livery dump; the dump pattern is the HD-pack pipeline's dump format precedent (hd_pack.h:28-34, 330-340)"},

        // ---- Path / infrastructure contract --------------------------------
        {"disc_root", "-",
         "RT_DISC", "display.disc_root",
         "~/Library/Application Support/RoadTripRecomp/disc", "always",
         "src/native/app/main.cpp|src/native/assets/setup_wizard.cpp|src/native/assets/hud_art.cpp|src/native/tools/pack_extract_main.cpp",
         "docs/native-reference/options-menu.md|docs/native-reference/asset-map.md",
         RetentionStatus::Live,
         "main.cpp:336 discRootPath(); android bootstrap sets it from the external files dir (main.cpp:151-165); every disc-dependent matrix row"},
        {"data_root", "-",
         "RT_DATA_DIR", "",
         "~/Library/Application Support/RoadTripRecomp", "always",
         "src/native/app/main.cpp|src/native/assets/setup_wizard.cpp|src/native/assets/hud_art.cpp",
         "docs/native-reference/oracle-golden-frames.md",
         RetentionStatus::Live,
         "main.cpp:102-104 appDataDirectory(); settings.toml + assets live under it; matrix settings row points it at a scratch dir"},
        {"asset_dir", "-",
         "RT_ASSET_DIR", "",
         "<data root>/assets", "always",
         "src/native/app/main.cpp|src/native/assets/setup_wizard.cpp|src/native/assets/hud_art.cpp",
         "code:src/native/app/main.cpp",
         RetentionStatus::Live,
         "main.cpp:349 resolveDiscOrPackaged asset override; android bootstrap sets it to internal storage assets (main.cpp:150)"},
        {"pause_menu_open", "-",
         "RT_MENU", "display.menu", "off", "always",
         "src/native/gameplay/settings.cpp|src/native/gameplay/menu.cpp",
         "docs/native-reference/options-menu.md",
         RetentionStatus::DocOnly,
         "GAP (report G-1): options-menu.md:59/81/194 + settings.cpp:411-413 spec RT_MENU=1 as the interim pause-menu open flag, but no src/native code reads it via getenv yet"},

        // ---- Diagnostics (oracle/debug aids) --------------------------------
        {"debug_overlay", "-",
         "RT_APP_DEBUG_OVERLAY", "display.debug_overlay", "off", "device (present path)",
         "src/native/preview/preview.cpp|src/native/gameplay/settings.cpp",
         "docs/native-reference/options-menu.md",
         RetentionStatus::OracleOnly,
         "preview.cpp:1161: frustum-stats overlay; debug aid without a matrix row by design (feature-retention.md:63-64)"},
        {"car_diagnostics", "-",
         "RT_APP_CARDIAG", "", "off", "always",
         "src/native/app/main.cpp|src/native/gameplay/camera.cpp|src/native/preview/preview.cpp|src/native/assets/hud_art.cpp",
         "code:src/native/gameplay/camera.cpp|plan",
         RetentionStatus::OracleOnly,
         "=1 prints car pose/band diagnostics (camera.cpp:115, preview.cpp:1089-1092, main.cpp:2435, hud_art.cpp:280)"},
        {"ground_fill_toggle", "-",
         "RT_APP_GROUNDFILL", "", "on (authored ground-fill sheet)", "disc terrain",
         "src/native/app/main.cpp",
         "code:src/native/app/main.cpp|plan",
         RetentionStatus::OracleOnly,
         "main.cpp:1103-1107: TEMP diagnostic; =0 skips the fill sheet (ident-underlay experiment 2026-10-17)"},
        {"setup_verify_mode", "-",
         "RT_APP_SETUP_VERIFY", "", "fast manifest check (existence+size)", "always",
         "src/native/assets/setup_wizard.cpp",
         "code:src/native/assets/setup_wizard.cpp|plan",
         RetentionStatus::Wired,
         "setup_wizard.cpp:122-153: \"full\" re-CRCs every entry on demand; full always runs once after extraction"},
        {"drive_tool_turn", "-",
         "RT_DRIVE_TURN", "", "0.2 rad/tick", "oracle tool (rt-drive)",
         "src/native/tools/drive_main.cpp",
         "code:src/native/tools/drive_main.cpp|plan",
         RetentionStatus::OracleOnly,
         "drive_main.cpp:87/107 nav-drive AI tuning override"},
        {"drive_tool_arrive", "-",
         "RT_DRIVE_ARRIVE", "", "30.0 (tool; module default smaller)", "oracle tool (rt-drive)",
         "src/native/tools/drive_main.cpp|src/native/gameplay/ai.cpp",
         "code:src/native/tools/drive_main.cpp|plan",
         RetentionStatus::OracleOnly,
         "drive_main.cpp:102/108 arrival radius override"},
        {"drive_tool_vmax", "-",
         "RT_DRIVE_VMAX", "", "20000.0", "oracle tool (rt-drive)",
         "src/native/tools/drive_main.cpp|src/native/gameplay/ai.cpp",
         "code:src/native/tools/drive_main.cpp|plan",
         RetentionStatus::OracleOnly,
         "drive_main.cpp:105/109 straight-line speed cap override"},
        {"drive_tool_slew", "-",
         "RT_DRIVE_SLEW", "", "0.5/tick", "oracle tool (rt-drive)",
         "src/native/tools/drive_main.cpp|src/native/gameplay/ai.cpp",
         "code:src/native/tools/drive_main.cpp|plan",
         RetentionStatus::OracleOnly,
         "drive_main.cpp:104/110 steer slew override"},
        {"drive_tool_norender", "-",
         "RT_DRIVE_NORENDER", "", "off (tool opens its window)", "oracle tool (rt-drive)",
         "src/native/tools/drive_main.cpp",
         "code:src/native/tools/drive_main.cpp|plan",
         RetentionStatus::OracleOnly,
         "drive_main.cpp:111 headless tool run"},

        // ---- Ledger features without a dedicated env flag (registered so the
        // 17 feature-retention.md rows are all covered; gating via settings
        // keys or module presence until a flag lands) -------------------------
        {"second_screen", "second_screen",
         "", "display.second_screen", "on (toggle), but no second presenter exists", "device (display)",
         "src/native/app/main.cpp|src/native/gameplay/settings.cpp",
         "docs/native-reference/feature-retention.md|docs/native-reference/options-menu.md",
         RetentionStatus::DocOnly,
         "Ledger row MISSING (one window/Presenter, main.cpp:768-777, 1204-1215); settings key registered (settings.cpp:441) with no runtime consumer — wave-3 presentation work"},
        {"save_states", "save_states",
         "", "save.slot|save.save_state|save.load_state", "two host memory-card slots", "always",
         "src/native/gameplay/session.cpp|src/native/gameplay/memcard.h",
         "docs/native-reference/feature-retention.md|docs/native-reference/save-layout.md",
         RetentionStatus::Wired,
         "session.cpp:67-73 save/load to slot<N>.sav; ledger PARTIAL (no app command/menu path yet)"},
        {"save_migration", "save_migration",
         "", "", "lossless P2 BASLUS-20398 import", "always",
         "src/native/gameplay/save.cpp|src/native/gameplay/save.h",
         "docs/native-reference/save-layout.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "save.cpp:456-510 size/title/slot validation + import; ledger IMPLEMENTED-UNVERIFIED (synthetic fixtures only)"},
        {"save_slot_load", "-",
         "RT_APP_LOAD_SLOT", "", "unset (no slot read)", "always (opt-in)",
         "src/native/app/main.cpp|src/native/gameplay/save_slots.cpp",
         "docs/native-reference/save-layout.md|plan",
         RetentionStatus::Wired,
         "developer opt-in (W3-04): RT_APP_LOAD_SLOT=0|1 loads slot<N>.sav into session.progress() after session construction via SlotStore; strict \"0\"/\"1\" parse; title/save-menu UI remains GAP"},
        {"save_slot_write", "-",
         "RT_APP_SAVE_SLOT", "", "unset (no slot write)", "always (opt-in)",
         "src/native/app/main.cpp|src/native/gameplay/save_slots.cpp",
         "docs/native-reference/save-layout.md|plan",
         RetentionStatus::Wired,
         "developer opt-in (W3-04): RT_APP_SAVE_SLOT=0|1 writes session.progress() to slot<N>.sav at shutdown before previewDestroy (atomic tmp+rename); default OFF keeps matrix determinism"},
        {"save_dir", "-",
         "RT_SAVE_DIR", "", "$HOME/Library/Application Support/RoadTripNative", "always (opt-in pair)",
         "src/native/gameplay/save_slots.cpp",
         "docs/native-reference/save-layout.md|plan",
         RetentionStatus::Wired,
         "root directory for slot0.sav/slot1.sav used by the SlotStore round-trip (W3-04); harness points it at a scratch dir so tests never touch user data"},
        {"traffic_ai", "traffic_ai",
         "", "", "resident spawn + integer speed law + default steering", "disc",
         "src/native/gameplay/ai.cpp|src/native/gameplay/ai.h",
         "docs/native-reference/traffic-ai.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "ai.cpp:33-59 verified speed/branch functions; 75-139 authored steering; ledger PARTIAL"},
        {"chase_camera", "chase_camera",
         "", "", "host look-at chase camera + slew + terrain-following view lift", "always",
         "src/native/gameplay/camera.cpp|src/native/app/main.cpp",
         "docs/native-reference/camera-constants.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "camera.cpp:19-117; terrain-following view lift main.cpp:2308-2321 (race datum fix 2026-10-17); ledger PARTIAL (no preset transitions/terrain collision)"},
        {"economy_rules", "economy",
         "", "", "simplified authored reward ladder (session) + compiled rules tables", "always",
         "src/native/gameplay/session.cpp|src/native/gameplay/rules_tables.cpp|src/native/gameplay/rules_tables.h",
         "docs/native-reference/rules-tables.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "ledger PARTIAL: session.cpp:16-21/75-81 authored ladder; original money funnel 0x23DC58 not yet routed (wave-2 W2-06 economy module owns the funnel)"},
        {"task_scheduler", "task_scheduler",
         "", "", "48-record scheduler with serialized loader", "always",
         "src/native/gameplay/task.cpp|src/native/gameplay/task.h",
         "docs/native-reference/task-system.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "ledger IMPLEMENTED-UNVERIFIED: scheduler compiled as the native task implementation"},
        {"minigame_rules", "minigames",
         "", "", "helper subset (no full task loop/physics/prize dialog)", "disc",
         "src/native/gameplay/minigames.cpp|src/native/gameplay/minigames.h",
         "docs/native-reference/minigame-logic.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "ledger PARTIAL: scope deliberately bounded (minigame-logic.md:384-402)"},
        {"hd_texture_packs", "hd_packs",
         "", "", "pack module + keying pipeline; no pack lookup wired into the app", "disc",
         "src/native/assets/hd_pack.cpp|src/native/assets/hd_pack.h|src/native/assets/image_converter.cpp",
         "docs/native-reference/hd-texture-packs.md|docs/native-reference/feature-retention.md",
         RetentionStatus::Wired,
         "StableContentKey from the native RGBA8 decode path (hd_pack.h:20-40); replacement/dump API exists; app glue (pack discovery + VkImage cache swap) pending"},
        // ---- Runtime ROM-path override + W3-06 registry-backed options -------
        {"elf_path", "-",
         "RT_ELF", "", "runtime-extracted ROM ELF under the recomp base data", "always (override)",
         "src/native/preview/text_metrics.h|src/native/preview/race_hud.h",
         "code:src/native/preview/text_metrics.h|plan",
         RetentionStatus::Live,
         "explicit path to the SLUS_203.98 ELF for runtime table readers (FONTE advances in text_metrics.h:80, HUD art getters in race_hud.h); else conventional disc path, else authored fallback — runtime tables are never committed"},
        {"enh_target_hz", "-",
         "RT_ENHANCE_TARGET_HZ", "", "unset: legacy RT_APP_INTERP_RATE path", "always",
         "src/native/app/enhancements_app.cpp|src/native/app/main.cpp",
         "code:src/native/app/enhancements_app.cpp|plan",
         RetentionStatus::Live,
         "W3-06 registry key target_hz; envInterpRate() (main.cpp:658) consults it as fallback after legacy RT_APP_INTERP_RATE (legacy precedence preserved)"},
        {"enh_fps_display", "-",
         "RT_ENHANCE_FPS_DISPLAY", "", "off", "device (present path)",
         "src/native/app/enhancements_app.cpp|src/native/app/main.cpp",
         "code:src/native/app/enhancements_app.cpp|plan",
         RetentionStatus::Live,
         "W3-06 key fps_display; ORs with legacy RT_APP_FPS_DISPLAY to enable the fps overlay"},
        {"enh_log_fps", "-",
         "RT_ENHANCE_LOG_FPS", "", "off", "always",
         "src/native/app/enhancements_app.cpp|src/native/app/main.cpp",
         "code:src/native/app/enhancements_app.cpp|plan",
         RetentionStatus::Live,
         "W3-06 key log_fps; ORs with the legacy fps-log flag to print frame-rate stats"},
        {"enh_ssaa", "-",
         "RT_ENHANCE_SSAA", "", "unset", "always",
         "src/native/app/enhancements_app.cpp",
         "code:src/native/app/enhancements_app.cpp|plan",
         RetentionStatus::Wired,
         "GAP: W3-06 parses RT_ENHANCE_SSAA (enhancements_app.cpp:36) but src/native has no apply site — oracle supersampling lives in the recomp GS backend (RT_GS_SSAA); logged at startup, not fake-wired"},
        {"enh_physics_hz", "-",
         "RT_ENHANCE_PHYSICS_HZ", "", "60", "always",
         "src/native/app/enhancements_app.cpp",
         "code:src/native/app/enhancements_app.cpp|plan",
         RetentionStatus::Wired,
         "GAP: W3-06 parses RT_ENHANCE_PHYSICS_HZ (enhancements_app.cpp:38) but FixedStepClock::kStepSeconds stays hard-fixed at 60 Hz (interp.h:82); logged at startup, not fake-wired"},
        {"dashcam_dir", "-",
         "RT_DASHCAM_DIR", "", "<unset>", "opt-in (dev route clips)",
         "src/native/media/dashcam_source.cpp",
         "code:src/native/media/dashcam_source.cpp|plan",
         RetentionStatus::DocOnly,
         "W4-05: root dir for dev-recorded route footage; resolution order RT_DASHCAM_DIR > android dev-app files dir (release id refused) > $RT_SAVE_DIR/dashcam > app-local > ./dashcam; ROM ships NO video - in-game replay re-renders recorded input (AcquireVehicleInput 0x21A990), so this is a native dev affordance only; not yet consumed by main.cpp"},
        {"ffmpeg_binary", "-",
         "RT_FFMPEG", "", "ffmpeg (PATH lookup)", "opt-in (override)",
         "src/native/media/dashcam_source.cpp",
         "code:src/native/media/dashcam_source.cpp|plan",
         RetentionStatus::DocOnly,
         "W4-05: ffmpeg override for clip frame decode; absent => isRuntimeAvailable() false, dashcam panel placeholder"},
        {"ffprobe_binary", "-",
         "RT_FFPROBE", "", "ffprobe (PATH lookup)", "opt-in (override)",
         "src/native/media/dashcam_source.cpp",
         "code:src/native/media/dashcam_source.cpp|plan",
         RetentionStatus::DocOnly,
         "W4-05: ffprobe override for clip metadata (size/fps)"},
         {"native_boot", "-",
          "RT_APP_NATIVE_BOOT", "", "off (host app flow)", "diagnostic (W12)",
          "src/native/app/main.cpp|src/native/guest/guest_kernel.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: boots the translated guest ELF on the native green-thread libkernel (entry 0x200008) instead of the host app flow; boot diagnostic — gates run the default path"},
         {"ktrace", "-",
          "RT_KTRACE", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_kernel.cpp",
          "code:src/native/guest/guest_kernel.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: logs every guest libkernel syscall (tid, number, a0..a3) from the host dispatcher"},
         {"pad_trace", "-",
          "RT_PAD_TRACE", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_libs_pad.cpp",
          "code:src/native/guest/guest_libs_pad.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: host libpad traces motor/mode changes (SetActDirect, SetMainMode, SetActAlign)"},
         {"dma_trace", "-",
          "RT_DMA_TRACE", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_libs_dma.cpp",
          "code:src/native/guest/guest_libs_dma.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: host libdma logs each DMAC submission (channel, chcr, chain walk, words) — payloads walk-discarded until the gs-sink lands"},
         {"boot_start", "-",
          "RT_BOOT_START", "", "unset", "diagnostic (W12)",
          "src/native/app/main.cpp|src/native/guest/guest_libs_pad.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: native-boot diag holds Start on port 0 for 3 vblanks at the given vblank (title->menu probes)"},
         {"cd_trace", "-",
          "RT_CD_TRACE", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_libs_cd.cpp",
          "code:src/native/guest/guest_libs_cd.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: host libcd logs each sceCdRead (lbn, sectors, buffer) — watch reload loops during loading"},
         {"cd_image", "-",
          "RT_IMAGE", "", "unset", "override (W12)",
          "src/native/guest/guest_libs_cd.cpp",
          "code:src/native/guest/guest_libs_cd.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: raw 2048-byte-sector disc image for CD reads outside the extracted-tree .lbn_map manifest"},
         {"cd_trace_bt", "-",
          "RT_CD_TRACE_BT", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_libs_cd.cpp",
          "code:src/native/guest/guest_libs_cd.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: with cd_trace, symbolized host backtrace per sceCdRead — names the task arming each load"},
         {"boot_trace_win", "-",
          "RT_BOOT_TRACE_WIN", "", "unset", "diagnostic (W12)",
          "src/native/app/main.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: native-boot per-vblank load-machine trace inside vblank window lo,hi (step/loading/req/loader+0x1c/handler/b26/b27)"},
         {"ram_canary", "-",
          "RT_RAM_CANARY", "", "off", "diagnostic (W12)",
          "src/native/app/main.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: per-vblank memcmp of guest .rodata vs the staged image; exit(3) + task-handler dump on divergence"},
         {"fault_bt", "-",
          "RT_FAULT_BT", "", "off", "diagnostic (W12)",
          "src/native/guest/guest_mem.cpp|src/native/guest/guest_kernel.cpp",
          "code:src/native/guest/guest_mem.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: symbolized host backtrace on a fault key's first occurrence (slide+offset via nm); also logs setSyscall/getSyscall override traffic and unresolved override dispatches"},
         {"state_hash", "-",
          "RT_STATE_HASH", "", "off", "diagnostic (W12)",
          "src/native/app/main.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: per-vblank FNV-1a 64 hash of guest RAM (ee= line), mirror of the oracle ps2_test_harness hash; W12 parity-debug substrate (ee-only: no scratchpad/VU1/IOP model)"},
         {"state_hash_interval", "-",
          "RT_STATE_HASH_INTERVAL", "60", "60", "diagnostic (W12)",
          "src/native/app/main.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: vblank stride of the RT_STATE_HASH ee= lines (same knob as the oracle harness)"},
         {"ram_dump", "-",
          "RT_RAM_DUMP", "", "off", "diagnostic (W12)",
          "src/native/app/main.cpp",
          "code:src/native/app/main.cpp|docs/native-reference/w12-kernel-design.md",
          RetentionStatus::Wired,
          "W12: raw 32MB kRam slices at vblank list \"vb:path,...\"; diffed against oracle socket state_dump EERM chunks (parity localization)"},
         {"vif_hist", "-",
          "RT_VIF_HIST", "", "off", "diagnostic (W3)",
          "src/native/guest/guest_libs_dma.cpp",
          "code:src/native/guest/guest_libs_dma.cpp|docs/native-reference/w13-gs-sink-design.md",
          RetentionStatus::Wired,
          "W3 step 1: histogram the VIF1 code subset (opcodes, MSCAL entries) and GIF chain segments (tag path fields) while consumeChannel walks; stderr [vifhist]/[gifhist]"},
         {"vu1_code_dump", "-",
          "RT_VIF1_CODE_DUMP", "", "off", "diagnostic (W3)",
          "src/native/guest/guest_vif1.cpp",
          "code:src/native/guest/guest_vif1.cpp|docs/native-reference/w13-gs-sink-design.md",
          RetentionStatus::Wired,
          "W3 step 1: write the VU1 micro image (as VIF1code uploads it) to the given path at MSCAL seam, hash-checked FNV-1a vs the oracle's image"},
     };
    return kRegistry;
}

const std::vector<std::string>& ledgerRows() {
    // feature-retention.md ledger order (2026-10-16 audit, 17 rows).
    static const std::vector<std::string> kLedger = {
        "widescreen", "hd_packs", "second_screen", "frame_generation", "rumble",
        "radio", "bgm", "analog_triggers", "save_states", "save_migration",
        "hud", "traffic_ai", "chase_camera", "economy", "world_streaming",
        "task_scheduler", "minigames",
    };
    return kLedger;
}

bool isRtNameChar(char c) { return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'; }

// Extracts quoted "RT_[A-Z0-9_]+" literals from one line (aware of simple
// backslash escapes so an escaped quote cannot split a literal).
void extractRtLiterals(const std::string& line, std::vector<std::string>& out) {
    for (std::size_t i = 0; i < line.size(); ++i) {
        if (line[i] != '"') continue;
        std::size_t j = i + 1;
        std::string token;
        bool closed = false;
        while (j < line.size()) {
            const char c = line[j];
            if (c == '\\' && j + 1 < line.size()) { j += 2; continue; }
            if (c == '"') { closed = true; break; }
            token.push_back(c);
            ++j;
        }
        if (closed && token.size() > 3 && token.compare(0, 3, "RT_") == 0) {
            bool ok = true;
            for (std::size_t k = 3; k < token.size(); ++k) ok = ok && isRtNameChar(token[k]);
            if (ok) out.push_back(token);
        }
        i = j;  // continue after the closing quote (or line end)
    }
}

std::string csvEscape(const std::string& value) {
    bool needsQuotes = value.find(',') != std::string::npos ||
                       value.find('"') != std::string::npos ||
                       value.find('\n') != std::string::npos;
    if (!needsQuotes) return value;
    std::string out = "\"";
    for (char c : value) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out += "\"";
    return out;
}

std::string jsonEscape(const std::string& value) {
    std::string out;
    for (char c : value) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out.push_back(c);
                }
        }
    }
    return out;
}

std::vector<std::string> splitPipe(const std::string& raw) {
    std::vector<std::string> parts;
    if (raw.empty()) return parts;
    std::string current;
    std::istringstream stream(raw);
    while (std::getline(stream, current, '|')) if (!current.empty()) parts.push_back(current);
    return parts;
}

}  // namespace

const char* retentionStatusName(RetentionStatus status) {
    switch (status) {
        case RetentionStatus::Live: return "live";
        case RetentionStatus::Wired: return "wired";
        case RetentionStatus::OracleOnly: return "oracle-only";
        case RetentionStatus::DocOnly: return "doc-only";
    }
    return "unknown";
}

RetentionStatus retentionStatusFromName(const std::string& name) {
    if (name == "live") return RetentionStatus::Live;
    if (name == "wired") return RetentionStatus::Wired;
    if (name == "oracle-only") return RetentionStatus::OracleOnly;
    if (name == "doc-only") return RetentionStatus::DocOnly;
    return RetentionStatus::DocOnly;  // unknown names fail the doc gate downstream
}

const std::vector<EnhancementSpec>& enhancementRegistry() { return registryTable(); }

const std::vector<std::string>& retentionLedgerRows() { return ledgerRows(); }

EnvScanResult scanRtEnvUsage(const std::filesystem::path& nativeRoot) {
    EnvScanResult result;
    static const char* kExtensions[] = {".cpp", ".h", ".cc", ".hpp", ".c"};

    std::function<void(const std::filesystem::path&, const std::string&)> walk =
        [&](const std::filesystem::path& dir, const std::string& relPrefix) {
            std::vector<std::filesystem::path> entries;
            std::error_code ec;
            for (std::filesystem::directory_iterator it(dir, ec), end; it != end && !ec; it.increment(ec)) {
                entries.push_back(it->path());
            }
            std::sort(entries.begin(), entries.end());
            for (const auto& entry : entries) {
                const std::string name = entry.filename().string();
                const std::string rel = relPrefix.empty() ? name : relPrefix + "/" + name;
                std::error_code isDirEc;
                const bool isDir = std::filesystem::is_directory(entry, isDirEc);
                // Exclusions: vendored/generated trees (android/ = Gradle +
                // SDL3 + .cxx build outputs) and any dotted directory.
                if (isDir) {
                    if (!name.empty() && name[0] == '.') continue;
                    if (name == "android") continue;
                    walk(entry, rel);
                    continue;
                }
                bool wanted = false;
                for (const char* ext : kExtensions) {
                    if (name.size() >= std::strlen(ext) &&
                        name.compare(name.size() - std::strlen(ext), std::strlen(ext), ext) == 0) {
                        wanted = true;
                        break;
                    }
                }
                if (!wanted) continue;
                std::ifstream input(entry);
                if (!input) continue;
                std::string line;
                int lineNo = 0;
                while (std::getline(input, line)) {
                    ++lineNo;
                    std::vector<std::string> literals;
                    extractRtLiterals(line, literals);
                    for (const auto& var : literals) {
                        EnvUsageSite site;
                        site.file = "src/native/" + rel;
                        site.line = lineNo;
                        site.var = var;
                        site.directGetenv = line.find("getenv(") != std::string::npos;
                        result.sites.push_back(site);
                        result.byVar[var].push_back(site);
                        if (site.directGetenv) ++result.directGetenvSites;
                    }
                }
            }
        };
    walk(nativeRoot, "");

    // FNV-1a hash over the deterministic site list (determinism check).
    std::uint64_t hash = 1469598103934665603ull;
    for (const auto& site : result.sites) {
        const std::string key = site.file + ":" + std::to_string(site.line) + ":" + site.var +
                                (site.directGetenv ? ":g" : ":q");
        for (char c : key) { hash ^= static_cast<std::uint8_t>(c); hash *= 1099511628211ull; }
    }
    result.siteListHash = hash;
    return result;
}

CoverageReport checkRetentionCoverage(const EnvScanResult& scan,
                                      const std::filesystem::path& repoRoot) {
    return checkRetentionCoverage(enhancementRegistry(), scan, repoRoot);
}

CoverageReport checkRetentionCoverage(const std::vector<EnhancementSpec>& registry,
                                      const EnvScanResult& scan,
                                      const std::filesystem::path& repoRoot) {
    CoverageReport report;
    report.registryEntries = registry.size();

    std::set<std::string> registryVars;
    std::set<std::string> ledgerCovered;
    std::set<std::string> codeVars;
    for (const auto& [var, sites] : scan.byVar) (void)sites, codeVars.insert(var);
    report.distinctCodeVars = codeVars.size();

    for (const EnhancementSpec& spec : registry) {
        const std::vector<std::string> envVars = splitPipe(spec.envVars);
        if (!envVars.empty()) ++report.registryEnvEntries;
        for (const auto& var : envVars) registryVars.insert(var);

        if (spec.status == RetentionStatus::DocOnly) report.docOnlyIds.push_back(spec.id);

        // Direction 2: registry env var must still occur in code.
        for (const auto& var : envVars) {
            if (scan.byVar.find(var) == scan.byVar.end()) {
                report.registryVarsAbsentFromCode.push_back(var);
            }
        }

        // Owner paths must exist.
        for (const auto& owner : splitPipe(spec.owners)) {
            ++report.ownerCount;
            std::error_code ec;
            if (!std::filesystem::exists(repoRoot / owner, ec)) {
                CoverageIssue issue;
                issue.id = spec.id;
                issue.detail = "owner path missing: " + owner;
                report.missingOwners.push_back(issue);
            }
        }

        // Doc citations: >= 1 required; every citation must verify (exist),
        // and at least one citation must still reference the entry's env
        // var(s) — a flag whose documentation vanished fails the gate.
        const std::vector<std::string> docs = splitPipe(spec.docs);
        if (docs.empty()) {
            CoverageIssue issue;
            issue.id = spec.id;
            issue.detail = "no doc/plan citation";
            report.docIssues.push_back(issue);
        }
        bool varReferenced = false;
        for (const auto& cite : docs) {
            ++report.docCiteCount;
            bool ok = false;
            bool referencesVar = false;
            std::string detail;
            if (cite == "plan") {
                const auto planPath = repoRoot / ".qoder" / "wave2-plan-sonnet.md";
                std::error_code ec;
                ok = std::filesystem::exists(planPath, ec);
                if (!ok) detail = "plan citation: .qoder/wave2-plan-sonnet.md missing";
            } else if (cite.rfind("code:", 0) == 0) {
                const std::string path = cite.substr(5);
                const auto fullPath = repoRoot / path;
                std::error_code ec;
                if (!std::filesystem::exists(fullPath, ec)) {
                    detail = "code citation path missing: " + path;
                } else if (!envVars.empty()) {
                    // The cited file ideally references the first env var; a
                    // miss here only means this citation is background — the
                    // entry-level check below requires >= 1 referencing cite.
                    std::ifstream input(fullPath);
                    std::string line;
                    while (std::getline(input, line) && !referencesVar) {
                        if (line.find(envVars.front()) != std::string::npos) referencesVar = true;
                    }
                    ok = true;  // existence is the per-citation contract
                } else {
                    ok = true;  // feature-only rows: existence is the contract
                }
            } else {
                // docs/native-reference/... path (optional :LINE suffix).
                std::string path = cite;
                const std::size_t colon = path.rfind(':');
                if (colon != std::string::npos && colon > 4) {
                    const std::string suffix = path.substr(colon + 1);
                    bool allDigits = !suffix.empty();
                    for (char c : suffix) allDigits = allDigits && c >= '0' && c <= '9';
                    if (allDigits) path = path.substr(0, colon);
                }
                std::error_code ec;
                const auto fullPath = repoRoot / path;
                if (!std::filesystem::exists(fullPath, ec)) {
                    detail = "doc file missing: " + path;
                } else if (!envVars.empty()) {
                    std::ifstream input(fullPath);
                    std::string line;
                    // Any one of the entry's env vars present marks this
                    // citation as the referencing one; a miss means it is a
                    // background citation (existence still verifies).
                    while (std::getline(input, line) && !referencesVar) {
                        for (const auto& var : envVars) {
                            if (line.find(var) != std::string::npos) { referencesVar = true; break; }
                        }
                    }
                    ok = true;
                } else {
                    ok = true;
                }
            }
            if (!ok) {
                CoverageIssue issue;
                issue.id = spec.id;
                issue.detail = detail.empty() ? ("citation failed: " + cite) : detail;
                report.docIssues.push_back(issue);
            }
            varReferenced = varReferenced || referencesVar;
        }
        if (!envVars.empty() && !varReferenced) {
            CoverageIssue issue;
            issue.id = spec.id;
            issue.detail = "no citation references " + envVars.front();
            report.docIssues.push_back(issue);
        }

        if (std::string(spec.ledger) != "-") ledgerCovered.insert(spec.ledger);
    }

    // Direction 1: every code var must be registered.
    for (const auto& var : codeVars) {
        if (registryVars.count(var) == 0) report.codeVarsMissingFromRegistry.push_back(var);
    }
    report.coveredCodeVars = codeVars.size() - report.codeVarsMissingFromRegistry.size();

    for (const auto& row : retentionLedgerRows()) {
        if (ledgerCovered.count(row) != 0) ++report.ledgerCovered;
        else report.ledgerUncovered.push_back(row);
    }
    return report;
}

bool CoverageReport::ok() const {
    return codeVarsMissingFromRegistry.empty() && registryVarsAbsentFromCode.empty() &&
           missingOwners.empty() && docIssues.empty() && ledgerUncovered.empty() &&
           ledgerCovered == retentionLedgerRows().size();
}

std::string registryCsv() {
    std::ostringstream out;
    out << kCsvHeader << "\n";
    for (const EnhancementSpec& spec : enhancementRegistry()) {
        out << csvEscape(spec.id) << ','
            << csvEscape(spec.ledger) << ','
            << csvEscape(spec.envVars) << ','
            << csvEscape(spec.persistedKeys) << ','
            << csvEscape(spec.defaultValue) << ','
            << csvEscape(spec.gating) << ','
            << csvEscape(spec.owners) << ','
            << csvEscape(spec.docs) << ','
            << csvEscape(retentionStatusName(spec.status)) << ','
            << csvEscape(spec.notes) << '\n';
    }
    return out.str();
}

std::string registryJson() {
    std::ostringstream out;
    out << "[\n";
    const auto& registry = enhancementRegistry();
    for (std::size_t i = 0; i < registry.size(); ++i) {
        const EnhancementSpec& spec = registry[i];
        out << "  {\n";
        out << "    \"id\": \"" << jsonEscape(spec.id) << "\",\n";
        out << "    \"ledger\": \"" << jsonEscape(spec.ledger) << "\",\n";
        auto array = [&](const char* key, const char* raw) {
            out << "    \"" << key << "\": [";
            const auto parts = splitPipe(raw);
            for (std::size_t p = 0; p < parts.size(); ++p) {
                if (p) out << ", ";
                out << '"' << jsonEscape(parts[p]) << '"';
            }
            out << "],\n";
        };
        array("env_vars", spec.envVars);
        array("persisted_keys", spec.persistedKeys);
        out << "    \"default\": \"" << jsonEscape(spec.defaultValue) << "\",\n";
        out << "    \"gating\": \"" << jsonEscape(spec.gating) << "\",\n";
        array("owners", spec.owners);
        array("docs", spec.docs);
        out << "    \"status\": \"" << jsonEscape(retentionStatusName(spec.status)) << "\",\n";
        out << "    \"notes\": \"" << jsonEscape(spec.notes) << "\"\n";
        out << "  }" << (i + 1 == registry.size() ? "\n" : ",\n");
    }
    out << "]\n";
    return out.str();
}

}  // namespace rt::native::gameplay
