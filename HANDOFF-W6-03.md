# W6-03 handoff — HUD settled-law audits

## A — MPH divisor: VERIFIED, no implementation change

`src/native/gameplay/hud_live.{h,cpp}` already applies the settled host law:
`floor(abs(speed_word) / 55.07f)`, clamped to `0..186`; the METER0 digit geometry
is already `(573,388)`, step `-17`, `16x23`, and `u=140+16*(d%7)`,
`v=23*(d/7)`.

The ROM does **not** contain `55.07` as a fixed-point divisor in this input
domain. It computes the original velocity-vector domain in `GetSpeedFromF15Vector`
(`0x0021D358`, Q15 vector magnitude times 216), then `CalculateSpeedometerDigits`
loads exact f32 `0x3FCDF3B6 = 1.6089999675750732` from `0x00332F48` and executes
`div.s` at `0x00249508`; handler 0 calls it at `0x00249A7C`. This is documented in
`docs/native-reference/gauge-calibration.md`; treating `1.609` as `55.07` would
cross unrelated units. No fixed-point equivalence assertion was added because none
was recovered.

Evidence: `build/scratch/w6-03/disasm.txt`.

## B — gear glyph: VERIFIED

`GetVehicleGear` is `jr ra; lb v0,0x1ff(a0)` at `0x002495B8/0x002495BC`; handler 0
calls it at `0x002499D4` and indexes it directly (`div g,5` at `0x00249A04`), with
no +/- adjustment. The canonical `ITEM/METER0.GSL` contact sheet at
`build/scratch/w6-03/gear_cells.png` reads:

```
R 1 2 3 4
5 6 blank blank blank
```

Therefore byte `g==0` is reverse; `g==1..6` are forward gears. The page actually
decodes as PSMT8 + CT32; CSA12 belongs to the separate PSMT4 needle page, not these
glyphs. Added grounded constants/predicate in `src/native/gameplay/gear_glyph.h` and
pinning test `tests/native_w17/gear_glyph_test.cpp`.

Coordinator-owned `src/native/gameplay/race_hud_layout.h` was not edited. Proposed
comment addition there:

```diff
+// Handler 0 gear byte car+0x1FF: 0 is the METER0 'R' cell; 1..6 are forward
+// gears. Glyph UV = ((g%5)*20+140, (g/5)*16+80), 20x14 (W6-03).
```

## C — minimap squash: VERIFIED and normalized

Raw ROM bytes at `0x00332CAC` (PT_LOAD mapping `0x200000/0x1000`) are
`d7 a3 f0 be`: little-endian f32 `0xBEF0A3D7 = -0.4699999988079071`.
`docs/native-reference/minimap.md` and the legacy minimap formula in
`docs/native-reference/map-view.md` now state the settled car-centred, north-up
law: `kKx≈0.20`, `kKy=kKx×0.47`, signed y scale `-0.47s`, with ROM citation.

## Test evidence

```text
$ c++ -std=c++20 -Isrc/native tests/native_w17/gear_glyph_test.cpp -o build/scratch/w6-03/gear_glyph_test
$ RT_ELF=build/rom/SLUS_203.98 build/scratch/w6-03/gear_glyph_test
W6-03 checks=15 failed=0 negative_control=CAUGHT

$ RT_ELF=build/scratch/w6-03/absent build/scratch/w6-03/gear_glyph_test
SKIP ROM pins: no ELF (set RT_ELF)
W6-03 checks=13 failed=0 negative_control=CAUGHT
```
