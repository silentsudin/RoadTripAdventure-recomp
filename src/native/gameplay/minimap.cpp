// W2-11 minimap projection.  See minimap.h and docs/native-reference/minimap.md.
#include "gameplay/minimap.h"

#include <cmath>

// The handlers run on a MIPS FPU with no fused multiply-add; keep the host from fusing.
#if defined(__clang__)
#pragma clang fp contract(off)
#elif defined(__GNUC__)
#pragma GCC optimize("fp-contract=off")
#endif

namespace rt::native::gameplay::minimap {

namespace {
constexpr float kHalfPi = 1.57079637f;  // 0x3FC90FDB
constexpr float kPi = 3.14159274f;      // 0x40490FDB
}  // namespace

// raceMapPlacements 0x2A4050 (39 x {x, y, scale, rot}); the symbol note says 24, but the table
// runs to the zoom table at 0x2A42C0 (0x270 bytes = 39 entries) and the course byte indexes it.
const std::array<Placement, kRacePlacementCount> kRacePlacements{{
    {-5.0f, 2014.0f, 50.0f, -kHalfPi},  // 0
    {-5.0f, 2014.0f, 50.0f, -kHalfPi},  // 1
    {244.0f, 2086.0f, 50.0f, kHalfPi},  // 2
    {212.0f, 1989.0f, 50.0f, 0.f},  // 3
    {188.0f, 2071.0f, 50.0f, kHalfPi},  // 4
    {176.0f, 2017.0f, 50.0f, 0.f},  // 5
    {92.0f, 2047.0f, 50.0f, kHalfPi},  // 6
    {129.0f, 2011.0f, 50.0f, 0.f},  // 7
    {75.0f, 2048.0f, 50.0f, -kHalfPi},  // 8
    {75.0f, 2048.0f, 50.0f, -kHalfPi},  // 9
    {-35.0f, 2092.0f, 50.0f, kPi},  // 10
    {92.0f, 2047.0f, 50.0f, kHalfPi},  // 11
    {-19.0f, 1996.0f, 50.0f, -kHalfPi},  // 12
    {80.0f, 2043.0f, 50.0f, kHalfPi},  // 13
    {112.0f, 2040.0f, 50.0f, kHalfPi},  // 14
    {122.0f, 2048.0f, 40.0f, -kHalfPi},  // 15
    {122.0f, 2048.0f, 40.0f, -kHalfPi},  // 16
    {125.0f, 2064.0f, 50.0f, -kHalfPi},  // 17
    {320.0f, 2048.0f, 50.0f, 0.f},  // 18
    {260.0f, 2089.0f, 50.0f, kHalfPi},  // 19
    {70.0f, 2090.0f, 70.0f, kHalfPi},  // 20
    {320.0f, 2048.0f, 50.0f, 0.f},  // 21
    {172.0f, 2023.0f, 50.0f, 0.f},  // 22
    {142.0f, 2063.0f, 33.5f, -kHalfPi},  // 23
    {320.0f, 2048.0f, 50.0f, 0.f},  // 24
    {-188.0f, 1910.0f, 105.0f, 0.f},  // 25
    {-299.0f, 1860.0f, 133.5f, -kHalfPi},  // 26
    {320.0f, 2048.0f, 50.0f, 0.f},  // 27
    {102.0f, 2048.0f, 50.0f, 0.f},  // 28
    {320.0f, 2048.0f, 50.0f, 0.f},  // 29
    {320.0f, 2048.0f, 50.0f, 0.f},  // 30
    {320.0f, 2048.0f, 50.0f, 0.f},  // 31
    {320.0f, 2048.0f, 50.0f, 0.f},  // 32
    {320.0f, 2048.0f, 50.0f, 0.f},  // 33
    {320.0f, 2048.0f, 50.0f, 0.f},  // 34
    {320.0f, 2048.0f, 50.0f, 0.f},  // 35
    {320.0f, 2048.0f, 50.0f, 0.f},  // 36
    {81.0f, 2022.0f, 50.0f, -kHalfPi},  // 37
    {98.0f, 2037.0f, 75.0f, kHalfPi},  // 38
}};

// scissoredMapPlacements 0x2A42E0 (5 x {x, y, scale, rot}) up to the colour sets at 0x2A4330.
const std::array<Placement, kSplitPlacementCount> kSplitPlacements{{
    {442.0f, 1910.0f, 105.0f, 0.f},  // 0
    {127.0f, 1910.0f, 105.0f, 0.f},  // 1
    {-188.0f, 1910.0f, 105.0f, 0.f},  // 2
    {284.0f, 2030.0f, 105.0f, 0.f},  // 3
    {-31.0f, 2030.0f, 105.0f, 0.f},  // 4
}};

bool scissorContains(const ScissorRect& r, FieldPoint p) {
    return p.x >= static_cast<float>(r.x0) && p.x < static_cast<float>(r.x1 + 1) &&
           p.y >= static_cast<float>(r.y0) && p.y < static_cast<float>(r.y1 + 1);
}

ViewBlock mapViewBlock(float tx, float ty, float s, VideoMode vm) {
    // 0x21F920: fVar4 = DAT_00332CA8 (0.8), forced to 1.0 when videoMode == 0.
    const float kx = vm == VideoMode::Ntsc ? 1.0f : kPalXScale;
    const float negSquash = -kYSquash;  // DAT_00332CAC
    ViewBlock b;
    b.tx = tx;           // param_4[8]
    b.ty = ty;           // param_4[9]
    b.sx = s * kx;       // param_4[0]
    b.sy = s * negSquash;  // param_4[5]
    return b;
}

const ZoomEntry& townZoom(std::uint8_t mode) { return kTownZoom[mode < kTownZoomCount ? mode : 0]; }

ViewBlock townViewBlock(std::uint8_t zoomMode, float px, float pz, VideoMode vm) {
    const ZoomEntry& z = townZoom(zoomMode);
    float tx, ty;
    if (vm == VideoMode::Ntsc) {  // 0x2425F8..0x242614
        tx = (z.x0 + 1728.0f) - (px * z.scale) / kWorldDivisor;
        ty = (z.y0 + 1936.0f) + ((pz * z.scale) * kYSquash) / kWorldDivisor;
    } else {  // 0x242548..0x242574
        tx = (z.x0 * kPalXScale + 1792.0f) - ((px * z.scale) * kPalXScale) / kWorldDivisor;
        ty = (z.y0 + 1920.0f) + ((pz * z.scale) * kYSquash) / kWorldDivisor;
    }
    return mapViewBlock(tx, ty, z.scale, vm);
}

ViewBlock placementViewBlock(const Placement& p, VideoMode vm) {
    const float tx = vm == VideoMode::Ntsc ? p.x + 1728.0f : p.x * kPalXScale + 1792.0f;
    return mapViewBlock(tx, p.y, p.scale, vm);
}

FieldPoint townProject(std::uint8_t zoomMode, float px, float pz, float wx, float wz, VideoMode vm) {
    const ViewBlock b = townViewBlock(zoomMode, px, pz, vm);
    const XyOffset o = xyOffset(vm);
    FieldPoint f;
    f.x = (b.tx + b.sx * ((wx + kViewOffsetX) / kWorldDivisor)) - o.x;
    f.y = (b.ty + b.sy * ((wz + kViewOffsetZ) / kWorldDivisor)) - o.y;
    return f;
}

FieldPoint townCarMarker(std::uint8_t zoomMode, VideoMode vm) { return townProject(zoomMode, 0.f, 0.f, 0.f, 0.f, vm); }

FieldPoint placementProject(const Placement& pl, float wx, float wz, VideoMode vm) {
    const ViewBlock b = placementViewBlock(pl, vm);
    const XyOffset o = xyOffset(vm);
    const float ax = wx + kViewOffsetX;
    const float az = wz + kViewOffsetZ;
    const float c = std::cos(pl.rot);
    const float s = std::sin(pl.rot);
    const float x1 = ax * c + az * s;   // RotY(rot)
    const float z1 = -ax * s + az * c;
    FieldPoint f;
    f.x = (b.tx + b.sx * (x1 / kWorldDivisor)) - o.x;
    f.y = (b.ty + b.sy * (z1 / kWorldDivisor)) - o.y;
    return f;
}

const Placement& coursePlacement(int course, int splitIndex) {
    if (course == kSplitCourse) {
        const int i = splitIndex < 0 ? 0 : (splitIndex >= kSplitPlacementCount ? kSplitPlacementCount - 1 : splitIndex);
        return kSplitPlacements[static_cast<std::size_t>(i)];
    }
    const int c = course < 0 ? 0 : (course >= kRacePlacementCount ? kRacePlacementCount - 1 : course);
    return kRacePlacements[static_cast<std::size_t>(c)];
}

MapKind classifyCourse(int course, bool splitScreen) {
    if (course == -1) return MapKind::Town;
    if (course == kSplitCourse) return MapKind::Split;
    if (course >= 0 && course < kRacePlacementCount) return splitScreen ? MapKind::Split : MapKind::Race;
    return MapKind::None;
}

ScissorRect scissorFor(MapKind kind, VideoMode vm) {
    switch (kind) {
        case MapKind::Town: return townScissor(vm);
        case MapKind::Split: return kScissorSplit;
        default: return ScissorRect{0, 0x7FF, 0, 0x7FF};  // no packet: the GS default (whole drawing area)
    }
}

bool clipsToScissor(MapKind kind) { return kind == MapKind::Town || kind == MapKind::Split; }

float markerScale(float zoomScale) { return kMarkerBaseScale / zoomScale; }

float headingRad(std::int32_t fixedHeading) {
    return (static_cast<float>(fixedHeading) * 3.14159274f) * 3.0517578e-05f;
}

UiPoint fieldToUi(FieldPoint p) { return {static_cast<double>(p.x), static_cast<double>(p.y) * 2.0}; }
UiPoint uiToShot448(UiPoint p) { return {p.x * 2.0, p.y * 447.0 / 448.0}; }
UiPoint uiToShot480(UiPoint p) { return {p.x * 2.0, (p.y + 16.0) * 0.93125}; }

std::uint32_t blipColourSet(BlipKind kind) {
    switch (kind) {
        case BlipKind::PlayerArrow:
        case BlipKind::FinishedDotRed: return 0x2A4370;
        case BlipKind::Player2Arrow:
        case BlipKind::FinishedDotTeal: return 0x2A4380;
        case BlipKind::RivalDot:
        case BlipKind::TownCarDot: return 0x2A4390;
        case BlipKind::CoinDot:
        case BlipKind::PickupDot: return 0x2A43A0;
    }
    return 0;
}

namespace {

Blip makeBlip(BlipKind kind, int source, float wx, float wz, float modelScale, float yaw, FieldPoint f, bool clip,
              const ScissorRect& r) {
    Blip b;
    b.kind = kind;
    b.source = source;
    b.worldX = wx;
    b.worldZ = wz;
    b.field = f;
    b.modelScale = modelScale;
    b.yaw = yaw;
    b.clipped = clip && !scissorContains(r, f);
    return b;
}

}  // namespace

std::vector<Blip> raceBlips(const Placement& place, VideoMode vm, std::span<const CarBlock> cars, bool twoPlayer) {
    std::vector<Blip> out;
    if (cars.empty()) return out;
    const float ms = markerScale(place.scale);  // 0x242D2C: 50 / entry.scale
    const ScissorRect none{};
    auto at = [&](std::size_t i) { return i < cars.size() ? cars[i] : CarBlock{}; };
    auto push = [&](BlipKind k, int slot, const CarBlock& c, float yaw) {
        out.push_back(makeBlip(k, slot, c.x, c.z, ms, yaw, placementProject(place, c.x, c.z, vm), false, none));
    };
    push(BlipKind::PlayerArrow, 0, cars[0], headingRad(cars[0].heading));
    int first = 1;
    if (twoPlayer) {
        push(BlipKind::Player2Arrow, 1, at(1), headingRad(at(1).heading));
        first = 2;
    }
    for (int i = first; i < kCarSlots && static_cast<std::size_t>(i) < cars.size(); ++i) {
        const CarBlock& c = cars[static_cast<std::size_t>(i)];
        if (c.status != 0 && (c.status & 0x30) == 0) push(BlipKind::RivalDot, i, c, 0.f);
    }
    for (int i = 1; i <= 6 && static_cast<std::size_t>(i) < cars.size(); ++i) {  // iVar3 = 5 .. 0 from slot 1
        const CarBlock& c = cars[static_cast<std::size_t>(i)];
        if ((c.status & 0x30) != 0) {
            push((c.status & 0x40) == 0 ? BlipKind::FinishedDotRed : BlipKind::FinishedDotTeal, i, c, 0.f);
        }
    }
    return out;
}

std::vector<Blip> racePickupBlips(const Placement& place, VideoMode vm, std::span<const PickupPos> p) {
    std::vector<Blip> out;
    const float ms = markerScale(place.scale);
    const ScissorRect none{};
    for (std::size_t i = 0; i < p.size(); ++i) {
        out.push_back(makeBlip(BlipKind::PickupDot, static_cast<int>(i), p[i].x, p[i].z, ms, 0.f,
                               placementProject(place, p[i].x, p[i].z, vm), false, none));
    }
    return out;
}

bool tileSlotLoaded(const TileSlots& s, std::uint32_t tileCode) {
    const std::size_t slot = tileCode & 3u;
    return s.tileId[slot] == tileCode && s.quadrant[slot] >= 0;
}

namespace {
// tileSlotOrigin: nullopt-equivalent via bool.
bool slotOrigin(const TileSlots& s, std::uint32_t tileCode, QuadOrigin& out) {
    const std::int8_t q = s.quadrant[tileCode & 3u];
    if (q < 0 || q >= kQuadrantCount) return false;
    out = kQuadrantOrigins[static_cast<std::size_t>(q)];
    return true;
}
}  // namespace

std::vector<Blip> townBlips(std::uint8_t zoomMode, VideoMode vm, const CarBlock& player, std::span<const TownCar> cars,
                            const TileSlots& tiles) {
    std::vector<Blip> out;
    const ZoomEntry& z = townZoom(zoomMode);
    const float ms = markerScale(z.scale);
    const ScissorRect r = townScissor(vm);
    out.push_back(makeBlip(BlipKind::PlayerArrow, 0, player.x, player.z, ms, headingRad(player.heading),
                           townProject(zoomMode, player.x, player.z, player.x, player.z, vm), true, r));
    const std::size_t n = cars.size() < 23 ? cars.size() : 23;  // iVar7 = 0x16 .. 0
    for (std::size_t k = 0; k < n; ++k) {
        const TownCar& c = cars[k];
        QuadOrigin o{};
        if (c.status == 0 || !tileSlotLoaded(tiles, c.tileCode) || !slotOrigin(tiles, c.tileCode, o)) continue;
        const float wx = c.x + o.x;
        const float wz = c.z + o.z;
        out.push_back(makeBlip(BlipKind::TownCarDot, static_cast<int>(k) + 1, wx, wz, ms, 0.f,
                               townProject(zoomMode, player.x, player.z, wx, wz, vm), true, r));
    }
    return out;
}

bool coinMaskBit(const std::array<std::uint64_t, 2>& mask, int coinId1Based) {
    if (coinId1Based < 1 || coinId1Based > 128) return false;
    const int bit = coinId1Based - 1;
    return ((mask[static_cast<std::size_t>(bit >> 6)] >> (bit & 63)) & 1ull) != 0;
}

std::vector<Blip> townCoinBlips(std::uint8_t zoomMode, VideoMode vm, float px, float pz, std::span<const Coin> coins,
                                const std::array<std::uint64_t, 2>& mask, const TileSlots& tiles) {
    std::vector<Blip> out;
    const ZoomEntry& z = townZoom(zoomMode);
    const float ms = markerScale(z.scale);
    const ScissorRect r = townScissor(vm);
    const std::size_t n = coins.size() < 100 ? coins.size() : 100;
    for (std::size_t i = 0; i < n; ++i) {
        const Coin& c = coins[i];
        QuadOrigin o{};
        if (!coinMaskBit(mask, static_cast<int>(i) + 1) || !tileSlotLoaded(tiles, c.tileCode) ||
            !slotOrigin(tiles, c.tileCode, o)) {
            continue;
        }
        const float wx = c.x + o.x;
        const float wz = c.z + o.z;
        out.push_back(makeBlip(BlipKind::CoinDot, static_cast<int>(i) + 1, wx, wz, ms, 0.f,
                               townProject(zoomMode, px, pz, wx, wz, vm), true, r));
    }
    return out;
}

}  // namespace rt::native::gameplay::minimap
