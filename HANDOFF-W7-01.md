# W7-01 handoff — tachometer needle

## Delivered files

- `src/native/preview/gauge_needle.h`
- `tests/native_w19/CMakeLists.txt`
- `tests/native_w19/gauge_needle_test.cpp`
- `docs/native-reference/gauge-needle-draw.md`

## Draw specification

The one-player handler's raw ROM instructions at **0x00249E04–0x00249EDC** set
the drive texture table to entry 1, CSA 12 and emit one rotated strip sourced
from `ITEM/METER0.GSL` unit 2 (the `128×120` PSMT4 page at file offset
`0x8550`, with its following CT16 palette). The needle cell is `(3,53,8,59)`.
Its pivot is the 1P dial origin `(484,294)` plus `(70,68)`, and its four local
vertices are `(-4,-59), (4,-59), (-4,0), (4,0)`.

`0x00249DC0` calls `CalculateTachometerValue` (`0x00249520`), which reads the
signed engine-RPM halfword at car `+0x1D0` and caps it at 10000. The handler
divides by the `10000.0` constant at `0x00332F50`, adds `-0.7` from
`0x00332F54`, and stores that turn at the record's `+0x88`.
`uiArenaRotQuads` (`0x0024EB10`) scales it by π (`0x00332F84`) before its VU0
rotation, yielding `theta = pi * (clamp(rpm,0,10000)/10000 - 0.7)`.

`uiSetSpriteClamp` (`0x002078A0`) region-clamps to the cell. The shared arena
batch is the standard HUD alpha path (`ALPHA_1=0x44`, source-alpha over the
destination; `TEST_1=0x5001B`, `TEX1=0x60`), reached through
`uiArenaBeginBatch` (`0x0024F5C8`). The handler writes no needle-local colour.
Ordering is dial face/gear/digits, needle, then the hub cap
(`0x00249EE0–0x00249F5C`), so the cap must draw after the needle.

## Integration note (do not edit `main.cpp` in this wave)

`main.cpp` loads `hudArt.gaugePage` through `loadHudArt()`; that is already the
METER0 unit-2 CSA-12 texture decoder. In the existing Meter0 pass around lines
2559–2638, draw the baked dial with `hudArt.speedDial`, then submit the four
vertices from `gauge_needle::needleQuad(theta, 484, 294)` against
`hudArt.gaugePage`, using its normalized UVs. Use the runtime RPM feed (not the
displayed mph), composite the needle after the dial and before a future hub-cap
quad, and preserve source-alpha blending. The current textured helper accepts
axis-aligned `HudQuad`s, so the integration needs the existing HUD textured
path extended to accept this four-vertex strip rather than approximating it as
an axis-aligned sprite.

## Commands and output

```sh
python3 scripts/ghidra/fn.py 0x24f640 0x24f728
/Users/holzr/.pyenv/versions/FaceSweeper/bin/python3 - <<'PY'
# Capstone MIPS64 little-endian disassembly of ROM vaddr 0x00249DC0..0x00249F10
PY
mkdir -p build/scratch/w7-01
clang++ -std=c++20 -I src/native tests/native_w19/gauge_needle_test.cpp -o build/scratch/w7-01/needle_test && build/scratch/w7-01/needle_test
```

The deliberate RED compile before the header existed produced:

```text
fatal error: 'preview/gauge_needle.h' file not found
```

The completed direct test produced:

```text
CAUGHT: +15 degree needle-angle perturbation misses the coast golden
checks=11 failed=0
```

## Open questions

- The two-player dial is a different METER10 strip renderer; do not reuse this
  1P needle without tracing that separate path.
- The current preview API has no four-corner textured primitive, so drawing the
  verified geometry awaits a small renderer-facing integration change.
