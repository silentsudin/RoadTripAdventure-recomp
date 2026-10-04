# Minimal wrapper: builds only paraLLEl-GS/Granite's slangmosh shader compiler.
cmake_minimum_required(VERSION 3.18)
project(slangmosh-tool LANGUAGES CXX C)

set(RT_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
set(PGS_DIR "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/third_party/parallel-gs")

set(GRANITE_PLATFORM "null" CACHE STRING "" FORCE)
set(GRANITE_RENDERER OFF CACHE BOOL "" FORCE)
set(GRANITE_VULKAN_SYSTEM_HANDLES OFF CACHE BOOL "" FORCE)
set(GRANITE_VULKAN_FOSSILIZE OFF CACHE BOOL "" FORCE)
set(GRANITE_VULKAN_SPIRV_CROSS ON CACHE BOOL "" FORCE)
set(GRANITE_VULKAN_SHADER_MANAGER_RUNTIME_COMPILER ON CACHE BOOL "" FORCE)
set(GRANITE_SHADER_COMPILER_OPTIMIZE ON CACHE BOOL "" FORCE)
set(GRANITE_TOOLS ON CACHE BOOL "" FORCE)
set(GRANITE_AUDIO OFF CACHE BOOL "" FORCE)
set(GRANITE_FAST_MATH OFF CACHE BOOL "" FORCE)

add_subdirectory("${PGS_DIR}/Granite" granite EXCLUDE_FROM_ALL)

if (APPLE)
    # Darwin lacks clock_nanosleep/TIMER_ABSTIME (same shim the runtime uses).
    target_compile_options(granite-util PRIVATE
        -include "${RT_ROOT}/third_party/PS2Recomp/ps2xRuntime/cmake/pgs/granite_macos_shim.h")
endif()

add_custom_target(build-slangmosh ALL DEPENDS slangmosh)
