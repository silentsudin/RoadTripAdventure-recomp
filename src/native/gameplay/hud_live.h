// W2-10 hud_live: live-data HUD (speed digits, race timer, lap, position) ->
// ordered draw list in the game's authored 640x448 UI space, plus a small host
// software rasterizer used only for oracle ROI diffs.
//
// Every constant is grounded to build/decomp/fn/*.c (file:line cited at the
// definition in hud_live.cpp) or to the coordinator-verified oracle readings in
// docs/native-reference/gauge-calibration.md.  Gauge digits live in the 448-line
// UI convention (no -16 shift); the menu "(y+16)/480" convention is NOT used.
//
// IMPORTANT (found this pass): the 1P gauge the oracle shows (ring at UI (484,294), 140x128,
// needle, lime boost arc, 16x23 speed digits, 20x14 gear glyph, town clock plate) is drawn by
// dispatcher handler 0 @0x2496C8 (jump table 0x2A8FB0; origin table 0x2A8FF0; tex table
// 0x2A9008 -> ITEM/METER0.GSL), NOT by FUN_0024E1A0.  0x24E1A0 is handler index 10, selected
// for the 2-player layout (UpdateRaceHud 0x2490A0: `uVar3 = 10; if (!twoPlayer) uVar3 = mode`)
// and uses ITEM/METER10.GSL (20x26 digits at v=102).  Handler 0 has no decomp .c file; it was
// transcribed from the MIPS disassembly (capstone) of 0x2496C8..0x249FA0.  Both are provided:
//   buildGaugeHud   = handler 0  (1P, oracle-validated)
//   buildDriveHud2P = 0x24E1A0   (2P layout)
//
// OPEN / GAP items (do not silently resolve):
//   * Host speed units -> displayed mph divisor.  The shipped chain is
//     mph = trunc((|v_f15| * 216.0) / 1.609)  (FUN_00249458/002494e8, floats at
//     0x332F44 / 0x332F48 are both 1.6089999676).  The host physics speed
//     (FrameSnapshot::speed, 0..20000) is a different domain; kDefaultSpeedDivisor
//     (100, the current main.cpp mapping) vs 20000/186 = 107.527 is unresolved and
//     is therefore a PARAMETER (see HudLiveInput::speedDivisor).
//   * Needle: ELF-derived law (no fit): z-rotation = pi * (rpm/10000 - 0.7) rad about the hub
//     (UI (554,362)); rpm = CalculateTachometerValue(car,0,1) clamped to 10000 (engine rpm
//     short @car+0x1D0), NOT speed.  The oracle gives 4 measured needle positions (3
//     independent) but rpm is unknown at every frame, so only the rest position is a
//     weak check; a quantitative fit needs >=4 RPM-known oracle points (deferred).
#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace rt::native::gameplay::hud_live {

// ----------------------------------------------------------------- draw list

// Which ELF texture-table a quad samples (entry index + CSA bank select the page).
enum class Table : std::uint8_t {
    Drive,  // 0x2A9B08: ITEM/METER*.GSL gauge pages (TBP0 0x237A / 0x23FE / 0x240C)
    Race,   // 0x29F5F8: SYS/SCORE.GSL race HUD atlas (TBP0 0x2C36 / 0x2CB8 / 0x2D3A)
};

// Which disc container backs the texture table (host rasterizer selector).
enum class Page : std::uint8_t { None, Meter0, Meter10 };

struct DrawQuad {
    Table table = Table::Drive;
    std::uint8_t texEntry = 0;  // uiArenaSetTexture arg 3 (table entry)
    std::uint8_t csa = 0;       // uiArenaSetTexture arg 4 (CLUT/CSA bank)
    Page page = Page::None;
    std::int16_t u = 0, v = 0, w = 0, h = 0;  // atlas texels
    std::int16_t x = 0, y = 0;                // absolute 640x448 UI space (origin + record offset)
    std::uint8_t tag = 0;                     // uiArenaDataTag count (sprite budget hint)
    // uiArenaRotQuads record: (x,y) is the pivot (absolute UI px); the four strip vertices
    // (TL, TR, BL, BR) are pivot-relative UI px, rotated by angleRad about the pivot with
    // x' = x cos - y sin, y' = x sin + y cos (y down; positive = clockwise on screen).
    bool rotated = false;
    float angleRad = 0.f;
    std::array<float, 8> verts{};

    bool operator==(const DrawQuad&) const = default;
};

// ----------------------------------------------------------------- speed

// Viewport anchors @0x2A9AF0 (u32 pairs) for the 2P handler 0x24E1A0:
// index = twoPlayer + playerIndex.
struct Anchor { int x, y; };
inline constexpr Anchor kDriveAnchors[3] = {{492, 336}, {492, 112}, {492, 336}};
// Handler-0 origin table @0x2A8FF0 (u32 pairs): 1P (484,294); entry 1 (484,70).
inline constexpr Anchor kGaugeOrigins[3] = {{484, 294}, {484, 70}, {484, 294}};

// Shipped km/h-like -> mph divisor (f32 @0x332F44 and @0x332F48, identical).
inline constexpr float kShippedMphDivisor = 1.6089999675750732f;
// Shipped speedometer clamp (FUN_00249458 `0xBA`): applies to CalculateSpeedometerValue
// only; CalculateSpeedometerDigits (the digit path) has no clamp.
inline constexpr int kShippedSpeedometerCap = 186;
// GAP: provisional host divisor (main.cpp:2479 `speed / 100`).
inline constexpr float kDefaultSpeedDivisor = 100.0f;
// GAP: alt reading (host cap 20000 <-> shipped 186 mph).
inline constexpr float kAltSpeedDivisor = 20000.0f / 186.0f;

// Shipped digit path: trunc(raw / 1.609) where raw = |v_f15| * 216.0 (FUN_0021d358).
[[nodiscard]] int shippedMphFromRaw(float raw216);
// Host path: trunc(|hostSpeed| / divisor); trunc matches the shipped (int) cast.
[[nodiscard]] int hostMph(float hostSpeed, float divisor = kDefaultSpeedDivisor);

// IntToPackedBCD 0x24F4A0: 8 nibbles, low nibble = ones.
[[nodiscard]] std::uint32_t packedBcd(int value);

struct SpeedCell { int digit; int dx; };  // dx relative to the element origin
// Digit loop shared by both handlers: always draws >=1 cell (a zero speed draws "0"),
// stops when the remaining BCD is 0, at most 3 cells, right-aligned.  step = 0x13 for
// handler 10 (0x24E1A0, first dx 0x4E) and 0x11 for handler 0 (first dx 0x59).
[[nodiscard]] std::vector<SpeedCell> speedCells(int mph, int firstDx = 0x4e, int step = 0x13);

// FUN_00249520 CalculateTachometerValue(car, 0x10, 1): strips 0..16 from rpm (handler 10).
[[nodiscard]] int tachStripCount(int rpm);

// ---- handler 0 (1P gauge; oracle-validated) -------------------------------------------

// Rotation laws (radians; RotQuads multiplies the stored half-turn value by pi, DAT_00332F84).
inline constexpr float kPi = 3.1415927410125732f;                // @0x332F84
inline constexpr float kNeedleRpmFull = 10000.0f;                // gp-0x7BA0 = @0x332F50
inline constexpr float kNeedleRpmBias = -0.699999988079071f;     // gp-0x7B9C = @0x332F54
inline constexpr float kBoostSweep = 0.2777777910232544f;        // gp-0x7BA4 = @0x332F4C
inline constexpr int kBoostFieldMax = 0x40000;                   // 00249568.c:14-17
inline constexpr int kBoostBlinkLimit = 0x17fff;                 // 0x249788
// Needle z-rotation: pi * (min(rpm,10000)/10000 - 0.7).  0 rad = pointing straight up;
// positive = clockwise.  rpm 0 -> -126 deg, rpm 10000 -> +54 deg (180 deg sweep).
[[nodiscard]] float needleAngleRad(int rpm);
// Lime boost arc rotation: -(field/0x40000) * 0.27778 * pi (<= 50 deg).
[[nodiscard]] float boostArcAngleRad(int field);
// GetRaceTimeRank 0x249670 (town clock ticks -> sun/dusk/moon icon index 0..2).
[[nodiscard]] int daylightPhase(std::int32_t clockTicks);
// FormatRaceTimeBCD 0x2495F0: minutes = ticks / 150; nibbles [min%10][(min/10)%6][0xA][h%10][(h/10)%10].
[[nodiscard]] std::uint32_t clockBcd(std::int32_t clockTicks);

struct GaugeInput {
    float speedRaw = 0.f;          // host speed units (FrameSnapshot::speed)
    float speedDivisor = kDefaultSpeedDivisor;
    int meterField = 0;            // car+0x23C ("boost" meter), clamped to 0x40000
    int rpm = 0;                   // car+0x1D0 (short), clamped to 10000
    int gear = 0;                  // car+0x1FF (byte)
    int transmissionMode = 0;      // GetVehicleTransmissionMode (0/1/2)
    bool showTransmission = false; // UpdateRaceHud param_6 (viewport word bit 12)
    bool townMode = false;         // UpdateRaceHud param_7: scene byte == -1 or 0x22 -> clock plate
    std::int32_t clockTicks = 0;   // param_2+0x14 (town clock; 9000 ticks / hour)
    std::uint32_t frame = 0;       // param_1+8; bit 5 blinks the low-meter arc palette
    int viewport = 0;              // index into kGaugeOrigins
};
// Ordered draw list of handler 0 @0x2496C8 (UpdateRaceHud jump table 0x2A8FB0 [0]).
[[nodiscard]] std::vector<DrawQuad> buildGaugeHud(const GaugeInput& in);

// ---- handler 10 = FUN_0024E1A0 (2P layout, METER10) -----------------------------------

struct HudLiveInput {
    float speedRaw = 0.f;          // host speed units (FrameSnapshot::speed)
    float speedDivisor = kDefaultSpeedDivisor;
    int rpm = 0;                   // 0..10000 (car+0x1D0); drives the 4x60 strips
    int boostGauge = 0;            // CalculateBoostGaugeValue(car, 0xF, 1) 0..15
    int gear = 0;                  // GetVehicleGear
    int transmissionMode = 0;      // GetVehicleTransmissionMode
    bool showTransmission = false; // param_6 (viewport flag bit 12)
    std::uint32_t frame = 0;       // param_1+8 (drives the idle sliver csa)
};

// Ordered draw list of FUN_0024E1A0 (handler 10) for viewport index 0..2.
[[nodiscard]] std::vector<DrawQuad> buildDriveHud2P(const HudLiveInput& in, int viewport = 0);

// ----------------------------------------------------------------- race HUD

// raceHudElementC 0x22F750 timer: 0 / 0x7FFFFFFF -> 0xAAAAAA ("--"), >=360000 ->
// 0x995999 (99'59"99), else 6 BCD nibbles [min10][min1][sec10][sec1][tenth][cs].
[[nodiscard]] std::uint32_t raceTimerBcd(std::int32_t frames);
// Coordinator/oracle finding: the HUD is fed (total-time frames - 1) (2447 -> 00'40"76).
[[nodiscard]] std::int32_t raceHudTimerValue(std::int32_t totalFrames);

struct TimePanelInput {
    int x = 60, y = 52;          // element origin (1P default (60,52) @0x233F10)
    int digitCsa = 1;            // DAT_0029FCA8[bank] ({0,1,12,0})
    std::int32_t timeFrames = 0; // value handed to ElementC (already frames-1 if from total)
    int lap = -1;                // -1 = no lap badge
};
[[nodiscard]] std::vector<DrawQuad> buildRaceTimePanel(const TimePanelInput& in);

struct PositionInput {
    int x = 40, y = 88;  // ElementD origin (small default (40,88); large (128,88))
    int rank = 0;        // zero-based place (param_2[3]); -1 = hidden (small only)
    int total = 0;       // param_2[4]: 0 = small variant, else large variant with "/total"
};
[[nodiscard]] std::vector<DrawQuad> buildRacePosition(const PositionInput& in);

// ----------------------------------------------------------------- needle (measurements)

struct NeedleOraclePoint {
    const char* frame;     // oracle golden name (build/scratch/oracle-golden/<frame>.png)
    int displayedMph;      // read from the digits
    double angleDeg;       // hub->needle in SHOT px, atan2(-dy, dx), 0 = right, +90 = up
    int maskPixels;        // needle mask size
};
// Hub (shot px) least-squares intersection of the 3 distinct needle axes; the ELF pivot
// is UI (554,362) -> shot (1108, 361.2).
inline constexpr double kNeedleHubShotX = 1109.25;
inline constexpr double kNeedleHubShotY = 361.06;
// Measured points; town_driving and town_driving_street are the SAME needle
// (identical masks) so only 3 are independent.  RPM is unknown for every point.
extern const NeedleOraclePoint kNeedleOraclePoints[4];

// Shot is anisotropic (x = 2*uiX, y = uiY): UI-space needle rotation (0 = up, clockwise +)
// implied by a shot-space hub->needle angle.
[[nodiscard]] double shotAngleToNeedleThetaDeg(double shotAngleDeg);
// Inverse of needleAngleRad: rpm implied by a needle rotation (clamped to nothing).
[[nodiscard]] double impliedRpm(double thetaDeg);

// ----------------------------------------------------------------- rasterizer

struct Rgba8Image {
    int width = 0, height = 0;
    std::vector<std::uint8_t> rgba;  // R first
};

// Decoded gauge texture pages for one METER container.  Entry 0 = first unit (PSMT8 +
// CT32); entry 1/2 = following PSMT4 + CT16 units, one image per CSA bank (0..15).
struct GaugeArt {
    Rgba8Image entry0;
    std::array<std::array<Rgba8Image, 16>, 2> entryT4;  // [0] = entry 1, [1] = entry 2
    [[nodiscard]] const Rgba8Image* page(int entry, int csa) const;
};
// Walks the GIF DMA unit chain of a METER*.GSL (disc bytes) and decodes it.
[[nodiscard]] bool decodeGaugeArt(const std::vector<std::uint8_t>& gsl, GaugeArt& out);

struct Canvas {
    int width = 640, height = 448;
    std::vector<std::uint8_t> rgb;  // opaque, init black
    Canvas() : rgb(static_cast<std::size_t>(640) * 448 * 3, 0) {}
};

// Nearest-neighbour straight-alpha blit; rotated quads use inverse mapping.  pages[Page].
void rasterize(const std::vector<DrawQuad>& quads, const GaugeArt* pages[3], Canvas& canvas);

// Oracle "shot" resolve model (1280x447, RGB): row sy shows UI row sy+1 (measured), x is a
// 2x linear upsample (GS bilinear stretch), then a separable Gaussian (sigma 0.5, radius 2,
// reflect) approximating the 4x-SSAA resolve.  sigma picked from {0.5, 0.7, 1.0} against the
// oracle ROI (report); this is a presentation model, not ELF data.
inline constexpr int kShotWidth = 1280;
inline constexpr int kShotHeight = 447;
inline constexpr float kShotSigma = 0.5f;
[[nodiscard]] std::vector<std::uint8_t> toShotSpace(const Canvas& canvas);

}  // namespace rt::native::gameplay::hud_live
