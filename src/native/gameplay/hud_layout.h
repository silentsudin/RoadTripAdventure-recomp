// Authored HUD layout constants for Road Trip (PS2 SLUS-20398), extracted from
// the shipped ELF (Ghidra session on /SLUS_203.98, ELF file offset = vaddr -
// 0x1ff000). Companion evidence ledger: docs/native-reference/hud-layout.md.
//
// All screen geometry lives in the game's authored 640x448 UI space
// (docs/native-2d.md §1: every UI vertex writer computes, for NTSC,
// X = (x << 4) + 0x6C00 and Y = (y << 3) + 0x7900; a native renderer takes UI
// coordinates as 640x448 pixels and ignores the field/PAL arithmetic). The
// 480-line frame centres that 448-line field, so normalized-to-640x480
// coordinates are (x / 640, (y + kFrameYOffset) / 480); the +16 offset is the
// one INFERRED value in this module (see the ledger) — everything else is
// read straight out of builder decompilations and ROM sprite-record templates.
//
// Element records follow the game's arena sprite layout
// {x, y, u, v, w, h, z, flags} (32 B, docs/native-2d.md §3.2), verified from
// the disassembly of raceHudElementC (0x22F750): sp+0 = x is patched with the
// running digit x, sp+8 = u with digit << 4, sp+0xc = v with the glyph row.
// Race-HUD sprite records are copied from ROM template tables at 0x2EBD00..
// 0x2EC040; the tables below quote those shipped bytes, not re-derivations.
//
// Provenance marks: kConfirmed = builder decompilation + ROM bytes agree;
// kInferred = semantic naming, 2P arrangement, or the 448->480 centring.
// No game assets are embedded here: only coordinates, sizes, atlas cell
// addresses, and palette/colour words.

#pragma once

#include <cstddef>
#include <cstdint>

namespace rt::native::gameplay {

// ---------------------------------------------------------------- frame

inline constexpr float kHudUiWidth = 640.f;   // authored UI space (NTSC)
inline constexpr float kHudUiHeight = 448.f;  // 224-line field, doubled
inline constexpr float kFrameWidth = 640.f;   // display frame
inline constexpr float kFrameHeight = 480.f;  // display frame
// 448-line UI field centred in the 480-line frame (INFERRED centring; the
// 640x448 values themselves are CONFIRMED).
inline constexpr float kFrameYOffset = 16.f;

// ---------------------------------------------------------------- element ids

enum class HudElement : std::uint8_t {
    // Race HUD (raceHudTask 0x22DDD0, arena rebuilt by raceHudDraw 0x233860;
    // atlas = hudTexTable 0x29F5F8 -> SCORE.GSL pages 0x2C36/0x2CB8/0x2D3A).
    RaceTopBanner,          // 0x29F628 pair centred at (320, 20)
    RaceLapTimePanel,       // ElementC: 132x24 time panel (+ LAP badge left)
    RaceLapTimePanelAlt1,   // ElementC 2P defaults (494, 76)
    RaceLapTimePanelAlt2,   // ElementC 2P defaults (494, 100)
    RaceTimeDigits,         // MM:SS.cc inside the panel
    RaceLapBadge,           // "LAP" icon 24x24 left of the panel
    RaceLapCount,           // 8x16 lap digits (or 32x16 flag glyph)
    RacePositionSmall,      // ElementD small: digit + ordinal suffix
    RacePositionLarge,      // ElementD large: panel + 28x30 digits
    RacePositionBanner,     // ElementJ: top-centre banner + 40x48 digits
    RaceProgressPanel,      // ElementG: fixed 112x32 panel at (32, 24)
    RaceStatusIcons,        // ElementH/I icon stack (right edge)
    RaceItemPanel,          // ElementB: 168x88 4x2 item roulette
    RaceStartReady,         // ElementA idx0 "READY" 244x40 at (320, 135)
    RaceStartGo,            // ElementA idx1 86x46 at (320, 92)
    RaceStartBars,          // ElementA idx2/5/6 fill bars at (320, 112)
    RacePlayerBadge,        // per-player 130x32 badge at (490, 20 | 234)

    // Drive HUD (task 0x2490A0 dispatcher, default handler 0x24E1A0;
    // atlas = tex table 0x2A9B08 -> ITEM/METER*.GSL pages 0x237A/0x23FE/0x240C;
    // viewport anchor table 0x2A9AF0: 1P (492, 336), 2P (492, 112)/(492, 336)).
    DriveDial,              // 132x96 gauge face at anchor
    DriveSpeedSegments,     // 4x60 bars inside the dial
    DriveSpeedDigits,       // 20x26 digits, right-aligned in the dial
    DriveGearBar,           // 100x18 bar + palette-swapped gear cell
    DrivePromptIcon,        // 16x16 context icon
    DriveOdometer,          // 16x10 odometer digit

    // Town map / clock (townMapTask 0x242FE8, bezel 0x249208,
    // atlas table 0x2A8FE0).
    TownMinimapFrame,       // 144x120 bezel, bottom-left
    TownMinimapWindow,      // SCISSOR_2 map interior
    TownClockLabel,         // "TIME"
    TownClockText,          // "%2d:%02d" from DAT_00335730
    TownDestinationText,    // "DESTINATION"
    TownLocationText,       // current gs_locations_table name
    TownRallyHeader,        // "+++ Q's Rally +++"

    // Prompt text.
    HintBarPanel,           // uiDrawHintBar 9-slice panel (top right)
    HintBarText,            // per-language hint line
    MessageWindow,          // dialogue frame (dialogue task 0x23B168)
    MessageNextArrow,       // blinking "next" arrow

    // Money readouts (DrawDialogTextLine 0x202D10 plate; the live counter is
    // DAT_0177FDB4, capped like the save money u32 at progress+0x654).
    ShopMoneyPlate,         // DrawScoreCounter 0x22E7C0 at (440, 26)
    ShopMoneyDigits,        // 16x20 digits inside the plate
    BetMoneyCurrent,        // 0x22E7F0 at (440, 338)
    BetMoneyAfter,          // (money - bet) at (440, 386)
};

enum class HudAtlas : std::uint8_t {
    RaceScorePages = 0,   // TEX0 TBP0 0x2C36 / 0x2CB8 / 0x2D3A (SCORE.GSL)
    DriveMeterPages = 1,  // TEX0 TBP0 0x237A / 0x23FE / 0x240C (METER*.GSL)
    UiBatchFontAtlas = 2, // TEX0 0x2013780E65321A40: TBP0 0x1A40, PSMT8 512x512
    TownMapPages = 3,     // tex table 0x2A8FE0 (map bezel family)
};

enum class HudProvenance : std::uint8_t {
    Confirmed = 0,  // builder decompilation + shipped ROM bytes agree
    Inferred = 1,   // semantics, 2P arrangement, or 448->480 centring
};

// Native 640x448 UI-space rectangle (top-left origin).
struct HudRect {
    float x;
    float y;
    float w;
    float h;
};

struct HudElementSpec {
    HudElement id;
    HudRect ui;                 // 640x448 UI space
    HudAtlas atlas;
    HudProvenance provenance;
    const char* note;           // ELF provenance (builder / template address)
};

// ---------------------------------------------------------------- placements
//
// Rects marked "(anchor)" are element origins that further sprites offset
// from; the note column names the builder that emits them. Multi-rect
// elements (dial + digits) list the outer bounds of the shipped sprite set.
inline constexpr HudElementSpec kHudElements[] = {
    // ---- race HUD -------------------------------------------------------
    {HudElement::RaceTopBanner, {239.f, 5.f, 162.f, 30.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "0x29F628 records centred (320,20); flag word 0x8078 set by 0x233F10"},
    {HudElement::RaceLapTimePanel, {60.f, 52.f, 132.f, 24.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementC origin (60,52) from 0x233F10; templates 0x2EBD40..0x2EBD80"},
    {HudElement::RaceLapTimePanelAlt1, {494.f, 76.f, 132.f, 24.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "raceHudInit 0x2321B8 default (494,76); flag-gated"},
    {HudElement::RaceLapTimePanelAlt2, {494.f, 100.f, 132.f, 24.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "raceHudInit 0x2321B8 default (494,100); flag-gated"},
    {HudElement::RaceTimeDigits, {64.f, 56.f, 114.f, 16.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "digits 16x16 at x+{4,18,46,60,88,102}, y+4; separators x+32/x+74"},
    {HudElement::RaceLapBadge, {34.f, 52.f, 24.f, 24.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "template 0x2EBE00 {x-26, y, u=232, v=168, 24x24}"},
    {HudElement::RaceLapCount, {38.f, 57.f, 16.f, 16.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "8x16 digits at (x-18|(-22,-14), y+5); lap==0 -> 32x16 at (x-30, y+5)"},
    {HudElement::RacePositionSmall, {54.f, 88.f, 38.f, 16.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementD small at (40,88): digit 16x16 at +14, suffix 22x16 at +30"},
    {HudElement::RacePositionLarge, {128.f, 88.f, 83.f, 32.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementD large at (128,88): panel 32x32 at +51, 28x30 digits"},
    {HudElement::RacePositionBanner, {258.f, 20.f, 124.f, 20.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementJ banner 124x20 atlas(0,193) at (258,20); rank digits are "
     "rotated 40x48-cell quads centred from (288,40) stepping -30 (untested)"},
    {HudElement::RaceProgressPanel, {32.f, 24.f, 112.f, 32.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementG fixed: template 0x2EBF20; label 32x20 atlas(160,194) at "
     "(98,65); 16x20 digits right-aligned from (82,65); arrow quad 0x2EBF60"},
    {HudElement::RaceStatusIcons, {453.f, 24.f, 163.f, 58.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementH/I: 48x48 (562,24), 56x38 (453,40), 22x20 (512,48), 40x48 "
     "rank digits from (522|576, 24|34)"},
    {HudElement::RaceItemPanel, {0.f, 0.f, 168.f, 88.f},
     HudAtlas::RaceScorePages, HudProvenance::Inferred,
     "ElementB: size CONFIRMED (16+17*8+16 x 88, icons pitch 40x36 from "
     "+8,+8); position = runtime-set element origin (updater untraced), "
     "rect shown at origin (0,0)"},
    {HudElement::RaceStartReady, {198.f, 115.f, 244.f, 40.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementA idx0 recs 0x29F768 {244x40 atlas(0,56)} centred (320,135)"},
    {HudElement::RaceStartGo, {277.f, 69.f, 86.f, 46.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementA idx1 recs 0x29F788 {86x46 atlas(163,96)} centred (320,92)"},
    {HudElement::RaceStartBars, {215.f, 84.f, 210.f, 56.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "ElementA idx2/5/6 bars 210x56/160x24/136x48 centred (320,112)"},
    {HudElement::RacePlayerBadge, {490.f, 20.f, 130.f, 32.f},
     HudAtlas::RaceScorePages, HudProvenance::Confirmed,
     "0x29F668 pair at dx=490, dy=20 (P0) / 234 (P1); group flag word +0x84A0"},

    // ---- drive HUD (anchor (492,336) 1P from table 0x2A9AF0) -------------
    {HudElement::DriveDial, {492.f, 336.f, 132.f, 96.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "0x24E1A0 sprite {0,0,132,96} at 1P anchor (492,336)"},
    {HudElement::DriveSpeedSegments, {526.f, 336.f, 58.f, 60.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "4x60 bars at anchor+(34+6i, 0), atlas u=0x88+6i; count runtime-driven "
     "(shown for 10 bars)"},
    {HudElement::DriveSpeedDigits, {532.f, 385.f, 58.f, 26.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "20x26 digits right-aligned anchor+(78,49), step -19, atlas u=d*20 v=102"},
    {HudElement::DriveGearBar, {499.f, 412.f, 100.f, 18.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "anchor+(7,76); gear overlay same rect, tex1 csa = 15 - gear"},
    {HudElement::DrivePromptIcon, {501.f, 348.f, 16.f, 16.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "anchor+(9,12) (alt y=36); 16x16 atlas(240, 0|24)"},
    {HudElement::DriveOdometer, {502.f, 398.f, 16.f, 10.f},
     HudAtlas::DriveMeterPages, HudProvenance::Confirmed,
     "anchor+(10,62); 16x10 atlas(u=(n+(n>0))*16, v=32)"},

    // ---- town map / clock ------------------------------------------------
    {HudElement::TownMinimapFrame, {30.f, 304.f, 144.f, 120.f},
     HudAtlas::TownMapPages, HudProvenance::Confirmed,
     "bezel 0x249208: caps 16x120 atlas(0,192)/(216,0), 8px tiled middle"},
    {HudElement::TownMinimapWindow, {45.f, 312.f, 113.f, 102.f},
     HudAtlas::TownMapPages, HudProvenance::Confirmed,
     "SCISSOR_2 45..158 x 156..207 half-lines (ROM 0x2A43D8 region)"},
    {HudElement::TownClockLabel, {530.f, 34.f, 64.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawText(530,17) -> y doubled to 34; RAM string \"TIME\" 0x333930"},
    {HudElement::TownClockText, {530.f, 58.f, 80.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "format \"%2d:%02d\" @0x2ECA58; DAT_00335730, 9000 ticks/hour"},
    {HudElement::TownDestinationText, {30.f, 58.f, 180.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawText(30,29) \"DESTINATION\" @0x2ECA48"},
    {HudElement::TownLocationText, {30.f, 82.f, 200.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawText(30,41) gs_locations_table[DAT_00335734*2]"},
    {HudElement::TownRallyHeader, {230.f, 34.f, 180.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawText(230,17) \"+++ Q's Rally +++\" @0x2ECA30"},

    // ---- prompt text ------------------------------------------------------
    {HudElement::HintBarPanel, {418.f, 46.f, 204.f, 48.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawPanel9(430,29,180,12|24): screen (x-12, y*2-12); corners 12x12"},
    {HudElement::HintBarText, {438.f, 58.f, 180.f, 12.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "uiDrawGlyph(438,58); 5 languages via PTR tables 0x29A8E0.."},
    {HudElement::MessageWindow, {32.f, 328.f, 576.f, 104.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "0x2028B0(0x158=344): frame y 344-16..344+88, x 32..608"},
    {HudElement::MessageNextArrow, {304.f, 392.f, 32.f, 24.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "0x2028B0 7th sprite atlas(88,256) at (304, y+48)"},

    // ---- money readouts ---------------------------------------------------
    {HudElement::ShopMoneyPlate, {435.f, 26.f, 154.f, 32.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "DrawScoreCounter 0x22E7C0 -> DrawDialogTextLine(440,26); 9-slice plate "
     "slabs (x-5 w5 | x w96 | x+96 w64 | x+160 w5) -> 435..589"},
    {HudElement::ShopMoneyDigits, {440.f, 32.f, 96.f, 20.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "16x20 digits RIGHT-aligned (0x201FD0 steps x-16 from right edge "
     "x+96=536), y=34-2; cells u=d*16+0x1B8 v=200 (d<4) / u=d*16-0x40 v=220"},
    {HudElement::BetMoneyCurrent, {435.f, 338.f, 154.f, 32.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "0x22E7F0 DrawDialogTextLine(440,0x152)"},
    {HudElement::BetMoneyAfter, {435.f, 386.f, 154.f, 32.f},
     HudAtlas::UiBatchFontAtlas, HudProvenance::Confirmed,
     "0x22E7F0 DrawDialogTextLine(440,0x182) = money - bet"},
};

inline constexpr std::size_t kHudElementCount = sizeof(kHudElements) / sizeof(kHudElements[0]);

// ---------------------------------------------------------------- font slots
//
// Digit/letter cells the HUD references, per atlas. The FONTE text atlas
// itself (16x24 cells, slot = (g/31)*32 + (g%31), g = ascii - 0x21) is owned
// by assets/glyph_atlas.h; only the placement-relevant metrics are repeated
// here (16 px advance grid, uiGlyphWidths 0x29A880 giving 8..16 px per ASCII
// char, 20 px for wide/JP cells, 12 px line pitch on the col*20/row*12 grid).
struct HudGlyphSet {
    HudAtlas atlas;
    std::uint16_t uBase;   // atlas-space u of cell 0 (or of the fixed cell)
    std::uint16_t vBase;   // atlas-space v of the cell row
    std::uint8_t cellW;
    std::uint8_t cellH;
    std::uint8_t uPitch;   // + per next cell (0 = fixed single cell)
    std::uint8_t vPitch;   // + per next row (grid sets)
    std::uint8_t count;    // cells (digits 0-9 when used as a digit set)
    const char* usage;
};

inline constexpr HudGlyphSet kHudGlyphSets[] = {
    // Race/SCORE atlas (entry 0, TBP0 0x2C36) ------------------------------
    {HudAtlas::RaceScorePages, 0, 64, 16, 16, 16, 0, 10,
     "race time digits (ElementC; u = digit*16, v=64) + small position digits"},
    {HudAtlas::RaceScorePages, 160, 48, 16, 16, 0, 0, 2,
     "time separators ':' (160,48) and '.' (176,48)"},
    {HudAtlas::RaceScorePages, 232, 168, 24, 24, 0, 0, 1,
     "\"LAP\" badge icon (template 0x2EBE00)"},
    {HudAtlas::RaceScorePages, 0, 200, 8, 16, 8, 0, 10,
     "lap-count digits 8x16 (u = lap*8, v=200); 32x16 flag glyph at (80,200)"},
    {HudAtlas::RaceScorePages, 32, 31, 22, 16, 22, 0, 4,
     "ordinal suffix icons st/nd/rd/th: u = k*22+32 (k=3 default 'th')"},
    {HudAtlas::RaceScorePages, 160, 105, 32, 32, 0, 0, 1,
     "large-position panel (ElementD big)"},
    {HudAtlas::RaceScorePages, 0, 30, 28, 30, 28, 30, 18,
     "28x30 position digits: u=(d%9)*28, v=(d/9)*30 (grid)"},
    {HudAtlas::RaceScorePages, 0, 56, 40, 48, 40, 48, 24,
     "40x48 big digits: u=(d%6)*40, v=(d/6)*48+56 (grid; Elements E/I/J/F)"},
    {HudAtlas::RaceScorePages, 32, 48, 12, 16, 12, 0, 10,
     "12x16 lap digits in large position display (u = d*12+32, v=48)"},
    {HudAtlas::RaceScorePages, 0, 194, 16, 20, 16, 0, 10,
     "16x20 digits (ElementG money/mileage: u = d*16, v=0xC2)"},
    // Drive/METER atlas (entry 0, TBP0 0x237A) ------------------------------
    {HudAtlas::DriveMeterPages, 0, 102, 20, 26, 20, 0, 10,
     "drive speed digits 20x26 (u = digit*20, v=102)"},
    {HudAtlas::DriveMeterPages, 136, 0, 4, 60, 6, 0, 10,
     "speed segment slivers (u = 0x88 + 6i, v=0)"},
    {HudAtlas::DriveMeterPages, 0, 32, 16, 10, 16, 0, 10,
     "odometer digits 16x10 (u = (n+(n>0))*16, v=32)"},
    {HudAtlas::DriveMeterPages, 240, 0, 16, 16, 0, 24, 2,
     "prompt icons 16x16: (240,0) and (240,24)"},
    // UiBatch font atlas (TBP0 0x1A40, 16 palette banks at CBP 0x1BC0) ------
    {HudAtlas::UiBatchFontAtlas, 440, 200, 16, 20, 16, 20, 20,
     "money/counter digits 16x20, right-aligned: 0-3 at u=d*16+0x1B8 v=200, "
     "4-9 at u=d*16-0x40 v=220 (uiDrawNumber16 0x201FD0/0x201E48)"},
    {HudAtlas::UiBatchFontAtlas, 0, 180, 10, 20, 0, 20, 10,
     "small 10x20 digit set, atlas rows v = 0xB4 + 20*row (uiDrawSmallText)"},
};

inline constexpr std::size_t kHudGlyphSetCount =
    sizeof(kHudGlyphSets) / sizeof(kHudGlyphSets[0]);

// ---------------------------------------------------------------- colours

// Item-roulette flash (ElementB 0x22F338): a 50x50 white sprite (atlas 0,0)
// MODULATE-tinted by uiArenaSetColour(alpha << 24 | rgb). Alpha ramps +0x40
// per frame up to 0x80 while an item lands, then -0x20 per frame to 0.
inline constexpr std::uint32_t kItemFlashRgbTopRow = 0x4334ECu;
inline constexpr std::uint32_t kItemFlashRgbBottomRow = 0x50E800u;
inline constexpr std::uint32_t kItemFlashAlphaMax = 0x80u;
inline constexpr std::uint32_t kItemFlashAlphaStepUp = 0x40u;
inline constexpr std::uint32_t kItemFlashAlphaStepDown = 0x20u;

// Race timer colour variants: ElementC picks its TEX2 palette bank from
// 0x29FCA8[record.bank] = {0, 1, 12, 0}.
inline constexpr std::uint8_t kTimePaletteBanks[4] = {0, 1, 12, 0};

// ElementC saturates the packed MM:SS.cc BCD outside the representable range
// and dims it (the digits keep rendering from the same cells).
inline constexpr std::uint32_t kTimeBcdUnderflow = 0xAAAAAAu;  // frames < 0
inline constexpr std::uint32_t kTimeBcdOverflow = 0x995999u;   // frames >= 360000
inline constexpr std::uint32_t kTimeFramesMax = 360000u;       // 99:59.99 + 1
inline constexpr std::uint32_t kTimeFramesPerSecond = 60u;
inline constexpr std::uint32_t kTimeFramesPerHour = 3600u;     // 0xE10

// Lap sentinel: ElementC word 5 = -1 hides the lap readout entirely.
inline constexpr std::int32_t kLapHidden = -1;
inline constexpr std::int32_t kLapNone = 0;  // draws the 32x16 flag glyph

// Money (DAT_0177FDB4 mirror of progress+0x654) and clock scale.
inline constexpr std::int32_t kMoneyCap = 999999;
inline constexpr std::int32_t kMoneyNewGame = 1000;
inline constexpr std::int32_t kClockTicksPerHour = 9000;   // DAT_00335730
inline constexpr std::int32_t kClockTicksPerMinute = 150;

// Drive-HUD gear palette: the gear cell reuses the bar rect with
// TEX2 CSA = 15 - gear (reverse/gear-0 draws a separate 58x8 sliver).
inline constexpr std::uint8_t kDriveGearBankOffset = 15;

// ---------------------------------------------------------------- helpers

// Maps a 640x448 UI rect into the 640x480 frame (field centred: y += 16),
// then normalizes each component to [0, 1].
constexpr void normalizedFrame640x480(const HudRect& r, float& nx, float& ny,
                                      float& nw, float& nh) {
    nx = r.x / kFrameWidth;
    ny = (r.y + kFrameYOffset) / kFrameHeight;
    nw = r.w / kFrameWidth;
    nh = r.h / kFrameHeight;
}

// Returns the spec for `id`, or nullptr when absent.
const HudElementSpec* findHudElement(HudElement id);

}  // namespace rt::native::gameplay
