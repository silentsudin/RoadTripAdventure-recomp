#!/usr/bin/env bash
# Build, install, and smoke the authored native Android scene on the dev app.
set -euo pipefail

serial="97a75c3e"
package="io.github.silentsudin.roadtrip.dev"
activity="$package/.RoadTripNativeActivity"
root="$(cd "$(dirname "$0")/../.." && pwd)"
project="$root/src/native/android"
scratch="$root/build/scratch/android-native"
sdk_root="${ANDROID_HOME:-$HOME/Library/Android/sdk}"
ndk_root="${ANDROID_NDK_HOME:-}"
if [[ -z "$ndk_root" ]]; then
  for candidate in "$sdk_root/ndk"/* /opt/homebrew/Caskroom/android-ndk/* /opt/homebrew/share/android-ndk; do
    [[ -f "$candidate/build/cmake/android.toolchain.cmake" ]] && { ndk_root="$candidate"; break; }
  done
fi
[[ -f "$ndk_root/build/cmake/android.toolchain.cmake" ]] || { echo "Android NDK not found" >&2; exit 1; }

export ANDROID_HOME="$sdk_root"
export ANDROID_NDK_HOME="$ndk_root"
mkdir -p "$scratch"
printf 'sdk.dir=%s\n' "$sdk_root" > "$project/local.properties"

"$root/platform/android/gradlew" -p "$project" :app:assembleDebug
apk="$project/app/build/outputs/apk/debug/app-debug.apk"
# `-s` (install to shared/external storage) is rejected on current Android
# builds. `-r` is the in-place same-signature update; allowBackup=false in the
# manifest prevents backup transport for this dev package.
adb -s "$serial" install -r "$apk"

# env.txt is app-private and survives the same-signature -r update. No game
# data crosses USB here; only runtime flags are written.
adb -s "$serial" shell "run-as $package sh -c 'printf \"RT_APP_FRAMES=600\\n\" > files/env.txt'"
adb -s "$serial" logcat -c
adb -s "$serial" shell am force-stop "$package"
adb -s "$serial" shell am start -n "$activity"
sleep 10
adb -s "$serial" shell pidof "$package" | grep -q .
adb -s "$serial" logcat -d -s RoadTrip:V > "$scratch/startup.log" || true
adb -s "$serial" exec-out screencap -p > "$scratch/smoke.png"

# The native default discovers the preserved files/disc directory. Exercise
# terrain only when that directory exists; never copy it into the repository.
if adb -s "$serial" shell "run-as $package sh -c 'test -d files/disc'"; then
  adb -s "$serial" shell "run-as $package sh -c 'printf \"RT_APP_FRAMES=600\\nRT_APP_STATE=PLAYING\\nRT_APP_TERRAIN=1\\n\" > files/env.txt'"
  adb -s "$serial" shell am force-stop "$package"
  adb -s "$serial" shell am start -n "$activity"
  sleep 10
  adb -s "$serial" shell pidof "$package" | grep -q .
  adb -s "$serial" logcat -d -s RoadTrip:V > "$scratch/terrain.log" || true
fi

python3 - "$scratch/smoke.png" <<'PY'
import struct, sys, zlib
p = open(sys.argv[1], 'rb').read()
assert p[:8] == b'\x89PNG\r\n\x1a\n'
pos = 8; chunks = []
while pos < len(p):
    n, = struct.unpack('>I', p[pos:pos+4]); kind = p[pos+4:pos+8]
    data = p[pos+8:pos+8+n]; pos += 12+n
    chunks.append((kind, data))
w, h, depth, color, _, _, interlace = struct.unpack('>IIBBBBB', next(d for k,d in chunks if k == b'IHDR'))
print(f'PNG {w}x{h}, color_type={color}, depth={depth}, interlace={interlace}')
if color == 6 and depth == 8 and interlace == 0:
    raw = zlib.decompress(b''.join(d for k,d in chunks if k == b'IDAT'))
    stride = w * 4; prev = bytearray(stride); totals = [0,0,0]; pixels = 0; off = 0
    for _ in range(h):
        f = raw[off]; off += 1; row = bytearray(raw[off:off+stride]); off += stride
        for i in range(stride):
            a = row[i-4] if i >= 4 else 0; b = prev[i]; c = prev[i-4] if i >= 4 else 0
            if f == 1: row[i] = (row[i] + a) & 255
            elif f == 2: row[i] = (row[i] + b) & 255
            elif f == 3: row[i] = (row[i] + ((a+b)//2)) & 255
            elif f == 4:
                q=a+b-c; pa=abs(q-a); pb=abs(q-b); pc=abs(q-c); row[i]=(row[i]+(a if pa<=pb and pa<=pc else b if pb<=pc else c))&255
        for i in range(0, stride, 4):
            totals[0] += row[i]; totals[1] += row[i+1]; totals[2] += row[i+2]; pixels += 1
        prev = row
    print('mean RGB=' + ','.join(f'{v/pixels:.1f}' for v in totals))
else:
    print('dominant-color analysis skipped: expected non-interlaced RGBA8 PNG')
PY

echo "APK: $apk"
echo "Smoke evidence: $scratch"
