#!/usr/bin/env bash
# Builds the Android app: libmain.so with CMake (the NDK), stages the native libraries, packages
# the APK with Gradle. Contains no game code or data (scripts/android/check_apk.py checks).
#
#   scripts/android/build_apk.sh            # build/android-dev, then the APK
#   scripts/android/build_apk.sh --install  # and adb install -r
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
NDK="${NDK:-/opt/homebrew/share/android-ndk}"
BUILD="$ROOT/build/android-dev"
JNI="$ROOT/build/android-jni/arm64-v8a"
PREBUILT="$NDK/toolchains/llvm/prebuilt/darwin-x86_64"

if [ ! -f "$BUILD/build.ninja" ]; then
    cmake -G Ninja -S "$ROOT" -B "$BUILD" -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
        -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-31 -DANDROID_STL=c++_shared -DCMAKE_BUILD_TYPE=Release
fi
cmake --build "$BUILD" --target main -j "$(sysctl -n hw.ncpu 2>/dev/null || nproc)"

mkdir -p "$JNI"
"$PREBUILT/bin/llvm-strip" --strip-debug -o "$JNI/libmain.so" "$BUILD/libmain.so"
cp "$PREBUILT/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so" "$JNI/"
# The compiler the app builds the game with (scripts/android/build_llvm.sh), when built.
if [ -f "$ROOT/build/android-llvm/jniLibs/arm64-v8a/libllvm.so" ]; then
    cp "$ROOT/build/android-llvm/jniLibs/arm64-v8a/libllvm.so" "$JNI/"
fi
# The build kit the app compiles the game with (assets/rt.tar): runtime headers, NDK headers and
# link stubs, clang resources. No game code.
python3 "$ROOT/scripts/bundle_sdk.py" --android \
    --compile-commands "$BUILD/compile_commands.json" --probe "$ROOT/src/game/sdk_probe.cpp" \
    --shim "$ROOT/src/game/game_shim.cpp" --recomp /dev/null --config "$ROOT/config/roadtrip.toml" \
    --resources "$ROOT/build/android-kit/res" --ndk-sysroot "$PREBUILT/sysroot" \
    --clang-resource "$ROOT/build/android-llvm/clang-resource" \
    --builtins "$(ls "$PREBUILT"/lib/clang/*/lib/linux/libclang_rt.builtins-aarch64-android.a | head -1)" \
    --compiler "$JNI/libllvm.so"
mkdir -p "$ROOT/build/android-assets" # (a fresh build dir has none: the kit was silently left out)
mv "$ROOT/build/android-kit/android-assets/"* "$ROOT/build/android-assets/"
# Every third-party component's licence, LLVM's included (the compiler and libc++ ship in the APK).
rm -rf "$ROOT/build/android-assets/licenses"
python3 "$ROOT/scripts/collect_licenses.py" "$BUILD" "$ROOT/build/android-assets/licenses" --llvm "$ROOT/build/android-llvm/src"

cd "$ROOT/platform/android"
[ -f local.properties ] || echo "sdk.dir=${ANDROID_HOME:-$HOME/Library/Android/sdk}" > local.properties
# Debug build (debuggable: `adb shell run-as` reaches its files) unless --release.
VARIANT=debug; TASK=assembleDebug
for a in "$@"; do [ "$a" = "--release" ] && VARIANT=release && TASK=assembleRelease; done
./gradlew -q "$TASK"
APK="$ROOT/platform/android/app/build/outputs/apk/$VARIANT/app-$VARIANT.apk"
ls -la "$APK"
for a in "$@"; do [ "$a" = "--install" ] && adb install -r "$APK"; done
true
