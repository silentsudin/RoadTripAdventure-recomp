#!/bin/zsh
# W11 wave closeout: decls regen -> build -> native sweep -> det pair. Run from repo root.
# Commit/push stays with the coordinator after reading the logs.
set -e
cd "$(dirname "$0")/../.."
LOG=/tmp/rt-closeout-$(date +%H%M)
echo "== refresh_decls"
python3 scripts/refresh_decls.py src/native/guest/gen build/decomp/guest_decls.inc --apply
echo "== configure"
RT_SKIP_ANDROID_CHECK=1 RT_SKIP_VRC_PLUGIN=1 cmake -S src/native -B build/w-G0-verify -DCMAKE_BUILD_TYPE=Release -DRT_NATIVE_ASSETS_BUILD_TESTS=ON > $LOG-cmake.log 2>&1
echo "== build"
cmake --build build/w-G0-verify -j 8 > $LOG-build.log 2>&1 || { grep -E "error|FAILED" $LOG-build.log | head -40; exit 1; }
grep -c "error:" $LOG-build.log || true
grep -c "non-void function does not return" $LOG-build.log > build/scratch/port/return_warn_count.txt || true
echo "return-type warnings: $(cat build/scratch/port/return_warn_count.txt 2>/dev/null || echo 0) (see overlap_ledger for pending; must trend to 0)"
echo "== sweep (from repo root — fixture-relative tests)"
fail=0; n=0
for t in build/w-G0-verify/*_test; do
  n=$((n+1))
  if ! "$t" > /tmp/rt-sweep-$(basename $t).log 2>&1; then fail=$((fail+1)); echo "FAIL: $t"; fi
done
echo "sweep $n binaries, $fail failed"
[ $fail -eq 0 ] || exit 1
echo "== det pair race@110"
A=build/scratch/closeout-a.ppm B=build/scratch/closeout-b.ppm
det_rc=0
for f in $A $B; do
  RT_TIME=virtual RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1 RT_APP_HEADLESS=1 \
  RT_APP_FRAMES=110 RT_APP_SNAPSHOT=$f build/w-G0-verify/rt-app > /tmp/rt-det-$(basename $f).log 2>&1 || det_rc=$?
  echo "$(basename $f) rc=$det_rc $(grep -o "simulation_frame=[0-9]*" /tmp/rt-det-$(basename $f).log | tail -1)"
done
[ $det_rc -eq 0 ] || { echo "DET PAIR: rt-app nonzero exit"; exit 1; }
cmp $A $B && echo "DET PAIR IDENTICAL"
echo "closeout logs: $LOG-{cmake,build}.log"
