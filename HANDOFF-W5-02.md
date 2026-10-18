# W5-02 handoff — native-reference ruling batch

## Applied

- `docs/native-reference/rumble-haptics.md`
  - **Before:** holds were documented only as literals `0x01..0x7F`.
  - **After:** every byte except `0x80..0x84` and `0xFF` is documented as a
    hold, explicitly including `0x00` and `0x85..0x8B` = 133..139 frames;
    `0x00` is a zero-frame hold, not a terminator.
  - **Evidence:** ROM disassembly at `0x0020A734`/`0x0020A758` branches all
    default bytes to `0x0020A7E0`; `0x0020A7E0` `lbu` and `0x0020A7E4` `sb`
    install that byte as the hold counter. Only the preceding comparisons
    special-case `0x80..0x84` and `0xFF`.

- `docs/native-reference/tsq-tvb-format.md`
  - **Before:** E5 was a tentative pitch operation, EA was “fine tuning,” F9
    had no operands/unknown semantics, and the offline-render sentence called
    E4/E5/EA signed semitone modifiers.
  - **After:** EA is the Q12 bend multiplier, E5 is absolute pitch plus voice
    retrigger, and F9 is the two-operand external-channel fork; all are
    explicitly identified as SNDMOD `play` (`0x5294`) opcodes, not road
    directives.
  - **Evidence:** extracted `SNDMOD.IRX` ELF bytes at `play` `0x5294` and
    dispatch entries `0x53D8..0x5620`; handler mapping is `pitch_cent`
    `0x48BC` (EA), `key_pitch` `0x4A40` (E5), `ext_chan` `0x45D4` (F9).

- `docs/native-reference/hud-layout.md`
  - **Before:** only a generic per-player badge row was present; the
    TOTALTIME/LAPTIME group identities and uncentred origin were absent.
  - **After:** TOTALTIME is documented as the uncentred `(20,20)` group and
    the per-player `(490,20)` label as LAPTIME; ElementC remains at y=52.
  - **Evidence:** SLUS PT_LOAD mapping reads `0x002EC1A8` as
    `{1,20,20,0x29F628,2,...}` and `0x002EC070` as
    `{0,490,20,0x29F668,2,...}`. `raceHudElementC` uses its `(dx,dy)`
    arguments directly (`0x0022F750`).

- `docs/native-reference/race-hud-atlas.md`
  - **Before:** the timer capsule was described as unmatched/opaque
    `(87,109,159)` and the rank digits were described as scaled screenshot
    mock geometry.
  - **After:** the capsule is CSA1 source tint RGB `(32,24,64)`, alpha
    `0.509`; rank digits are 1:1 28x30 cells with
    `u=(d%9)*28`, `v=(d/9)*30`.
  - **Evidence:** `raceHudElementC` selects texture 0 / CSA1 at
    `0x0022F7A4..0x0022F7B4`; `raceHudElementD`
    `0x002302B0..0x0023030C` emits `(u,v,w,h)` =
    `((d%9)*28,(d/9)*30,28,30)` without scaling.

## Rejected corrections

None. All four supplied rulings matched the ROM/data-path evidence.
