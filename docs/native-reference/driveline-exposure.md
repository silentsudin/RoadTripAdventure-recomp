# Native session driveline exposure (W6-04)

## Motion source and units

The session reads `GameLoop::car_.speed` after `tickCar` in
`GameLoop::integrateCar`. It is the existing signed native **speed word**:
one physics tick changes it by 40 while accelerating, it is capped at 20000,
and the settled HUD conversion is `floor(speed_word / 55.07)` MPH. It remains
the sole input to the new presentation driveline; the gearbox/RPM result does
not feed position, heading, steering, or future speed integration.

ROM evidence says tach RPM needs a separate longitudinal/coupling state, not
this speed word. Until a live trace supplies that missing link, the session
forms that state with deterministic integer calibration:

* coasting: `trunc(speed_word * 106 / 2148)`;
* loaded (`CarInput::gas`): `trunc(speed_word * 5634 / 3469)`.

The denominators are `floor(39 * 55.07)` and `floor(63 * 55.07)`. With row-0
gear 1 (ratio 116), the ROM-law projection `trunc(motion * 128 / ratio)` gives
116 RPM at the coast point and 6216 RPM at the loaded point: residual **-1
RPM** versus the approximate 117/6217 oracle readings. This is an authored
bridge, not a claim that ROM speed and coupling values have been proven to
relate this way.

## Gearbox policy

`advanceDriveline` consumes `gameplay/gauge_rpm.h` directly, using gearbox row
0 and automatic-selector policy 0: upshift at 8201 RPM; downshift when current
RPM is below 5000 and projected next-lower RPM is below 8000. The row has
forward ratios 1..5, so this session currently uses 1..5. Reverse input (or a
negative speed word) reports gear 0 and tach RPM 0.

The documented `AdvanceVehicleMotion` literals are represented as deterministic
`DrivelineState::boostTargetRpm`: 9000 while loaded and below 9501 RPM, 3000
while coasting and below 9501 RPM, otherwise 0. The flag-selector semantics
are not recovered, so this target is intentionally not allowed to alter the
ROM-derived coupling projection.

## Snapshot/API layout

`FrameSnapshot` keeps every prior member in place and appends:

```cpp
int rpm;   // 0..10000
int gear;  // 0=R, 1..5 forward (provisional)
```

The deterministic per-physics-tick state is held by `GameLoop::driveline_`.
`makeSnapshot()` copies it to the appended snapshot fields. The legacy
`CarState::rpm` and `CarState::gear` are mirrored only for coherent session
presentation and are not physics inputs.

## Coordinator integration

In `main.cpp`, after receiving the session `FrameSnapshot snapshot`, pass
`snapshot.rpm` and `snapshot.gear` to the HUD state/renderer. No environment
variable, new session call, or conversion is needed. The values are already
valid for the settled 0..10000 needle law and the glyph byte domain.

## Open questions

* Capture a live ROM trace relating the HUD speed word to the longitudinal
  argument passed to `AdvanceVehicleMotion`; replace the authored calibration
  bridge when that relation is known.
* Identify the flag selecting ROM auto policies 0/1 and the semantics of the
  3000/9000 target path.
* W6-03 must confirm whether the HUD should retain all six forward glyphs;
  this native row-0 session currently exposes only 1..5, provisionally.
