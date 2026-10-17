#pragma once

// Data-only title / main-menu / options screens for the native preview.
// No SDL, no Vulkan, no drawing. Another owner composites `quads` then `text`.
//
// Measured against the untracked oracle frames
// build/scratch/oracle-golden/{title,main_menu,options}.png (1280x447 GS
// scanout, 4x SSAA of the interlaced field; oracle-golden-frames.md:109).
// Horizontal is 2x the 640-wide UI field; vertical is 447 vs 448 (<1 px, T1),
// so ui_x = scan_x / 2 and ui_y = scan_y. Placement then uses the centered
// 640x480 frame (ui-parity.md:9, race_hud.h:103-108). On a 16:9 window that
// scale equals uiDesignScale(h) * (1080/480), which is the S rule in
// Theme.cpp:89 (handheldBoost = 1, Theme.cpp:84).
//
// Scanout measurements (bright cyan rim, body median, selection caps):
//
//   | Element                         | scanout px              | UI 640x448        |
//   |---------------------------------|-------------------------|-------------------|
//   | Main menu rim                   | x 866..1189, y 268..400 | 433, 268, 162x133 |
//   | Options rim                     | x 456..843,  y 168..276 | 228, 168, 194x109 |
//   | Row pitch                       | 24                      | 24                |
//   | Selection inset below rim       | 6                       | 6                 |
//   | Selection cap span              | y+6 .. y+30 (25 tall)   | height 24 (T1)    |
//   | Label ink inset from panel left | 58                      | 29                |
//   | Label ink height                | 16..19                  | FONTE cell * 0.75 |
//   | Body fill                       | (48,64,144) exact       | Prompt 304090     |
//   | Rim sample                      | (26,162,231)            | 1CA1E6 within T1  |
//   | Selection interior sample       | (0,110,224)             | GAP vs 2C6CE8     |
//   | Gold cap sample                 | (232,146,11)            | GAP vs F8C838     |
//   | Label ink                       | ~F1F1F1 white           | GAP vs F6E68F     |
//   | Plate AABB                      | x 114..1099, y 38..219  | 57, 38, 493x181   |
//   | PRESS START ink band            | y 236..257, centered    | y 240, centered   |
//   | Copyright ink                   | x 904..1227, y 422..429 | small FONTE scale |
//   | Sky sample (100,10)             | (43,154,216)            | backdrop stand-in |
//
// 1080p (S=1, 1920x1080, offX=240, uniform 2.25) landing of the main rim:
//   x = 240 + 433*2.25 = 1214.25, y = (268+16)*2.25 = 639, 364.5 x 299.25.
//
// GAP: the 3D title scene (cars, tilted plate art, yellow stripe) is not a
// quad. A sky field plus a cream plate mark the card bounds only.
// GAP: "button Enter" / "button Exit" are the oracle keyboard prompt
// (oracle-golden-frames.md:107), not game art, and are not drawn.
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
