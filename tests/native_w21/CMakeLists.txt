# W8-04: CPU preview rotated textured quad and tachometer needle draw.
add_executable(w21_rot_quad_test rot_quad_test.cpp)
target_include_directories(w21_rot_quad_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w21_rot_quad_test PRIVATE cxx_std_20)
target_compile_options(w21_rot_quad_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
