#!/usr/bin/env bash
# rt-app feature regression matrix.
#
# Runs the headless app across the feature-flag matrix (terrain, textures,
# streaming, nav overlay, single-tile, states, render modes, audio) and
# asserts each run exits 0 and emits its deterministic feature log lines.
# Disc-dependent rows SKIP cleanly when no disc extract is present.
#
# Usage: scripts/rt_feature_matrix.sh [path-to-rt-app]
set -u

RT="${1:-build/native/rt-app}"
DISC="${RT_DISC:-$HOME/Library/Application Support/RoadTripRecomp/disc}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0; skip=0

# row <name> <ENV=V...> -- <expected-substring...>
row() {
  local name="$1"; shift
  local envs=() wants=()
  while [ $# -gt 0 ] && [ "$1" != "--" ]; do envs+=("$1"); shift; done
  [ $# -gt 0 ] && shift
  wants=("$@")
  local out="$TMP/$name.log"
  if ! ( env "${envs[@]}" RT_APP_FRAMES=60 RT_APP_SNAPSHOT="$TMP/$name.ppm" "$RT" ) >"$out" 2>&1; then
    echo "FAIL $name (exit!=0)"; fail=$((fail+1)); return
  fi
  for want in "${wants[@]}"; do
    if ! grep -qF "$want" "$out"; then
      echo "FAIL $name — missing: $want"
      fail=$((fail+1)); return
    fi
  done
  echo "PASS $name"; pass=$((pass+1))
}

row_required_disc() { # like row but SKIPs when $DISC missing
  local name="$1"
  if [ ! -d "$DISC" ]; then echo "SKIP $name (no disc extract)"; skip=$((skip+1)); return; fi
  row "$@"
}

[ -x "$RT" ] || { echo "rt-app not found at $RT (build first)"; exit 1; }

# --- core states -------------------------------------------------------------
row title      RT_APP_STATE=TITLE RT_APP_NOAUDIO=1 -- "rt-app state=TITLE"
row menu       RT_APP_STATE=MENU RT_APP_NOAUDIO=1   -- "rt-app state=MENU"
row playing    RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 -- "rt-app state=PLAYING" "session state="

# --- terrain / textures / streaming / nav --------------------------------------
row terrain    RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NOAUDIO=1 \
               -- "rt-app terrain: tiles=64"
row textured   RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NOAUDIO=1 \
               -- "pages=" "unresolved=0"
row stream     RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_STREAM=1 RT_APP_STREAM_STEP_FRAMES=5 RT_APP_NOAUDIO=1 \
               -- "4-slot streaming" "slot 0 <- id" "loads="
row stream_row RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_STREAM=1 RT_APP_STREAM_ROW=0 RT_APP_NOAUDIO=1 \
               -- "window row 0"
row nav        RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NAV=1 RT_APP_NOAUDIO=1 \
               -- "rt-app nav: static graph 0 segments=35"

# --- render modes -------------------------------------------------------------
row norender   RT_APP_STATE=PLAYING RT_APP_NORENDER=1 RT_APP_NOAUDIO=1 \
               -- "rt-app state=PLAYING"
row snapshot   RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 -- "rt-app snapshot:"
[ -s "$TMP/snapshot.ppm" ] && { echo "PASS snapshot-file-nonempty"; pass=$((pass+1)); } \
                          || { echo "FAIL snapshot-file-nonempty"; fail=$((fail+1)); }

# --- disc-dependent -----------------------------------------------------------
row_required_disc single_tile RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 \
                  RT_APP_TERRAIN_FILE="$DISC/FLD/000.BIN" RT_APP_NOAUDIO=1 \
                  -- "rt-app terrain: loaded 000.BIN"
row_required_disc bgm         RT_APP_STATE=PLAYING RT_DISC="$DISC" RT_APP_BGM="$DISC/SOUND/1CH_L.VAG" \
                  -- "rt-app BGM loaded"
row_required_disc bgm_tsq     RT_APP_STATE=PLAYING RT_DISC="$DISC" RT_APP_BGM_TSQ=1 \
                  -- "rt-app BGM TSQ loaded"
row_required_disc disc_terrain RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_DISC="$DISC" RT_APP_NOAUDIO=1 \
                  -- "rt-app terrain: tiles=64"
row_required_disc disc_nav     RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NAV=1 RT_DISC="$DISC" RT_APP_NOAUDIO=1 \
                  -- "rt-app nav: static graph 0 segments=35"

echo
echo "rt-app feature matrix: $pass passed, $fail failed, $skip skipped"
[ "$fail" -eq 0 ]
