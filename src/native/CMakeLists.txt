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
    gameplay/nav_path.cpp       # deterministic route-graph centerline traversal
    gameplay/nav_walkable.cpp   # fixed-step centerline waypoint traversal diagnostics
    gameplay/nav_route_bridge.cpp # NavPath centerline -> authored RaceRoute zones
    assets/image_converter.cpp  # texture upload chain -> RGBA8 ImageRecord
    assets/image_loader.cpp     # ImageRecord -> NativeImage
    assets/model_loader.cpp     # MeshRecord + images -> NativeModel
    assets/music_seq.cpp        # Road Trip TVB voice-bank and TSQ sequence container parsers
    assets/tsq_sequencer.cpp    # bounded TSQ event decoder and offline TVB PCM renderer
    gameplay/car_physics.cpp    # authored car-physics tick (no guest code)
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
    gameplay/tile_streamer.cpp  # hardware-style 4-slot field-tile streaming algebra (slot = id & 3)
    audio/vag.cpp               # authored PS ADPCM (VAG) decoder (no PS2/guest code)
    audio/vag_loader.cpp        # authored VAG file/raw-stream loader -> PCM source storage
    audio/mixer.cpp             # authored mixer model: N channels -> stereo out, no device
    audio/engine.cpp            # authored sound engine: voices + BGM/SE + mixer driver (replaces SNDMOD RPC)
    audio/music_library.cpp     # injected BGM file-table index -> VAG resolver + SoundEngine bridge
    audio/bgm_player.cpp        # bounded TSQ+TVB BGM pre-render and stereo PCM loop player
    gameplay/save.cpp           # authored memory-card save parse/serialize/migrate (13,384-byte progress block)
    gameplay/progression.cpp    # authored progression mutations over the documented save fields
    gameplay/collectibles.cpp   # authored stamp and coin pickup model
    gameplay/shop.cpp           # authored item-shop buy/sell and selection model
    gameplay/memcard.cpp        # host-file replacement for two memory-card save slots
    gameplay/menu.cpp           # authored menu, pause, and controller-config state models
    gameplay/input.cpp          # authored player input (pad/analog -> CarInput)
    gameplay/game_loop.cpp      # authored simulation frame (task walker -> AI/car/camera -> cull -> snapshot)
)
add_library(rt_native_assets STATIC ${RT_NATIVE_ASSET_SOURCES})
# PUBLIC so consumers #include "assets/container.h" against this include root;
# cxx_std_20 PUBLIC because the headers themselves require C++20 (std::span).
target_include_directories(rt_native_assets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(rt_native_assets PUBLIC cxx_std_20)
target_compile_options(rt_native_assets PRIVATE ${RT_NATIVE_WARN_OPTIONS})

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

    # First interactive host increment: the scene is authored by preview.cpp
    # and copied to an SDL3 Vulkan swapchain for presentation.
    if(SDL3_FOUND)
        add_executable(rt-app app/main.cpp preview/preview.cpp)
        target_link_libraries(rt-app PRIVATE rt_native_assets Vulkan::Vulkan SDL3::SDL3)
        target_compile_features(rt-app PRIVATE cxx_std_20)
        target_compile_options(rt-app PRIVATE ${RT_NATIVE_WARN_OPTIONS} -Wno-missing-field-initializers)
    endif()
endif()

# ---------------------------------------------------------------- tests (not run)
option(RT_NATIVE_ASSETS_BUILD_TESTS
       "Build the synthetic native-asset contract tests as executables (not run)" OFF)
if(RT_NATIVE_ASSETS_BUILD_TESTS)
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

    add_executable(native_assets_field_converter_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_field_converter.cpp")
    target_link_libraries(native_assets_field_converter_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_field_converter_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_field_converter_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})

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

    add_executable(native_assets_input_test
        "${CMAKE_CURRENT_SOURCE_DIR}/../../tests/native_assets/test_input.cpp")
    target_link_libraries(native_assets_input_test PRIVATE rt_native_assets)
    target_compile_features(native_assets_input_test PRIVATE cxx_std_20)
    target_compile_options(native_assets_input_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
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
