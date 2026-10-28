#!/usr/bin/env bash
# Build and exercise the native app on Linux, with both renderer paths.
#
# Assets are NOT baked into the image: the host's own extracted disc folder is
# mounted read-only at /disc and the app reads it through RT_DISC (the same rule
# the app follows everywhere — assets come from the player's own disc).
set -euo pipefail

root=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
image=${RT_NATIVE_W16_IMAGE:-roadtrip-native-w16}
build=/work/build/linux-w16
out=/work/build/scratch/w16-linux
report=$out/LINUX.md
disc=${RT_DISC:-$HOME/Library/Application Support/RoadTripRecomp/disc}

if [[ ${RT_NATIVE_W16_CONTAINER:-} != 1 ]]; then
    docker build -f "$root/docker/Dockerfile.native-w16" -t "$image" "$root/docker"
    mounts=(--mount "type=bind,src=$root,dst=/work")
    # The same disc the macOS goldens were made from, when the host has it.
    if [[ -d "$disc" ]]; then
        mounts+=(--mount "type=bind,src=$disc,dst=/disc,readonly")
    fi
    exec docker run --rm "${mounts[@]}" -e RT_NATIVE_W16_CONTAINER=1 -e RT_DISC=/disc \
        "$image" /work/scripts/native/linux_w16.sh
fi

mkdir -p "$out" /tmp/rt-xdg
export XDG_RUNTIME_DIR=/tmp/rt-xdg
export VK_ICD_FILENAMES=${VK_ICD_FILENAMES:-/usr/share/vulkan/icd.d/lvp_icd.aarch64.json}

cmake -S /work/src/native -B "$build" -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release \
    -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
cmake --build "$build" -j"${RT_NATIVE_W16_JOBS:-2}"

smoke=(RT_TIME=virtual RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1
       RT_APP_HEADLESS=1 RT_APP_FRAMES=110 RT_APP_SNAPSHOT=$out/linux.ppm)
env "${smoke[@]}" "$build/rt-app" > "$out/vulkan-headless.log" 2>&1
env "${smoke[@]}" RT_APP_SNAPSHOT=$out/linux2.ppm "$build/rt-app" > "$out/vulkan-headless2.log" 2>&1

determinism=FAIL
if cmp "$out/linux.ppm" "$out/linux2.ppm"; then determinism=IDENTICAL; fi
golden=SKIPPED
if [[ -f /disc/SLUS_203.98 || -d /disc/CAR0 ]]; then
    if cmp "$out/linux.ppm" /work/build/scratch/closeout-a.ppm; then golden=IDENTICAL; else golden=DIFFERS; fi
else
    golden="SKIPPED (no host disc mounted)"
fi

tests=(native_assets_settings_test native_assets_container_test native_assets_car_physics_test
       native_assets_input_test native_assets_game_loop_test native_assets_route_test
       w14_ui_parity w16_gl_backend_seam_test)
passed=0
failed=0
: > "$out/tests.log"
for test_name in "${tests[@]}"; do
    if "$build/$test_name" >> "$out/tests.log" 2>&1; then ((passed+=1)); else ((failed+=1)); fi
done

Xvfb :99 -screen 0 800x600x24 > "$out/xvfb.log" 2>&1 &
xvfb_pid=$!
trap 'kill "$xvfb_pid" 2>/dev/null || true' EXIT
sleep 1

run_windowed() {
    local backend=$1
    env SDL_VIDEODRIVER=x11 DISPLAY=:99 LIBGL_ALWAYS_SOFTWARE=1 RT_APP_RENDERER="$backend" \
        RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1 RT_APP_FRAMES=10 \
        RT_APP_SNAPSHOT="$out/${backend}.ppm" "$build/rt-app" > "$out/${backend}-windowed.log" 2>&1
}

vulkan_windowed=FAIL
gl_windowed=FAIL
if run_windowed vulkan; then vulkan_windowed=PASS; fi
if run_windowed gl; then gl_windowed=PASS; fi

cat > "$report" <<EOF
# W16 Linux container evidence

## Recipe

From the repository root:

    scripts/native/linux_w16.sh

It builds docker/Dockerfile.native-w16 (Debian bookworm, clang/CMake/Ninja, SDL
3.4.18, Vulkan loader/headers, Mesa lavapipe, Mesa GL/EGL, X11, Xvfb), mounts the
repository at /work, the host's extracted disc folder read-only at /disc, and
builds /work/build/linux-w16. No ROM or extracted asset is baked into the image;
the app reads the player's own disc through RT_DISC, as everywhere else.

## Commands

    cmake -S /work/src/native -B /work/build/linux-w16 -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DRT_NATIVE_ASSETS_BUILD_TESTS=ON
    cmake --build /work/build/linux-w16 -j${RT_NATIVE_W16_JOBS:-2}
    VK_ICD_FILENAMES=$VK_ICD_FILENAMES RT_TIME=virtual RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1 RT_APP_HEADLESS=1 RT_APP_FRAMES=110 RT_APP_SNAPSHOT=$out/linux.ppm /work/build/linux-w16/rt-app
    cmp $out/linux.ppm $out/linux2.ppm
    cmp $out/linux.ppm /work/build/scratch/closeout-a.ppm
    SDL_VIDEODRIVER=x11 DISPLAY=:99 RT_APP_RENDERER=vulkan ... /work/build/linux-w16/rt-app
    SDL_VIDEODRIVER=x11 DISPLAY=:99 RT_APP_RENDERER=gl ... /work/build/linux-w16/rt-app

## Results

- Build: PASS.
- Headless lavender-pipe (lavapipe) race smoke: PASS.
- Linux run-to-run determinism: $determinism.
- Against the macOS golden closeout-a.ppm: $golden.
- Disc-free test subset: $passed passed, $failed failed (${tests[*]}).
- Xvfb/lavapipe Vulkan presenter: $vulkan_windowed.
- Xvfb/Mesa software-GL presenter and GL renderer: $gl_windowed.

## Portability fixes (with the compiler error each fixed)

- src/native/guest/guest_kernel.cpp: added <unordered_map> (clang 14: no template named 'unordered_map' in namespace 'std' at the RT_FAULT_BT diagnostic).
- src/native/guest/gen/{00223d58,0023a8e8,00243848}.cpp: a bare label ending a block needed a statement (clang 14: expected statement); scripts/ghidra/transliterate.py now emits the ';' so a regen keeps it.
- src/native/gameplay/camera.cpp: <cstdio> (clang 14: no member named 'printf' in namespace 'std').
- src/native/gameplay/results.cpp: <algorithm> (clang 14: no member named 'clamp' in namespace 'std').
- tests/native_w2/enhancements_app_test.cpp: clang 14 cannot capture structured bindings in a lambda; copies keep every check identical.
- tests/native_w4/race_hud_layout_test.cpp: libstdc++ rejects std::pair with an incomplete element inside struct Json; parallel key/value vectors.

## Container limits

Mesa software implementations only: physical Linux GPU drivers, handheld GLES
behaviour, audio devices and the second-screen presentation display are not
covered here.
EOF

[[ $failed -eq 0 && $vulkan_windowed == PASS && $gl_windowed == PASS && $determinism == IDENTICAL && $golden != DIFFERS ]]
