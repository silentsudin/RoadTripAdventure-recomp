#pragma once
// Authentic world geography: fixed-point <-> streaming tile id <-> tile-local
// game units. All constants and formulas are PROVEN against SLUS-203.98
// (docs/native-reference/world-geography.md, instruction-level at 0x21c8d0
// and divisor bytes at 0x332c0c). Pure and deterministic — no disc access.
#include <cstdint>
#include <array>

namespace rt::native::gameplay {

// One tile spans exactly 1 << 25 fixed units on a grid that wraps at
// 0x10000000 (8 rows x 16 half-columns -> 8x8 tiles = the 64-id space).
inline constexpr std::int32_t kTileFixedUnits = 0x02000000;
inline constexpr std::int32_t kHalfTileFixedUnits = 0x01000000;
inline constexpr std::int32_t kTorasWrapUnits = 0x10000000;

// DAT_00332c0c: f32 nearest to 2^25/1600 (fixed units per game unit).
inline constexpr float kFixedUnitsPerGameUnit = 20971.51953125f;

// Game-unit extent of one tile edge.
inline constexpr double kTileGameUnits = 1600.0;

struct FixedPos {
    std::int32_t x; // car record +0xa0 axis
    std::int32_t z; // car record +0xa8 axis
};

struct TilePlacement {
    std::int32_t x0;   // fixed X of tile origin (half-column based)
    std::int32_t z0;   // fixed Z of tile origin
    int row;           // 0..7
    int halfColumn;    // 0..15 (odd rows shear by one half column)
};

struct TileLocal {
    int id;            // streaming tile id 0..63
    double localX;     // game units within the tile, nominally [0,1600)
    double localZ;     // game units within the tile, nominally [0,1600)
};

// FUN_0021c8d0 verbatim semantics: fixed world position -> streaming id.
// Retains the original negative-operand corrections and masks.
int gridCellId(FixedPos p);

// Inverse placement: id -> origin (row = (A<<1)|b, halfColumn = (col<<1)|b).
TilePlacement tilePlacement(int id);

// Fixed position -> tile id + tile-local game units (X carries the odd-row
// half-column shear correction from FUN_0021ba70: +0x01000000 when bit 25).
TileLocal tileLocalOf(FixedPos p);

// Normalise to the stored 8x8-tile torus (FUN_0021ba70 masks & 0x0fffffff).
FixedPos wrapToras(FixedPos p);

// All 64 placements, indexed by id (drives decorative->authentic mapping).
std::array<TilePlacement, 64> allTilePlacements();

} // namespace rt
