cmake_minimum_required(VERSION 3.21)

# The DLSS / XeSS add-on: rt_vendor_upscalers.dll. Built with MSVC (NVIDIA's NGX is a static MSVC
# library; Intel's XeSS an import library) and the static CRT, so it needs neither the app's libc++
# nor a redistributable. Neither SDK may be linked into the GPL app: this DLL is the separate piece,
# loaded by the app through runtime/gs/rt_upscaler_api.h when it sits beside RoadTrip.exe.
#
#   (in a VS developer prompt, or after vcvars64.bat)
#   cmake -S plugins/upscalers/vendor -B build/plugins-vendor -G Ninja -DCMAKE_BUILD_TYPE=Release \
#         -DDLSS_SDK_DIR=<NVIDIA/DLSS checkout> -DXESS_SDK_DIR=<XeSS SDK unzipped>
project(RoadTripVendorUpscalers CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded") # /MT, as nvsdk_ngx_s.lib

get_filename_component(RT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(RT_RUNTIME_INCLUDE "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/include")
set(RT_VULKAN_HEADERS "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs/Granite/third_party/khronos/vulkan-headers/include")

set(DLSS_SDK_DIR "" CACHE PATH "NVIDIA DLSS SDK (github.com/NVIDIA/DLSS: include/, lib/Windows_x86_64/)")
set(XESS_SDK_DIR "" CACHE PATH "Intel XeSS SDK (inc/, lib/, bin/)")

add_library(rt_vendor_upscalers SHARED rt_vendor_upscalers.cpp)
set_target_properties(rt_vendor_upscalers PROPERTIES PREFIX "")
target_include_directories(rt_vendor_upscalers PRIVATE "${RT_RUNTIME_INCLUDE}" "${RT_VULKAN_HEADERS}")
target_compile_options(rt_vendor_upscalers PRIVATE /W3 /utf-8)

if(DLSS_SDK_DIR)
    target_compile_definitions(rt_vendor_upscalers PRIVATE RT_HAVE_DLSS)
    target_include_directories(rt_vendor_upscalers PRIVATE "${DLSS_SDK_DIR}/include")
    target_link_libraries(rt_vendor_upscalers PRIVATE "${DLSS_SDK_DIR}/lib/Windows_x86_64/x64/nvsdk_ngx_s.lib")
    # The release build of the DLSS feature library (the NGX runtime finds it beside this DLL).
    add_custom_command(TARGET rt_vendor_upscalers POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLSS_SDK_DIR}/lib/Windows_x86_64/rel/nvngx_dlss.dll" "$<TARGET_FILE_DIR:rt_vendor_upscalers>"
        VERBATIM)
endif()
if(XESS_SDK_DIR)
    target_compile_definitions(rt_vendor_upscalers PRIVATE RT_HAVE_XESS)
    target_include_directories(rt_vendor_upscalers PRIVATE "${XESS_SDK_DIR}/inc")
    target_link_libraries(rt_vendor_upscalers PRIVATE "${XESS_SDK_DIR}/lib/libxess.lib")
    add_custom_command(TARGET rt_vendor_upscalers POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${XESS_SDK_DIR}/bin/libxess.dll" "$<TARGET_FILE_DIR:rt_vendor_upscalers>"
        VERBATIM)
endif()
if(NOT DLSS_SDK_DIR AND NOT XESS_SDK_DIR)
    message(WARNING "Neither DLSS_SDK_DIR nor XESS_SDK_DIR is set: the plugin would offer nothing")
endif()
