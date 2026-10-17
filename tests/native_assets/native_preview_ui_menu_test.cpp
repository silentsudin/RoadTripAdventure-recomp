// Title / main-menu / options screen-builder contract.
//
// Expected strings are embedded here (with citations), not read back from the
// builder's own tables, so a wrong literal fails. ui-parity.md does not list
// the game title-menu rows; those come from the oracle golden frames and the
// cited game sources. A spot-read of ui-parity.md, when the file is reachable,
// pins the scale rule and the recomp rows these screens borrow words from.
#include "../../src/native/preview/ui_menu.h"

#include "../../src/native/assets/glyph_atlas.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace {

int g_checks = 0;
int g_fail = 0;

void check(bool ok, const char* what) {
    ++g_checks;
    if (!ok) {
        std::fprintf(stderr, "FAIL %s\n", what);
        ++g_fail;
    }
}

bool near(float a, float b, float eps = 1e-3f) {
    return std::fabs(a - b) <= eps;
}

// Independent of uiDesignScale: Theme.cpp:89, handheldBoost = 1 (Theme.cpp:84).
float contractScale(int height) {
    return std::max(0.5f, static_cast<float>(height) / 1080.f);
}

std::uint64_t fnv(const rt::native::preview::UiScreen& screen) {
    std::uint64_t h = 14695981039346656037ull;
    auto mix = [&](float v) {
        const std::uint32_t bits = static_cast<std::uint32_t>(std::bit_cast<std::uint32_t>(v));
        for (int i = 0; i < 4; ++i) {
            h ^= static_cast<unsigned char>((bits >> (8 * i)) & 0xffu);
            h *= 1099511628211ull;
        }
    };
    auto mixColor = [&](const std::array<float, 4>& c) {
        for (float v : c) mix(v);
    };
    for (const auto& q : screen.quads) {
        mix(q.x); mix(q.y); mix(q.width); mix(q.height); mixColor(q.color);
        mix(q.texture ? 1.f : 0.f);
        mix(q.u0); mix(q.v0); mix(q.u1); mix(q.v1);
    }
    for (const auto& q : screen.text) {
        mix(q.x); mix(q.y); mix(q.width); mix(q.height);
        mix(q.u0); mix(q.v0); mix(q.u1); mix(q.v1);
        mixColor(q.color);
    }
    h ^= screen.quads.size();
    h *= 1099511628211ull;
    h ^= screen.text.size();
    return h;
}

// Walk FONTE slots in order. Space advances and emits nothing (glyph_atlas.h
// fonteSlotForChar). Fails if the quad stream has extra or missing glyphs.
bool glyphsMatch(const std::vector<rt::native::preview::HudQuad>& text,
                 const std::vector<const char*>& strings, const char* what) {
    using rt::native::assets::fonteSlotForChar;
    using rt::native::assets::fonteSlotUv;
    std::size_t i = 0;
    for (const char* s : strings) {
        if (!s) {
            check(false, what);
            return false;
        }
        for (const char* p = s; *p; ++p) {
            const int slot = fonteSlotForChar(*p);
            if (slot < 0) continue;
            const auto uv = fonteSlotUv(slot);
            if (i >= text.size() || text[i].u0 != uv.u0 || text[i].v0 != uv.v0 ||
                text[i].u1 != uv.u1 || text[i].v1 != uv.v1) {
                std::fprintf(stderr, "FAIL %s glyph %zu (char '%c')\n", what, i, *p);
                ++g_checks;
                ++g_fail;
                return false;
            }
            ++i;
        }
    }
    ++g_checks;
    if (i != text.size()) {
        std::fprintf(stderr, "FAIL %s glyph count %zu != %zu\n", what, i, text.size());
        ++g_fail;
        return false;
    }
    return true;
}

// Citations live next to the literals the builders must emit.
// Title art: oracle-golden-frames.md:31, config/game_symbols.toml:168,
// docs/native-2d.md:48. Publisher / copyright are painted on title.png;
// © is not in the FONTE range and is not an approved substitution
// (ui-parity.md:102), so the builder's ASCII stand-in is "(C)".
const char* kPublisher = "Published by Conspiracy Entertainment Inc.";
const char* kCopyright = "(C) Takara Co., Ltd. 2002";
const char* kPressStart = "PRESS START BUTTON";

// menus-and-save.md:40, game_state.toml:227, golden main_menu.png.
// "Options" also PauseMenu.cpp:300. "Results" is the golden fifth row;
// GAP: not one of the ui-parity.md 184 templates.
const char* kMainRows[] = {"Adventure", "Quick Race", "2 Player", "Options", "Results"};

// game_state.toml:251, GameOptions.h:3, golden options.png.
// "Vibration" PauseMenu.cpp:846, "Speaker" PauseMenu.cpp:533.
// GAP: ui-parity Sound row is "Volume" (PauseMenu.cpp:527), not "Sound Volume".
// GAP: recomp pause uses "Quit", not "Exit".
const char* kOptionRows[] = {"Vibration", "Speaker", "Sound Volume", "Exit"};

std::vector<const char*> titleStrings(bool pressVisible) {
    std::vector<const char*> s{kPublisher};
    if (pressVisible) s.push_back(kPressStart);
    s.push_back(kCopyright);
    return s;
}

// menu-calibration.md §4.10, oracle-golden-frames.md:107 — harness keyboard
// prompt painted above the main panel; the builder emits it first in the text
// stream. Lowercase chars have no FONTE slot and are skipped by both sides.
const char* kEnterLabel = "button Enter";

std::vector<const char*> mainStrings() {
    auto s = titleStrings(false);
    s.insert(s.begin(), kEnterLabel);
    for (const char* row : kMainRows) s.push_back(row);
    return s;
}

std::vector<const char*> optionStrings() {
    auto s = mainStrings();
    for (const char* row : kOptionRows) s.push_back(row);
    return s;
}

const rt::native::preview::ScreenQuad* findColor(
    const rt::native::preview::UiScreen& screen, const std::array<float, 4>& color) {
    for (const auto& q : screen.quads) {
        bool same = true;
        for (int i = 0; i < 4; ++i) same = same && q.color[i] == color[i];
        if (same) return &q;
    }
    return nullptr;
}

void testStrings() {
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;
    using rt::native::preview::buildTitleScreen;
    glyphsMatch(buildTitleScreen(1920, 1080, true).text, titleStrings(true), "title press");
    glyphsMatch(buildTitleScreen(1920, 1080, false).text, titleStrings(false), "title idle");
    glyphsMatch(buildMainMenu(1920, 1080, 0).text, mainStrings(), "main menu");
    glyphsMatch(buildOptionsScreen(1280, 720, 0).text, optionStrings(), "options");
    // Blink drops only PRESS START; the rest of the title card stays.
    check(buildTitleScreen(800, 600, false).text.size() <
              buildTitleScreen(800, 600, true).text.size(),
          "press-start blink removes glyphs");
}

void testDesignScale() {
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;
    using rt::native::preview::uiDesignScale;
    check(near(uiDesignScale(1080), 1.f), "S(1080)=1");
    check(near(uiDesignScale(720), 720.f / 1080.f), "S(720)");
    check(near(uiDesignScale(400), 0.5f), "S floor 0.5");
    check(near(contractScale(1080) / contractScale(720), 1.5f), "S ratio 1.5");

    const auto hi = buildMainMenu(1920, 1080, 0);
    const auto lo = buildMainMenu(1280, 720, 0);
    const float ratio = contractScale(1080) / contractScale(720);
    check(hi.quads.size() == lo.quads.size(), "quad count stable across scale");
    check(hi.text.size() == lo.text.size(), "text count stable across scale");
    bool scaled = !hi.quads.empty();
    for (std::size_t i = 0; i < hi.quads.size() && i < lo.quads.size(); ++i) {
        scaled = scaled && near(hi.quads[i].x, lo.quads[i].x * ratio);
        scaled = scaled && near(hi.quads[i].y, lo.quads[i].y * ratio);
        scaled = scaled && near(hi.quads[i].width, lo.quads[i].width * ratio);
        scaled = scaled && near(hi.quads[i].height, lo.quads[i].height * ratio);
        for (int c = 0; c < 4; ++c)
            scaled = scaled && hi.quads[i].color[c] == lo.quads[i].color[c];
    }
    for (std::size_t i = 0; i < hi.text.size() && i < lo.text.size(); ++i) {
        scaled = scaled && near(hi.text[i].x, lo.text[i].x * ratio);
        scaled = scaled && near(hi.text[i].y, lo.text[i].y * ratio);
        scaled = scaled && near(hi.text[i].width, lo.text[i].width * ratio);
        scaled = scaled && near(hi.text[i].height, lo.text[i].height * ratio);
        scaled = scaled && hi.text[i].u0 == lo.text[i].u0 && hi.text[i].v1 == lo.text[i].v1;
    }
    check(scaled, "1080p vs 720p offsets scale by S ratio");

    // Same string inventory at 4:3; placement may recenter (ui-parity.md:9).
    check(buildMainMenu(800, 600, 1).text.size() == hi.text.size(), "4:3 text count");
    check(buildOptionsScreen(800, 600, 1).text.size() ==
              buildOptionsScreen(1920, 1080, 1).text.size(),
          "options text count aspect-stable");
}

void testFocusUniqueness() {
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;
    const auto base = buildMainMenu(1920, 1080, 0);
    auto movedDelta = [&](int focus) {
        const auto screen = buildMainMenu(1920, 1080, focus);
        check(screen.quads.size() == base.quads.size(), "focus keeps quad count");
        float delta = 0.f;
        int n = 0;
        bool uniform = true;
        for (std::size_t i = 0; i < base.quads.size() && i < screen.quads.size(); ++i) {
            const float dy = screen.quads[i].y - base.quads[i].y;
            if (std::fabs(dy) > 1e-3f) {
                if (n == 0) delta = dy;
                // Sub-pixel: horn offsets are not binary fractions, so the
                // mapped delta is one row but not bitwise identical.
                uniform = uniform && std::fabs(dy - delta) < 0.05f;
                ++n;
            }
        }
        check(n > 0, "focus moves a row");
        check(uniform, "focus moves a single row");
        return delta;
    };
    // Row pitch 25.8 UI px * 2.25 frame scale at 1080p
    // (menu-calibration.md §2.2: 25.77 measured).
    const float pitch = 25.8f * 2.25f;
    check(near(movedDelta(1), pitch), "focus 1 is one row down");
    check(near(movedDelta(2), pitch * 2.f), "focus 2 is two rows down");
    check(near(movedDelta(4), pitch * 4.f), "focus 4 is the last main row");
    check(fnv(buildMainMenu(1920, 1080, 99)) == fnv(buildMainMenu(1920, 1080, 4)),
          "focus clamps to last main row");
    check(fnv(buildMainMenu(1920, 1080, -3)) == fnv(base), "negative focus clamps to 0");

    const auto opt0 = buildOptionsScreen(1920, 1080, 0);
    const auto opt2 = buildOptionsScreen(1920, 1080, 2);
    int moved = 0;
    float delta = 0.f;
    bool uniform = true;
    check(opt0.quads.size() == opt2.quads.size(), "options focus keeps quad count");
    for (std::size_t i = 0; i < opt0.quads.size() && i < opt2.quads.size(); ++i) {
        const float dy = opt2.quads[i].y - opt0.quads[i].y;
        if (std::fabs(dy) > 1e-3f) {
            if (moved == 0) delta = dy;
            uniform = uniform && std::fabs(dy - delta) < 0.05f;
            ++moved;
        }
    }
    check(moved > 0 && uniform, "options focus moves one row");
    check(near(delta, pitch * 2.f), "options focus 2 is two rows down");
    // The title-menu "Options" highlight stays put while the submenu cursor moves.
    check(buildOptionsScreen(1920, 1080, 3).text.size() == opt0.text.size(),
          "options focus does not change strings");
}

void testMeasuredPanel() {
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;
    using rt::native::preview::kUiPromptRimOuter;
    // Bright rim of main_menu.png: scanout x=866 → UI 433; panel top placement
    // y=272 (menu-calibration.md §4.2, replaces the earlier y=268 estimate).
    // 1920x1080 centered frame: offX=240, scale=2.25, y'=(y+16)*2.25.
    const auto menu = buildMainMenu(1920, 1080, 0);
    const auto* rim = findColor(menu, kUiPromptRimOuter);
    check(rim != nullptr, "main menu has a prompt rim");
    if (rim) {
        check(near(rim->x, 240.f + 433.f * 2.25f), "main panel x");
        check(near(rim->y, (272.f + 16.f) * 2.25f), "main panel y");
        check(near(rim->width, 162.f * 2.25f), "main panel w");
        check(near(rim->height, 143.f * 2.25f), "main panel h");
    }
    const auto options = buildOptionsScreen(1920, 1080, 0);
    // Two rims: options panel (UI x=228) is left of the main panel (UI x=433).
    float left = 1e9f;
    float right = -1e9f;
    int rims = 0;
    for (const auto& q : options.quads) {
        bool same = true;
        for (int i = 0; i < 4; ++i) same = same && q.color[i] == kUiPromptRimOuter[i];
        if (!same) continue;
        ++rims;
        left = std::min(left, q.x);
        right = std::max(right, q.x);
    }
    check(rims == 2, "options screen draws both panels");
    check(left < right, "options panel is left of the main menu");
    check(near(left, 240.f + 228.f * 2.25f), "options panel x");
}

void testDeterministic() {
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;
    using rt::native::preview::buildTitleScreen;
    check(fnv(buildTitleScreen(1920, 1080, true)) == fnv(buildTitleScreen(1920, 1080, true)),
          "title hash stable");
    check(fnv(buildMainMenu(1280, 720, 2)) == fnv(buildMainMenu(1280, 720, 2)),
          "main hash stable");
    check(fnv(buildOptionsScreen(800, 600, 1)) == fnv(buildOptionsScreen(800, 600, 1)),
          "options hash stable");
    check(fnv(buildMainMenu(1920, 1080, 0)) != fnv(buildMainMenu(1920, 1080, 1)),
          "focus changes the hash");
    check(buildTitleScreen(0, 1080, true).quads.empty(), "zero width is empty");
    check(buildMainMenu(1920, -1, 0).text.empty(), "negative height is empty");
    check(buildOptionsScreen(0, 0, 0).quads.empty(), "zero size is empty");
}

void testBackdropQuad() {
    using rt::native::preview::PreviewTexture;
    using rt::native::preview::buildMainMenu;
    using rt::native::preview::buildOptionsScreen;

    const auto mainWithout = buildMainMenu(1280, 720, 2);
    const auto optionsWithout = buildOptionsScreen(1280, 720, 1);
    const PreviewTexture backdrop{2, 2, {1, 2, 3, 4, 5, 6, 7, 8,
                                         9, 10, 11, 12, 13, 14, 15, 16}};
    const auto mainWith = buildMainMenu(1280, 720, 2, &backdrop);
    const auto optionsWith = buildOptionsScreen(1280, 720, 1, &backdrop);

    // Default-compatible calls retain their exact pre-backdrop quad stream.
    check(fnv(mainWithout) == fnv(buildMainMenu(1280, 720, 2, nullptr)),
          "null main backdrop keeps legacy quad list");
    check(fnv(optionsWithout) == fnv(buildOptionsScreen(1280, 720, 1, nullptr)),
          "null options backdrop keeps legacy quad list");
    check(mainWith.quads.size() == mainWithout.quads.size() + 1,
          "main backdrop prepends one quad");
    check(optionsWith.quads.size() == optionsWithout.quads.size() + 1,
          "options backdrop prepends one quad");
    if (!mainWith.quads.empty()) {
        const auto& q = mainWith.quads.front();
        check(q.texture == &backdrop && near(q.x, 0.f) && near(q.y, 0.f) &&
                  near(q.width, 1280.f) && near(q.height, 720.f),
              "main backdrop is full-window textured quad");
        check(q.color == std::array<float, 4>{1.f, 1.f, 1.f, 1.f} &&
                  near(q.u0, 0.f) && near(q.v0, 0.f) && near(q.u1, 1.f) && near(q.v1, 1.f),
              "main backdrop has white tint and full UVs");
    }
    if (!optionsWith.quads.empty())
        check(optionsWith.quads.front().texture == &backdrop,
              "options backdrop is first textured quad");
}

void testParitySpotRead() {
    std::ifstream in("docs/native-reference/ui-parity.md");
    if (!in) in.open("../../docs/native-reference/ui-parity.md");
    if (!in) {
        std::fprintf(stderr, "note: ui-parity.md not reachable from cwd; embedded citations still apply\n");
        return;
    }
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    check(text.find("S=max(.5,H/1080") != std::string::npos, "ui-parity S rule");
    check(text.find("`Speaker`") != std::string::npos, "ui-parity Speaker");
    check(text.find("`Volume`") != std::string::npos, "ui-parity Volume");
    check(text.find("`Vibration`") != std::string::npos, "ui-parity Vibration");
    check(text.find("`Options`") != std::string::npos, "ui-parity Options");
}

}  // namespace

int main() {
    testStrings();
    testDesignScale();
    testFocusUniqueness();
    testMeasuredPanel();
    testDeterministic();
    testBackdropQuad();
    testParitySpotRead();
    if (g_fail) {
        std::fprintf(stderr, "%d/%d ui_menu checks failed\n", g_fail, g_checks);
        return 1;
    }
    std::printf("native_preview_ui_menu_test passed (%d checks)\n", g_checks);
    return 0;
}
