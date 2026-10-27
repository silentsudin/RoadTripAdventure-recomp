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
#include "preview/ui_theme.h"
#include "gameplay/settings.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

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
inline constexpr std::array<float, 4> kUiPrompt = theme::rgba(theme::kPrompt);
inline constexpr std::array<float, 4> kUiPromptRimOuter = theme::rgba(theme::kPromptRimOuter);
inline constexpr std::array<float, 4> kUiPromptRimInner = theme::rgba(theme::kPromptRimInner);
inline constexpr std::array<float, 4> kUiFrame = theme::rgba(theme::kFrame);
inline constexpr std::array<float, 4> kUiSelectionTop = theme::rgba(theme::kSelectionTop);
inline constexpr std::array<float, 4> kUiSelectionBottom = theme::rgba(theme::kSelectionBottom);
inline constexpr std::array<float, 4> kUiCapGold = theme::rgba(theme::kCapGold);
inline constexpr std::array<float, 4> kUiCapShine{0xFF / 255.f, 0xF0 / 255.f, 0xA0 / 255.f, 1.f};
inline constexpr std::array<float, 4> kUiHorn = theme::rgba(theme::kHorn);
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

// ---------------------------------------------------------------------------
// Recomp-parity Pause panel (ui-parity.md "Pause Options" / "Controllers").
// One headed list: non-focusable Heading/Note rows, selectable Item rows with a
// right-aligned value. Geometry is viewport-relative (S = uiDesignScale(h)):
// 880*S wide (capped 92%), rows 80*S, pad 30*S, margin 24*S, name tab above.
enum class PanelRowKind { Heading, Item, Note };
struct PanelRow {
    PanelRowKind kind = PanelRowKind::Item;
    std::string label;
    std::string value;
    bool enabled = true;
};
struct PausePanel {
    std::string title;
    std::vector<PanelRow> rows;
    int focus = 0;       // row index of an Item
    bool wide = true;    // 880*S, else the 520*S root width
};
UiScreen buildPausePanelScreen(int w, int h, const PausePanel& panel);

// Options + Controllers children driven by OptionsModel / SettingsStore.
struct OptionsPanelState {
    bool controllers = false;  // false: the Display/Sound/Graphics page
    int focus = -1;            // row index; -1 = first selectable row
};
PausePanel buildOptionsPanel(const gameplay::OptionsModel& model, const gameplay::SettingsStore& store,
                             const gameplay::MenuContext& context, OptionsPanelState& state);
// Return true when something changed. Adjust/confirm persist via store.save().
bool optionsPanelMove(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                      const gameplay::MenuContext& context, int direction);
bool optionsPanelAdjust(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                        const gameplay::MenuContext& context, int direction);
// Confirm on the Controllers link row opens the child page.
bool optionsPanelConfirm(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                         const gameplay::MenuContext& context);
// Controllers -> Options. False when already on Options (caller leaves the screen).
bool optionsPanelBack(OptionsPanelState& state);

}  // namespace rt::native::preview
