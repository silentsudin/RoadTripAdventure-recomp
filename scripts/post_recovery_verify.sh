#!/bin/sh
# Post-recovery full verification chain: run after the sandbox regains DNS
# and WindowServer access (post-crash artifact). Waits, then performs every
# gated check and pushes. Safe to run manually too.
set -- "$(cd "$(dirname "$0")/.." && pwd)"
cd "$1" || exit 1
MAXWAIT=${MAXWAIT:-2400}
WAITED=0
while [ "$WAITED" -lt "$MAXWAIT" ]; do
  if dscacheutil -q host -a name github.com | grep -q ip_address && \
     RT_APP_STATE=TITLE RT_APP_NOAUDIO=1 RT_APP_FRAMES=2 ./build/native/rt-app >/dev/null 2>&1; then
    echo "ENV-RECOVERED after ${WAITED}s"; break
  fi
  sleep 20; WAITED=$((WAITED + 20))
done
if [ "$WAITED" -ge "$MAXWAIT" ]; then echo "ENV-STILL-BROKEN"; exit 1; fi

mkdir -p build/scratch/gamelook build/scratch/frame-compare
G=build/scratch/gamelook
run() { # name env...
  name=$1; shift
  env "$@" RT_APP_NOAUDIO=1 RT_APP_SNAPSHOT=$G/$name.ppm ./build/native/rt-app >/tmp/vcap-$name.log 2>&1
  echo "CAP $name rc=$?"
}
run title-v9  RT_APP_STATE=TITLE RT_APP_FRAMES=150
run menu-v9   RT_APP_STATE=MENU RT_APP_FRAMES=60
run town-v9   RT_APP_STATE=PLAYING RT_APP_FRAMES=120
run race-v9   RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_FRAMES=240

score() { # label oracle
  python3 scripts/frame_compare.py --oracle build/scratch/oracle-golden/$2 --native $G/$1.ppm \
    --label $1 --json build/scratch/frame-compare/$1.json >/dev/null 2>&1 && \
  python3 -c "import json; d=json.load(open('build/scratch/frame-compare/$1.json')); print('$1 hist=', round(d['histogram_intersection'],3), d['verdict'])"
}
score title-v9 title.png
score menu-v9 main_menu.png
score town-v9 town_driving.png
score race-v9 race_hud_driving.png

scripts/rt_feature_matrix.sh > /tmp/matrix-v9.log 2>&1; echo "MATRIX_EXIT=$?"; grep -E "passed|FAIL" /tmp/matrix-v9.log | tail -4

# Intra-run determinism pair (town).
RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 RT_APP_SNAPSHOT=/tmp/detA.ppm ./build/native/rt-app >/dev/null 2>&1
RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_FRAMES=60 RT_APP_SNAPSHOT=/tmp/detB.ppm ./build/native/rt-app >/dev/null 2>&1
cmp -s /tmp/detA.ppm /tmp/detB.ppm && echo DET-OK || echo DET-DIFF

git push -q origin re/native-complete && echo PUSHED || echo PUSH-FAILED
echo CHAIN-DONE
