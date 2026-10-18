# Camera and projection constants

Updated 2026-10-16. Addresses are SLUS-203.98 virtual addresses; ELF file
offset is `vaddr - 0x1ff000`. The table labels instruction-verified facts as
**CONFIRMED** and host-model mappings as **INFERRED**.

## Confirmed values

| Parameter | Value | Evidence / load or store sites | Confidence |
|---|---:|---|---|
| Default preset offset | `(0.0, 2.0, -7.0)` | `0x0029ce00..0x0029ce0f`; copied by `LoadCameraPreset` at `0x0021faf8..0x0021fb18`. | CONFIRMED |
| Default focal value | `500.0` | f32 `0x0029ce10`; loaded from `cam+0x10` in `buildCamera` at `0x0021f748`, passed at `0x0021f73c`. | CONFIRMED |
| Default pitch target | `0x0380` turns = `0.08590293 rad` (4.921875°) | u16 `0x0029ce14`; copied at `0x0021fb08..0x0021fb0c`, targeted at `0x0021eb2c`, stored at `0x0021eb74`. | CONFIRMED |
| Pitch/collision step | `0x0080` turns = `0.02454369 rad` per 60 Hz frame | target clamp `0x0021eb50..0x0021eb74`; collision writes `0x0021e110`, `0x0021e1b8`, `0x0021e230`, `0x0021e2d8`. | CONFIRMED |
| Chase slew acceleration | `+/-0.00100000005` render units/frame² | f32 `DAT_00332c30..44` at `0x00332c30..44`; lateral `0x0021dd08..0x0021dd78`, height `0x0021de08..0x0021def0`. | CONFIRMED |
| Standard view distance pair | `2048.0`, `2048.0` | `0x45000000` materialized and passed at `0x0021e73c..0x0021e764`; stored to `DAT_003356e0/e4` by `0x0021f910`. Mode 2 far is `1992.0` at `0x0021e814..20`; mode 3 reads `DAT_00332c4c` at `0x0021e840..48`. | CONFIRMED |
| Projection horizontal scale | `0.4699999988` | `DAT_00332c80`: `lwc1 f23,-0x7e74(gp)` at `0x0021f500`; GP resolves to `0x0033aaf4`, hence `0x00332c80`. It divides the X numerator at `0x0021f558..0x0021f56c`. | CONFIRMED |
| Projection vertical scale | `0.8000000119` (re-verified against ELF f32 at `0x00332c84` = 0.800000011920929, 2026-10-18) | `DAT_00332c84`: `lwc1 f22,-0x7e70(gp)` at `0x0021f50c`; GP resolves to `0x0033aaf4`, hence `0x00332c84`. It divides the Y numerator at `0x0021f594..0x0021f5c8`. | CONFIRMED |
| GS/VU projection terms | `{-383.0038147, 25166074, 1.00004578, -3.00006866, 240.639999, -8388499.5, 8388500.5}` | `DAT_00332c88..a0`, loaded/written at `0x0021f750..0x0021f7b8`. | CONFIRMED |

`buildCamera` makes matrices at `+0/+0x40/+0x80/+0xc0/+0x100`, copies world
position at `+0x180`, and stores focal at `+0x18c` (`0x0021f698..0x0021f908`).

## Host mapping and unknowns

**CONFIRMED:** the normal chase task selects presets; no inspected
`RunCarCameraTask` branch derives focal, offset, or pitch from car speed. The
old continuous 9→16 speed zoom and 5°→25° pitch ramp were guesses.

**CONFIRMED:** `ComputeChaseCameraBasis` accelerates its stored delta by
`0.001` and clips it to remaining displacement. This is a rate-limited slew,
not a damped spring.

**INFERRED:** host `distanceNear/Far = 7` and `baseHeight = 2` represent
preset 0's `(0,2,-7)` in the simplified eye/target model. Native code builds
basis vectors, not a host eye/target pair. Host `lookAtHeight = 1.5` remains
unproven.

The native projection is GS/VU screen-space construction, not a standard clip
matrix. Conventional vertical FOV/aspect/host clip planes are **UNKNOWN**;
the supported equivalents are focal `500` (ELF `0x0029ce10`), X/Y scales `0.47/0.8` (ELF f32 `0x00332c80`/`0x00332c84`, re-verified 2026-10-18), and the
view-distance pair. No camera-shake/knockback scalar chain was found in the
bounded normal-chase functions `0x21dc18`, `0x21e070`, `0x21e690`, `0x21ec58`,
and `0x21f698`; other states remain **UNKNOWN**.
