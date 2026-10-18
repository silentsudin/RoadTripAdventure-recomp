// W2-04 terrain height / surface / tile-id.
// Usage: w2_terrain_query_test [--elf PATH] [--allow-skip]
//        (or RT_ELF=PATH). Exit 0 thresholds met, 1 miss, 2 NO_ELF.

#include "gameplay/terrain_query.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr std::uint32_t kElfDelta = 0x1ff000;
constexpr int kTorusPoints = 1000;
constexpr int kCoveragePoints = 10000;
constexpr double kOracleYMax = 0.05;
constexpr double kCoverageMin = 0.99;
constexpr std::uint32_t kSeed = 0x20398u;

struct Pose {
    const char* name;
    float x;
    float z;
    double expectY;
    bool town;  // Peach Town tile-local, else Peach Raceway (COURSE/C00)
};

// oracle-golden-frames.md harness car floats (car+0x90/+0x94/+0x98).
constexpr Pose kPoses[] = {
    {"town_driving", 506.283f, 442.230f, 30.000, true},
    {"town_street", 518.557f, 481.538f, 30.098, true},
    {"race_hud", 616.684f, 568.756f, 1.100, false},
    {"race_driving", 747.502f, 479.340f, 1.100, false},
};

std::uint32_t lcg(std::uint32_t& state) {
    state = state * 1664525u + 1013904223u;
    return state;
}

std::uint32_t le32(const std::vector<std::uint8_t>& elf, std::uint32_t vaddr) {
    const std::size_t off = static_cast<std::size_t>(vaddr - kElfDelta);
    return static_cast<std::uint32_t>(elf[off]) |
           (static_cast<std::uint32_t>(elf[off + 1]) << 8) |
           (static_cast<std::uint32_t>(elf[off + 2]) << 16) |
           (static_cast<std::uint32_t>(elf[off + 3]) << 24);
}

std::vector<std::uint8_t> readAll(const std::filesystem::path& path, bool& ok) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        ok = false;
        return {};
    }
    ok = true;
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

// Independent transcription of 0021c8d0.c:11-18. Not a call into tileIdAt.
int closedFormId(std::int32_t x, std::int32_t z) {
    if (z < 0) z += 0x01ffffff;
    if (x < 0) x += 0x00ffffff;
    const unsigned uz = static_cast<unsigned>(z);
    const unsigned ux = static_cast<unsigned>(x);
    const unsigned b = (uz >> 25) & 1u;
    return static_cast<int>(((uz >> 25) & 6u) * 8u + (((ux >> 24) - b) & 0x0eu) + b);
}

std::uint64_t fnv(std::uint64_t h, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        h ^= static_cast<std::uint8_t>(v >> (8 * i));
        h *= 0x100000001b3ull;
    }
    return h;
}

struct Measure {
    double oracleYErrMax = 1.0e9;
    int tileOk = 0;
    int tileN = kTorusPoints;
    int coverageHits = 0;
    int coverageN = kCoveragePoints;
    int fallbackHits = 0;
    int surfacePins = 0;
    int surfacePinN = 4;
    int elfPins = 0;
    int elfPinN = 0;
    int roundTrip = 0;
    int townBound = 0;
    std::uint64_t hash = 0xcbf29ce484222325ull;
};

int pinElf(const std::vector<std::uint8_t>& elf, Measure& m) {
    struct Word {
        std::uint32_t vaddr;
        std::uint32_t bits;
    };
    // Scale and height bounds, plus the ComputeGridCellIndex immediates.
    const Word words[] = {
        {0x00332bfc, 0x46a3d70a}, {0x00332c0c, 0x46a3d70a},
        {0x00332b08, 0xc61c4000}, {0x00332b0c, 0x461c4000},
        {0x0021c8d4, 0x3c0301ff}, {0x0021c8d8, 0x3463ffff},
        {0x0021c8e4, 0x3c0400ff}, {0x0021c8e8, 0x3484ffff},
        {0x0021c8fc, 0x00052e43}, {0x0021c900, 0x30a20001},
        {0x0021c904, 0x30a50006}, {0x0021c908, 0x00061e03},
        {0x0021c914, 0x3066000e},
    };
    m.elfPinN = static_cast<int>(sizeof(words) / sizeof(words[0]));
    int ok = 0;
    if (elf.size() < 0x00332c10u - kElfDelta) return 0;
    for (const Word& w : words) {
        if (le32(elf, w.vaddr) == w.bits) ++ok;
    }
    return ok;
}

Measure measure(const rt::native::gameplay::TerrainMesh& town,
                const rt::native::gameplay::TerrainMesh& race,
                const std::vector<std::uint8_t>& elf) {
    using rt::native::gameplay::tileIdAt;
    using rt::native::gameplay::tilePlacementOf;
    Measure m;
    m.elfPins = pinElf(elf, m);
    m.hash = fnv(m.hash, static_cast<std::uint64_t>(m.elfPins));

    double errMax = 0.0;
    for (const Pose& pose : kPoses) {
        const auto& mesh = pose.town ? town : race;
        const auto sample = mesh.heightAt(pose.x, pose.z);
        const double err = sample.hit ? std::abs(static_cast<double>(sample.height) - pose.expectY)
                                      : 1.0e6;
        if (err > errMax) errMax = err;
        std::uint32_t bits = 0;
        static_assert(sizeof(bits) == sizeof(sample.height));
        std::memcpy(&bits, &sample.height, sizeof(bits));
        m.hash = fnv(m.hash, bits);
        m.hash = fnv(m.hash, sample.surface);
        m.hash = fnv(m.hash, sample.hit ? 1u : 0u);
        // Peach town spawn/street are surface word 0 (asphalt class 0).
        // Peach Raceway asphalt is 0x2000 (config/game_state.toml).
        const std::uint32_t expectSurface = pose.town ? 0u : 0x2000u;
        if (sample.hit && sample.surface == expectSurface) ++m.surfacePins;
    }
    m.oracleYErrMax = errMax;

    std::uint32_t state = kSeed;
    for (int i = 0; i < kTorusPoints; ++i) {
        const auto x = static_cast<std::int32_t>(lcg(state) & 0x0fffffffu);
        const auto z = static_cast<std::int32_t>(lcg(state) & 0x0fffffffu);
        const int got = tileIdAt(x, z);
        const int want = closedFormId(x, z);
        if (got == want && got >= 0 && got < 64) ++m.tileOk;
        m.hash = fnv(m.hash, static_cast<std::uint64_t>(got));
    }

    for (int id = 0; id < 64; ++id) {
        const auto place = tilePlacementOf(id);
        if (place.id == id && tileIdAt(place.x0, place.z0) == id &&
            tileIdAt(place.x0 + 1, place.z0 + 1) == id) {
            ++m.roundTrip;
        }
    }
    m.hash = fnv(m.hash, static_cast<std::uint64_t>(m.roundTrip));

    // Oracle town poses sit in tile 43 once placed on the Peach origin
    // (scene-tile-map.md: X0=0x0b000000, Z0=0x0a000000).
    const auto peach = tilePlacementOf(43);
    for (const Pose& pose : kPoses) {
        if (!pose.town) continue;
        const auto fx = peach.x0 + static_cast<std::int32_t>(std::llround(
                            static_cast<double>(pose.x) * 20971.51953125));
        const auto fz = peach.z0 + static_cast<std::int32_t>(std::llround(
                            static_cast<double>(pose.z) * 20971.51953125));
        if (tileIdAt(fx, fz) == 43) ++m.townBound;
    }
    m.hash = fnv(m.hash, static_cast<std::uint64_t>(m.townBound));

    state = kSeed ^ 0x9e3779b9u;
    for (int i = 0; i < kCoveragePoints; ++i) {
        const float x = static_cast<float>(lcg(state) >> 8) * (1600.0f / 16777216.0f);
        const float z = static_cast<float>(lcg(state) >> 8) * (1600.0f / 16777216.0f);
        const auto sample = town.heightAt(x, z);
        if (sample.hit && std::isfinite(sample.height)) {
            ++m.coverageHits;
            std::uint32_t bits = 0;
            std::memcpy(&bits, &sample.height, sizeof(bits));
            m.hash = fnv(m.hash, bits);
        } else {
            ++m.fallbackHits;
            m.hash = fnv(m.hash, 0);
        }
    }
    return m;
}

}  // namespace

// ---------------------------------------------------------------------------------------------------------------
// Screen-space terrain-Y law (W2-04r3). Derivation and citations: docs/native-reference/terrain-query.md section 6.
// The law is read from the ELF (preset 0 offset/focal/pitch @0x29ce00..14, 0.47 @0x332c88, 2048 @0x21e73c,
// XYOFFSET y 1936 @0x2425e4, wheel contacts @0x29ca80) and is checked against the live engine camera block
// (cameraBlocks 0x177E6D0, world->clip matrix at +0) captured from the oracle app. The engine-side numbers are
// pinned derived values (448-space px), never game bytes.
namespace screen {

struct V3 {
    double x, y, z;
};
double dot(const V3& a, const V3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

struct Law {
    double off[3] = {0, 0, 0};  // right, up, forward (preset 0)
    double focal = 0, pitch = 0, yScale = 0, centre = 0, xyOffY = 0;
    double rear[2][3] = {};  // car-local (right, up, forward)
    double front[3] = {};    // wheel contact 0 (0x29ca80)
    int pins = 0;
    int pinN = 0;
};

float f32at(const std::vector<std::uint8_t>& elf, std::uint32_t vaddr) {
    const std::uint32_t bits = le32(elf, vaddr);
    float f = 0.f;
    std::memcpy(&f, &bits, sizeof f);
    return f;
}

Law readLaw(const std::vector<std::uint8_t>& elf) {
    Law l;
    struct Pin {
        std::uint32_t vaddr, word;
    };
    const Pin pins[] = {
        {0x0029ce00, 0x00000000}, {0x0029ce04, 0x40000000}, {0x0029ce08, 0xc0e00000}, {0x0029ce10, 0x43fa0000},
        {0x0029ce14, 0x00000380}, {0x00332c88, 0x3ef0a3d7}, {0x0021e73c, 0x3c014500}, {0x002425e4, 0x3c0144f2},
        {0x0029ca90, 0xbf3d70a4}, {0x0029ca98, 0xbf28f5c3}, {0x0029caa0, 0x3f3d70a4}, {0x0029caa8, 0xbf28f5c3},
    };
    l.pinN = static_cast<int>(sizeof pins / sizeof pins[0]);
    if (elf.size() < 0x00332c8cu - kElfDelta) return l;
    for (const Pin& p : pins) {
        if (le32(elf, p.vaddr) == p.word) ++l.pins;
    }
    l.off[0] = f32at(elf, 0x0029ce00);
    l.off[1] = f32at(elf, 0x0029ce04);
    l.off[2] = f32at(elf, 0x0029ce08);
    l.focal = f32at(elf, 0x0029ce10);
    l.pitch = static_cast<double>(le32(elf, 0x0029ce14) & 0xffffu) * 2.0 * 3.14159265358979323846 / 65536.0;
    l.yScale = l.focal * static_cast<double>(f32at(elf, 0x00332c88));
    // lui imm << 16 is the f32 literal (2048.0 / 1936.0)
    auto luiFloat = [&](std::uint32_t vaddr) {
        const std::uint32_t bits = (le32(elf, vaddr) & 0xffffu) << 16;
        float f = 0.f;
        std::memcpy(&f, &bits, sizeof f);
        return static_cast<double>(f);
    };
    l.centre = luiFloat(0x0021e73c);
    l.xyOffY = luiFloat(0x002425e4);
    l.front[0] = f32at(elf, 0x0029ca80);
    l.front[1] = f32at(elf, 0x0029ca84);
    l.front[2] = f32at(elf, 0x0029ca88);
    l.rear[0][0] = f32at(elf, 0x0029ca90);
    l.rear[0][1] = f32at(elf, 0x0029ca94);
    l.rear[0][2] = f32at(elf, 0x0029ca98);
    l.rear[1][0] = f32at(elf, 0x0029caa0);
    l.rear[1][1] = f32at(elf, 0x0029caa4);
    l.rear[1][2] = f32at(elf, 0x0029caa8);
    return l;
}

// Chase camera: car matrix rows (right, up, forward) = car+0x00/+0x10/+0x20, pitched down by `pitch` about
// right; the eye is the preset offset in that pitched frame. Returns the 448-row UI y of a world point.
struct Basis {
    V3 r, u, f;
};
double ui448(const Law& l, const V3& pos, const Basis& car, const V3& p) {
    const double c = std::cos(l.pitch), s = std::sin(l.pitch);
    const V3 f{c * car.f.x - s * car.u.x, c * car.f.y - s * car.u.y, c * car.f.z - s * car.u.z};
    const V3 u{c * car.u.x + s * car.f.x, c * car.u.y + s * car.f.y, c * car.u.z + s * car.f.z};
    const V3 eye{pos.x + l.off[0] * car.r.x + l.off[1] * u.x + l.off[2] * f.x,
                 pos.y + l.off[0] * car.r.y + l.off[1] * u.y + l.off[2] * f.y,
                 pos.z + l.off[0] * car.r.z + l.off[1] * u.z + l.off[2] * f.z};
    const V3 d{p.x - eye.x, p.y - eye.y, p.z - eye.z};
    const double syGs = l.centre - l.yScale * dot(d, u) / dot(d, f);
    return 2.0 * (syGs - l.xyOffY);
}

constexpr double kShotPerUi = 447.0 / 448.0;

struct Pose {
    const char* name;
    bool town;
    float pos[3];
    float basis[3][3];
    double mesh[3];    // pinned terrain height: origin, rear-left, rear-right contact
    double engine[3];  // engine-true 448 UI y of the same three ground points (live camera block)
};

// Static, flat poses. Engine px = row 1 of the live world->clip matrix / row 3, 2*(sy - 1936).
constexpr Pose kStatic[] = {
    {"town_spawn", true, {506.282684f,29.9999714f,442.229797f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.291334,376.410417,376.410416}},  // 
    {"race_grid", false, {457.199951f,1.09996796f,568.799988f}, {{0.0f,0.0f,-0.99999994f},{0.0f,0.99999994f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.294407,376.413826,376.413826}},  // 
    {"town_settled_flat", true, {504.2966f,29.9999714f,450.594208f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.285164,376.403628,376.403627}},  // 
};
// Oracle PNG landmark per static pose: the rear-tyre bottom edge row (continuous shot-row coordinate, the first
// bright row below the dark tyre at x = 523 and 757). It is quantised to one GS field row (2 shot rows) and
// jitters by one row with field parity (town_flat1 reads 377), so it is reported, not gated.
constexpr double kPngTireEdgeRow[] = {378.0, 378.0, 378.0};

// Driving / tilted poses (informational): engine camera block of tick t paired with the car state of tick t-1
// (the camera is built from the previous tick's car, measured: eye error 0.03 vs 0.19 at lag 0). Two tilted
// static dumps (town_*_tilt) plus gas-held consecutive-vblank samples. The slewed camera tilt is not modelled.
constexpr Pose kDriving[] = {
    {"town_street_tilt", true, {518.904968f,30.0984383f,481.101227f}, {{0.590363324f,7.82310963e-07f,-0.80713737f},{-0.120458715f,0.988800764f,-0.0881059915f},{0.798098087f,0.149241284f,0.583751738f}}, {30.2000008,30.0,30.0}, {351.372249,376.445477,376.534867}},  // dump
    {"town_settled_tilt", true, {518.503418f,30.0984383f,480.811401f}, {{0.590363324f,7.82310963e-07f,-0.80713737f},{-0.120458715f,0.988800764f,-0.0881059915f},{0.798098087f,0.149241284f,0.583751738f}}, {30.0,30.0,30.0}, {365.006985,376.445774,376.515229}},  // dump
    {"race_s1_03", false, {637.454712f,1.09996796f,568.333374f}, {{0.0f,0.0f,-1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.304037,376.424323,376.424323}},  // cam vblank 4281, car vblank 4280
    {"race_s1_09", false, {638.343933f,1.09996796f,568.333374f}, {{0.0f,0.0f,-1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.288372,376.406985,376.406985}},  // cam vblank 4287, car vblank 4286
    {"race_s1_15", false, {639.282898f,1.09996796f,568.333374f}, {{0.0f,0.0f,-1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.307586,376.428143,376.428143}},  // cam vblank 4293, car vblank 4292
    {"race_s1_21", false, {640.29657f,1.09996796f,568.333374f}, {{0.0f,0.0f,-1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.289898,376.408663,376.408663}},  // cam vblank 4299, car vblank 4298
    {"race_s1_27", false, {641.385071f,1.09996796f,568.333374f}, {{0.0f,0.0f,-1.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}, {1.10000002,1.10000002,1.10000002}, {358.304872,376.425147,376.425147}},  // cam vblank 4305, car vblank 4304
    {"race_s1_33", false, {642.544556f,1.11646652f,568.333374f}, {{0.000129250475f,-0.0142060854f,-0.999899089f},{-0.00909695216f,0.999857724f,-0.0142066739f},{0.999958634f,0.00909787044f,0.0f}}, {1.10548752,1.1210073,1.10000002}, {359.004371,375.848494,376.953249}},  // cam vblank 4311, car vblank 4310
    {"race_s1_39", false, {643.765869f,1.13949776f,568.333374f}, {{0.000723207253f,-0.0350457765f,-0.999385238f},{-0.0206190012f,0.999172628f,-0.035053242f},{0.999787092f,0.0206316803f,0.0f}}, {1.13637343,1.1518684,1.10000002}, {358.519194,375.374494,377.489024}},  // cam vblank 4317, car vblank 4316
    {"race_s1_45", false, {645.047607f,1.16872787f,568.333374f}, {{0.00109913305f,-0.0434473976f,-0.999054968f},{-0.0252660327f,0.998735607f,-0.0434613042f},{0.999680042f,0.0252899248f,0.0f}}, {1.16878743,1.18426651,1.11993743}, {358.301440,375.281380,377.562816}},  // cam vblank 4323, car vblank 4322
    {"race_s1_51", false, {646.389526f,1.202631f,568.333374f}, {{0.00109715469f,-0.0434474498f,-0.999055028f},{-0.0252205543f,0.998736799f,-0.0434613042f},{0.999681234f,0.0252444036f,0.0f}}, {1.20270151,1.21818906,1.15387328}, {358.281980,374.900409,377.902969}},  // cam vblank 4329, car vblank 4328
    {"race_s1_57", false, {647.791809f,1.23806f,568.333374f}, {{0.00109715469f,-0.0434474498f,-0.999055028f},{-0.0252205543f,0.998736799f,-0.0434613042f},{0.999681234f,0.0252444036f,0.0f}}, {1.2380989,1.25358646,1.18930161}, {358.287066,374.506413,378.305378}},  // cam vblank 4335, car vblank 4334
    {"town_s1_03", true, {504.293915f,29.9999714f,450.605743f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.287969,376.406666,376.406664}},  // cam vblank 4389, car vblank 4388
    {"town_s1_09", true, {504.276184f,29.9999714f,450.680878f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.290100,376.409080,376.409078}},  // cam vblank 4395, car vblank 4394
    {"town_s1_15", true, {504.241943f,29.9999714f,450.825592f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.290600,376.409636,376.409634}},  // cam vblank 4401, car vblank 4400
    {"town_s1_21", true, {504.191254f,29.9999714f,451.039612f}, {{0.97287339f,0.0f,0.231338277f},{0.0f,1.0f,0.0f},{-0.231338277f,0.0f,0.97287339f}}, {30.0,30.0,30.0}, {358.273927,376.391282,376.391280}},  // cam vblank 4407, car vblank 4406
};

struct Result {
    double staticMax = 1.0e9;
    double pngResMax = 0.0;
    double pngResMin = 1.0e9;
    int meshOk = 0;
    int meshN = 0;
    double drivingMax = 0.0;
    double drivingMean = 0.0;
    int drivingN = 0;
    int traceWritten = 0;
    int traceRows = 0;
    double ymapAgree = 1.0e9;
    double carYErr = 0.0;
    int carYN = 0;
    int lawPins = 0;
    int lawPinN = 0;
    std::uint64_t hash = 0xcbf29ce484222325ull;
};

void pointsFor(const Law& l, const Pose& p, V3 pts[3], const rt::native::gameplay::TerrainMesh& mesh, double hh[3]) {
    const V3 pos{p.pos[0], p.pos[1], p.pos[2]};
    const Basis b{{p.basis[0][0], p.basis[0][1], p.basis[0][2]},
                  {p.basis[1][0], p.basis[1][1], p.basis[1][2]},
                  {p.basis[2][0], p.basis[2][1], p.basis[2][2]}};
    for (int k = 0; k < 3; ++k) {
        V3 w = pos;
        if (k > 0) {
            const double ox = l.rear[k - 1][0], oz = l.rear[k - 1][2];
            w.x += ox * b.r.x + oz * b.f.x;
            w.z += ox * b.r.z + oz * b.f.z;
        }
        const auto s = mesh.heightAt(static_cast<float>(w.x), static_cast<float>(w.z));
        hh[k] = s.hit ? static_cast<double>(s.height) : 1.0e6;
        pts[k] = {w.x, hh[k], w.z};
    }
}

bool loadTraceRows(std::vector<std::array<double, 4>>& xyzCol, int& rows) {
    const std::filesystem::path path = std::filesystem::path(__FILE__).parent_path() / "data/vehicle_trace.csv";
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    std::string line;
    std::getline(in, line);
    if (line.find("terrain_y_px") == std::string::npos) return false;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> col;
        std::stringstream ss(line);
        std::string v;
        while (std::getline(ss, v, ',')) col.push_back(v);
        // rows end "...,<vz>,<dial or empty>,<terrain_y_px>": empty dial still yields the field
        if (col.size() < 9) return false;
        ++rows;
        xyzCol.push_back({std::stod(col[2]), std::stod(col[3]), std::stod(col[4]), std::stod(col.back())});
    }
    return true;
}

// ResolveGroundContact (0021b3d0.c: (f0*0x21 + ((f1+f2)/2)*0x22)/0x43): the car height is a weighted mean of the
// terrain heights under the front contact (0,0,0.68) and the two rear contacts, before suspension.
double carYFromContacts(const Law& l, const Pose& p, const rt::native::gameplay::TerrainMesh& mesh) {
    const V3 r{p.basis[0][0], p.basis[0][1], p.basis[0][2]};
    const V3 f{p.basis[2][0], p.basis[2][1], p.basis[2][2]};
    auto h = [&](const double o[3]) {
        const auto s = mesh.heightAt(static_cast<float>(p.pos[0] + o[0] * r.x + o[2] * f.x),
                                     static_cast<float>(p.pos[2] + o[0] * r.z + o[2] * f.z));
        return s.hit ? static_cast<double>(s.height) : 1.0e6;
    };
    const double h0 = h(l.front), h1 = h(l.rear[0]), h2 = h(l.rear[1]);
    return (h0 * 33.0 + (h1 + h2) / 2.0 * 34.0) / 67.0;
}

Result run(const Law& l, const rt::native::gameplay::TerrainMesh& town, const rt::native::gameplay::TerrainMesh& race) {
    Result r;
    r.lawPins = l.pins;
    r.lawPinN = l.pinN;
    double staticMax = 0.0;
    int idx = 0;
    for (const Pose& p : kStatic) {
        V3 pts[3];
        double hh[3];
        pointsFor(l, p, pts, p.town ? town : race, hh);
        const V3 pos{p.pos[0], p.pos[1], p.pos[2]};
        const Basis b{{p.basis[0][0], p.basis[0][1], p.basis[0][2]},
                      {p.basis[1][0], p.basis[1][1], p.basis[1][2]},
                      {p.basis[2][0], p.basis[2][1], p.basis[2][2]}};
        for (int k = 0; k < 3; ++k) {
            ++r.meshN;
            if (std::abs(hh[k] - p.mesh[k]) <= 1.0e-4) ++r.meshOk;
            const double px = ui448(l, pos, b, pts[k]);
            staticMax = std::max(staticMax, std::abs(px - p.engine[k]));
            std::uint64_t bits = 0;
            std::memcpy(&bits, &px, sizeof bits);
            r.hash = fnv(r.hash, bits);
        }
        r.carYErr = std::max(r.carYErr, std::abs(carYFromContacts(l, p, p.town ? town : race) - p.pos[1]));
        ++r.carYN;
        const double rearShot = ui448(l, pos, b, pts[1]) * kShotPerUi;
        const double res = kPngTireEdgeRow[idx] - rearShot;
        r.pngResMax = std::max(r.pngResMax, res);
        r.pngResMin = std::min(r.pngResMin, res);
        ++idx;
    }
    r.staticMax = staticMax;

    double sum = 0.0;
    for (const Pose& p : kDriving) {
        if (std::string(p.name).find("_tilt") != std::string::npos) {
            r.carYErr = std::max(r.carYErr, std::abs(carYFromContacts(l, p, p.town ? town : race) - p.pos[1]));
            ++r.carYN;
        }
        V3 pts[3];
        double hh[3];
        pointsFor(l, p, pts, p.town ? town : race, hh);
        const V3 pos{p.pos[0], p.pos[1], p.pos[2]};
        const Basis b{{p.basis[0][0], p.basis[0][1], p.basis[0][2]},
                      {p.basis[1][0], p.basis[1][1], p.basis[1][2]},
                      {p.basis[2][0], p.basis[2][1], p.basis[2][2]}};
        for (int k = 0; k < 3; ++k) {
            const double e = std::abs(ui448(l, pos, b, pts[k]) - p.engine[k]);
            r.drivingMax = std::max(r.drivingMax, e);
            sum += e;
            ++r.drivingN;
        }
    }
    r.drivingMean = r.drivingN ? sum / r.drivingN : 0.0;

    std::vector<std::array<double, 4>> rows;
    int n = 0;
    if (loadTraceRows(rows, n)) {
        double worst = 0.0;
        int ok = 0;
        const Basis level{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        for (const auto& row : rows) {
            const auto s = race.heightAt(static_cast<float>(row[0]), static_cast<float>(row[2]));
            const double h = s.hit ? static_cast<double>(s.height) : 1.0e6;
            const double px = ui448(l, {row[0], row[1], row[2]}, level, {row[0], h, row[2]});
            const double diff = std::abs(px - row[3]);
            worst = std::max(worst, diff);
            if (diff <= 1.0e-3) ++ok;
            std::uint64_t bits = 0;
            std::memcpy(&bits, &px, sizeof bits);
            r.hash = fnv(r.hash, bits);
        }
        r.traceWritten = ok;
        r.traceRows = n;
        r.ymapAgree = worst;
    }
    return r;
}

}  // namespace screen

int main(int argc, char** argv) {
    bool allowSkip = false;
    std::string elfPath;
    if (const char* env = std::getenv("RT_ELF")) elfPath = env;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--allow-skip") allowSkip = true;
        else if (arg == "--elf" && i + 1 < argc) elfPath = argv[++i];
    }
    if (elfPath.empty()) {
        if (const char* home = std::getenv("HOME")) {
            elfPath = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
        }
    }
    bool elfOk = false;
    const auto elf = readAll(elfPath, elfOk);
    if (!elfOk || elf.size() < 0x100) {
        std::printf("W2-04 NO_ELF path=%s\n", elfPath.c_str());
        return allowSkip ? 0 : 2;
    }

    const std::filesystem::path disc = std::filesystem::path(elfPath).parent_path();
    bool townOk = false;
    bool raceOk = false;
    const auto townBytes = readAll(disc / "FLD" / "223.BIN", townOk);
    const auto raceBytes = readAll(disc / "COURSE" / "C00.BIN", raceOk);
    rt::native::gameplay::TerrainMesh town;
    rt::native::gameplay::TerrainMesh race;
    const bool loaded = townOk && raceOk && town.load(townBytes) && race.load(raceBytes);
    if (!loaded) {
        std::printf("W2-04 SCORE oracle_y_err_max=1 tile_id_ok=0/%d coverage=0 fallback_hits=%d "
                    "deterministic=0 FAIL\n",
                    kTorusPoints, kCoveragePoints);
        return 1;
    }

    const Measure first = measure(town, race, elf);
    const Measure second = measure(town, race, elf);
    const bool deterministic = first.hash == second.hash && first.coverageHits == second.coverageHits &&
                               first.tileOk == second.tileOk && first.fallbackHits == second.fallbackHits;

    const double coverage = static_cast<double>(first.coverageHits) / static_cast<double>(first.coverageN);
    const bool yOk = first.oracleYErrMax <= kOracleYMax;
    const bool tileOk = first.tileOk == first.tileN;
    const bool coverOk = coverage >= kCoverageMin;
    const bool pinsOk = first.surfacePins == first.surfacePinN && first.elfPins == first.elfPinN &&
                        first.roundTrip == 64 && first.townBound == 2;
    const bool pass = yOk && tileOk && coverOk && deterministic && pinsOk;

    std::printf("W2-04 SCORE oracle_y_err_max=%.6f tile_id_ok=%d/%d coverage=%.6f "
                "fallback_hits=%d deterministic=%d surface_pins=%d/%d elf_pins=%d/%d "
                "round_trip=%d/64 town_tile=%d/2 %s\n",
                first.oracleYErrMax, first.tileOk, first.tileN, coverage, first.fallbackHits,
                deterministic ? 1 : 0, first.surfacePins, first.surfacePinN, first.elfPins,
                first.elfPinN, first.roundTrip, first.townBound, pass ? "PASS" : "FAIL");

    // Screen-space section (docs/native-reference/terrain-query.md section 6).
    const screen::Law law = screen::readLaw(elf);
    const screen::Result s1 = screen::run(law, town, race);
    const screen::Result s2 = screen::run(law, town, race);
    const bool sDet = s1.hash == s2.hash && s1.traceWritten == s2.traceWritten && s1.staticMax == s2.staticMax &&
                      s1.drivingMax == s2.drivingMax;
    const bool staticGate = s1.staticMax <= 0.100 && s1.meshOk == s1.meshN && law.pins == law.pinN;
    const bool traceOk = s1.traceRows > 0 && s1.traceWritten == s1.traceRows && s1.ymapAgree <= 0.01;
    const bool screenPass = staticGate && traceOk && sDet;
    std::printf("W2-04 SCORE static_px_err_max=%.6f static_gate=%s terrain_y_px_written=%d "
                "driving_px_err_max=%.6f driving_px_err_mean=%.6f ymap_agreement_max=%.6f deterministic=%d "
                "law_pins=%d/%d mesh_pins=%d/%d trace_rows=%d png_tire_edge_res=%.3f..%.3f png_gated=0 car_y_contact_err=%.6f(n=%d) %s\n",
                s1.staticMax, staticGate ? "PASS" : "FAIL", s1.traceWritten, s1.drivingMax, s1.drivingMean,
                s1.ymapAgree, sDet ? 1 : 0, law.pins, law.pinN, s1.meshOk, s1.meshN, s1.traceRows,
                s1.pngResMin, s1.pngResMax, s1.carYErr, s1.carYN, screenPass ? "PASS" : "FAIL");
    return (pass && screenPass) ? 0 : 1;
}
