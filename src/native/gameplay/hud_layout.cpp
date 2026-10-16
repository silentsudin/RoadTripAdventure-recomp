// Authored HUD layout constants — implementation.
//
// Everything geometric lives as constexpr tables in hud_layout.h (the shipped
// ROM values need no runtime computation). This translation unit exists so the
// staged library source gameplay/hud_layout.cpp (src/native/CMakeLists.txt
// staged-source list) contributes a real object file and so the lookups that
// would bloat the header with <cstring> stay out of it.
//
// Evidence: docs/native-reference/hud-layout.md (this module's ledger),
// docs/native-2d.md §1-§3 (640x448 UI space, arena sprite records), and the
// Ghidra session on SLUS_203.98: raceHudDraw 0x233860 and its element
// builders 0x22EE70..0x231F28, the drive-HUD dispatcher 0x2490A0 with jump
// table 0x2A8FB0 and default handler 0x24E1A0, the town map task 0x242FE8
// with bezel builder 0x249208, and the text/plate helpers 0x202D10/0x202350/
// 0x2028B0/0x203240.

#include "hud_layout.h"

namespace rt::native::gameplay {

const HudElementSpec* findHudElement(HudElement id) {
    for (std::size_t i = 0; i < kHudElementCount; ++i) {
        if (kHudElements[i].id == id) {
            return &kHudElements[i];
        }
    }
    return nullptr;
}

}  // namespace rt::native::gameplay
