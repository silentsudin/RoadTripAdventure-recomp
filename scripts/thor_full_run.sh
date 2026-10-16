#!/usr/bin/env bash
# Full native TOWN + RACE rehearsal on the Thor development package only.
set -uo pipefail

serial=97a75c3e
package=io.github.silentsudin.roadtrip.dev
activity="$package/.RoadTripNativeActivity"
root="$(cd "$(dirname "$0")/.." && pwd)"
scratch="$root/build/scratch/thor-run"
disc="$HOME/Library/Application Support/RoadTripRecomp/disc"
remote=/sdcard/Download/RoadTripDisc
mkdir -p "$scratch"
exec > >(tee "$scratch/command.log") 2>&1

declare -A phase=([staging]=FAIL [install]=FAIL [extract]=FAIL [title]=FAIL [town]=FAIL [race]=FAIL [artifacts]=FAIL)
note() { printf '\n== %s ==\n' "$*"; }
mark() { phase["$1"]="$2"; printf '%s  %-10s %s\n' "$2" "$1" "${3:-}"; }

note "staging development disc"
if [[ ! -f "$disc/SLUS_203.98" ]]; then
  mark staging FAIL "missing local disc tree: $disc"
else
  local_kib=$(du -sk "$disc" | awk '{print $1}')
  remote_kib=$(adb -s "$serial" shell "du -sk '$remote' 2>/dev/null | awk '{print \$1}'" | tr -d '\r' || true)
  if [[ -z "$remote_kib" || "$remote_kib" == 0 ]]; then
    adb -s "$serial" shell "mkdir -p '$remote'" && adb -s "$serial" push "$disc/." "$remote/"
    remote_kib=$(adb -s "$serial" shell "du -sk '$remote' | awk '{print \$1}'" | tr -d '\r' || true)
  fi
  [[ -n "$remote_kib" && "$remote_kib" != 0 ]] && mark staging PASS "local=${local_kib}KiB remote=${remote_kib}KiB" || mark staging FAIL "local=${local_kib}KiB remote=${remote_kib:-absent}KiB"
fi

note "build and install development APK"
if "$root/scripts/native/android_build.sh"; then
  mark install PASS "$package"
else
  mark install FAIL "android_build.sh failed; continuing with existing dev install"
fi

note "fresh asset extraction"
adb -s "$serial" shell "run-as '$package' sh -c 'rm -rf files/assets files/states; mkdir -p files; printf \"RT_APP_FRAMES=1200\\nRT_APP_TERRAIN=1\\nRT_APP_STATE=PLAYING\\nRT_DISC=/storage/emulated/0/Download/RoadTripDisc\\n\" > files/env.txt'" || true
adb -s "$serial" logcat -c || true
extract_start=$(date +%s)
adb -s "$serial" shell am force-stop "$package" || true
adb -s "$serial" shell am start -n "$activity" || true
ready=0
for _ in $(seq 1 300); do
  adb -s "$serial" logcat -d -s RoadTrip:V > "$scratch/extract.log" || true
  if grep -q 'assets: ready' "$scratch/extract.log"; then ready=1; break; fi
  sleep 1
done
extract_seconds=$(( $(date +%s) - extract_start ))
if (( ready )); then mark extract PASS "${extract_seconds}s"; else mark extract FAIL "timeout after ${extract_seconds}s"; fi
adb -s "$serial" exec-out screencap -p > "$scratch/title-or-extract.png" || true

note "test socket and scenarios"
adb -s "$serial" forward --remove tcp:9876 >/dev/null 2>&1 || true
if adb -s "$serial" forward tcp:9876 localabstract:roadtrip-test; then
  if python3 "$root/scripts/thor_native_scenarios.py" --serial "$serial" --screenshots "$scratch" --result "$scratch/scenarios.json"; then
    :
  else
    echo "scenario runner returned non-zero (see scenarios.json)"
  fi
else
  printf '{"error":"adb forward failed","gaps":[]}' > "$scratch/scenarios.json"
fi
adb -s "$serial" forward --remove tcp:9876 >/dev/null 2>&1 || true

if [[ -f "$scratch/scenarios.json" ]]; then
  read -r title town race socket_closed < <(python3 - "$scratch/scenarios.json" <<'PY'
import json, sys
r=json.load(open(sys.argv[1]))
print(r.get("title",{}).get("status","FAIL"), r.get("town",{}).get("status","FAIL"),
      r.get("race",{}).get("status","FAIL"), str(r.get("socket_closed",False)).lower())
PY
)
  mark title "$title" "socket close=$socket_closed"
  mark town "$town"
  mark race "$race"
fi

note "artifacts and logs"
adb -s "$serial" logcat -d -v threadtime -s RoadTrip:V > "$scratch/logcat-roadtrip.log" || true
grep -Ei 'fatal|error|exception|abort|signal|assert' "$scratch/logcat-roadtrip.log" > "$scratch/logcat-errors.log" || true
if [[ -s "$scratch/title-or-extract.png" && -s "$scratch/scenarios.json" && -s "$scratch/logcat-roadtrip.log" ]]; then
  mark artifacts PASS "$scratch"
else
  mark artifacts FAIL "missing screenshot, scenario result, or logcat slice"
fi

printf '\nTHOR FULL RUN TABLE\n'
for key in staging install extract title town race artifacts; do printf '%-10s %s\n' "$key" "${phase[$key]}"; done
printf '\nGAPS\n'
python3 - "$scratch/scenarios.json" <<'PY' 2>/dev/null || true
import json, sys
for gap in json.load(open(sys.argv[1])).get("gaps", []): print("NATIVE-MODEL GAP: " + gap)
PY
printf '\nSCREENSHOTS\n'
find "$scratch" -maxdepth 1 -name '*.png' -exec sh -c 'wc -c < "$1" | tr -d " " | xargs printf "%s bytes  %s\\n" "$1"' _ {} \;
printf '\nCOMMAND LOG TAIL\n'
tail -n 40 "$scratch/command.log" || true
printf '\nLOGCAT ERROR TAIL\n'
tail -n 40 "$scratch/logcat-errors.log" || true

for key in staging install extract title town artifacts; do [[ "${phase[$key]}" == PASS ]] || exit 1; done
exit 0
