#!/usr/bin/env bash
# Cross-build the native Windows app with the pinned llvm-mingw toolchain.
set -euo pipefail

repo=$(cd "$(dirname "$0")/../.." && pwd)
build_root=${RT_WINDOWS_BUILD_ROOT:-"$repo/build/scratch/w18-windows"}
toolchain="$repo/cmake/toolchains/llvm-mingw.cmake"
jobs=${CMAKE_BUILD_PARALLEL_LEVEL:-4}

mkdir -p "$build_root"
llvm_root=$(python3 "$repo/scripts/fetch_llvm_mingw.py" --out "$build_root/llvm-mingw")
bin="$llvm_root/bin"
prefix="$build_root/sdl-install"
vulkan_lib="$build_root/vulkan/libvulkan-1.a"

tool() {
    if [[ -x "$bin/$1" ]]; then echo "$bin/$1"; else echo "$bin/$1.exe"; fi
}

if [[ ! -f "$prefix/lib/cmake/SDL3/SDL3Config.cmake" ]]; then
    mkdir -p "$build_root/sdl-src"
    cat > "$build_root/sdl-src/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.21)
project(rt_windows_sdl LANGUAGES C)
include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.18
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SDL3)
EOF
    cmake -S "$build_root/sdl-src" -B "$build_root/sdl-build" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DLLVM_MINGW_ROOT="$llvm_root" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$prefix"
    cmake --build "$build_root/sdl-build" -j "$jobs"
    cmake --install "$build_root/sdl-build"
fi

# CMake needs a library at configure time. Populate it after the first link
# attempt from the vk imports that the linked native app actually uses.
mkdir -p "$(dirname "$vulkan_lib")"
if [[ ! -f "$vulkan_lib" ]]; then
    "$(tool llvm-ar)" rcs "$vulkan_lib"
fi

configure() {
    cmake -S "$repo/src/native" -B "$build_root/app" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="$toolchain" -DLLVM_MINGW_ROOT="$llvm_root" \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$prefix" \
        -DVulkan_INCLUDE_DIR=/opt/homebrew/include \
        -DVulkan_LIBRARY="$vulkan_lib" \
        "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld -stdlib=libc++"
}

configure
if ! cmake --build "$build_root/app" --target rt-app -j "$jobs"; then
    mapfile -t vk_symbols < <(find "$build_root/app/CMakeFiles" -name '*.obj' -print0 | \
        xargs -0 "$(tool llvm-nm)" -u 2>/dev/null | \
        awk '$NF ~ /^vk[A-Za-z0-9_]+$/ { print $NF }' | sort -u)
    if ((${#vk_symbols[@]} == 0)); then
        echo "windows_cross: rt-app failed before Vulkan imports could be collected" >&2
        exit 1
    fi
    def="$build_root/vulkan/vulkan-1.def"
    { echo 'LIBRARY vulkan-1.dll'; echo EXPORTS; printf '  %s\n' "${vk_symbols[@]}"; } > "$def"
    "$(tool x86_64-w64-mingw32-dlltool)" -d "$def" -l "$vulkan_lib"
    cmake --build "$build_root/app" --target rt-app -j "$jobs"
fi

echo "$build_root/app/rt-app.exe"
