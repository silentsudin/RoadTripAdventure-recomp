add_compile_options(-UNDEBUG)  # W13: Release gates must not compile test asserts out
# W7-01: tachometer needle draw spec (header-only, pure math).
add_executable(w19_gauge_needle_test gauge_needle_test.cpp)
target_link_libraries(w19_gauge_needle_test PRIVATE rt_native_assets)
target_compile_features(w19_gauge_needle_test PRIVATE cxx_std_20)
target_compile_options(w19_gauge_needle_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
