# B5 gauge calibration report

- Decoded `METER0.GSL` and `METER10.GSL` from the available extracted disc to
  `build/scratch/hud-art/`; METER10 is available and is PSMT8+CT32 here.
- Oracle speed readings: `race_hud_driving` = **63 mph**; `race_hud` =
  **99 mph**.  The speed-digits atlas cells are METER10 `(digit*20,102)`.
- Current native display divisor is `/100`; no gauge is present in
  `race-v13.ppm`, so it does not validate that provisional mapping.  A
  cap-constrained alternative is `20000/186 = 107.5269` native units/mph.
- Needle measurements: 63 mph = +118.80°, 99 mph = +0.36° (right=0°, up=+90°).
  The two-point-only candidate is `-33.93 - 3.2900*mph`; it has zero fitted
  residual by construction and needs a low-speed oracle before implementation.
- `main.cpp` currently renders no speed needle; it only blits METER0 and
  emits METER10 digit quads.  No source files were changed.

Full method, decoder, source citations, limitations, and reproduction Python:
[`docs/native-reference/gauge-calibration.md`](../../docs/native-reference/gauge-calibration.md).
