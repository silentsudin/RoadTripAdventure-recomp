cmake_minimum_required(VERSION 3.21)

# The upscaler plugins (Windows): FSR 3, DLSS and XeSS behind the C interface in
# ps2xRuntime/include/runtime/gs/rt_upscaler_api.h. Built apart from the app, with the same
# llvm-mingw toolchain (cmake/toolchains/llvm-mingw.cmake); each plugin goes beside RoadTrip.exe.
#
#   cmake -S plugins/upscalers -B build/plugins -G Ninja -DCMAKE_BUILD_TYPE=Release \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/llvm-mingw.cmake -DLLVM_MINGW_ROOT=<llvm-mingw> \
#         -DFFX_SDK_DIR=<FidelityFX-SDK v1.1.4 checkout>
#
# scripts/fetch_upscaler_sdks.py downloads the pinned SDKs into build/sdks and prints these options.
project(RoadTripUpscalerPlugins C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(RT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(RT_RUNTIME_INCLUDE "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/include")
set(RT_VULKAN_HEADERS "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs/Granite/third_party/khronos/vulkan-headers/include")

set(FFX_SDK_DIR "" CACHE PATH "AMD FidelityFX SDK v1.1.x checkout (ffx-api/, PrebuiltSignedDLL/)")

if(FFX_SDK_DIR)
    # FSR 3 (MIT): the ffx-api headers, and AMD's signed amd_fidelityfx_vk.dll loaded at run time.
    add_library(rt_fsr3 SHARED fsr3/rt_fsr3.cpp)
    set_target_properties(rt_fsr3 PROPERTIES PREFIX "")
    target_include_directories(rt_fsr3 PRIVATE "${RT_RUNTIME_INCLUDE}" "${RT_VULKAN_HEADERS}" "${FFX_SDK_DIR}/ffx-api/include")
    target_compile_definitions(rt_fsr3 PRIVATE NOGDI NOUSER)
    add_custom_command(TARGET rt_fsr3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FFX_SDK_DIR}/PrebuiltSignedDLL/amd_fidelityfx_vk.dll" "$<TARGET_FILE_DIR:rt_fsr3>"
        VERBATIM)
else()
    message(STATUS "FFX_SDK_DIR not set: rt_fsr3 is not built")
endif()
