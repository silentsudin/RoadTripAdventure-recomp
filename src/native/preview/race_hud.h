#pragma once

#include "assets/glyph_atlas.h"
#include "gameplay/hud_layout.h"
#include "preview/preview.h"

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
    const float sx = static_cast<float>(width) / rt::native::gameplay::kFrameWidth;
    const float sy = static_cast<float>(height) / rt::native::gameplay::kFrameHeight;
    float x = uiX * sx;
    const float y = (uiY + rt::native::gameplay::kFrameYOffset) * sy;
    const float cw = static_cast<float>(rt::native::assets::kGlyphCellWidth) * sx;
    const float ch = static_cast<float>(rt::native::assets::kGlyphCellHeight) * sy;
    for (const char* p = text; *p; ++p) {
        const int slot = rt::native::assets::fonteSlotForChar(*p);
        if (slot < 0) { x += cw; continue; }
        const auto uv = rt::native::assets::fonteSlotUv(slot);
        quads.push_back({x, y, cw, ch, uv.u0, uv.v0, uv.u1, uv.v1, color});
        x += cw;
    }
    return quads;
}

// Appends text built at (uiX, uiY) to `out`.
inline void appendFonteText(std::vector<HudQuad>& out, const char* text, float uiX, float uiY,
                           const std::array<float, 4>& color, int width, int height) {
    auto quads = fonteTextQuads(text, uiX, uiY, color, width, height);
    out.insert(out.end(), quads.begin(), quads.end());
}

// Race HUD text stand-in: "LAP n/m" left of the ElementC panel and MM:SS
// elapsed on the digit strip (placements CONFIRMED in kHudElements from
// 0x233F10 builders). The shipped HUD samples these from SCORE.GSL page
// glyphs; the native presentation renders the same strings with the proven
// FONTE glyph shapes at the proven placements until SCORE-page binding lands.
inline std::vector<HudQuad> raceHudTextQuads(const gameplay::FrameSnapshot& snapshot,
                                             int totalLaps, int width, int height) {
    std::vector<HudQuad> quads;
    namespace hud = rt::native::gameplay;
    constexpr std::array<float, 4> kWhite{1.f, 1.f, 1.f, .95f};
    const int laps = std::max(totalLaps, 1);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "LAP %d/%d", std::min(snapshot.lap + 1, laps), laps);
    if (const auto* panel = hud::findHudElement(hud::HudElement::RaceLapTimePanel))
        appendFonteText(quads, buf, panel->ui.x - 26.f, panel->ui.y, kWhite, width, height);
    const unsigned sec = snapshot.elapsedFrames / 60u;
    std::snprintf(buf, sizeof(buf), "%u:%02u", sec / 60u, sec % 60u);
    if (const auto* digits = hud::findHudElement(hud::HudElement::RaceTimeDigits))
        appendFonteText(quads, buf, digits->ui.x, digits->ui.y, kWhite, width, height);
    return quads;
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
