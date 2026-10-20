add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# tests/native_w13: W5-01 RPM/driveline ROM-law contract.
file(GLOB RT_W13_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*_test.cpp")
foreach(_rt_w13_test_source IN LISTS RT_W13_TEST_SOURCES)
    get_filename_component(_rt_w13_test_we "${_rt_w13_test_source}" NAME_WE)
    add_executable(w13_${_rt_w13_test_we} "${_rt_w13_test_source}")
    target_link_libraries(w13_${_rt_w13_test_we} PRIVATE rt_native_assets)
    target_compile_features(w13_${_rt_w13_test_we} PRIVATE cxx_std_20)
    target_compile_options(w13_${_rt_w13_test_we} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endforeach()
