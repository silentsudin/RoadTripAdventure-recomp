// Title, main menu, and Options screen builders. Pure data: ScreenQuad chrome
// plus FONTE HudQuads. See ui_menu.h for the golden-frame measurement table.
#include "preview/ui_menu.h"

#include "assets/glyph_atlas.h"
#include "gameplay/hud_layout.h"

#include <algorithm>

namespace rt::native::preview {
namespace {

using gameplay::kFrameHeight;
using gameplay::kFrameWidth;
using gameplay::kFrameYOffset;

// Main-menu values are UI640 placement values from menu-calibration.md §4.
constexpr float kMainX = 433.f;
constexpr float kMainY = 272.f;
constexpr float kMainW = 162.f;
constexpr float kMainH = 143.f;
constexpr float kOptX = 228.f;
constexpr float kOptY = 168.f;
constexpr float kOptW = 194.f;
constexpr float kOptH = 109.f;
constexpr float kRowPitch = 25.8f;
constexpr float kRowInsetY = 6.f;
constexpr float kRowH = 26.5f;
constexpr float kTextInsetX = 29.f; // 58 scanout px / 2
constexpr float kSelectedTextInsetX = 23.f;
constexpr float kRim = 2.f;         // 4 scanout px of bright rim / 2
constexpr float kMenuGlyphScaleX = 0.77f;
constexpr float kMenuGlyphScaleY = 1.0f;
constexpr float kSmallGlyphScaleX = 0.40f;
constexpr float kSmallGlyphScaleY = 0.44f;
constexpr float kHornHeight = 24.f;

constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, 1.f};
// main_menu.png audit (1280x447): selected row x=870..1185, y=274..297.
// Exact flat samples: top (920,278)=(0,110,224), lower (920,279)=(0,102,224),
// edge (920,275)=(211,93,32), and gold highlight (920,274)=(232,146,11).
constexpr std::array<float, 4> kMenuSelectionFrame{211.f / 255.f, 93.f / 255.f, 32.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuSelectionTop{0.f, 110.f / 255.f, 224.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuSelectionMiddle{2.f / 255.f, 88.f / 255.f, 190.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuSelectionBottom{10.f / 255.f, 60.f / 255.f, 150.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuCapGold{232.f / 255.f, 146.f / 255.f, 11.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuCapShine{246.f / 255.f, 242.f / 255.f, 224.f / 255.f, 1.f};
// Main-menu label cores: selected Adventure at (972,280) is (244,244,244);
// unselected Quick Race at (1068,309) is (246,246,246). The panel's exact
// body sample is (48,64,144) at (900,305); kUiPrompt retains its documented
// alpha F2 (the oracle PNG is composited RGB, so alpha cannot be read back).
constexpr std::array<float, 4> kMenuInk{246.f / 255.f, 246.f / 255.f, 246.f / 255.f, 1.f};
constexpr std::array<float, 4> kMenuSelectedInk{244.f / 255.f, 244.f / 255.f, 244.f / 255.f, 1.f};
// title.png (100, 10). GAP: flat stand-in for the 3D sky.
constexpr std::array<float, 4> kSky{43.f / 255.f, 154.f / 255.f, 216.f / 255.f, 1.f};
constexpr std::array<float, 4> kBandNavy{7.f / 255.f, 13.f / 255.f, 93.f / 255.f, 1.f};
constexpr std::array<float, 4> kBandStripe{244.f / 255.f, 193.f / 255.f, 10.f / 255.f, 1.f};
constexpr std::array<float, 4> kBandStripeEdge{211.f / 255.f, 166.f / 255.f, 10.f / 255.f, 1.f};
constexpr std::array<float, 4> kBandShoulder{95.f / 255.f, 88.f / 255.f, 55.f / 255.f, 1.f};
constexpr std::array<float, 4> kBandGrass{2.f / 255.f, 125.f / 255.f, 42.f / 255.f, 1.f};
constexpr std::array<float, 4> kEnterRimOuter{158.f / 255.f, 213.f / 255.f, 230.f / 255.f, 1.f};
constexpr std::array<float, 4> kEnterRimInner{17.f / 255.f, 159.f / 255.f, 232.f / 255.f, 1.f};
constexpr std::array<float, 4> kEnterInk{220.f / 255.f, 114.f / 255.f, 88.f / 255.f, 1.f};
constexpr std::array<float, 4> kEnterIcon{30.f / 255.f, 30.f / 255.f, 40.f / 255.f, 1.f};
// title.png plate sample (500, 120).
constexpr std::array<float, 4> kPlate{212.f / 255.f, 201.f / 255.f, 177.f / 255.f, 1.f};
// Lower-edge gray of title.png (column scans y=431..446).
constexpr std::array<float, 4> kRoad{125.f / 255.f, 125.f / 255.f, 125.f / 255.f, 1.f};

// config/game_symbols.toml:168, docs/native-2d.md:48, oracle-golden-frames.md:31.
constexpr const char* kPressStart = "PRESS START BUTTON";
// Painted on title.png. Not an ui-parity.md template.
constexpr const char* kPublisher = "Published by Conspiracy Entertainment Inc.";
// GAP: the painted mark is U+00A9, which FONTE does not encode and which is
// not one of the four approved substitutions (ui-parity.md:102).
constexpr const char* kCopyright = "(C) Takara Co., Ltd. 2002";

// menus-and-save.md:40, game_state.toml:227, golden main_menu.png.
// "Options" is also PauseMenu.cpp:300. "Results" is the golden fifth row
// (GAP: not in the ui-parity.md 184).
constexpr const char* kMainLabels[] = {
    "Adventure", "Quick Race", "2 Player", "Options", "Results",
};
// game_state.toml:251, GameOptions.h:3, golden options.png.
// "Vibration" PauseMenu.cpp:846. "Speaker" PauseMenu.cpp:533.
// GAP: ui-parity Sound row is "Volume" (PauseMenu.cpp:527), not "Sound Volume".
// GAP: recomp pause says "Quit" (PauseMenu.cpp root), not "Exit".
constexpr const char* kOptionLabels[] = {
    "Vibration", "Speaker", "Sound Volume", "Exit",
};

struct Map {
    float scale = 1.f;
    float ox = 0.f;
    float oy = 0.f;
};

Map frameMap(int w, int h) {
    // ui-parity.md:9. On 16:9, min(W/640, H/480) == S * (1080/480).
    const float fromS = uiDesignScale(h) * (1080.f / kFrameHeight);
    const float frame = std::min(static_cast<float>(w) / kFrameWidth,
                                 static_cast<float>(h) / kFrameHeight);
    const bool sixteenNine = w * 9 == h * 16;
    const float scale = sixteenNine ? fromS : frame;
    return {scale,
            (static_cast<float>(w) - kFrameWidth * scale) * 0.5f,
            (static_cast<float>(h) - kFrameHeight * scale) * 0.5f};
}

float px(const Map& m, float ui) { return m.ox + ui * m.scale; }
float py(const Map& m, float ui) { return m.oy + (ui + kFrameYOffset) * m.scale; }
float ps(const Map& m, float ui) { return ui * m.scale; }

void addRect(std::vector<ScreenQuad>& out, const Map& m, float x, float y, float w, float h,
             const std::array<float, 4>& color) {
    if (w <= 0.f || h <= 0.f) return;
    out.push_back(ScreenQuad{px(m, x), py(m, y), ps(m, w), ps(m, h), color});
}

void addWindowRect(std::vector<ScreenQuad>& out, float x, float y, float w, float h,
                   const std::array<float, 4>& color) {
    if (w <= 0.f || h <= 0.f) return;
    out.push_back(ScreenQuad{x, y, w, h, color});
}

void addWindowTexture(std::vector<ScreenQuad>& out, int w, int h, const PreviewTexture* texture) {
    if (!texture) return;
    out.push_back(ScreenQuad{0.f, 0.f, static_cast<float>(w), static_cast<float>(h),
                             kWhite, texture, 0.f, 0.f, 1.f, 1.f});
}

// UV convention is fonteSlotForChar / fonteSlotUv, the same one race_hud.h
// fonteTextQuads uses. Cell size is scaled; the race HUD uses a full cell
// because those placements were measured for 16x24. Menu bitmap metrics are
// narrower (header table). Space advances and emits nothing.
void appendText(std::vector<HudQuad>& out, const char* text, float uiX, float uiY, float scaleX, float scaleY,
                const std::array<float, 4>& color, const Map& m) {
    if (!text) return;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * scaleX;
    const float ch = static_cast<float>(rt::native::assets::kGlyphCellHeight) * scaleY;
    float x = uiX;
    for (const char* p = text; *p; ++p) {
        const int slot = rt::native::assets::fonteSlotForChar(*p);
        if (slot >= 0) {
            const auto uv = rt::native::assets::fonteSlotUv(slot);
            out.push_back(HudQuad{px(m, x), py(m, uiY), ps(m, cw), ps(m, ch), uv.u0, uv.v0, uv.u1, uv.v1,
                                  color});
        }
        x += cw;
    }
}

float textWidth(const char* text, float scaleX) {
    if (!text) return 0.f;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * scaleX;
    float n = 0.f;
    for (const char* p = text; *p; ++p) n += 1.f;
    return n * cw;
}

int clampFocus(int index, int count) {
    if (count <= 0) return 0;
    return std::clamp(index, 0, count - 1);
}

void addBackdrop(std::vector<ScreenQuad>& out, const Map& m, int w, int h) {
    // Full window so the 16:9 letterbox is the same sky, not the clear color.
    addWindowRect(out, 0.f, 0.f, static_cast<float>(w), static_cast<float>(h), kSky);
    addRect(out, m, 0.f, 165.f, kFrameWidth, 41.f, kBandNavy);
    addRect(out, m, 0.f, 209.f, kFrameWidth, 19.f, kBandStripe);
    addRect(out, m, 0.f, 209.f, kFrameWidth, 1.f, kBandStripeEdge);
    addRect(out, m, 0.f, 227.f, kFrameWidth, 1.f, kBandStripeEdge);
    addRect(out, m, 0.f, 228.f, kFrameWidth, 11.f, kBandShoulder);
    // Road precedes the grass so the field is composited above it.
    addRect(out, m, 0.f, 247.f, kFrameWidth, 201.f, kRoad);
    addRect(out, m, 0.f, 239.f, 139.f, 97.f, kBandGrass);
    // Plate card. AABB of the tilted license plate (header table). Gold rim is
    // Theme.h Gold, standing in for the yellow halo. GAP: not the plate texture.
    constexpr float plateX = 57.f;
    constexpr float plateY = 25.f;
    constexpr float plateW = 493.f;
    constexpr float plateH = 194.f;
    addRect(out, m, plateX - 8.f, plateY - 8.f, plateW + 16.f, plateH + 16.f, kUiCapGold);
    addRect(out, m, plateX, plateY, plateW, plateH, kPlate);
}

void addPanel(std::vector<ScreenQuad>& out, const Map& m, float x, float y, float w, float h) {
    // promptBox (Theme.cpp:202-209): Prompt fill, two rims. Measured body is
    // opaque 304090; contract alpha is F2 (Theme.h:44). Using the contract.
    addRect(out, m, x, y, w, h, kUiPromptRimOuter);
    addRect(out, m, x + 1.f, y + 1.f, w - 2.f, h - 2.f, kUiPromptRimInner);
    addRect(out, m, x + kRim, y + kRim, w - 2.f * kRim, h - 2.f * kRim, kUiPrompt);
}

void addEnterButton(std::vector<ScreenQuad>& out, std::vector<HudQuad>& text, const Map& m) {
    constexpr float x = 423.f;
    constexpr float y = 37.7f;
    constexpr float w = 193.5f;
    constexpr float h = 39.7f;
    addRect(out, m, x, y, w, h, kEnterRimOuter);
    addRect(out, m, x + 1.f, y + 1.f, w - 2.f, h - 2.f, kEnterRimInner);
    addRect(out, m, x + 2.f, y + 2.f, w - 4.f, h - 4.f, kUiPrompt);
    addRect(out, m, 432.f, 47.f, 21.f, 24.f, kEnterIcon);
    appendText(text, "button Enter", 460.f, 54.f, 0.45f, 0.5f, kEnterInk, m);
}

void addSelection(std::vector<ScreenQuad>& out, const Map& m, float x, float y, float w, float h) {
    // GAP: rounded capsule is an axis-aligned rect (ScreenQuad has no radii).
    const float bodyY = y + 2.f;
    const float bodyH = h - 4.f;
    const float bodyX = x + 1.f;
    const float bodyW = w - 2.f;
    const float topH = bodyH * 0.30f;
    const float middleH = bodyH * 0.35f;
    addRect(out, m, bodyX, bodyY, bodyW, topH, kMenuSelectionTop);
    addRect(out, m, bodyX, bodyY + topH, bodyW, middleH, kMenuSelectionMiddle);
    addRect(out, m, bodyX, bodyY + topH + middleH, bodyW, bodyH - topH - middleH, kMenuSelectionBottom);
    constexpr float capW = 8.f;
    addRect(out, m, x + w - capW, bodyY, capW, bodyH, kMenuCapGold);
    addRect(out, m, x + w - 6.4f, y + (h - 6.f) * 0.5f, 3.6f, 6.f, kMenuCapShine);
    addRect(out, m, x, y, w, 1.f, kMenuCapGold);
    addRect(out, m, x, y + 1.f, w, 1.f, kMenuSelectionFrame);
    addRect(out, m, x, y + h - 2.f, w, 1.f, kMenuSelectionFrame);
    addRect(out, m, x, y + h - 1.f, w, 1.f, kMenuCapGold);
    addRect(out, m, x, y + 2.f, 1.f, h - 4.f, kMenuSelectionFrame);
    addRect(out, m, x + w - 1.f, y + 2.f, 1.f, h - 4.f, kMenuSelectionFrame);
}

void addHorn(std::vector<ScreenQuad>& out, const Map& m, float tipX, float centerY, float height) {
    // Theme.cpp:229-248. Tip touches the selection bar's left end. Bob is a
    // time term; this builder is stateless, so the horn is the rest pose.
    // GAP: circles/ellipses are rects.
    const float width = height * 1.7f;
    const float left = tipX - width;
    const float top = centerY - height * 0.5f;
    addRect(out, m, left, top + height * 0.36f, height * 0.28f, height * 0.28f, kUiHornShade);
    addRect(out, m, left + width * 0.08f, top + height * 0.40f, width * 0.47f, height * 0.20f, kUiHorn);
    addRect(out, m, left + width * 0.50f, top, width * 0.42f, height, kUiHorn);
    addRect(out, m, left + width * 0.88f, top, width * 0.12f, height, kUiHornLine);
}

void addRowText(std::vector<HudQuad>& text, const Map& m, const char* const* labels, int count,
                 float panelX, float panelY, int focus) {
    for (int i = 0; i < count; ++i) {
        const float rowY = panelY + kRowInsetY + static_cast<float>(i) * kRowPitch;
        const float textX = panelX + (i == focus ? kSelectedTextInsetX : kTextInsetX);
        appendText(text, labels[i], textX, rowY + 4.3f, kMenuGlyphScaleX, kMenuGlyphScaleY,
                    i == focus ? kMenuSelectedInk : kMenuInk, m);
    }
}

void addTitleCardText(std::vector<HudQuad>& text, const Map& m, bool pressVisible) {
    appendText(text, kPublisher, 321.f, 212.f, 0.45f, 0.55f, kWhite, m);
    if (pressVisible) {
        // Centered over the road, inside the measured ink band y 236..257.
        const float w = textWidth(kPressStart, kMenuGlyphScaleX);
        appendText(text, kPressStart, (kFrameWidth - w) * 0.5f, 240.f,
                   kMenuGlyphScaleX, kMenuGlyphScaleY, kUiFrame, m);
    }
    appendText(text, kCopyright, 452.f, 436.f, kSmallGlyphScaleX, kSmallGlyphScaleY, kWhite, m);
}

bool valid(int w, int h) { return w > 0 && h > 0; }

}  // namespace

const char* mainMenuRowLabel(int index) {
    if (index < 0 || index >= kMainMenuRowCount) return nullptr;
    return kMainLabels[index];
}

const char* optionsRowLabel(int index) {
    if (index < 0 || index >= kOptionsRowCount) return nullptr;
    return kOptionLabels[index];
}

UiScreen buildTitleScreen(int w, int h, bool pressVisible) {
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    addBackdrop(screen.quads, m, w, h);
    addTitleCardText(screen.text, m, pressVisible);
    return screen;
}

UiScreen buildMainMenu(int w, int h, int focusIndex) {
    return buildMainMenu(w, h, focusIndex, nullptr);
}

UiScreen buildMainMenu(int w, int h, int focusIndex, const PreviewTexture* backdrop) {
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    const int focus = clampFocus(focusIndex, kMainMenuRowCount);
    addWindowTexture(screen.quads, w, h, backdrop);
    addBackdrop(screen.quads, m, w, h);
    addEnterButton(screen.quads, screen.text, m);
    addPanel(screen.quads, m, kMainX, kMainY, kMainW, kMainH);
    const float rowY = kMainY + kRowInsetY + static_cast<float>(focus) * kRowPitch;
    const float barX = kMainX + 24.f;
    constexpr float barW = 138.5f;
    addSelection(screen.quads, m, barX, rowY, barW, kRowH);
    addHorn(screen.quads, m, 461.f, rowY + kRowH * 0.5f, kHornHeight);
    addTitleCardText(screen.text, m, false);
    addRowText(screen.text, m, kMainLabels, kMainMenuRowCount, kMainX, kMainY, focus);
    return screen;
}

UiScreen buildOptionsScreen(int w, int h, int focusIndex) {
    return buildOptionsScreen(w, h, focusIndex, nullptr);
}

UiScreen buildOptionsScreen(int w, int h, int focusIndex, const PreviewTexture* backdrop) {
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    const int focus = clampFocus(focusIndex, kOptionsRowCount);
    addWindowTexture(screen.quads, w, h, backdrop);
    addBackdrop(screen.quads, m, w, h);
    addEnterButton(screen.quads, screen.text, m);
    // The golden keeps the title menu up, cursor on Options (index 3).
    addPanel(screen.quads, m, kMainX, kMainY, kMainW, kMainH);
    constexpr int kOptionsOnMainMenu = 3;
    const float mainRowY = kMainY + kRowInsetY + static_cast<float>(kOptionsOnMainMenu) * kRowPitch;
    const float mainBarX = kMainX + 24.f;
    constexpr float mainBarW = 138.5f;
    addSelection(screen.quads, m, mainBarX, mainRowY, mainBarW, kRowH);
    addHorn(screen.quads, m, 461.f, mainRowY + kRowH * 0.5f, kHornHeight);

    addPanel(screen.quads, m, kOptX, kOptY, kOptW, kOptH);
    const float rowY = kOptY + kRowInsetY + static_cast<float>(focus) * kRowPitch;
    const float barX = kOptX + kRim;
    const float barW = kOptW - 2.f * kRim;
    addSelection(screen.quads, m, barX, rowY, barW, kRowH);
    addHorn(screen.quads, m, barX, rowY + kRowH * 0.5f, kRowH);

    addTitleCardText(screen.text, m, false);
    addRowText(screen.text, m, kMainLabels, kMainMenuRowCount, kMainX, kMainY, kOptionsOnMainMenu);
    addRowText(screen.text, m, kOptionLabels, kOptionsRowCount, kOptX, kOptY, focus);
    return screen;
}


namespace {

using gameplay::OptionPage;
using gameplay::OptionSpec;

struct BuiltPanel {
    PausePanel panel;
    std::vector<const OptionSpec*> spec;  // parallel to panel.rows; nullptr = not a model row
    int linkRow = -1;                     // the Options -> Controllers row
};

// PauseMenu.cpp:813-872 language for the Controllers child page.
constexpr const char* kDevices = "Devices";
constexpr const char* kNoPadNote = "Connect a controller and it appears here.";
constexpr const char* kDriving = "Driving";
constexpr const char* kVibrationHeading = "Vibration";
constexpr const char* kPlayer1 = "Player 1";

void addRow(BuiltPanel& b, PanelRowKind kind, std::string label, std::string value, bool enabled,
            const OptionSpec* spec) {
    b.panel.rows.push_back(PanelRow{kind, std::move(label), std::move(value), enabled});
    b.spec.push_back(spec);
}

BuiltPanel buildRows(const gameplay::OptionsModel& model, const gameplay::SettingsStore& store,
                     const gameplay::MenuContext& context, const OptionsPanelState& state) {
    BuiltPanel b;
    const auto item = [&](const OptionSpec* spec, bool enabled = true) {
        addRow(b, PanelRowKind::Item, spec->label, model.valueOf(*spec, store), enabled, spec);
    };
    if (!state.controllers) {
        b.panel.title = "Options";
        for (OptionPage page : {OptionPage::Display, OptionPage::Sound, OptionPage::Graphics}) {
            const auto rows = model.pageRows(page, context);
            if (rows.empty()) continue;
            addRow(b, PanelRowKind::Heading, gameplay::optionPageName(page), {}, false, nullptr);
            for (const OptionSpec* spec : rows) item(spec);
        }
        b.linkRow = static_cast<int>(b.panel.rows.size());
        addRow(b, PanelRowKind::Item, "Controllers", {}, true, nullptr);
    } else {
        b.panel.title = "Controllers";
        addRow(b, PanelRowKind::Heading, kDevices, {}, false, nullptr);
        if (context.gamepadPresent)
            addRow(b, PanelRowKind::Item, "Gamepad", kPlayer1, false, nullptr);
        else
            addRow(b, PanelRowKind::Note, kNoPadNote, {}, false, nullptr);
        addRow(b, PanelRowKind::Heading, kDriving, {}, false, nullptr);
        if (const OptionSpec* controls = model.find("controllers.controls")) item(controls);
        if (context.gamepadPresent) {
            addRow(b, PanelRowKind::Heading, kVibrationHeading, {}, false, nullptr);
            const OptionSpec* vibration = model.find("controllers.vibration");
            const OptionSpec* strength = model.find("controllers.strength");
            if (vibration) item(vibration);
            if (strength) item(strength, !vibration || model.valueOf(*vibration, store) != "Off");
        }
    }
    return b;
}

bool selectable(const BuiltPanel& b, int i) {
    return i >= 0 && i < static_cast<int>(b.panel.rows.size()) &&
           b.panel.rows[static_cast<std::size_t>(i)].kind == PanelRowKind::Item &&
           b.panel.rows[static_cast<std::size_t>(i)].enabled;
}

void settleFocus(const BuiltPanel& b, OptionsPanelState& state) {
    if (selectable(b, state.focus)) return;
    state.focus = -1;
    for (int i = 0; i < static_cast<int>(b.panel.rows.size()); ++i)
        if (selectable(b, i)) { state.focus = i; break; }
}

}  // namespace

PausePanel buildOptionsPanel(const gameplay::OptionsModel& model, const gameplay::SettingsStore& store,
                             const gameplay::MenuContext& context, OptionsPanelState& state) {
    BuiltPanel b = buildRows(model, store, context, state);
    settleFocus(b, state);
    b.panel.focus = state.focus;
    return std::move(b.panel);
}

bool optionsPanelMove(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                      const gameplay::MenuContext& context, int direction) {
    const BuiltPanel b = buildRows(model, store, context, state);
    settleFocus(b, state);
    const int n = static_cast<int>(b.panel.rows.size());
    if (n == 0 || state.focus < 0 || direction == 0) return false;
    int i = state.focus;
    for (int step = 0; step < n; ++step) {
        i = ((i + (direction > 0 ? 1 : -1)) % n + n) % n;
        if (selectable(b, i)) { const bool moved = i != state.focus; state.focus = i; return moved; }
    }
    return false;
}

bool optionsPanelAdjust(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                        const gameplay::MenuContext& context, int direction) {
    const BuiltPanel b = buildRows(model, store, context, state);
    settleFocus(b, state);
    if (!selectable(b, state.focus)) return false;
    const OptionSpec* spec = b.spec[static_cast<std::size_t>(state.focus)];
    if (!spec || !model.adjustValue(*spec, store, direction)) return false;
    (void)store.save();
    return true;
}

bool optionsPanelConfirm(OptionsPanelState& state, const gameplay::OptionsModel& model, gameplay::SettingsStore& store,
                         const gameplay::MenuContext& context) {
    const BuiltPanel b = buildRows(model, store, context, state);
    settleFocus(b, state);
    if (!state.controllers && state.focus >= 0 && state.focus == b.linkRow) {
        state.controllers = true;
        state.focus = -1;
        return true;
    }
    return optionsPanelAdjust(state, model, store, context, 1);
}

bool optionsPanelBack(OptionsPanelState& state) {
    if (!state.controllers) return false;
    state.controllers = false;
    state.focus = -1;
    return true;
}

UiScreen buildPausePanelScreen(int w, int h, const PausePanel& panel) {
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    const float S = uiDesignScale(h);
    // Window pixels -> UI640 placement units (inverse of px()/py()).
    const auto ux = [&](float pix) { return (pix - m.ox) / m.scale; };
    const auto uy = [&](float pix) { return (pix - m.oy) / m.scale - kFrameYOffset; };
    const auto us = [&](float pix) { return pix / m.scale; };
    const auto rect = [&](float x, float y, float rw, float rh, const std::array<float, 4>& c) {
        addRect(screen.quads, m, ux(x), uy(y), us(rw), us(rh), c);
    };

    // PauseMenu.cpp:1838-1887.
    const float rowH = 80.f * S, pad = 30.f * S, margin = 24.f * S, tabH = 44.f * S, promptsH = 90.f * S;
    const float width = std::min((panel.wide ? 880.f : 520.f) * S, static_cast<float>(w) * 0.92f);
    const float chrome = pad + pad * 0.6f;
    const float avail = static_cast<float>(h) - 2.f * margin - tabH - promptsH - chrome;
    const int fit = std::max(1, static_cast<int>(avail / rowH));
    const int cap = std::min(fit, std::max(1, static_cast<int>(static_cast<float>(h) * 0.62f / rowH)));
    const int n = static_cast<int>(panel.rows.size());
    const int visible = std::max(1, panel.wide ? cap : std::min(n, cap));
    const float bodyH = rowH * static_cast<float>(visible);
    const float height = bodyH + chrome;
    const float px0 = (static_cast<float>(w) - width) * 0.5f;
    const float py0 = std::clamp((static_cast<float>(h) - height - promptsH * 0.5f) * 0.5f, margin + tabH,
                                 std::max(margin + tabH, static_cast<float>(h) - margin - promptsH - height));

    addWindowRect(screen.quads, 0.f, 0.f, static_cast<float>(w), static_cast<float>(h), theme::rgba(theme::kScrim));
    // Theme.cpp:162-200: list body, 3 px bevel at 12*S, then the 3/6/3 frame.
    rect(px0, py0, width, height, theme::rgba(theme::kFrameOuter));
    rect(px0 + 3.f * S, py0 + 3.f * S, width - 6.f * S, height - 6.f * S, theme::rgba(theme::kFrame));
    rect(px0 + 9.f * S, py0 + 9.f * S, width - 18.f * S, height - 18.f * S, theme::rgba(theme::kFrameLight));
    rect(px0 + 12.f * S, py0 + 12.f * S, width - 24.f * S, height - 24.f * S, theme::rgba(theme::kListBody));
    const float in = 12.f * S, bv = 3.f * S;
    rect(px0 + in, py0 + in, width - 2.f * in, bv, theme::rgba(theme::kListBevelLight));
    rect(px0 + in, py0 + in, bv, height - 2.f * in, theme::rgba(theme::kListBevelLight));
    rect(px0 + in, py0 + height - in - bv, width - 2.f * in, bv, theme::rgba(theme::kListBevelDark));
    rect(px0 + width - in - bv, py0 + in, bv, height - 2.f * in, theme::rgba(theme::kListBevelDark));

    const float glyphH = 36.f * S;
    const float cellH = static_cast<float>(rt::native::assets::kGlyphCellHeight);
    const float cellW = static_cast<float>(rt::native::assets::kGlyphCellWidth);
    const float scaleY = us(glyphH) / cellH;
    const float scaleXBase = scaleY * 0.77f;
    const auto textAt = [&](const std::string& t, float xPix, float yPix, float sx, float sy,
                            const std::array<float, 4>& c) { appendText(screen.text, t.c_str(), ux(xPix), uy(yPix), sx, sy, c, m); };
    const auto pixWidth = [&](const std::string& t, float sx) { return static_cast<float>(t.size()) * cellW * sx * m.scale; };

    // Name tab (Theme.cpp:131-141).
    if (!panel.title.empty()) {
        const float tabPad = 26.f * S, th = 52.f * S;
        const float tw = pixWidth(panel.title, scaleXBase) + 2.f * tabPad;
        const float tx = px0 + 40.f * S, ty = py0 - th * 0.55f;
        rect(tx, ty, tw, th, theme::rgba(theme::kFrameOuter));
        rect(tx + 3.f * S, ty + 3.f * S, tw - 6.f * S, th - 6.f * S, theme::rgba(theme::kFrame));
        textAt(panel.title, tx + tabPad, ty + (th - glyphH) * 0.5f, scaleXBase, scaleY, theme::rgba(theme::kListSelected));
    }

    // Keep the cursor (and its heading) in view.
    int first = 0;
    if (panel.focus >= visible) first = panel.focus - visible + 1;
    first = std::clamp(first, 0, std::max(0, n - visible));
    if (panel.focus >= 0 && panel.focus - 1 >= 0 && panel.focus == first &&
        panel.rows[static_cast<std::size_t>(panel.focus - 1)].kind == PanelRowKind::Heading && first > 0)
        --first;

    const float innerX = px0 + pad + 30.f * S;  // 30*S inner side inset past the pad
    const float innerW = width - 2.f * (pad + 30.f * S);
    for (int r = 0; r < visible && first + r < n; ++r) {
        const PanelRow& row = panel.rows[static_cast<std::size_t>(first + r)];
        const float top = py0 + pad + rowH * static_cast<float>(r);
        const bool focused = first + r == panel.focus && row.kind == PanelRowKind::Item;
        std::array<float, 4> ink = theme::rgba(row.kind == PanelRowKind::Heading ? theme::kFrameLight
                                               : row.kind == PanelRowKind::Note   ? theme::kListBevelLight
                                               : focused                          ? theme::kListSelected
                                                                                   : theme::kListText);
        if (!row.enabled && row.kind == PanelRowKind::Item) ink[3] = 0.5f;
        if (focused) {
            const float bx = px0 + pad + 6.f * S, bw = width - 2.f * (pad + 6.f * S);
            const float bh = rowH - 8.f * S, by = top + 4.f * S;
            addSelection(screen.quads, m, ux(bx), uy(by), us(bw), us(bh));
            addHorn(screen.quads, m, ux(bx), uy(by + bh * 0.5f), us(bh * 0.6f));
        }
        float sx = scaleXBase;
        const float need = pixWidth(row.label, sx) + (row.value.empty() ? 0.f : pixWidth(row.value, sx) + 40.f * S);
        if (need > innerW) sx *= innerW / need;
        const float ty = top + (rowH - glyphH) * 0.5f;
        textAt(row.label, innerX, ty, sx, scaleY, ink);
        if (!row.value.empty())
            textAt(row.value, innerX + innerW - pixWidth(row.value, sx), ty, sx, scaleY, ink);
    }
    return screen;
}

}  // namespace rt::native::preview
