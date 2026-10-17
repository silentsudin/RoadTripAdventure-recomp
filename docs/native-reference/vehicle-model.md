# Vehicle model (W2-03)

`gameplay/vehicle_model` is an integration-safe fixed-point kernel for the
native app.  It deliberately does **not** choose engine, brake, or drag
constants: those depend on the equipped part records loaded by
`LoadCarPerformanceSpecs`, rather than the former invented universal host
constants.

## Confirmed scalar path

| operation | implementation | ELF evidence |
|---|---|---|
| f15 vector magnitude to raw speed | `sqrt((x/32768)^2 + ...) * 216.0` | `build/decomp/fn/0021d358.c:11-16` |
| raw speed to dial | `trunc(raw / 1.6089999675750732)`, normally cap 186 | `build/decomp/fn/00249458.c:13-21`; `docs/native-reference/rules-tables.md:254-268` |
| airborne vertical delta | `-0x59` f15/tick | `build/decomp/fn/0021ba70.c:158-164`; `rules-tables.md:261-263` |
| position commit | `(velocity << 4) / 0x19`, signed truncation toward zero | `build/decomp/fn/0021ba70.c:394-400` |
| position words to road units | divide by `20971.51953125` | `build/decomp/fn/0021ba70.c:421-426` and ELF `0x00332c0c` |
| steering command | target clamp +/-512, slew 8/tick | `build/decomp/fn/0021a088.c:19-49` |

The resulting pinned conversion is

```
road-units / (raw-speed-unit * frame)
= 32768 * 16 / (25 * 20971.51953125 * 216)
= 0.004623... .
```

Here `raw-speed-unit` is the `GetSpeedFromF15Vector` value before dividing by
the MPH divisor; it is **not** the native renderer's provisional `/100`
display state.  `SpeedSample` exposes both raw units/frame and MPH so the
coordinator can retain that open mapping.

## Scope and integration

`UpdateVehiclePhysics` applies gravity before calling collision/ground
resolution (`0021ba70.c:158-179`, `389-428`).  This standalone kernel expects
that resolver's result through `VehicleKinematics::airborne`; grounded motion
has vertical velocity cleared.  The remaining engine/gear/tire force comes
from `gs_controls_car_update` (`build/decomp/fn/0021a310.c:148-253`) and
`LoadCarPerformanceSpecs` (`build/decomp/fn/002180c0.c:19-71`) and must be
bound with actual selected part records at coordinator integration.

No ROM bytes are stored.  The test verifies the read-only ELF's scalar at
`0x00332f44` (`0x3fcdf3b6`) and FNV-1a function ranges:

| range | FNV-1a-64 |
|---|---:|
| `0x0021d358..0x0021d387` | `078c393c7af6d8d1` |
| `0x00249458..0x002494e7` | `0c0a7123de6d7f6a` |
| `0x0021ba70..0x0021c69b` | `c92f40e0033aa59a` |
