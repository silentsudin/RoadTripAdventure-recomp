#!/usr/bin/env bash
# First-run native asset-cache rehearsal. Never targets the release package.
set -euo pipefail
serial=97a75c3e
package=io.github.silentsudin.roadtrip.dev
activity="$package/.RoadTripNativeActivity"
root="$(cd "$(dirname "$0")/.." && pwd)"
scratch="$root/build/scratch/thor-setup"
disc="$HOME/Library/Application Support/RoadTripRecomp/disc"
remote=/sdcard/Download/RoadTripDisc
mkdir -p "$scratch"
pass(){ printf 'PASS  %-26s %s\n' "$1" "${2:-}"; }
fail(){ printf 'FAIL  %-26s %s\n' "$1" "${2:-}" >&2; exit 1; }

[[ -f "$disc/SLUS_203.98" ]] || fail stage "Mac disc tree unavailable"
local_kib=$(du -sk "$disc" | awk '{print $1}')
remote_kib=$(adb -s "$serial" shell "du -sk '$remote' 2>/dev/null | awk '{print \$1}'" | tr -d '\r' || true)
if [[ "$local_kib" != "$remote_kib" ]]; then
  adb -s "$serial" shell "mkdir -p '$remote'"
  adb -s "$serial" push "$disc/." "$remote/" >/dev/null
fi
pass stage "${local_kib} KiB"

"$root/scripts/native/android_build.sh"
pass build-install
adb -s "$serial" shell "run-as $package sh -c 'rm -rf files/assets; printf "'"'RT_APP_FRAMES=900\nRT_APP_TERRAIN=1\nRT_APP_STATE=PLAYING\n'"'" > files/env.txt'"
adb -s "$serial" logcat -c
start=$(date +%s)
adb -s "$serial" shell am force-stop "$package"
adb -s "$serial" shell am start -n "$activity" >/dev/null
for _ in $(seq 1 300); do
  adb -s "$serial" logcat -d -s RoadTrip:I > "$scratch/first-run.log" || true
  grep -q 'assets: ready' "$scratch/first-run.log" && break
  sleep 1
done
grep -q 'assets: ready' "$scratch/first-run.log" || fail extract "timeout after 300s"
adb -s "$serial" exec-out screencap -p > "$scratch/progress.png"
elapsed=$(( $(date +%s) - start ))
pass extract "${elapsed}s"
adb -s "$serial" shell "run-as $package ls -la files/assets; run-as $package du -sh files/assets" > "$scratch/assets-ls.txt"
pass inventory "$(tail -1 "$scratch/assets-ls.txt")"

adb -s "$serial" logcat -c
adb -s "$serial" shell am force-stop "$package"
adb -s "$serial" shell am start -n "$activity" >/dev/null
sleep 3
adb -s "$serial" logcat -d -s RoadTrip:I > "$scratch/restart.log" || true
grep -q 'assets: ready' "$scratch/restart.log" || fail restart "no ready marker"
grep -q 'extract:' "$scratch/restart.log" && fail restart "unexpected extraction"
pass restart "cache skip"
