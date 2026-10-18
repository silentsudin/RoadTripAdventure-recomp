# W6-04: deterministic native session driveline exposure.
add_executable(w18_driveline_test driveline_test.cpp)
target_link_libraries(w18_driveline_test PRIVATE rt_native_assets)
target_compile_features(w18_driveline_test PRIVATE cxx_std_20)
target_compile_options(w18_driveline_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
