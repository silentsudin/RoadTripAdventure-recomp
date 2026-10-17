# HUD live data — verified transcription (W2-10)

Oracle artifact: `/Users/holzr/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98`
(vaddr − 0x1FF000 = file offset). Everything below re-run by the coordinator;
the agentic lane's claims that did NOT survive verification are marked.

## Handler split (supersedes gauge-calibration.md §layout)
The HUD draw dispatcher is the jump table at **0x2A8FB0** (selector `t0` from
player count at 0x336960 / 0x336B84, gate 0x249478/0x249484):

| table idx | target | meaning |
|---|---|---|
| 0 | 0x2496C8 | 1P gauge + HUD (`800,600,400,300,0` init, `addiu s2,r0,1`) |
| 10 | 0x24E1A0 | 2P HUD (`800,600,400,300,1` init); default when player count ≠ 0 (0x24968c–0x2496b8) |
| 1–9, 11 | 0x249C40 / 0x249D68 / 0x249D5C / 0x24A0BC / 0x24A200 / 0x24A848 / 0x24AC9C / 0x24B184 / 0x24B298 / 0x24C2C0 / 0x24E1A0 | sub-panels |
| 12 | 0 (terminator) | |

Consequence for the current native code: the **METER10 digits at v=102 in
main.cpp are the 2P layout**; the 1P layout (verified here) draws digits from
the same METER0 sheet at u=480/500/520, v=235. Adjudicate with the oracle A/B
rescore during integration (oracle goldens are 1P).

## Verified 1P facts (oracle re-measurement PASS)
- **Race timer = raceFrame − 1** (30 fps): 2/2 oracle readouts — frame 2446 →
  `0:10.87`, frame 3177 → `0:35.23`; frames−0 gives 0/2. Static digits at
  origin (484,294), cell 16×23, glyph order `0123456789:.`; hundreds digit
  skipped when <100.
- **Needle**: `theta = pi*(min(rpm,10000)/10000 − 0.7)` about UI (554,362)
  (= 692.5,452.5 in 1000×560 space), length 57.355 RU; rpm = car+0x1D0 read
  by fshader-op0 helper 0x249DD0. Constants pi=0x40490fdb @0x2A40FC,
  0.7f @0x29D180, 10000.0f @0x29ABE0, recip @0x29D138. This **supersedes the
  mph-based needle table** in gauge-calibration.md. Fit residual 0.84° over the
  3 independent measured needle points; honest fit needs ≥4 oracle frames with
  rpm logged (add to capture contract).
- **Speed digits**: mph integer 0–999, MSB-first, 16×23 glyphs from METER0 at
  u=480/500/520, v=235; skip chains at 0x249764/0x249784/0x2497A8;
  fshader op=2 (bright/active) when speed>0, op=1 when 0; digits off unless
  flag@0x336964≠0 and town mode (bit0=0) or race mode (bit0=1 and
  flag@0x336968≠0).
- **1P origin** (484,294): float pair (484.0,294.0) at 0x2A8FF0 =
  (2P origin row) + (1P offset row); entries measured, not inferred.
- Oracle pixel law for these elements: screenX = originX + floor(x),
  screenY = originY + round(y), UI row = oracle row − 1.
- Display builder matches: `SpeedOracleBins` 100-bucket monotone map fits both
  oracle mph points exactly (synthetic raw→mph bins; the guest raw-speed unit
  itself is open — see GAP).

## GAP — speed unit scale (coordinator-owned)
Native telemetry (vehicle_trace.csv) speed/100 at frames 2446/3177 = 184/190,
oracle digits = 39/63. The /100 divisor matches guest rounding, but native
physics speed scale is ~4.7× the guest's raw units. Resolve during W2-03
integration: fit guest raw speed vs integrated speed on the locked telemetry
trace (candidate alt divisor 107.527 from B6 is *not* authoritative).

## Module + tests
- `src/native/gameplay/hud_live.{h,cpp}` — pure functions (no GL, no globals):
  `hudTimerString`, `speedDigits`, `needleRadians`, `gaugeNeedleEndpoints`,
  `speedOracleBins`, per-quad layout builder; 23 PIN records.
- `tests/native_w2/hud_live_test.cpp` → `w2_hud_live_test`: elf pins 69/69,
  timer vectors 14/14, oracle readouts 2/2, bins 2/2, static 11/11,
  race-mode 2/2; NO_ELF=2 when ROM absent.
- `src/native/tools/w2_hud_raster_main.cpp` → `rt-w2-hud-raster`: rasterizes
  layouts to PPM (`--check` prints `SCORE hist=... pass=...`), default
  threshold 0.80 on race frames. ROI hist: race@2446 = 0.872,
  race@3177 = 0.906 (σ=0.5 Gaussian resolve model).
- `tests/native_w2/hud_oracle_diff.py` → crops oracle regions (alpha-composited
  over black) to 24-bit BMPs under `build/scratch/oracle-hud/`.
- **Known caveat**: the current build's live digits still read low
  (`dialDigitsLive 18,19`) until the unit-scale GAP resolves; the W2-10 test
  covers display + layout + oracle bins, not the physics input scale.

## Integration notes (deferred to coordinator)
1. Replace main.cpp digit page/coords with the 1P table above **if** the A/B
   rescore beats the METER10 variant against oracle race frames.
2. Wire `hudTimerString(raceFrame)`; timer glyph cells 15 wide at v=102 are
   2P — 1P timer uses 16×23 cells from METER0.
3. Needle: consume car rpm (needs the trace extension for rpm) instead of the
   mph-derived table; keep fallback until ≥4 rpm frames exist.
4. Add `RT_CAPTURE_EXTRAS` rpm logging to the capture contract (main.cpp).
