#pragma once

#include <algorithm>

// Race HUD handler 0 (0x002499D4..0x00249A74) reads car+0x1FF and selects a
// 20x14 METER0 cell. The PSMT8/CT32 page visibly labels cell 0 as R, then 1-6.
namespace rt::native::gameplay::gear_glyph {

inline constexpr int kGearByteOffset = 0x1ff;
inline constexpr int kFirstGear = 0;
inline constexpr int kLastGear = 9;
inline constexpr int kColumns = 5;
inline constexpr int kCellWidth = 20;
inline constexpr int kCellHeight = 14;
inline constexpr int kAtlasU0 = 140;
inline constexpr int kAtlasV0 = 80;
inline constexpr int kAtlasRowStep = 16;
inline constexpr char kSettledGlyphs[kLastGear + 1] = {'R', '1', '2', '3', '4', '5', '6', '\0', '\0', '\0'};

[[nodiscard]] constexpr int clampGlyphIndex(int gear) {
    return std::clamp(gear, kFirstGear, kLastGear);
}
[[nodiscard]] constexpr bool isReverse(int gear) { return clampGlyphIndex(gear) == 0; }
[[nodiscard]] constexpr int atlasU(int gear) { return kAtlasU0 + (clampGlyphIndex(gear) % kColumns) * kCellWidth; }
[[nodiscard]] constexpr int atlasV(int gear) { return kAtlasV0 + (clampGlyphIndex(gear) / kColumns) * kAtlasRowStep; }

}  // namespace rt::native::gameplay::gear_glyph
