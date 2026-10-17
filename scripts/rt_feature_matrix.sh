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

# Per-row wall-clock bound so a hung run fails its row (rc 124 coreutils
# timeout / 142 perl SIGALRM) instead of hanging the whole matrix. 300s
# covers the heaviest legit row observed (first TERRAIN=1 TILES_RADIUS=all
# run pays scene/pipeline warm-up; it exceeded 120s on the B7 host).
ROW_TIMEOUT="${RT_MATRIX_ROW_TIMEOUT:-300}"

run_timed() { # SECS CMD...
  local secs="$1"; shift
  if command -v timeout >/dev/null 2>&1; then
    timeout "$secs" "$@"
  elif command -v gtimeout >/dev/null 2>&1; then
    gtimeout "$secs" "$@"
  else
    perl -e 'alarm shift @ARGV; exec @ARGV or exit 127' "$secs" "$@"
  fi
}

pass=0; fail=0; skip=0

# row <name> <ENV=V...> -- <expected-substring...>
row() {
  local name="$1"; shift
  local envs=() wants=()
  while [ $# -gt 0 ] && [ "$1" != "--" ]; do envs+=("$1"); shift; done
  [ $# -gt 0 ] && shift
  wants=("$@")
  local out="$TMP/$name.log" rc=0
  run_timed "$ROW_TIMEOUT" env "${envs[@]}" RT_APP_FRAMES=60 RT_APP_SNAPSHOT="$TMP/$name.ppm" "$RT" >"$out" 2>&1 || rc=$?
  if [ "$rc" -ne 0 ]; then
    case "$rc" in
      124|142) echo "FAIL $name (timeout after ${ROW_TIMEOUT}s)";;
      *)       echo "FAIL $name (exit=$rc)";;
    esac
    fail=$((fail+1)); return
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

# row_without <name> <ENV=V...> -- <forbidden-substring>: same run pattern as
# row(), but the row passes only when the substring is ABSENT from the log.
row_without() {
  local name="$1"; shift
  local envs=() rejects=()
  while [ $# -gt 0 ] && [ "$1" != "--" ]; do envs+=("$1"); shift; done
  [ $# -gt 0 ] && shift
  rejects=("$@")
  local out="$TMP/$name.log" rc=0 want
  run_timed "$ROW_TIMEOUT" env "${envs[@]}" RT_APP_FRAMES=60 RT_APP_SNAPSHOT="$TMP/$name.ppm" "$RT" >"$out" 2>&1 || rc=$?
  if [ "$rc" -ne 0 ]; then
    case "$rc" in
      124|142) echo "FAIL $name (timeout after ${ROW_TIMEOUT}s)";;
      *)       echo "FAIL $name (exit=$rc)";;
    esac
    fail=$((fail+1)); return
  fi
  for want in "${rejects[@]}"; do
    if grep -qF "$want" "$out"; then
      echo "FAIL $name — forbidden line present: $want"
      fail=$((fail+1)); return
    fi
  done
  echo "PASS $name"; pass=$((pass+1))
}

# row_without_required_disc: like row_without but SKIPs when $DISC missing.
row_without_required_disc() {
  local name="$1"
  if [ ! -d "$DISC" ]; then echo "SKIP $name (no disc extract)"; skip=$((skip+1)); return; fi
  row_without "$@"
}

[ -x "$RT" ] || { echo "rt-app not found at $RT (build first)"; exit 1; }

# --- core states -------------------------------------------------------------
row title      RT_APP_STATE=TITLE RT_APP_NOAUDIO=1 -- "rt-app state=TITLE"
row menu       RT_APP_STATE=MENU RT_APP_NOAUDIO=1   -- "rt-app state=MENU"
row playing    RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 -- "rt-app state=PLAYING" "session state="

# --- RT_APP_MODE gates -------------------------------------------------------
# RT_APP_MODE=explore holds SessionState::EXPLORE indefinitely (main.cpp); the
# bare PLAYING direct-launch above auto-starts the first AI race after one
# EXPLORE frame, so town/drive-view captures MUST set explore. This row is the
# regression gate for that contract.
row explore    RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_NOAUDIO=1 \
               -- "rt-app session state=EXPLORE"
row mode_race  RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1 \
               -- "rt-app mode: RACE (forced by RT_APP_MODE=race)"

# --- terrain / textures / streaming / nav --------------------------------------
row terrain    RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TILES_RADIUS=all RT_APP_NOAUDIO=1 \
               -- "rt-app terrain: tiles=64"
row textured   RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NOAUDIO=1 \
               -- "pages=" "unresolved=0"
row stream     RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_STREAM=1 RT_APP_STREAM_STEP_FRAMES=5 RT_APP_NOAUDIO=1 \
               -- "4-slot streaming" "slot 0 <- id" "loads="
row stream_row RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_STREAM=1 RT_APP_STREAM_ROW=0 RT_APP_NOAUDIO=1 \
               -- "window row 0"
row stream_pos RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_STREAM=1 RT_APP_STREAM_POS=1 RT_APP_NOAUDIO=1 \
               -- "demand slot" "demanded id 43"
row nav        RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NAV=1 RT_APP_NOAUDIO=1 \
               -- "rt-app nav: static graph 0 segments=35"
row nav_town   RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TEXTURES=1 RT_APP_NAV=1 RT_APP_NAV_TOWN=1 RT_APP_NOAUDIO=1 \
               -- "rt-app nav: town graph 1 segments=84"
row hudtext    RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_HUDTEXT=1 \
               -- "rt-app hudtext: FONTE decoded"
row scorehud   RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_HUDTEXT=1 RT_APP_SCOREHUD=1 \
               -- "rt-app scorehud: page=SCORE"
row carmodel   RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NOAUDIO=1 RT_APP_CARMODEL=0 \
               -- "rt-app carmodel: Q0"
row route      RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NOAUDIO=1 RT_APP_ROUTE=1 \
               -- "rt-app route: nav lap drive segments=34"
row route_town RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NOAUDIO=1 RT_APP_ROUTE=1 RT_APP_ROUTE_TOWN=1 \
               -- "rt-app route: town lap drive segments=80"

# --- render modes -------------------------------------------------------------
row norender   RT_APP_STATE=PLAYING RT_APP_NORENDER=1 RT_APP_NOAUDIO=1 \
               -- "rt-app state=PLAYING"
row snapshot   RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 -- "rt-app snapshot:"
[ -s "$TMP/snapshot.ppm" ] && { echo "PASS snapshot-file-nonempty"; pass=$((pass+1)); } \
                          || { echo "FAIL snapshot-file-nonempty"; fail=$((fail+1)); }
row interp     RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NOAUDIO=1 RT_APP_INTERP=1 \
               -- "rt-app interp: fixed-step sim"

# --- host wiring -------------------------------------------------------------
row triggers   RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_TRIGGERS=1 \
               -- "rt-app triggers: enabled"
row perflog    RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_PERFLOG=1 RT_APP_INTERP=1 RT_APP_INTERP_RATE=120 \
               -- "[perf]"
# Headless CI has no gamepad. The forced-family decision is deliberately logged
# at startup so this contract does not depend on device attachment.
row glyphs     RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_APP_GLYPH_FAMILY=ps \
               -- "family ps"
row settings   RT_APP_STATE=PLAYING RT_APP_NOAUDIO=1 RT_DATA_DIR="$TMP/settings-data" \
               -- "rt-app settings: defaults"

# --- disc-dependent -----------------------------------------------------------
# Minimap: the town street graph loads from the disc ELF whenever the pill is
# drawn; RT_APP_MINIMAP=0 kills both pill and roads (main.cpp).
row_required_disc minimap     RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_NOAUDIO=1 RT_DISC="$DISC" \
                  -- "rt-app minimap: town street segments="
row_without_required_disc minimap_off RT_APP_STATE=PLAYING RT_APP_MODE=explore RT_APP_NOAUDIO=1 RT_APP_MINIMAP=0 RT_DISC="$DISC" \
                  -- "rt-app minimap: town street segments="
row_required_disc single_tile RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 \
                  RT_APP_TERRAIN_FILE="$DISC/FLD/000.BIN" RT_APP_NOAUDIO=1 \
                  -- "rt-app terrain: loaded 000.BIN"
row_required_disc bgm         RT_APP_STATE=PLAYING RT_DISC="$DISC" RT_APP_BGM="$DISC/SOUND/1CH_L.VAG" \
                  -- "rt-app BGM loaded"
row_required_disc bgm_tsq     RT_APP_STATE=PLAYING RT_DISC="$DISC" RT_APP_BGM_TSQ=1 \
                  -- "rt-app BGM TSQ loaded"
row_required_disc disc_terrain RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_TILES_RADIUS=all RT_DISC="$DISC" RT_APP_NOAUDIO=1 \
                  -- "rt-app terrain: tiles=64"
row_required_disc disc_nav     RT_APP_STATE=PLAYING RT_APP_TERRAIN=1 RT_APP_NAV=1 RT_DISC="$DISC" RT_APP_NOAUDIO=1 \
                  -- "rt-app nav: static graph 0 segments=35"

echo
echo "rt-app feature matrix: $pass passed, $fail failed, $skip skipped"
[ "$fail" -eq 0 ]
