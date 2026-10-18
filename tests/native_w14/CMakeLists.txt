# W5-03: ROM-backed race HUD state-input pins.
add_executable(w14_race_state_probe_test race_state_probe_test.cpp)
target_link_libraries(w14_race_state_probe_test PRIVATE rt_native_assets)
target_compile_features(w14_race_state_probe_test PRIVATE cxx_std_20)
target_compile_options(w14_race_state_probe_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
