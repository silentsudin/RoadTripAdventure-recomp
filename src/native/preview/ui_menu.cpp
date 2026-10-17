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

// Bright-rim boxes from the header table. Width/height are inclusive scanout
// spans converted by /2 (x) and 1:1 (y): 1189-866+1 = 324 scanout = 162 UI;
// 400-268+1 = 133; 843-456+1 = 388 scanout = 194 UI; 276-168+1 = 109.
constexpr float kMainX = 433.f;
constexpr float kMainY = 268.f;
constexpr float kMainW = 162.f;
constexpr float kMainH = 133.f;
constexpr float kOptX = 228.f;
constexpr float kOptY = 168.f;
constexpr float kOptW = 194.f;
constexpr float kOptH = 109.f;
constexpr float kRowPitch = 24.f;
constexpr float kRowInsetY = 6.f;
constexpr float kRowH = 24.f;       // measured cap span is 25; T1
constexpr float kTextInsetX = 29.f; // 58 scanout px / 2
constexpr float kRim = 2.f;         // 4 scanout px of bright rim / 2
// Menu bitmap advance measured ~12.2 UI px/char vs FONTE's 16. 0.75 keeps
// "Sound Volume" inside the options panel. GAP: not the race-HUD full cell.
constexpr float kMenuGlyphScale = 0.75f;
// Copyright ink is ~6.2 UI px/char (scanout span 323 over the painted line).
constexpr float kSmallGlyphScale = 0.40f;

constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, 1.f};
// title.png (100, 10). GAP: flat stand-in for the 3D sky.
constexpr std::array<float, 4> kSky{43.f / 255.f, 154.f / 255.f, 216.f / 255.f, 1.f};
// title.png plate sample (500, 120).
constexpr std::array<float, 4> kPlate{212.f / 255.f, 201.f / 255.f, 177.f / 255.f, 1.f};
// Lower-edge gray of title.png (column scans y=431..446).
constexpr std::array<float, 4> kRoad{129.f / 255.f, 128.f / 255.f, 128.f / 255.f, 1.f};

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

// UV convention is fonteSlotForChar / fonteSlotUv, the same one race_hud.h
// fonteTextQuads uses. Cell size is scaled; the race HUD uses a full cell
// because those placements were measured for 16x24. Menu bitmap metrics are
// narrower (header table). Space advances and emits nothing.
void appendText(std::vector<HudQuad>& out, const char* text, float uiX, float uiY, float glyphScale,
                const std::array<float, 4>& color, const Map& m) {
    if (!text) return;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * glyphScale;
    const float ch = static_cast<float>(rt::native::assets::kGlyphCellHeight) * glyphScale;
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

float textWidth(const char* text, float glyphScale) {
    if (!text) return 0.f;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * glyphScale;
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
    // Road band under the plate. UI y 250..448, gray measured at the frame bottom.
    addRect(out, m, 0.f, 250.f, kFrameWidth, 198.f, kRoad);
    // Plate card. AABB of the tilted license plate (header table). Gold rim is
    // Theme.h Gold, standing in for the yellow halo. GAP: not the plate texture.
    constexpr float plateX = 57.f;
    constexpr float plateY = 38.f;
    constexpr float plateW = 493.f;
    constexpr float plateH = 181.f;
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

void addSelection(std::vector<ScreenQuad>& out, const Map& m, float x, float y, float w, float h) {
    // selectionBar (Theme.cpp:212-227): orange edge, two-tone blue, gold cap.
    // GAP: measured interior (0,110,224) and cap (232,146,11) differ from
    // BarTop 2C6CE8 / BarCapGold F8C838. Contract colors win (ui-parity.md:110).
    // GAP: rounded capsule is an axis-aligned rect (ScreenQuad has no radii).
    addRect(out, m, x, y, w, h, kUiFrame);
    const float in = 1.f;
    const float mid = h * 0.5f;
    addRect(out, m, x + in, y + in, w - 2.f * in, mid - in, kUiSelectionTop);
    addRect(out, m, x + in, y + mid, w - 2.f * in, h - mid - in, kUiSelectionBottom);
    const float cap = h * 0.55f; // Theme.cpp:225
    addRect(out, m, x + w - cap, y, cap, h, kUiCapGold);
    addRect(out, m, x + w - cap * 0.8f, y + 2.f, cap * 0.45f, h * 0.3f, kUiCapShine);
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
                float panelX, float panelY) {
    const float glyphH = static_cast<float>(rt::native::assets::kGlyphCellHeight) * kMenuGlyphScale;
    const float textYOffset = (kRowH - glyphH) * 0.5f;
    for (int i = 0; i < count; ++i) {
        const float rowY = panelY + kRowInsetY + static_cast<float>(i) * kRowPitch;
        appendText(text, labels[i], panelX + kTextInsetX, rowY + textYOffset, kMenuGlyphScale, kWhite, m);
    }
}

void addTitleCardText(std::vector<HudQuad>& text, const Map& m, bool pressVisible) {
    const float pubW = textWidth(kPublisher, kSmallGlyphScale);
    appendText(text, kPublisher, (kFrameWidth - pubW) * 0.5f, 156.f, kSmallGlyphScale, kWhite, m);
    if (pressVisible) {
        // Centered over the road, inside the measured ink band y 236..257.
        const float w = textWidth(kPressStart, kMenuGlyphScale);
        appendText(text, kPressStart, (kFrameWidth - w) * 0.5f, 240.f, kMenuGlyphScale, kUiFrame, m);
    }
    // Measured ink starts at scanout x=904 → UI 452, y=422.
    appendText(text, kCopyright, 452.f, 422.f, kSmallGlyphScale, kWhite, m);
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
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    const int focus = clampFocus(focusIndex, kMainMenuRowCount);
    addBackdrop(screen.quads, m, w, h);
    addPanel(screen.quads, m, kMainX, kMainY, kMainW, kMainH);
    const float rowY = kMainY + kRowInsetY + static_cast<float>(focus) * kRowPitch;
    const float barX = kMainX + kRim;
    const float barW = kMainW - 2.f * kRim;
    addSelection(screen.quads, m, barX, rowY, barW, kRowH);
    addHorn(screen.quads, m, barX, rowY + kRowH * 0.5f, kRowH);
    addTitleCardText(screen.text, m, false);
    addRowText(screen.text, m, kMainLabels, kMainMenuRowCount, kMainX, kMainY);
    return screen;
}

UiScreen buildOptionsScreen(int w, int h, int focusIndex) {
    UiScreen screen;
    if (!valid(w, h)) return screen;
    const Map m = frameMap(w, h);
    const int focus = clampFocus(focusIndex, kOptionsRowCount);
    addBackdrop(screen.quads, m, w, h);
    // The golden keeps the title menu up, cursor on Options (index 3).
    addPanel(screen.quads, m, kMainX, kMainY, kMainW, kMainH);
    constexpr int kOptionsOnMainMenu = 3;
    const float mainRowY = kMainY + kRowInsetY + static_cast<float>(kOptionsOnMainMenu) * kRowPitch;
    const float mainBarX = kMainX + kRim;
    const float mainBarW = kMainW - 2.f * kRim;
    addSelection(screen.quads, m, mainBarX, mainRowY, mainBarW, kRowH);
    addHorn(screen.quads, m, mainBarX, mainRowY + kRowH * 0.5f, kRowH);

    addPanel(screen.quads, m, kOptX, kOptY, kOptW, kOptH);
    const float rowY = kOptY + kRowInsetY + static_cast<float>(focus) * kRowPitch;
    const float barX = kOptX + kRim;
    const float barW = kOptW - 2.f * kRim;
    addSelection(screen.quads, m, barX, rowY, barW, kRowH);
    addHorn(screen.quads, m, barX, rowY + kRowH * 0.5f, kRowH);

    addTitleCardText(screen.text, m, false);
    addRowText(screen.text, m, kMainLabels, kMainMenuRowCount, kMainX, kMainY);
    addRowText(screen.text, m, kOptionLabels, kOptionsRowCount, kOptX, kOptY);
    return screen;
}

}  // namespace rt::native::preview
