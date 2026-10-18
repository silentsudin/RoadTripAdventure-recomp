// SPEED LAW acceptance (w2_hud_layout_test). No ROM or disc needed.
//
//   V = floor(speed_word / 55.07), clamped to 0..186 (coordinator ruling 2026-10-18;
//   ELF METER0 atlas inspection + oracle HUD goldens).  Zero-suppressed digits, V = 0 draws
//   one "0"; ones digit at UI (573,388), step -17 per higher digit, 16x23 cells on METER0 at
//   u = 140 + 16*(d%7), v = 23*(d/7).  Exit 0 only when every check passes.

#include "gameplay/hud_layout.h"
#include "gameplay/hud_live.h"

#include <cstdio>
#include <vector>

namespace hd = rt::native::gameplay;
namespace hl = rt::native::gameplay::hud_live;

namespace {

int g_checks = 0;
int g_fail = 0;

void check(bool ok, const char* what) {
    ++g_checks;
    if (!ok) {
        ++g_fail;
        std::fprintf(stderr, "FAIL %s\n", what);
    }
}

struct Fixture {
    float speedWord;
    int mph;
};

constexpr Fixture kFixtures[] = {
    {0.f, 0},          {55.f, 0},       {55.07f, 1},     {5506.999f, 99},
    {5507.f, 100},     {10243.f, 185},  {10244.f, 186},  {999999.f, 186},
};

// Speed digit quads of the 1P gauge (buildGaugeHud, viewport 0): METER0 16x23 cells at y 388.
std::vector<hl::DrawQuad> speedQuads(float speedWord) {
    hl::GaugeInput in;
    in.speedRaw = speedWord;
    std::vector<hl::DrawQuad> out;
    for (const hl::DrawQuad& q : hl::buildGaugeHud(in)) {
        if (q.page == hl::Page::Meter0 && q.w == hl::kSpeedDigitW && q.h == hl::kSpeedDigitH &&
            q.y == hl::kSpeedOnesY)
            out.push_back(q);
    }
    return out;
}

// Digits of V, ones first (V = 0 -> one digit).
std::vector<int> digitsOf(int mph) {
    std::vector<int> d;
    int v = mph;
    do {
        d.push_back(v % 10);
        v /= 10;
    } while (v != 0);
    return d;
}

void checkRender(const Fixture& f) {
    const auto quads = speedQuads(f.speedWord);
    const auto digits = digitsOf(f.mph);
    bool ok = quads.size() == digits.size();
    for (std::size_t i = 0; ok && i < digits.size(); ++i) {
        const int d = digits[i];
        const int x = hl::kSpeedOnesX + hl::kSpeedDigitStep * static_cast<int>(i);
        const int u = hl::kSpeedAtlasU0 + hl::kSpeedDigitW * (d % hl::kSpeedAtlasCols);
        const int v = hl::kSpeedDigitH * (d / hl::kSpeedAtlasCols);
        ok = quads[i].x == x && quads[i].u == u && quads[i].v == v && quads[i].csa == 0 &&
             quads[i].texEntry == 0;
    }
    if (!ok) {
        ++g_fail;
        std::fprintf(stderr, "FAIL render word=%.3f want_mph=%d quads=%zu\n", static_cast<double>(f.speedWord),
                     f.mph, quads.size());
    }
    ++g_checks;
}

void testLawConstants() {
    check(hl::kSpeedDivisorMph == 55.07f, "law divisor is 55.07");
    check(hl::kDefaultSpeedDivisor == hl::kSpeedDivisorMph, "default divisor is the law divisor");
    check(hl::kSpeedMax == 186 && hl::kShippedSpeedometerCap == hl::kSpeedMax, "speed clamp is 186");
    check(hl::kSpeedOnesX == 573 && hl::kSpeedOnesY == 388 && hl::kSpeedDigitStep == -17,
          "ones digit at (573,388), step -17");
    check(hl::kSpeedDigitW == 16 && hl::kSpeedDigitH == 23, "METER0 speed cell is 16x23");
    check(hl::kSpeedAtlasU0 == 140 && hl::kSpeedAtlasVStep == 23 && hl::kSpeedAtlasCols == 7,
          "METER0 speed atlas grid: u0 140, v step 23, 7 columns");
    // Three digits span x 539..589; the ones digit sits inside the mph badge box 556..604 x 388..416.
    check(hl::kSpeedOnesX + 2 * hl::kSpeedDigitStep == 539, "hundreds digit left edge at 539");
    check(hl::kSpeedOnesX + hl::kSpeedDigitW == 589, "ones digit right edge at 589");
    check(hl::kSpeedBadgeX == 556 && hl::kSpeedBadgeX + hl::kSpeedBadgeW == 604, "badge x 556..604");
    check(hl::kSpeedBadgeY == 388 && hl::kSpeedBadgeY + hl::kSpeedBadgeH == 416, "badge y 388..416");
    check(hl::kSpeedOnesX >= hl::kSpeedBadgeX && hl::kSpeedOnesX + hl::kSpeedDigitW <= hl::kSpeedBadgeX + hl::kSpeedBadgeW,
          "ones digit inside badge");
}

void testLayoutRecords() {
    const hd::HudElementSpec* sp = hd::findHudElement(hd::HudElement::DriveSpeedDigits);
    check(sp != nullptr && sp->ui.x == 539.f && sp->ui.y == 388.f && sp->ui.w == 50.f && sp->ui.h == 23.f,
          "DriveSpeedDigits bbox 539,388,50x23");
    check(sp != nullptr && sp->ui.x + sp->ui.w == hl::kSpeedOnesX + hl::kSpeedDigitW,
          "DriveSpeedDigits right edge = ones digit right edge");
    check(sp != nullptr && sp->atlas == hd::HudAtlas::DriveMeterPages, "DriveSpeedDigits on METER page");

    // METER0 speed glyph rows: the last two table entries (row 0 = digits 0-6, row 1 = 7-9).
    const hd::HudGlyphSet& r0 = hd::kHudGlyphSets[hd::kHudGlyphSetCount - 2];
    const hd::HudGlyphSet& r1 = hd::kHudGlyphSets[hd::kHudGlyphSetCount - 1];
    check(r0.atlas == hd::HudAtlas::DriveMeterPages && r1.atlas == hd::HudAtlas::DriveMeterPages,
          "METER0 speed rows on METER page");
    check(r0.uBase == hl::kSpeedAtlasU0 && r0.vBase == 0 && r0.cellW == hl::kSpeedDigitW &&
              r0.cellH == hl::kSpeedDigitH && r0.uPitch == hl::kSpeedDigitW && r0.count == hl::kSpeedAtlasCols,
          "METER0 speed row 0 (digits 0-6)");
    check(r1.uBase == hl::kSpeedAtlasU0 && r1.vBase == hl::kSpeedAtlasVStep && r1.cellW == hl::kSpeedDigitW &&
              r1.cellH == hl::kSpeedDigitH && r1.uPitch == hl::kSpeedDigitW && r1.count == 3,
          "METER0 speed row 1 (digits 7-9)");
    for (int d = 0; d < 10; ++d) {
        const hd::HudGlyphSet& g = d < hl::kSpeedAtlasCols ? r0 : r1;
        const int k = d < hl::kSpeedAtlasCols ? d : d - hl::kSpeedAtlasCols;
        const int u = g.uBase + g.uPitch * k;
        const int v = g.vBase;
        check(u == hl::kSpeedAtlasU0 + hl::kSpeedDigitW * (d % hl::kSpeedAtlasCols) &&
                  v == hl::kSpeedDigitH * (d / hl::kSpeedAtlasCols),
              "METER0 digit cell origin");
    }
}

void testLawMph() {
    for (const Fixture& f : kFixtures) {
        const int got = hl::hostMph(f.speedWord);
        ++g_checks;
        if (got != f.mph) {
            ++g_fail;
            std::fprintf(stderr, "FAIL mph word=%.3f want=%d got=%d\n", static_cast<double>(f.speedWord), f.mph,
                         got);
        }
        checkRender(f);
    }
    check(hl::speedCells(0, 0x59, 0x11).size() == 1, "V=0 draws one cell");
    check(hl::speedCells(99, 0x59, 0x11).size() == 2, "V=99 draws two cells");
    check(hl::speedCells(100, 0x59, 0x11).size() == 3, "V=100 draws three cells");
    check(hl::speedCells(186, 0x59, 0x11).size() == 3, "V=186 draws three cells");
}

}  // namespace

int main() {
    testLawConstants();
    testLayoutRecords();
    testLawMph();
    std::printf("W4-10 hud_layout SPEED LAW: %d checks, %d failed -> %s\n", g_checks, g_fail,
                g_fail == 0 ? "PASS" : "FAIL");
    return g_fail == 0 ? 0 : 1;
}
