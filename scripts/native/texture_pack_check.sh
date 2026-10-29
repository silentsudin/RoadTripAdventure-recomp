#!/usr/bin/env bash
# HD texture-pack app-level check: dumping changes no pixel, a pack changes
# pixels where its content is drawn, an empty pack changes nothing.
#
#   scripts/native/texture_pack_check.sh [build dir] [renderer]
#
# Uses the built rt-app on the deterministic race scene (virtual time, headless,
# 110 frames) and makes its own pack from the dumps, so nothing committed is
# needed. Unit contract coverage lives in native_assets_texture_pack_test.
set -euo pipefail

repo=$(cd "$(dirname "$0")/../.." && pwd)
build=${1:-"$repo/build/w-G0-verify"}
renderer=${2:-vulkan}
app="$build/rt-app"
[ -x "$app" ] || { echo "texture_pack_check: no rt-app in $build" >&2; exit 1; }

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
mkdir -p "$work/dumps" "$work/pack" "$work/empty"

run() {  # run <snapshot> [extra env...]
  local shot=$1; shift
  env RT_TIME=virtual RT_APP_STATE=PLAYING RT_APP_MODE=race RT_APP_NOAUDIO=1 \
      RT_APP_HEADLESS=1 RT_APP_FRAMES=110 RT_APP_RENDERER="$renderer" \
      RT_APP_SNAPSHOT="$shot" "$@" "$app" > "$work/log" 2>&1
}

run "$work/base.ppm"
run "$work/dump.ppm" RT_TEXTURE_DUMP="$work/dumps"
cmp -s "$work/base.ppm" "$work/dump.ppm" || { echo "FAIL: dumping changed the picture" >&2; exit 1; }
images=$(ls "$work/dumps" | wc -l | tr -d ' ')
[ "$images" -gt 0 ] || { echo "FAIL: dumping wrote nothing" >&2; exit 1; }

python3 - "$work" <<'PY'
import sys
from pathlib import Path
work = Path(sys.argv[1])
for f in (work / 'dumps').glob('*.pam'):
    b = f.read_bytes()
    i = b.index(b'ENDHDR\n') + 7
    raster = b[i:]
    out = bytearray()
    for k in range(0, len(raster), 4):
        out += bytes((255, 0, 255, raster[k + 3]))
    (work / 'pack' / f.name).write_bytes(b[:i] + bytes(out))
PY

run "$work/pack.ppm" RT_TEXTURE_PACK="$work/pack"
run "$work/empty.ppm" RT_TEXTURE_PACK="$work/empty"
cmp -s "$work/empty.ppm" "$work/base.ppm" || { echo "FAIL: an empty pack changed the picture" >&2; exit 1; }
python3 - "$work" <<'PY'
import sys
from pathlib import Path
work = Path(sys.argv[1])
a = (work / 'base.ppm').read_bytes()
b = (work / 'pack.ppm').read_bytes()
n = sum(1 for i in range(min(len(a), len(b))) if a[i] != b[i])
print(f"texture_pack_check: {len(list((work / 'dumps').glob('*.pam')))} dumped textures, "
      f"{n} bytes changed by the pack")
sys.exit(0 if n > 0 else 1)
PY
echo "texture_pack_check: PASS ($renderer)"
