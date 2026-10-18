#pragma once
// SDL-agnostic device-to-PS2 input mapper for the native rebuild.
// Maps keyboard/gamepad controls -> DualShock physical button bits (the
// 16-bit word consumed by PadState / ReadPadState / UpdateControllerInput).
// Remap table is the 10 u16 action->button-mask values (action_map order
// at 0x1780A48, persisted at save +0x12E8) exactly as written by
// FUN_00229F20 / InitTuningConstants.
// Menu navigation (up/down/left/right/confirm/cancel/start) is fixed to
// the documented physical bits (grounded below); not affected by driving remap.
// Deterministic per-tick: last-write-wins on a bit wins regardless of
// input event arrival order (shuffle-proof collector).
// Pure; no SDL, no guest memory, no ROM bytes.

#include "gameplay/menu.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace rt::native::gameplay::input_map {

// ---------------------------------------------------------------------------
// Default action remap (10 u16 masks in ControllerAction order).
// Grounded in ELF 0x229F20 (InitTuningConstants) stores:
//   0x40 (gas/CROSS), 0x80 (brake/SQUARE), 0x20 (rev/CIRCLE), 0x02 (jet/R2),
//   0x04 (wingUp/L1), 0x01 (wingDn/L2), 0x08 (horn/R1), 0x10 (view/TRI),
//   0x08 (lights/R1), 0x100 (nav/SELECT).
// Matches the authored defaults in ControllerConfig::resetDefaults().
// ---------------------------------------------------------------------------
inline constexpr std::array<std::uint16_t, kControllerActionCount> kDefaultRemap{
    buttonMask(PhysicalButton::CROSS),    // ACCELERATE
    buttonMask(PhysicalButton::SQUARE),   // BRAKE
    buttonMask(PhysicalButton::CIRCLE),   // REVERSE
    buttonMask(PhysicalButton::R2),       // JET
    buttonMask(PhysicalButton::L1),       // WING_UP
    buttonMask(PhysicalButton::L2),       // WING_DOWN
    buttonMask(PhysicalButton::R1),       // HORN
    buttonMask(PhysicalButton::TRIANGLE), // VIEW
    buttonMask(PhysicalButton::R1),       // LIGHTS
    buttonMask(PhysicalButton::SELECT)    // NAVIGATOR
};

// Serializable remap table (exactly SaveData.controllerRemap layout).
using RemapTable = std::array<std::uint16_t, kControllerActionCount>;

// Return the pinned defaults (authoritative for no-ELF runs and roundtrips).
inline const RemapTable& defaultRemap() { return kDefaultRemap; }

// ---------------------------------------------------------------------------
// Device profiles (SDL-agnostic). Each provides a mapping from a device
// control identifier to a unique PhysicalButton bit. Profiles are chosen
// so that all 16 physical bits are reachable with zero conflicting
// assignments within a profile.
// Keyboard: uses small positive integers (treat as "scancode-like" IDs;
//            documented below with common names for repro).
// Gamepad: uses small IDs (0=South, 1=East, ... + axis directions as +100).
// The mappings are the canonical "standard" layout for this rebuild.
// ---------------------------------------------------------------------------

// Keyboard profile. Returns the PhysicalButton emulated by the given key ID,
// or NONE if unbound. 16 distinct keys map to the 16 bits.
PhysicalButton mapKeyboard(uint32_t keyId);

// Gamepad profile. buttonOrAxisId: 0.. for buttons (South=0,East=1,...),
// or 100+dir for axes (e.g. 100=LeftX+, 101=LeftX-, ...). Value for analog
// (positive for + side). Returns the emulated bit or NONE.
PhysicalButton mapGamepad(uint32_t buttonOrAxisId, float value = 0.f);

// Return true if the profile (kb or gamepad) maps at least N distinct
// non-NONE physical bits (used by tests for bits_mapped and conflicts).
int countMappedBits(bool isGamepadProfile);

// ---------------------------------------------------------------------------
// Deterministic per-tick collector.
// Events may arrive in any order; final mask for each physical bit is the
// last write seen for that bit (last-write-wins). Shuffles of the same
// set always yield identical PadState.
// Analog stick axes can be fed as pairs that synthesize dpad or direct bits
// (profile decides).
// ---------------------------------------------------------------------------
struct DeviceEvent {
    enum Source : uint8_t { KEYBOARD = 0, GAMEPAD = 1 };
    Source source = KEYBOARD;
    uint32_t controlId = 0;  // keyId or buttonOrAxisId
    bool pressed = false;
    float analog = 0.f;      // for axes; profile interprets
};

void beginInputTick();  // clear collector state for new 60 Hz tick

// Feed one event (may be called from SDL pump or test harness in any order).
void feedDeviceEvent(const DeviceEvent& ev);

// Finalize: apply last-wins rules, return the PadState (active-high bits).
// Repeated calls without new feeds return the same.
PadState endInputTick();

// Pure helper: given an unordered bag of (physical, down) pairs, produce
// the canonical mask using last-wins on duplicate physicals (by enum order).
PadState canonicalMaskFromBits(const std::vector<std::pair<PhysicalButton, bool>>& bits);

// ---------------------------------------------------------------------------
// Menu navigation semantics (fixed, independent of driving remap).
// Grounded in ELF pad handlers + menu tasks (UpdateMenuInputState,
// HandleNumberSpinner 0x23A320, spinner up/down=0x1000/0x4000, confirm=0x40;
// title/mode/pause flows; race placement 0x2000/0x8000 x 0x1000/0x4000 y).
// start = 0x0800 (START) observed for pause/confirm in several flows (GAP
// for some name-entry paths).
// cancel often CIRCLE 0x0020 or SELECT in config; we use the authored
// resolveMenuInput contract for compatibility.
// ---------------------------------------------------------------------------
struct MenuNav {
    bool up = false;      // DPAD_UP  0x1000
    bool down = false;    // DPAD_DOWN 0x4000
    bool left = false;    // DPAD_LEFT 0x8000
    bool right = false;   // DPAD_RIGHT 0x2000
    bool confirm = false; // CROSS 0x0040 (primary), START 0x0800 secondary in some
    bool cancel = false;  // CIRCLE 0x0020 (primary)
    bool start = false;   // START 0x0800
};

// Map a raw PadState (post device map) to menu nav. Mirrors resolveMenuInput
// for dpad/confirm/cancel; adds start. Grounding notes in input-map.md.
MenuNav resolveMenuNav(const PadState& pad);

// Convenience: feed a device event and immediately resolve menu nav for it
// (for tests; still respects tick collector if between begin/end).
MenuNav mapDeviceToMenuNav(const DeviceEvent& ev);

// ---------------------------------------------------------------------------
// ELF reader (read-only, for test verification of defaults only).
// offset = vaddr - 0x1FF000. Returns the 10 values if the stores at
// 0x229F20 decode to exactly the expected immediates; std::nullopt on
// missing/short file or parse mismatch. Never embeds ROM bytes.
// ---------------------------------------------------------------------------
std::optional<RemapTable> readRemapFromElf(const std::string& elfPath);

} // namespace rt::native::gameplay::input_map
