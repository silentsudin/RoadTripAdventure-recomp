#pragma once

#include "assets/glyph_atlas.h"
#include "gameplay/hud_layout.h"
#include "gameplay/hud_live.h"
#include "gameplay/race_hud_layout.h"
#include "preview/preview.h"
#include "preview/text_metrics.h"

#include <algorithm>
#include <array>
#include <cmath>
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
// SUPERSEDED (W4-10/W4-06): the 1P speed text is the mph law — ones digit at
// (573,388), step -17, 16x23 quads into METER0 u=(140+16*(d%7))/256,
// v=(23*(d/7))/128 (0x2498FC/0x249AB8/0x249AE8; W4-06 cell box 556..589 x
// 388..411 matches the goldens). The old (532,385) 3-digit reading is wrong.
// AUTHORED here: converting FrameSnapshot::speed to an integer and presenting
// it as three decimal digits for the livery-test presentation only.
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

// ---------------------------------------------------------------------------
// W4-06 race HUD element layout (gameplay/race_hud_layout.h).
//
// ROM arena records -> absolute design px (640x448) -> target-pixel HudQuads,
// one call per texture pass so the caller can upload each page once
// (previewUploadFontAtlas + previewRender2DTextured, as the main.cpp banner and
// dial passes do). The mph numeral (coordinator law) and the speedometer needle
// (GAP: needle law unresolved) are never emitted here.
// ---------------------------------------------------------------------------

// Live inputs. Times are the values handed to ElementC 0x22F750 (frames).
// The HUD shows the total race time minus one frame, which is
// hud_live::raceHudTimerValue (verified 2/2 on the goldens, hud-live.md).
struct RaceHudLayoutState {
    std::int32_t totalTime = 0;  // TOTAL ElementC value; 0 shows the dimmed "--" BCD 0xAAAAAA
    std::int32_t lapTime = 0;    // LAP ElementC value (current-lap frames)
    int lap = 1;                 // lap badge number; -1 hides the badge, 0 draws the flag glyph
    int rank = 0;                // zero-based place (24th of 24 = 23)
    int totalCars = 24;          // ElementD total; != 0 selects the large variant (goldens)
    int gear = 1;                // car+0x1FF: 0 = R, 1..6
    bool showLapBlock = true;    // LAPTIME banner + LAP ElementC (+0x84A0 / +0x84B4 enables)
    bool showRank = true;        // ElementD +0x84FC enable
};

// Convenience: state for a native frame. Gear now comes from the session
// (FrameSnapshot::gear, W6-04); the caller still owns lap time and place.
// FrameSnapshot::rank is the time rank (getRaceTimeRank), not the race place,
// so it is not used here.
inline RaceHudLayoutState raceHudLayoutStateFor(const gameplay::FrameSnapshot& snapshot, std::int32_t lapFrames,
                                                int place0, int totalCars, int gear) {
    RaceHudLayoutState s;
    s.totalTime = gameplay::hud_live::raceHudTimerValue(static_cast<std::int32_t>(snapshot.elapsedFrames));
    s.lapTime = lapFrames;
    s.lap = snapshot.lap + 1;  // FrameSnapshot::lap counts completed laps
    s.rank = place0;
    s.totalCars = totalCars;
    s.gear = gear;
    return s;
}

// Texture pass = one decoded page (+ CLUT bank) the caller binds.
enum class RaceHudPass : std::uint8_t {
    ScoreCsa0,   // hudArt.raceAtlas  (SYS/SCORE.GSL unit @0x70, CSA 0): rank digits, suffix, oval
    ScoreCsa1,   // hudArt.raceBanner (same texels, CSA 1): banners, capsules, timer digits, rank total
    ScorePage1,  // SYS/SCORE.GSL unit @0x8350: lap disc (csa 8) + lap digits (csa 7). Not decoded by HudArt (GAP)
    Meter0,      // hudArt.speedDial (ITEM/METER0.GSL unit @0x70): dial face + gear glyph
};

// Which pass draws a sprite, or false when no native page exists for it
// (Score0 banks other than 0/1, Meter0Sub needle/hub/arc page).
inline bool raceHudPassFor(const roadtrip::gameplay::RaceHudSprite& s, RaceHudPass& out) {
    using roadtrip::gameplay::RaceHudPage;
    switch (s.page) {
        case RaceHudPage::Score0:
            if (s.csa == 0) { out = RaceHudPass::ScoreCsa0; return true; }
            if (s.csa == 1) { out = RaceHudPass::ScoreCsa1; return true; }
            return false;
        case RaceHudPage::Score1: out = RaceHudPass::ScorePage1; return true;
        case RaceHudPage::Meter0: out = RaceHudPass::Meter0; return true;
        case RaceHudPage::Meter0Sub: return false;
    }
    return false;
}

// Full ordered sprite list in ROM draw order: raceHudDraw 0x233860 (TOTAL banner,
// TOTAL ElementC, then per player the LAPTIME banner, LAP ElementC and ElementD),
// then the gauge task (dial face, gear glyph).
inline std::vector<roadtrip::gameplay::RaceHudSprite> raceHudLayoutSprites(const RaceHudLayoutState& s) {
    namespace L = roadtrip::gameplay;
    using L::RaceHudPage;
    using L::RaceHudSprite;
    std::vector<RaceHudSprite> out;
    const auto i16 = [](int v) { return static_cast<std::int16_t>(v); };

    // ElementC 0x22F750: panel, separators, six BCD digits (low nibble first), lap badge.
    const auto elementC = [&](L::RaceHudPoint o, std::int32_t value, int lap, int csa) {
        out.push_back(L::raceHudAt(L::kRaceHudPanelCapLeft, o));
        out.push_back(L::raceHudAt(L::kRaceHudPanelCapRight, o));
        for (int k = 0; k < L::kRaceHudPanelMiddleTiles; ++k) {
            RaceHudSprite m = L::raceHudAt(L::kRaceHudPanelMiddle, o);
            m.x = i16(m.x + k * L::kRaceHudPanelMiddleStep);
            out.push_back(m);
        }
        RaceHudSprite sep0 = L::raceHudAt(L::kRaceHudTimerSepMinutes, o);
        RaceHudSprite sep1 = L::raceHudAt(L::kRaceHudTimerSepSeconds, o);
        sep0.csa = sep1.csa = static_cast<std::uint8_t>(csa);
        out.push_back(sep0);
        out.push_back(sep1);
        std::uint32_t bcd = gameplay::hud_live::raceTimerBcd(value);
        for (int i = 0; i < 6; ++i) {
            out.push_back(L::raceHudTimerDigitSprite(o, 5 - i, static_cast<int>(bcd & 0xFu), csa));
            bcd >>= 4;
        }
        if (lap == -1) return;
        out.push_back(L::raceHudAt(L::kRaceHudLapDisc, o));
        const auto lapDigit = [&](int d, L::RaceHudPoint at) {
            out.push_back({RaceHudPage::Score1, static_cast<std::uint8_t>(L::kRaceHudLapDigitCsa),
                           i16(d * L::kRaceHudLapDigitW), i16(L::kRaceHudLapDigitV), i16(L::kRaceHudLapDigitW),
                           i16(L::kRaceHudLapDigitH), i16(o.x + at.x), i16(o.y + at.y), 0});
        };
        if (lap == 0) out.push_back(L::raceHudAt(L::kRaceHudLapFlag, o));
        else if (lap < 10) lapDigit(lap, L::kRaceHudLapDigitOne);
        else {
            lapDigit((lap / 10) % 10, L::kRaceHudLapDigitTens);
            lapDigit(lap % 10, L::kRaceHudLapDigitOnes);
        }
    };

    // TOTALTIME (0x233F10 group + ElementC bank 1, lap -1).
    for (const RaceHudSprite& b : L::kRaceHudTotalBanner) out.push_back(b);
    elementC(L::kRaceHudTotalPanelOrigin, s.totalTime, -1, L::kRaceHudTimerBankCsa[1]);

    // LAPTIME (per-player group +0x84A0 + ElementC slot 0). The digit bank the
    // updater selects is untraced; the goldens show the CSA 1 yellow.
    if (s.showLapBlock) {
        for (const RaceHudSprite& b : L::kRaceHudLapBanner) out.push_back(b);
        elementC(L::kRaceHudLapPanelOrigin, s.lapTime, s.lap, L::kRaceHudTimerBankCsa[1]);
    }

    // Rank, ElementD 0x22FE50 at (40,88).
    if (s.showRank && s.rank >= 0) {
        const L::RaceHudPoint o = L::kRaceHudRankOrigin;
        const int k = L::raceHudOrdinalIndex(s.rank);
        const int place = s.rank + 1;
        const auto suffix = [&](int dx) {
            out.push_back({RaceHudPage::Score0, 0, i16(L::kRaceHudRankSuffixU0 + k * L::kRaceHudRankSuffixW),
                           i16(L::kRaceHudRankSuffixV), i16(L::kRaceHudRankSuffixW), i16(L::kRaceHudRankSuffixH),
                           i16(o.x + dx), i16(o.y), 0});
        };
        if (s.totalCars != 0) {
            out.push_back(L::raceHudAt(L::kRaceHudRankPanel, o));
            suffix(L::kRaceHudRankSuffixDx);
            const auto big = [&](int d, int dx) {
                out.push_back({RaceHudPage::Score0, 0, i16((d % 9) * L::kRaceHudRankDigitW),
                               i16((d / 9) * L::kRaceHudRankDigitH), i16(L::kRaceHudRankDigitW),
                               i16(L::kRaceHudRankDigitH), i16(o.x + dx), i16(o.y), 0});
            };
            if (place < 10) big(place, L::kRaceHudRankDigitOnesDx);
            else {
                big(place % 10, L::kRaceHudRankDigitOnesDx);
                big((place / 10) % 10, L::kRaceHudRankDigitTensDx);
            }
            const auto total = [&](int d, L::RaceHudPoint at) {
                out.push_back({RaceHudPage::Score0, 1, i16(L::kRaceHudRankTotalU0 + d * L::kRaceHudRankTotalW),
                               i16(L::kRaceHudRankTotalV), i16(L::kRaceHudRankTotalW), i16(L::kRaceHudRankTotalH),
                               i16(o.x + at.x), i16(o.y + at.y), 0});
            };
            if (s.totalCars < 10) total(s.totalCars, L::kRaceHudRankTotalOnes);
            else {
                total(s.totalCars % 10, L::kRaceHudRankTotalOnes);
                total((s.totalCars / 10) % 10, L::kRaceHudRankTotalTens);
            }
        } else {
            // Small variant (0x2302D8..0x2304DC): suffix at +30, 16x16 digits v=64
            // at +14 (ones) / +0 (tens), csa 0. Not on the 1P goldens.
            suffix(0x1e);
            const auto small = [&](int d, int dx) {
                out.push_back({RaceHudPage::Score0, 0, i16(d * 16), i16(64), i16(16), i16(16), i16(o.x + dx), i16(o.y), 0});
            };
            if (place < 10) small(place, 0xe);
            else {
                small(place % 10, 0xe);
                small((place / 10) % 10, 0);
            }
        }
    }

    // Gauge (UpdateRaceHud handler 0): dial face, gear glyph. The mph digits are
    // drawn by main.cpp (coordinator law). Needle, hub cap and boost arc live on
    // Meter0Sub and are not emitted (needle: GAP, law unresolved).
    out.push_back(L::kRaceHudDialFace);
    out.push_back(L::raceHudGearSprite(s.gear));
    return out;
}

// Design px -> target px. main.cpp's race passes use a per-axis stretch
// (sx = W/640, sy = H/448, no offset); raceHudStretchViewport reproduces that.
struct RaceHudViewport {
    float sx = 1.f, sy = 1.f, ox = 0.f, oy = 0.f;
};
inline RaceHudViewport raceHudStretchViewport(int width, int height) {
    return {static_cast<float>(width) / static_cast<float>(roadtrip::gameplay::kRaceHudDesignWidth),
            static_cast<float>(height) / static_cast<float>(roadtrip::gameplay::kRaceHudDesignHeight), 0.f, 0.f};
}

// One sprite as a textured quad: UVs normalised by the page size, uniform white
// (the pages are TFX DECAL), alpha = kRaceHudCapsuleAlpha on the half-alpha
// capsule/disc texels (the decoder returns them opaque).
inline HudQuad raceHudSpriteQuad(const roadtrip::gameplay::RaceHudSprite& s, const RaceHudViewport& vp) {
    const auto ps = roadtrip::gameplay::raceHudPageSize(s.page);
    const float pw = static_cast<float>(ps.width), ph = static_cast<float>(ps.height);
    const float a = s.translucent ? roadtrip::gameplay::kRaceHudCapsuleAlpha : 1.f;
    return {vp.ox + static_cast<float>(s.x) * vp.sx, vp.oy + static_cast<float>(s.y) * vp.sy,
            static_cast<float>(s.w) * vp.sx, static_cast<float>(s.h) * vp.sy,
            static_cast<float>(s.u) / pw, static_cast<float>(s.v) / ph,
            static_cast<float>(s.u + s.w) / pw, static_cast<float>(s.v + s.h) / ph,
            {1.f, 1.f, 1.f, a}};
}

// Quads of one pass, in ROM draw order.
inline std::vector<HudQuad> raceHudLayoutQuads(const RaceHudLayoutState& state, RaceHudPass pass,
                                               const RaceHudViewport& vp) {
    std::vector<HudQuad> quads;
    for (const auto& s : raceHudLayoutSprites(state)) {
        RaceHudPass p{};
        if (raceHudPassFor(s, p) && p == pass) quads.push_back(raceHudSpriteQuad(s, vp));
    }
    return quads;
}

// Stand-in for the ScorePage1 pass until SCORE.GSL unit @0x8350 is decoded:
// the lap disc as flat scanlines in the measured capsule material
// ((32,24,64) at alpha 0.509, see race_hud_layout.h). Shape AUTHORED (circle
// in the 24x24 cell); golden disc box 468..492 x 51..76.
inline std::vector<ScreenQuad> raceHudLapDiscFallback(const RaceHudLayoutState& state, const RaceHudViewport& vp) {
    namespace L = roadtrip::gameplay;
    std::vector<ScreenQuad> quads;
    if (!state.showLapBlock || state.lap == -1) return quads;
    const float x0 = static_cast<float>(L::kRaceHudLapPanelOrigin.x + L::kRaceHudLapDisc.x);
    const float y0 = static_cast<float>(L::kRaceHudLapPanelOrigin.y + L::kRaceHudLapDisc.y);
    const float r = static_cast<float>(L::kRaceHudLapDisc.w) * 0.5f;
    const std::array<float, 4> c{L::kRaceHudCapsuleTexel.r / 255.f, L::kRaceHudCapsuleTexel.g / 255.f,
                                 L::kRaceHudCapsuleTexel.b / 255.f, L::kRaceHudCapsuleAlpha};
    for (int row = 0; row < L::kRaceHudLapDisc.h; ++row) {
        const float dy = (static_cast<float>(row) + 0.5f) - r;
        const float half = std::sqrt(std::max(0.f, r * r - dy * dy));
        quads.push_back(ScreenQuad{vp.ox + (x0 + r - half) * vp.sx, vp.oy + (y0 + static_cast<float>(row)) * vp.sy,
                                   2.f * half * vp.sx, vp.sy, c});
    }
    return quads;
}

// Stand-in lap number on the ScoreCsa1 pass: the yellow 12x16 digit row
// (u = 32 + 12d, v = 48, the rank-total cells) centred on the 8-wide lap slot.
// AUTHORED substitute for the 8x16 Score1 csa 7 digits; drop it once
// ScorePage1 is decoded.
inline std::vector<HudQuad> raceHudLapDigitFallbackQuads(const RaceHudLayoutState& state, const RaceHudViewport& vp) {
    namespace L = roadtrip::gameplay;
    std::vector<HudQuad> quads;
    if (!state.showLapBlock || state.lap <= 0) return quads;
    const auto put = [&](int d, L::RaceHudPoint at) {
        const L::RaceHudSprite s{L::RaceHudPage::Score0, 1,
                                 static_cast<std::int16_t>(L::kRaceHudRankTotalU0 + d * L::kRaceHudRankTotalW),
                                 static_cast<std::int16_t>(L::kRaceHudRankTotalV),
                                 static_cast<std::int16_t>(L::kRaceHudRankTotalW),
                                 static_cast<std::int16_t>(L::kRaceHudRankTotalH),
                                 static_cast<std::int16_t>(L::kRaceHudLapPanelOrigin.x + at.x - 2),
                                 static_cast<std::int16_t>(L::kRaceHudLapPanelOrigin.y + at.y), 0};
        quads.push_back(raceHudSpriteQuad(s, vp));
    };
    if (state.lap < 10) put(state.lap, L::kRaceHudLapDigitOne);
    else {
        put((state.lap / 10) % 10, L::kRaceHudLapDigitTens);
        put(state.lap % 10, L::kRaceHudLapDigitOnes);
    }
    return quads;
}

}  // namespace rt::native::preview
