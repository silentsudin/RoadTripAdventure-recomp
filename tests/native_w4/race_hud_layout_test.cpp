// W4-06 race HUD layout contract (w4_race_hud_layout_test).
//
// Run from the repo root:  build/<dir>/native_w4/w4_race_hud_layout_test
//
// Three tiers:
//   1. Numeric layout checks on gameplay/race_hud_layout.h, the hud_live parity and the
//      preview/race_hud.h sprite builder. These need no game data.
//   2. Fixture drift: tests/native_w4/data/race_hud_fixture.json against the header table.
//   3. Optional re-measurement. The oracle goldens (build/scratch/oracle-golden/*.png,
//      needs zlib) and the ELF (RT_ELF or build/rom/SLUS_203.98) are each re-read when
//      present. A missing input prints SKIP and does not fail the run.
// Prints one `W4-06 SCORE ...` line; exit 0 only when no check failed.

#include "gameplay/hud_live.h"
#include "gameplay/race_hud_layout.h"
#include "preview/race_hud.h"

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <vector>

#if defined(RT_W4_HAVE_ZLIB)
#include <zlib.h>
#endif

namespace L = roadtrip::gameplay;
namespace P = rt::native::preview;
namespace hl = rt::native::gameplay::hud_live;
namespace fs = std::filesystem;

namespace {

int g_checks = 0;
int g_failed = 0;

void check(bool ok, const std::string& what) {
    ++g_checks;
    if (!ok) {
        ++g_failed;
        std::printf("FAIL %s\n", what.c_str());
    }
}

std::string fmt(const char* f, ...) __attribute__((format(printf, 1, 2)));
std::string fmt(const char* f, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, f);
    std::vsnprintf(buf, sizeof(buf), f, ap);
    va_end(ap);
    return buf;
}

std::vector<std::uint8_t> readFile(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

// ------------------------------------------------------------------ minimal JSON

struct Json {
    enum Kind { Null, Bool, Num, Str, Arr, Obj } kind = Null;
    double num = 0;
    bool b = false;
    std::string str;
    std::vector<Json> arr;
    std::vector<std::pair<std::string, Json>> obj;

    const Json* get(const std::string& k) const {
        for (const auto& kv : obj)
            if (kv.first == k) return &kv.second;
        return nullptr;
    }
    const Json& operator[](const std::string& k) const {
        static const Json kNull;
        const Json* j = get(k);
        return j ? *j : kNull;
    }
    const Json& operator[](std::size_t i) const {
        static const Json kNull;
        return i < arr.size() ? arr[i] : kNull;
    }
    int i() const { return static_cast<int>(std::lround(num)); }
};

struct JsonParser {
    const std::string& s;
    std::size_t p = 0;
    bool ok = true;
    explicit JsonParser(const std::string& src) : s(src) {}
    void ws() {
        while (p < s.size() && (s[p] == ' ' || s[p] == '\n' || s[p] == '\r' || s[p] == '\t')) ++p;
    }
    Json value() {
        ws();
        Json j;
        if (p >= s.size()) { ok = false; return j; }
        const char c = s[p];
        if (c == '{') {
            j.kind = Json::Obj;
            ++p;
            ws();
            if (p < s.size() && s[p] == '}') { ++p; return j; }
            while (ok) {
                ws();
                Json k = value();
                ws();
                if (p >= s.size() || s[p] != ':') { ok = false; break; }
                ++p;
                Json v = value();
                j.obj.emplace_back(k.str, std::move(v));
                ws();
                if (p < s.size() && s[p] == ',') { ++p; continue; }
                if (p < s.size() && s[p] == '}') { ++p; break; }
                ok = false;
            }
        } else if (c == '[') {
            j.kind = Json::Arr;
            ++p;
            ws();
            if (p < s.size() && s[p] == ']') { ++p; return j; }
            while (ok) {
                j.arr.push_back(value());
                ws();
                if (p < s.size() && s[p] == ',') { ++p; continue; }
                if (p < s.size() && s[p] == ']') { ++p; break; }
                ok = false;
            }
        } else if (c == '"') {
            j.kind = Json::Str;
            ++p;
            while (p < s.size() && s[p] != '"') {
                if (s[p] == '\\' && p + 1 < s.size()) {
                    ++p;
                    const char e = s[p];
                    j.str += (e == 'n') ? '\n' : (e == 't') ? '\t' : e;
                } else {
                    j.str += s[p];
                }
                ++p;
            }
            ++p;
        } else if (s.compare(p, 4, "null") == 0) {
            p += 4;
        } else if (s.compare(p, 4, "true") == 0) {
            j.kind = Json::Bool; j.b = true; p += 4;
        } else if (s.compare(p, 5, "false") == 0) {
            j.kind = Json::Bool; p += 5;
        } else {
            j.kind = Json::Num;
            char* end = nullptr;
            j.num = std::strtod(s.c_str() + p, &end);
            if (end == s.c_str() + p) ok = false;
            p = static_cast<std::size_t>(end - s.c_str());
        }
        return j;
    }
};

// ------------------------------------------------------------------ helpers

fs::path firstExisting(const std::vector<fs::path>& candidates) {
    for (const auto& c : candidates)
        if (!c.empty() && fs::exists(c)) return c;
    return {};
}

fs::path fixturePath() {
    std::vector<fs::path> c{"tests/native_w4/data/race_hud_fixture.json"};
#if defined(RT_W4_DATA_DIR)
    c.emplace_back(fs::path(RT_W4_DATA_DIR) / "race_hud_fixture.json");
#endif
    return firstExisting(c);
}

// An explicitly set RT_ORACLE_GOLDEN_DIR / RT_ELF is the only candidate (so a
// bogus value exercises the SKIP path); otherwise repo-relative, then the
// configure-time repo root.
fs::path goldenDir() {
    std::vector<fs::path> c;
    if (const char* e = std::getenv("RT_ORACLE_GOLDEN_DIR")) c.emplace_back(e);
    else c.emplace_back("build/scratch/oracle-golden");
#if defined(RT_W4_REPO_DIR)
    if (!std::getenv("RT_ORACLE_GOLDEN_DIR")) c.emplace_back(fs::path(RT_W4_REPO_DIR) / "build/scratch/oracle-golden");
#endif
    for (const auto& d : c)
        if (fs::exists(d / "race_hud.png") && fs::exists(d / "race_hud_driving.png")) return d;
    return {};
}

fs::path elfPath() {
    std::vector<fs::path> c;
    if (const char* e = std::getenv("RT_ELF")) return firstExisting({fs::path(e)});
    c.emplace_back("build/rom/SLUS_203.98");
#if defined(RT_W4_REPO_DIR)
    c.emplace_back(fs::path(RT_W4_REPO_DIR) / "build/rom/SLUS_203.98");
#endif
    return firstExisting(c);
}

bool within(double a, double b, double tol) { return std::fabs(a - b) <= tol + 1e-9; }

std::string timerString(std::uint32_t bcd) {
    // ElementC digit order: [min10][min1]'[sec10][sec1]"[tenth][hundredth]
    auto n = [&](int k) { return static_cast<char>('0' + ((bcd >> (4 * k)) & 0xF)); };
    std::string s;
    s += n(5); s += n(4); s += '\''; s += n(3); s += n(2); s += '"'; s += n(1); s += n(0);
    return s;
}

const L::RaceHudElement* findElement(const std::string& id) {
    for (const auto& e : L::kRaceHudElements)
        if (id == e.id) return &e;
    return nullptr;
}

// ------------------------------------------------------------------ tier 1: numeric

void numericChecks() {
    const float tol = L::kRaceHudTolerancePx;
    // Coordinate law.
    check(within(L::raceHudDesignX(1280), 640, 0) && within(L::raceHudDesignY(447), 448, 1e-9),
          "coordinate law maps the 1280x447 golden onto 640x448");

    // Element boxes: in bounds, well formed, measured vs ROM within tolerance.
    for (const auto& e : L::kRaceHudElements) {
        const auto& m = e.measured;
        check(m.x0 >= 0 && m.y0 >= 0 && m.x1 <= L::kRaceHudDesignWidth && m.y1 <= L::kRaceHudDesignHeight &&
                  m.x0 < m.x1 && m.y0 < m.y1,
              fmt("%s measured box in 640x448 and non-empty", e.id));
        if (e.check == L::RaceHudCheck::None) continue;
        const auto& r = e.rom;
        check(r.x0 >= 0 && r.y0 >= 0 && r.x1 <= L::kRaceHudDesignWidth && r.y1 <= L::kRaceHudDesignHeight &&
                  r.x0 < r.x1 && r.y0 < r.y1,
              fmt("%s ROM box in 640x448 and non-empty", e.id));
        if (e.check == L::RaceHudCheck::Ink) {
            check(within(m.x0, r.x0, tol) && within(m.y0, r.y0, tol) && within(m.x1, r.x1, tol) &&
                      within(m.y1, r.y1, tol),
                  fmt("%s measured (%.2f,%.2f,%.2f,%.2f) matches predicted ink (%d,%d,%d,%d) within %.1f px", e.id,
                      m.x0, m.y0, m.x1, m.y1, r.x0, r.y0, r.x1, r.y1, tol));
        } else {
            check(m.x0 >= r.x0 - tol && m.y0 >= r.y0 - tol && m.x1 <= r.x1 + tol && m.y1 <= r.y1 + tol,
                  fmt("%s measured fill inside ROM cell box", e.id));
        }
        // PNG boxes convert to the stored design union.
        double x0 = 1e9, y0 = 1e9, x1 = -1, y1 = -1;
        for (const auto* b : {&e.pngDriving, &e.pngStart}) {
            if (b->x1 == 0) continue;
            x0 = std::min<double>(x0, b->x0); x1 = std::max<double>(x1, b->x1);
            y0 = std::min<double>(y0, b->y0); y1 = std::max<double>(y1, b->y1);
        }
        check(within(L::raceHudDesignX(x0), m.x0, 0.01) && within(L::raceHudDesignY(y0), m.y0, 0.01) &&
                  within(L::raceHudDesignX(x1), m.x1, 0.01) && within(L::raceHudDesignY(y1), m.y1, 0.01),
              fmt("%s design union = converted PNG boxes", e.id));
    }

    // Timer layout: monotonic, 14/28 pitch, separators between groups, 16x16 cells.
    const auto& dx = L::kRaceHudTimerDigitDx;
    bool mono = true;
    for (std::size_t i = 1; i < dx.size(); ++i) mono = mono && dx[i] > dx[i - 1];
    check(mono, "timer digit slots strictly increase left to right");
    check(dx[1] - dx[0] == L::kRaceHudTimerPairPitch && dx[3] - dx[2] == 14 && dx[5] - dx[4] == 14,
          "timer pair pitch 14");
    check(dx[2] - dx[1] == L::kRaceHudTimerGroupPitch && dx[4] - dx[3] == 28, "timer group pitch 28");
    check(dx[1] < L::kRaceHudTimerSepMinutes.x && L::kRaceHudTimerSepMinutes.x < dx[2] &&
              dx[3] < L::kRaceHudTimerSepSeconds.x && L::kRaceHudTimerSepSeconds.x < dx[4],
          "separators sit between digit groups");
    check(L::kRaceHudTimerCell == 16 && L::kRaceHudTimerDigitV == 64, "timer cell 16x16 on the v=64 row");
    check(dx[5] + L::kRaceHudTimerCell <= L::kRaceHudPanelWidth, "timer digits fit in the 132 px panel");
    // The timer pitch is neither FONTE nor METER: this is the page attribution.
    check(L::kRaceHudTimerPairPitch != L::kRaceHudFonteDigitAdvance &&
              L::kRaceHudTimerPairPitch != -L::kRaceHudMphStep,
          "timer pitch 14 differs from FONTE digit advance 12 and METER step 17");
    check(L::kRaceHudFonteSpaceAdvance == 8 && L::kRaceHudFontePeriodAdvance == 12 &&
              L::kRaceHudFonteUpperAAdvance == 15 && L::kRaceHudFonteLowerAAdvance == 14,
          "FONTE advance excerpt (0x29A880)");

    // TOTAL and LAP blocks share the layout, shifted by the origin delta.
    const int ddx = L::kRaceHudLapPanelOrigin.x - L::kRaceHudTotalPanelOrigin.x;
    check(L::kRaceHudLapPanelOrigin.y == L::kRaceHudTotalPanelOrigin.y && ddx == 434, "LAP panel = TOTAL panel + (434,0)");
    const auto* tp = findElement("total_panel");
    const auto* lp = findElement("lap_panel");
    check(tp && lp && lp->rom.x0 - tp->rom.x0 == ddx && lp->rom.x1 - tp->rom.x1 == ddx,
          "panel ink boxes follow the origin shift");
    check(tp && within(tp->measured.x0, L::kRaceHudTotalPanelOrigin.x, 0.01), "TOTAL panel left edge = ElementC x 60");
    check(lp && within(lp->measured.x0, L::kRaceHudLapPanelOrigin.x, 0.01), "LAP panel left edge = ElementC x 494");

    // Banners: medallion abuts the word; word row = banner row + 12.
    for (const auto* b : {&L::kRaceHudTotalBanner, &L::kRaceHudLapBanner}) {
        check((*b)[0].x + (*b)[0].w == (*b)[1].x && (*b)[1].y == (*b)[0].y + 12, "banner medallion|word records abut");
    }
    check(L::kRaceHudTotalBanner[0].x == L::kRaceHudTotalBannerOrigin.x &&
              L::kRaceHudLapBanner[0].x == L::kRaceHudLapBannerOrigin.x,
          "banner records at group origins");

    // Rank badge geometry: ones right of tens, suffix/oval right of the digits.
    check(L::kRaceHudRankDigitTensDx < L::kRaceHudRankDigitOnesDx &&
              L::kRaceHudRankDigitOnesDx + L::kRaceHudRankDigitW <= L::kRaceHudRankPanel.x + 1 &&
              L::kRaceHudRankTotalTens.x < L::kRaceHudRankTotalOnes.x,
          "rank badge ordering");
    const int ord[] = {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 1, 2, 3};
    bool ordOk = true;
    for (int r = 0; r < 24; ++r) ordOk = ordOk && L::raceHudOrdinalIndex(r) == ord[r];
    check(ordOk, "ordinal suffix law st/nd/rd/th for ranks 1..24");

    // mph parity with the coordinator law (hud_live.h) and the badge.
    check(L::kRaceHudMphOnes.x == hl::kSpeedOnesX && L::kRaceHudMphOnes.y == hl::kSpeedOnesY,
          "mph ones anchor (573,388) parity");
    check(L::kRaceHudMphStep == hl::kSpeedDigitStep && L::kRaceHudMphStep == -17, "mph step -17 parity");
    check(L::kRaceHudMphCellW == hl::kSpeedDigitW && L::kRaceHudMphCellH == hl::kSpeedDigitH &&
              L::kRaceHudMphCellW == 16 && L::kRaceHudMphCellH == 23,
          "mph cell 16x23 parity");
    check(L::kRaceHudMphU0 == hl::kSpeedAtlasU0 && L::kRaceHudMphCols == hl::kSpeedAtlasCols &&
              L::kRaceHudMphVStep == hl::kSpeedAtlasVStep,
          "mph atlas u0 140 / 7 cols / v step 23 parity");
    check(L::kRaceHudMphBadge.x0 == hl::kSpeedBadgeX && L::kRaceHudMphBadge.y0 == hl::kSpeedBadgeY &&
              L::kRaceHudMphBadge.x1 == 604 && L::kRaceHudMphBadge.y1 == 416,
          "mph badge 556..604 x 388..416 parity");
    check(L::kRaceHudMphBadge.x1 - L::kRaceHudMphBadge.x0 == hl::kSpeedBadgeW &&
              L::kRaceHudMphBadge.y1 - L::kRaceHudMphBadge.y0 == hl::kSpeedBadgeH,
          "mph badge size parity");
    for (int d = 0; d < 10; ++d) {
        const float u0 = static_cast<float>(140 + 16 * (d % 7)) / 256.f;
        const float v0 = static_cast<float>(23 * (d / 7)) / 128.f;
        check(u0 >= 0.f && u0 + 16.f / 256.f <= 1.f && v0 >= 0.f && v0 + 23.f / 128.f <= 1.f,
              fmt("mph texel cell %d inside METER0", d));
    }
    const auto* md = findElement("mph_digits_fill");
    check(md && md->measured.x0 >= L::kRaceHudMphBadge.x0 - 0.01f && md->measured.x1 <= L::kRaceHudMphBadge.x1 &&
              md->measured.y0 >= L::kRaceHudMphBadge.y0 - tol && md->measured.y1 <= L::kRaceHudMphBadge.y1,
          "measured mph digits inside the mph badge");
    check(L::kRaceHudMphOnes.x + 2 * L::kRaceHudMphStep == 539, "3-digit mph leftmost cell at 539");

    // Gauge nesting.
    const auto inside = [](L::RaceHudRect a, L::RaceHudRect b) {
        return a.x0 >= b.x0 && a.y0 >= b.y0 && a.x1 <= b.x1 && a.y1 <= b.y1;
    };
    const L::RaceHudRect dial{L::kRaceHudDialFace.x, L::kRaceHudDialFace.y, L::kRaceHudDialFace.x + L::kRaceHudDialFace.w,
                              L::kRaceHudDialFace.y + L::kRaceHudDialFace.h};
    check(dial.x0 == L::kRaceHudGaugeOrigin.x && dial.y0 == L::kRaceHudGaugeOrigin.y && dial.x1 == 624 && dial.y1 == 422,
          "dial face = 1P origin (484,294) + 140x128");
    check(inside(L::kRaceHudMphBadgePlate, dial) && inside(L::kRaceHudMphLabel, dial) &&
              inside(L::kRaceHudGearPlate, dial),
          "baked plates inside the dial face");
    const auto g4 = L::raceHudGearSprite(4);
    const L::RaceHudRect gearCell{g4.x, g4.y, g4.x + g4.w, g4.y + g4.h};
    check(inside(gearCell, L::kRaceHudGearPlate), "gear glyph cell inside the gear plate");
    check(g4.u == 220 && g4.v == 80 && L::raceHudGearSprite(1).u == 160 && L::raceHudGearSprite(5).v == 96 &&
              L::raceHudGearSprite(0).u == 140,
          "gear glyph texel law u=(g%5)*20+140, v=(g/5)*16+80");
    const L::RaceHudRect mphCells{L::kRaceHudMphOnes.x + 2 * L::kRaceHudMphStep, L::kRaceHudMphOnes.y,
                                  L::kRaceHudMphOnes.x + L::kRaceHudMphCellW, L::kRaceHudMphOnes.y + L::kRaceHudMphCellH};
    check(inside(mphCells, L::kRaceHudMphBadgePlate), "3-digit mph cells inside the baked badge plate");

    // Needle law resolved (W4-11 tach law, coordinator ROM-verified); drawing
    // still gated on a native rpm feed (kRaceHudNeedleLawResolved = wired? no).
    check(L::kRaceHudNeedleLawResolved, "needle law flagged resolved (W4-11)");
    check(L::kRaceHudNeedlePivot.x == 554 && L::kRaceHudNeedlePivot.y == 362 &&
              within(L::kRaceHudNeedleMeasuredPivotX, 554, 1.0) && within(L::kRaceHudNeedleMeasuredPivotY, 362, 1.0),
          "measured needle pivot within 1 px of the ROM pivot");
    check(L::kRaceHudNeedleTipLenDriving <= L::kRaceHudNeedleStripLength &&
              L::kRaceHudNeedleTipLenStart <= L::kRaceHudNeedleStripLength,
          "visible needle tip shorter than the 59 px strip");

    // Translucency: only capsule pieces and the lap disc carry the half-alpha flag.
    check(L::kRaceHudPanelCapLeft.translucent != 0 && L::kRaceHudPanelCapRight.translucent != 0 &&
              L::kRaceHudPanelMiddle.translucent != 0 && L::kRaceHudLapDisc.translucent != 0 &&
              L::kRaceHudTotalBanner[0].translucent == 0 && L::kRaceHudRankPanel.translucent == 0,
          "translucent flags on capsule/disc only");
    check(L::kRaceHudCapsuleAlpha > 0.45f && L::kRaceHudCapsuleAlpha < 0.55f, "capsule alpha ~0.5");
}

// ------------------------------------------------------------------ tier 1b: builder + timer law

void builderChecks() {
    // Timer law vs the golden readings (manifest frames; lap frames inferred, see fixture).
    check(timerString(hl::raceTimerBcd(hl::raceHudTimerValue(3178))) == "00'52\"95", "total timer 3178 -> 00'52\"95");
    check(timerString(hl::raceTimerBcd(hl::raceHudTimerValue(2447))) == "00'40\"76", "total timer 2447 -> 00'40\"76");
    check(timerString(hl::raceTimerBcd(814)) == "00'13\"56", "lap timer 814 -> 00'13\"56");
    check(timerString(hl::raceTimerBcd(83)) == "00'01\"38", "lap timer 83 -> 00'01\"38");
    check(hl::raceHudTimerValue(3178) - 814 == hl::raceHudTimerValue(2447) - 83, "lap offset constant across goldens");

    P::RaceHudLayoutState s;
    s.totalTime = hl::raceHudTimerValue(3178);
    s.lapTime = 814;
    s.lap = 1;
    s.rank = 23;
    s.totalCars = 24;
    s.gear = 4;
    const auto sprites = P::raceHudLayoutSprites(s);
    // 2 banner + (14 panel + 2 sep + 6 digits) + 2 banner + (22 + disc + digit) + rank (1+1+2+2) + dial + gear
    check(sprites.size() == 2 + 22 + 2 + 24 + 6 + 2, fmt("sprite count %zu == 58", sprites.size()));
    bool noSub = true, inBounds = true;
    for (const auto& sp : sprites) {
        noSub = noSub && sp.page != L::RaceHudPage::Meter0Sub;
        const auto ps = L::raceHudPageSize(sp.page);
        inBounds = inBounds && sp.u >= 0 && sp.v >= 0 && sp.u + sp.w <= ps.width && sp.v + sp.h <= ps.height &&
                   sp.x >= -32 && sp.x + sp.w <= L::kRaceHudDesignWidth && sp.y >= 0 &&
                   sp.y + sp.h <= L::kRaceHudDesignHeight;
    }
    check(noSub, "no needle/hub/arc (Meter0Sub) sprite is emitted");
    check(inBounds, "every sprite texel rect inside its page and dest inside 640x448");

    // Total digits: "00'52"95" at x 64,78,106,120,148,162 y 56.
    const int want[6] = {0, 0, 5, 2, 9, 5};
    const int wantX[6] = {64, 78, 106, 120, 148, 162};
    int found = 0;
    for (const auto& sp : sprites) {
        if (sp.page != L::RaceHudPage::Score0 || sp.v != 64 || sp.w != 16 || sp.y != 56 || sp.x > 200) continue;
        for (int i = 0; i < 6; ++i)
            if (sp.x == wantX[i] && sp.u == want[i] * 16 && sp.csa == 1) ++found;
    }
    check(found == 6, "TOTAL timer digit sprites match ElementC slots and BCD");

    const auto has = [&](L::RaceHudPage pg, int u, int v, int w, int h, int x, int y) {
        for (const auto& sp : sprites)
            if (sp.page == pg && sp.u == u && sp.v == v && sp.w == w && sp.h == h && sp.x == x && sp.y == y) return true;
        return false;
    };
    check(has(L::RaceHudPage::Score1, 232, 168, 24, 24, 468, 52), "lap disc at (468,52)");
    check(has(L::RaceHudPage::Score1, 8, 200, 8, 16, 476, 57), "lap digit '1' at (476,57)");
    check(has(L::RaceHudPage::Score0, 56, 0, 28, 30, 40, 88) && has(L::RaceHudPage::Score0, 112, 0, 28, 30, 63, 88),
          "rank '24' big digits at (40,88)/(63,88)");
    check(has(L::RaceHudPage::Score0, 98, 31, 22, 16, 91, 88), "rank suffix 'th' at (91,88)");
    check(has(L::RaceHudPage::Score0, 160, 105, 32, 32, 91, 88), "rank oval at (91,88)");
    check(has(L::RaceHudPage::Score0, 56, 48, 12, 16, 101, 104) && has(L::RaceHudPage::Score0, 80, 48, 12, 16, 111, 104),
          "rank total '24' at (101,104)/(111,104)");
    check(has(L::RaceHudPage::Meter0, 0, 0, 140, 128, 484, 294), "dial face at (484,294)");
    check(has(L::RaceHudPage::Meter0, 220, 80, 20, 14, 581, 354), "gear '4' glyph at (581,354)");
    check(has(L::RaceHudPage::Score0, 0, 112, 32, 30, 20, 20) && has(L::RaceHudPage::Score0, 32, 159, 98, 17, 522, 32),
          "TOTALTIME / LAPTIME banner records");

    // Every predicted Ink element of the static parts is covered by an emitted sprite.
    const auto covered = [&](const char* id) {
        const auto* e = findElement(id);
        if (!e) return false;
        for (const auto& sp : sprites)
            if (sp.x <= e->rom.x0 && sp.y <= e->rom.y0 && sp.x + sp.w >= e->rom.x0 && sp.y + sp.h >= e->rom.y0) return true;
        return false;
    };
    for (const char* id : {"total_banner", "total_panel", "lap_banner", "lap_panel", "lap_badge_disc", "rank_oval",
                           "dial_face", "gear_plate"})
        check(covered(id), fmt("%s top-left covered by an emitted sprite", id));

    // Hidden blocks.
    P::RaceHudLayoutState h = s;
    h.showLapBlock = false;
    h.showRank = false;
    check(P::raceHudLayoutSprites(h).size() == 2 + 22 + 2, "lap block and rank can be hidden");
    P::RaceHudLayoutState z = s;
    z.totalTime = 0;
    bool dash = false;
    for (const auto& sp : P::raceHudLayoutSprites(z))
        if (sp.y == 56 && sp.x == 64 && sp.v == 64 && sp.u == 160) dash = true;
    check(dash, "time 0 draws BCD nibble 0xA (dimmed dash cell u=160)");

    // Pass split: identity viewport at 640x448 keeps design px; UVs normalised by page.
    const auto vp = P::raceHudStretchViewport(640, 448);
    const auto meter = P::raceHudLayoutQuads(s, P::RaceHudPass::Meter0, vp);
    check(meter.size() == 2 && within(meter[0].x, 484, 1e-4) && within(meter[0].y, 294, 1e-4) &&
              within(meter[0].u1, 140.f / 256.f, 1e-6) && within(meter[0].v1, 1.0, 1e-6),
          "Meter0 pass = dial face + gear, UV by 256x128");
    const auto csa1 = P::raceHudLayoutQuads(s, P::RaceHudPass::ScoreCsa1, vp);
    const auto csa0 = P::raceHudLayoutQuads(s, P::RaceHudPass::ScoreCsa0, vp);
    const auto pg1 = P::raceHudLayoutQuads(s, P::RaceHudPass::ScorePage1, vp);
    check(csa0.size() == 4 && pg1.size() == 2 && csa0.size() + csa1.size() + pg1.size() + meter.size() == sprites.size(),
          fmt("pass split csa0=%zu csa1=%zu page1=%zu meter=%zu covers every sprite", csa0.size(), csa1.size(),
              pg1.size(), meter.size()));
    int halfAlpha = 0;
    for (const auto& q : csa1) halfAlpha += q.color[3] < 0.99f ? 1 : 0;
    check(halfAlpha == 28, fmt("28 translucent capsule quads in the CSA1 pass (got %d)", halfAlpha));
    const auto vp2 = P::raceHudStretchViewport(1280, 896);
    const auto meter2 = P::raceHudLayoutQuads(s, P::RaceHudPass::Meter0, vp2);
    check(within(meter2[0].x, 968, 1e-3) && within(meter2[0].width, 280, 1e-3), "viewport scales design px");
    check(P::raceHudLapDiscFallback(s, vp).size() == 24 && P::raceHudLapDigitFallbackQuads(s, vp).size() == 1,
          "page-1 fallbacks emit disc scanlines + one digit");
}

// ------------------------------------------------------------------ tier 2: fixture drift

bool loadFixture(Json& out) {
    const fs::path p = fixturePath();
    if (p.empty()) {
        check(false, "fixture tests/native_w4/data/race_hud_fixture.json not found (run from the repo root)");
        return false;
    }
    const auto bytes = readFile(p);
    const std::string text(bytes.begin(), bytes.end());
    JsonParser parser(text);
    out = parser.value();
    check(parser.ok && out.kind == Json::Obj, "fixture parses as a JSON object");
    return parser.ok;
}

L::RaceHudPngBox pngBoxOf(const Json& j) {
    if (j.kind != Json::Arr) return {0, 0, 0, 0};
    return {j[0].i(), j[1].i(), j[2].i(), j[3].i()};
}

void fixtureChecks(const Json& fx) {
    const Json& els = fx["elements"];
    check(els.arr.size() == L::kRaceHudElements.size(),
          fmt("fixture element count %zu == header %zu", els.arr.size(), L::kRaceHudElements.size()));
    for (const auto& e : els.arr) {
        const auto* h = findElement(e["id"].str);
        check(h != nullptr, "fixture element " + e["id"].str + " present in header");
        if (!h) continue;
        const Json& rom = e["rom_ink_design"];
        if (rom.kind == Json::Arr)
            check(rom[0].i() == h->rom.x0 && rom[1].i() == h->rom.y0 && rom[2].i() == h->rom.x1 && rom[3].i() == h->rom.y1,
                  h->id + std::string(" ROM box drift"));
        const Json& des = e["measured_design_union"];
        check(within(des[0].num, h->measured.x0, 0.01) && within(des[1].num, h->measured.y0, 0.01) &&
                  within(des[2].num, h->measured.x1, 0.01) && within(des[3].num, h->measured.y1, 0.01),
              h->id + std::string(" measured design drift"));
        const auto d = pngBoxOf(e["measured_png"]["race_hud_driving"]);
        const auto s = pngBoxOf(e["measured_png"]["race_hud"]);
        check(std::memcmp(&d, &h->pngDriving, sizeof d) == 0 && std::memcmp(&s, &h->pngStart, sizeof s) == 0,
              h->id + std::string(" PNG box drift"));
        const std::string kind = e["check"].str;
        const bool kOk = (kind == "ink" && h->check == L::RaceHudCheck::Ink) ||
                         (kind == "contain" && h->check == L::RaceHudCheck::Contain) ||
                         (kind == "none" && h->check == L::RaceHudCheck::None);
        check(kOk, h->id + std::string(" check kind drift"));
    }
    const Json& cs = fx["color_samples"];
    check(cs.arr.size() == L::kRaceHudColorSamples.size(), "fixture colour sample count == header");
    for (std::size_t i = 0; i < cs.arr.size() && i < L::kRaceHudColorSamples.size(); ++i) {
        const auto& h = L::kRaceHudColorSamples[i];
        const Json& c = cs[i];
        const bool golden = (c["frame"].str == "race_hud_driving") == (h.golden == L::RaceHudGolden::Driving);
        check(golden && c["png"][0].i() == h.pngX && c["png"][1].i() == h.pngY && c["rgb"][0].i() == h.rgb.r &&
                  c["rgb"][1].i() == h.rgb.g && c["rgb"][2].i() == h.rgb.b,
              fmt("colour sample %zu (%s) drift", i, h.what));
    }
    // Timer cells: yellow fill inside the ROM cell (+tolerance), design pitch 14/28.
    for (const auto& c : fx["timer_cells"].arr) {
        const Json& cell = c["cell_design"];
        for (const char* fr : {"race_hud_driving", "race_hud"}) {
            const auto b = pngBoxOf(c["yellow_png"][fr]);
            if (b.x1 == 0) continue;
            check(L::raceHudDesignX(b.x0) >= cell[0].num - 0.01 && L::raceHudDesignX(b.x1) <= cell[2].num + 0.01 &&
                      L::raceHudDesignY(b.y0) >= cell[1].num - 0.01 && L::raceHudDesignY(b.y1) <= cell[3].num + 0.01,
                  fmt("%s %s fill inside its 16x16 cell on %s", c["block"].str.c_str(), c["cell"].str.c_str(), fr));
        }
    }
    const Json& pitch = fx["pitch"];
    check(pitch["timer_pair_png"].i() == 2 * L::kRaceHudTimerPairPitch && pitch["meter_mph_step"].i() == 17 &&
              pitch["fonte_digit_advance"].i() == L::kRaceHudFonteDigitAdvance,
          "fixture pitch block agrees with header");
    const Json& nd = fx["needle"];
    check(nd["status"].str == "GAP: needle law unresolved" && nd["rom_pivot"][0].i() == L::kRaceHudNeedlePivot.x &&
              within(nd["tip_angle_deg"]["race_hud_driving"].num, L::kRaceHudNeedleTipAngleDriving, 1e-6) &&
              within(nd["tip_angle_deg"]["race_hud"].num, L::kRaceHudNeedleTipAngleStart, 1e-6),
          "fixture needle block agrees with header (GAP)");
    // Golden readings vs the ElementC law.
    for (const char* fr : {"race_hud_driving", "race_hud"}) {
        const Json& f = fx["frames"][fr];
        const int total = f["manifest_race_total_time_frames"].i();
        check(timerString(hl::raceTimerBcd(hl::raceHudTimerValue(total))) == f["readings"]["total"].str,
              fmt("%s total reading via frames-1", fr));
        check(timerString(hl::raceTimerBcd(f["lap_frames_inferred"].i())) == f["readings"]["lap"].str,
              fmt("%s lap reading via inferred lap frames", fr));
        check(f["manifest_race_place_minus_1"].i() + 1 == f["readings"]["rank_place"].i(), fmt("%s place = manifest+1", fr));
    }
}

// ------------------------------------------------------------------ tier 3a: PNG re-measurement

struct Rgb8 {
    int w = 0, h = 0;
    std::vector<std::uint8_t> px;  // RGB
    const std::uint8_t* at(int x, int y) const { return &px[(static_cast<std::size_t>(y) * w + x) * 3]; }
};

#if defined(RT_W4_HAVE_ZLIB)
bool decodePng(const fs::path& path, Rgb8& out) {
    const auto f = readFile(path);
    if (f.size() < 33 || std::memcmp(f.data(), "\x89PNG\r\n\x1a\n", 8) != 0) return false;
    auto be32 = [&](std::size_t o) {
        return (std::uint32_t(f[o]) << 24) | (std::uint32_t(f[o + 1]) << 16) | (std::uint32_t(f[o + 2]) << 8) | f[o + 3];
    };
    std::size_t o = 8;
    int w = 0, h = 0, bpp = 0;
    std::vector<std::uint8_t> idat;
    while (o + 8 <= f.size()) {
        const std::uint32_t len = be32(o);
        const std::string type(reinterpret_cast<const char*>(&f[o + 4]), 4);
        const std::size_t d = o + 8;
        if (d + len > f.size()) return false;
        if (type == "IHDR") {
            w = static_cast<int>(be32(d));
            h = static_cast<int>(be32(d + 4));
            const int depth = f[d + 8], ct = f[d + 9], il = f[d + 12];
            if (depth != 8 || il != 0 || (ct != 2 && ct != 6)) return false;
            bpp = ct == 2 ? 3 : 4;
        } else if (type == "IDAT") {
            idat.insert(idat.end(), f.begin() + static_cast<long>(d), f.begin() + static_cast<long>(d + len));
        } else if (type == "IEND") {
            break;
        }
        o = d + len + 4;
    }
    if (w <= 0 || h <= 0 || bpp == 0) return false;
    const std::size_t stride = static_cast<std::size_t>(w) * bpp;
    std::vector<std::uint8_t> raw((stride + 1) * h);
    uLongf rawLen = static_cast<uLongf>(raw.size());
    if (uncompress(raw.data(), &rawLen, idat.data(), static_cast<uLong>(idat.size())) != Z_OK || rawLen != raw.size())
        return false;
    std::vector<std::uint8_t> img(stride * h);
    for (int y = 0; y < h; ++y) {
        const std::uint8_t ft = raw[y * (stride + 1)];
        const std::uint8_t* src = &raw[y * (stride + 1) + 1];
        std::uint8_t* dst = &img[y * stride];
        const std::uint8_t* up = y ? &img[(y - 1) * stride] : nullptr;
        for (std::size_t x = 0; x < stride; ++x) {
            const int a = x >= static_cast<std::size_t>(bpp) ? dst[x - bpp] : 0;
            const int b = up ? up[x] : 0;
            const int c = (up && x >= static_cast<std::size_t>(bpp)) ? up[x - bpp] : 0;
            int pred = 0;
            switch (ft) {
                case 0: pred = 0; break;
                case 1: pred = a; break;
                case 2: pred = b; break;
                case 3: pred = (a + b) / 2; break;
                case 4: {
                    const int p = a + b - c, pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
                    pred = (pa <= pb && pa <= pc) ? a : (pb <= pc ? b : c);
                    break;
                }
                default: return false;
            }
            dst[x] = static_cast<std::uint8_t>(src[x] + pred);
        }
    }
    out.w = w;
    out.h = h;
    out.px.resize(static_cast<std::size_t>(w) * h * 3);
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i)
        for (int c = 0; c < 3; ++c) out.px[i * 3 + c] = img[i * bpp + c];
    return true;
}
#endif

// Colour classes, identical to build/scratch/w4-06/gen_fixture2.py (HANDOFF-W4-06.md).
bool colourClass(const std::string& cls, const std::uint8_t* p) {
    const int r = p[0], g = p[1], b = p[2];
    const bool navy = r <= 40 && g <= 40 && b <= 80;
    if (cls == "nonsky") return !(b >= 235 && (g - r) >= 12);
    if (cls == "yellow") return g >= 200 && r >= 150 && b <= 110;
    if (cls == "medal") return (g >= 200 && r >= 150 && b <= 110) || (r >= 200 && g >= 140 && g <= 200 && b <= 60);
    if (cls == "lavender") return r >= 190 && b >= 230 && r > g;
    if (cls == "pink") return r >= 200 && g <= 130 && b >= 150;
    if (cls == "lilac_navy") return (std::abs(r - 176) <= 24 && std::abs(g - 120) <= 24 && std::abs(b - 168) <= 24) || navy;
    if (cls == "green") return g >= 60 && g > r + 15 && g > b + 15;
    if (cls == "badge_green") return std::abs(r - 50) <= 10 && std::abs(g - 129) <= 12 && std::abs(b - 62) <= 10;
    if (cls == "cyan") return b >= 180 && g >= 150 && r <= 150;
    if (cls == "label_blue") return b >= 150 && r <= 120 && g <= 160 && b > r + 60;
    if (cls == "plate_blue") return b >= 170 && r <= 120 && g <= 140;
    if (cls == "white") return r >= 200 && g >= 200 && b >= 200;
    if (cls == "arc") return r >= 150 && g >= 180 && b <= 120;
    return false;
}

L::RaceHudPngBox measure(const Rgb8& img, const std::string& cls, const Json& win) {
    int x0 = 1 << 30, x1 = -1, y0 = 1 << 30, y1 = -1;
    for (int y = win[2].i(); y < win[3].i() && y < img.h; ++y)
        for (int x = win[0].i(); x < win[1].i() && x < img.w; ++x)
            if (colourClass(cls, img.at(x, y))) {
                x0 = std::min(x0, x); x1 = std::max(x1, x + 1);
                y0 = std::min(y0, y); y1 = std::max(y1, y + 1);
            }
    if (x1 < 0) return {0, 0, 0, 0};
    return {x0, x1, y0, y1};
}

std::string pngChecks(const Json& fx) {
#if !defined(RT_W4_HAVE_ZLIB)
    (void)fx;
    std::printf("SKIP png: built without zlib\n");
    return "skip";
#else
    const fs::path dir = goldenDir();
    if (dir.empty()) {
        std::printf("SKIP png: build/scratch/oracle-golden/race_hud{,_driving}.png not present\n");
        return "skip";
    }
    std::map<std::string, Rgb8> img;
    for (const char* n : {"race_hud_driving", "race_hud", "town_driving", "attract"}) {
        Rgb8 im;
        if (decodePng(dir / (std::string(n) + ".png"), im)) img[n] = std::move(im);
    }
    if (!img.count("race_hud_driving") || !img.count("race_hud")) {
        check(false, "race goldens present but failed to decode");
        return "fail";
    }
    const int before = g_failed;
    for (const char* n : {"race_hud_driving", "race_hud"})
        check(img[n].w == L::kRaceHudGoldenWidth && img[n].h == L::kRaceHudGoldenHeight, fmt("%s is 1280x447", n));
    // Element boxes re-measured with the fixture's class + window.
    for (const auto& e : fx["elements"].arr) {
        const auto* h = findElement(e["id"].str);
        if (!h) continue;
        for (const char* n : {"race_hud_driving", "race_hud"}) {
            const Json& want = e["measured_png"][n];
            const auto got = measure(img[n], e["class"].str, e["window_png"]);
            const auto exp = pngBoxOf(want);
            if (want.kind != Json::Arr) continue;  // not measurable on that frame (documented)
            check(std::memcmp(&got, &exp, sizeof got) == 0,
                  fmt("%s on %s re-measured [%d,%d,%d,%d] == fixture [%d,%d,%d,%d]", h->id, n, got.x0, got.x1, got.y0,
                      got.y1, exp.x0, exp.x1, exp.y0, exp.y1));
        }
    }
    // Timer cells.
    for (const auto& c : fx["timer_cells"].arr)
        for (const char* n : {"race_hud_driving", "race_hud"}) {
            const Json& want = c["yellow_png"][n];
            const auto got = measure(img[n], "yellow", c["window_png"]);
            const auto exp = pngBoxOf(want);
            check(std::memcmp(&got, &exp, sizeof got) == 0,
                  fmt("timer cell %s %s on %s re-measured", c["block"].str.c_str(), c["cell"].str.c_str(), n));
        }
    // Colour samples (exact).
    for (const auto& s : L::kRaceHudColorSamples) {
        const Rgb8& im = img[s.golden == L::RaceHudGolden::Driving ? "race_hud_driving" : "race_hud"];
        const auto* p = im.at(s.pngX, s.pngY);
        check(p[0] == s.rgb.r && p[1] == s.rgb.g && p[2] == s.rgb.b,
              fmt("colour %s at (%d,%d) = (%d,%d,%d), golden (%d,%d,%d)", s.what, s.pngX, s.pngY, s.rgb.r, s.rgb.g,
                  s.rgb.b, p[0], p[1], p[2]));
    }
    // Presence: race-only parts absent from town_driving; gauge present there; attract has neither.
    const auto near = [](const std::uint8_t* p, L::RaceHudRgb c) {
        return std::abs(p[0] - c.r) <= 12 && std::abs(p[1] - c.g) <= 12 && std::abs(p[2] - c.b) <= 12;
    };
    if (img.count("town_driving")) {
        const Rgb8& t = img["town_driving"];
        check(!near(t.at(68, 20), {8, 8, 48}) && !near(t.at(54, 28), {208, 248, 48}) && !near(t.at(94, 102), {248, 40, 200}),
              "town_driving has no TOTALTIME banner / rank badge (Race presence)");
        check(near(t.at(1100, 294), {38, 86, 29}) && near(t.at(1080, 400), {50, 129, 62}) &&
                  near(t.at(1162, 364), {74, 95, 196}),
              "town_driving shows dial face, mph badge, gear plate (RaceAndTown presence)");
    } else {
        std::printf("SKIP png presence: town_driving.png absent\n");
    }
    if (img.count("attract")) {
        const Rgb8& a = img["attract"];
        check(!near(a.at(1100, 294), {38, 86, 29}) && !near(a.at(68, 20), {8, 8, 48}), "attract golden has no race HUD");
    }
    return g_failed == before ? "pass" : "fail";
#endif
}

// ------------------------------------------------------------------ tier 3b: ELF pins

struct Elf {
    std::vector<std::uint8_t> b;
    bool ok(std::uint32_t va, std::size_t n) const { return va >= 0x1FF000u && va - 0x1FF000u + n <= b.size(); }
    std::uint32_t u32(std::uint32_t va) const {
        const std::size_t o = va - 0x1FF000u;
        return std::uint32_t(b[o]) | (std::uint32_t(b[o + 1]) << 8) | (std::uint32_t(b[o + 2]) << 16) |
               (std::uint32_t(b[o + 3]) << 24);
    }
    std::int32_t s32(std::uint32_t va) const { return static_cast<std::int32_t>(u32(va)); }
    std::uint8_t u8(std::uint32_t va) const { return b[va - 0x1FF000u]; }
};

constexpr std::uint32_t addiu(int rt, int rs, int imm) {
    return (9u << 26) | (static_cast<std::uint32_t>(rs) << 21) | (static_cast<std::uint32_t>(rt) << 16) |
           (static_cast<std::uint32_t>(imm) & 0xFFFFu);
}

std::string elfChecks() {
    const fs::path p = elfPath();
    if (p.empty()) {
        std::printf("SKIP elf: set RT_ELF or provide build/rom/SLUS_203.98\n");
        return "skip";
    }
    Elf e{readFile(p)};
    if (!e.ok(0x2EC1BC, 4) || e.b.size() < 0x100000) {
        std::printf("SKIP elf: %s is not the SLUS_203.98 image\n", p.string().c_str());
        return "skip";
    }
    const int before = g_failed;
    // Arena records {x,y,u,v,w,h,...} 32 B.
    const auto rec = [&](std::uint32_t va, int x, int y, int u, int v, int w, int h) {
        return e.s32(va) == x && e.s32(va + 4) == y && e.s32(va + 8) == u && e.s32(va + 12) == v &&
               e.s32(va + 16) == w && e.s32(va + 20) == h;
    };
    const auto spr = [&](std::uint32_t va, const L::RaceHudSprite& s, int ox, int oy) {
        return rec(va, s.x - ox, s.y - oy, s.u, s.v, s.w, s.h);
    };
    check(spr(0x29F628, L::kRaceHudTotalBanner[0], 20, 20) && spr(0x29F648, L::kRaceHudTotalBanner[1], 20, 20),
          "0x29F628/0x29F648 TOTALTIME records");
    check(spr(0x29F668, L::kRaceHudLapBanner[0], 490, 20) && spr(0x29F688, L::kRaceHudLapBanner[1], 490, 20),
          "0x29F668/0x29F688 LAPTIME records");
    check(e.s32(0x2EC1A8) == 1 && e.s32(0x2EC1AC) == L::kRaceHudTotalBannerOrigin.x &&
              e.s32(0x2EC1B0) == L::kRaceHudTotalBannerOrigin.y && e.u32(0x2EC1B4) == 0x29F628u && e.s32(0x2EC1B8) == 2,
          "0x2EC1A8 TOTALTIME group {1,20,20,0x29F628,2}");
    check(e.s32(0x2EC070) == 0 && e.s32(0x2EC074) == L::kRaceHudLapBannerOrigin.x &&
              e.s32(0x2EC078) == L::kRaceHudLapBannerOrigin.y && e.u32(0x2EC07C) == 0x29F668u && e.s32(0x2EC080) == 2,
          "0x2EC070 LAPTIME group {0,490,20,0x29F668,2} (enable 0 = conditional)");
    check(e.s32(0x2EC088) == 0 && e.s32(0x2EC08C) == L::kRaceHudLapPanelOrigin.x && e.s32(0x2EC090) == L::kRaceHudLapPanelOrigin.y,
          "0x2EC088 ElementC P0 slot 0 {0,494,52}");
    check(e.u32(0x233F18) == addiu(6, 0, L::kRaceHudTotalPanelOrigin.x) &&
              e.u32(0x233F1C) == addiu(7, 0, L::kRaceHudTotalPanelOrigin.y),
          "0x233F18/1C ElementC TOTAL origin (60,52)");
    check(e.u32(0x232388) == addiu(3, 0, L::kRaceHudRankOrigin.x) && e.u32(0x23238C) == addiu(2, 0, L::kRaceHudRankOrigin.y),
          "0x232388/8C ElementD origin (40,88)");
    check(spr(0x2EBD40, L::kRaceHudPanelCapLeft, 0, 0) && spr(0x2EBD60, L::kRaceHudPanelCapRight, 0, 0) &&
              spr(0x2EBD80, L::kRaceHudPanelMiddle, 0, 0),
          "0x2EBD40/60/80 panel templates");
    check(spr(0x2EBDA0, L::kRaceHudTimerSepMinutes, 0, 0) && spr(0x2EBDC0, L::kRaceHudTimerSepSeconds, 0, 0),
          "0x2EBDA0/C0 separator templates");
    check(rec(0x2EBDE0, L::kRaceHudTimerDigitDx[5], L::kRaceHudTimerDigitDy, 0, 0, 16, 16), "0x2EBDE0 digit template x 102");
    check(spr(0x2EBE00, L::kRaceHudLapDisc, 0, 0) && spr(0x2EBE20, L::kRaceHudLapFlag, 0, 0), "0x2EBE00/20 lap badge templates");
    check(spr(0x2EBE40, L::kRaceHudRankPanel, 0, 0), "0x2EBE40 rank panel template");
    bool bank = true;
    for (int i = 0; i < 4; ++i) bank = bank && e.s32(0x29FCA8 + 4 * i) == L::kRaceHudTimerBankCsa[static_cast<std::size_t>(i)];
    check(bank, "0x29FCA8 timer bank table {0,1,12,0}");
    check(e.s32(0x2A8FF0) == L::kRaceHudGaugeOrigin.x && e.s32(0x2A8FF4) == L::kRaceHudGaugeOrigin.y,
          "0x2A8FF0 1P gauge origin (484,294)");
    check(e.u32(0x2498FC) == addiu(18, 0, 0x59) && e.u32(0x249AB8) == addiu(2, 0, 0x5e) &&
              e.u32(0x249AE8) == addiu(18, 18, L::kRaceHudMphStep),
          "handler-0 mph x 0x59 / y 0x5e / step -0x11");
    check(e.u32(0x249A08) == addiu(2, 0, L::kRaceHudGearGlyphOffset.x) &&
              e.u32(0x2498D0) == addiu(17, 0, L::kRaceHudGearGlyphOffset.y),
          "handler-0 gear glyph offset (97,60)");
    check(e.u32(0x249E54) == addiu(2, 0, 70) && e.u32(0x249E24) == addiu(3, 0, 68) &&
              e.u32(0x249E30) == addiu(10, 0, L::kRaceHudNeedleStripLength),
          "handler-0 needle pivot (70,68) and strip 59");
    // FONTE advance excerpt.
    check(e.u8(0x29A880 + (' ' - 0x20)) == L::kRaceHudFonteSpaceAdvance &&
              e.u8(0x29A880 + ('.' - 0x20)) == L::kRaceHudFontePeriodAdvance &&
              e.u8(0x29A880 + ('A' - 0x20)) == L::kRaceHudFonteUpperAAdvance &&
              e.u8(0x29A880 + ('a' - 0x20)) == L::kRaceHudFonteLowerAAdvance,
          "0x29A880 advance ' ' . A a");
    bool digits = true;
    for (char c = '0'; c <= '9'; ++c) digits = digits && e.u8(0x29A880 + (c - 0x20)) == L::kRaceHudFonteDigitAdvance;
    check(digits, "0x29A880 digit advance 12");
    // TFX DECAL on race entries 0/1 and gauge entry 0 (TEX0 bits 35..36 == 1).
    const auto tfx = [&](std::uint32_t va) { return (e.u32(va + 4) >> 3) & 3u; };
    check(tfx(0x29F600) == 1 && tfx(0x29F610) == 1 && tfx(0x2A9010) == 1, "TEX0 TFX = DECAL (uniform-white draw is exact)");
    return g_failed == before ? "pass" : "fail";
}

}  // namespace

int main() {
    numericChecks();
    builderChecks();
    Json fx;
    std::string png = "skip";
    if (loadFixture(fx)) {
        fixtureChecks(fx);
        png = pngChecks(fx);
    }
    const std::string elf = elfChecks();
    std::printf("W4-06 SCORE checks=%d failed=%d elements=%zu png=%s elf=%s needle=GAP %s\n", g_checks, g_failed,
                L::kRaceHudElements.size(), png.c_str(), elf.c_str(), g_failed == 0 ? "PASS" : "FAIL");
    return g_failed == 0 ? 0 : 1;
}
