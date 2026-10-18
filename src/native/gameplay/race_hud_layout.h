// W4-06: race HUD element layout (1P Quick Race), in the PS2 design space
// (640x448, top-left origin, half-open boxes [x0,x1) x [y0,y1)).
//
// Grounding, in order of authority:
//   1. ELF records and immediates in SLUS_203.98 (vaddr; file offset = vaddr - 0x1FF000),
//      re-read from build/rom/SLUS_203.98 for this module. The checks in
//      tests/native_w4/race_hud_layout_test.cpp re-read them when the ROM is present.
//   2. Pixel measurements on build/scratch/oracle-golden/race_hud_driving.png and
//      race_hud.png (1280x447). Coordinate law: design_x = png_x / 2,
//      design_y = png_y * 448 / 447. Every measured box is listed with its PNG box in
//      kRaceHudElements and in tests/native_w4/data/race_hud_fixture.json.
//   3. Texel ink boxes. Where a box is "predicted", it is the ROM sprite rect cropped to
//      the inked texels of the decoded page: SCORE.GSL unit @0x70 (CSA 0 and CSA 1),
//      SCORE.GSL unit @0x8350, ITEM/METER0.GSL unit @0x70. The decoded pages are scratch
//      files only (build/scratch/hud-art/). No art is embedded here.
//
// Scope:
//   * TOTALTIME banner + total timer, LAPTIME banner + lap timer + lap badge, rank badge
//     (raceHudDraw 0x233860), and the 1P gauge parts (UpdateRaceHud handler 0 @0x2496C8).
//   * The mph numeral law is owned by the coordinator (hud_live.h kSpeed*). It is only
//     mirrored here for parity checks and is never emitted from this module.
//   * Speedometer needle: GAP, the needle law is unresolved. Only the observed geometry
//     is recorded (kRaceHudNeedle*). Nothing in this module draws a needle.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace roadtrip::gameplay {

// ------------------------------------------------------------------ coordinate law

// UI space: native-2d.md §1 (X = (x << 4) + 0x6C00, Y = (y << 3) + 0x7900).
inline constexpr int kRaceHudDesignWidth = 640;
inline constexpr int kRaceHudDesignHeight = 448;
// Oracle goldens: build/scratch/oracle-golden/manifest.json "resolution": [1280, 447].
inline constexpr int kRaceHudGoldenWidth = 1280;
inline constexpr int kRaceHudGoldenHeight = 447;
constexpr double raceHudDesignX(double pngX) { return pngX / 2.0; }
constexpr double raceHudDesignY(double pngY) { return pngY * 448.0 / 447.0; }
// Measurement tolerance (design px). Two effects set it. Linear texture filtering
// (TEX1 0x60, hud-layout.md §1.1) erodes 1-texel fills. race_hud_driving.png also sits one
// PNG row above race_hud.png for every static element (banner y0 19 vs 20, panel 51 vs 52,
// dial 293 vs 294). race_hud.png matches the ROM rows exactly.
inline constexpr float kRaceHudTolerancePx = 2.0f;

struct RaceHudPoint {
    int x, y;
};
struct RaceHudRect {
    int x0, y0, x1, y1;  // design px, half-open
};
struct RaceHudRectF {
    float x0, y0, x1, y1;
};
struct RaceHudPngBox {
    int x0, x1, y0, y1;  // golden PNG px, half-open; all-zero = not measurable on that frame
};
struct RaceHudRgb {
    std::uint8_t r, g, b;
};

// ------------------------------------------------------------------ atlas pages

// hudTexTable 0x29F5F8 (3 x 16 B; q0 = TEX2 template, q1 = TEX0). Both race entries
// decode TFX = DECAL, so texel colour is not modulated by the vertex colour. A native
// "uniform white" draw is therefore exact.
// gauge tex table 0x2A9008 (handler 0 @0x2496C8): entry 0 = TBP0 0x237A PSMT8 256x128
// DECAL; entry 1 = TBP0 0x23FE PSMT4 128x128 DECAL.
enum class RaceHudPage : std::uint8_t {
    Score0,     // 0x29F5F8 entry 0: SYS/SCORE.GSL unit @0x70, 256x256, TBP0 0x2C36 (native: hudArt.raceAtlas = CSA 0, hudArt.raceBanner = CSA 1)
    Score1,     // 0x29F5F8 entry 1: SYS/SCORE.GSL unit @0x8350, 256x256, TBP0 0x2CB8 (lap disc + 8x16 lap digits; NOT decoded by HudArt)
    Meter0,     // 0x2A9008 entry 0: ITEM/METER0.GSL unit @0x70, 256x128 (native: hudArt.speedDial)
    Meter0Sub,  // 0x2A9008 entry 1: ITEM/METER0.GSL unit @0x8550 (needle, hub, boost arc; NOT decoded, never emitted here)
};
struct RaceHudPageSize {
    int width, height;
};
constexpr RaceHudPageSize raceHudPageSize(RaceHudPage page) {
    switch (page) {
        case RaceHudPage::Score0:
        case RaceHudPage::Score1: return {256, 256};
        case RaceHudPage::Meter0: return {256, 128};
        case RaceHudPage::Meter0Sub: return {128, 128};
    }
    return {256, 256};
}

// One arena sprite record ({x,y,u,v,w,h,z,flags}, hud-layout.md grammar) resolved to
// absolute design px. translucent = 1 marks the half-alpha CLUT entries measured on the
// goldens (see kRaceHudCapsuleAlpha).
struct RaceHudSprite {
    RaceHudPage page;
    std::uint8_t csa;
    std::int16_t u, v, w, h;
    std::int16_t x, y;
    std::uint8_t translucent;
};

// ------------------------------------------------------------------ TOTALTIME block

// Banner group {enabled 1, dx 20, dy 20, recs 0x29F628, n 2} at 0x2EC1A8. InitHudConfig
// 0x233F10 copies it to +0x8078. raceHudDraw 0x2338C8..0x23390C draws it with tex entry 0,
// csa 1 (0x2338D4 a2=0 / 0x2338D8 a3=1), offset by (dx,dy). It is not centred.
inline constexpr RaceHudPoint kRaceHudTotalBannerOrigin{20, 20};
// recs 0x29F628 {0,0,u0,v112,32x30} (T medallion) + 0x29F648 {32,12,u32,v81,130x18} (OTALTIME)
inline constexpr std::array<RaceHudSprite, 2> kRaceHudTotalBanner{{
    {RaceHudPage::Score0, 1, 0, 112, 32, 30, 20, 20, 0},
    {RaceHudPage::Score0, 1, 32, 81, 130, 18, 52, 32, 0},
}};
// ElementC 0x22F750 origin (60,52): 0x233F10 `addiu a2,zero,0x3c` @0x233F18 /
// `addiu a3,zero,0x34` @0x233F1C, bank 1 (0x29FCA8[1] = 1), lap -1 (no lap badge).
inline constexpr RaceHudPoint kRaceHudTotalPanelOrigin{60, 52};

// ------------------------------------------------------------------ LAPTIME block

// Per-player group {enabled 0, dx 490, dy 20, recs 0x29F668, n 2} at 0x2EC070. raceHudInit
// 0x2321B8 copies it to +0x84A0; drawn 0x2339C4..0x233A04, tex entry 0, csa 1.
// enabled = 0 at init, so an untraced updater turns it on (hud-layout.md §4 OPEN).
// It is present in both Quick Race goldens. This resolves hud-layout.md's
// "Per-player badge (490,20)" row: that badge is the LAPTIME banner.
inline constexpr RaceHudPoint kRaceHudLapBannerOrigin{490, 20};
// recs 0x29F668 {0,0,u0,v80,32x32} (L medallion) + 0x29F688 {32,12,u32,v159,98x17} (APTIME)
inline constexpr std::array<RaceHudSprite, 2> kRaceHudLapBanner{{
    {RaceHudPage::Score0, 1, 0, 80, 32, 32, 490, 20, 0},
    {RaceHudPage::Score0, 1, 32, 159, 98, 17, 522, 32, 0},
}};
// ElementC P0 slot 0 {enabled 0, x 494, y 52} at 0x2EC088 (raceHudInit). The golden
// LAP capsule measures exactly 494..619 x 52..76 on race_hud.png.
inline constexpr RaceHudPoint kRaceHudLapPanelOrigin{494, 52};

// ------------------------------------------------------------------ ElementC (0x22F750)

// Panel: 18x24 caps + 12 x 8-wide middle tiles, tex entry 0 csa 1 (0x22F7A4..0x22F908).
// Templates: 0x2EBD40 {0,0,u0,v176,18x24}, 0x2EBD60 {114,0,u26,v176,18x24},
// 0x2EBD80 {18,0,u18,v176,8x24} stepped +8 twelve times.
inline constexpr RaceHudSprite kRaceHudPanelCapLeft{RaceHudPage::Score0, 1, 0, 176, 18, 24, 0, 0, 1};
inline constexpr RaceHudSprite kRaceHudPanelCapRight{RaceHudPage::Score0, 1, 26, 176, 18, 24, 114, 0, 1};
inline constexpr RaceHudSprite kRaceHudPanelMiddle{RaceHudPage::Score0, 1, 18, 176, 8, 24, 18, 0, 1};
inline constexpr int kRaceHudPanelMiddleTiles = 12;
inline constexpr int kRaceHudPanelMiddleStep = 8;
inline constexpr int kRaceHudPanelWidth = 132;  // 18 + 12*8 + 18 (hud-layout.md §1.1)
inline constexpr int kRaceHudPanelHeight = 24;
// Panel texels are CLUT colour (32,24,64) on CSA 1 (SCORE.GSL unit @0x70, texel (0,176)).
// The goldens show them half-blended over the scene. A least-squares fit of
// C = a*S + (1-a)*B over 1803 capsule pixel-channels (sky background interpolated from png rows
// 49/78) gives a = 0.509, S = (29.3,25.0,67.7), rms 1.64. That fits the GS ALPHA_1 0x44
// blend with As = 0x40 on these CLUT entries.
inline constexpr float kRaceHudCapsuleAlpha = 0.509f;
inline constexpr RaceHudRgb kRaceHudCapsuleTexel{32, 24, 64};

// Digits: 16x16 cells, u = digit*16, v = 64 (template 0x2EBDE0 v patched to 0x40 at
// 0x22FB6C), csa = 0x29FCA8[bank] (bank table {0,1,12,0}), y + 4. Loop 0x22FB40..0x22FBAC
// starts at x 0x66 and steps -14 / -28 alternately, so the left-to-right slots are:
inline constexpr std::array<int, 6> kRaceHudTimerDigitDx{4, 18, 46, 60, 88, 102};
inline constexpr int kRaceHudTimerDigitDy = 4;
inline constexpr int kRaceHudTimerCell = 16;
inline constexpr int kRaceHudTimerDigitV = 64;
inline constexpr int kRaceHudTimerPairPitch = 14;   // measured: d0->d1 fill left png 136->164 (= 28 png = 14 design)
inline constexpr int kRaceHudTimerGroupPitch = 28;  // 46-18, 88-60
inline constexpr std::array<int, 4> kRaceHudTimerBankCsa{0, 1, 12, 0};  // 0x29FCA8
// Separators: 0x2EBDA0 {32,4,u160,v48,16x16} (') and 0x2EBDC0 {74,4,u176,v48,16x16} (").
inline constexpr RaceHudSprite kRaceHudTimerSepMinutes{RaceHudPage::Score0, 1, 160, 48, 16, 16, 32, 4, 0};
inline constexpr RaceHudSprite kRaceHudTimerSepSeconds{RaceHudPage::Score0, 1, 176, 48, 16, 16, 74, 4, 0};

// Which atlas the timer digits use. The PNG pitch is 28 px, which is 14 design px, and the
// fill is 10 rows inside a 16-row cell. SCORE.GSL v=64 row: 16x16 cells, step 14.
// Not FONTE: 16x24 cells, digit advance 12 (0x29A880). Not METER0: 16x23 cells, step 17.
// The fill colour (201..208, 239..248, 48) is SCORE CSA 1 texel (208,248,48), see
// race-hud-atlas.md §1.
inline constexpr int kRaceHudFonteDigitAdvance = 12;  // u8 table 0x29A880['0'..'9']
inline constexpr int kRaceHudFonteSpaceAdvance = 8;   // 0x29A880[' ']
inline constexpr int kRaceHudFontePeriodAdvance = 12; // 0x29A880['.']
inline constexpr int kRaceHudFonteUpperAAdvance = 15; // 0x29A880['A']
inline constexpr int kRaceHudFonteLowerAAdvance = 14; // 0x29A880['a']

// Lap badge (only when lap != -1): template 0x2EBE00 {-26,0,u232,v168,24x24}, tex entry 1
// csa 8 (0x22FBC4/0x22FBD0). The disc is translucent, with the same material as the panel:
// it reads (45,85,159) on race_hud_driving.png and (77,103,158) on race_hud.png.
inline constexpr RaceHudSprite kRaceHudLapDisc{RaceHudPage::Score1, 8, 232, 168, 24, 24, -26, 0, 1};
// Lap digits: 8x16, u = lap*8, v = 200, tex entry 1, csa 7 (0x22FC54..0x22FD3C).
// 1 digit at (-18,+5); 2 digits at (-22,+5) and (-14,+5); lap 0 = flag 32x16
// (template 0x2EBE20 {-30,5,u80,v200,32x16}).
inline constexpr int kRaceHudLapDigitW = 8;
inline constexpr int kRaceHudLapDigitH = 16;
inline constexpr int kRaceHudLapDigitV = 200;
inline constexpr int kRaceHudLapDigitCsa = 7;
inline constexpr RaceHudPoint kRaceHudLapDigitOne{-18, 5};
inline constexpr RaceHudPoint kRaceHudLapDigitTens{-22, 5};
inline constexpr RaceHudPoint kRaceHudLapDigitOnes{-14, 5};
inline constexpr RaceHudSprite kRaceHudLapFlag{RaceHudPage::Score1, 7, 80, 200, 32, 16, -30, 5, 0};

// ------------------------------------------------------------------ rank badge (ElementD 0x22FE50)

// P0 origin (40,88): raceHudInit `addiu v1,zero,0x28` @0x232388 / `addiu v0,zero,0x58`
// @0x23238C. The large variant applies when total != 0 (the 1P Quick Race goldens show
// "24th" over a split oval with a small "24").
inline constexpr RaceHudPoint kRaceHudRankOrigin{40, 88};
// Panel: template 0x2EBE40 {51,0,u160,v105,32x32}, csa 0.
inline constexpr RaceHudSprite kRaceHudRankPanel{RaceHudPage::Score0, 0, 160, 105, 32, 32, 51, 0, 0};
// Ordinal suffix: 22x16 at +51, u = 32 + 22*k, v = 31, csa 0 (0x22FF34..0x22FFD0).
inline constexpr int kRaceHudRankSuffixDx = 51;
inline constexpr int kRaceHudRankSuffixW = 22;
inline constexpr int kRaceHudRankSuffixH = 16;
inline constexpr int kRaceHudRankSuffixU0 = 32;
inline constexpr int kRaceHudRankSuffixV = 31;
// Place digits: 28x30, u = (d%9)*28, v = (d/9)*30, csa 0. Ones at +23 (also the single
// digit), tens at +0 (0x22FFEC..0x230184). The digits are 1:1 blits. Mask IoU vs the
// golden is 0.720 (drv) / 0.775 (start) at 0 shift, best 0.892 at dy -1. The
// race-hud-atlas.md "scaled" reading came from its 480-line conversion.
inline constexpr int kRaceHudRankDigitW = 28;
inline constexpr int kRaceHudRankDigitH = 30;
inline constexpr int kRaceHudRankDigitOnesDx = 23;
inline constexpr int kRaceHudRankDigitTensDx = 0;
// Field total: 12x16, u = 32 + 12*d, v = 48, csa 1; ones at (+71,+16), tens at (+61,+16)
// (0x23019C..0x2302CC).
inline constexpr int kRaceHudRankTotalW = 12;
inline constexpr int kRaceHudRankTotalH = 16;
inline constexpr int kRaceHudRankTotalU0 = 32;
inline constexpr int kRaceHudRankTotalV = 48;
inline constexpr RaceHudPoint kRaceHudRankTotalOnes{71, 16};
inline constexpr RaceHudPoint kRaceHudRankTotalTens{61, 16};

// Suffix index for a zero-based rank (0x22FF34..0x22FF74): r%10 if r%10 < 3 and r/10 != 1,
// else 3. Cells: 0 "st", 1 "nd", 2 "rd", 3 "th".
constexpr int raceHudOrdinalIndex(int rank0) {
    const int r10 = rank0 % 10;
    return (r10 >= 0 && r10 < 3 && rank0 / 10 != 1) ? r10 : 3;
}

// ------------------------------------------------------------------ 1P gauge (handler 0 @0x2496C8)

// Origin table 0x2A8FF0 (u32 pairs): 1P (484,294).
inline constexpr RaceHudPoint kRaceHudGaugeOrigin{484, 294};
// Dial face: sprite (0,0,u0,v0,140x128), tex entry 0 csa 0 (0x24997C..0x2499C4).
// The golden dial measures 484..624 x 294..422 on race_hud.png. Current main.cpp blits
// (0,0,132,128) at (492,336). (492,336) is the 2P handler-10 anchor (0x2A9AF0).
inline constexpr RaceHudSprite kRaceHudDialFace{RaceHudPage::Meter0, 0, 0, 0, 140, 128, 484, 294, 0};
// Baked into the dial-face texels. These are not separate draws.
inline constexpr RaceHudRect kRaceHudMphBadgePlate{526, 382, 622, 422};  // METER0 texels u42..138 v88..128, colour (50,129,62)
inline constexpr RaceHudRect kRaceHudMphLabel{595, 403, 617, 416};       // METER0 texels u111..133 v109..122 ("mph")
inline constexpr RaceHudRect kRaceHudGearPlate{579, 350, 601, 370};      // METER0 texels u95..117 v56..76, colour (74,95,196)
// Gear glyph: 20x14 at origin + (97,60), u = (g%5)*20 + 140, v = (g/5)*16 + 80, entry 0
// csa 0. Immediates: `addiu v0,zero,0x61` @0x249A08 (x) and `addiu s1,zero,0x3c` @0x2498D0
// (y). g = car+0x1FF (GetVehicleGear 0x2495B8). Cells are R,1,2,3,4 / 5,6.
// Golden check: glyph mask IoU is best at g=4 on race_hud_driving.png (0.809) and at g=1
// on race_hud.png (0.765), so the gear lozenge reads "4" and "1".
inline constexpr RaceHudPoint kRaceHudGearGlyphOffset{97, 60};
inline constexpr int kRaceHudGearGlyphW = 20;
inline constexpr int kRaceHudGearGlyphH = 14;
inline constexpr int kRaceHudGearGlyphU0 = 140;
inline constexpr int kRaceHudGearGlyphV0 = 80;

// mph numeral, mirror of the coordinator law (hud_live.h kSpeed*, gauge-calibration.md).
// It is NOT emitted here. Handler-0 immediates: first x `addiu s2,zero,0x59` @0x2498FC, y
// `addiu v0,zero,0x5e` @0x249AB8, step `addiu s2,s2,-0x11` @0x249AE8. So the ones digit
// lands at origin + (89,94) = (573,388).
inline constexpr RaceHudPoint kRaceHudMphOnes{573, 388};
inline constexpr int kRaceHudMphStep = -17;
inline constexpr int kRaceHudMphCellW = 16;
inline constexpr int kRaceHudMphCellH = 23;
inline constexpr int kRaceHudMphU0 = 140;
inline constexpr int kRaceHudMphCols = 7;
inline constexpr int kRaceHudMphVStep = 23;
inline constexpr RaceHudRect kRaceHudMphBadge{556, 388, 604, 416};

// Boost arc: two rotated 20x60 quads about origin + (70,70) (template 0x327820
// {70,70,u40,v48,20x60}), on the Meter0Sub page. Measured lime fill box on both goldens:
// png x[1204,1236) y[328,380), i.e. design (602,328.7)-(618,380.9). Documented only.
inline constexpr RaceHudPoint kRaceHudBoostArcPivot{554, 364};

// ------------------------------------------------- needle (law RESOLVED W4-11)

// RESOLVED (W4-11, coordinator ROM-verified 2026-10-18, gauge-calibration.md +
// gauge-rpm-source.md): theta_rad = pi*(clamp(rpm,0,10000)/10000 - 0.7), 0 up,
// positive clockwise; input = signed engine RPM at car+0x1D0 (handler 0x249DC0 ->
// CalculateTachometerValue 0x249520 -> div.s 10000.0f 0x332F50 -> add.s -0.7f
// 0x332F54).  No mph->angle mapping exists and none may be introduced.  Drawing
// stays withheld until the native session exposes an rpm feed (gauge_needle.h
// builders are ready); this flag means law-resolved, NOT wired.
inline constexpr bool kRaceHudNeedleLawResolved = true;
// ROM geometry (handler 0): rotated quad about origin + (70,68). `addiu v0,zero,0x46`
// @0x249E54 and `addiu v1,zero,0x44` @0x249E24 give the pivot (554,362). The sprite is
// u3 v53 8x59 (0x249E40/44/48, 0x249E30) on Meter0Sub csa 12. Strip vertices are
// (-4,-59),(4,-59),(-4,0),(4,0). Hub cap: 12x12 u16 v80 at origin + (64,64).
inline constexpr RaceHudPoint kRaceHudNeedlePivot{554, 362};
inline constexpr int kRaceHudNeedleStripLength = 59;
inline constexpr RaceHudPoint kRaceHudNeedleHubCapOrigin{548, 358};
// Measured on the goldens: the needle was isolated from the difference mask of the two
// race goldens, grey texels only, design-space PCA. Angles use 0 = right, + = counter-clockwise.
// The two axes intersect at (554.67,362.58) and miss the ROM pivot by 0.51 / 0.86 px.
inline constexpr double kRaceHudNeedleMeasuredPivotX = 554.67;
inline constexpr double kRaceHudNeedleMeasuredPivotY = 362.58;
inline constexpr double kRaceHudNeedleTipAngleDriving = 104.09;  // race_hud_driving.png (63 mph, gas held)
inline constexpr double kRaceHudNeedleTipAngleStart = -146.11;   // race_hud.png (39 mph, no gas)
inline constexpr double kRaceHudNeedleTipLenDriving = 52.38;     // visible tip distance from (554,362)
inline constexpr double kRaceHudNeedleTipLenStart = 49.09;
// Dial art (outer white ticks, static between the two goldens, from (554,362)):
// numeral 0 tick at -142.9 deg, then 2 = -178.4, 3 = 163.5, 5 = 127.0, 6 = 108.6, 7 = 90.0,
// 8 = 71.6, 9 = 53.3. That is ~18.2 deg per numeral, clockwise, so 0..10 spans ~180 deg.
// The race_hud.png needle rests just below the 0 tick at 39 mph. The needle does not
// track the mph display.
inline constexpr double kRaceHudDialZeroTickDeg = -142.9;
inline constexpr double kRaceHudDialStepDeg = -18.2;

// ------------------------------------------------------------------ measured element table

enum class RaceHudPresence : std::uint8_t {
    Race,             // both race goldens; absent from town/attract/menu goldens
    RaceConditional,  // both race goldens; enable flag is 0 at raceHudInit and set by an untraced updater
    RaceAndTown,      // gauge task: both race goldens and town_driving / town_driving_street
};
enum class RaceHudCheck : std::uint8_t {
    Ink,      // predicted ink box: every edge within kRaceHudTolerancePx of the measured box
    Contain,  // ROM cell box: the measured fill lies inside it (+- tolerance)
    None,     // measured only (rotated quad)
};
struct RaceHudElement {
    const char* id;
    RaceHudPresence presence;
    RaceHudCheck check;
    RaceHudRect rom;            // predicted ink box (Ink) or ROM cell box (Contain), design px
    RaceHudRectF measured;      // union over the two race goldens, design px
    RaceHudPngBox pngDriving;   // race_hud_driving.png
    RaceHudPngBox pngStart;     // race_hud.png
};

// Generated from build/scratch/w4-06/gen_fixture2.py. Colour classes and PNG windows are in
// tests/native_w4/data/race_hud_fixture.json. ELF sources per element are in HANDOFF-W4-06.md.
inline constexpr std::array<RaceHudElement, 23> kRaceHudElements{{
    {"total_banner", RaceHudPresence::Race, RaceHudCheck::Ink, {20, 20, 182, 50}, {20.00f, 19.04f, 182.00f, 50.11f}, {40, 364, 19, 50}, {40, 364, 20, 50}},
    {"total_medallion_fill", RaceHudPresence::Race, RaceHudCheck::Ink, {25, 26, 45, 47}, {26.00f, 26.06f, 44.00f, 46.10f}, {52, 88, 26, 45}, {52, 88, 26, 46}},
    {"total_word_fill", RaceHudPresence::Race, RaceHudCheck::Ink, {49, 33, 179, 47}, {49.00f, 32.07f, 178.00f, 46.10f}, {98, 356, 33, 45}, {98, 356, 32, 46}},
    {"total_panel", RaceHudPresence::Race, RaceHudCheck::Ink, {60, 52, 185, 76}, {60.00f, 51.11f, 185.00f, 76.17f}, {120, 370, 51, 75}, {120, 370, 52, 76}},
    {"total_digits_fill", RaceHudPresence::Race, RaceHudCheck::Contain, {64, 56, 178, 72}, {68.00f, 57.13f, 175.00f, 68.15f}, {136, 350, 57, 68}, {136, 348, 58, 68}},
    {"lap_banner", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {490, 21, 620, 50}, {490.00f, 20.04f, 620.00f, 50.11f}, {980, 1240, 20, 50}, {980, 1240, 20, 50}},
    {"lap_medallion_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {497, 26, 517, 47}, {498.00f, 26.06f, 516.00f, 46.10f}, {996, 1032, 26, 45}, {996, 1032, 26, 46}},
    {"lap_word_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {521, 33, 617, 46}, {521.00f, 33.07f, 616.00f, 46.10f}, {1042, 1232, 33, 45}, {1042, 1232, 34, 46}},
    {"lap_panel", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {494, 52, 619, 76}, {494.00f, 52.12f, 619.00f, 76.17f}, {0, 0, 0, 0}, {988, 1238, 52, 76}},
    {"lap_digits_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Contain, {498, 56, 612, 72}, {501.00f, 57.13f, 608.00f, 68.15f}, {1004, 1216, 57, 68}, {1002, 1214, 58, 68}},
    {"lap_badge_disc", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {468, 52, 492, 76}, {468.00f, 51.11f, 492.00f, 76.17f}, {936, 984, 51, 75}, {936, 984, 52, 76}},
    {"lap_digit_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Contain, {476, 58, 482, 73}, {476.00f, 58.13f, 481.00f, 70.16f}, {952, 962, 58, 70}, {952, 962, 58, 70}},
    {"rank_digits_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {43, 95, 85, 114}, {44.00f, 94.21f, 84.00f, 112.25f}, {88, 168, 94, 112}, {88, 168, 94, 112}},
    {"rank_oval", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {91, 88, 123, 120}, {91.00f, 87.19f, 123.00f, 120.27f}, {182, 246, 87, 119}, {182, 246, 88, 120}},
    {"rank_suffix_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {94, 91, 109, 101}, {95.00f, 90.20f, 108.00f, 100.22f}, {190, 216, 90, 99}, {190, 216, 90, 100}},
    {"rank_total_fill", RaceHudPresence::RaceConditional, RaceHudCheck::Ink, {105, 106, 120, 118}, {106.00f, 106.24f, 120.00f, 116.26f}, {212, 240, 106, 116}, {212, 240, 106, 116}},
    {"dial_face", RaceHudPresence::RaceAndTown, RaceHudCheck::Ink, {484, 294, 624, 422}, {484.00f, 293.66f, 624.00f, 422.94f}, {968, 1248, 293, 421}, {968, 1248, 294, 422}},
    {"mph_badge_plate", RaceHudPresence::RaceAndTown, RaceHudCheck::Ink, {526, 382, 622, 422}, {526.00f, 381.85f, 620.00f, 422.94f}, {1052, 1240, 381, 421}, {1052, 1240, 382, 422}},
    {"mph_digits_fill", RaceHudPresence::RaceAndTown, RaceHudCheck::Contain, {556, 388, 589, 411}, {556.00f, 387.87f, 586.00f, 407.91f}, {1112, 1172, 387, 407}, {1114, 1172, 388, 406}},
    {"mph_label", RaceHudPresence::RaceAndTown, RaceHudCheck::Ink, {595, 403, 617, 416}, {595.00f, 402.90f, 616.00f, 416.93f}, {1190, 1232, 402, 415}, {1190, 1232, 402, 416}},
    {"gear_plate", RaceHudPresence::RaceAndTown, RaceHudCheck::Ink, {579, 350, 601, 370}, {579.00f, 349.78f, 600.00f, 370.83f}, {1158, 1200, 349, 369}, {1158, 1200, 350, 370}},
    {"gear_digit", RaceHudPresence::RaceAndTown, RaceHudCheck::Contain, {581, 354, 601, 368}, {584.00f, 353.79f, 600.50f, 366.82f}, {1168, 1201, 353, 365}, {1174, 1190, 354, 366}},
    {"boost_arc_fill", RaceHudPresence::RaceAndTown, RaceHudCheck::None, {0, 0, 0, 0}, {602.00f, 328.73f, 618.00f, 380.85f}, {1204, 1236, 328, 380}, {1204, 1236, 328, 380}},
}};

// ------------------------------------------------------------------ colour samples

enum class RaceHudGolden : std::uint8_t {
    Driving,  // build/scratch/oracle-golden/race_hud_driving.png (total 3178 frames)
    Start,    // build/scratch/oracle-golden/race_hud.png (total 2447 frames)
};
struct RaceHudColorSample {
    const char* what;
    RaceHudGolden golden;
    int pngX, pngY;
    RaceHudRgb rgb;
};
// Exact golden pixel values (8-bit RGB). Elements marked translucent depend on the scene
// behind them. The others are texel colours after linear filtering.
inline constexpr std::array<RaceHudColorSample, 26> kRaceHudColorSamples{{
    {"total_medallion_yellow", RaceHudGolden::Driving, 54, 28, {208, 248, 48}},
    {"total_medallion_orange", RaceHudGolden::Driving, 62, 42, {232, 176, 16}},
    {"banner_outline_navy", RaceHudGolden::Driving, 68, 20, {8, 8, 48}},
    {"total_word_lavender", RaceHudGolden::Driving, 100, 34, {208, 200, 248}},
    {"total_digit_fill", RaceHudGolden::Driving, 144, 58, {201, 239, 48}},
    {"total_digit_fill", RaceHudGolden::Start, 144, 58, {205, 244, 48}},
    {"total_capsule_translucent", RaceHudGolden::Driving, 238, 52, {81, 105, 158}},
    {"total_capsule_translucent", RaceHudGolden::Start, 214, 52, {76, 102, 158}},
    {"lap_medallion_yellow", RaceHudGolden::Driving, 996, 28, {208, 248, 48}},
    {"lap_word_lavender", RaceHudGolden::Driving, 1042, 34, {208, 200, 248}},
    {"lap_digit_fill", RaceHudGolden::Driving, 1010, 58, {201, 239, 48}},
    {"lap_badge_disc_translucent", RaceHudGolden::Driving, 960, 52, {45, 85, 159}},
    {"lap_badge_disc_translucent", RaceHudGolden::Start, 966, 58, {77, 103, 158}},
    {"lap_badge_digit", RaceHudGolden::Driving, 960, 60, {199, 238, 48}},
    {"rank_digit_pink", RaceHudGolden::Driving, 94, 102, {248, 40, 200}},
    {"rank_oval_lilac", RaceHudGolden::Driving, 226, 92, {176, 120, 168}},
    {"rank_oval_navy", RaceHudGolden::Driving, 234, 96, {8, 8, 48}},
    {"rank_suffix_pink", RaceHudGolden::Driving, 192, 95, {245, 39, 198}},
    {"rank_total_yellow", RaceHudGolden::Start, 232, 112, {201, 240, 48}},
    {"dial_face_green", RaceHudGolden::Driving, 1100, 294, {38, 86, 29}},
    {"mph_badge_green", RaceHudGolden::Driving, 1080, 382, {50, 129, 62}},
    {"gear_plate_blue", RaceHudGolden::Driving, 1162, 352, {74, 95, 196}},
    {"gear_digit_white", RaceHudGolden::Driving, 1170, 354, {247, 250, 249}},
    {"mph_label_blue", RaceHudGolden::Driving, 1196, 407, {1, 95, 183}},
    {"mph_digit_cyan", RaceHudGolden::Driving, 1160, 397, {3, 222, 239}},
    {"boost_arc_lime", RaceHudGolden::Driving, 1210, 352, {199, 223, 79}},
}};

// ------------------------------------------------------------------ pure sprite helpers

// One ElementC timer digit (slot 0..5 left to right) at panel origin `o`.
constexpr RaceHudSprite raceHudTimerDigitSprite(RaceHudPoint o, int slot, int digit, int csa) {
    return {RaceHudPage::Score0, static_cast<std::uint8_t>(csa), static_cast<std::int16_t>(digit * kRaceHudTimerCell),
            static_cast<std::int16_t>(kRaceHudTimerDigitV), static_cast<std::int16_t>(kRaceHudTimerCell),
            static_cast<std::int16_t>(kRaceHudTimerCell),
            static_cast<std::int16_t>(o.x + kRaceHudTimerDigitDx[static_cast<std::size_t>(slot)]),
            static_cast<std::int16_t>(o.y + kRaceHudTimerDigitDy), 0};
}

// Offsets a relative template record by an element origin.
constexpr RaceHudSprite raceHudAt(RaceHudSprite s, RaceHudPoint o) {
    s.x = static_cast<std::int16_t>(s.x + o.x);
    s.y = static_cast<std::int16_t>(s.y + o.y);
    return s;
}

// Gear glyph cell for gear g (0 = R, 1..6).
constexpr RaceHudSprite raceHudGearSprite(int gear) {
    const int g = gear < 0 ? 0 : (gear > 6 ? 6 : gear);
    return {RaceHudPage::Meter0, 0, static_cast<std::int16_t>((g % 5) * kRaceHudGearGlyphW + kRaceHudGearGlyphU0),
            static_cast<std::int16_t>((g / 5) * 16 + kRaceHudGearGlyphV0), static_cast<std::int16_t>(kRaceHudGearGlyphW),
            static_cast<std::int16_t>(kRaceHudGearGlyphH),
            static_cast<std::int16_t>(kRaceHudGaugeOrigin.x + kRaceHudGearGlyphOffset.x),
            static_cast<std::int16_t>(kRaceHudGaugeOrigin.y + kRaceHudGearGlyphOffset.y), 0};
}

static_assert(kRaceHudPanelCapRight.x + kRaceHudPanelCapRight.w == kRaceHudPanelWidth);
static_assert(kRaceHudPanelMiddle.x + kRaceHudPanelMiddleTiles * kRaceHudPanelMiddleStep == kRaceHudPanelCapRight.x);
static_assert(kRaceHudGaugeOrigin.x + 0x59 == kRaceHudMphOnes.x && kRaceHudGaugeOrigin.y + 0x5e == kRaceHudMphOnes.y);
static_assert(kRaceHudGaugeOrigin.x + 70 == kRaceHudNeedlePivot.x && kRaceHudGaugeOrigin.y + 68 == kRaceHudNeedlePivot.y);
static_assert(raceHudOrdinalIndex(23) == 3 && raceHudOrdinalIndex(0) == 0 && raceHudOrdinalIndex(11) == 3 &&
              raceHudOrdinalIndex(21) == 1);

}  // namespace roadtrip::gameplay
