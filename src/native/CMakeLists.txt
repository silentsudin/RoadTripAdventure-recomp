# src/native/CMakeLists.txt
#
# Standalone build for the authored native asset pipeline (src/native/assets).
# Self-contained by design: the library and tools below are pure host-side C++
# (wire-format container, VIF/DMA chain decode, CAR model and texture-chain
# converters, loaders). Nothing links PS2Recomp, parallel-gs, the EE/VU1/GS
# layers, any submodule, or any guest/disc code, and no game data ships here:
# the tools take a disc file path argument at run time (the user's own
# extracted files). Configure on its own, e.g.:
#   cmake -S src/native -B build/native-assets
#   cmake --build build/native-assets
# or add_subdirectory(src/native) from a parent project: no FetchContent, no
# REQUIRED find_package, no CTest registration, so it stays quiet everywhere.

cmake_minimum_required(VERSION 3.21)
project(rt_native_assets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------- warnings
if(MSVC)
    set(RT_NATIVE_WARN_OPTIONS /W4)
else()
    set(RT_NATIVE_WARN_OPTIONS -Wall -Wextra)
endif()

# ---------------------------------------------------------------- library
# The authored asset code only; explicit file list (no GLOB). The header-only
# files (native_image.h, native_model.h) are pulled in alongside their .cpp
# files by the quoted includes.
set(RT_NATIVE_ASSET_SOURCES
    assets/container.cpp        # wire format: serialize/deserialize + provenance rules
    assets/vif_chain.cpp        # VIF/DMA chain -> vertex batches
    assets/model_converter.cpp  # CAR (CARn/Qnn.BIN) -> mesh-only HostAssetContainer
    assets/field_converter.cpp  # FLD/COURSE field-tile VIF region -> mesh-only HostAssetContainer
    assets/field_tile_index.cpp # ROM LBA descriptor table -> streaming id/FLD filename index
    assets/course_data.cpp      # COURSE container header/region parser (route semantics deferred)
    assets/nav_routes.cpp       # ROM-resident AI route-graph runtime reader (no embedded tables)
    assets/iso_extract.cpp      # first-run ISO9660 reader + gate + native asset-set extractor (user's image only)
    gameplay/nav_path.cpp       # deterministic route-graph centerline traversal
    gameplay/nav_walkable.cpp   # fixed-step centerline waypoint traversal diagnostics
    gameplay/nav_drive.cpp      # deterministic kinematic centerline follower (render-visible drive)
    gameplay/nav_route_bridge.cpp # NavPath centerline -> authored RaceRoute zones
    gameplay/world_layout.cpp    # authentic fixed-point <-> tile-id <-> local-unit mapping (proven formulas)
    preview/ui_menu.cpp           # title/menu/options screen builders per ui-parity.md
    preview/title_ui.cpp         # W6-02 title New Game / Continue UI state (W8-01 wiring)
    assets/title_screen.cpp       # runtime SYS/TITLE.GSL backdrop decoder
    assets/hud_art.cpp            # runtime race HUD art (METER dial + SCORE atlas)
    assets/image_converter.cpp  # texture upload chain -> RGBA8 ImageRecord
    assets/image_loader.cpp     # ImageRecord -> NativeImage
    assets/model_loader.cpp     # MeshRecord + images -> NativeModel
    preview/car_mesh.cpp          # CAR container -> render-ready NativeModel (scale/centred/normals)
    assets/music_seq.cpp        # Road Trip TVB voice-bank and TSQ sequence container parsers
    assets/setup_wizard.cpp     # first-run validated native asset cache
    assets/tsq_sequencer.cpp    # bounded TSQ event decoder and offline TVB PCM renderer
    gameplay/car_physics.cpp    # authored car-physics tick (no guest code)
    gameplay/guest_vehicle.cpp  # translated guest longitudinal model (0x218EE0/0x21A310)
    gameplay/camera.cpp         # authored chase camera (spring/slew + look-at view matrix)
    gameplay/task.cpp           # authored task scheduler (48-slot pool + parent/sibling tree)
    gameplay/dialogue.cpp       # authored NPC dialogue line and input-advance model
    gameplay/ai.cpp             # authored waypoint traffic brain (no guest code)
    gameplay/world.cpp          # authored world/field model (8x8 cells, road path, cull)
    gameplay/daynight.cpp       # authored day/night lighting cycle (phase bands, fog, light-matrix blend)
    gameplay/daycycle.cpp       # authored scripted day/night lighting step machine
    gameplay/race.cpp           # authored race timer, best records, and standings
    gameplay/results.cpp        # authored post-race result snapshot and record updates
    gameplay/session.cpp        # authored Adventure explore/race/results session loop
    gameplay/geometry.cpp       # authored ground-plane zone predicates and arrival checks
    gameplay/minigames.cpp      # authored mini-game scoring predicates and score model
    gameplay/route.cpp          # authored race-route zones, progress, and arrival model
    gameplay/motion.cpp         # authored render motion-vector math (pure C++, no GPU)
    gameplay/tile_streamer.cpp  # hardware-style 4-slot field-tile streaming algebra (slot = id & 3)
    audio/vag.cpp               # authored PS ADPCM (VAG) decoder (no PS2/guest code)
    audio/vag_loader.cpp        # authored VAG file/raw-stream loader -> PCM source storage
    audio/mixer.cpp             # authored mixer model: N channels -> stereo out, no device
    audio/engine.cpp            # authored sound engine: voices + BGM/SE + mixer driver (replaces SNDMOD RPC)
    audio/music_library.cpp     # injected BGM file-table index -> VAG resolver + SoundEngine bridge
    audio/bgm_player.cpp        # bounded TSQ+TVB BGM pre-render and stereo PCM loop player
    gameplay/save.cpp           # authored memory-card save parse/serialize/migrate (13,384-byte progress block)
    gameplay/radio.cpp          # authored radio-station model (save byte -> stereo VAG pair, town-frame apply)
    gameplay/rumble.cpp         # authored rumble/haptics grading (collision/landing/scrape -> motor levels)
    gameplay/progression.cpp    # authored progression mutations over the documented save fields
    gameplay/collectibles.cpp   # authored stamp and coin pickup model
    gameplay/shop.cpp           # authored item-shop buy/sell and selection model
    gameplay/memcard.cpp        # host-file replacement for two memory-card save slots
    gameplay/menu.cpp           # authored menu, pause, and controller-config state models
    gameplay/input.cpp          # authored player input (pad/analog -> CarInput)
    gameplay/game_loop.cpp      # authored simulation frame (task walker -> AI/car/camera -> cull -> snapshot)
    gameplay/interp.cpp         # render-frame interpolation (fixed-step accumulator + render-pose blend)
    gameplay/perfstats.cpp      # host performance windows and ASCII overlay model
    runtime/testsocket.cpp      # JSON-lines native regression control socket
    runtime/guest_facade.cpp    # explicitly mapped guest-address compatibility facade
    render/race_terrain.cpp     # COURSE WorldStrip terrain -> renderer-neutral textured mesh
)
# ---- staged module sources (coordinator-managed parallel dispatch). These
# candidate files land from independent work streams; they are compiled into
# the library the moment the file exists, so the owning agent never needs to
# edit this file. The coordinator converts to unconditional entries after the
# staged files are integrated.
foreach(_rt_staged_source
        assets/course_props.cpp assets/car_models.cpp assets/glyph_atlas.cpp assets/hd_pack.cpp
        gameplay/rules_tables.cpp gameplay/hud_layout.cpp gameplay/glyphs.cpp
        gameplay/settings.cpp gameplay/map_view.cpp
        app/output_modes.cpp
        # Wave-2 staged sources (coordinator Step 0; see .qoder/wave2-plan-sonnet.md §0).
        # EXISTS-guarded: absent files are harmless until their batch lands.
        assets/script_image.cpp assets/price_tables.cpp gameplay/script_vm.cpp
        gameplay/app_flow.cpp gameplay/vehicle_model.cpp gameplay/terrain_query.cpp
        gameplay/race_rules.cpp gameplay/item_economy.cpp gameplay/text_script.cpp
        gameplay/save_v2.cpp gameplay/audio_graph.cpp audio/vag_stream.cpp
         gameplay/hud_live.cpp gameplay/minimap.cpp gameplay/save_slots.cpp
         gameplay/input_map.cpp gameplay/enhancements.cpp app/enhancements_app.cpp)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_rt_staged_source}")
        list(APPEND RT_NATIVE_ASSET_SOURCES "${_rt_staged_source}")
    endif()
endforeach()
add_library(rt_native_assets STATIC ${RT_NATIVE_ASSET_SOURCES})
# PUBLIC so consumers #include "assets/container.h" against this include root;
# cxx_std_20 PUBLIC because the headers themselves require C++20 (std::span).
target_include_directories(rt_native_assets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(rt_native_assets PUBLIC cxx_std_20)
target_compile_options(rt_native_assets PRIVATE ${RT_NATIVE_WARN_OPTIONS})

# W9-02: player-car mesh/livery asset and chase-scene submission.
add_library(rt_native_car_render STATIC preview/car_render.cpp)

# W10: the translated guest corpus (scripts/ghidra/transliterate.py output in
# guest/gen + shim). Built as a static library so the 0-error gate compiles
# every TU; it links into the app subsystem by subsystem at W12. Needs the
# generated decls table: python3 scripts/ghidra/transliterate.py --decls.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../build/decomp/guest_decls.inc")
    add_library(rt_guest STATIC guest/guest_mem.cpp)
    file(GLOB RT_GUEST_GEN CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/guest/gen/*.cpp")
    target_sources(rt_guest PRIVATE ${RT_GUEST_GEN})
    target_include_directories(rt_guest PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../build/decomp")
    target_compile_features(rt_guest PRIVATE cxx_std_17)
    # W11 package pins (G2): worker-computed expected values from pseudocode.
    foreach(p 02 03 04 05 06 07)
        add_executable(w11_p${p}_test "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_guest/test_w11_${p}.cpp")
        target_link_libraries(w11_p${p}_test PRIVATE rt_guest)
        target_include_directories(w11_p${p}_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
        target_compile_features(w11_p${p}_test PRIVATE cxx_std_17)
    endforeach()
endif()
target_link_libraries(rt_native_car_render PUBLIC rt_native_assets)
target_include_directories(rt_native_car_render PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(rt_native_car_render PUBLIC cxx_std_20)
target_compile_options(rt_native_car_render PRIVATE ${RT_NATIVE_WARN_OPTIONS})
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w5" "${CMAKE_CURRENT_BINARY_DIR}/native_w5")  # W4-05 media + w5 tests

# ---------------------------------------------------------------- Android app
# SDL's Android activity loads a single libmain.so. Keep this branch before the
# host-only tools: Android packages only the authored app, never host tools.
if(ANDROID)
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.18
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(SDL3)

    find_library(RT_ANDROID_VULKAN_LIB vulkan REQUIRED)
    find_library(RT_ANDROID_LOG_LIB log REQUIRED)
    add_library(main SHARED app/main.cpp app/Autopilot.cpp preview/preview.cpp)
    target_link_libraries(main PRIVATE rt_native_assets SDL3::SDL3
        ${RT_ANDROID_VULKAN_LIB} ${RT_ANDROID_LOG_LIB})
    target_compile_features(main PRIVATE cxx_std_20)
    target_compile_options(main PRIVATE ${RT_NATIVE_WARN_OPTIONS}
        -Wno-missing-field-initializers)
    # Android 15-capable devices may use 16 KiB pages. NDK r27+ honors this
    # linker contract; older linkers safely ignore this ordinary ELF setting.
    target_link_options(main PRIVATE "-Wl,-z,max-page-size=16384")
    return()
endif()

# ---------------------------------------------------------------- optional host packages
# Vulkan and SDL3 are prospective hosts: the asset code (rt_native_assets) is
# pure C++ and links neither. The rt-asset-preview tool DOES link Vulkan when
# it is found. Both lookups are guarded (QUIET, never REQUIRED) so this tree
# configures and builds on machines with neither installed.
find_package(Vulkan QUIET)
find_package(SDL3 QUIET)
if(Vulkan_FOUND)
    message(STATUS "rt_native_assets: Vulkan found (optional; used by rt-asset-preview)")
endif()
if(SDL3_FOUND)
    message(STATUS "rt_native_assets: SDL3 found (optional; not linked by any target here)")
endif()
if(NOT Vulkan_FOUND AND NOT SDL3_FOUND)
    message(STATUS "rt_native_assets: Vulkan/SDL3 not found (optional; configuring without them)")
endif()

# ---------------------------------------------------------------- tools
# Offline dev tools over the authored pipeline; each takes a disc file path
# argument at run time and links rt_native_assets only (no PS2/guest deps; no
# game data ships with this tree).
add_executable(rt-asset-convert tools/asset_convert_main.cpp)  # convertCarModels + summary
target_link_libraries(rt-asset-convert PRIVATE rt_native_assets)
target_compile_features(rt-asset-convert PRIVATE cxx_std_20)
target_compile_options(rt-asset-convert PRIVATE ${RT_NATIVE_WARN_OPTIONS})

add_executable(rt-asset-inspect tools/asset_inspect_main.cpp)  # convertImage + summary
target_link_libraries(rt-asset-inspect PRIVATE rt_native_assets)
target_compile_features(rt-asset-inspect PRIVATE cxx_std_20)
target_compile_options(rt-asset-inspect PRIVATE ${RT_NATIVE_WARN_OPTIONS})

# ---- staged tool target (coordinator-managed parallel dispatch; see the
# staged-source comment above). Registers itself once the source lands.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools/pack_extract_main.cpp")
    add_executable(rt-pack-extract tools/pack_extract_main.cpp)
    target_link_libraries(rt-pack-extract PRIVATE rt_native_assets)
    target_compile_features(rt-pack-extract PRIVATE cxx_std_20)
    target_compile_options(rt-pack-extract PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endif()

# Wave-2 offline tools (coordinator Step 0 glob; see .qoder/wave2-plan-sonnet.md).
# tools/w2_<name>_main.cpp -> rt-w2-<name>; links rt_native_assets only.
file(GLOB RT_W2_TOOL_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/tools/w2_*_main.cpp")
foreach(_rt_w2_tool_source IN LISTS RT_W2_TOOL_SOURCES)
    get_filename_component(_rt_w2_tool_we "${_rt_w2_tool_source}" NAME_WE)
    string(REGEX REPLACE "^w2_" "" _rt_w2_tool_short "${_rt_w2_tool_we}")
    if(_rt_w2_tool_short MATCHES "_main$")
        string(REGEX REPLACE "_main$" "" _rt_w2_tool_short "${_rt_w2_tool_short}")
    endif()
    add_executable(rt-w2-${_rt_w2_tool_short} "${_rt_w2_tool_source}")
    target_link_libraries(rt-w2-${_rt_w2_tool_short} PRIVATE rt_native_assets)
    target_compile_features(rt-w2-${_rt_w2_tool_short} PRIVATE cxx_std_20)
    target_compile_options(rt-w2-${_rt_w2_tool_short} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endforeach()

# Optional offscreen Vulkan/MoltenVK preview (only when Vulkan is found).
if(Vulkan_FOUND)
    add_executable(rt-asset-preview tools/preview_main.cpp preview/preview.cpp)
    target_link_libraries(rt-asset-preview PRIVATE rt_native_assets Vulkan::Vulkan)
    target_compile_features(rt-asset-preview PRIVATE cxx_std_20)
    target_compile_options(rt-asset-preview PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    add_executable(rt-asset-animate tools/animate_main.cpp preview/preview.cpp)
    target_link_libraries(rt-asset-animate PRIVATE rt_native_assets Vulkan::Vulkan)
    target_compile_features(rt-asset-animate PRIVATE cxx_std_20)
    target_compile_options(rt-asset-animate PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    add_executable(rt-asset-drive tools/drive_main.cpp preview/preview.cpp)
    target_link_libraries(rt-asset-drive PRIVATE rt_native_assets Vulkan::Vulkan)
    target_compile_features(rt-asset-drive PRIVATE cxx_std_20)
    target_compile_options(rt-asset-drive PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    add_executable(rt-asset-scene tools/scene_main.cpp preview/preview.cpp)
    target_link_libraries(rt-asset-scene PRIVATE rt_native_assets Vulkan::Vulkan)
    target_compile_features(rt-asset-scene PRIVATE cxx_std_20)
    target_compile_options(rt-asset-scene PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    add_executable(rt-asset-menu tools/menu_main.cpp preview/preview.cpp)
    target_link_libraries(rt-asset-menu PRIVATE rt_native_assets Vulkan::Vulkan)
    target_compile_features(rt-asset-menu PRIVATE cxx_std_20)
    target_compile_options(rt-asset-menu PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    add_executable(rt-w25-car-preview "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w25/car_preview_main.cpp" preview/preview.cpp)
    target_link_libraries(rt-w25-car-preview PRIVATE rt_native_car_render Vulkan::Vulkan)
    target_compile_features(rt-w25-car-preview PRIVATE cxx_std_20)
    target_compile_options(rt-w25-car-preview PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)

    # First interactive host increment: the scene is authored by preview.cpp
    # and copied to an SDL3 Vulkan swapchain for presentation.
    if(SDL3_FOUND)
        add_executable(rt-app app/main.cpp app/Autopilot.cpp preview/preview.cpp)
        target_link_libraries(rt-app PRIVATE rt_native_assets Vulkan::Vulkan SDL3::SDL3)
        target_compile_features(rt-app PRIVATE cxx_std_20)
        target_compile_options(rt-app PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)
    endif()
endif()

# ---------------------------------------------------------------- tests (not run)
option(RT_NATIVE_ASSETS_BUILD_TESTS
       "Build the synthetic native-asset contract tests as executables (not run)" OFF)
if(RT_NATIVE_ASSETS_BUILD_TESTS)
    if(Vulkan_FOUND)
        add_executable(w25_car_render_test
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w25/car_render_test.cpp" preview/preview.cpp)
        target_link_libraries(w25_car_render_test PRIVATE rt_native_car_render Vulkan::Vulkan)
        target_compile_features(w25_car_render_test PRIVATE cxx_std_20)
        target_compile_options(w25_car_render_test PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)
    endif()
    # Wave-2 batch tests (coordinator Step 0 glob; see .qoder/wave2-plan-sonnet.md).
    # tests/native_w2/<stem>_test.cpp -> executable w2_<stem> (stem keeps _test,
    # so targets read w2_script_vm_test etc.). Synthesize nothing when empty.
    file(GLOB RT_W2_TEST_SOURCES CONFIGURE_DEPENDS
         "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/*_test.cpp")
    foreach(_rt_w2_test_source IN LISTS RT_W2_TEST_SOURCES)
        get_filename_component(_rt_w2_test_we "${_rt_w2_test_source}" NAME_WE)
        add_executable(w2_${_rt_w2_test_we} "${_rt_w2_test_source}")
        target_link_libraries(w2_${_rt_w2_test_we} PRIVATE rt_native_assets)
        target_compile_features(w2_${_rt_w2_test_we} PRIVATE cxx_std_20)
        target_compile_options(w2_${_rt_w2_test_we} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    endforeach()

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w8" "${CMAKE_CURRENT_BINARY_DIR}/native_w8")  # W4-08 script_ops_test
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w4" "${CMAKE_CURRENT_BINARY_DIR}/native_w4")  # W4-06 race_hud_layout_test
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w12" "${CMAKE_CURRENT_BINARY_DIR}/native_w12")  # W4-12 HUD GSL pages
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w11" "${CMAKE_CURRENT_BINARY_DIR}/native_w11")  # W4-11 gauge_needle_test
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w14" "${CMAKE_CURRENT_BINARY_DIR}/native_w14")  # W5-03 race state probe
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w13" "${CMAKE_CURRENT_BINARY_DIR}/native_w13")  # W5-01 gauge_rpm_test
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w16" "${CMAKE_CURRENT_BINARY_DIR}/native_w16")  # W6-02 title save/load UI
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w17" "${CMAKE_CURRENT_BINARY_DIR}/native_w17")  # W6-03 gear glyph + mph reconciliation
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w18" "${CMAKE_CURRENT_BINARY_DIR}/native_w18")  # W6-04 driveline rpm/gear exposure
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w19" "${CMAKE_CURRENT_BINARY_DIR}/native_w19")  # W7-01 gauge needle draw spec
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w20" "${CMAKE_CURRENT_BINARY_DIR}/native_w20")  # W7-03 town collision grid contract
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w21" "${CMAKE_CURRENT_BINARY_DIR}/native_w21")  # W8-04 rotated-quad preview path
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w22" "${CMAKE_CURRENT_BINARY_DIR}/native_w22")  # W8-02 autopilot decision logic
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w23" "${CMAKE_CURRENT_BINARY_DIR}/native_w23")  # W8-03 collision sizing repair
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w24" "${CMAKE_CURRENT_BINARY_DIR}/native_w24")  # W9-01 race terrain fidelity
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w25" "${CMAKE_CURRENT_BINARY_DIR}/native_w25")  # W9-02 own car chase view
    add_executable(w24_race_terrain_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w24/race_terrain_test.cpp")
    target_link_libraries(w24_race_terrain_test PRIVATE rt_native_assets)
    target_compile_features(w24_race_terrain_test PRIVATE cxx_std_20)
    target_compile_options(w24_race_terrain_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    add_executable(w26_guest_vehicle_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w26/guest_vehicle_test.cpp")
    target_link_libraries(w26_guest_vehicle_test PRIVATE rt_native_assets)
    target_compile_features(w26_guest_vehicle_test PRIVATE cxx_std_20)
    target_compile_options(w26_guest_vehicle_test PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)
    add_executable(w3_enhancements_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/enhancements_app_test.cpp")
    target_link_libraries(w3_enhancements_test PRIVATE rt_native_assets)
    target_compile_features(w3_enhancements_test PRIVATE cxx_std_20)
    target_compile_options(w3_enhancements_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(w3_minimap_track_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/minimap_track_test.cpp")
    target_link_libraries(w3_minimap_track_test PRIVATE rt_native_assets)
    target_compile_features(w3_minimap_track_test PRIVATE cxx_std_20)
    target_compile_options(w3_minimap_track_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(w3_text_metrics_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/text_metrics_test.cpp")
    target_link_libraries(w3_text_metrics_test PRIVATE rt_native_assets)
    target_compile_features(w3_text_metrics_test PRIVATE cxx_std_20)
    target_compile_options(w3_text_metrics_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(w3_collision_grid_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/collision_grid_test.cpp")
    target_link_libraries(w3_collision_grid_test PRIVATE rt_native_assets)
    target_compile_features(w3_collision_grid_test PRIVATE cxx_std_20)
    target_compile_options(w3_collision_grid_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(w3_save_slots_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/save_slots_test.cpp")
    target_link_libraries(w3_save_slots_test PRIVATE rt_native_assets)
    target_compile_features(w3_save_slots_test PRIVATE cxx_std_20)
    target_compile_options(w3_save_slots_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_facade_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_native_facade.cpp")
    target_link_libraries(native_facade_test PRIVATE rt_native_assets)
    target_compile_features(native_facade_test PRIVATE cxx_std_20)
    target_compile_options(native_facade_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    # tests/native_assets/test_*.cpp: synthetic contract tests, no game data.
    # They include the headers by repo-relative path ("../../src/native/assets/...")
    # and link rt_native_assets for the implementations under test. Executables
    # only: no CTest registration, no add_test, nothing runs at build time;
    # run them by hand.
    add_executable(native_assets_container_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_container.cpp")
    target_link_libraries(native_assets_container_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_container_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_container_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_loader_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_loader.cpp")
    target_link_libraries(native_assets_loader_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_loader_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_loader_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_vif_chain_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_vif_chain.cpp")
    target_link_libraries(native_assets_vif_chain_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_vif_chain_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_vif_chain_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_model_converter_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_model_converter.cpp")
    target_link_libraries(native_assets_model_converter_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_model_converter_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_model_converter_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_preview_car_mesh_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/native_preview_car_mesh_test.cpp")
    target_link_libraries(native_preview_car_mesh_test PRIVATE rt_native_assets)
    target_compile_features(native_preview_car_mesh_test PRIVATE cxx_std_20)
    target_compile_options(native_preview_car_mesh_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_preview_lighting_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/native_preview_lighting_test.cpp")
    target_link_libraries(native_preview_lighting_test PRIVATE rt_native_assets)
    target_compile_features(native_preview_lighting_test PRIVATE cxx_std_20)
    target_compile_options(native_preview_lighting_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_preview_ui_menu_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/native_preview_ui_menu_test.cpp")
    target_link_libraries(native_preview_ui_menu_test PRIVATE rt_native_assets)
    target_compile_features(native_preview_ui_menu_test PRIVATE cxx_std_20)
    target_compile_options(native_preview_ui_menu_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_title_screen_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/native_title_screen_test.cpp")
    target_link_libraries(native_title_screen_test PRIVATE rt_native_assets)
    target_compile_features(native_title_screen_test PRIVATE cxx_std_20)
    target_compile_options(native_title_screen_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_hud_art_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/native_hud_art_test.cpp")
    target_link_libraries(native_hud_art_test PRIVATE rt_native_assets)
    target_compile_features(native_hud_art_test PRIVATE cxx_std_20)
    target_compile_options(native_hud_art_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_field_converter_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_field_converter.cpp")
    target_link_libraries(native_assets_field_converter_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_field_converter_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_field_converter_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_iso_extract_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_iso_extract.cpp")
    target_link_libraries(native_assets_iso_extract_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_iso_extract_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_iso_extract_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_field_tile_index_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_field_tile_index.cpp")
    target_link_libraries(native_assets_field_tile_index_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_field_tile_index_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_field_tile_index_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_nav_routes_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_nav_routes.cpp")
    target_link_libraries(native_assets_nav_routes_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_nav_routes_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_nav_routes_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_nav_path_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_nav_path.cpp")
    target_link_libraries(native_assets_nav_path_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_nav_path_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_nav_path_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_nav_route_bridge_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_nav_route_bridge.cpp")
    target_link_libraries(native_assets_nav_route_bridge_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_nav_route_bridge_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_nav_route_bridge_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_nav_walkable_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_nav_walkable.cpp")
    target_link_libraries(native_assets_nav_walkable_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_nav_walkable_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_nav_walkable_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_nav_drive_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_nav_drive.cpp")
    target_link_libraries(native_assets_nav_drive_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_nav_drive_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_nav_drive_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_world_layout_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_world_layout.cpp")
    target_link_libraries(native_assets_world_layout_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_world_layout_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_world_layout_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_course_data_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_course_data.cpp")
    target_link_libraries(native_assets_course_data_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_course_data_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_course_data_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_image_converter_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_image_converter.cpp")
    target_link_libraries(native_assets_image_converter_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_image_converter_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_image_converter_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_car_physics_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_car_physics.cpp")
    target_link_libraries(native_assets_car_physics_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_car_physics_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_car_physics_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_audio_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_audio.cpp")
    target_link_libraries(native_assets_audio_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_audio_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_audio_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_engine_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_engine.cpp")
    target_link_libraries(native_assets_engine_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_engine_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_engine_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_music_library_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_music_library.cpp")
    target_link_libraries(native_assets_music_library_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_music_library_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_music_library_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_music_seq_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_music_seq.cpp")
    target_link_libraries(native_assets_music_seq_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_music_seq_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_music_seq_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_tsq_sequencer_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_tsq_sequencer.cpp")
    target_link_libraries(native_assets_tsq_sequencer_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_tsq_sequencer_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_tsq_sequencer_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_bgm_player_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_bgm_player.cpp")
    target_link_libraries(native_assets_bgm_player_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_bgm_player_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_bgm_player_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_camera_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_camera.cpp")
    target_link_libraries(native_assets_camera_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_camera_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_camera_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_task_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_task.cpp")
    target_link_libraries(native_assets_task_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_task_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_task_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_dialogue_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_dialogue.cpp")
    target_link_libraries(native_assets_dialogue_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_dialogue_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_dialogue_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_save_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_save.cpp")
    target_link_libraries(native_assets_save_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_save_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_save_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_rumble_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_rumble.cpp")
    target_link_libraries(native_assets_rumble_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_rumble_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_rumble_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_progression_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_progression.cpp")
    target_link_libraries(native_assets_progression_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_progression_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_progression_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_collectibles_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_collectibles.cpp")
    target_link_libraries(native_assets_collectibles_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_collectibles_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_collectibles_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_shop_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_shop.cpp")
    target_link_libraries(native_assets_shop_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_shop_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_shop_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_memcard_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_memcard.cpp")
    target_link_libraries(native_assets_memcard_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_memcard_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_memcard_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_radio_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_radio.cpp")
    target_link_libraries(native_assets_radio_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_radio_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_radio_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_input_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_input.cpp")
    target_link_libraries(native_assets_input_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_input_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_input_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    add_executable(native_assets_trigger_map_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_trigger_map.cpp")
    target_link_libraries(native_assets_trigger_map_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_trigger_map_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_trigger_map_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    add_executable(native_assets_trigger_integration_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_trigger_integration.cpp")
    target_link_libraries(native_assets_trigger_integration_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_trigger_integration_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_trigger_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    add_executable(native_assets_menu_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_menu.cpp")
    target_link_libraries(native_assets_menu_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_menu_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_menu_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_ai_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_ai.cpp")
    target_link_libraries(native_assets_ai_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_ai_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_ai_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_world_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_world.cpp")
    target_link_libraries(native_assets_world_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_world_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_world_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_daynight_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_daynight.cpp")
    target_link_libraries(native_assets_daynight_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_daynight_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_daynight_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_daycycle_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_daycycle.cpp")
    target_link_libraries(native_assets_daycycle_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_daycycle_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_daycycle_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_race_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_race.cpp")
    target_link_libraries(native_assets_race_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_race_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_race_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_results_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_results.cpp")
    target_link_libraries(native_assets_results_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_results_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_results_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_session_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_session.cpp")
    target_link_libraries(native_assets_session_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_session_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_session_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_geometry_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_geometry.cpp")
    target_link_libraries(native_assets_geometry_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_geometry_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_geometry_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_minigames_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_minigames.cpp")
    target_link_libraries(native_assets_minigames_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_minigames_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_minigames_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_route_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_route.cpp")
    target_link_libraries(native_assets_route_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_route_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_route_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_motion_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_motion.cpp")
    target_link_libraries(native_assets_motion_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_motion_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_motion_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_tile_streamer_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_tile_streamer.cpp")
    target_link_libraries(native_assets_tile_streamer_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_tile_streamer_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_tile_streamer_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_game_loop_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_game_loop.cpp")
    target_link_libraries(native_assets_game_loop_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_game_loop_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_game_loop_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_interp_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_interp.cpp")
    target_link_libraries(native_assets_interp_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_interp_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_interp_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_perfstats_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_perfstats.cpp")
    target_link_libraries(native_assets_perfstats_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_perfstats_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_perfstats_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    # ---- map/second-screen logic wave (gameplay/map_view.cpp): own guarded
    # block so concurrent edits to the staged lists cannot collide.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_map_view.cpp")
        add_executable(native_assets_map_view_test
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_map_view.cpp")
        target_link_libraries(native_assets_map_view_test PRIVATE rt_native_assets)
        target_compile_features(native_assets_map_view_test PRIVATE cxx_std_20)
        target_compile_options(native_assets_map_view_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    endif()

    add_executable(native_assets_perf_glyphs_integration_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_perf_glyphs_integration.cpp")
    target_link_libraries(native_assets_perf_glyphs_integration_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_perf_glyphs_integration_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_perf_glyphs_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_setup_wizard_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_setup_wizard.cpp")
    target_link_libraries(native_assets_setup_wizard_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_setup_wizard_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_setup_wizard_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    # ---- staged test targets (coordinator-managed parallel dispatch; see the
    # staged-source comment above). Each registers once its source lands.
    foreach(_rt_staged course_props car_models glyph_atlas rules_tables hud_layout glyphs settings output_modes)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_${_rt_staged}.cpp")
            add_executable(native_assets_${_rt_staged}_test
                "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_${_rt_staged}.cpp")
            target_link_libraries(native_assets_${_rt_staged}_test PRIVATE rt_native_assets)
            target_compile_features(native_assets_${_rt_staged}_test PRIVATE cxx_std_20)
            target_compile_options(native_assets_${_rt_staged}_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
        endif()
    endforeach()
    # staged test target (hd_pack): same registration shape as the list above,
    # on its own block so concurrent work streams don't share an anchor line.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_hd_pack.cpp")
        add_executable(native_assets_hd_pack_test
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_hd_pack.cpp")
        target_link_libraries(native_assets_hd_pack_test PRIVATE rt_native_assets)
        target_compile_features(native_assets_hd_pack_test PRIVATE cxx_std_20)
        target_compile_options(native_assets_hd_pack_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/integration/test_packs_integration.cpp")
        add_executable(native_assets_packs_integration_test
            "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/integration/test_packs_integration.cpp")
        target_link_libraries(native_assets_packs_integration_test PRIVATE rt_native_assets)
        target_compile_features(native_assets_packs_integration_test PRIVATE cxx_std_20)
        target_compile_options(native_assets_packs_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    endif()

    # tests/native_assets/integration/test_*.cpp: end-to-end pipeline tests
    # over the user's own extracted disc files, located at run time through the
    # RT_DISC environment variable (default "~/Library/Application
    # Support/RoadTripRecomp/disc"); no game data ships with this tree. Same
    # executables-only pattern: no CTest registration, run them by hand.
    add_executable(native_assets_car_integration_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/integration/test_car_integration.cpp")
    target_link_libraries(native_assets_car_integration_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_car_integration_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_car_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_texture_integration_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/integration/test_texture_integration.cpp")
    target_link_libraries(native_assets_texture_integration_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_texture_integration_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_texture_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

    add_executable(native_assets_edges_integration_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/integration/test_edges_integration.cpp")
    target_link_libraries(native_assets_edges_integration_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_edges_integration_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_edges_integration_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endif()

# W9-01 standalone terrain evidence image; it writes a PNG from the user's
# extracted COURSE/C00.BIN at run time and never packages disc-derived bytes.
add_executable(rt-w24-terrain-preview
    "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w24/terrain_preview_main.cpp")
target_link_libraries(rt-w24-terrain-preview PRIVATE rt_native_assets)
target_compile_features(rt-w24-terrain-preview PRIVATE cxx_std_20)
target_compile_options(rt-w24-terrain-preview PRIVATE ${RT_NATIVE_WARN_OPTIONS})

# ---------------------------------------------------------------- coverage
# Opt-in clang source-based coverage instrumentation (-fprofile-instr-generate
# -fcoverage-mapping) for rt_native_assets and every test executable. OFF by
# default, so ordinary builds are untouched. Drive it e.g. with:
#   cmake -S src/native -B build/native-cov -DRT_NATIVE_ASSETS_BUILD_TESTS=ON \
#         -DRT_NATIVE_ASSETS_COVERAGE=ON
#   cmake --build build/native-cov
#   LLVM_PROFILE_FILE=build/native-cov/default.%m.profraw <run each test exe>
#   xcrun llvm-profdata merge -sparse build/native-cov/*.profraw -o cov.profdata
#   xcrun llvm-cov report build/native-cov/<test exe> -instr-profile=cov.profdata \
#         <extra -object <test exe> ...> <src/native/assets/*.cpp>
option(RT_NATIVE_ASSETS_COVERAGE
       "Instrument rt_native_assets and the test targets with clang coverage flags" OFF)
if(RT_NATIVE_ASSETS_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR
                "RT_NATIVE_ASSETS_COVERAGE requires a clang compiler "
                "(-fprofile-instr-generate/-fcoverage-mapping); got ${CMAKE_CXX_COMPILER_ID}")
    endif()
    set(RT_NATIVE_COVERAGE_FLAGS -fprofile-instr-generate -fcoverage-mapping)
    set(RT_NATIVE_COVERAGE_TARGETS rt_native_assets)
    if(RT_NATIVE_ASSETS_BUILD_TESTS)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS
            native_assets_container_test
            native_assets_loader_test
            native_assets_vif_chain_test
            native_assets_model_converter_test
            native_assets_field_converter_test
            native_assets_course_data_test
            native_assets_image_converter_test
            native_assets_car_integration_test
            native_assets_texture_integration_test
            native_assets_edges_integration_test
            native_assets_audio_test
            native_assets_dialogue_test
            native_assets_menu_test
            native_assets_input_test
            native_assets_world_test
            native_assets_game_loop_test
            native_assets_engine_test
            native_assets_music_library_test
            native_assets_music_seq_test
            native_assets_tsq_sequencer_test
            native_assets_save_test
            native_assets_progression_test
            native_assets_collectibles_test
            native_assets_shop_test
            native_assets_memcard_test
            native_assets_daynight_test
            native_assets_daycycle_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_race_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_results_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_session_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_geometry_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_minigames_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_route_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_tile_streamer_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_nav_routes_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_nav_path_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_nav_route_bridge_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_nav_walkable_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_nav_drive_test)
        list(APPEND RT_NATIVE_COVERAGE_TARGETS native_assets_interp_test)
    endif()
    foreach(rt_native_cov_target IN LISTS RT_NATIVE_COVERAGE_TARGETS)
        target_compile_options(${rt_native_cov_target} PRIVATE ${RT_NATIVE_COVERAGE_FLAGS})
        target_link_options(${rt_native_cov_target} PRIVATE ${RT_NATIVE_COVERAGE_FLAGS})
    endforeach()
    # The instrumented static library references the clang profiling runtime,
    # so every consumer needs -fprofile-instr-generate at link time. The dev
    # tools do not instrument their own code (no compile flags), but they must
    # still link against the instrumented rt_native_assets.
    foreach(rt_native_cov_tool rt-asset-convert rt-asset-inspect)
        target_link_options(${rt_native_cov_tool} PRIVATE -fprofile-instr-generate)
    endforeach()
    message(STATUS "rt_native_assets: coverage instrumentation ON for: ${RT_NATIVE_COVERAGE_TARGETS}")
endif()

# W3-05: one isolated registration block; no changes to other workers' targets.
target_sources(rt_native_assets PRIVATE gameplay/event_bridge.cpp)
if(RT_NATIVE_ASSETS_BUILD_TESTS)
    add_executable(w3_script_bridge_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_w2/event_bridge_test.cpp")
    target_link_libraries(w3_script_bridge_test PRIVATE rt_native_assets)
    target_compile_features(w3_script_bridge_test PRIVATE cxx_std_20)
    target_compile_options(w3_script_bridge_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endif()
