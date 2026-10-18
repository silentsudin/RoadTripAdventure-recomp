#pragma once

#include "assets/glyph_atlas.h"
#include "gameplay/hud_layout.h"
#include "preview/preview.h"
#include "preview/text_metrics.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace rt::native::preview {

// Pure, deterministic HUD normalisation. Invalid maxima intentionally produce
// an empty bar; the compile-time checks below keep its [0, 1] contract honest.
constexpr float raceHudFraction(float value, float maximum) {
    return maximum <= 0.f ? 0.f : value <= 0.f ? 0.f : value >= maximum ? 1.f : value / maximum;
}

static_assert(raceHudFraction(-1.f, 20000.f) == 0.f);
static_assert(raceHudFraction(10000.f, 20000.f) == .5f);
static_assert(raceHudFraction(30000.f, 20000.f) == 1.f);
static_assert(raceHudFraction(1.f, 0.f) == 0.f);

// Builds untextured screen-space race indicators. The speed bar is normalised
// to 20,000 car-physics speed units; elapsed time is normalised to 240 frames
// per configured lap. The caller renders these over its scene with
// previewRender2D().
inline std::vector<ScreenQuad> raceHudQuads(const gameplay::FrameSnapshot& snapshot, int totalLaps,
                                            int width, int height) {
    using Color = std::array<float, 4>;
    const auto rect = [](float x, float y, float w, float h, Color color) {
        return ScreenQuad{x, y, w, h, color};
    };
    const float scale = std::max(1.f, std::min(static_cast<float>(width) / 800.f,
                                                 static_cast<float>(height) / 600.f));
    const float margin = 20.f * scale;
    const float square = 14.f * scale;
    const float gap = 5.f * scale;
    const float barWidth = 180.f * scale;
    const float barHeight = 14.f * scale;
    const Color panel{.015f, .025f, .055f, .72f};
    const Color complete{.18f, .90f, .42f, .95f};
    const Color current{1.f, .76f, .12f, .95f};
    const Color pending{.22f, .29f, .38f, .75f};
    const Color speed{.18f, .72f, 1.f, .95f};
    const Color elapsed{.92f, .36f, .18f, .95f};
    std::vector<ScreenQuad> quads;

    const int laps = std::max(totalLaps, 1);
    const float lapPanelWidth = laps * (square + gap) + margin;
    quads.push_back(rect(margin - 8.f * scale, margin - 8.f * scale, lapPanelWidth, square + 16.f * scale, panel));
    for (int lap = 0; lap < laps; ++lap) {
        Color color = pending;
        if (lap < snapshot.lap) color = complete;
        else if (!snapshot.finished && lap == std::min(snapshot.lap, laps - 1)) color = current;
        quads.push_back(rect(margin + lap * (square + gap), margin, square, square, color));
    }

    const float speedY = margin + square + 24.f * scale;
    quads.push_back(rect(margin, speedY, barWidth, barHeight, panel));
    quads.push_back(rect(margin, speedY, barWidth * raceHudFraction(snapshot.speed, 20000.f), barHeight, speed));

    const float timeY = speedY + barHeight + 12.f * scale;
    quads.push_back(rect(margin, timeY, barWidth, barHeight, panel));
    const float timeBudget = static_cast<float>(laps) * 240.f;
    quads.push_back(rect(margin, timeY, barWidth * raceHudFraction(static_cast<float>(snapshot.elapsedFrames), timeBudget),
                         barHeight, elapsed));

    if (snapshot.finished) {
        const std::array<Color, 4> finishColors{{
            {.22f, .95f, .48f, .78f}, {.25f, .66f, 1.f, .78f},
            {1.f, .62f, .16f, .78f}, {.82f, .34f, .92f, .78f},
        }};
        const int colorIndex = std::clamp(snapshot.rank, 0, static_cast<int>(finishColors.size()) - 1);
        quads.push_back(rect(0.f, static_cast<float>(height) - 18.f * scale,
                             static_cast<float>(width), 18.f * scale, finishColors[colorIndex]));
    }
    return quads;
}

// ---------------------------------------------------------------------------
// FONTE text placement (RT_APP_HUDTEXT=1 presentation path).
// ---------------------------------------------------------------------------

// Builds glyph quads for an ASCII string in target-pixel space. The origin
// (uiX, uiY) is in the authentic 640x448 UI space (docs/native-reference/
// hud-layout.md) and is mapped to the target with the 4:3 frame convention
// (+16 px vertical centring, uniform x = W/640, y = H/480 scaling). Cell
// and advance are one FONTE cell (16x24 UI px; docs/native-reference/
// sys-gsl-packs.md §4 corrected 2026-10-16). Characters without a proven
// glyph (space, outside '!'..'~') advance without emitting a quad.
inline std::vector<HudQuad> fonteTextQuads(const char* text, float uiX, float uiY,
                                           const std::array<float, 4>& color,
                                           int width, int height) {
    std::vector<HudQuad> quads;
    if (!text || width <= 0 || height <= 0) return quads;
    // Never-stretch HUD policy (output-modes.md Q2): one uniform factor over
    // the 640x480 display frame, centred.  At 4:3 sizes this equals the former
    // per-axis math exactly (1.25 at 800x600), so default pixels are
    // unchanged; non-4:3 windows gain centring instead of shear.
    const float s = std::min(static_cast<float>(width) / rt::native::gameplay::kFrameWidth,
                             static_cast<float>(height) / rt::native::gameplay::kFrameHeight);
    const float offX = (static_cast<float>(width) - rt::native::gameplay::kFrameWidth * s) * 0.5f;
    const float offY = (static_cast<float>(height) - rt::native::gameplay::kFrameHeight * s) * 0.5f;
    float x = offX + uiX * s;
    const float y = offY + (uiY + rt::native::gameplay::kFrameYOffset) * s;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * s;
    const float ch = static_cast<float>(rt::native::assets::kGlyphCellHeight) * s;
    // Proportional pen (W3-01): the advance is the glyph's uiGlyphWidths
    // entry (96 u8 values at ELF 0x0029A880..0x0029A8DF, re-verified from
    // the ROM), while the inked cell stays a fixed 16x24 slot. Unloaded
    // cells and non-ASCII bytes advance without a quad, matching the
    // original cell-walk. Process-static table: the disc is read once at
    // first use (RT_ELF override, else disc path, else authored fallback).
    static const GlyphTable kGlyphMetrics{GlyphTable::fromRuntime()};
    for (const char* p = text; *p; ++p) {
        const int slot = rt::native::assets::fonteSlotForChar(*p);
        if (slot >= 0) {
            const auto uv = rt::native::assets::fonteSlotUv(slot);
            quads.push_back({x, y, cw, ch, uv.u0, uv.v0, uv.u1, uv.v1, color});
        }
        x += kGlyphMetrics.glyph(static_cast<unsigned char>(*p)).advance * s;
    }
    return quads;
}

// Appends text built at (uiX, uiY) to `out`.
inline void appendFonteText(std::vector<HudQuad>& out, const char* text, float uiX, float uiY,
                           const std::array<float, 4>& color, int width, int height) {
    auto quads = fonteTextQuads(text, uiX, uiY, color, width, height);
    out.insert(out.end(), quads.begin(), quads.end());
}

// Race HUD text: MM:SS elapsed on the digit strip (placement CONFIRMED in
// kHudElements from 0x233F10 builders). The shipped HUD samples these from
// SCORE.GSL page glyphs; the native presentation renders the same string with
// the proven FONTE glyph shapes at the proven placement until SCORE-page
// binding lands. The former "LAP n/m" label was removed 2026-10-17: the
// oracle race HUD (race_hud_driving.png) shows a TOTAL TIME banner + rank
// badge in that corner, not lap text, and the label collided with the timer
// digits at the same row. Lap state stays on the lap-squares panel.
inline std::vector<HudQuad> raceHudTextQuads(const gameplay::FrameSnapshot& snapshot,
                                             int /*totalLaps*/, int width, int height) {
    std::vector<HudQuad> quads;
    namespace hud = rt::native::gameplay;
    constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, .95f};
    char buf[32];
    const unsigned sec = snapshot.elapsedFrames / 60u;
    std::snprintf(buf, sizeof(buf), "%u:%02u", sec / 60u, sec % 60u);
    if (const auto* digits = hud::findHudElement(hud::HudElement::RaceTimeDigits))
        appendFonteText(quads, buf, digits->ui.x, digits->ui.y, kWhite, width, height);
    return quads;
}

// Presentation selector for the experimental SCORE/METER text route. These
// values select native presentation content only; they do not identify a
// shipped PS2 UI state.
enum class ScoreMeterPage : std::uint8_t {
    None,
    Score,
    Meter,
};

// SCORE-page text for the livery-test presentation.
//
// CONFIRMED: the timer starts at RaceTimeDigits (64,56), the ElementC digit
// strip emitted by 0x22F750/0x233F10 (hud-layout.md §1.1).
// AUTHORED: SCORE at (192,216), MONEY at (352,216), their labels, six-digit
// zero padding, and the association of `score`/`money` with this page. The
// evidence ledger contains no livery-test SCORE-page builder, score/money
// placement, or field source; do not treat those presentation choices as ROM
// facts. `money` is clamped to the proven live-counter cap only so the UI has
// a bounded display contract.
inline std::vector<HudQuad> scorePageTextQuads(const gameplay::FrameSnapshot& snapshot,
                                               int score, int money, int width, int height) {
    namespace hud = rt::native::gameplay;
    constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, .95f};
    constexpr float kAuthoredScoreX = 192.f;
    constexpr float kAuthoredScoreY = 216.f;
    constexpr float kAuthoredMoneyX = 352.f;
    constexpr float kAuthoredMoneyY = 216.f;
    std::vector<HudQuad> quads;
    char buf[32];

    const int shownScore = std::clamp(score, 0, hud::kHudMoneyCap);
    std::snprintf(buf, sizeof(buf), "SCORE %06d", shownScore);
    appendFonteText(quads, buf, kAuthoredScoreX, kAuthoredScoreY, kWhite, width, height);

    // The timer placement is proven; this FONTE spelling remains a native
    // stand-in for the SCORE.GSL 16x16 digit sprites.
    const unsigned seconds = snapshot.elapsedFrames / 60u;
    std::snprintf(buf, sizeof(buf), "%u:%02u", seconds / 60u, seconds % 60u);
    if (const auto* digits = hud::findHudElement(hud::HudElement::RaceTimeDigits))
        appendFonteText(quads, buf, digits->ui.x, digits->ui.y, kWhite, width, height);

    const int shownMoney = std::clamp(money, 0, hud::kHudMoneyCap);
    std::snprintf(buf, sizeof(buf), "MONEY %06d", shownMoney);
    appendFonteText(quads, buf, kAuthoredMoneyX, kAuthoredMoneyY, kWhite, width, height);
    return quads;
}

// METER-page speed text for the livery-test presentation.
// CONFIRMED: DriveSpeedDigits is (532,385), emitted by 0x24E1A0 at the 1P
// dial anchor (hud-layout.md §1.2). AUTHORED: converting FrameSnapshot::speed
// to an integer and presenting it as three decimal digits; the ledger does not
// pin the game's speed-value conversion or a livery-test METER page state.
inline std::vector<HudQuad> meterPageTextQuads(const gameplay::FrameSnapshot& snapshot,
                                               int width, int height) {
    namespace hud = rt::native::gameplay;
    constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, .95f};
    std::vector<HudQuad> quads;
    char buf[8];
    const int shownSpeed = std::clamp(static_cast<int>(snapshot.speed), 0, 999);
    std::snprintf(buf, sizeof(buf), "%03d", shownSpeed);
    if (const auto* digits = hud::findHudElement(hud::HudElement::DriveSpeedDigits))
        appendFonteText(quads, buf, digits->ui.x, digits->ui.y, kWhite, width, height);
    return quads;
}

// Selects exactly one SCORE/METER presentation builder. `score` and `money`
// are intentionally caller-supplied until the livery-test state has proven
// snapshot fields; Meter ignores them.
inline std::vector<HudQuad> scoreMeterPageTextQuads(ScoreMeterPage page,
                                                    const gameplay::FrameSnapshot& snapshot,
                                                    int score, int money, int width, int height) {
    switch (page) {
        case ScoreMeterPage::Score:
            return scorePageTextQuads(snapshot, score, money, width, height);
        case ScoreMeterPage::Meter:
            return meterPageTextQuads(snapshot, width, height);
        case ScoreMeterPage::None:
            return {};
    }
    return {};
}

// Time-of-day readout at the TownClockText placement ("%2d:%02d", matching
// the shipped digital clock; ticks per hour/minute per DAT_00335730 —
// kClockTicksPerHour/kClockTicksPerMinute).
inline std::vector<HudQuad> townClockTextQuads(std::uint32_t clockTicks, int width, int height) {
    namespace hud = rt::native::gameplay;
    constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, .95f};
    std::vector<HudQuad> quads;
    char buf[16];
    const unsigned hour = clockTicks / static_cast<unsigned>(hud::kClockTicksPerHour);
    const unsigned minute = (clockTicks % static_cast<unsigned>(hud::kClockTicksPerHour)) /
                           static_cast<unsigned>(hud::kClockTicksPerMinute);
    std::snprintf(buf, sizeof(buf), "%2u:%02u", hour % 24u, minute);
    if (const auto* el = hud::findHudElement(hud::HudElement::TownClockText))
        appendFonteText(quads, buf, el->ui.x, el->ui.y, kWhite, width, height);
    return quads;
}

}  // namespace rt::native::preview
