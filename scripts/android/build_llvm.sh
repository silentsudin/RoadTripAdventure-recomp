#!/usr/bin/env bash
# Cross-builds the compiler the Android app ships to compile the game on the device: one
# multicall LLVM binary (clang + lld) for aarch64-linux-android, AArch64 target only.
# The output contains no game code. Its major version matches the NDK's clang, so it compiles
# against the NDK's libc++ headers.
#
#   scripts/android/build_llvm.sh            # -> build/android-llvm/jniLibs/arm64-v8a/libllvm.so
#   NDK=/path/to/ndk LLVM_TAG=llvmorg-21.1.8 scripts/android/build_llvm.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
NDK="${NDK:-/opt/homebrew/share/android-ndk}"
LLVM_TAG="${LLVM_TAG:-llvmorg-21.1.8}"
API="${API:-31}"
OUT="$ROOT/build/android-llvm"
SRC="$OUT/src"
JOBS="${JOBS:-$(sysctl -n hw.ncpu 2>/dev/null || nproc)}"

mkdir -p "$OUT"
if [ ! -d "$SRC/llvm" ]; then
    git clone --depth 1 --branch "$LLVM_TAG" https://github.com/llvm/llvm-project.git "$SRC"
fi

# Stage 1: host tablegen tools.
cmake -G Ninja -S "$SRC/llvm" -B "$OUT/host" -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_PROJECTS="clang;lld" -DLLVM_TARGETS_TO_BUILD=AArch64 \
    -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF
ninja -C "$OUT/host" -j "$JOBS" llvm-tblgen clang-tblgen llvm-min-tblgen

# Stage 2: the Android-hosted toolchain.
cmake -G Ninja -S "$SRC/llvm" -B "$OUT/android" \
    -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM="android-$API" -DANDROID_STL=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_PROJECTS="clang;lld" -DLLVM_TARGETS_TO_BUILD=AArch64 \
    -DLLVM_HOST_TRIPLE=aarch64-linux-android -DLLVM_DEFAULT_TARGET_TRIPLE="aarch64-linux-android$API" \
    -DLLVM_NATIVE_TOOL_DIR="$OUT/host/bin" \
    -DLLVM_TABLEGEN="$OUT/host/bin/llvm-tblgen" -DCLANG_TABLEGEN="$OUT/host/bin/clang-tblgen" \
    -DLLVM_TOOL_LLVM_DRIVER_BUILD=ON \
    -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF \
    -DLLVM_INCLUDE_UTILS=OFF -DLLVM_INCLUDE_DOCS=OFF -DLLVM_BUILD_TOOLS=ON \
    -DLLVM_ENABLE_ZLIB=OFF -DLLVM_ENABLE_ZSTD=OFF -DLLVM_ENABLE_LIBXML2=OFF \
    -DLLVM_ENABLE_TERMINFO=OFF -DLLVM_ENABLE_LIBEDIT=OFF -DLLVM_ENABLE_LIBPFM=OFF \
    -DCLANG_ENABLE_STATIC_ANALYZER=OFF -DCLANG_ENABLE_ARCMT=OFF \
    -DCLANG_PLUGIN_SUPPORT=OFF -DLLVM_ENABLE_PLUGINS=OFF -DLLVM_BUILD_LLVM_DYLIB=OFF
ninja -C "$OUT/android" -j "$JOBS" llvm-driver clang-resource-headers

mkdir -p "$OUT/jniLibs/arm64-v8a"
"$NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-strip" -o "$OUT/jniLibs/arm64-v8a/libllvm.so" "$OUT/android/bin/llvm"
rm -rf "$OUT/clang-resource" && mkdir -p "$OUT/clang-resource"
cp -R "$OUT/android/lib/clang/"*/include "$OUT/clang-resource/include"
ls -la "$OUT/jniLibs/arm64-v8a/libllvm.so"
