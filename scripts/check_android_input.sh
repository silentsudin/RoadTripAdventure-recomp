#!/bin/sh
# Checks that the input layer (src/platform/input) builds for Android arm64 with the NDK
# (ANDROID_NDK_HOME, or Homebrew's android-ndk). Nothing is run on a device.
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
NDK=${ANDROID_NDK_HOME:-/opt/homebrew/share/android-ndk}
cmake -S "$ROOT/tests/android_input" -B "$ROOT/build/android-input" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DCMAKE_BUILD_TYPE=Release
cmake --build "$ROOT/build/android-input"
echo "input layer builds for Android arm64"
