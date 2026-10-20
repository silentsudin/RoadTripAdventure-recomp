add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# W7-03: town collision grid -> car-physics integration contract.
add_executable(w20_collision_grid_test
    collision_grid_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native/assets/nav_routes.cpp)
target_include_directories(w20_collision_grid_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w20_collision_grid_test PRIVATE cxx_std_20)
target_compile_options(w20_collision_grid_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
