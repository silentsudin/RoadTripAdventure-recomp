# W6-03: gear glyph semantics + mph fixed-point pins (ROM-backed, NO_ELF skips).
add_executable(w17_gear_glyph_test gear_glyph_test.cpp)
target_include_directories(w17_gear_glyph_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w17_gear_glyph_test PRIVATE cxx_std_20)
target_compile_options(w17_gear_glyph_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
