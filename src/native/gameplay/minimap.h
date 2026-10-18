// HUD minimap projection (W2-11): world (x, z) -> minimap field pixels, zoom table,
// scissor windows, car marker placement and blip rules for the town map and the
// race / split-screen course maps.  Pure C++ (no rendering, no globals, deterministic).
//
// Grounding: docs/native-reference/minimap.md.  Handlers (USA SLUS_203.98):
//   gs_map_draw_functions_town      0x242438   town / field-area map view block
//   gs_map_draw_functions_race      0x2427D0   course map view block
//   gs_map_draw_functions_scissored 0x242A88   2-player / mini-game course map
//   mapViewMatrix                   0x21F920   (tx, ty, s) -> view block
//   townMapTask                     0x242FE8   gating, zoom toggle, town blips
//   raceMapMarkers                  0x242D20   race blips
//   townMapCoins 0x241E90, raceMapPickups 0x243398, mapDot 0x243338
// Data: townMapZoomTable 0x2A42C0, raceMapPlacements 0x2A4050, scissoredMapPlacements
// 0x2A42E0, scissor packets 0x2A43F0 / 0x2A4410 / 0x2A4430, view offset vector 0x2A43E0,
// quadrant origins 0x29D080.  All math is single-precision, in the handlers' operation order.
//
// Coordinate spaces:
//   world  = game units (the subject block 0x177ACE0 x / z; oracle manifest car_xyz [0], [2]).
//   view   = the GS primitive space the view block produces (origin = XYOFFSET:
//            NTSC (1728, 1936), PAL (1792, 1920)).
//   field  = view - XYOFFSET; the space SCISSOR_2 and the 640x448 UI use (x px = UI x px,
//            one field line = two UI lines).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::gameplay::minimap {

enum class VideoMode : std::uint8_t { Ntsc = 0, Pal = 1 };  // videoMode 0x333288: 0 = 640x224 fields

struct FieldPoint {
    float x = 0.f, y = 0.f;
};

// ---------------------------------------------------------------- tables

struct ZoomEntry {
    float x0, y0, scale;  // townMapZoomTable 0x2A42C0, 16-byte stride {x0, y0, scale, 0}
};
inline constexpr int kTownZoomCount = 2;
inline constexpr std::array<ZoomEntry, kTownZoomCount> kTownZoom{{
    {302.f, 89.f, 50.f},   // mode 0 "near" @0x2A42C0
    {202.f, 136.f, 25.f},  // mode 1 "far"  @0x2A42D0
}};

struct Placement {
    float x, y, scale, rot;  // {x, y, scale, rotation rad}
};
// raceMapPlacements 0x2A4050: 39 entries (0x2A4050..0x2A42BF), index = course byte (task+0x22).
inline constexpr int kRacePlacementCount = 39;
// scissoredMapPlacements 0x2A42E0: 5 entries (0x2A42E0..0x2A432F), index = split index.
inline constexpr int kSplitPlacementCount = 5;
inline constexpr int kSplitCourse = 0x19;  // course byte that selects the split table (0x242AA8)
extern const std::array<Placement, kRacePlacementCount> kRacePlacements;
extern const std::array<Placement, kSplitPlacementCount> kSplitPlacements;

// vu0 constants.
inline constexpr float kWorldDivisor = 200.f;        // 0x242518 lui 0x4348
inline constexpr float kYSquash = 0.47f;             // 0x332CAC (negated in the view block) / 0x332EFC / 0x332EF8
inline constexpr float kPalXScale = 0.8f;            // 0x332CA8 (videoMode != 0), 0x332EF4, 0x332F14, 0x332F2C
inline constexpr float kViewOffsetX = -800.f;        // 0x2A43E0 (x, y, z, w) = (-800, -200, -800, 1)
inline constexpr float kViewOffsetZ = -800.f;        // (the -200 is depth after RotX and never reaches the screen)
inline constexpr float kMarkerBaseScale = 50.f;      // marker object matrix scale = 50 / zoom scale
inline constexpr float kHeadingToRad = 3.14159274f * 3.0517578e-05f;  // pi * 2^-15 (0x332F38 / 0x332F34)

struct XyOffset {
    float x, y;
};
[[nodiscard]] constexpr XyOffset xyOffset(VideoMode vm) {
    return vm == VideoMode::Ntsc ? XyOffset{1728.f, 1936.f} : XyOffset{1792.f, 1920.f};  // 0x2425D4/0x2425E4, 0x24253C/0x242530
}

// SCISSOR_2 rectangles (SCAX0, SCAX1, SCAY0, SCAY1 inclusive), decoded from the shipped
// vifDirect packets: town NTSC 0x2A43F0, town PAL 0x2A4410, split-screen 0x2A4430.
struct ScissorRect {
    int x0, x1, y0, y1;
};
inline constexpr ScissorRect kScissorTownNtsc{45, 158, 156, 207};
inline constexpr ScissorRect kScissorTownPal{36, 126, 156, 207};
inline constexpr ScissorRect kScissorSplit{48, 208, 68, 200};
[[nodiscard]] constexpr ScissorRect townScissor(VideoMode vm) {
    return vm == VideoMode::Ntsc ? kScissorTownNtsc : kScissorTownPal;
}
// A continuous field point is inside pixel range [x0, x1+1) x [y0, y1+1).
[[nodiscard]] bool scissorContains(const ScissorRect& r, FieldPoint p);

// ---------------------------------------------------------------- view block

// mapViewMatrix 0x21F920 output fields that reach the screen: translation (+0x20, +0x24)
// and the x / y scale of the 8-qword block ([0] = s * (1 | 0.8 PAL), [5] = s * -0.47).
struct ViewBlock {
    float tx, ty, sx, sy;
};
[[nodiscard]] ViewBlock mapViewBlock(float tx, float ty, float s, VideoMode vm);

// Zoom selection: Select toggles bit 0 of the state byte (townMapTask 0x24302C); the byte indexes
// the zoom table directly.  townZoom clamps to the two shipped entries.
[[nodiscard]] constexpr std::uint8_t nextZoomMode(std::uint8_t mode, bool selectPressed) {
    return selectPressed ? static_cast<std::uint8_t>(mode ^ 1u) : mode;
}
[[nodiscard]] const ZoomEntry& townZoom(std::uint8_t mode);

// Town: view block exactly as 0x242438 builds it (subject px, pz).
[[nodiscard]] ViewBlock townViewBlock(std::uint8_t zoomMode, float px, float pz, VideoMode vm);
// Race / split: view block exactly as 0x2427D0 / 0x242A88 build it.
[[nodiscard]] ViewBlock placementViewBlock(const Placement& p, VideoMode vm);

// ---------------------------------------------------------------- projection

// Town map point (north-up): the game's closed form (NTSC)
//   fx = x0 - 4 s + (wx - px) * s / 200,   fy = y0 + 1.88 s - 0.47 * s * (wz - pz) / 200
// computed through the view block (translation + scale * (world + (-800, -800)) / 200).
[[nodiscard]] FieldPoint townProject(std::uint8_t zoomMode, float px, float pz, float wx, float wz, VideoMode vm);
// The player's marker: world == subject, so independent of the subject position.
[[nodiscard]] FieldPoint townCarMarker(std::uint8_t zoomMode, VideoMode vm);

// Course / split map point: rotate (world + (-800, -800)) by RotY(rot), then scale (s/200, -0.47 s/200).
[[nodiscard]] FieldPoint placementProject(const Placement& p, float wx, float wz, VideoMode vm);
// course byte -> placement (the split table when course == 0x19 and a split index is given).
[[nodiscard]] const Placement& coursePlacement(int course, int splitIndex = 0);

enum class MapKind : std::uint8_t { None, Town, Race, Split };
// task+0x22 = -1 -> town map (townMapTask 0x242FE8: drawn only when the byte is 0xFF);
// 0x19 -> split / scissored map; 0..38 -> course map; anything else -> None.
[[nodiscard]] MapKind classifyCourse(int course, bool splitScreen);
[[nodiscard]] ScissorRect scissorFor(MapKind kind, VideoMode vm);
[[nodiscard]] bool clipsToScissor(MapKind kind);  // race (0x2427D0) sends no scissor packet

// Marker model scale (objectMatrix [0] and [10]): 50 / zoom scale.
[[nodiscard]] float markerScale(float zoomScale);
// fixed-point heading (DAT_0177AE24 int) -> RotY angle given to vu0RotMatrixY.
[[nodiscard]] float headingRad(std::int32_t fixedHeading);

// field -> 640x448 UI space (x unchanged, y doubled) and -> the 1280x447 oracle scanout.
struct UiPoint {
    double x, y;
};
[[nodiscard]] UiPoint fieldToUi(FieldPoint p);
// Oracle scanout "448 mapping": shot = (2 * ui_x, ui_y * 447/448).  VERIFIED (docs: mapping_verdict=448).
[[nodiscard]] UiPoint uiToShot448(UiPoint p);
// Refuted "480 mapping" kept only so the verdict stays reproducible: shot_y = (ui_y + 16) * 0.93125.
[[nodiscard]] UiPoint uiToShot480(UiPoint p);

// ---------------------------------------------------------------- blips

enum class BlipKind : std::uint8_t {
    PlayerArrow,    // 0x290870 chain, red 0x2A4370
    Player2Arrow,   // 0x290870 chain, teal 0x2A4380
    RivalDot,       // 0x290940 chain, green half-alpha 0x2A4390
    FinishedDotRed, // status & 0x30 set, bit 0x40 clear: 0x2A4370
    FinishedDotTeal,// status & 0x30 set, bit 0x40 set:   0x2A4380
    TownCarDot,     // town drivers, 0x2A4390
    CoinDot,        // 0x2A43A0 yellow
    PickupDot       // race pickups, 0x2A43A0 yellow
};
[[nodiscard]] std::uint32_t blipColourSet(BlipKind kind);  // vaddr of the colour set the handler uploads

struct Blip {
    BlipKind kind = BlipKind::RivalDot;
    int source = 0;        // slot / coin / pickup index the blip came from
    float worldX = 0.f, worldZ = 0.f;
    FieldPoint field;
    float modelScale = 1.f;   // 50 / zoom scale (objectMatrix [0], [10])
    float yaw = 0.f;          // RotY(heading) of the arrow chain (0 for dots)
    bool clipped = false;     // true when the point is outside the scissor (always false for Race)
};

// Subject / car block as the handlers read it.
struct CarBlock {
    float x = 0.f, z = 0.f;          // +0 / +8 of the car block (0x177ACE0 + slot * 0x270)
    std::int32_t heading = 0;        // fixed heading (0x177AE24 player 1, 0x177B094 player 2)
    std::uint16_t status = 0;        // rival status word (slot base + 0x108 .. = 0x177B058 + (slot-1)*0x270)
};
inline constexpr int kCarSlots = 24;     // race loop bound 0x18

// raceMapMarkers 0x242D20.  `cars[0]` is player 1, `cars[1]` player 2 when twoPlayer.
// Order: P1 arrow, [P2 arrow], rivals (status != 0 && (status & 0x30) == 0) from slot
// (twoPlayer ? 2 : 1) to 23, then slots 1..6 whose status has 0x30 (red, or teal if 0x40).
[[nodiscard]] std::vector<Blip> raceBlips(const Placement& place, VideoMode vm, std::span<const CarBlock> cars,
                                          bool twoPlayer);
// raceMapPickups 0x243398 (yellow dots, x at +0 and z at +8 of 16-byte entries).
struct PickupPos {
    float x, z;
};
[[nodiscard]] std::vector<Blip> racePickupBlips(const Placement& place, VideoMode vm, std::span<const PickupPos> p);

// Quadrant origin of a tile slot (tileSlotOrigin 0x221288 adds tileQuadrantOrigins[quadrant]
// 0x29D080 + 16 * q = {x, 0, z, 0}); quadrant < 0 means "slot empty".
inline constexpr int kQuadrantCount = 6;
struct QuadOrigin {
    float x, z;
};
inline constexpr std::array<QuadOrigin, kQuadrantCount> kQuadrantOrigins{{
    {-800.f, 1600.f}, {800.f, 1600.f}, {-1600.f, 0.f}, {0.f, 0.f}, {1600.f, 0.f}, {-800.f, -1600.f}}};

// Loaded-tile state for tileSlotLoaded 0x221258 / tileSlotOrigin 0x221288.
struct TileSlots {
    std::array<std::uint32_t, 4> tileId{};     // task + 0x60 + 4 * slot
    std::array<std::int8_t, 4> quadrant{-1, -1, -1, -1};  // byte 0x3335B8 + slot (>= 0: loaded)
};
// tileSlotLoaded: ids match and quadrant byte >= 0.
[[nodiscard]] bool tileSlotLoaded(const TileSlots& s, std::uint32_t tileCode);

struct TownCar {
    float x = 0.f, z = 0.f;           // tile-local position (+0 / +8)
    std::uint16_t status = 0;         // psVar6[0] at 0x177B058 + k * 0x270; non-zero = active
    std::uint32_t tileCode = 0;       // u16 at 0x2BE43C + 8 * (byte at +0xAF)
};
// townMapTask 0x242FE8 (course byte == -1): player arrow at the subject, then the 23 town cars
// (status != 0, tile loaded, origin added).  Coins are separate (townCoinBlips).
[[nodiscard]] std::vector<Blip> townBlips(std::uint8_t zoomMode, VideoMode vm, const CarBlock& player,
                                          std::span<const TownCar> cars, const TileSlots& tiles);

struct Coin {
    float x = 0.f, z = 0.f;           // gs_map_coin_table 0x2A3970 entry {x, y, z, tile code}
    std::uint32_t tileCode = 0;
};
// townMapCoins 0x241E90: coin id i (1-based) is drawn when bit (i-1) of the 128-bit mask at
// 0x177FCA8 / 0x177FCB0 is SET (TestFlagFca8Clear == 0; collection clears the bit,
// UpdateMapCoinCollection 0x241BA8), its tile is loaded and the slot origin exists.
[[nodiscard]] std::vector<Blip> townCoinBlips(std::uint8_t zoomMode, VideoMode vm, float px, float pz,
                                              std::span<const Coin> coins, const std::array<std::uint64_t, 2>& mask,
                                              const TileSlots& tiles);
[[nodiscard]] bool coinMaskBit(const std::array<std::uint64_t, 2>& mask, int coinId1Based);

}  // namespace rt::native::gameplay::minimap
