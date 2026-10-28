// Second screen (Map / Notebook / Radio) page builder. Twin of the recomp's
// src/ui/SecondScreen.cpp: same labels, geometry and empty states. See the header.
#include "preview/second_screen.h"

#include "assets/glyph_atlas.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace rt::native::preview {
namespace {

using Color = std::array<float, 4>;
using gameplay::SecondScreenPage;

constexpr Color col(std::uint32_t rgba) { return theme::rgba(rgba); }
// SecondScreen.cpp:42-56 (RRGGBBAA).
constexpr Color kSkyTop = col(0x5E9BE0FF), kSkyBottom = col(0xB4D2F2FF), kShadow = col(0x0A2A6059);
constexpr Color kPanelBody = col(0x304090FF), kPanelRimOuter = col(0x91D2E8FF), kPanelRimInner = col(0x1CA1E6FF);
constexpr Color kLabel = col(0xF6E68FFF), kValue = col(0xFFFFFFFF), kOutline = col(0x000820FF), kName = col(0x58E048FF);
constexpr Color kTrack = col(0x1C2E70FF), kPaper = col(0xDCE6B8FF), kPaperRim = col(0xA8D070FF), kFrame = col(0xEEF2D0FF);
constexpr Color kInk = col(0x1A3C8CFF), kInkFaint = col(0x4C5A34FF), kMenuFill = col(0x2E8A3AFF), kMenuRim = col(0x9EE07AFF);
constexpr Color kTabIdle = col(0x263478FF), kTabIdleRim = col(0x1CA1E6FF), kTabDim = col(0x8A96C8FF);
constexpr Color kGold = col(0xF8C838FF), kRed = col(0xFF7060FF), kPlateOn = col(0x3A6EE8FF);
constexpr Color kTabTop = col(0xF8B030FF), kTabBottom = col(0xE8840CFF), kTabShine = col(0xFFD878FF);

constexpr const char* kStationNames[3] = {"PEACH FM", "E-RADIO", "Off"};

struct Canvas {
    UiScreen& screen;
    float S;

    void rect(float x0, float y0, float x1, float y1, const Color& c) {
        if (x1 <= x0 || y1 <= y0) return;
        screen.quads.push_back(ScreenQuad{x0, y0, x1 - x0, y1 - y0, c});
    }
    void rectPx(float x, float y, float w, float h, const Color& c) { rect(x, y, x + w, y + h, c); }
    float u(float v) const { return v * S; }

    float scaleY(float size) const { return size / static_cast<float>(assets::kGlyphCellHeight); }
    float scaleX(float size) const { return scaleY(size) * 0.77f; }
    float width(const std::string& t, float size) const {
        return static_cast<float>(t.size()) * static_cast<float>(assets::kGlyphCellWidth) * scaleX(size);
    }
    void glyphs(const std::string& t, float x, float y, float size, const Color& c) {
        const float cw = static_cast<float>(assets::kGlyphCellWidth) * scaleX(size), ch = size;
        for (char p : t) {
            const int slot = assets::fonteSlotForChar(p);
            if (slot >= 0) {
                const auto uv = assets::fonteSlotUv(slot);
                screen.text.push_back(HudQuad{x, y, cw, ch, uv.u0, uv.v0, uv.u1, uv.v1, c});
            }
            x += cw;
        }
    }
    // Outlined text (the recomp's `outlined`): a dark copy offset behind.
    void text(const std::string& t, float x, float y, float size, const Color& c, float outline = 0.f) {
        const float o = outline > 0.f ? outline : std::max(1.f, size / 16.f);
        glyphs(t, x + o, y + o, size, kOutline);
        glyphs(t, x, y, size, c);
    }
    void textFit(const std::string& t, float x, float y, float size, const Color& c, float maxW) {
        const float w = width(t, size);
        text(t, x, y, w > maxW && w > 0.f ? size * maxW / w : size, c);
    }
    void centred(const std::string& t, float x0, float x1, float y, float size, const Color& c) {
        text(t, (x0 + x1 - width(t, size)) * 0.5f, y, size, c);
    }
};

std::string withCommas(std::uint64_t v) {
    std::string s = std::to_string(v);
    for (int i = static_cast<int>(s.size()) - 3; i > 0; i -= 3) s.insert(static_cast<std::size_t>(i), ",");
    return s;
}

std::string ordinal(int n) {
    const char* suffix = (n % 100 >= 11 && n % 100 <= 13) ? "th" : n % 10 == 1 ? "st" : n % 10 == 2 ? "nd" : n % 10 == 3 ? "rd" : "th";
    return std::to_string(n) + suffix;
}

std::string ellipsize(const Canvas& cv, const std::string& text, float size, float maxW) {
    if (cv.width(text, size) <= maxW) return text;
    std::string t = text;
    while (!t.empty() && cv.width(t + "...", size) > maxW) t.pop_back();
    return t + "...";
}

// Words split into at most two lines that fit.
std::vector<std::string> wrap2(const Canvas& cv, const std::string& text, float size, float maxW) {
    if (cv.width(text, size) <= maxW) return {text};
    std::size_t best = std::string::npos;
    for (std::size_t i = text.find(' '); i != std::string::npos; i = text.find(' ', i + 1))
        if (cv.width(text.substr(0, i), size) <= maxW) best = i;
    if (best == std::string::npos) return {text};
    return {text.substr(0, best), text.substr(best + 1)};
}

const char* licenceName(int licence) {
    static const char* names[] = {"None", "B", "A", "Super A"};
    return licence >= 0 && licence < 4 ? names[licence] : "?";
}

const char* townName(int index) {
    static const char* names[] = {"My Garage", "Peach Town", "Fuji City", "Sandpolis", "Chestnut Canyon",
                                  "Mushroom Road", "White Mountain", "Papaya Island", "Cloud Hill", "My City"};
    return index >= 0 && index < 10 ? names[index] : "?";
}

int popcount(std::uint64_t v) { int n = 0; for (; v; v &= v - 1) ++n; return n; }

// ---- the column beside the map (SecondScreen.cpp:138-245) ----

struct Column {
    Canvas& cv;
    float statSize;  // value size in S units (the town column is more compact)

    float stat(float x, float y, float w, const std::string& label, const std::string& value, const Color& colour = kValue,
               const std::string& note = {}, const Color& noteColour = kValue) {
        const float body = cv.u(32), big = cv.u(statSize);
        cv.text(label, x, y, body, kLabel);
        const float vw = cv.width(value, big);
        cv.text(value, x, y + body * 1.1f, vw > w ? big * w / vw : big, colour, cv.u(4));
        float next = y + body * 1.1f + big + cv.u(18);
        if (!note.empty()) {
            cv.text(note, x, next - cv.u(10), body, noteColour);
            next += body + cv.u(8);
        }
        return next;
    }
    float heading(float x, float y, float w, const std::string& text) {
        for (const auto& l : wrap2(cv, text, cv.u(44), w)) {
            cv.text(l, x, y, cv.u(44), kName);
            y += cv.u(44) * 1.1f;
        }
        return y + cv.u(14);
    }
};

void raceColumn(Canvas& cv, float x, float y, float w, const SecondScreenData& d) {
    Column c{cv, 60};
    if (!d.course.empty()) y = c.heading(x, y, w, d.course);
    if (d.countdown >= 2 && d.countdown <= 6) {
        c.stat(x, y, w, "Get ready", d.countdown == 6 ? "GO!" : "...", kGold);
        return;
    }
    if (d.laps > 0)
        y = c.stat(x, y, w, d.finished ? "Finished" : "Lap",
                     d.finished && d.place > 0 ? ordinal(d.place) : std::to_string(d.lap) + " / " + std::to_string(d.laps));
    if (d.lastLapFrames) {
        bool newBest = false;
        const std::string note = secondScreenLapNote(d.lastLapFrames, d.bestBeforeLast, &newBest);
        y = c.stat(x, y, w, "Last lap", secondScreenRaceTime(d.lastLapFrames), kValue, note, newBest ? kName : kRed);
    }
    c.stat(x, y, w, d.newBestLap ? "New best lap!" : "Best lap",
           d.bestLapFrames ? secondScreenRaceTime(d.bestLapFrames) : "--'--\"--", kGold);
}

void townColumn(Canvas& cv, float x, float y, float w, float bottom, const SecondScreenData& d) {
    Column c{cv, 46};
    const float each = cv.u(32) * 1.1f + cv.u(c.statSize) + cv.u(18);
    y = c.heading(x, y, w, d.townLabel);
    char clock[16];
    std::snprintf(clock, sizeof(clock), "%d:%02d %s", d.hour % 12 == 0 ? 12 : d.hour % 12, d.minute, d.hour < 12 ? "am" : "pm");
    // As many as fit, most useful first.
    if (y + each <= bottom) y = c.stat(x, y, w, "Time", clock);
    if (d.townPhotos && y + each <= bottom)
        y = c.stat(x, y, w, "Photos here", std::to_string(d.townPhotosTaken) + " / " + std::to_string(d.townPhotos),
                   d.townPhotosTaken == d.townPhotos ? kGold : kValue);
    if (y + each <= bottom) y = c.stat(x, y, w, "Houses to visit", std::to_string(d.housesLeft), d.housesLeft ? kValue : kGold);
    if (y + each <= bottom) c.stat(x, y, w, "Coins found", std::to_string(d.coins) + " / 100", kGold);
}

// ---- the map (reconstructed draw list, unit square -> rect) ----

void drawMap(Canvas& cv, const gameplay::MapProjectedFrame& m, float x0, float y0, float x1, float y1) {
    cv.rect(x0, y0, x1, y1, kTrack);
    const float side = std::min(x1 - x0, y1 - y0), cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f;
    const auto at = [&](const gameplay::NdcPoint& p) { return std::pair<float, float>{cx + static_cast<float>(p.u) * side * 0.5f, cy - static_cast<float>(p.v) * side * 0.5f}; };
    const float dot = std::max(2.f, cv.u(3));
    for (const auto& run : m.roads)
        for (std::size_t i = 1; i < run.points.size(); ++i) {
            const auto a = at(run.points[i - 1]), b = at(run.points[i]);
            const int steps = std::max(1, static_cast<int>(std::hypot(b.first - a.first, b.second - a.second) / dot));
            for (int s = 0; s <= steps; ++s)
                cv.rectPx(a.first + (b.first - a.first) * s / steps - dot * 0.5f, a.second + (b.second - a.second) * s / steps - dot * 0.5f, dot, dot, kLabel);
        }
    for (const auto& mk : m.markers) {
        const auto p = at(mk.at);
        cv.rectPx(p.first - dot * 1.5f, p.second - dot * 1.5f, dot * 3, dot * 3, kGold);
    }
    if (m.playerPresent) {
        const auto p = at(m.player);
        cv.rectPx(p.first - dot * 2, p.second - dot * 2, dot * 4, dot * 4, kValue);
    }
}

// A stamp earned while the map is up: a card along the bottom of the map page for a moment.
void stampToast(Canvas& cv, float x0, float y0, float x1, float y1, int n, float since) {
    const float t = std::clamp(since / 0.25f, 0.f, 1.f), h = y1 - y0, drop = (1.f - t) * (h + cv.u(24));
    y0 += drop, y1 += drop;
    cv.rect(x0, y0 + cv.u(8), x1, y1 + cv.u(8), kShadow);
    cv.rect(x0, y0, x1, y1, kPaperRim);
    cv.rect(x0 + cv.u(6), y0 + cv.u(6), x1 - cv.u(6), y1 - cv.u(6), kPaper);
    const float icon = h - cv.u(24), fx = x0 + cv.u(14), fy = y0 + cv.u(12);
    cv.rect(fx, fy, fx + icon, fy + icon, kFrame);
    const float tx = fx + icon + cv.u(20), body = cv.u(32);
    cv.text("New stamp!", tx, y0 + cv.u(14), body, kGold);
    cv.glyphs(ellipsize(cv, "Stamp " + std::to_string(n), body, x1 - cv.u(20) - tx), tx, y0 + cv.u(14) + body * 1.2f, body, kInk);
}

// ---- notebook (SecondScreen.cpp:323-480) ----

void drawNotebook(Canvas& cv, float x0, float y0, float x1, float y1, const SecondScreenData& d) {
    const float body = cv.u(32), heading = cv.u(44);
    if (!d.adventure) {
        const std::string msg = "Start or load an Adventure to fill your notebook.";
        cv.text(msg, (x0 + x1 - cv.width(msg, body)) * 0.5f, (y0 + y1 - body) * 0.5f, body, kLabel);
        return;
    }
    float y = y0;
    cv.text(d.playerName, x0, y, heading, kName);
    char miles[32];
    std::snprintf(miles, sizeof(miles), "%.0f miles driven", d.mileageM / 1609.344);
    cv.text(miles, x1 - cv.width(miles, body), y + (heading - body) * 0.5f, body, kLabel);
    y += heading * 1.2f;
    const float chipH = cv.u(64), chipGap = cv.u(12), chipW = (x1 - x0 - chipGap * 4) / 5;
    const std::string currency = d.currency.empty() ? std::string("Money") : d.currency;
    const std::string values[5][2] = {{"Licence", licenceName(d.licence)},
                                      {ellipsize(cv, currency, cv.u(24), chipW - cv.u(20)), withCommas(d.money)},
                                      {"Coins", std::to_string(d.coins) + " / 100"},
                                      {"Towns", std::to_string(d.townsVisited) + " / 10"},
                                      {"Photos", std::to_string(d.photos) + " / 100"}};
    for (int i = 0; i < 5; ++i) {
        const float x = x0 + static_cast<float>(i) * (chipW + chipGap);
        cv.rect(x, y, x + chipW, y + chipH, kTrack);
        const float ls = cv.u(24), vs = body, w = chipW - cv.u(20);
        const float lw = cv.width(values[i][0], ls), vw = cv.width(values[i][1], vs);
        const float l = lw > w ? ls * w / lw : ls, v = vw > w ? vs * w / vw : vs;
        const float top = y + (chipH - l - v * 1.05f) * 0.5f;
        cv.text(values[i][0], x + cv.u(10), top, l, kLabel, cv.u(2));
        cv.text(values[i][1], x + cv.u(10), top + l * 1.05f, v, i >= 2 ? kGold : kValue);
    }
    y += chipH + cv.u(16);

    // Got / To get segmented control.
    const int got = static_cast<int>(d.stamps.count());
    const float segH = cv.u(72), mid = (x0 + x1) * 0.5f;
    cv.rect(x0, y, x1, y + segH, kTrack);
    const float sx0 = d.stampsGot ? x0 : mid, sx1 = d.stampsGot ? mid : x1;
    cv.rect(sx0, y, sx1, y + segH, col(theme::kCapGold));
    cv.rect(sx0 + cv.u(10), y + cv.u(4), sx1 - cv.u(10), y + segH - cv.u(4), col(theme::kSelectionBottom));
    cv.rect(sx0 + cv.u(10), y + cv.u(4), sx1 - cv.u(10), y + segH * 0.5f, col(theme::kSelectionTop));
    const std::string a = "Got " + std::to_string(got), b = "To get " + std::to_string(100 - got);
    cv.text(a, (x0 + mid - cv.width(a, body)) * 0.5f, y + (segH - body) * 0.5f, body, d.stampsGot ? kValue : kLabel);
    cv.text(b, (mid + x1 - cv.width(b, body)) * 0.5f, y + (segH - body) * 0.5f, body, d.stampsGot ? kLabel : kValue);
    y += segH + cv.u(16);

    // Cards.
    std::vector<int> shown;
    for (int i = 0; i < 100; ++i)
        if (d.stamps[static_cast<std::size_t>(i)] == d.stampsGot) shown.push_back(i + 1);
    if (shown.empty()) {
        const std::string msg = d.stampsGot ? "No stamps yet: they come from people, places and races." : "Every stamp collected!";
        cv.text(msg, (x0 + x1 - cv.width(msg, body)) * 0.5f, y + cv.u(40), body, kLabel);
        return;
    }
    const float cardH = cv.u(112), gap = cv.u(10);
    for (std::size_t k = 0; k < shown.size(); ++k) {
        const float top = y + static_cast<float>(k) * (cardH + gap) - d.scroll;
        if (top < y || top + cardH > y1) continue;  // no clip rect: only whole cards
        const int n = shown[k];
        const float kx1 = x1;
        cv.rect(x0, top, kx1, top + cardH, kPaperRim);
        cv.rect(x0 + cv.u(6), top + cv.u(6), kx1 - cv.u(6), top + cardH - cv.u(6), kPaper);
        const float icon = cv.u(96), fx = x0 + cv.u(16), fy = top + (cardH - icon) * 0.5f;
        cv.rect(fx - cv.u(4), fy - cv.u(4), fx + icon + cv.u(4), fy + icon + cv.u(4), kPaperRim);
        cv.rect(fx, fy, fx + icon, fy + icon, kFrame);
        const std::string num = std::to_string(n);
        const float tagW = std::max(cv.u(44), cv.width(num, cv.u(24)) + cv.u(16));
        cv.rect(kx1 - cv.u(6) - tagW, top + cv.u(6), kx1 - cv.u(6), top + cv.u(40), kPaperRim);
        cv.glyphs(num, kx1 - cv.u(6) - tagW + (tagW - cv.width(num, cv.u(24))) * 0.5f, top + cv.u(6) + (cv.u(34) - cv.u(24)) * 0.5f, cv.u(24), kInk);
        const std::string caption = "Stamp " + num;
        const float tx = fx + icon + cv.u(24);
        const auto lines = wrap2(cv, caption, body, kx1 - cv.u(70) - tx);
        const float lh = body * 1.25f;
        float ty = top + (cardH - lh * static_cast<float>(lines.size())) * 0.5f;
        for (const auto& l : lines) {
            cv.glyphs(l, tx, ty, body, d.stampsGot ? kInk : kInkFaint);
            cv.rect(tx, ty + body + cv.u(4), kx1 - cv.u(20), ty + body + cv.u(6), kPaperRim);
            ty += lh;
        }
    }
}

// ---- radio (SecondScreen.cpp:482-624) ----

void drawRadioIcon(Canvas& cv, float cx, float top, float ih, const Color& ink, const Color& dial) {
    const float rx0 = cx - ih * 0.55f, ry0 = top + ih * 0.35f, rx1 = cx + ih * 0.55f, ry1 = top + ih;
    cv.rect(rx0 + ih * 0.2f, top, rx0 + ih * 0.2f + cv.u(3), ry0, ink);  // aerial
    cv.rect(rx0, ry0, rx1, ry1, ink);
    const float r = ih * 0.16f, dx = rx1 - ih * 0.28f, dy = (ry0 + ry1) * 0.5f;
    cv.rect(dx - r, dy - r, dx + r, dy + r, dial);
}

void nowPlayingStrip(Canvas& cv, float x0, float y0, float x1, float y1, const SecondScreenData& d) {
    const float body = cv.u(32), h = y1 - y0, r = h * 0.5f;
    const std::string station = kStationNames[d.station == gameplay::RadioStation::PeachFm ? 0 : 1];
    const std::string songAscii = d.title.empty() ? d.artist : d.title + "   -   " + d.artist;  // FONTE has no em dash
    cv.rect(x0, y0, x1, y1, kTrack);
    drawRadioIcon(cv, x0 + r * 0.6f + h * 0.35f, y0 + h * 0.12f, h * 0.7f, kGold, kTrack);
    float tx = x0 + r * 0.6f + h * 0.7f + cv.u(18);
    const float ty = (y0 + y1) * 0.5f - body * 0.5f;
    cv.text(station, tx, ty, body, kGold);
    tx += cv.width(station, body) + cv.u(18);
    if (!songAscii.empty()) cv.text(ellipsize(cv, songAscii, body, x1 - r * 0.6f - tx), tx, ty, body, kValue);
}

void drawRadio(Canvas& cv, float x0, float y0, float x1, float y1, const SecondScreenData& d) {
    const float body = cv.u(32), heading = cv.u(44);
    if (!d.radioAvailable) {
        const std::string msg = "The radio plays in town, in an Adventure game.";
        cv.text(msg, (x0 + x1 - cv.width(msg, body)) * 0.5f, (y0 + y1 - body) * 0.5f, body, kLabel);
        return;
    }
    const int current = d.station == gameplay::RadioStation::PeachFm ? 0 : d.station == gameplay::RadioStation::ERadio ? 1 : 2;
    const float gap = cv.u(24), h = std::min(cv.u(260), (y1 - y0) * 0.45f), w = (x1 - x0 - 2 * gap) / 3;
    for (int i = 0; i < 3; ++i) {
        const float bx = x0 + static_cast<float>(i) * (w + gap);
        const bool on = i == current;
        const float rim = cv.u(on ? 5 : 3);
        cv.rect(bx, y0, bx + w, y0 + h, on ? kGold : kTabIdleRim);
        cv.rect(bx + rim, y0 + rim, bx + w - rim, y0 + h - rim, on ? kPlateOn : kTrack);
        const std::string name = kStationNames[i];
        const float ih = cv.u(56), top = (y0 + y0 + h - ih - cv.u(16) - heading) * 0.5f;
        const Color ink = i == 2 ? col(0xF8C83866) : kGold;
        drawRadioIcon(cv, bx + w * 0.5f, top, ih, ink, on ? kPlateOn : kTrack);
        cv.text(name, bx + (w - cv.width(name, heading)) * 0.5f, top + ih + cv.u(16), heading, on ? kValue : kLabel);
    }
    float y = y0 + h + cv.u(48);
    if (d.nowPlaying) {
        nowPlayingStrip(cv, x0, y, x1, y + cv.u(64), d);
        y += cv.u(64) + cv.u(24);
        if (!d.title.empty()) {
            const std::string fit = ellipsize(cv, d.title, heading, x1 - x0 - cv.u(48));
            cv.text(fit, (x0 + x1 - cv.width(fit, heading)) * 0.5f, y, heading, kValue);
            y += heading + cv.u(20);
        }
        if (d.lengthSec > 0) {
            char t[32];
            std::snprintf(t, sizeof(t), "%d:%02d / %d:%02d", d.elapsedSec / 60, d.elapsedSec % 60, d.lengthSec / 60, d.lengthSec % 60);
            cv.text(t, x0 + cv.u(24), y, body, kLabel);
        }
    } else {
        const char* msg = d.station == gameplay::RadioStation::Off ? "The radio is off."
                          : d.inTown                               ? "It plays while you drive."
                                                                   : "You'll hear it when you're back in a town.";
        cv.text(msg, (x0 + x1 - cv.width(msg, body)) * 0.5f, y, body, kLabel);
    }
}

// ---- tabs and chrome (SecondScreen.cpp:626-654, 731-765) ----

void activeTab(Canvas& cv, float x0, float y0, float x1, float y1, const char* label) {
    const float body = cv.u(32);
    cv.rect(x0, y0, x1, y1, kTabBottom);
    cv.rect(x0, y0, x1, (y0 + y1) * 0.5f, kTabTop);
    cv.rect(x0 + cv.u(14), y0, x1 - cv.u(14), y0 + cv.u(4), kTabShine);
    const float cy = (y0 + y1 - cv.u(8)) * 0.5f, lw = cv.width(label, body);
    cv.text(label, (x0 + x1 - lw) * 0.5f, cy - body * 0.5f, body, kValue);
    // The rivets only where they clear the label.
    if (lw + 2 * cv.u(22 + 7 + 10) <= x1 - x0)
        for (float x : {x0 + cv.u(22), x1 - cv.u(22)}) {
            const float r = cv.u(7);
            cv.rect(x - r, cy - r, x + r, cy + r, kInk);
            cv.rect(x - r + cv.u(2), cy - r + cv.u(2), x + r - cv.u(2), cy + r - cv.u(2), col(0x48B8F0FF));
        }
}

void idleTab(Canvas& cv, float x0, float y0, float x1, float y1, const char* label, bool dim) {
    const float body = cv.u(32);
    cv.rect(x0, y0, x1, y1, kTabIdleRim);
    cv.rect(x0 + cv.u(4), y0 + cv.u(4), x1 - cv.u(4), y1, kTabIdle);
    cv.text(label, (x0 + x1 - cv.width(label, body)) * 0.5f, (y0 + y1 - cv.u(8) - body) * 0.5f, body, dim ? kTabDim : kLabel);
}

}  // namespace

std::string secondScreenRaceTime(std::uint32_t frames) {
    if (frames >= 0x7FFFFFFFu) return "--'--\"--";
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%u'%02u\"%02u", frames / 3600, frames / 60 % 60, (frames % 60) * 100 / 60);
    return buf;
}

std::string secondScreenLapNote(std::uint32_t lastLapFrames, std::uint32_t bestBeforeLast, bool* newBest) {
    if (newBest) *newBest = false;
    if (!lastLapFrames || !bestBeforeLast) return {};
    const std::int32_t d = static_cast<std::int32_t>(lastLapFrames) - static_cast<std::int32_t>(bestBeforeLast);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%s%d\"%02d", d < 0 ? "-" : "+", std::abs(d) / 60, std::abs(d) % 60 * 100 / 60);
    if (newBest) *newBest = d < 0;
    return d < 0 ? std::string("New best ") + buf : std::string(buf);
}

void fillSecondScreenFromSave(SecondScreenData& d, const gameplay::SaveData& save) {
    d.adventure = true;
    d.playerName = save.playerName;
    d.currency = save.currencyName;
    d.licence = save.licence;
    d.money = save.money;
    d.mileageM = save.mileageM;
    d.stamps.reset();
    for (int i = 0; i < 100; ++i)
        if (save.stampsEarned[static_cast<std::size_t>(i / 64)] >> (i % 64) & 1u) d.stamps.set(static_cast<std::size_t>(i));
    int missing = 0, photos = 0;
    for (auto b : save.coinsMissing) missing += popcount(b);
    for (int i = 0; i < 100; ++i)
        if (save.photosTaken[static_cast<std::size_t>(i / 8)] >> (i % 8) & 1u) ++photos;
    d.coins = std::clamp(100 - missing, 0, 100);
    d.photos = photos;
    d.townsVisited = 10 - popcount(save.townsUnvisited & gameplay::kTownsAllUnvisited);
    if (save.location < gameplay::kLocationCount) {
        d.housesLeft = popcount(save.unvisitedDoors[save.location]);
        d.townLabel = townName(save.location);
    }
    d.station = gameplay::stationFromSaveByte(save.radioStation);
    d.radioAvailable = true;
}

UiScreen buildSecondScreen(int w, int h, SecondScreenPage page, const SecondScreenData& d) {
    UiScreen screen;
    if (w <= 0 || h <= 0) return screen;
    Canvas cv{screen, uiDesignScale(h)};
    const float W = static_cast<float>(w), H = static_cast<float>(h);
    // Daylight sky (a vertical gradient in eight bands).
    for (int i = 0; i < 8; ++i) {
        const float t = static_cast<float>(i) / 7.f;
        Color c{};
        for (int k = 0; k < 4; ++k) c[static_cast<std::size_t>(k)] = kSkyTop[static_cast<std::size_t>(k)] + (kSkyBottom[static_cast<std::size_t>(k)] - kSkyTop[static_cast<std::size_t>(k)]) * t;
        cv.rect(0, H * static_cast<float>(i) / 8.f, W, H * static_cast<float>(i + 1) / 8.f + 1.f, c);
    }
    const float pad = cv.u(24), rowH = cv.u(84);
    const float pminX = pad, pminY = pad + rowH, pmaxX = W - pad, pmaxY = H - pad;
    const float tabGap = cv.u(16), tabTop = pad + cv.u(8), menuW = cv.u(168);
    const float mmaxX = pmaxX - cv.u(24), mmaxY = pminY - cv.u(8);
    const float tabW = std::min(cv.u(240), (mmaxX - menuW - cv.u(24) - (pminX + cv.u(24)) - 2 * tabGap) / 3);
    const std::pair<SecondScreenPage, const char*> tabs[3] = {{SecondScreenPage::Map, "Map"}, {SecondScreenPage::Notebook, "Notebook"}, {SecondScreenPage::Radio, "Radio"}};
    const auto tabX = [&](int i) { return pminX + cv.u(24) + static_cast<float>(i) * (tabW + tabGap); };
    for (int i = 0; i < 3; ++i)
        if (tabs[i].first != page)
            idleTab(cv, tabX(i), tabTop, tabX(i) + tabW, pminY + cv.u(2), tabs[i].second,
                    tabs[i].first == SecondScreenPage::Map && !d.mapUp && !d.racing);
    // The panel (shadow, two rims, body).
    cv.rect(pminX, pminY + cv.u(8), pmaxX, pmaxY + cv.u(8), kShadow);
    cv.rect(pminX, pminY, pmaxX, pmaxY, kPanelRimOuter);
    cv.rect(pminX + cv.u(3), pminY + cv.u(3), pmaxX - cv.u(3), pmaxY - cv.u(3), kPanelRimInner);
    cv.rect(pminX + cv.u(6), pminY + cv.u(6), pmaxX - cv.u(6), pmaxY - cv.u(6), kPanelBody);
    for (int i = 0; i < 3; ++i)
        if (tabs[i].first == page) activeTab(cv, tabX(i), pad, tabX(i) + tabW, pminY + cv.u(6), tabs[i].second);
    // Menu / Close pill.
    cv.rect(mmaxX - menuW, tabTop, mmaxX, mmaxY, kMenuRim);
    cv.rect(mmaxX - menuW + cv.u(4), tabTop + cv.u(4), mmaxX - cv.u(4), mmaxY - cv.u(4), kMenuFill);
    cv.centred(d.menuOpen ? "Close" : "Menu", mmaxX - menuW, mmaxX, (tabTop + mmaxY - cv.u(32)) * 0.5f, cv.u(32), kValue);

    const float inset = cv.u(30);
    const float cx0 = pminX + inset, cy0 = pminY + inset, cx1 = pmaxX - inset, cy1 = pmaxY - inset;
    if (page == SecondScreenPage::Notebook) { drawNotebook(cv, cx0, cy0, cx1, cy1, d); return screen; }
    if (page == SecondScreenPage::Radio) { drawRadio(cv, cx0, cy0, cx1, cy1, d); return screen; }

    // Map page: the map on the left at full height, a column on the right (race or town),
    // and in town the radio along the bottom.
    float my1 = cy1;
    if (d.nowPlaying && d.inTown) {
        const float stripH = cv.u(64);
        nowPlayingStrip(cv, cx0, cy1 - stripH, cx1, cy1, d);
        my1 -= stripH + cv.u(16);
    }
    const float colW = cv.u(250);
    const bool column = d.racing || d.inTown;
    const bool drawn = d.mapHere && d.map != nullptr;
    const float mapX1 = column ? cx1 - colW - cv.u(24) : cx1;
    if (drawn) {
        drawMap(cv, *d.map, cx0, cy0, mapX1, my1);
        if (column) {
            const float left = cx0 + std::min(mapX1 - cx0, my1 - cy0) + cv.u(32);  // square map
            const float cw = std::min(colW + cv.u(160), cx1 - left);
            if (d.racing) raceColumn(cv, left, cy0, cw, d);
            else townColumn(cv, left, cy0, cw, my1, d);
        }
    } else if (d.racing) {
        raceColumn(cv, cx1 - colW, cy0, colW, d);
    } else {
        const char* msg = d.mapUp && !d.mapHere ? "The map is on the top screen." : "The map shows here while you drive or race.";
        cv.text(msg, (cx0 + cx1 - cv.width(msg, cv.u(32))) * 0.5f, (cy0 + my1 - cv.u(32)) * 0.5f, cv.u(32), kLabel);
    }
    if (d.newStamp && drawn)
        stampToast(cv, cx0 + cv.u(40), cy1 - cv.u(120), cx1 - cv.u(40), cy1, d.newStamp, d.newStampSince);
    return screen;
}

bool secondScreenDebugEnabled() {
    const char* value = std::getenv("RT_SECOND_DEBUG");
    return value && std::strcmp(value, "1") == 0;
}

std::string secondScreenDebugLine(const SecondScreenData& d, SecondScreenPage page, int autoPage, bool overridden,
                                  int chosenPage, bool haveMap) {
    char line[400];
    std::snprintf(line, sizeof(line), "racing %d town %d drive %d demo %d here %d haveMap %d up %d page %d auto %d chosen %d/%d | %s",
                  d.racing, d.inTown, d.canDrive, 0, d.mapHere, haveMap, d.mapUp, static_cast<int>(page), autoPage, overridden,
                  chosenPage, d.stateWords.c_str());
    return line;
}

std::string secondScreenDebugPrint(const std::string& line) {
    char out[460];
    std::snprintf(out, sizeof(out), "[second] %s\n", line.c_str());
    std::string s = out;
    if (!s.empty() && s.back() == '\n') s.pop_back();
    return s;
}

void setSecondScreenTownPhotos(SecondScreenData& d, const gameplay::SaveData& save, const std::vector<int>& photoNumbers) {
    d.townPhotos = static_cast<int>(photoNumbers.size());
    d.townPhotosTaken = 0;
    for (int n : photoNumbers) d.townPhotosTaken += gameplay::photoTaken(save, n) ? 1 : 0;
}

void SecondScreenStampWatch::observe(const std::bitset<100>& stamps, double now) {
    const std::bitset<100> added = stamps & ~seen_;
    if (valid_ && added.count() == 1)
        for (int i = 0; i < 100; ++i)
            if (added[static_cast<std::size_t>(i)]) recent_ = i + 1, at_ = now;
    seen_ = stamps;
    valid_ = true;
}

void SecondScreenLapTracker::note(int lap, std::uint32_t elapsedFrames, std::uint32_t bestLap, double now) {
    if (bestLap && best_ && bestLap < best_) bestAt_ = now;
    if (lap > lap_) {
        last_ = elapsedFrames - lapStart_;
        lapStart_ = elapsedFrames;
        bestBefore_ = best_;
    }
    lap_ = lap;
    best_ = bestLap;
}

bool SecondScreenThrottle::due(double now, bool touch) {
    if (!touch && now - last_ < 1.0 / 30.0 - 0.002) return false;
    last_ = now;
    return true;
}

}  // namespace rt::native::preview
