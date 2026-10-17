#!/bin/sh
# Post-recovery full verification chain: run after the sandbox regains DNS
# and WindowServer access (post-crash artifact). Waits, then performs every
# gated check and pushes. Safe to run manually too.
#
# Hardening (wave1 B7):
#   - every rt-app invocation (and the matrix run) is bounded by a timeout
#     (default 300s per run; RT_VERIFY_TIMEOUT / RT_MATRIX_TIMEOUT to change)
#     so a hung capture fails its step instead of hanging the chain;
#   - a failed capture no longer silently skips scoring: the per-capture
#     CAP-SUMMARY line says FAIL with the reason, and the chain exits
#     non-zero at the end;
#   - each capture echoes a one-line PASS/FAIL summary carrying the histogram
#     score and verdict band when a score exists. Bands (FAR/NEAR/CLOSE) are
#     calibration info (docs/native-reference/frame-compare.md), not gates:
#     only capture/scoring tool failures count toward the chain exit code;
#   - town (drive-view) captures set RT_APP_MODE=explore. RT_APP_STATE=PLAYING
#     alone exposes its initial EXPLORE state for one frame, then starts the
#     first AI race (main.cpp "directStartRace"), racing past the town view
#     that the town_driving.png oracle comparison needs. RT_APP_MODE=explore
#     holds SessionState::EXPLORE indefinitely.
set -- "$(cd "$(dirname "$0")/.." && pwd)"
cd "$1" || exit 1
MAXWAIT=${MAXWAIT:-2400}
RUNTIMEOUT=${RT_VERIFY_TIMEOUT:-300}
PROBETIMEOUT=${RT_VERIFY_PROBE_TIMEOUT:-60}
MATRIXTIMEOUT=${RT_MATRIX_TIMEOUT:-2400}
FAILED=0

fail() { FAILED=$((FAILED + 1)); }

# run_timed SECS CMD...: bound CMD so a hung run is killed (rc 124 with
# coreutils timeout, rc 142 via perl SIGALRM) and fails the step.
run_timed() {
  _secs=$1; shift
  if command -v timeout >/dev/null 2>&1; then
    timeout "$_secs" "$@"
  elif command -v gtimeout >/dev/null 2>&1; then
    gtimeout "$_secs" "$@"
  else
    perl -e 'alarm shift @ARGV; exec @ARGV or exit 127' "$_secs" "$@"
  fi
}

WAITED=0
while [ "$WAITED" -lt "$MAXWAIT" ]; do
  if run_timed "$PROBETIMEOUT" env RT_APP_STATE=TITLE RT_APP_NOAUDIO=1 RT_APP_FRAMES=2 ./build/native/rt-app >/dev/null 2>&1; then
    echo "ENV-RECOVERED after ${WAITED}s"; break
  fi
  sleep 20; WAITED=$((WAITED + 20))
done
if [ "$WAITED" -ge "$MAXWAIT" ]; then echo "ENV-STILL-BROKEN"; exit 1; fi

mkdir -p build/scratch/gamelook build/scratch/frame-compare
G=build/scratch/gamelook

capture() { # name oracle env...: run rt-app, score the snapshot, summarize.
  name=$1; oracle=$2; shift 2
  run_timed "$RUNTIMEOUT" env "$@" RT_APP_NOAUDIO=1 RT_APP_SNAPSHOT=$G/$name.ppm ./build/native/rt-app >/tmp/vcap-$name.log 2>&1
  crc=$?
  echo "CAP $name rc=$crc"
  if [ "$crc" -ne 0 ] || [ ! -s "$G/$name.ppm" ]; then
    echo "CAP-SUMMARY $name FAIL (capture rc=$crc, snapshot missing/empty; no score; log /tmp/vcap-$name.log)"
    fail
    return
  fi
  if python3 scripts/frame_compare.py --oracle build/scratch/oracle-golden/$oracle --native "$G/$name.ppm" \
      --label "$name" --json build/scratch/frame-compare/$name.json >/dev/null 2>&1; then
    hist=$(RT_COMPARE_JSON=build/scratch/frame-compare/$name.json python3 -c 'import json, os; d = json.load(open(os.environ["RT_COMPARE_JSON"])); print(round(d["histogram_intersection"], 3), d["verdict"])')
    echo "CAP-SUMMARY $name PASS hist=$hist"
  else
    echo "CAP-SUMMARY $name FAIL (frame_compare failed; snapshot kept at $G/$name.ppm)"
    fail
  fi
}

# Town line carries RT_APP_MODE=explore (see header); race keeps
# RT_APP_MODE=race. Frame counts unchanged.
capture title-v9 title.png            RT_APP_STATE=TITLE   RT_APP_FRAMES=150
capture menu-v9  main_menu.png        RT_APP_STATE=MENU    RT_APP_FRAMES=60
capture town-v9  town_driving.png     RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_FRAMES=120
capture race-v9  race_hud_driving.png RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_FRAMES=240

if run_timed "$MATRIXTIMEOUT" scripts/rt_feature_matrix.sh > /tmp/matrix-v9.log 2>&1; then
  echo "MATRIX-PASS"
else
  echo "MATRIX-FAIL rc=$? (log /tmp/matrix-v9.log)"
  fail
fi
grep -E "passed|FAIL" /tmp/matrix-v9.log | tail -4

# Intra-run determinism pair (town). Both legs need RT_APP_MODE=explore so the
# pair compares the held town view, not the single EXPLORE frame the legacy
# direct-launch auto-race leaves.
run_timed "$RUNTIMEOUT" env RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 RT_APP_SNAPSHOT=/tmp/detA.ppm ./build/native/rt-app >/dev/null 2>&1
arc=$?
run_timed "$RUNTIMEOUT" env RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 RT_APP_SNAPSHOT=/tmp/detB.ppm ./build/native/rt-app >/dev/null 2>&1
brc=$?
if [ "$arc" -eq 0 ] && [ "$brc" -eq 0 ] && cmp -s /tmp/detA.ppm /tmp/detB.ppm; then
  echo DET-OK
else
  echo "DET-DIFF (rcA=$arc rcB=$brc)"
  fail
fi

git push -q origin re/native-complete && echo PUSHED || { echo PUSH-FAILED; fail; }
echo "CHAIN-SUMMARY failures=$FAILED"
echo CHAIN-DONE
[ "$FAILED" -eq 0 ] && echo CHAIN-PASS || echo CHAIN-FAIL
[ "$FAILED" -eq 0 ]
