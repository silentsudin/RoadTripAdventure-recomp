# W4-08 script_ops contract test.  Self-contained: compiles gameplay/script_ops.cpp straight
# into the test (no rt_native_assets link), so it also works with a bare
#   cmake -S tests/native_w8 -B build/native-w408-t && cmake --build build/native-w408-t
cmake_minimum_required(VERSION 3.21)
if(NOT PROJECT_NAME)
    project(rt_native_w8 LANGUAGES CXX)
endif()

set(_rt_w8_native "${CMAKE_CURRENT_LIST_DIR}/../../src/native")
add_executable(w8_script_ops_test
    script_ops_test.cpp
    "${_rt_w8_native}/gameplay/script_ops.cpp")
target_include_directories(w8_script_ops_test PRIVATE "${_rt_w8_native}")
target_compile_features(w8_script_ops_test PRIVATE cxx_std_20)
if(MSVC)
    target_compile_options(w8_script_ops_test PRIVATE /W4)
else()
    target_compile_options(w8_script_ops_test PRIVATE -Wall -Wextra)
endif()
