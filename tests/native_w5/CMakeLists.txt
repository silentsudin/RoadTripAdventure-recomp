add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# tests/native_w5: wave-5 worker tests (W4-05 dashcam source first).
# Registered by a single add_subdirectory line in src/native/CMakeLists.txt,
# placed right after rt_native_assets is defined so Android builds also get
# the media sources. Tests are host-only executables (no CTest, run by hand
# from the repo root); ROM/footage/ffmpeg absence prints SKIP and exits 0.

target_sources(rt_native_assets PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../../src/native/media/dashcam_source.cpp")  # W4-05 dashcam footage source

if(RT_NATIVE_ASSETS_BUILD_TESTS AND NOT ANDROID)
    # tests/native_w5/<stem>_test.cpp -> executable w5_<stem>_test.
    file(GLOB RT_W5_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*_test.cpp")
    foreach(_rt_w5_test_source IN LISTS RT_W5_TEST_SOURCES)
        get_filename_component(_rt_w5_test_we "${_rt_w5_test_source}" NAME_WE)
        add_executable(w5_${_rt_w5_test_we} "${_rt_w5_test_source}")
        target_link_libraries(w5_${_rt_w5_test_we} PRIVATE rt_native_assets)
        target_compile_features(w5_${_rt_w5_test_we} PRIVATE cxx_std_20)
        target_compile_options(w5_${_rt_w5_test_we} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    endforeach()
endif()
