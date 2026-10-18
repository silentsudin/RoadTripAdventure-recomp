# tests/native_w11: W4-11 gauge needle ROM-law contract.
file(GLOB RT_W11_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*_test.cpp")
foreach(_rt_w11_test_source IN LISTS RT_W11_TEST_SOURCES)
    get_filename_component(_rt_w11_test_we "${_rt_w11_test_source}" NAME_WE)
    add_executable(w11_${_rt_w11_test_we} "${_rt_w11_test_source}")
    target_link_libraries(w11_${_rt_w11_test_we} PRIVATE rt_native_assets)
    target_compile_features(w11_${_rt_w11_test_we} PRIVATE cxx_std_20)
    target_compile_options(w11_${_rt_w11_test_we} PRIVATE ${RT_NATIVE_WARN_OPTIONS})
endforeach()
