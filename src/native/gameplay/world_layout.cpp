#include "world_layout.h"

namespace rt::native::gameplay {

int gridCellId(FixedPos p) {
    // FUN_0021c8d0 verbatim: negative corrections, then the shift/mask tree.
    std::int32_t x = p.x;
    std::int32_t z = p.z;
    if (z < 0) z += 0x01ffffff;
    if (x < 0) x += 0x00ffffff;
    const unsigned uz = static_cast<unsigned>(z);
    const unsigned ux = static_cast<unsigned>(x);
    const unsigned b = (uz >> 25) & 1u;
    return static_cast<int>(((uz >> 25) & 6u) * 8u + (((ux >> 24) - b) & 0x0eu) + b);
}

TilePlacement tilePlacement(int id) {
    const int A = (id >> 4) & 3;
    const int col = (id >> 1) & 7;
    const int b = id & 1;
    TilePlacement t;
    t.row = (A << 1) | b;                     // 0..7
    t.halfColumn = (col << 1) | b;           // 0..15 (odd rows offset by half)
    t.x0 = t.halfColumn << 24;
    t.z0 = t.row << 25;
    return t;
}

TileLocal tileLocalOf(FixedPos p) {
    TileLocal out;
    out.id = gridCellId(p);
    // FUN_0021ba70 render conversion: X gets the odd-bit-25 half-column shear
    // before masking to the tile span; Z wraps at the tile size directly.
    const std::uint32_t ux = static_cast<std::uint32_t>(p.x);
    const std::uint32_t uz = static_cast<std::uint32_t>(p.z);
    const std::uint32_t sheared = ux + ((uz & 0x02000000u) >> 1);
    out.localX = static_cast<double>(sheared & 0x01ffffffu) /
                 static_cast<double>(kFixedUnitsPerGameUnit);
    out.localZ = static_cast<double>(uz & 0x01ffffffu) /
                 static_cast<double>(kFixedUnitsPerGameUnit);
    return out;
}

FixedPos wrapToras(FixedPos p) {
    // FUN_0021ba70 retains stored positions modulo 0x10000000.
    return {static_cast<std::int32_t>(static_cast<std::uint32_t>(p.x) & 0x0fffffffu),
            static_cast<std::int32_t>(static_cast<std::uint32_t>(p.z) & 0x0fffffffu)};
}

std::array<TilePlacement, 64> allTilePlacements() {
    std::array<TilePlacement, 64> out{};
    for (int id = 0; id < 64; ++id) out[id] = tilePlacement(id);
    return out;
}

} // namespace rt::native::gameplay
