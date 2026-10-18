// W2-11 minimap projection acceptance (w2_minimap_test).
//
//   w2_minimap_test [--elf <SLUS_203.98>] [--oracle-crops DIR] [--golden FILE]
//
// Prints one `W2-11 SCORE k=v ... PASS|FAIL` line; exit 0 only when every threshold is met.
// Missing ELF -> prints NO_ELF and exits 0 (the brief's skip mode).  Missing oracle crops ->
// NO_ORACLE, exit 2 (run tests/native_w2/minimap_oracle_shift.py export first).
// No ROM bytes are embedded: ELF floats / instruction immediates are compared against the module's
// authored constants; tests/native_w2/data/minimap_golden.tsv holds derived numbers only.

#include "gameplay/minimap.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace mm = rt::native::gameplay::minimap;
namespace fs = std::filesystem;

namespace {

struct Hasher {
    std::uint64_t h = 1469598103934665603ull;
    void u64(std::uint64_t v) {
        for (int i = 0; i < 8; ++i) { h ^= (v >> (8 * i)) & 0xff; h *= 1099511628211ull; }
    }
    void f64(double v) { std::uint64_t b; std::memcpy(&b, &v, 8); u64(b); }
    void str(const std::string& s) { for (unsigned char c : s) { h ^= c; h *= 1099511628211ull; } }
};

struct Results {
    int elfPass = 0, elfTotal = 0;
    int goldPass = 0, goldTotal = 0, goldLevels = 0;
    int carPass = 0, carTotal = 0;
    int clipPass = 0, clipTotal = 0;
    int lawPass = 0, lawTotal = 0;
    int rulePass = 0, ruleTotal = 0;
    int oracleOk = 0;
    double dx = 0, dy = 0;
    double townResid448[2] = {0, 0}, townResid480[2] = {0, 0};
    double raceResid[2][2] = {{0, 0}, {0, 0}};
    double raceDeltaErr[2] = {0, 0};
    double raceDeltaMeas[2] = {0, 0}, raceDeltaPred[2] = {0, 0};
    double townMeas[2] = {0, 0}, townPred[2] = {0, 0};
    int verdictVotes448 = 0, verdictVotes480 = 0;
    std::vector<std::string> info;
    std::uint64_t hash = 0;
};

void check(int& pass, int& total, bool ok, Results& r, const char* what) {
    ++total;
    if (ok) ++pass;
    else r.info.push_back(std::string("FAIL ") + what);
}

// ------------------------------------------------------------ ELF helpers

std::vector<std::uint8_t> readFile(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

std::uint32_t le32(const std::vector<std::uint8_t>& e, std::uint32_t v) {
    const std::size_t o = v - 0x1FF000u;
    return e[o] | (e[o + 1] << 8) | (e[o + 2] << 16) | (static_cast<std::uint32_t>(e[o + 3]) << 24);
}
float lef(const std::vector<std::uint8_t>& e, std::uint32_t v) {
    const std::uint32_t u = le32(e, v);
    float f;
    std::memcpy(&f, &u, 4);
    return f;
}
std::uint64_t le64(const std::vector<std::uint8_t>& e, std::uint32_t v) {
    return le32(e, v) | (static_cast<std::uint64_t>(le32(e, v + 4)) << 32);
}
bool sameBits(float a, float b) { return std::memcmp(&a, &b, 4) == 0; }

void runElf(Results& r, const std::vector<std::uint8_t>& e) {
    auto ck = [&](bool ok, const char* w) { check(r.elfPass, r.elfTotal, ok, r, w); };
    // zoom table 0x2A42C0: 16-byte stride {x0, y0, scale, 0}
    for (int i = 0; i < mm::kTownZoomCount; ++i) {
        const std::uint32_t a = 0x2A42C0u + 16u * static_cast<std::uint32_t>(i);
        ck(sameBits(lef(e, a), mm::kTownZoom[static_cast<std::size_t>(i)].x0), "zoom.x0");
        ck(sameBits(lef(e, a + 4), mm::kTownZoom[static_cast<std::size_t>(i)].y0), "zoom.y0");
        ck(sameBits(lef(e, a + 8), mm::kTownZoom[static_cast<std::size_t>(i)].scale), "zoom.scale");
        ck(le32(e, a + 12) == 0, "zoom.pad");
    }
    // placement tables (bitwise)
    bool raceAll = true, splitAll = true;
    for (int i = 0; i < mm::kRacePlacementCount; ++i) {
        const std::uint32_t a = 0x2A4050u + 16u * static_cast<std::uint32_t>(i);
        const mm::Placement& p = mm::kRacePlacements[static_cast<std::size_t>(i)];
        raceAll &= sameBits(lef(e, a), p.x) && sameBits(lef(e, a + 4), p.y) && sameBits(lef(e, a + 8), p.scale) &&
                   sameBits(lef(e, a + 12), p.rot);
    }
    for (int i = 0; i < mm::kSplitPlacementCount; ++i) {
        const std::uint32_t a = 0x2A42E0u + 16u * static_cast<std::uint32_t>(i);
        const mm::Placement& p = mm::kSplitPlacements[static_cast<std::size_t>(i)];
        splitAll &= sameBits(lef(e, a), p.x) && sameBits(lef(e, a + 4), p.y) && sameBits(lef(e, a + 8), p.scale) &&
                    sameBits(lef(e, a + 12), p.rot);
    }
    ck(raceAll, "raceMapPlacements 39x4 == ELF 0x2A4050");
    ck(splitAll, "scissoredMapPlacements 5x4 == ELF 0x2A42E0");
    ck(0x2A4050u + 39u * 16u == 0x2A42C0u, "placement table abuts the zoom table (39 entries)");
    // scissor packets: {A+D header qword, SCISSOR_2 qword}: SCAX0 0..10, SCAX1 16..26, SCAY0 32..42, SCAY1 48..58
    auto sc = [&](std::uint32_t a) {
        const std::uint64_t q = le64(e, a + 16);
        return mm::ScissorRect{static_cast<int>(q & 0x7FF), static_cast<int>((q >> 16) & 0x7FF),
                               static_cast<int>((q >> 32) & 0x7FF), static_cast<int>((q >> 48) & 0x7FF)};
    };
    auto eq = [](const mm::ScissorRect& a, const mm::ScissorRect& b) {
        return a.x0 == b.x0 && a.x1 == b.x1 && a.y0 == b.y0 && a.y1 == b.y1;
    };
    ck(eq(sc(0x2A43F0), mm::kScissorTownNtsc), "scissor town NTSC 0x2A43F0");
    ck(eq(sc(0x2A4410), mm::kScissorTownPal), "scissor town PAL 0x2A4410");
    ck(eq(sc(0x2A4430), mm::kScissorSplit), "scissor split 0x2A4430");
    ck(le32(e, 0x2A43F0) == 0x8001 && le32(e, 0x2A43F4) == 0x10000000 && le32(e, 0x2A43F8) == 0xE,
       "scissor packet header (A+D, NLOOP 1, REGS 0xE)");
    // vu0 / sdata constants
    ck(sameBits(lef(e, 0x332CAC), -mm::kYSquash), "DAT_00332CAC = -0.47");
    ck(sameBits(lef(e, 0x332EFC), mm::kYSquash) && sameBits(lef(e, 0x332EF8), mm::kYSquash), "0x332EFC/0x332EF8 = 0.47");
    ck(sameBits(lef(e, 0x332CA8), mm::kPalXScale) && sameBits(lef(e, 0x332EF4), mm::kPalXScale) &&
           sameBits(lef(e, 0x332F14), mm::kPalXScale) && sameBits(lef(e, 0x332F2C), mm::kPalXScale),
       "PAL x scale 0.8 (4 sites)");
    ck(sameBits(lef(e, 0x2A43E0), mm::kViewOffsetX) && sameBits(lef(e, 0x2A43E8), mm::kViewOffsetZ) &&
           sameBits(lef(e, 0x2A43E4), -200.f) && sameBits(lef(e, 0x2A43EC), 1.f),
       "view offset (-800,-200,-800,1) 0x2A43E0");
    ck(sameBits(lef(e, 0x332F18), -1.57079637f) && sameBits(lef(e, 0x332F00), -1.57079637f) &&
           sameBits(lef(e, 0x332F30), -1.57079637f), "RotX(-pi/2) constants");
    ck(sameBits(lef(e, 0x332F34), 3.14159274f) && sameBits(lef(e, 0x332F38), 3.14159274f), "pi for heading");
    // lui immediates (float literals) inside gs_map_draw_functions_town
    struct Lui { std::uint32_t a; std::uint16_t imm; };
    for (const Lui& l : {Lui{0x242518, 0x4348}, Lui{0x2425BC, 0x4348}, Lui{0x2425D4, 0x44D8}, Lui{0x2425E4, 0x44F2},
                         Lui{0x242530, 0x44F0}, Lui{0x24253C, 0x44E0}, Lui{0x24261C, 0x4440}}) {
        const std::uint32_t w = le32(e, l.a);
        ck((w >> 16) == 0x3C01 && (w & 0xFFFF) == l.imm, "lui immediate in 0x242438");
    }
    ck(mm::xyOffset(mm::VideoMode::Ntsc).x == 1728.f && mm::xyOffset(mm::VideoMode::Ntsc).y == 1936.f &&
           mm::xyOffset(mm::VideoMode::Pal).x == 1792.f && mm::xyOffset(mm::VideoMode::Pal).y == 1920.f,
       "XYOFFSET = 2048 - (320,112) / 2048 - (256,128)");
    // quadrant origins 0x29D080 {x, 0, z, 0}
    bool quad = true;
    for (int i = 0; i < mm::kQuadrantCount; ++i) {
        const std::uint32_t a = 0x29D080u + 16u * static_cast<std::uint32_t>(i);
        quad &= sameBits(lef(e, a), mm::kQuadrantOrigins[static_cast<std::size_t>(i)].x) &&
                sameBits(lef(e, a + 8), mm::kQuadrantOrigins[static_cast<std::size_t>(i)].z) && le32(e, a + 4) == 0;
    }
    ck(quad, "tileQuadrantOrigins 6 x {x,0,z,0}");
    // colour sets
    const std::array<std::pair<std::uint32_t, std::array<float, 4>>, 4> cs{{
        {0x2A4370, {1.f, 0.f, 0.f, 1.f}}, {0x2A4380, {0.f, 0.7f, 0.7f, 1.f}},
        {0x2A4390, {0.f, 1.f, 0.f, 0.5f}}, {0x2A43A0, {1.f, 1.f, 0.f, 1.f}}}};
    for (const auto& c : cs) {
        bool ok = true;
        for (int k = 0; k < 4; ++k) ok &= sameBits(lef(e, c.first + 4u * static_cast<std::uint32_t>(k)), c.second[static_cast<std::size_t>(k)]);
        ck(ok, "colour set");
    }
    ck(mm::blipColourSet(mm::BlipKind::PlayerArrow) == 0x2A4370 && mm::blipColourSet(mm::BlipKind::Player2Arrow) == 0x2A4380 &&
           mm::blipColourSet(mm::BlipKind::RivalDot) == 0x2A4390 && mm::blipColourSet(mm::BlipKind::CoinDot) == 0x2A43A0,
       "blip colour-set vaddrs");
    // operation order of the NTSC branch (mul.s/div.s/add.s/sub.s, COP1 fmt S)
    struct Op { std::uint32_t a; std::uint32_t fn; };
    bool order = true;
    for (const Op& o : {Op{0x2425B8, 2}, Op{0x2425C4, 2}, Op{0x2425E0, 2}, Op{0x2425F4, 3}, Op{0x2425F8, 0},
                        Op{0x2425FC, 0}, Op{0x242608, 3}, Op{0x24260C, 1}, Op{0x242614, 0}}) {
        const std::uint32_t w = le32(e, o.a);
        order &= (w >> 26) == 0x11 && (w & 0x3F) == o.fn;
    }
    ck(order, "NTSC view-translation op order 0x2425B8..0x242614");
}

// ------------------------------------------------------------ golden vectors

void runGolden(Results& r, const fs::path& file) {
    std::ifstream in(file);
    std::set<float> levels;
    std::string line;
    std::uint64_t digest = 0;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream is(line);
        std::string kind;
        int pal, idx;
        double px, pz, wx, wz, vtx, vty, fx, fy;
        if (!(is >> kind >> pal >> idx >> px >> pz >> wx >> wz >> vtx >> vty >> fx >> fy)) continue;
        const mm::VideoMode vm = pal ? mm::VideoMode::Pal : mm::VideoMode::Ntsc;
        mm::FieldPoint f{};
        mm::ViewBlock b{};
        if (kind == "town") {
            f = mm::townProject(static_cast<std::uint8_t>(idx), static_cast<float>(px), static_cast<float>(pz),
                                static_cast<float>(wx), static_cast<float>(wz), vm);
            b = mm::townViewBlock(static_cast<std::uint8_t>(idx), static_cast<float>(px), static_cast<float>(pz), vm);
            levels.insert(mm::townZoom(static_cast<std::uint8_t>(idx)).scale);
        } else {
            const mm::Placement& p = kind == "race" ? mm::kRacePlacements[static_cast<std::size_t>(idx)]
                                                    : mm::kSplitPlacements[static_cast<std::size_t>(idx)];
            f = mm::placementProject(p, static_cast<float>(wx), static_cast<float>(wz), vm);
            b = mm::placementViewBlock(p, vm);
            levels.insert(p.scale);
        }
        const bool ok = std::fabs(f.x - fx) < 2e-3 && std::fabs(f.y - fy) < 2e-3 && std::fabs(b.tx - vtx) < 2e-3 &&
                        std::fabs(b.ty - vty) < 2e-3;
        check(r.goldPass, r.goldTotal, ok, r, "golden vector");
        digest = digest * 31 + static_cast<std::uint64_t>(static_cast<std::int64_t>(f.x * 1000) ^ static_cast<std::int64_t>(f.y * 1000));
    }
    r.goldLevels = static_cast<int>(levels.size());
    r.hash ^= digest;
}

// ------------------------------------------------------------ analytic laws

std::uint32_t lcg(std::uint32_t& s) { s = s * 1664525u + 1013904223u; return s >> 8; }
float frand(std::uint32_t& s, float lo, float hi) { return lo + (hi - lo) * static_cast<float>(lcg(s) & 0xFFFF) / 65535.f; }
float quant16(float v) { return std::round(v * 16.f) / 16.f; }  // GS 12.4 fixed point

void runLaws(Results& r) {
    std::uint32_t s = 0x1234567u;
    // 1000 random poses: the player lands at field px (102,183) for both zoom modes (NTSC exact at
    // GS 1/16 px resolution; PAL (81.6, 183)).
    for (int i = 0; i < 1000; ++i) {
        const float px = frand(s, 0.f, 1600.f), pz = frand(s, 0.f, 1600.f);
        for (std::uint8_t mode = 0; mode < 2; ++mode) {
            const mm::FieldPoint f = mm::townProject(mode, px, pz, px, pz, mm::VideoMode::Ntsc);
            const bool ok = quant16(f.x) == 102.f && quant16(f.y) == 183.f && std::fabs(f.x - 102.f) < 5e-3f &&
                            std::fabs(f.y - 183.f) < 5e-3f;
            check(r.carPass, r.carTotal, ok, r, "car at (102,183)");
            const mm::FieldPoint g = mm::townProject(mode, px, pz, px, pz, mm::VideoMode::Pal);
            check(r.carPass, r.carTotal, std::fabs(g.x - 81.6f) < 0.04f && std::fabs(g.y - 183.f) < 5e-3f, r, "PAL car at (81.6,183)");
        }
    }
    {
        const mm::FieldPoint c0 = mm::townCarMarker(0, mm::VideoMode::Ntsc), c1 = mm::townCarMarker(1, mm::VideoMode::Ntsc);
        check(r.carPass, r.carTotal, quant16(c0.x) == 102.f && quant16(c0.y) == 183.f && quant16(c1.x) == 102.f &&
                                         quant16(c1.y) == 183.f, r, "townCarMarker");
    }
    // closed form vs view-block path (NTSC): fx = x0 - 4 s + (wx-px) s/200 ; fy = y0 + 1.88 s - 0.47 s (wz-pz)/200
    for (int i = 0; i < 1000; ++i) {
        const double px = frand(s, 0.f, 1600.f), pz = frand(s, 0.f, 1600.f);
        const double wx = px + frand(s, -600.f, 600.f), wz = pz + frand(s, -600.f, 600.f);
        for (std::uint8_t mode = 0; mode < 2; ++mode) {
            const mm::ZoomEntry& z = mm::kTownZoom[mode];
            const double ex = z.x0 - 4.0 * z.scale + (wx - px) * z.scale / 200.0;
            const double ey = z.y0 + 4.0 * 0.47 * z.scale - 0.47 * z.scale * (wz - pz) / 200.0;
            const mm::FieldPoint f = mm::townProject(mode, static_cast<float>(px), static_cast<float>(pz),
                                                     static_cast<float>(wx), static_cast<float>(wz), mm::VideoMode::Ntsc);
            check(r.lawPass, r.lawTotal, std::fabs(f.x - ex) < 2e-3 && std::fabs(f.y - ey) < 2e-3, r, "closed form");
        }
    }
    // clip: every point outside the scissor is clipped, inside never (blip pipeline + raw predicate)
    for (const mm::VideoMode vm : {mm::VideoMode::Ntsc, mm::VideoMode::Pal}) {
        const mm::ScissorRect rc = mm::townScissor(vm);
        for (int i = 0; i < 1000; ++i) {
            const mm::FieldPoint p{frand(s, -40.f, 260.f), frand(s, 100.f, 260.f)};
            const bool inside = p.x >= static_cast<float>(rc.x0) && p.x < static_cast<float>(rc.x1 + 1) &&
                                p.y >= static_cast<float>(rc.y0) && p.y < static_cast<float>(rc.y1 + 1);
            check(r.clipPass, r.clipTotal, mm::scissorContains(rc, p) == inside, r, "scissorContains");
        }
        // blip pipeline: coins at random world offsets from the car, tile slot 3 / quadrant 3 (origin 0,0)
        mm::TileSlots tiles;
        tiles.tileId[3] = 3;
        tiles.quadrant[3] = 3;
        std::vector<mm::Coin> coins;
        for (int i = 0; i < 100; ++i) coins.push_back({frand(s, 0.f, 1600.f), frand(s, 0.f, 1600.f), 3});
        const std::array<std::uint64_t, 2> all{~0ull, ~0ull};
        for (int pose = 0; pose < 10; ++pose) {
            const float px = frand(s, 0.f, 1600.f), pz = frand(s, 0.f, 1600.f);
            for (std::uint8_t mode = 0; mode < 2; ++mode) {
                const auto blips = mm::townCoinBlips(mode, vm, px, pz, coins, all, tiles);
                check(r.clipPass, r.clipTotal, blips.size() == 100, r, "coin count");
                for (const mm::Blip& b : blips) {
                    const mm::ZoomEntry& z = mm::kTownZoom[mode];
                    const double kx = vm == mm::VideoMode::Ntsc ? 1.0 : 0.8;
                    const double fx = (z.x0 * kx - 4.0 * z.scale * kx) + (b.worldX - px) * z.scale * kx / 200.0;
                    const double fy = (z.y0 + 4.0 * 0.47 * z.scale) - 0.47 * z.scale * (b.worldZ - pz) / 200.0;
                    const bool inside = fx >= rc.x0 && fx < rc.x1 + 1 && fy >= rc.y0 && fy < rc.y1 + 1;
                    const bool edge = std::fabs(fx - rc.x0) < 0.01 || std::fabs(fx - (rc.x1 + 1)) < 0.01 ||
                                      std::fabs(fy - rc.y0) < 0.01 || std::fabs(fy - (rc.y1 + 1)) < 0.01;
                    check(r.clipPass, r.clipTotal, edge || (b.clipped == !inside), r, "blip clip flag");
                }
            }
        }
    }
    // boundary points
    {
        const mm::ScissorRect rc = mm::kScissorTownNtsc;
        const mm::FieldPoint in[] = {{45.f, 156.f}, {158.75f, 207.75f}, {100.f, 180.f}};
        const mm::FieldPoint out[] = {{44.75f, 160.f}, {159.f, 160.f}, {100.f, 155.75f}, {100.f, 208.f}};
        for (const auto& p : in) check(r.clipPass, r.clipTotal, mm::scissorContains(rc, p), r, "boundary in");
        for (const auto& p : out) check(r.clipPass, r.clipTotal, !mm::scissorContains(rc, p), r, "boundary out");
    }
    // race: no scissor; split: scissor 48..208 x 68..200
    check(r.clipPass, r.clipTotal, !mm::clipsToScissor(mm::MapKind::Race) && mm::clipsToScissor(mm::MapKind::Split) &&
                                       mm::clipsToScissor(mm::MapKind::Town), r, "clip kinds");
}

// ------------------------------------------------------------ rules

void runRules(Results& r) {
    auto ck = [&](bool ok, const char* w) { check(r.rulePass, r.ruleTotal, ok, r, w); };
    ck(mm::nextZoomMode(0, true) == 1 && mm::nextZoomMode(1, true) == 0 && mm::nextZoomMode(1, false) == 1, "zoom toggle");
    ck(mm::classifyCourse(-1, false) == mm::MapKind::Town && mm::classifyCourse(0, false) == mm::MapKind::Race &&
           mm::classifyCourse(0x19, true) == mm::MapKind::Split && mm::classifyCourse(39, false) == mm::MapKind::None &&
           mm::classifyCourse(-2, false) == mm::MapKind::None, "classifyCourse");
    ck(&mm::coursePlacement(0x19, 3) == &mm::kSplitPlacements[3] && &mm::coursePlacement(7) == &mm::kRacePlacements[7],
       "coursePlacement selection");
    ck(std::fabs(mm::markerScale(50.f) - 1.f) < 1e-6f && std::fabs(mm::markerScale(25.f) - 2.f) < 1e-6f, "markerScale 50/s");
    ck(std::fabs(mm::headingRad(0x4000) - 1.5707964f) < 1e-5f && std::fabs(mm::headingRad(-0x8000) + 3.1415927f) < 1e-5f,
       "heading pi*2^-15");

    // race blips: P1 arrow, rival filter, finished dots, two-player ordering
    std::array<mm::CarBlock, mm::kCarSlots> cars{};
    for (int i = 0; i < mm::kCarSlots; ++i) cars[static_cast<std::size_t>(i)] = {100.f + 10.f * i, 200.f + 5.f * i, 0, 0};
    cars[2].status = 1;        // active rival
    cars[3].status = 0x31;     // finished, red
    cars[4].status = 0x70;     // finished, teal
    cars[5].status = 0x20;     // finished (bit 0x20 is in 0x30), red
    cars[9].status = 0x0004;   // active rival
    cars[10].status = 0x10;    // finished but slot > 6: not drawn
    cars[0].heading = 0x2000;
    const mm::Placement& pl = mm::kRacePlacements[0];
    auto b1 = mm::raceBlips(pl, mm::VideoMode::Ntsc, cars, false);
    ck(b1.size() == 1 + 2 + 3 && b1[0].kind == mm::BlipKind::PlayerArrow && b1[1].kind == mm::BlipKind::RivalDot &&
           b1[1].source == 2 && b1[2].source == 9 && b1[3].kind == mm::BlipKind::FinishedDotRed && b1[3].source == 3 &&
           b1[4].kind == mm::BlipKind::FinishedDotTeal && b1[4].source == 4 && b1[5].kind == mm::BlipKind::FinishedDotRed,
       "race blips 1P order/filters");
    ck(std::fabs(b1[0].yaw - mm::headingRad(0x2000)) < 1e-6f && b1[0].modelScale == 1.f && !b1[0].clipped, "race P1 yaw/scale");
    cars[1].status = 1;
    auto b2 = mm::raceBlips(pl, mm::VideoMode::Ntsc, cars, true);
    ck(b2.size() == 1 + 1 + 2 + 3 && b2[1].kind == mm::BlipKind::Player2Arrow && b2[1].source == 1 &&
           b2[2].kind == mm::BlipKind::RivalDot && b2[2].source == 2, "race blips 2P order (rivals from slot 2)");
    cars[1].status = 0x30;  // slot 1 finished: still drawn in the last loop even in 2P
    auto b3 = mm::raceBlips(pl, mm::VideoMode::Ntsc, cars, true);
    ck(b3.size() == b2.size() + 1 && b3[4].kind == mm::BlipKind::FinishedDotRed && b3[4].source == 1, "slot 1 finished dot in last loop");
    const mm::Placement& p15 = mm::kRacePlacements[15];  // scale 40
    ck(std::fabs(mm::raceBlips(p15, mm::VideoMode::Ntsc, cars, false)[0].modelScale - 1.25f) < 1e-6f, "marker scale uses entry scale");
    const mm::PickupPos pk[] = {{10.f, 20.f}, {30.f, 40.f}};
    const auto pb = mm::racePickupBlips(pl, mm::VideoMode::Ntsc, pk);
    ck(pb.size() == 2 && pb[0].kind == mm::BlipKind::PickupDot && pb[1].source == 1, "pickup blips");

    // town blips: gating by status, tile loaded, quadrant origin added
    mm::TileSlots tiles;
    tiles.tileId = {4, 1, 6, 11};
    tiles.quadrant = {0, -1, 2, 3};
    std::vector<mm::TownCar> tc(25);
    tc[0] = {100.f, 100.f, 1, 11};  // slot 3 -> quadrant 3 (0,0)
    tc[1] = {100.f, 100.f, 0, 11};  // inactive
    tc[2] = {50.f, 60.f, 5, 6};     // slot 2 -> quadrant 2 (-1600, 0)
    tc[3] = {10.f, 10.f, 1, 1};     // slot 1 empty (quadrant -1)
    tc[4] = {10.f, 10.f, 1, 7};     // slot 3 holds 11 not 7: not loaded
    tc[24] = {10.f, 10.f, 1, 11};   // beyond the 23 entries
    const mm::CarBlock pc{500.f, 500.f, 0x1000, 0};
    const auto tb = mm::townBlips(0, mm::VideoMode::Ntsc, pc, tc, tiles);
    ck(tb.size() == 3 && tb[0].kind == mm::BlipKind::PlayerArrow && tb[1].source == 1 && tb[2].source == 3, "town car gating");
    ck(tb[2].worldX == 50.f - 1600.f && tb[2].worldZ == 60.f, "town car quadrant origin added");
    ck(std::fabs(tb[0].field.x - 102.f) < 5e-3f && std::fabs(tb[0].field.y - 183.f) < 5e-3f && !tb[0].clipped, "town arrow at (102,183)");
    ck(tb[2].clipped, "far car clipped");
    // coins: bit set => drawn
    std::vector<mm::Coin> coins = {{500.f, 500.f, 11}, {520.f, 500.f, 11}, {540.f, 500.f, 11}, {560.f, 500.f, 7}};
    std::array<std::uint64_t, 2> mask{0b0101ull, 0};
    const auto cb = mm::townCoinBlips(0, mm::VideoMode::Ntsc, 500.f, 500.f, coins, mask, tiles);
    ck(cb.size() == 2 && cb[0].source == 1 && cb[1].source == 3, "coin mask bit set = drawn; unloaded tile skipped");
    ck(mm::coinMaskBit({0, 1ull}, 65) && !mm::coinMaskBit({0, 1ull}, 64) && !mm::coinMaskBit({~0ull, ~0ull}, 0),
       "coin mask bit addressing");
    ck(cb.size() == 2 && std::fabs(cb[0].field.x - 102.f) < 5e-3f, "coin at car -> car marker position");
}

// ------------------------------------------------------------ oracle

struct Ppm {
    int w = 0, h = 0;
    std::vector<std::uint8_t> rgb;
};

bool readPpm(const fs::path& p, Ppm& out) {
    const auto data = readFile(p);
    if (data.size() < 12) return false;
    std::size_t pos = 0;
    auto token = [&]() {
        while (pos < data.size() && std::isspace(data[pos])) ++pos;
        std::string t;
        while (pos < data.size() && !std::isspace(data[pos])) t.push_back(static_cast<char>(data[pos++]));
        return t;
    };
    if (token() != "P6") return false;
    out.w = std::atoi(token().c_str());
    out.h = std::atoi(token().c_str());
    token();
    ++pos;
    if (out.w <= 0 || out.h <= 0 || data.size() - pos < static_cast<std::size_t>(out.w) * out.h * 3) return false;
    out.rgb.assign(data.begin() + static_cast<std::ptrdiff_t>(pos), data.begin() + static_cast<std::ptrdiff_t>(pos) + out.w * out.h * 3);
    return true;
}

struct Centroid { double x = 0, y = 0; int n = 0; };
Centroid redCentroid(const Ppm& p, int ox, int oy) {
    Centroid c;
    for (int y = 0; y < p.h; ++y) {
        for (int x = 0; x < p.w; ++x) {
            const std::uint8_t* q = &p.rgb[(static_cast<std::size_t>(y) * p.w + x) * 3];
            if (q[0] > 200 && q[1] < 60 && q[2] < 60) { c.x += x + ox; c.y += y + oy; ++c.n; }
        }
    }
    if (c.n) { c.x /= c.n; c.y /= c.n; }
    return c;
}

// Feature image: road (45,45,116)-like and building (78,65,51)-like pixels (same classifier as the python helper).
std::vector<double> features(const Ppm& p) {
    std::vector<double> f(static_cast<std::size_t>(p.w) * p.h);
    for (std::size_t i = 0; i < f.size(); ++i) {
        const int r = p.rgb[i * 3], g = p.rgb[i * 3 + 1], b = p.rgb[i * 3 + 2];
        const bool road = (b - r) > 40 && r < 80;
        const bool bld = (r - b) > 15 && r < 110 && g < 90;
        f[i] = (road ? 1.0 : 0.0) + (bld ? 1.0 : 0.0);
    }
    return f;
}

std::vector<double> gauss(const std::vector<double>& src, int w, int h, double sigma) {
    const int rad = static_cast<int>(std::ceil(sigma * 3));
    std::vector<double> k(static_cast<std::size_t>(2 * rad + 1));
    double sum = 0;
    for (int i = -rad; i <= rad; ++i) { k[static_cast<std::size_t>(i + rad)] = std::exp(-0.5 * i * i / (sigma * sigma)); sum += k[static_cast<std::size_t>(i + rad)]; }
    for (double& v : k) v /= sum;
    std::vector<double> tmp(src.size()), out(src.size());
    auto cl = [](int v, int n) { return v < 0 ? 0 : (v >= n ? n - 1 : v); };
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            double a = 0;
            for (int i = -rad; i <= rad; ++i) a += k[static_cast<std::size_t>(i + rad)] * src[static_cast<std::size_t>(y) * w + cl(x + i, w)];
            tmp[static_cast<std::size_t>(y) * w + x] = a;
        }
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            double a = 0;
            for (int i = -rad; i <= rad; ++i) a += k[static_cast<std::size_t>(i + rad)] * tmp[static_cast<std::size_t>(cl(y + i, h)) * w + x];
            out[static_cast<std::size_t>(y) * w + x] = a;
        }
    return out;
}

// Content shift (dx, dy) of B relative to A in crop pixels: B(p) ~ A(p - shift).
void measureShift(const Ppm& A, const Ppm& B, int ox, int oy, double& dxOut, double& dyOut) {
    const int w = A.w, h = A.h;
    const auto sa = gauss(features(A), w, h, 1.5);
    const auto sb = gauss(features(B), w, h, 1.5);
    // SCISSOR_2 45..158 x 156..207 -> shot x 91..319, y 312..414, inset 2; exclusion disc around the marker (204,366) r 24
    std::vector<std::uint8_t> valid(static_cast<std::size_t>(w) * h, 0);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const int sx = x + ox, sy = y + oy;
            const bool win = sx >= 93 && sx < 317 && sy >= 314 && sy < 412;
            const double d2 = (sx - 204.0) * (sx - 204.0) + (sy - 366.0) * (sy - 366.0);
            valid[static_cast<std::size_t>(y) * w + x] = win && d2 >= 24.0 * 24.0;
        }
    auto err = [&](double dx, double dy) {
        double sum = 0;
        long n = 0;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                if (!valid[static_cast<std::size_t>(y) * w + x]) continue;
                const double fx = x - dx, fy = y - dy;
                const int ix = static_cast<int>(std::floor(fx)), iy = static_cast<int>(std::floor(fy));
                if (ix < 0 || iy < 0 || ix + 1 >= w || iy + 1 >= h) continue;
                if (!(valid[static_cast<std::size_t>(iy) * w + ix] && valid[static_cast<std::size_t>(iy) * w + ix + 1] &&
                      valid[static_cast<std::size_t>(iy + 1) * w + ix] && valid[static_cast<std::size_t>(iy + 1) * w + ix + 1])) continue;
                const double ax = fx - ix, ay = fy - iy;
                const double v = sa[static_cast<std::size_t>(iy) * w + ix] * (1 - ax) * (1 - ay) +
                                 sa[static_cast<std::size_t>(iy) * w + ix + 1] * ax * (1 - ay) +
                                 sa[static_cast<std::size_t>(iy + 1) * w + ix] * (1 - ax) * ay +
                                 sa[static_cast<std::size_t>(iy + 1) * w + ix + 1] * ax * ay;
                const double d = v - sb[static_cast<std::size_t>(y) * w + x];
                sum += d * d;
                ++n;
            }
        return n ? sum / static_cast<double>(n) : 1e9;
    };
    double best = 1e18, bx = 0, by = 0;
    for (double dy = -4; dy <= 16; dy += 0.5)
        for (double dx = -16; dx <= 4; dx += 0.5) {
            const double e = err(dx, dy);
            if (e < best) { best = e; bx = dx; by = dy; }
        }
    const double cx = bx, cy = by;
    for (int j = -10; j <= 10; ++j)
        for (int i = -10; i <= 10; ++i) {
            const double dx = cx + 0.05 * i, dy = cy + 0.05 * j;
            const double e = err(dx, dy);
            if (e < best) { best = e; bx = dx; by = dy; }
        }
    dxOut = bx;
    dyOut = by;
}

bool runOracle(Results& r, const fs::path& crops) {
    Ppm town0, town1, race0, race1;
    auto need = [&](const char* n, Ppm& p, int w, int h) {
        if (!readPpm(crops / n, p) || p.w != w || p.h != h) {
            std::fprintf(stderr, "NO_ORACLE: missing/invalid %s in %s (run minimap_oracle_shift.py export)\n", n, crops.c_str());
            return false;
        }
        return true;
    };
    if (!need("town_driving_town.ppm", town0, 250, 125) || !need("town_driving_street_town.ppm", town1, 250, 125) ||
        !need("race_hud_race.ppm", race0, 240, 180) || !need("race_hud_driving_race.ppm", race1, 240, 180)) {
        return false;
    }
    const int tox = 80, toy = 300, rox = 60, roy = 110;
    const mm::VideoMode vm = mm::VideoMode::Ntsc;

    // (a) content shift between the two town frames.  Car deltas come from the oracle manifest car_xyz:
    //     (506.283, 442.23) -> (518.557, 481.538).
    const double cdx = 518.557 - 506.283, cdz = 481.538 - 442.23;
    const mm::ZoomEntry& z0 = mm::kTownZoom[0];
    const double predFx = -cdx * z0.scale / 200.0, predFy = cdz * 0.47 * z0.scale / 200.0;  // field px
    const double predDx = predFx * 2.0;                                                     // shot px
    const double predDy448 = predFy * 2.0 * 447.0 / 448.0;
    const double predDy480 = (predFy * 2.0) * 0.93125;
    measureShift(town0, town1, tox, toy, r.dx, r.dy);
    r.raceDeltaPred[0] = predDx;  // reused below only for info
    r.info.push_back("shift pred_dx=" + std::to_string(predDx) + " pred_dy448=" + std::to_string(predDy448) +
                     " pred_dy480=" + std::to_string(predDy480));
    const bool dxOk = std::fabs(r.dx - predDx) <= 1.5;
    const bool dyOk = std::fabs(r.dy - predDy448) <= 1.0 || std::fabs(r.dy - predDy480) <= 1.0;
    // shift vote: which mapping predicts dy better
    if (std::fabs(r.dy - predDy448) < std::fabs(r.dy - predDy480)) ++r.verdictVotes448; else ++r.verdictVotes480;

    // (b) absolute: the red arrow centroid vs the law-predicted car marker (town_driving).  The arrow's own shape
    //     biases the centroid by a few px, so the check tolerance is 3 shot px; the *mapping* votes compare 448 vs 480.
    for (int f = 0; f < 2; ++f) {
        const Centroid c = redCentroid(f == 0 ? town0 : town1, tox, toy);
        const mm::UiPoint ui = mm::fieldToUi(mm::townCarMarker(0, vm));
        const mm::UiPoint s448 = mm::uiToShot448(ui), s480 = mm::uiToShot480(ui);
        r.townMeas[f] = c.y;
        r.townPred[f] = s448.y;
        r.townResid448[f] = std::hypot(c.x - s448.x, c.y - s448.y);
        r.townResid480[f] = std::hypot(c.x - s480.x, c.y - s480.y);
        if (r.townResid448[f] < r.townResid480[f]) ++r.verdictVotes448; else ++r.verdictVotes480;
        r.info.push_back("town marker frame" + std::to_string(f) + " meas=(" + std::to_string(c.x) + "," + std::to_string(c.y) +
                         ") px=" + std::to_string(c.n) + " pred448=(" + std::to_string(s448.x) + "," + std::to_string(s448.y) +
                         ") pred480=(" + std::to_string(s480.x) + "," + std::to_string(s480.y) + ")");
    }

    // (c) race: oracle car poses (manifest car_xyz [0], [2]) on course 0.
    const double carX[2] = {616.684, 747.502}, carZ[2] = {568.756, 479.340};
    mm::UiPoint pred[2];
    Centroid meas[2];
    for (int f = 0; f < 2; ++f) {
        const mm::FieldPoint fp = mm::placementProject(mm::kRacePlacements[0], static_cast<float>(carX[f]), static_cast<float>(carZ[f]), vm);
        pred[f] = mm::uiToShot448(mm::fieldToUi(fp));
        meas[f] = redCentroid(f == 0 ? race0 : race1, rox, roy);
        r.raceResid[f][0] = meas[f].x - pred[f].x;
        r.raceResid[f][1] = meas[f].y - pred[f].y;
        r.info.push_back("race marker frame" + std::to_string(f) + " meas=(" + std::to_string(meas[f].x) + "," + std::to_string(meas[f].y) +
                         ") px=" + std::to_string(meas[f].n) + " pred=(" + std::to_string(pred[f].x) + "," + std::to_string(pred[f].y) + ")");
    }
    r.raceDeltaMeas[0] = meas[1].x - meas[0].x;
    r.raceDeltaMeas[1] = meas[1].y - meas[0].y;
    r.raceDeltaPred[0] = pred[1].x - pred[0].x;
    r.raceDeltaPred[1] = pred[1].y - pred[0].y;
    r.raceDeltaErr[0] = r.raceDeltaMeas[0] - r.raceDeltaPred[0];
    r.raceDeltaErr[1] = r.raceDeltaMeas[1] - r.raceDeltaPred[1];

    bool townOk = r.townResid448[0] <= 3.0 && r.townResid448[1] <= 3.0;
    bool raceOk = meas[0].n > 0 && meas[1].n > 0;
    for (int f = 0; f < 2; ++f) raceOk = raceOk && std::hypot(r.raceResid[f][0], r.raceResid[f][1]) <= 3.0;
    raceOk = raceOk && std::fabs(r.raceDeltaErr[0]) <= 2.0 && std::fabs(r.raceDeltaErr[1]) <= 2.0;
    r.oracleOk = dxOk && dyOk && townOk && raceOk;
    if (!dxOk) r.info.push_back("FAIL oracle_dx outside +-1.5");
    if (!dyOk) r.info.push_back("FAIL oracle_dy outside +-1.0");
    if (!townOk) r.info.push_back("FAIL town marker residual > 3 px");
    if (!raceOk) r.info.push_back("FAIL race marker residual > 3 px or delta err > 2 px");
    r.hash ^= static_cast<std::uint64_t>(std::llround(r.dx * 1000)) * 7919u ^ static_cast<std::uint64_t>(std::llround(r.dy * 1000));
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    fs::path elfPath;
    if (const char* h = std::getenv("HOME")) elfPath = fs::path(h) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    if (const char* e = std::getenv("RT_ELF")) elfPath = e;
    fs::path crops = "build/w-W2-11/oracle";
    fs::path golden = fs::path(__FILE__).parent_path() / "data/minimap_golden.tsv";
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&]() -> std::string { return i + 1 < argc ? argv[++i] : ""; };
        if (a == "--elf") elfPath = next();
        else if (a == "--oracle-crops") crops = next();
        else if (a == "--golden") golden = next();
        else if (a == "--allow-skip") {}
        else { std::fprintf(stderr, "unknown arg %s\n", a.c_str()); return 2; }
    }
    const auto elf = readFile(elfPath);
    if (elf.size() < 0x136000) {
        std::printf("NO_ELF\n");
        return 0;
    }

    Results pass[2];
    for (int run = 0; run < 2; ++run) {
        Results& r = pass[run];
        runElf(r, elf);
        runGolden(r, golden);
        runLaws(r);
        runRules(r);
        if (!runOracle(r, crops)) return 2;
        Hasher h;
        for (int v : {r.elfPass, r.elfTotal, r.goldPass, r.goldTotal, r.goldLevels, r.carPass, r.carTotal, r.clipPass,
                      r.clipTotal, r.lawPass, r.lawTotal, r.rulePass, r.ruleTotal, r.oracleOk}) h.u64(static_cast<std::uint64_t>(v));
        h.f64(r.dx); h.f64(r.dy);
        for (int f = 0; f < 2; ++f) {
            h.f64(r.townResid448[f]); h.f64(r.townResid480[f]);
            h.f64(r.raceResid[f][0]); h.f64(r.raceResid[f][1]);
        }
        for (const auto& s : r.info) h.str(s);
        h.u64(r.hash);
        r.hash = h.h;
    }
    const Results& r = pass[0];
    const bool det = pass[0].hash == pass[1].hash;
    for (const auto& s : r.info) std::printf("INFO %s\n", s.c_str());
    std::printf("INFO elf_pins=%d/%d golden=%d/%d zoom_levels=%d car_trials=%d/%d closed_form=%d/%d clip_trials=%d/%d rules=%d/%d\n",
                r.elfPass, r.elfTotal, r.goldPass, r.goldTotal, r.goldLevels, r.carPass, r.carTotal, r.lawPass, r.lawTotal,
                r.clipPass, r.clipTotal, r.rulePass, r.ruleTotal);
    std::printf("INFO town marker residual (shot px): 448-map %.2f/%.2f, 480-map %.2f/%.2f; mapping votes 448=%d 480=%d\n",
                r.townResid448[0], r.townResid448[1], r.townResid480[0], r.townResid480[1], r.verdictVotes448, r.verdictVotes480);
    std::printf("INFO race marker residual meas-pred (shot px): frame0 (%.2f,%.2f) frame1 (%.2f,%.2f); frame delta meas (%.2f,%.2f) pred (%.2f,%.2f)\n",
                r.raceResid[0][0], r.raceResid[0][1], r.raceResid[1][0], r.raceResid[1][1], r.raceDeltaMeas[0], r.raceDeltaMeas[1],
                r.raceDeltaPred[0], r.raceDeltaPred[1]);
    const int verdict = r.verdictVotes448 > r.verdictVotes480 ? 448 : 480;
    const bool ok = r.elfPass == r.elfTotal && r.goldPass == r.goldTotal && r.goldTotal >= 30 && r.goldLevels >= 3 &&
                    r.carPass == r.carTotal && r.clipPass == r.clipTotal && r.lawPass == r.lawTotal &&
                    r.rulePass == r.ruleTotal && r.oracleOk && verdict == 448 && det;
    const int clipPct = r.clipTotal ? static_cast<int>(100.0 * r.clipPass / r.clipTotal + 1e-9) : 0;
    const mm::FieldPoint cp = mm::townCarMarker(0, mm::VideoMode::Ntsc);
    std::printf("W2-11 SCORE car_px=(%g,%g) clip_ok=%d%% oracle_dx=%.2f oracle_dy=%.2f mapping_verdict=%d deterministic=%d "
                "elf_pins=%d/%d golden=%d/%d zoom_levels=%d car_poses=%d/%d %s\n",
                static_cast<double>(quant16(cp.x)), static_cast<double>(quant16(cp.y)), clipPct, r.dx, r.dy, verdict, det ? 1 : 0, r.elfPass, r.elfTotal, r.goldPass, r.goldTotal, r.goldLevels,
                r.carPass, r.carTotal, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
