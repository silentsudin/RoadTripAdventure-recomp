// W2-10 hud_live acceptance (w2_hud_live_test).
//
//   w2_hud_live_test --elf <SLUS_203.98> [--disc DIR] [--oracle-crops DIR] [--allow-skip]
//
// Prints exactly one `W2-10 SCORE k=v ... PASS|FAIL` line; exit 0 only when every
// pre-registered threshold is met, 2 when the ELF / disc / oracle crops are missing
// (NO_ELF / NO_DISC / NO_ORACLE; never a silent pass).  No ROM bytes are embedded:
// only counts, hashes and derived numbers with the cited ELF addresses.

#include "assets/hud_art.h"
#include "gameplay/game_loop.h"
#include "gameplay/hud_live.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace hl = rt::native::gameplay::hud_live;
namespace fs = std::filesystem;

namespace {

struct Hasher {
    std::uint64_t h = 1469598103934665603ull;
    void u64(std::uint64_t v) {
        for (int i = 0; i < 8; ++i) { h ^= (v >> (8 * i)) & 0xff; h *= 1099511628211ull; }
    }
    void str(const std::string& s) { for (unsigned char c : s) { h ^= c; h *= 1099511628211ull; } }
};

struct Rect { int x, y, w, h; };

struct Results {
    long rectErr = 0;
    int rectQuads = 0;
    int timerPass = 0, timerTotal = 0;
    int timerOraclePass = 0, timerOracleTotal = 0;
    int timerOracleAltPass = 0;
    int speedOraclePass = 0, speedOracleTotal = 0;
    int elfPass = 0, elfTotal = 0;
    int roiFrames = 0;
    double roiMadMax = -1, roiHistMin = 2;
    std::vector<std::string> info;
    std::uint64_t hash = 0;
};

// ------------------------------------------------------------ ELF helpers

std::vector<std::uint8_t> readFile(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

std::uint32_t le32(const std::vector<std::uint8_t>& elf, std::uint32_t vaddr) {
    const std::size_t o = vaddr - 0x1FF000u;
    return elf[o] | (elf[o + 1] << 8) | (elf[o + 2] << 16) | (static_cast<std::uint32_t>(elf[o + 3]) << 24);
}

std::uint64_t le64(const std::vector<std::uint8_t>& elf, std::uint32_t vaddr) {
    return le32(elf, vaddr) | (static_cast<std::uint64_t>(le32(elf, vaddr + 4)) << 32);
}

std::uint64_t fnvRange(const std::vector<std::uint8_t>& elf, std::uint32_t a, std::uint32_t e) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (std::uint32_t v = a; v < e; ++v) { h ^= elf[v - 0x1FF000u]; h *= 0x100000001b3ull; }
    return h;
}

// ------------------------------------------------------------ rect checks

long rectDiff(const hl::DrawQuad& q, const Rect& r) {
    return std::labs(q.x - r.x) + std::labs(q.y - r.y) + std::labs(q.w - r.w) + std::labs(q.h - r.h);
}

// Compare a produced list against an expected rect list; count mismatch = 10000 per missing/extra.
void checkRects(Results& r, const std::vector<hl::DrawQuad>& got, const std::vector<Rect>& want) {
    r.rectQuads += static_cast<int>(want.size());
    if (got.size() != want.size()) r.rectErr += 10000 * std::labs(static_cast<long>(got.size()) - static_cast<long>(want.size()));
    for (std::size_t i = 0; i < std::min(got.size(), want.size()); ++i) r.rectErr += rectDiff(got[i], want[i]);
}

void uvCheck(Results& r, const hl::DrawQuad& q, int u, int v) {
    r.rectErr += std::labs(q.u - u) + std::labs(q.v - v);
}

void runRects(Results& r) {
    // ---- drive HUD, 1P: hud-layout.md §1.2 (anchor (492,336)).
    {
        hl::HudLiveInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = 63.f;   // -> "63"
        in.rpm = 7000;        // (7000*16+9999)/10000 = 12 strips
        in.boostGauge = 3;    // non-zero: no idle sliver
        in.gear = 2;
        const auto q = hl::buildDriveHud2P(in, 0);
        std::vector<Rect> want = {
            {499, 412, 100, 18}, {499, 412, 100, 18},   // gear bar base + boost overlay
            {492, 336, 132, 96},                        // dial face
        };
        for (int k = 0; k < 12; ++k) want.push_back({526 + 6 * k, 336, 4, 60});  // strips
        want.push_back({570, 385, 20, 26});             // ones  (anchor+78, +49)
        want.push_back({551, 385, 20, 26});             // tens  (anchor+59)
        want.push_back({502, 398, 16, 10});             // gear/odometer cell (10,62)
        checkRects(r, q, want);
        if (q.size() == want.size()) {
            uvCheck(r, q[15], 3 * 20, 102);   // ones digit 3
            uvCheck(r, q[16], 6 * 20, 102);   // tens digit 6
            uvCheck(r, q[17], (2 + 1) * 16, 32);  // gear 2 -> u=(g+1)*16
            if (q[0].texEntry != 2 || q[1].texEntry != 1 || q[1].csa != 12) r.rectErr += 1000;
        }
    }
    // ---- boost==0 sliver + transmission icons.
    {
        hl::HudLiveInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = 0.f;
        in.boostGauge = 0;
        in.frame = 8;
        in.showTransmission = true;
        in.transmissionMode = 1;
        auto q = hl::buildDriveHud2P(in, 0);
        std::vector<Rect> want = {
            {499, 412, 100, 18}, {499, 412, 100, 18},
            {522, 418, 58, 8},                           // anchor+(30,82)
            {492, 336, 132, 96},
            {570, 385, 20, 26},                          // "0" ones cell (anchor+78, +49)
            {501, 348, 16, 16},                          // prompt icon mode 1: anchor+(9,12)
            {502, 398, 16, 10},
        };
        checkRects(r, q, want);
        if (q.size() == want.size() && q[2].csa != (8 >> 3) % 3 + 1) r.rectErr += 1000;
        in.transmissionMode = 2;
        q = hl::buildDriveHud2P(in, 0);
        want[5] = {501, 372, 16, 16};                    // anchor+(9,36)
        checkRects(r, q, want);
    }
    // ---- 2P viewport anchors (0x2A9AF0): index 1 = (492,112), index 2 = (492,336).
    for (int vp = 1; vp <= 2; ++vp) {
        hl::HudLiveInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = 5.f;
        in.boostGauge = 1;
        const int ay = vp == 1 ? 112 : 336;
        const auto q = hl::buildDriveHud2P(in, vp);
        std::vector<Rect> want = {
            {499, ay + 76, 100, 18}, {499, ay + 76, 100, 18}, {492, ay, 132, 96},
            {570, ay + 49, 20, 26}, {502, ay + 62, 16, 10},
        };
        checkRects(r, q, want);
    }
    // ---- 3-digit speed 123 -> x = 532, 551, 570 (right-aligned step 19).
    {
        hl::HudLiveInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = 123.f;
        in.boostGauge = 2;
        const auto q = hl::buildDriveHud2P(in, 0);
        std::vector<Rect> want = {
            {499, 412, 100, 18}, {499, 412, 100, 18}, {492, 336, 132, 96},
            {570, 385, 20, 26}, {551, 385, 20, 26}, {532, 385, 20, 26}, {502, 398, 16, 10},
        };
        checkRects(r, q, want);
    }
    // ---- handler 0 (1P gauge, origin (484,294) @0x2A8FF0).
    {
        hl::GaugeInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = 63.f;
        in.rpm = 7000;           // needle rotation exactly 0 rad
        in.meterField = 0x20000;
        in.gear = 2;
        auto q = hl::buildGaugeHud(in);
        std::vector<Rect> want = {
            {554, 364, 20, 60}, {554, 364, 20, 60},     // lime arc quads (pivot (70,70), 20x60 sprite)
            {484, 294, 140, 128},                       // ring
            {581, 354, 20, 14},                         // gear glyph (97,60)
            {573, 388, 16, 23}, {556, 388, 16, 23},     // ones/tens (0x59, 0x48; y 0x5e)
            {554, 362, 8, 59},                          // needle pivot (70,68)
            {548, 358, 12, 12},                         // hub cap (64,64)
        };
        checkRects(r, q, want);
        if (q.size() == want.size()) {
            uvCheck(r, q[4], (3 % 7) * 16 + 140, 0);       // '3'
            uvCheck(r, q[5], (6 % 7) * 16 + 140, 0);       // '6'
            uvCheck(r, q[3], 2 * 20 + 140, 80);            // gear 2
            uvCheck(r, q[0], 40, 48); uvCheck(r, q[1], 64, 48); uvCheck(r, q[6], 3, 53); uvCheck(r, q[7], 16, 80);
            if (!q[0].rotated || !q[1].rotated || !q[6].rotated || q[2].rotated) r.rectErr += 1000;
            if (q[0].csa != 0 || q[1].csa != 2 || q[6].csa != 12 || q[7].csa != 12 || q[6].texEntry != 1) r.rectErr += 1000;
            if (std::fabs(q[6].angleRad) > 1e-6f) r.rectErr += 1000;                   // rpm 7000 -> 0
            if (std::fabs(q[1].angleRad + (0x20000 / 262144.f) * 0.2777778f * 3.14159265f) > 1e-5f) r.rectErr += 1000;
        }
        in.speedRaw = 123.f; in.gear = 7; in.rpm = 0; in.meterField = 0;
        q = hl::buildGaugeHud(in);
        std::vector<Rect> want3 = {
            {554, 364, 20, 60}, {554, 364, 20, 60}, {484, 294, 140, 128}, {581, 354, 20, 14},
            {573, 388, 16, 23}, {556, 388, 16, 23}, {539, 388, 16, 23}, {554, 362, 8, 59}, {548, 358, 12, 12}};
        checkRects(r, q, want3);
        if (q.size() == want3.size()) {
            uvCheck(r, q[3], (7 % 5) * 20 + 140, (7 / 5) * 16 + 80);
            uvCheck(r, q[6], (1 % 7) * 16 + 140, 0);        // hundreds digit '1'
            if (q[1].csa != 5) r.rectErr += 1000;           // m==0 -> csa 5
            if (std::fabs(q[7].angleRad - (-126.f * 3.14159265f / 180.f)) > 1e-5f) r.rectErr += 1000;  // rpm 0
        }
        // digit 7 uses the second glyph row: v = (7/7)*23.
        in.speedRaw = 7.f; q = hl::buildGaugeHud(in);
        if (q.size() < 5 || q[4].v != 23 || q[4].u != 140) r.rectErr += 1000;
        // low meter + blink bit: csa 3 (static arc) and csa 4 (m != 0).
        in.meterField = 100; in.frame = 0x20; q = hl::buildGaugeHud(in);
        if (q[0].csa != 3 || q[1].csa != 4) r.rectErr += 1000;
        in.frame = 0; q = hl::buildGaugeHud(in);
        if (q[0].csa != 0 || q[1].csa != 2) r.rectErr += 1000;
        // m above 0x17FFF never blinks.
        in.meterField = 0x18000; in.frame = 0x20; q = hl::buildGaugeHud(in);
        if (q[0].csa != 0 || q[1].csa != 2) r.rectErr += 1000;
        // town plate + clock "09:15" (ticks 9*9000 + 15*150 = 83250) + sun, transmission icon mode 2.
        in = {};
        in.townMode = true; in.clockTicks = 83250; in.showTransmission = true; in.transmissionMode = 2;
        q = hl::buildGaugeHud(in);
        std::vector<Rect> wantT = {
            {554, 364, 20, 60}, {554, 364, 20, 60}, {484, 294, 140, 128}, {581, 354, 20, 14},
            {573, 388, 16, 23},
            {504, 394, 94, 38},                         // plate (20,100)
            {506, 407, 24, 24},                         // sun icon (22,113)
            {575, 416, 8, 8}, {566, 416, 8, 8}, {557, 416, 8, 8}, {548, 416, 8, 8}, {539, 416, 8, 8},
            {513, 353, 16, 27},                         // transmission icon (29,59)
            {554, 362, 8, 59}, {548, 358, 12, 12}};
        checkRects(r, q, wantT);
        if (q.size() == wantT.size()) {
            uvCheck(r, q[6], 1 * 24 + 140, 49);          // daytime phase 1
            // glyph nibbles low->high x 575..539: 5, 1, 0xA(colon), 9, 0
            const int nib[5] = {5, 1, 10, 9, 0};
            for (int k = 0; k < 5; ++k) uvCheck(r, q[7 + k], nib[k] * 8, 40);
            if (q[12].csa != 13) r.rectErr += 1000;      // mode 2 -> csa 13
        }
    }
    // ---- race time panel, 1P default origin (60,52): hud-layout.md §1.1.
    {
        hl::TimePanelInput in;
        in.timeFrames = 4076;     // 00'40"76 pattern but any value; lap 3
        in.lap = 3;
        const auto q = hl::buildRaceTimePanel(in);
        std::vector<Rect> want;
        want.push_back({60, 52, 18, 24});            // left cap
        want.push_back({60 + 114, 52, 18, 24});      // right cap
        for (int k = 0; k < 12; ++k) want.push_back({60 + 18 + 8 * k, 52, 8, 24});
        want.push_back({60 + 32, 56, 16, 16});       // separator 1
        want.push_back({60 + 74, 56, 16, 16});       // separator 2
        for (int dx : {102, 88, 60, 46, 18, 4}) want.push_back({60 + dx, 56, 16, 16});
        want.push_back({60 - 26, 52, 24, 24});       // LAP badge
        want.push_back({60 - 18, 57, 8, 16});        // lap count (1 digit)
        checkRects(r, q, want);
        // lap 12 -> two 8x16 digits at x-22 / x-14; lap 0 -> 32x16 flag at x-30.
        in.lap = 12;
        auto q2 = hl::buildRaceTimePanel(in);
        if (q2.size() != q.size() + 1 || q2[q2.size() - 2].x != 60 - 22 || q2.back().x != 60 - 14) r.rectErr += 1000;
        in.lap = 0;
        q2 = hl::buildRaceTimePanel(in);
        if (q2.back().w != 32 || q2.back().x != 60 - 30 || q2.back().u != 80 || q2.back().v != 200) r.rectErr += 1000;
        in.lap = -1;
        if (hl::buildRaceTimePanel(in).size() != q.size() - 2) r.rectErr += 1000;
    }
    // ---- race position small (40,88) / large (128,88).
    {
        hl::PositionInput in;
        in.rank = 0;  // 1st
        auto q = hl::buildRacePosition(in);
        checkRects(r, q, {{40 + 30, 88, 22, 16}, {40 + 14, 88, 16, 16}});
        if (q.size() == 2) { uvCheck(r, q[0], 32, 31); uvCheck(r, q[1], 16, 64); }
        in.rank = 11;  // 12th: "th" (teens) and two digits
        q = hl::buildRacePosition(in);
        checkRects(r, q, {{70, 88, 22, 16}, {54, 88, 16, 16}, {40, 88, 16, 16}});
        if (q.size() == 3) uvCheck(r, q[0], 3 * 22 + 32, 31);
        in.x = 128; in.rank = 2; in.total = 8;  // 3rd of 8, large
        q = hl::buildRacePosition(in);
        checkRects(r, q, {{128 + 51, 88, 32, 32}, {128 + 51, 88, 22, 16}, {128 + 23, 88, 28, 30},
                          {128 + 71, 88 + 16, 12, 16}});
        if (q.size() == 4) { uvCheck(r, q[0], 160, 105); uvCheck(r, q[1], 2 * 22 + 32, 31); uvCheck(r, q[2], 3 * 28, 0); uvCheck(r, q[3], 8 * 12 + 32, 48); }
    }
}

// ------------------------------------------------------------ timer / speed

void runTimer(Results& r) {
    struct V { std::int32_t f; std::uint32_t bcd; };
    // Independent expected values (python divmod decomposition, not the decomp expression).
    static const V vecs[14] = {
        {0, 0xaaaaaa}, {1, 0x1}, {59, 0x98}, {60, 0x100}, {3599, 0x5998}, {3600, 0x10000},
        {35999, 0x95998}, {36000, 0x100000}, {359999, 0x995998}, {360000, 0x995999},
        {0x7fffffff, 0xaaaaaa}, {100, 0x166}, {12345, 0x32575}, {215999, 0x595998},
    };
    for (const V& v : vecs) {
        ++r.timerTotal;
        if (hl::raceTimerBcd(v.f) == v.bcd) ++r.timerPass;
    }
    // Oracle readouts: race_hud 00'40"76 (manifest total-time 2447), race_hud_driving 00'52"95 (3178).
    struct O { std::int32_t total; std::uint32_t bcd; };
    for (const O& o : {O{2447, 0x4076}, O{3178, 0x5295}}) {
        ++r.timerOracleTotal;
        if (hl::raceTimerBcd(hl::raceHudTimerValue(o.total)) == o.bcd) ++r.timerOraclePass;
        if (hl::raceTimerBcd(o.total) == o.bcd) ++r.timerOracleAltPass;  // frames-0 reading (must be 0/2)
    }
}

std::vector<int> digitsOf(const std::vector<hl::SpeedCell>& cells) {
    std::vector<int> d;
    for (auto it = cells.rbegin(); it != cells.rend(); ++it) d.push_back(it->digit);
    return d;
}

void runSpeed(Results& r) {
    // Oracle: race_hud "39", race_hud_driving "63" (coordinator-verified visual reads).
    for (int want : {39, 63}) {
        ++r.speedOracleTotal;
        // Shipped chain: raw (km/h-like, |v|*216) = want*1.609 + 0.5 -> trunc(raw/1.609) == want.
        const float raw = static_cast<float>(want) * hl::kShippedMphDivisor + 0.5f;
        const bool shipped = hl::shippedMphFromRaw(raw) == want;
        // Host divisors: raw hostSpeed that lands inside the digit bin must reproduce the digits.
        const bool d100 = hl::hostMph(static_cast<float>(want) * 100.f + 50.f, hl::kDefaultSpeedDivisor) == want;
        const bool d107 = hl::hostMph(static_cast<float>(want) * hl::kAltSpeedDivisor + 0.5f, hl::kAltSpeedDivisor) == want;
        const auto cells = hl::speedCells(want);
        const auto digs = digitsOf(cells);
        const bool cellsOk = digs.size() == 2 && digs[0] == want / 10 && digs[1] == want % 10 &&
                             cells[0].dx == 78 && cells[1].dx == 59;
        if (shipped && d100 && d107 && cellsOk) ++r.speedOraclePass;
    }
    // Handler 0 (oracle layout): same digits, x = 0x59 / 0x48 (step 0x11).
    for (int want : {39, 63}) {
        const auto c = hl::speedCells(want, 0x59, 0x11);
        if (c.size() != 2 || c[0].digit != want % 10 || c[1].digit != want / 10 || c[0].dx != 89 || c[1].dx != 72)
            r.rectErr += 1000;
    }
    // Single-digit town golden ("0"): one cell at x=78.
    {
        const auto c = hl::speedCells(0);
        if (c.size() != 1 || c[0].digit != 0 || c[0].dx != 78) r.rectErr += 1000;
    }
    // BCD / count properties.
    if (hl::packedBcd(186) != 0x186 || hl::packedBcd(0) != 0 || hl::packedBcd(12345678) != 0x12345678) r.rectErr += 1000;
    if (hl::speedCells(1234).size() != 3) r.rectErr += 1000;          // max 3 cells
    if (hl::tachStripCount(0) != 0 || hl::tachStripCount(10000) != 16 || hl::tachStripCount(20000) != 16 ||
        hl::tachStripCount(1) != 1) r.rectErr += 1000;

    // Informational: which host divisor reproduces the oracle digits at the paired native
    // frames (race timer value 2446 / 3177)?  The host AI speed is ~18.5k units, i.e. a
    // different regime from the oracle's 39/63 mph, so neither divisor can match.
    rt::native::gameplay::GameLoopParams p;
    p.raceMode = true;
    rt::native::gameplay::GameLoop g(p);
    g.reset();
    rt::native::gameplay::MenuInput mi{};
    float sp2446 = 0, sp3177 = 0;
    for (int f = 1; f <= 3177; ++f) {
        const auto s = g.tick(1.f / 60.f, mi);
        if (f == 2446) sp2446 = s.speed;
        if (f == 3177) sp3177 = s.speed;
    }
    char b[256];
    std::snprintf(b, sizeof b,
                  "native_pair: frame2446 speed=%.0f -> /100=%d /107.527=%d (oracle 39); "
                  "frame3177 speed=%.0f -> /100=%d /107.527=%d (oracle 63)",
                  sp2446, hl::hostMph(sp2446, hl::kDefaultSpeedDivisor), hl::hostMph(sp2446, hl::kAltSpeedDivisor),
                  sp3177, hl::hostMph(sp3177, hl::kDefaultSpeedDivisor), hl::hostMph(sp3177, hl::kAltSpeedDivisor));
    r.info.push_back(b);
    std::snprintf(b, sizeof b, "raw_for_oracle: 39mph needs host raw [%.0f,%.0f) @/100 or [%.1f,%.1f) @/107.527; "
                  "63mph [%.0f,%.0f) / [%.1f,%.1f)",
                  3900.0, 4000.0, 39 * hl::kAltSpeedDivisor, 40 * hl::kAltSpeedDivisor,
                  6300.0, 6400.0, 63 * hl::kAltSpeedDivisor, 64 * hl::kAltSpeedDivisor);
    r.info.push_back(b);
}

// ------------------------------------------------------------ ELF pins

void runElf(Results& r, const std::vector<std::uint8_t>& elf) {
    auto expect = [&](bool ok) { ++r.elfTotal; if (ok) ++r.elfPass; };
    // Anchors @0x2A9AF0 (u32 x,y pairs).
    const std::uint32_t wantAnchors[6] = {492, 336, 492, 112, 492, 336};
    for (int i = 0; i < 6; ++i) expect(le32(elf, 0x2A9AF0 + 4 * i) == wantAnchors[i]);
    for (int i = 0; i < 3; ++i) {
        expect(hl::kDriveAnchors[i].x == static_cast<int>(le32(elf, 0x2A9AF0 + 8 * i)));
        expect(hl::kDriveAnchors[i].y == static_cast<int>(le32(elf, 0x2A9AF0 + 8 * i + 4)));
    }
    // Texture tables: TEX0 (+8) TBP0 (bits 0..13), PSM bits 20..25 = 0x13 (PSMT8).
    const std::uint32_t driveTbp[3] = {0x237A, 0x23FE, 0x240C};
    const std::uint32_t raceTbp[3] = {0x2C36, 0x2CB8, 0x2D3A};
    for (int i = 0; i < 3; ++i) {
        const std::uint64_t d = le64(elf, 0x2A9B08 + 16 * i + 8);
        const std::uint64_t c = le64(elf, 0x29F5F8 + 16 * i + 8);
        expect((d & 0x3fff) == driveTbp[i] && ((d >> 20) & 0x3f) == 0x13);
        expect((c & 0x3fff) == raceTbp[i] && ((c >> 20) & 0x3f) == 0x13);
    }
    // CSA table DAT_0029FCA8 = {0,1,12,0}; shipped mph divisors are both 1.609f.
    const std::uint32_t csa[4] = {0, 1, 12, 0};
    for (int i = 0; i < 4; ++i) expect(le32(elf, 0x29FCA8 + 4 * i) == csa[i]);
    std::uint32_t kb;
    std::memcpy(&kb, &hl::kShippedMphDivisor, 4);
    expect(le32(elf, 0x332F44) == kb && le32(elf, 0x332F48) == kb);
    // Handler 0 (oracle gauge): origin table @0x2A8FF0, tex table @0x2A9008, float constants, and the
    // four sprite-record templates the handler memcpy's (0x327820 / 0x327890 / 0x3278B0 / 0x3278D0).
    const std::uint32_t wantOrg[6] = {484, 294, 484, 70, 484, 294};
    for (int i = 0; i < 6; ++i) expect(le32(elf, 0x2A8FF0 + 4 * i) == wantOrg[i]);
    for (int i = 0; i < 3; ++i) {
        expect(hl::kGaugeOrigins[i].x == static_cast<int>(le32(elf, 0x2A8FF0 + 8 * i)));
        expect(hl::kGaugeOrigins[i].y == static_cast<int>(le32(elf, 0x2A8FF0 + 8 * i + 4)));
    }
    {
        const std::uint64_t e0 = le64(elf, 0x2A9008), e1 = le64(elf, 0x2A9018), e2 = le64(elf, 0x2A9028);
        expect((e0 & 0x3fff) == 0x237A && ((e0 >> 20) & 0x3f) == 0x13 && ((e0 >> 26) & 15) == 8 && ((e0 >> 30) & 15) == 7);
        expect((e1 & 0x3fff) == 0x23FE && ((e1 >> 20) & 0x3f) == 0x14 && ((e1 >> 26) & 15) == 7 && ((e1 >> 30) & 15) == 7);
        expect((e2 & 0x3fff) == 0x23FE && ((e2 >> 20) & 0x3f) == 0x14);
    }
    auto f32at = [&](std::uint32_t a) { const std::uint32_t w = le32(elf, a); float f; std::memcpy(&f, &w, 4); return f; };
    expect(f32at(0x332F4C) == hl::kBoostSweep);
    expect(f32at(0x332F50) == hl::kNeedleRpmFull);
    expect(f32at(0x332F54) == hl::kNeedleRpmBias);
    expect(f32at(0x332F84) == hl::kPi);
    {
        hl::GaugeInput gi;
        gi.townMode = true; gi.showTransmission = true; gi.transmissionMode = 1;
        const auto q = hl::buildGaugeHud(gi);
        // templates: {x,y,u,v,w,h} ints; arc verts floats at +0x20.
        auto tmpl = [&](std::uint32_t a, const hl::DrawQuad& d, int px, int py) {
            const int rx = static_cast<int>(le32(elf, a)), ry = static_cast<int>(le32(elf, a + 4));
            expect(rx == px && ry == py);  // record x,y (pivot or sprite offset)
            expect(static_cast<int>(le32(elf, a + 8)) == d.u && static_cast<int>(le32(elf, a + 12)) == d.v);
            expect(static_cast<int>(le32(elf, a + 16)) == d.w && static_cast<int>(le32(elf, a + 20)) == d.h);
        };
        tmpl(0x327820, q[0], 70, 70);
        for (int k = 0; k < 8; ++k) expect(f32at(0x327820 + 0x20 + 4 * (k / 2) * 4 + 4 * (k % 2)) == q[0].verts[static_cast<std::size_t>(k)]);
        // town plate (0x327890), transmission icon (0x3278B0), hub cap (0x3278D0).
        tmpl(0x327890, q[5], 20, 100);
        tmpl(0x3278B0, q[12], 29, 59);
        tmpl(0x3278D0, q[q.size() - 1], 64, 64);
    }
    // Code hashes (FNV-1a 64) of the builders this module transcribes; detect ROM drift.
    expect(fnvRange(elf, 0x2496C8, 0x249FA0) == 0xb3919e7c8b8084cfull);   // handler 0 (no decomp file)
    expect(fnvRange(elf, 0x24E1A0, 0x24E7D8) == 0x9589230dc3a80010ull);   // drive HUD
    expect(fnvRange(elf, 0x22F750, 0x22FE4C) == 0xea031ba988be6a10ull);   // ElementC
    expect(fnvRange(elf, 0x22FE50, 0x230520) == 0x9920137b002b2c07ull);   // ElementD
    expect(fnvRange(elf, 0x249458, 0x2494E8) == 0xe33d8cf2f40f4fd7ull);   // speedometer value
    expect(fnvRange(elf, 0x2494E8, 0x24951C) == 0xc6f3ca412f611b0aull);   // speedometer digits
    expect(fnvRange(elf, 0x24F4A0, 0x24F4E0) == 0xa892943c927d6b6aull);   // IntToPackedBCD
}

// ------------------------------------------------------------ ROI vs oracle

struct Ppm { int w = 0, h = 0; std::vector<std::uint8_t> rgb; };

bool readPpm(const fs::path& p, Ppm& out) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return false;
    std::string magic;
    int maxv = 0;
    in >> magic >> out.w >> out.h >> maxv;
    if (magic != "P6" || maxv != 255) return false;
    in.get();
    out.rgb.resize(static_cast<std::size_t>(out.w) * out.h * 3);
    in.read(reinterpret_cast<char*>(out.rgb.data()), static_cast<std::streamsize>(out.rgb.size()));
    return static_cast<std::size_t>(in.gcount()) == out.rgb.size();
}

bool loadArt(const fs::path& path, hl::GaugeArt& out) {
    const auto bytes = readFile(path);
    return !bytes.empty() && hl::decodeGaugeArt(bytes, out);
}

constexpr int kRoiX0 = 1056, kRoiY0 = 380, kRoiX1 = 1188, kRoiY1 = 415;

double histIntersection(const std::vector<std::uint8_t>& a, const std::vector<std::uint8_t>& b) {
    double total = 0;
    const std::size_t n = a.size() / 3;
    for (int c = 0; c < 3; ++c) {
        std::array<int, 32> ha{}, hb{};
        for (std::size_t i = 0; i < n; ++i) { ++ha[a[i * 3 + c] / 8]; ++hb[b[i * 3 + c] / 8]; }
        int s = 0;
        for (int k = 0; k < 32; ++k) s += std::min(ha[k], hb[k]);
        total += static_cast<double>(s) / static_cast<double>(n);
    }
    return total / 3.0;
}

// Returns false when inputs are missing.
bool runRoi(Results& r, const fs::path& disc, const fs::path& crops) {
    hl::GaugeArt art0;
    if (!loadArt(disc / "ITEM/METER0.GSL", art0)) {
        std::fprintf(stderr, "NO_DISC: cannot decode ITEM/METER0.GSL under %s\n", disc.c_str());
        return false;
    }
    const hl::GaugeArt* pages[3] = {nullptr, &art0, nullptr};
    // rpm per frame comes from the measured needle (implied rpm), the gear glyph from the box read
    // ("7" race_hud, "4" race_hud_driving; town frames show "4"); neither is inside the ROI except
    // the needle shaft, so ROI MAD is dominated by ring + digits.
    struct F { const char* name; int mph; int rpm; int gear; bool town; bool thresholded; };
    for (const F& f : {F{"race_hud", 39, 128, 7, false, true}, F{"race_hud_driving", 63, 6178, 4, false, true},
                       F{"town_driving_street", 0, 39, 4, true, false}}) {
        Ppm oracle;
        if (!readPpm(crops / (std::string(f.name) + "_roi.ppm"), oracle) ||
            oracle.w != kRoiX1 - kRoiX0 || oracle.h != kRoiY1 - kRoiY0) {
            std::fprintf(stderr, "NO_ORACLE: missing/invalid %s_roi.ppm in %s (run hud_oracle_diff.py export)\n",
                         f.name, crops.c_str());
            return false;
        }
        hl::GaugeInput in;
        in.speedDivisor = 1.f;
        in.speedRaw = static_cast<float>(f.mph) + 0.25f;
        in.rpm = f.rpm;
        in.gear = f.gear;
        in.townMode = f.town;
        in.clockTicks = 83250;
        in.meterField = 0x40000;  // full boost meter: best arc match vs oracle (all frames)
        hl::Canvas canvas;
        hl::rasterize(hl::buildGaugeHud(in), pages, canvas);
        const std::vector<std::uint8_t> shot = hl::toShotSpace(canvas);
        std::vector<std::uint8_t> ours(oracle.rgb.size());
        std::size_t o = 0;
        double sad = 0;
        for (int sy = kRoiY0; sy < kRoiY1; ++sy) {
            for (int sx = kRoiX0; sx < kRoiX1; ++sx) {
                for (int c = 0; c < 3; ++c) {
                    ours[o] = shot[(static_cast<std::size_t>(sy) * hl::kShotWidth + sx) * 3 + c];
                    sad += std::abs(static_cast<int>(ours[o]) - static_cast<int>(oracle.rgb[o]));
                    ++o;
                }
            }
        }
        const double mad = sad / static_cast<double>(o);
        const double hist = histIntersection(ours, oracle.rgb);
        char b[200];
        std::snprintf(b, sizeof b, "roi %-20s mph=%2d mad=%.3f hist=%.4f%s", f.name, f.mph, mad, hist,
                      f.thresholded ? "" : " (informational)");
        r.info.push_back(b);
        if (f.thresholded) {
            ++r.roiFrames;
            r.roiMadMax = std::max(r.roiMadMax, mad);
            r.roiHistMin = std::min(r.roiHistMin, hist);
        }
        for (std::size_t i = 0; i < ours.size(); ++i) r.hash = (r.hash ^ ours[i]) * 1099511628211ull;
    }
    return true;
}

fs::path defaultElf() {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    if (const char* h = std::getenv("HOME"))
        return fs::path(h) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    return {};
}

}  // namespace

int main(int argc, char** argv) {
    fs::path elfPath = defaultElf();
    fs::path disc;
    if (const char* h = std::getenv("HOME")) disc = fs::path(h) / "Library/Application Support/RoadTripRecomp/disc";
    fs::path crops = "build/w-W2-10/oracle_crops";
    bool allowSkip = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&]() -> std::string { return i + 1 < argc ? argv[++i] : ""; };
        if (a == "--elf") elfPath = next();
        else if (a == "--disc") disc = next();
        else if (a == "--oracle-crops") crops = next();
        else if (a == "--allow-skip") allowSkip = true;
        else { std::fprintf(stderr, "unknown arg %s\n", a.c_str()); return 2; }
    }
    const auto elf = readFile(elfPath);
    if (elf.size() < 0x136000) {
        std::fprintf(stderr, "NO_ELF: cannot read %s\n", elfPath.c_str());
        return allowSkip ? 0 : 2;
    }

    Results pass[2];
    for (int run = 0; run < 2; ++run) {
        Results& r = pass[run];
        runRects(r);
        runTimer(r);
        runSpeed(r);
        runElf(r, elf);
        if (!runRoi(r, disc, crops)) return allowSkip ? 0 : 2;
        Hasher h;
        h.u64(static_cast<std::uint64_t>(r.rectErr)); h.u64(r.rectQuads);
        h.u64(r.timerPass); h.u64(r.timerOraclePass); h.u64(r.timerOracleAltPass);
        h.u64(r.speedOraclePass); h.u64(r.elfPass);
        std::uint64_t madBits, histBits;
        std::memcpy(&madBits, &r.roiMadMax, 8);
        std::memcpy(&histBits, &r.roiHistMin, 8);
        h.u64(madBits); h.u64(histBits);
        for (const auto& s : r.info) h.str(s);
        h.u64(r.hash);
        r.hash = h.h;
    }
    const Results& r = pass[0];
    const bool det = pass[0].hash == pass[1].hash;

    for (const auto& s : r.info) std::printf("INFO %s\n", s.c_str());
    std::printf("INFO timer_oracle_alt(frames-0)=%d/%d (must be 0/2 for the frames-1 reading to be the discriminator)\n",
                r.timerOracleAltPass, r.timerOracleTotal);
    std::printf("INFO elf_pins=%d/%d rect_quads_checked=%d\n", r.elfPass, r.elfTotal, r.rectQuads);
    std::printf("INFO needle law (ELF): theta = pi*(min(rpm,10000)/10000 - 0.7), rpm = car+0x1D0 via 0x249DD0; "
                "pivot UI (554,362) = shot (1108,361.2); measured hub (%.2f,%.2f)\n", hl::kNeedleHubShotX,
                hl::kNeedleHubShotY);
    double restResid = 0;
    for (const auto& p : hl::kNeedleOraclePoints) {
        const double theta = hl::shotAngleToNeedleThetaDeg(p.angleDeg);
        const double rpm = hl::impliedRpm(theta);
        std::printf("INFO needle_point frame=%-20s mph=%2d shot_angle=%.2f theta_ui=%.2f implied_rpm=%.0f mask_px=%d\n",
                    p.frame, p.displayedMph, p.angleDeg, theta, rpm, p.maskPixels);
    }
    {
        // Only RPM-anchored check available: a stationary car (town, speed 0) is at rpm ~0 => -126 deg.
        const double theta0 = hl::needleAngleRad(0) * 180.0 / 3.14159265358979323846;
        restResid = std::fabs(hl::shotAngleToNeedleThetaDeg(hl::kNeedleOraclePoints[2].angleDeg) - theta0);
        std::printf("INFO needle_rest_resid_deg=%.2f (town_driving, speed 0 => rpm 0 assumed; 1 point, NOT a fit)\n",
                    restResid);
    }
    std::printf("INFO needle: 4 measured oracle points (3 independent), RPM unknown at every point -> "
                "needle_resid_deg_max DEFERRED (need >=4 independent RPM-known points)\n");
    const bool ok = r.rectErr == 0 && r.timerPass == r.timerTotal && r.timerOraclePass == r.timerOracleTotal &&
                    r.timerOracleAltPass == 0 && r.speedOraclePass == r.speedOracleTotal &&
                    r.elfPass == r.elfTotal && r.roiMadMax <= 20.0 && r.roiHistMin >= 0.85 && det;
    std::printf("W2-10 SCORE rect_err_px=%ld timer_vectors=%d/%d timer_oracle=%d/%d speed_oracle=%d/%d "
                "elf_pins=%d/%d needle_points=4 needle_independent=3 needle_resid_deg_max=DEFERRED "
                "needle_rest_resid_deg=%.2f roi_mad=%.3f roi_hist=%.4f deterministic=%d %s\n",
                r.rectErr, r.timerPass, r.timerTotal, r.timerOraclePass, r.timerOracleTotal, r.speedOraclePass,
                r.speedOracleTotal, r.elfPass, r.elfTotal, restResid, r.roiMadMax, r.roiHistMin, det ? 1 : 0,
                ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
