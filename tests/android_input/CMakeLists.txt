# Builds the input layer (without the game or raylib) for Android, to check it stays portable:
#   scripts/check_android_input.sh
cmake_minimum_required(VERSION 3.24)
project(rt_input_android LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-3.4.18 GIT_SHALLOW TRUE)
FetchContent_Declare(toml11 GIT_REPOSITORY https://github.com/ToruNiina/toml11.git GIT_TAG v4.4.0 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SDL3 toml11)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
add_library(rt_input STATIC
    ${SRC}/platform/input/Config.cpp
    ${SRC}/platform/input/Devices.cpp
    ${SRC}/platform/input/Mapping.cpp
    ${SRC}/platform/input/Players.cpp
    ${SRC}/platform/input/Types.cpp)
target_include_directories(rt_input PRIVATE ${SRC})
target_compile_options(rt_input PRIVATE -Wall -Wextra)
target_link_libraries(rt_input PUBLIC SDL3::SDL3 toml11::toml11)
