# Tachometer needle draw

## Evidence-backed specification

The one-player gauge handler assembles the needle at **0x00249E10–0x00249EDC**.
It selects drive texture-table entry 1 with palette bank (CSA) 12, then emits one
rotated quad.  The drive table binding and the METER0 container mapping identify
this as the second METER0 image: `ITEM/METER0.GSL` unit 2, a 128×120 PSMT4 page
whose adjacent unit supplies its CT16 palette.  The page layout is documented
from the disc bytes in `gsl-vm.md`; it begins at file offset `0x8550`.

| field | value | evidence |
|---|---:|---|
| source page | `METER0.GSL` unit 2, CSA 12, 128×120 | `0x00249E04–0x00249E0C`; GSL units at `0x8550` / `0xA350` |
| source rectangle | `(u,v,w,h) = (3,53,8,59)` | immediate stores at `0x00249E44–0x00249E6C` |
| pivot from 1P gauge origin | `(70,68)` | stores at `0x00249E24` and `0x00249E58` |
| local strip vertices | `(-4,-59), (4,-59), (-4,0), (4,0)` | float stores at `0x00249E78–0x00249EAC` |
| turn source | car signed halfword at `+0x1D0`, capped at 10000 | `CalculateTachometerValue`, `0x00249520–0x00249563` |

The 1P gauge origin is `(484,294)` (`0x002A8FF0`), so the dial-centre pivot is
`(554,362)`.  With `r` clamped to `[0,10000]`, the stored turn is
`r / 10000 - 0.7` (`0x00249DD0–0x00249E0C`). `uiArenaRotQuads`
(`0x0024EB10`) multiplies it by π from `0x00332F84`; therefore the screen-space
angle is `π * (r / 10000 - 0.7)`.  Zero points toward the top of the display;
positive turns clockwise because the UI y axis grows downward.

The quad emitter region-clamps the source rectangle: `uiSetSpriteClamp`
(`0x002078A0`) sets the inclusive limits to `(u, u+w-1, v, v+h-1)`.  Its batch
header uses the arena sprite blend state: `ALPHA_1 = 0x44`, equivalent to
source-alpha interpolation over the existing colour, with `TEST_1 = 0x5001B`
and `TEX1 = 0x60` (`gifBeginSpriteBatch`, reached through `0x0024F5C8`).  The
needle path has no local colour write, so it uses that batch's unmodulated
palette colour.  It follows the dial face, gear and speed sprites, and the hub
cap is emitted immediately afterward (`0x00249EE0–0x00249F5C`); retain this
order so the cap hides the needle root.

## Native helper

`src/native/preview/gauge_needle.h` exposes the exact cell, pivot, and rotated
four-vertex strip. Its UV output is normalized for the 128×120 CSA-12 page;
the coordinates remain in the 640×448 design space used by the gauge pass.

`tests/native_w19/gauge_needle_test.cpp` fixes two measured directions in the
usual counter-clockwise `atan2(y-up, x-right)` frame: 117 RPM maps to
`-146.11°`, and 6217 RPM maps to `+104.09°`. It also checks the 59-pixel
pivot-to-tip distance and rigid edges over eight turns.

## Open item

This describes only the single-player rotating needle. Two-player gauges use a
different strip display path and need separate evidence before sharing this
asset or geometry.
