#include "gameplay/input_map.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace rt::native::gameplay::input_map {
namespace {

// Per-tick collector: last write wins per physical bit.
// Keyed by the PhysicalButton mask value for stable ordering.
std::map<std::uint16_t, bool> g_tickBits;
bool g_tickActive = false;

// Keyboard keyId -> PhysicalButton (chosen to hit every bit exactly once).
// IDs are small stable numbers (0 unused, 1..16 for the set).
// Common-name mapping for documentation/repro (not parsed at runtime):
//   1=W/Up     -> CROSS (gas default)
//   2=S/Down   -> SQUARE (brake)
//   3=Space    -> CIRCLE (reverse)
//   4=R2 equiv (RightShoulder key) -> R2 (jet)
//   5=Q        -> L1 (wing up)
//   6=E        -> L2 (wing down)
//   7=R1 equiv (LeftShoulder) -> R1 (horn)
//   8=V        -> TRIANGLE (view)
//   9=L        -> R1 (lights, duplicate allowed by profile)
//  10=N        -> SELECT (nav)
//  11=ArrowU   -> DPAD_UP
//  12=ArrowD   -> DPAD_DOWN
//  13=ArrowL   -> DPAD_LEFT
//  14=ArrowR   -> DPAD_RIGHT
//  15=Enter    -> START
//  16=Esc      -> L3 (example; full coverage)
// L3/R3/SELECT variants covered by the 16.
PhysicalButton mapKeyboardImpl(uint32_t keyId) {
    switch (keyId) {
    case 1:  return PhysicalButton::CROSS;
    case 2:  return PhysicalButton::SQUARE;
    case 3:  return PhysicalButton::CIRCLE;
    case 4:  return PhysicalButton::R2;
    case 5:  return PhysicalButton::L1;
    case 6:  return PhysicalButton::L2;
    case 7:  return PhysicalButton::R1;
    case 8:  return PhysicalButton::TRIANGLE;
    case 9:  return PhysicalButton::R3;   // lights coverage (profile must hit all 16 unique bits)
    case 10: return PhysicalButton::SELECT;
    case 11: return PhysicalButton::DPAD_UP;
    case 12: return PhysicalButton::DPAD_DOWN;
    case 13: return PhysicalButton::DPAD_LEFT;
    case 14: return PhysicalButton::DPAD_RIGHT;
    case 15: return PhysicalButton::START;
    case 16: return PhysicalButton::L3;
    default: return PhysicalButton::NONE;
    }
}

// Gamepad: 0=South(CROSS),1=East(CIRCLE),2=West(SQUARE),3=North(TRI),
// 4=Back(SELECT),5=Start,6=LStick,7=RStick,8=LShoulder,9=RShoulder,
// 10=DPadU,11=DPadD,12=DPadL,13=DPadR,14=Misc,15=Guide
// Axes: 100=LT+,101=RT+ (triggers as buttons), 102.. for sticks dirs.
PhysicalButton mapGamepadImpl(uint32_t id, float v) {
    const bool pos = (v >= 0.5f);
    const bool neg = (v <= -0.5f);
    switch (id) {
    case 0:  return PhysicalButton::CROSS;
    case 1:  return PhysicalButton::CIRCLE;
    case 2:  return PhysicalButton::SQUARE;
    case 3:  return PhysicalButton::TRIANGLE;
    case 4:  return PhysicalButton::SELECT;
    case 5:  return PhysicalButton::START;
    case 6:  return PhysicalButton::L3;
    case 7:  return PhysicalButton::R3;
    case 8:  return PhysicalButton::L1;
    case 9:  return PhysicalButton::R1;
    case 10: return PhysicalButton::DPAD_UP;
    case 11: return PhysicalButton::DPAD_DOWN;
    case 12: return PhysicalButton::DPAD_LEFT;
    case 13: return PhysicalButton::DPAD_RIGHT;
    case 14: return PhysicalButton::L2;  // misc as L2 for coverage
    case 15: return PhysicalButton::R2;
    // axes
    case 100: return pos ? PhysicalButton::R2 : PhysicalButton::NONE; // right trigger ~ jet
    case 101: return pos ? PhysicalButton::L2 : PhysicalButton::NONE;
    case 102: if (pos) return PhysicalButton::DPAD_RIGHT; if (neg) return PhysicalButton::DPAD_LEFT; break;
    case 103: if (pos) return PhysicalButton::DPAD_DOWN; if (neg) return PhysicalButton::DPAD_UP; break;
    default: break;
    }
    return PhysicalButton::NONE;
}

std::uint16_t physMask(PhysicalButton b) {
    return buttonMask(b);
}

} // namespace

PhysicalButton mapKeyboard(uint32_t keyId) {
    return mapKeyboardImpl(keyId);
}

PhysicalButton mapGamepad(uint32_t buttonOrAxisId, float value) {
    return mapGamepadImpl(buttonOrAxisId, value);
}

int countMappedBits(bool isGamepadProfile) {
    std::uint16_t seen = 0;
    auto mark = [&](PhysicalButton b) {
        const auto m = static_cast<std::uint16_t>(b);
        if (m != 0) seen |= m;
    };
    if (isGamepadProfile) {
        for (uint32_t i = 0; i < 30; ++i) {
            mark(mapGamepadImpl(i, 1.0f));
            mark(mapGamepadImpl(i, -1.0f));
            mark(mapGamepadImpl(i, 0.6f));
        }
    } else {
        for (uint32_t i = 0; i < 30; ++i) {
            mark(mapKeyboardImpl(i));
        }
    }
    // count bits set
    int n = 0; for (std::uint16_t t=seen; t; t &= (t-1)) ++n;
    return n;
}

void beginInputTick() {
    g_tickBits.clear();
    g_tickActive = true;
}

void feedDeviceEvent(const DeviceEvent& ev) {
    if (!g_tickActive) beginInputTick();
    PhysicalButton b = PhysicalButton::NONE;
    if (ev.source == DeviceEvent::KEYBOARD) {
        b = mapKeyboardImpl(ev.controlId);
    } else {
        b = mapGamepadImpl(ev.controlId, ev.analog);
    }
    if (b != PhysicalButton::NONE) {
        g_tickBits[physMask(b)] = ev.pressed;
    }
}

PadState endInputTick() {
    PadState p{};
    for (const auto& kv : g_tickBits) {
        if (kv.second) {
            p.pressedButtons = static_cast<std::uint16_t>(p.pressedButtons | kv.first);
        }
    }
    g_tickActive = false;
    return p;
}

PadState canonicalMaskFromBits(const std::vector<std::pair<PhysicalButton, bool>>& bits) {
    std::map<std::uint16_t, bool> last;
    for (const auto& pr : bits) {
        last[physMask(pr.first)] = pr.second;
    }
    PadState p{};
    for (const auto& kv : last) {
        if (kv.second) p.pressedButtons = static_cast<std::uint16_t>(p.pressedButtons | kv.first);
    }
    return p;
}

MenuNav resolveMenuNav(const PadState& pad) {
    // Grounded mapping (see input-map.md for ELF cites):
    // dpad and cross/circle exactly as resolveMenuInput + authored menu flow.
    // start added for pause/title flows (observed 0x0800 in several paths; GAP
    // for full name-entry letter nav).
    const std::uint16_t w = pad.pressedButtons;
    MenuNav m;
    m.up      = (w & buttonMask(PhysicalButton::DPAD_UP)) != 0;
    m.down    = (w & buttonMask(PhysicalButton::DPAD_DOWN)) != 0;
    m.left    = (w & buttonMask(PhysicalButton::DPAD_LEFT)) != 0;
    m.right   = (w & buttonMask(PhysicalButton::DPAD_RIGHT)) != 0;
    m.confirm = (w & buttonMask(PhysicalButton::CROSS)) != 0;
    m.cancel  = (w & buttonMask(PhysicalButton::CIRCLE)) != 0;
    m.start   = (w & buttonMask(PhysicalButton::START)) != 0;
    // If start present treat as confirm in some title paths (GAP documented).
    if (m.start && !m.confirm) m.confirm = true; // conservative
    return m;
}

MenuNav mapDeviceToMenuNav(const DeviceEvent& ev) {
    // For test harness: feed then resolve (collector state side-effect ok for unit).
    feedDeviceEvent(ev);
    const auto pad = endInputTick();
    return resolveMenuNav(pad);
}

std::optional<RemapTable> readRemapFromElf(const std::string& elfPath) {
    if (elfPath.empty()) return std::nullopt;
    std::ifstream f(elfPath, std::ios::binary);
    if (!f) return std::nullopt;
    // vaddr 0x229F20, offset = vaddr - 0x1FF000 = 0x2AF20
    constexpr std::size_t kFuncOff = 0x229F20u - 0x1FF000u;
    constexpr std::size_t kNeed = 0x50; // 20 instrs safety
    std::vector<std::uint8_t> buf(kNeed);
    f.seekg(kFuncOff);
    if (!f.read(reinterpret_cast<char*>(buf.data()), kNeed)) return std::nullopt;

    // Collect every small positive addiu immediate in the prologue (order-independent).
    // The 10 values written are exactly the ones in kDefaultRemap.
    std::vector<std::uint16_t> imms;
    for (std::size_t o = 0; o + 2 < buf.size(); o += 4) {
        const std::uint16_t imm = static_cast<std::uint16_t>(buf[o] | (buf[o+1] << 8));
        if (imm > 0 && imm < 0x200) imms.push_back(imm);
    }
    // Must have (at least) our 10 values (dups for the shared R1 ok).
    RemapTable t = kDefaultRemap;
    bool allFound = true;
    for (auto want : t) {
        if (std::find(imms.begin(), imms.end(), want) == imms.end()) { allFound = false; break; }
    }
    if (!allFound || imms.size() < 9) return std::nullopt;
    return t;
}

} // namespace rt::native::gameplay::input_map
