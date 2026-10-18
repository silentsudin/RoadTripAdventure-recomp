# Input mapping — device to PS2 action masks and menu navigation

W2-12 module `src/native/gameplay/input_map.{h,cpp}`.

## 1. Scope and grounding

- Device (keyboard / gamepad / synthetic) → 16 DualShock physical button bits.
- The 10 driving action remap table (exactly the u16 values at save +0x12E8 and action_map 0x1780A48 in action order: gas, brake, reverse, jet, wingUp, wingDown, horn, view, lights, navigator).
- Menu navigation semantics (up/down/left/right/confirm/cancel/start) derived from fixed physical bits (independent of driving remap).
- Deterministic last-write-wins collector for a tick (order of events from SDL pump or test socket does not matter).
- Pure, no SDL headers, no guest RAM, no ROM bytes in the binary.

All facts are from:
- ELF instruction stream at `FUN_00229F20` (InitTuningConstants) decoded via decomp + direct vaddr-0x1FF000 reads (see test).
- `ReadPadState 0x20A8A8`, `UpdateControllerInput 0x20AB10`, `UpdateMenuInputState 0x20A660`.
- `HandleNumberSpinner 0x23A320` (0x1000 up, 0x4000 down, 0x0040 confirm).
- `AcquireVehicleInput 0x21A990` + action_map layout in `car-physics.md`.
- `resolveMenuInput` / `ControllerConfig` authored contract (menu.h).
- Test-socket `TestPad` (active-low guest format, host `PadState` is active-high).

Rumble-haptics.md documents the same pad status reader family.

## 2. Remap table (defaults)

Exactly 10 u16 written by 0x229F20:

```
0x0040, 0x0080, 0x0020, 0x0002, 0x0004, 0x0001, 0x0008, 0x0010, 0x0008, 0x0100
```

Corresponding to:

- gas: CROSS (0x40)
- brake: SQUARE (0x80)
- reverse: CIRCLE (0x20)
- jet: R2 (0x02)
- wing up: L1 (0x04)
- wing down: L2 (0x01)
- horn: R1 (0x08)
- view: TRIANGLE (0x10)
- lights: R1 (0x08)   // documented shared alias with horn
- navigator: SELECT (0x100)

The single allowed duplicate (horn/lights) is accepted by `ControllerConfig::setButtonConfig`; all other duplicates are conflicts.

`input_map::kDefaultRemap` / `defaultRemap()` is the source of truth for the native rebuild (matches the ELF immediates on every supported disc).

## 3. Device profiles

Two fixed profiles (keyboard + gamepad). Each maps a set of device control IDs to the 16 distinct physical bits (0 conflicts within a profile).

Keyboard IDs (stable small integers, documented for repro):
- 1 → CROSS, 2 → SQUARE, 3 → CIRCLE, 4 → R2, 5 → L1, 6 → L2,
- 7 → R1, 8 → TRIANGLE, 9 → R1 (lights), 10 → SELECT,
- 11 → DPAD_UP, 12 → DPAD_DOWN, 13 → DPAD_LEFT, 14 → DPAD_RIGHT,
- 15 → START, 16 → L3

Gamepad IDs follow SDL_GamepadButton numbering + axis conventions (south=0, triggers as 100/101, stick dirs as 102+).

All 16 bits are covered by each profile (test asserts `countMappedBits(kb) == 16 && countMappedBits(gp) == 16`).

## 4. Deterministic tick collector

`beginInputTick()` / `feedDeviceEvent(ev)` / `endInputTick()`.

- Multiple events for the same physical bit: last value wins.
- 10 000 shuffles of any same-tick bag always produce byte-identical `PadState`.
- Pure helper `canonicalMaskFromBits` for tests (sorts by mask, last wins).

Used by coordinator integration to replace ad-hoc keyboard + `setPressed` + test-socket injection.

## 5. Menu navigation (grounded + GAPs)

`resolveMenuNav(const PadState&)` → `MenuNav` (up/down/left/right/confirm/cancel/start).

Fixed physical bits (not the driving remap):

| nav      | physical | mask   | ELF evidence                              | status |
|----------|----------|--------|-------------------------------------------|--------|
| up       | DPAD_UP  | 0x1000 | HandleNumberSpinner, menu flow            | grounded |
| down     | DPAD_DOWN| 0x4000 | same + spinner                            | grounded |
| left     | DPAD_LEFT| 0x8000 | placement editor + list nav               | grounded |
| right    | DPAD_RIGHT|0x2000 | same                                      | grounded |
| confirm  | CROSS    | 0x0040 | spinner (0x30 & 0x40), title/mode flows   | grounded |
| cancel   | CIRCLE   | 0x0020 | pause/menu flow (input.cancel)            | grounded |
| start    | START    | 0x0800 | pause entry, some title confirms (also used as secondary confirm) | grounded |

**GAPs marked:**
- Name-entry letter grid navigation (0x25B908) uses D-pad + confirm; full adjacency table not re-derived here.
- Some config sub-screens accept SELECT as cancel (observed in decomp but not universal).
- Start button semantics for attract/demo exit are indirect (via game_mode).

`resolveMenuInput` in menu.cpp is the prior contract; `input_map::resolveMenuNav` is a strict superset (adds start) for the rebuild.

## 6. Usage (host side)

```cpp
#include "gameplay/input_map.h"
using namespace rt::native::gameplay::input_map;

// per frame (after SDL event pump or test socket)
beginInputTick();
feedDeviceEvent({DeviceEvent::KEYBOARD, 1 /*W*/, true});
feedDeviceEvent({DeviceEvent::GAMEPAD, 9, true}); // R1
auto pad = endInputTick();

// driving
auto cfg = ...; // ControllerConfig with remap or load from SaveData
auto carIn = playerCarInput(pad, axes, cfg);

// menu
auto m = resolveMenuNav(pad);
if (m.confirm) ...
```

Remap persistence:
```cpp
SaveData s; s.controllerRemap = defaultRemap();
ControllerConfig c; loadControllerConfig(c, s);
```

## 7. Tests

`tests/native_w2/input_map_test.cpp` (w2_input_map_test).

- default_remap=10/10 (authored + ELF decode match)
- conflicts=0
- bits_mapped=16/16 (kb and gp profiles)
- roundtrip_save=1000/1000 (SaveData roundtrips)
- order_deterministic=1 (10k shuffles)
- menu nav spot checks
- NO_ELF path prints NO_ELF and exits 0 (pure tests still run)

Repro (with ELF):
```
cmake -S . -B build/w-W2-12 -DRT_NATIVE_ASSETS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build/w-W2-12 --target w2_input_map_test -j
build/w-W2-12/w2_input_map_test --elf "$RT_ELF"
```

The 10 constants are reproducible from the addiu immediates immediately preceding the `sh` stores at vaddr 0x229F20 (file offset 0x2AF20).

## 8. Open questions / future

- Full analog stick → steering + wing axis mapping lives in playerCarInput + AcquireVehicleInput (not in this batch).
- Per-device profile overrides (e.g. "xbox family") are a coordinator + wave-3 concern.
- Touch / mouse nav for second-screen is out of scope.
