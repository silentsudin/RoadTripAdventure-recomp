# Tachometer RPM producer (`car+0x1D0`)

This traces the signed tachometer halfword consumed by `CalculateTachometerValue` at `0x00249520`. It is not a fit to the MPH HUD. Addresses are SLUS-203.98 EE virtual addresses. Tests map them through ELF32 `PT_LOAD` entries (`e_phoff=0x34`, entry size 32, count 3), rather than assuming a flat ELF base.

## State and relation to motion

The physics-car pointer is the base for all offsets below:

| offset | type | ROM evidence / meaning |
|---:|---|---|
| `0x1BC` | `s32` | gear-coupled driveline/arc state; written `(rpm * ratio) >> 7`, `0x2193E4..0x2193FC` |
| `0x1D0` | `s16` | tach RPM, clamped `0..10000`, `0x2193C0..0x2193D4` |
| `0x1FF` | `s8` | selected gear; ratio index |
| `0x21C..0x228` | seven `s16` | loaded gearbox row; selected ratio is `car+0x22C + 2*gear` at `0x218F88..0x218F98` |
| `0x214` / `0x218` | `s32` | engine acceleration / response parameters consumed by the RPM integrator |

`gs_controls_car_update` (`0x21A310`) passes its local longitudinal motion (`param_3[2]`) to `AdvanceVehicleMotion` as argument 3 (decompile lines 153--155). Surface/drive state, a throttle-derived value, flags, and an active-motion boolean are passed separately. Hence this argument is **not** the speed HUD word: the HUD independently displays `floor(speed_word / 55.07)` (`gauge-calibration.md`).

Both gear selectors project this longitudinal value through the selected ratio:

```text
projected_rpm = trunc_toward_zero(longitudinal_motion * 128 / gear_ratio)
```

The instructions are `sll a0,a1,7; div a0,ratio` at `0x21CE88..0x21CEA0` and `0x21CFC0..0x21CFD8`. The reciprocal relation after the motion update is `car+0x1BC = (car+0x1D0 * gear_ratio) >> 7`. `rpmFromMotionState` publishes this useful forward-range projection for native physics.

## Gear rows and shift policy

`LoadCarPerformanceSpecs` derives its gearbox-table address from `0x2ED334` (`addiu t5,v0,-0x3584` at `0x2180CC`) and eventually copies the selected seven-halfword row to the car using records of stride `0x1C`. Numeric table:

| gearbox | R | 1 | 2 | 3 | 4 | 5 | 6 |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | -95 | 116 | 162 | 227 | 318 | 446 | 0 |
| 1 | -95 | 116 | 182 | 291 | 408 | 490 | 0 |
| 2 | -95 | 128 | 220 | 276 | 387 | 464 | 557 |
| 3 | -95 | 128 | 260 | 327 | 458 | 550 | 660 |
| 4 | -95 | 144 | 300 | 414 | 539 | 647 | 711 |
| 5 | -95 | 156 | 350 | 446 | 550 | 625 | 750 |

`SelectScene3dHandler` (`0x21CE08`) selects either policy below. Automatic downshift needs both the current projected RPM and the prospective lower-gear RPM to meet its limits.

| selector | automatic upshift | automatic downshift |
|---|---:|---|
| `0x21CE48` | >= **8201** (`slti 0x2009`, `0x21CEA4`) | current < **5000**, lower gear < **8000** (`0x21CF00`, `0x21CF20`) |
| `0x21CF80` | >= **5001** (`slti 0x1389`, `0x21CFDC`) | current < **3000**, lower gear < **4000** (`0x21D038`, `0x21D058`) |

The semantic flag selecting those two policies is not named/proven here, but the two address-level threshold laws are.

## Integrator, idle, and redline

`AdvanceVehicleMotion` (`0x218EE0..0x2194B3`) owns the write. It loads prior RPM (`lh`, `0x218F24`), selects the gear ratio, then integrates a bounded delta from longitudinal and lateral motion, `car+0x214`, `car+0x218`, surface/active state, and throttle. It stores the result at `0x2193C0`, caps it at **10000** (`addiu v0,0x2710`, `0x2193C4..0x2193C8`), then caps negative values at zero (`0x2193CC..0x2193D4`).

There is **no nonzero idle-RPM floor**: `ResetVehicleState` (`0x21C998`) stores zero into `car+0x1D0`. Thus this gauge's idle is zero without an active coupling state; a near-zero screenshot is not evidence for an idle constant. The only RPM target literals in this function are **3000** and **9000** at `0x218F70` and `0x218F78`, in its flag path when `rpm < 9501` (`0x218F34`, `0x251D`).

## Oracle calibration and GAP

The supplied observations remain: coast at **39 mph** is about **117 RPM**; driving at **63 mph** is about **6217 RPM**. Neither screenshot records the selected gearbox, longitudinal-motion argument, or integrator history, so an MPH-only RPM law is underdetermined and intentionally absent.

For ratio 116, the direct integer projection produces `rpmFromMotionState(106,116) = 116` (residual **-1 RPM** from ~117) and `rpmFromMotionState(5634,116) = 6216` (residual **-1 RPM** from ~6217). These are coupling-domain checks, not assertions that 106/5634 derive from 39/63 MPH; the one-RPM delta is integer truncation plus approximate needle reading.

**GAP:** no captured car state links either oracle frame's `speed_word` to the longitudinal argument, selected gearbox, throttle/surface inputs, or prior RPM. Static ROM evidence proves the integrator inputs and ratio projection, but a live trace is needed for frame-exact transient replay.
