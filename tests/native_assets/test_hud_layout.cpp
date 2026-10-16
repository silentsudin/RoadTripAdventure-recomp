// Authored HUD layout contract tests. No game data: pins the extracted
// geometry of docs/native-reference/hud-layout.md (evidence ledger) against
// the constexpr tables in src/native/gameplay/hud_layout.h.
#include "../../src/native/gameplay/hud_layout.h"
#include "../../src/native/preview/race_hud.h"

#include <cstdio>
#include <cstdint>

using namespace rt::native::gameplay;

namespace {

int g_checks = 0;
int g_fail = 0;

void check(bool ok) {
    ++g_checks;
    if (!ok) {
        std::fprintf(stderr, "FAIL hud_layout check #%d\n", g_checks);
        ++g_fail;
    }
}

constexpr bool rectEq(const HudRect& r, float x, float y, float w, float h) {
    return r.x == x && r.y == y && r.w == w && r.h == h;
}

void testAnchoredGeometry() {
    // Drive HUD (1P anchor (492,336) from table 0x2A9AF0; handler 0x24E1A0).
    const HudElementSpec* dial = findHudElement(HudElement::DriveDial);
    check(dial != nullptr && rectEq(dial->ui, 492.f, 336.f, 132.f, 96.f));
    check(dial != nullptr && dial->provenance == HudProvenance::Confirmed);
    check(dial != nullptr && dial->atlas == HudAtlas::DriveMeterPages);

    const HudElementSpec* speed = findHudElement(HudElement::DriveSpeedDigits);
    check(speed != nullptr && rectEq(speed->ui, 532.f, 385.f, 58.f, 26.f));
    // Rightmost 20px digit starts at anchor+78: right edge 492+78+20 = 590.
    check(speed != nullptr && speed->ui.x + speed->ui.w == 590.f);

    const HudElementSpec* gear = findHudElement(HudElement::DriveGearBar);
    check(gear != nullptr && rectEq(gear->ui, 499.f, 412.f, 100.f, 18.f));

    // Race HUD (ElementC at (60,52) from 0x233F10; panel templates 0x2EBD40..).
    const HudElementSpec* lapPanel = findHudElement(HudElement::RaceLapTimePanel);
    check(lapPanel != nullptr && rectEq(lapPanel->ui, 60.f, 52.f, 132.f, 24.f));
    // Panel width = 18 left cap + 12*8 middle + 18 right cap.
    check(lapPanel != nullptr && lapPanel->ui.w == 18.f + 12.f * 8.f + 18.f);

    const HudElementSpec* timeDigits = findHudElement(HudElement::RaceTimeDigits);
    check(timeDigits != nullptr && rectEq(timeDigits->ui, 64.f, 56.f, 114.f, 16.f));
    // Rightmost digit column starts at panel x + 102 (MM tens).
    check(timeDigits != nullptr && timeDigits->ui.x == lapPanel->ui.x + 4.f);
    check(timeDigits != nullptr &&
          timeDigits->ui.x + timeDigits->ui.w == lapPanel->ui.x + 118.f);

    const HudElementSpec* lapBadge = findHudElement(HudElement::RaceLapBadge);
    check(lapBadge != nullptr && rectEq(lapBadge->ui, 34.f, 52.f, 24.f, 24.f));
    check(lapBadge != nullptr && lapBadge->ui.x == lapPanel->ui.x - 26.f);

    // Minimap bezel (0x249208) + its SCISSOR interior.
    const HudElementSpec* frame = findHudElement(HudElement::TownMinimapFrame);
    check(frame != nullptr && rectEq(frame->ui, 30.f, 304.f, 144.f, 120.f));
    check(frame != nullptr && frame->ui.w == 16.f + 112.f + 16.f);
    const HudElementSpec* window = findHudElement(HudElement::TownMinimapWindow);
    check(window != nullptr && rectEq(window->ui, 45.f, 312.f, 113.f, 102.f));
    check(window != nullptr && window->ui.x >= frame->ui.x);
    check(window != nullptr && window->ui.y >= frame->ui.y);
    check(window != nullptr &&
          window->ui.x + window->ui.w <= frame->ui.x + frame->ui.w);
    check(window != nullptr &&
          window->ui.y + window->ui.h <= frame->ui.y + frame->ui.h);

    // Message window (0x2028B0 called with y=0x158) and money plate.
    const HudElementSpec* msg = findHudElement(HudElement::MessageWindow);
    check(msg != nullptr && rectEq(msg->ui, 32.f, 328.f, 576.f, 104.f));
    const HudElementSpec* plate = findHudElement(HudElement::ShopMoneyPlate);
    check(plate != nullptr && rectEq(plate->ui, 435.f, 26.f, 154.f, 32.f));
    const HudElementSpec* money = findHudElement(HudElement::ShopMoneyDigits);
    check(money != nullptr && rectEq(money->ui, 440.f, 32.f, 96.f, 20.f));
    check(money != nullptr && money->ui.x >= plate->ui.x);
    check(money != nullptr && money->ui.x + money->ui.w <= plate->ui.x + plate->ui.w);
}

void testNormalizationBounds() {
    for (std::size_t i = 0; i < kHudElementCount; ++i) {
        float nx = 0.f, ny = 0.f, nw = 0.f, nh = 0.f;
        normalizedFrame640x480(kHudElements[i].ui, nx, ny, nw, nh);
        check(nx >= 0.f && nx <= 1.f);
        check(ny >= 0.f && ny <= 1.f);
        check(nw >= 0.f && nw <= 1.f);
        check(nh >= 0.f && nh <= 1.f);
        check(nx + nw <= 1.0f + 1e-6f);
        check(ny + nh <= 1.0f + 1e-6f);
        // The centring offset must appear exactly once in y.
        float nyRaw = kHudElements[i].ui.y / kFrameHeight;
        check(ny == (kHudElements[i].ui.y + kFrameYOffset) / kFrameHeight);
        check(ny > nyRaw || kHudElements[i].ui.y == 0.f);
    }
    // Spot-check the mapping itself.
    float nx = 0.f, ny = 0.f, nw = 0.f, nh = 0.f;
    HudRect dial{492.f, 336.f, 132.f, 96.f};
    normalizedFrame640x480(dial, nx, ny, nw, nh);
    check(nx == 492.f / 640.f);
    check(ny == (336.f + 16.f) / 480.f);
    check(nh == 96.f / 480.f);
}

void testSentinelsAndConventions() {
    check(kLapHidden == -1);
    check(kHudMoneyCap == 999999);
    check(kHudMoneyNewGame == 1000);
    check(kClockTicksPerHour == 9000 && kClockTicksPerMinute == 150);
    check(kTimeFramesPerHour == 3600 && kTimeFramesPerSecond == 60);
    check(kTimeFramesMax == 360000);
    check(kTimeBcdUnderflow == 0xAAAAAAu && kTimeBcdOverflow == 0x995999u);
    check(kItemFlashAlphaMax == 0x80 && kItemFlashAlphaStepUp == 0x40 &&
          kItemFlashAlphaStepDown == 0x20);
    check(kItemFlashRgbTopRow == 0x4334ECu &&
          kItemFlashRgbBottomRow == 0x50E800u);
    check(kDriveGearBankOffset == 15);
    for (int i = 0; i < 4; ++i) {
        check(kTimePaletteBanks[i] <= 15);  // TEX2 CSA is 4 bits
    }
    check(kFrameYOffset == 16.f);
    check(kHudUiWidth == 640.f && kHudUiHeight == 448.f);
}

void testGlyphSetTables() {
    check(kHudGlyphSetCount >= 15);
    for (std::size_t i = 0; i < kHudGlyphSetCount; ++i) {
        const HudGlyphSet& g = kHudGlyphSets[i];
        check(g.cellW > 0 && g.cellH > 0);
        check(g.count > 0);
        check(g.usage != nullptr);
        // Cell extents stay inside their atlas page. SCORE pages are 256x256,
        // METER entry 0 is 256x128. The UiBatch font atlas is 512x512 but the
        // shipped digit formulas intentionally run past TW=9 and wrap, so its
        // bounds are not enforceable here (values are pinned instead).
        if (g.atlas == HudAtlas::UiBatchFontAtlas) {
            check(g.vBase + g.cellH <= 512u);
            continue;
        }
        const std::uint32_t atlasW = 256u;
        const std::uint32_t atlasH =
            g.atlas == HudAtlas::DriveMeterPages ? 128u : 256u;
        if (g.uPitch == 0) {
            check(g.uBase + g.cellW <= atlasW);
        } else {
            // Grid sets wrap per row (u = (d % cols) * pitch).
            const std::uint32_t cols = atlasW / g.uPitch;
            const std::uint32_t maxU = g.uBase +
                ((g.count - 1) % cols) * g.uPitch + g.cellW;
            check(maxU <= atlasW);
        }
        const std::uint32_t colsForRows =
            g.uPitch == 0 ? 1u : atlasW / g.uPitch;
        const std::uint32_t rows = g.vPitch == 0
                                       ? 1u
                                       : (g.count + colsForRows - 1) / colsForRows;
        check(g.vBase + (rows - 1) * g.vPitch + g.cellH <= atlasH);
    }
    // Drive speed digits: 20x26 cells on the v=102 row of the METER page
    // (v 102..128 exactly fills the 128-tall page).
    const HudGlyphSet& speed = kHudGlyphSets[10];
    check(speed.atlas == HudAtlas::DriveMeterPages);
    check(speed.cellW == 20 && speed.cellH == 26 && speed.vBase == 102);
    // Money digits: u = d*16 + 0x1B8 on the UiBatch atlas, rows v=200/220.
    const HudGlyphSet& money = kHudGlyphSets[14];
    check(money.uBase == 0x1B8 && money.vBase == 200 && money.cellW == 16 &&
          money.cellH == 20 && money.atlas == HudAtlas::UiBatchFontAtlas);
}

void testScoreMeterTextPins() {
    using rt::native::assets::fonteSlotForChar;
    using rt::native::preview::ScoreMeterPage;
    using rt::native::preview::meterPageTextQuads;
    using rt::native::preview::scoreMeterPageTextQuads;
    using rt::native::preview::scorePageTextQuads;

    // FONTE's stream is 31 characters per row, with the final column reserved
    // for an icon; numeric glyphs occupy the first row after '!'.
    for (char digit = '0'; digit <= '9'; ++digit)
        check(fonteSlotForChar(digit) == digit - '!');

    FrameSnapshot snapshot{};
    snapshot.elapsedFrames = 3723;  // 1:02
    snapshot.speed = 87.f;

    const auto score43 = scorePageTextQuads(snapshot, 42, 9001, 640, 480);
    // "SCORE 000042" + "1:02" + "MONEY 009001": spaces advance without quads.
    check(score43.size() == 26u);
    check(score43[0].x == 192.f && score43[0].y == 232.f);
    check(score43[11].x == 64.f && score43[11].y == 72.f);
    check(score43[15].x == 352.f && score43[15].y == 232.f);

    const auto scoreWide = scorePageTextQuads(snapshot, 42, 9001, 1280, 720);
    check(scoreWide.size() == score43.size());
    // Never-stretch policy (output-modes.md Q2): uniform s = min(1280/640,
    // 720/480) = 1.5 with the authored column centred (offX = (1280-960)/2
    // = 160).  The former pins here encoded the per-axis shear (x*2.0),
    // which the policy removes; y values are unchanged by the fix.
    check(scoreWide[0].x == 448.f && scoreWide[0].y == 348.f);
    check(scoreWide[11].x == 256.f && scoreWide[11].y == 108.f);
    check(scoreWide[15].x == 688.f && scoreWide[15].y == 348.f);

    const auto meter = meterPageTextQuads(snapshot, 640, 480);
    check(meter.size() == 3u);
    check(meter[0].x == 532.f && meter[0].y == 401.f);

    check(scoreMeterPageTextQuads(ScoreMeterPage::None, snapshot, 42, 9001, 640, 480).empty());
    check(scoreMeterPageTextQuads(ScoreMeterPage::Score, snapshot, 42, 9001, 640, 480).size() ==
          score43.size());
    check(scoreMeterPageTextQuads(ScoreMeterPage::Meter, snapshot, 42, 9001, 640, 480).size() ==
          meter.size());
}

}  // namespace

int main() {
    testAnchoredGeometry();
    testNormalizationBounds();
    testSentinelsAndConventions();
    testGlyphSetTables();
    testScoreMeterTextPins();
    if (g_fail) {
        std::fprintf(stderr, "%d/%d checks failed\n", g_fail, g_checks);
        return 1;
    }
    std::printf("%d checks passed\n", g_checks);
    return 0;
}
