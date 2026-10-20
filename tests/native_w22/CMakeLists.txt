add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
add_executable(w22_autopilot_test
    autopilot_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native/app/Autopilot.cpp)
target_include_directories(w22_autopilot_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w22_autopilot_test PRIVATE cxx_std_20)
target_compile_options(w22_autopilot_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
