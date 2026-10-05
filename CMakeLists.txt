cmake_minimum_required(VERSION 3.21)

project(RoadTripRecomp VERSION 0.1.0 LANGUAGES C CXX)

if(APPLE)
    enable_language(OBJC OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(ANDROID)
    # Everything ends up in libmain.so.
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

option(RT_ENABLE_DEBUG_UI "Include the PS2Recomp ImGui debug panel" ON)

# ---------------------------------------------------------------- PS2Recomp runtime + recompiler
# The app ships the runtime and the recompiler, never game code: the game is recompiled and
# compiled on the user's machine from their own disc on first launch (see src/game/GameBuilder.cpp).
set(PS2X_BUILD_RECOMP ON CACHE BOOL "" FORCE)
set(PS2X_BUILD_ANALYZER OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_STUDIO OFF CACHE BOOL "" FORCE)
set(PS2X_BUILD_RUNTIME ON CACHE BOOL "" FORCE)
set(PS2X_ENABLE_FFMPEG OFF CACHE BOOL "Road Trip has no MPEG movies we rely on yet")
set(PS2X_ENABLE_DEBUG_UI ${RT_ENABLE_DEBUG_UI} CACHE BOOL "" FORCE)
# Upstream defaults to per-function entry logging and per-register GS logging, which costs a lot
# of speed. Off by default; turn back on with -DRT_VERBOSE_RUNTIME_LOGS=ON when debugging.
option(RT_VERBOSE_RUNTIME_LOGS "Enable PS2Recomp aggressive logs + function log tracker" OFF)
set(PS2X_ENABLE_AGRESSIVE_LOGS ${RT_VERBOSE_RUNTIME_LOGS} CACHE BOOL "" FORCE)
# SDL3 is the platform layer (window, events, controllers and rumble) under raylib.
set(PS2X_HOST_PLATFORM "SDL3" CACHE STRING "" FORCE)
# Keep the bundled ps2_recomp free of Homebrew dylibs (libdwarf would pick up zstd).
set(ENABLE_DECOMPRESSION OFF CACHE BOOL "" FORCE)
# Android recompiles the game on the device too: the recompiler is linked into the app.
set(PS2X_ANDROID_RECOMP ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/PS2Recomp EXCLUDE_FROM_ALL)

set(PS2X_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/PS2Recomp/ps2xRuntime")

# Every Apple Silicon Mac supports the M1 feature set; let the hot interpreter/rasterizer use it.
if(APPLE AND CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
    foreach(_rt_target ps2_runtime ps2_iop raylib)
        target_compile_options(${_rt_target} PRIVATE -mcpu=apple-m1)
    endforeach()
endif()

# ---------------------------------------------------------------- game SDK probe
# Compiles one file with exactly the settings the recompiled game needs. Its entry in
# compile_commands.json is turned into Resources/sdk/flags.json by scripts/bundle_sdk.py,
# so the on-device build matches the runtime ABI the app was built with.
# sse2neon is wired up with directory-scoped include_directories() upstream; mirror it for our targets.
add_library(rt_sse2neon INTERFACE)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
    include(FetchContent)
    FetchContent_GetProperties(sse2neon)
    target_include_directories(rt_sse2neon INTERFACE "${sse2neon_SOURCE_DIR}")
    target_compile_definitions(rt_sse2neon INTERFACE USE_SSE2NEON)
endif()

add_library(rt_game_sdk_probe OBJECT src/game/sdk_probe.cpp)
target_link_libraries(rt_game_sdk_probe PRIVATE ps2_runtime rt_sse2neon)

# ---------------------------------------------------------------- libchdr
# CHD disc images (the compressed format emulator users keep their games in; BSD-3, with LZMA,
# zstd and miniz inside). Static only: BUILD_SHARED_LIBS stays as it is for everything else.
include(FetchContent)
set(_rt_shared_libs "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)
set(CHDR_WANT_TESTS OFF CACHE BOOL "" FORCE)
set(INSTALL_STATIC_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    libchdr
    GIT_REPOSITORY "https://github.com/rtissera/libchdr.git"
    GIT_TAG "607694ca0812edfc9cc2030c64634fc2393668de"
)
FetchContent_MakeAvailable(libchdr)
set(BUILD_SHARED_LIBS "${_rt_shared_libs}")
# Its licence and those of the decompressors inside it (theirs sit in their sources' headers).
file(READ "${libchdr_SOURCE_DIR}/LICENSE.txt" _rt_chdr_licence)
file(GLOB _rt_lzma_licence_file "${libchdr_SOURCE_DIR}/deps/lzma-*/LICENSE")
file(READ "${_rt_lzma_licence_file}" _rt_lzma_licence)
file(GLOB _rt_miniz_c "${libchdr_SOURCE_DIR}/deps/miniz-*/miniz.c")
file(STRINGS "${_rt_miniz_c}" _rt_miniz_head LIMIT_COUNT 26)
string(JOIN "\n" _rt_miniz_head ${_rt_miniz_head})
file(GLOB _rt_zstd_h "${libchdr_SOURCE_DIR}/deps/zstd-*/zstd.h")
file(STRINGS "${_rt_zstd_h}" _rt_zstd_head LIMIT_COUNT 10)
string(JOIN "\n" _rt_zstd_head ${_rt_zstd_head})
set(RT_CHDR_LICENCES "${CMAKE_CURRENT_BINARY_DIR}/libchdr-LICENSES.txt")
file(WRITE "${RT_CHDR_LICENCES}"
     "libchdr (https://github.com/rtissera/libchdr)\n\n${_rt_chdr_licence}\n\n"
     "LZMA SDK (inside libchdr)\n\n${_rt_lzma_licence}\n\n"
     "miniz (inside libchdr)\n\n${_rt_miniz_head}\n\n"
     "Zstandard (inside libchdr; BSD licence)\n\n${_rt_zstd_head}\n")

# ---------------------------------------------------------------- app
set(RT_APP_SOURCES
    src/main.cpp
    src/debug/FpsOverlay.cpp
    src/debug/FrameDump.cpp
    src/debug/RamDump.cpp
    src/debug/ThreadDump.cpp
    src/game/GameBuilder.cpp
    src/game/overrides.cpp
    src/platform/Paths.cpp
    src/platform/input/Config.cpp
    src/platform/input/Devices.cpp
    src/platform/input/Input.cpp
    src/platform/Host.cpp
    src/platform/input/Mapping.cpp
    src/platform/input/Players.cpp
    src/platform/input/Types.cpp
    src/rom/IsoReader.cpp
    src/rom/RomInstaller.cpp
    src/rom/Sha1.cpp
    src/settings/Apply.cpp
    src/settings/Capabilities.cpp
    src/settings/Settings.cpp
)
if(APPLE)
    list(APPEND RT_APP_SOURCES src/platform/macos/Dialogs.mm)
elseif(ANDROID)
    list(APPEND RT_APP_SOURCES
        src/platform/android/Assets.cpp
        src/platform/android/Dialogs.cpp
        src/platform/android/LogRedirect.cpp)
endif()

# The empty, full-range function table; the game library fills it in when loaded.
list(APPEND RT_APP_SOURCES "${PS2X_RUNTIME_DIR}/src/runner/register_functions.cpp")

if(ANDROID)
    # libmain.so, loaded by SDLActivity. The game library is linked against it (DT_NEEDED), so
    # every runtime symbol must be in it and exported; the recompilers run in-process.
    set(RT_APP_TARGET main)
    add_library(main SHARED ${RT_APP_SOURCES})
    target_link_libraries(main PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,ps2_runtime>" "$<LINK_LIBRARY:WHOLE_ARCHIVE,ps2_iop>"
        ps2_recomp_lib ps2_vu1_recomp_lib android log GLESv3)
    add_dependencies(main rt_game_sdk_probe)
else()
    set(RT_APP_TARGET RoadTrip)
    add_executable(RoadTrip MACOSX_BUNDLE ${RT_APP_SOURCES})
    add_dependencies(RoadTrip ps2_recomp rt_game_sdk_probe)
    # The game library is linked with -undefined dynamic_lookup and resolves the runtime from the
    # executable, so every runtime symbol must be present and exported.
    target_link_options(RoadTrip PRIVATE
        "LINKER:-export_dynamic"
        "LINKER:-force_load,$<TARGET_FILE:ps2_runtime>"
        "LINKER:-force_load,$<TARGET_FILE:ps2_iop>")
endif()
target_include_directories(${RT_APP_TARGET} PRIVATE src)
target_link_libraries(${RT_APP_TARGET} PRIVATE ps2_runtime rt_sse2neon SDL3::SDL3 toml11::toml11 chdr-static)

if(RT_ENABLE_DEBUG_UI)
    target_sources(${RT_APP_TARGET} PRIVATE "${PS2X_RUNTIME_DIR}/src/lib/ps2_debug_panel.cpp"
                                    src/platform/input/Controllers.cpp
                                    src/ui/PauseMenu.cpp src/ui/SetupScreen.cpp src/ui/Theme.cpp)
    target_link_libraries(${RT_APP_TARGET} PRIVATE rlImGui imgui)
    target_compile_definitions(${RT_APP_TARGET} PRIVATE PS2X_ENABLE_DEBUG_UI=1)
endif()

if(APPLE)
    target_link_libraries(RoadTrip PRIVATE "-framework AppKit" "-framework UniformTypeIdentifiers")
    set_target_properties(RoadTrip PROPERTIES
        OUTPUT_NAME "RoadTrip"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/platform/macos/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "Road Trip"
        MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.roadtrip-recomp"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
if(NOT ANDROID)
# Always re-run (cheap): picks up a rebuilt ps2_recomp, header or config change even when the
# app executable itself didn't relink.
add_custom_target(rt_bundle ALL
    COMMAND Python3::Interpreter "${CMAKE_CURRENT_SOURCE_DIR}/scripts/bundle_sdk.py"
            --compile-commands "${CMAKE_BINARY_DIR}/compile_commands.json"
            --probe "${CMAKE_CURRENT_SOURCE_DIR}/src/game/sdk_probe.cpp"
            --shim "${CMAKE_CURRENT_SOURCE_DIR}/src/game/game_shim.cpp"
            --recomp "$<TARGET_FILE:ps2_recomp>"
            --config "${CMAKE_CURRENT_SOURCE_DIR}/config/roadtrip.toml"
            --vu1recomp "$<TARGET_FILE:ps2_vu1_recomp>"
            --resources "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources"
    COMMENT "Bundling recompiler + game SDK (no game code) into RoadTrip.app"
    VERBATIM)
add_dependencies(rt_bundle RoadTrip ps2_recomp ps2_vu1_recomp rt_game_sdk_probe)
# The UI's font (Fredoka, SIL Open Font License; licence alongside).
add_custom_command(TARGET rt_bundle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/Fredoka-SemiBold.ttf"
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/Fredoka-Bold.ttf"
            "${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts/OFL.txt"
            "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources/fonts"
    # Every third-party component's licence (scripts/collect_licenses.py fails on a missing one).
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/collect_licenses.py" "${CMAKE_BINARY_DIR}"
            "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources/licenses"
    VERBATIM)
endif()

# Bundle MoltenVK (Vulkan on Metal) for the paraLLEl-GS GPU backend, so players need neither the
# Vulkan SDK nor Homebrew. The app points Granite at Contents/Frameworks/libMoltenVK.dylib.
if(APPLE AND PS2X_ENABLE_PGS)
    find_library(RT_MOLTENVK_LIBRARY NAMES MoltenVK
                 HINTS /opt/homebrew/opt/molten-vk/lib /usr/local/opt/molten-vk/lib ENV VULKAN_SDK
                 PATH_SUFFIXES lib)
    if(NOT RT_MOLTENVK_LIBRARY)
        message(FATAL_ERROR "MoltenVK not found: `brew install molten-vk` (or set VULKAN_SDK), "
                            "or configure with -DPS2X_ENABLE_PGS=OFF for the CPU GS only.")
    endif()
    get_filename_component(_rt_mvk_prefix "${RT_MOLTENVK_LIBRARY}" DIRECTORY)
    get_filename_component(_rt_mvk_prefix "${_rt_mvk_prefix}" DIRECTORY)
    message(STATUS "Bundling MoltenVK: ${RT_MOLTENVK_LIBRARY}")
    add_custom_command(TARGET rt_bundle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Frameworks"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RT_MOLTENVK_LIBRARY}"
                "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Frameworks/libMoltenVK.dylib"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources/licenses"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_rt_mvk_prefix}/LICENSE"
                "$<TARGET_BUNDLE_CONTENT_DIR:RoadTrip>/Resources/licenses/MoltenVK-LICENSE.txt"
        COMMENT "Bundling MoltenVK"
        VERBATIM)
endif()

# ---------------------------------------------------------------- tests
include(CTest)
if(BUILD_TESTING AND NOT ANDROID)
    add_executable(iso_reader_test tests/iso_reader_test.cpp src/rom/IsoReader.cpp src/rom/Sha1.cpp)
    target_include_directories(iso_reader_test PRIVATE src)
    target_link_libraries(iso_reader_test PRIVATE chdr-static)
    add_test(NAME iso_reader COMMAND iso_reader_test)
    set_tests_properties(iso_reader PROPERTIES SKIP_RETURN_CODE 77)

    add_executable(input_test tests/input_test.cpp
        src/platform/input/Config.cpp src/platform/input/Devices.cpp src/platform/input/Mapping.cpp
        src/platform/input/Players.cpp src/platform/input/Types.cpp)
    target_include_directories(input_test PRIVATE src)
    target_link_libraries(input_test PRIVATE SDL3::SDL3 toml11::toml11)
    add_test(NAME input COMMAND input_test)

    add_executable(settings_test tests/settings_test.cpp src/settings/Settings.cpp src/settings/Capabilities.cpp
        src/platform/Paths.cpp)
    target_include_directories(settings_test PRIVATE src)
    target_link_libraries(settings_test PRIVATE toml11::toml11)
    add_test(NAME settings COMMAND settings_test)
    add_executable(wide_layout_test tests/wide_layout_test.cpp
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/PS2Recomp/ps2xRuntime/src/lib/gs/gs_wide_layout.cpp")
    target_include_directories(wide_layout_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/PS2Recomp/ps2xRuntime/include")
    add_test(NAME wide_layout COMMAND wide_layout_test)
endif()

if(NOT ANDROID)
# Every texture on the disc as PNG, named like texture dumps (tools/texture_extract.cpp). Reads the
# user's extracted disc at run time; contains no game data.
add_executable(rt_texture_extract tools/texture_extract.cpp src/textures/Extract.cpp)
target_include_directories(rt_texture_extract PRIVATE src)
target_link_libraries(rt_texture_extract PRIVATE ps2_runtime rt_sse2neon)

# The disc's VAG streams as WAV (tools/audio_extract.cpp). Reads the user's disc at run time.
add_executable(rt_audio_extract tools/audio_extract.cpp)
endif()
