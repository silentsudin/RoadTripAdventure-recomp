#pragma once

// Data-only title / main-menu / options screens for the native preview.
// No SDL, no Vulkan, no drawing. Another owner composites `quads` then `text`.
//
// Calibrated against docs/native-reference/menu-calibration.md §4. Constants
// are in UI640 placement space: placement_x = scan_x / 2 and placement_y =
// scan_y / 0.93125 - 16. Do not use raw scanout y values here.
//
// GAP: the 3D title scene (cars and tilted plate art) is not a quad. A sky
// field plus a cream plate mark the card bounds only.
// GAP: the Enter icon is represented by a dark quad; its game-art glyph is not
// available to this data-only preview.
// GAP: handheldBoost is not applied (no SDL in this module).
// GAP: non-16:9 windows use the centered 640/480 frame rather than raw S
// times the 1080p design pixels, so a 4:3 field stays centered
// (ui-parity.md deviation 3). On 16:9 the two formulas match.

#include "preview/preview.h"

#include <algorithm>
#include <array>

namespace rt::native::preview {

struct UiScreen {
    std::vector<ScreenQuad> quads;
    std::vector<HudQuad> text;
};

// Theme.cpp:89. Desktop handheldBoost is 1 (Theme.cpp:84).
inline float uiDesignScale(int height) {
    return std::max(0.5f, static_cast<float>(height) / 1080.f);
}

// Theme.h:17-50, required identical by ui-parity.md:105-111.
inline constexpr std::array<float, 4> kUiPrompt{0x30 / 255.f, 0x40 / 255.f, 0x90 / 255.f, 0xF2 / 255.f};
inline constexpr std::array<float, 4> kUiPromptRimOuter{0x91 / 255.f, 0xD2 / 255.f, 0xE8 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiPromptRimInner{0x1C / 255.f, 0xA1 / 255.f, 0xE6 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiFrame{0xF8 / 255.f, 0x60 / 255.f, 0x18 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiSelectionTop{0x2C / 255.f, 0x6C / 255.f, 0xE8 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiSelectionBottom{0x08 / 255.f, 0x38 / 255.f, 0xB0 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiCapGold{0xF8 / 255.f, 0xC8 / 255.f, 0x38 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiCapShine{0xFF / 255.f, 0xF0 / 255.f, 0xA0 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiHorn{0xF2 / 255.f, 0xC2 / 255.f, 0x30 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiHornShade{0xC0 / 255.f, 0x8A / 255.f, 0x18 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiHornLine{0x4A / 255.f, 0x30 / 255.f, 0x00 / 255.f, 1.f};

inline constexpr int kMainMenuRowCount = 5;
inline constexpr int kOptionsRowCount = 4;

// Title > Options rows. nullptr when index is out of range.
const char* mainMenuRowLabel(int index);
const char* optionsRowLabel(int index);

UiScreen buildTitleScreen(int w, int h, bool pressVisible);
UiScreen buildMainMenu(int w, int h, int focusIndex);
UiScreen buildOptionsScreen(int w, int h, int focusIndex);

}  // namespace rt::native::preview
