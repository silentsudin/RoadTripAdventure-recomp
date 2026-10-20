add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# W4-12: ROM-backed HUD GSL page decoder contract.
add_executable(w12_hud_pages_test hud_pages_test.cpp)
target_link_libraries(w12_hud_pages_test PRIVATE rt_native_assets)
target_compile_features(w12_hud_pages_test PRIVATE cxx_std_20)
target_compile_options(w12_hud_pages_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
