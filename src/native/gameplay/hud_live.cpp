// W2-10 hud_live. Constants are quoted from build/decomp/fn/0024e1a0.c
// (UpdateCarControlState = default drive HUD), 0022f750.c (raceHudElementC),
// 0022fe50.c (raceHudElementD), 00249458/002494e8/00249520/0024f4a0.c and
// docs/native-reference/gauge-calibration.md.  Record layout in the decomp
// locals is {x|y<<32, u|v<<32, w|h<<32, ...}.

#include "gameplay/hud_live.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace rt::native::gameplay::hud_live {

namespace {

constexpr std::int16_t s16(int v) { return static_cast<std::int16_t>(v); }

DrawQuad quad(Table table, int entry, int csa, Page page, int u, int v, int w, int h, int x, int y,
              int tag) {
    DrawQuad q;
    q.table = table;
    q.texEntry = static_cast<std::uint8_t>(entry);
    q.csa = static_cast<std::uint8_t>(csa);
    q.page = page;
    q.u = s16(u); q.v = s16(v); q.w = s16(w); q.h = s16(h);
    q.x = s16(x); q.y = s16(y);
    q.tag = static_cast<std::uint8_t>(tag);
    return q;
}

}  // namespace

// ----------------------------------------------------------------- speed

int shippedMphFromRaw(float raw216) {
    // 002494e8.c:15-17: IntToPackedBCD((int)(raw / DAT_00332f48)).
    return static_cast<int>(raw216 / kShippedMphDivisor);
}

int hostMph(float hostSpeed, float divisor) {
    if (!(divisor > 0.f)) return 0;
    const float q = std::fabs(hostSpeed) / divisor;
    if (!(q > 0.f)) return 0;
    if (q >= static_cast<float>(kSpeedMax)) return kSpeedMax;
    return static_cast<int>(q);
}

std::uint32_t packedBcd(int value) {
    // 0024f4a0.c:16-21: nibble (n & 7) = value % 10, value /= 10 until 0.
    std::uint32_t out = 0;
    unsigned n = 0;
    for (; value != 0; value /= 10) {
        out |= static_cast<std::uint32_t>(value % 10) << ((n & 7u) << 2);
        ++n;
    }
    return out;
}

std::vector<SpeedCell> speedCells(int mph, int firstDx, int step) {
    // 0024e1a0.c:91-106: x = 0x4e (78), step -0x13 (19); loop draws a cell, shifts the BCD
    // right by 4 and breaks when it reaches 0; guard `iVar8 < 3`.  Handler 0 @0x249A90..
    // 0x249B48 is the same loop with x = 0x59 and step 0x11.
    std::vector<SpeedCell> cells;
    std::uint32_t bcd = packedBcd(mph);
    int x = firstDx;
    for (int i = 0; i < 3; ++i) {
        const int d = static_cast<int>(bcd & 0xf);
        bcd = static_cast<std::uint32_t>(static_cast<std::int32_t>(bcd) >> 4);
        cells.push_back({d, x});
        if (bcd == 0) break;
        x -= step;
    }
    return cells;
}

int tachStripCount(int rpm) {
    // 00249520.c:14-22 with param_2 = 0x10, param_3 = 1.
    int v = std::min(rpm, 10000);
    return (v * 0x10 + 9999) / 10000;
}

std::vector<DrawQuad> buildDriveHud2P(const HudLiveInput& in, int viewport) {
    viewport = std::clamp(viewport, 0, 2);
    const int ax = kDriveAnchors[viewport].x;
    const int ay = kDriveAnchors[viewport].y;
    std::vector<DrawQuad> q;
    const auto T = Table::Drive;

    // 0024e1a0.c:34-44: gear bar base, tex entry 2, csa 0; sprite (7,0x4c) uv(0,0) 100x18.
    q.push_back(quad(T, 2, 0, Page::Meter10, 0, 0, 100, 18, ax + 7, ay + 0x4c, 1));
    // :45-55: boost overlay, tex entry 1, csa = 15 - boost.
    const int boost = std::clamp(in.boostGauge, 0, 15);
    q.push_back(quad(T, 1, 0xf - boost, Page::Meter10, 0, 0, 100, 18, ax + 7, ay + 0x4c, 1));
    // :56-65: boost==0 sliver, entry 2, csa (frame>>3)%3+1; sprite (0x1e,0x52) uv(0,0x18) 58x8.
    if (boost == 0) {
        const int csa = (static_cast<int>(in.frame) >> 3) % 3 + 1;
        q.push_back(quad(T, 2, csa, Page::Meter10, 0, 0x18, 0x3a, 8, ax + 0x1e, ay + 0x52, 1));
    }
    // :66-70: dial face, entry 0, uv(0,0) 132x96 at origin.
    q.push_back(quad(T, 0, 0, Page::Meter10, 0, 0, 0x84, 0x60, ax, ay, 1));
    // :71-90: tach strips; k-th (0-based) strip x = 0x22+6k, u = 0x88+6k, v = 0, 4x60.
    const int strips = tachStripCount(in.rpm);
    if (strips != 0) {
        for (int k = 0; k < strips; ++k)
            q.push_back(quad(T, 0, 0, Page::Meter10, 0x88 + 6 * k, 0, 4, 0x3c, ax + 0x22 + 6 * k, ay,
                             strips));
    }
    // :91-106: speed digits, y = 0x31, u = d*0x14, v = 0x66, 20x26.
    const int mph = hostMph(in.speedRaw, in.speedDivisor);
    for (const SpeedCell& c : speedCells(mph))
        q.push_back(quad(T, 0, 0, Page::Meter10, c.digit * 0x14, 0x66, 0x14, 0x1a, ax + c.dx,
                         ay + 0x31, 1));
    // :107-127: transmission icon (only when the viewport flag is set).
    if (in.showTransmission) {
        if (in.transmissionMode == 1)
            q.push_back(quad(T, 0, 0, Page::Meter10, 0xf0, 0, 0x10, 0x10, ax + 9, ay + 0xc, 1));
        else if (in.transmissionMode != 0)
            q.push_back(quad(T, 0, 0, Page::Meter10, 0xf0, 0x18, 0x10, 0x10, ax + 9, ay + 0x24, 1));
    }
    // :128-137: gear cell, entry 2, csa 4; u = (g + (g != 0)) * 16, v = 0x20, 16x10 at (10, 0x3e).
    const int g = in.gear;
    q.push_back(quad(T, 2, 4, Page::Meter10, (g + (g != 0 ? 1 : 0)) * 0x10, 0x20, 0x10, 0xa, ax + 10,
                     ay + 0x3e, 1));
    return q;
}


// ---- handler 0 ---------------------------------------------------------------------------

float needleAngleRad(int rpm) {
    // 0x249DD0..0x249E0C: f20 = (float)CalculateTachometerValue(car,0,1) / 10000.0f + -0.7f;
    // 0x249EDC stores it as the z rotation, RotQuads scales by pi (0x24EC..).
    const int r = std::clamp(rpm, 0, 10000);
    return (static_cast<float>(r) / kNeedleRpmFull + kNeedleRpmBias) * kPi;
}

float boostArcAngleRad(int field) {
    // 0x249838..0x249864: f20 = -(float)m * 2^-18 * 0.27777779f; z rotation, scaled by pi.
    const int m = std::clamp(field, 0, kBoostFieldMax);
    return -(static_cast<float>(m) * 3.8146972656250e-06f) * kBoostSweep * kPi;
}

int daylightPhase(std::int32_t t) {
    // 00249670.c:14-22 (thresholds 0xE483, 80999, 0x2327F, 0x278D0).
    int v = 0;
    if (t > 0xe483 && (v = 1, t > 80999) && t > 0x2327f && (v = 2, t > 0x278d0)) v = 0;
    return v;
}

std::uint32_t clockBcd(std::int32_t t) {
    // 002495f0.c:15-21.
    const std::int32_t m = t / 0x96;
    const std::int32_t m10 = m / 10;
    const std::int32_t h = m10 / 6;
    return static_cast<std::uint32_t>(m % 10) | static_cast<std::uint32_t>(m10 % 6) << 4 | 0xa00u |
           static_cast<std::uint32_t>(h % 10) << 12 | static_cast<std::uint32_t>((h / 10) % 10) << 16;
}

namespace {

DrawQuad rotQuad(int entry, int csa, int px, int py, int u, int v, int w, int h, const float vs[8],
                 float angle) {
    DrawQuad q = quad(Table::Drive, entry, csa, Page::Meter0, u, v, w, h, px, py, 0);
    q.rotated = true;
    q.angleRad = angle;
    for (int i = 0; i < 8; ++i) q.verts[i] = vs[i];
    return q;
}

}  // namespace

std::vector<DrawQuad> buildGaugeHud(const GaugeInput& in) {
    const int vp = std::clamp(in.viewport, 0, 2);
    const int ox = kGaugeOrigins[vp].x, oy = kGaugeOrigins[vp].y;
    const auto T = Table::Drive;
    const auto P = Page::Meter0;  // tex table 0x2A9008 -> ITEM/METER0.GSL
    std::vector<DrawQuad> q;

    // 0x249780..0x2497A0: m = CalculateBoostGaugeValue(car,0,1) (clamped to 0x40000);
    // blink = (m <= 0x17FFF) && (param_1[2] & 0x20).
    const int m = std::clamp(in.meterField, 0, kBoostFieldMax);
    const bool blink = m <= kBoostBlinkLimit && (in.frame & 0x20u) != 0;

    // Lime arc: both quads use the template geometry from 0x327820 (pivot (70,70), 20x60 sprite,
    // strip vertices (46,-30),(66,-30),(46,30),(66,30)); and the inline copy at 0x2498C4.
    static const float kArcVerts[8] = {46.f, -30.f, 66.f, -30.f, 46.f, 30.f, 66.f, 30.f};
    // #1: entry 1, csa = blink ? 3 : 0, static record u=40 v=48.
    q.push_back(rotQuad(1, blink ? 3 : 0, ox + 70, oy + 70, 40, 48, 20, 60, kArcVerts, 0.f));
    // #2: csa = m==0 ? 5 : (blink ? 4 : 2), u=64 v=48, rotated by the boost law.
    q.push_back(rotQuad(1, m == 0 ? 5 : (blink ? 4 : 2), ox + 70, oy + 70, 64, 48, 20, 60, kArcVerts,
                        boostArcAngleRad(m)));
    // 0x24997C..0x2499C4: baked ring, entry 0 csa 0, sprite (0,0,u0,v0,140x128).
    q.push_back(quad(T, 0, 0, P, 0, 0, 140, 128, ox, oy, 1));
    // 0x2499D4..0x249A74: gear glyph: x=97 y=60, u=(g%5)*20+140, v=(g/5)*16+80, 20x14.
    const int g = in.gear & 0xff;
    q.push_back(quad(T, 0, 0, P, (g % 5) * 20 + 140, (g / 5) * 16 + 80, 20, 14, ox + 97, oy + 60, 1));
    // 0x249A7C..0x249B48: speed digits x=0x59 step 0x11, y=0x5e, u=(d%7)*16+140, v=(d/7)*23, 16x23.
    const int mph = hostMph(in.speedRaw, in.speedDivisor);
    for (const SpeedCell& c : speedCells(mph, 0x59, 0x11))
        q.push_back(quad(T, 0, 0, P, (c.digit % 7) * 16 + 140, (c.digit / 7) * 23, 16, 23, ox + c.dx,
                         oy + 0x5e, 1));
    // 0x249B4C..0x249D00: town clock plate (UpdateRaceHud param_7).
    if (in.townMode) {
        q.push_back(quad(T, 1, 12, P, 0, 0, 94, 38, ox + 20, oy + 100, 1));  // template 0x327890
        const int phase = daylightPhase(in.clockTicks);
        q.push_back(quad(T, 0, 0, P, phase * 24 + 140, 49, 24, 24, ox + 22, oy + 113, 1));
        std::uint32_t bcd = clockBcd(in.clockTicks);
        int x = 0x5b;
        for (int i = 0; i < 5; ++i) {
            q.push_back(quad(T, 1, 12, P, static_cast<int>(bcd & 0xf) * 8, 40, 8, 8, ox + x, oy + 0x7a, 5));
            bcd = static_cast<std::uint32_t>(static_cast<std::int32_t>(bcd) >> 4);
            x -= 9;
        }
    }
    // 0x249D04..0x249DBC: transmission icon (template 0x3278B0), csa 12/14/13 for mode 0/1/2.
    if (in.showTransmission) {
        const int csa = in.transmissionMode == 0 ? 12 : (in.transmissionMode == 1 ? 14 : 13);
        q.push_back(quad(T, 1, csa, P, 16, 48, 16, 27, ox + 29, oy + 59, 1));
    }
    // 0x249DC0..0x249EDC: tachometer needle, entry 1 csa 12, pivot (70,68), u=3 v=53 8x59,
    // strip vertices (-4,-59),(4,-59),(-4,0),(4,0), z rotation = needleAngleRad(rpm).
    static const float kNeedleVerts[8] = {-4.f, -59.f, 4.f, -59.f, -4.f, 0.f, 4.f, 0.f};
    q.push_back(rotQuad(1, 12, ox + 70, oy + 68, 3, 53, 8, 59, kNeedleVerts, needleAngleRad(in.rpm)));
    // 0x249EE0..0x249F5C: hub cap (template 0x3278D0) entry 1 csa 12, (64,64,u16,v80,12x12).
    q.push_back(quad(T, 1, 12, P, 16, 80, 12, 12, ox + 64, oy + 64, 1));
    return q;
}

// ----------------------------------------------------------------- race HUD

std::uint32_t raceTimerBcd(std::int32_t f) {
    // 0022f750.c:49-62.
    if (f == 0 || f == 0x7fffffff) return 0xaaaaaa;
    if (f >= 360000) return 0x995999;
    const std::int32_t sec = (f / 0x3c) % 0x3c;
    std::uint32_t v = static_cast<std::uint32_t>(f / 36000);
    v = v * 0x10 + static_cast<std::uint32_t>((f / 0xe10) % 10);
    v = v * 0x10 + static_cast<std::uint32_t>(sec / 10);
    v = v * 0x10 + static_cast<std::uint32_t>(sec % 10);
    v = v * 0x10 + static_cast<std::uint32_t>((f % 0x3c) / 6);
    v = v * 0x10 + static_cast<std::uint32_t>(((f % 0x3c) * 100 / 0x3c) % 10);
    return v;
}

std::int32_t raceHudTimerValue(std::int32_t totalFrames) {
    return totalFrames > 0 ? totalFrames - 1 : 0;
}

std::vector<DrawQuad> buildRaceTimePanel(const TimePanelInput& in) {
    std::vector<DrawQuad> q;
    const auto R = Table::Race;
    const int ox = in.x, oy = in.y;
    // 0022f750.c:24-48: panel (entry 0, csa 1, tag 0xe): caps 18x24 at atlas v=176,
    // 12 x 8-wide middle tiles at x = 0x12 + 8k.
    q.push_back(quad(R, 0, 1, Page::None, 0, 0xb0, 0x12, 0x18, ox + 0, oy, 0xe));
    q.push_back(quad(R, 0, 1, Page::None, 0x1a, 0xb0, 0x12, 0x18, ox + 0x72, oy, 0xe));
    for (int k = 0; k < 12; ++k)
        q.push_back(quad(R, 0, 1, Page::None, 0x12, 0xb0, 8, 0x18, ox + 0x12 + 8 * k, oy, 0xe));
    // :63-96: separators (u 0xa0/0xb0, v 0x30) at x 0x20 / 0x4a, y 4; digits v=0x40 at
    // x = 0x66, then steps -14 / -28 alternately (even iteration -16+2, odd -32+4).
    const int csa = in.digitCsa;
    q.push_back(quad(R, 0, csa, Page::None, 0xa0, 0x30, 0x10, 0x10, ox + 0x20, oy + 4, 8));
    q.push_back(quad(R, 0, csa, Page::None, 0xb0, 0x30, 0x10, 0x10, ox + 0x4a, oy + 4, 8));
    std::uint32_t bcd = raceTimerBcd(in.timeFrames);
    int x = 0x66;
    for (int i = 0; i < 6; ++i) {
        const int d = static_cast<int>(bcd & 0xf);
        bcd = static_cast<std::uint32_t>(static_cast<std::int32_t>(bcd) >> 4);
        q.push_back(quad(R, 0, csa, Page::None, d << 4, 0x40, 0x10, 0x10, ox + x, oy + 4, 8));
        x = (i % 2 == 0) ? x - 0x10 + 2 : x - 0x20 + 4;
    }
    // :97-140: lap badge + count (entry 1).
    const int lap = in.lap;
    if (lap != -1) {
        q.push_back(quad(R, 1, 8, Page::None, 0xe8, 0xa8, 0x18, 0x18, ox - 26, oy, 1));
        if (lap == 0) {
            q.push_back(quad(R, 1, 7, Page::None, 0x50, 0xc8, 0x20, 0x10, ox - 30, oy + 5, 1));
        } else if (lap < 10) {
            q.push_back(quad(R, 1, 7, Page::None, lap << 3, 0xc8, 8, 0x10, ox - 18, oy + 5, 1));
        } else {
            q.push_back(quad(R, 1, 7, Page::None, (lap / 10) << 3, 0xc8, 8, 0x10, ox - 22, oy + 5, 2));
            q.push_back(quad(R, 1, 7, Page::None, (lap % 10) << 3, 0xc8, 8, 0x10, ox - 14, oy + 5, 2));
        }
    }
    return q;
}

std::vector<DrawQuad> buildRacePosition(const PositionInput& in) {
    std::vector<DrawQuad> q;
    const auto R = Table::Race;
    const int ox = in.x, oy = in.y;
    const int rank = in.rank;
    // 0022fe50.c:41-49 / 82-90: ordinal suffix index (zero-based rank r): r%10 when < 3 and
    // not in 10..19, else 3 ("th").
    const int r10 = rank % 10;
    int k = 3;
    if (r10 < 3 && rank / 10 != 1) k = r10;
    const int place = rank + 1;
    if (in.total == 0) {
        if (rank == -1) return q;  // 0022fe50.c:37 `param_2[3] != -1` (small variant only)
        q.push_back(quad(R, 0, 0, Page::None, k * 0x16 + 0x20, 0x1f, 0x16, 0x10, ox + 0x1e, oy, 1));
        if (place < 10) {
            q.push_back(quad(R, 0, 0, Page::None, place * 0x10, 0x40, 0x10, 0x10, ox + 0xe, oy, 1));
        } else {
            q.push_back(quad(R, 0, 0, Page::None, (place % 10) << 4, 0x40, 0x10, 0x10, ox + 0xe, oy, 2));
            q.push_back(quad(R, 0, 0, Page::None, (place / 10) << 4, 0x40, 0x10, 0x10, ox, oy, 2));
        }
        return q;
    }
    q.push_back(quad(R, 0, 0, Page::None, 0xa0, 0x69, 0x20, 0x20, ox + 0x33, oy, 2));
    q.push_back(quad(R, 0, 0, Page::None, k * 0x16 + 0x20, 0x1f, 0x16, 0x10, ox + 0x33, oy, 2));
    if (place < 10) {
        q.push_back(quad(R, 0, 0, Page::None, (place % 9) * 0x1c, (place / 9) * 0x1e, 0x1c, 0x1e,
                         ox + 0x17, oy, 1));
    } else {
        q.push_back(quad(R, 0, 0, Page::None, ((place % 10) % 9) * 0x1c, ((place % 10) / 9) * 0x1e,
                         0x1c, 0x1e, ox + 0x17, oy, 2));
        q.push_back(quad(R, 0, 0, Page::None, ((place / 10) % 9) * 0x1c, (place / 0x5a) * 0x1e, 0x1c,
                         0x1e, ox, oy, 2));
    }
    const int total = in.total;
    if (total < 10) {
        q.push_back(quad(R, 0, 1, Page::None, total * 0xc + 0x20, 0x30, 0xc, 0x10, ox + 0x47, oy + 0x10, 1));
    } else {
        q.push_back(quad(R, 0, 1, Page::None, (total % 10) * 0xc + 0x20, 0x30, 0xc, 0x10, ox + 0x47,
                         oy + 0x10, 2));
        q.push_back(quad(R, 0, 1, Page::None, (total / 10) * 0xc + 0x20, 0x30, 0xc, 0x10, ox + 0x3d,
                         oy + 0x10, 2));
    }
    return q;
}

// ----------------------------------------------------------------- needle

// Measured 2026-10-17 from build/scratch/oracle-golden/*.png (alpha over black, 1280x447):
// frame-diff mask (pixel = non-bg vs the greenest of the 4 frames), component touching the
// hub, PCA axis oriented away from the hub; hub = LS intersection of the 3 distinct axes
// (residuals 0.24/0.00/0.24 px).  Angle = atan2(-dy, dx).  See the W2-10 report.
const NeedleOraclePoint kNeedleOraclePoints[4] = {
    {"race_hud", 39, 198.29, 504},
    {"race_hud_driving", 63, 117.80, 586},
    {"town_driving", 0, 199.40, 518},
    {"town_driving_street", 0, 199.40, 518},
};

double shotAngleToNeedleThetaDeg(double shotAngleDeg) {
    const double pi = 3.14159265358979323846;
    const double a = shotAngleDeg * pi / 180.0;
    const double phiUi = std::atan2(std::sin(a), std::cos(a) / 2.0) * 180.0 / pi;  // 0 = +x, up +
    double theta = 90.0 - phiUi;
    while (theta > 180.0) theta -= 360.0;
    while (theta < -180.0) theta += 360.0;
    return theta;
}

double impliedRpm(double thetaDeg) {
    return (thetaDeg / 180.0 - static_cast<double>(kNeedleRpmBias)) * static_cast<double>(kNeedleRpmFull);
}

// ----------------------------------------------------------------- rasterizer

namespace {

std::uint64_t rd64(const std::vector<std::uint8_t>& b, std::size_t o) {
    std::uint64_t v = 0;
    for (int i = 7; i >= 0; --i) v = (v << 8) | b[o + static_cast<std::size_t>(i)];
    return v;
}
std::uint64_t bitsOf(std::uint64_t v, unsigned first, unsigned count) {
    return (v >> first) & ((1ull << count) - 1ull);
}
std::uint32_t csm1(std::uint32_t i) { return (i & ~0x18u) | ((i & 8u) << 1) | ((i & 0x10u) >> 1); }

struct GslUnit { unsigned psm, w, h; std::size_t pix, bytes; };

bool walkUnits(const std::vector<std::uint8_t>& b, std::vector<GslUnit>& units) {
    std::size_t at = 0;
    while (at + 112 <= b.size() && b[at + 3] != 0x70) {
        if (b[at + 3] != 0x10) return false;
        const std::size_t qwc = b[at] | (static_cast<std::size_t>(b[at + 1]) << 8);
        const std::size_t p = at + 16;
        const std::uint64_t tag = rd64(b, p + 80), blit = rd64(b, p + 16), reg = rd64(b, p + 48);
        const std::size_t n = tag & 0x7fff;
        if (qwc != 6 + n || p + 96 + n * 16 > b.size()) return false;
        units.push_back({static_cast<unsigned>(bitsOf(blit, 56, 6)), static_cast<unsigned>(bitsOf(reg, 0, 12)),
                         static_cast<unsigned>(bitsOf(reg, 32, 12)), p + 96, n * 16});
        at += (qwc + 1) * 16;
    }
    return true;
}

}  // namespace

const Rgba8Image* GaugeArt::page(int entry, int csa) const {
    if (entry == 0) return entry0.width ? &entry0 : nullptr;
    if (entry < 1 || entry > 2) return nullptr;
    const Rgba8Image& im = entryT4[static_cast<std::size_t>(entry - 1)][static_cast<std::size_t>(csa & 15)];
    return im.width ? &im : nullptr;
}

bool decodeGaugeArt(const std::vector<std::uint8_t>& b, GaugeArt& out) {
    out = {};
    std::vector<GslUnit> u;
    if (!walkUnits(b, u)) return false;
    int t4 = 0;
    for (std::size_t i = 0; i + 1 < u.size(); ++i) {
        const GslUnit& t = u[i];
        const GslUnit& pal = u[i + 1];
        if (t.psm == 0x13 && pal.psm == 0 && pal.bytes == 1024 && t.w == 256 && out.entry0.width == 0 &&
            t.bytes == static_cast<std::size_t>(t.w) * t.h) {
            out.entry0.width = static_cast<int>(t.w);
            out.entry0.height = static_cast<int>(t.h);
            out.entry0.rgba.resize(static_cast<std::size_t>(t.w) * t.h * 4);
            for (std::size_t k = 0; k < static_cast<std::size_t>(t.w) * t.h; ++k) {
                const std::size_t c = pal.pix + csm1(b[t.pix + k]) * 4u;
                std::uint8_t* d = &out.entry0.rgba[k * 4];
                d[0] = b[c]; d[1] = b[c + 1]; d[2] = b[c + 2];
                const unsigned a = b[c + 3];
                d[3] = static_cast<std::uint8_t>(a > 128 ? 255 : a * 255u / 128u);
            }
        } else if (t.psm == 0x14 && pal.psm == 2 && pal.bytes == 512 && t4 < 2 &&
                   t.bytes == (static_cast<std::size_t>(t.w) * t.h + 1) / 2) {
            for (int csa = 0; csa < 16; ++csa) {
                Rgba8Image& im = out.entryT4[static_cast<std::size_t>(t4)][static_cast<std::size_t>(csa)];
                im.width = static_cast<int>(t.w);
                im.height = static_cast<int>(t.h);
                im.rgba.resize(static_cast<std::size_t>(t.w) * t.h * 4);
                for (std::size_t k = 0; k < static_cast<std::size_t>(t.w) * t.h; ++k) {
                    const unsigned nib = (b[t.pix + k / 2] >> ((k & 1u) * 4u)) & 0xfu;
                    const std::size_t c = pal.pix + csm1(static_cast<std::uint32_t>(csa) * 16u + nib) * 2u;
                    const unsigned p16 = b[c] | (static_cast<unsigned>(b[c + 1]) << 8);
                    std::uint8_t* d = &im.rgba[k * 4];
                    d[0] = static_cast<std::uint8_t>((p16 & 0x1f) << 3);
                    d[1] = static_cast<std::uint8_t>(((p16 >> 5) & 0x1f) << 3);
                    d[2] = static_cast<std::uint8_t>(((p16 >> 10) & 0x1f) << 3);
                    d[3] = p16 == 0 ? 0 : 255;
                }
            }
            ++t4;
        }
    }
    return out.entry0.width != 0;
}

namespace {

inline void blend(Canvas& canvas, int cx, int cy, const std::uint8_t* s) {
    const int a = s[3];
    if (a == 0 || cx < 0 || cx >= canvas.width || cy < 0 || cy >= canvas.height) return;
    std::uint8_t* d = &canvas.rgb[(static_cast<std::size_t>(cy) * canvas.width + cx) * 3];
    for (int c = 0; c < 3; ++c) d[c] = static_cast<std::uint8_t>((s[c] * a + d[c] * (255 - a) + 127) / 255);
}

}  // namespace

void rasterize(const std::vector<DrawQuad>& quads, const GaugeArt* pages[3], Canvas& canvas) {
    for (const DrawQuad& q : quads) {
        const GaugeArt* art = pages[static_cast<int>(q.page)];
        if (q.page == Page::None || art == nullptr) continue;
        const Rgba8Image* img = art->page(q.texEntry, q.csa);
        if (img == nullptr) continue;
        if (!q.rotated) {
            for (int j = 0; j < q.h; ++j) {
                const int sy = q.v + j;
                if (sy < 0 || sy >= img->height) continue;
                for (int i = 0; i < q.w; ++i) {
                    const int sx = q.u + i;
                    if (sx < 0 || sx >= img->width) continue;
                    blend(canvas, q.x + i, q.y + j,
                          &img->rgba[(static_cast<std::size_t>(sy) * img->width + sx) * 4]);
                }
            }
            continue;
        }
        // Rotated strip quad: local rectangle [x0,x1] x [y0,y2] around the pivot.
        const float x0 = q.verts[0], x1 = q.verts[2], y0 = q.verts[1], y1 = q.verts[5];
        const float c = std::cos(q.angleRad), s = std::sin(q.angleRad);
        float minx = 1e9f, maxx = -1e9f, miny = 1e9f, maxy = -1e9f;
        for (int k = 0; k < 4; ++k) {
            const float lx = q.verts[static_cast<std::size_t>(k * 2)], ly = q.verts[static_cast<std::size_t>(k * 2 + 1)];
            const float px = q.x + lx * c - ly * s, py = q.y + lx * s + ly * c;
            minx = std::min(minx, px); maxx = std::max(maxx, px);
            miny = std::min(miny, py); maxy = std::max(maxy, py);
        }
        for (int cy = static_cast<int>(std::floor(miny)); cy <= static_cast<int>(std::ceil(maxy)); ++cy) {
            for (int cx = static_cast<int>(std::floor(minx)); cx <= static_cast<int>(std::ceil(maxx)); ++cx) {
                const float dx = static_cast<float>(cx) + 0.5f - static_cast<float>(q.x);
                const float dy = static_cast<float>(cy) + 0.5f - static_cast<float>(q.y);
                const float lx = dx * c + dy * s;     // inverse rotation
                const float ly = -dx * s + dy * c;
                if (lx < x0 || lx >= x1 || ly < y0 || ly >= y1) continue;
                const int sx = q.u + static_cast<int>((lx - x0) / (x1 - x0) * static_cast<float>(q.w));
                const int sy = q.v + static_cast<int>((ly - y0) / (y1 - y0) * static_cast<float>(q.h));
                if (sx < 0 || sx >= img->width || sy < 0 || sy >= img->height) continue;
                blend(canvas, cx, cy, &img->rgba[(static_cast<std::size_t>(sy) * img->width + sx) * 4]);
            }
        }
    }
}

std::vector<std::uint8_t> toShotSpace(const Canvas& canvas) {
    const int W = kShotWidth, H = kShotHeight;
    std::vector<float> lin(static_cast<std::size_t>(W) * H * 3);
    for (int sy = 0; sy < H; ++sy) {
        const int uy = std::min(canvas.height - 1, sy + 1);
        for (int sx = 0; sx < W; ++sx) {
            const float xs = (static_cast<float>(sx) + 0.5f) / 2.f - 0.5f;
            const int i0 = std::clamp(static_cast<int>(std::floor(xs)), 0, canvas.width - 1);
            const int i1 = std::min(canvas.width - 1, i0 + 1);
            const float t = xs - std::floor(xs);
            for (int c = 0; c < 3; ++c) {
                const float a = canvas.rgb[(static_cast<std::size_t>(uy) * canvas.width + i0) * 3 + c];
                const float b = canvas.rgb[(static_cast<std::size_t>(uy) * canvas.width + i1) * 3 + c];
                lin[(static_cast<std::size_t>(sy) * W + sx) * 3 + c] = a * (1.f - t) + b * t;
            }
        }
    }
    constexpr int R = 2;
    float k[2 * R + 1], ksum = 0.f;
    for (int d = -R; d <= R; ++d) {
        k[d + R] = std::exp(-static_cast<float>(d * d) / (2.f * kShotSigma * kShotSigma));
        ksum += k[d + R];
    }
    for (float& v : k) v /= ksum;
    auto refl = [](int i, int n) { if (i < 0) i = -i - 1; if (i >= n) i = 2 * n - i - 1; return std::clamp(i, 0, n - 1); };
    std::vector<float> tmp(lin.size());
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            for (int c = 0; c < 3; ++c) {
                float acc = 0.f;
                for (int d = -R; d <= R; ++d)
                    acc += k[d + R] * lin[(static_cast<std::size_t>(y) * W + refl(x + d, W)) * 3 + c];
                tmp[(static_cast<std::size_t>(y) * W + x) * 3 + c] = acc;
            }
    std::vector<std::uint8_t> out(lin.size());
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            for (int c = 0; c < 3; ++c) {
                float acc = 0.f;
                for (int d = -R; d <= R; ++d)
                    acc += k[d + R] * tmp[(static_cast<std::size_t>(refl(y + d, H)) * W + x) * 3 + c];
                out[(static_cast<std::size_t>(y) * W + x) * 3 + c] =
                    static_cast<std::uint8_t>(std::clamp(acc, 0.f, 255.f));
            }
    return out;
}

}  // namespace rt::native::gameplay::hud_live
