add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# tests/native_w4: wave-4 worker tests (W4-06 race HUD layout first).
# Registered by a single add_subdirectory line inside the RT_NATIVE_ASSETS_BUILD_TESTS
# block of src/native/CMakeLists.txt (same convention as tests/native_w8).
# tests/native_w4/<stem>_test.cpp -> executable w4_<stem>_test. Executables only (no
# CTest); run them from the repo root. A missing ROM or oracle golden prints SKIP and
# exits 0. zlib is optional and only enables the PNG re-measurement tier.

find_package(ZLIB QUIET)

file(GLOB RT_W4_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*_test.cpp")
foreach(_rt_w4_test_source IN LISTS RT_W4_TEST_SOURCES)
    get_filename_component(_rt_w4_test_we "${_rt_w4_test_source}" NAME_WE)
    add_executable(w4_${_rt_w4_test_we} "${_rt_w4_test_source}")
    target_link_libraries(w4_${_rt_w4_test_we} PRIVATE rt_native_assets)
    target_compile_features(w4_${_rt_w4_test_we} PRIVATE cxx_std_20)
    target_compile_options(w4_${_rt_w4_test_we} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
    target_compile_definitions(w4_${_rt_w4_test_we} PRIVATE
        RT_W4_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
        RT_W4_REPO_DIR="${CMAKE_CURRENT_LIST_DIR}/../..")
    if(ZLIB_FOUND)
        target_link_libraries(w4_${_rt_w4_test_we} PRIVATE ZLIB::ZLIB)
        target_compile_definitions(w4_${_rt_w4_test_we} PRIVATE RT_W4_HAVE_ZLIB=1)
    endif()
endforeach()
