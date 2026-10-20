add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# W8-03: collision grid road-ribbon sizing contract.
add_executable(w23_collision_sizing_test
    collision_sizing_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native/assets/nav_routes.cpp)
target_include_directories(w23_collision_sizing_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w23_collision_sizing_test PRIVATE cxx_std_20)
target_compile_options(w23_collision_sizing_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
