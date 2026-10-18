# W6-02: title save/load UI state machine.
add_executable(w16_title_ui_test
    title_ui_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native/preview/title_ui.cpp)
target_include_directories(w16_title_ui_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/native)
target_compile_features(w16_title_ui_test PRIVATE cxx_std_20)
target_compile_options(w16_title_ui_test PRIVATE ${RT_NATIVE_WARN_OPTIONS})
