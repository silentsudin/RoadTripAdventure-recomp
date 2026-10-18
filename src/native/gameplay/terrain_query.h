#pragma once

// Height, surface word, and streaming-tile id at a car (x, z).
//
// Tile id is FUN_0021c8d0 (build/decomp/fn/0021c8d0.c:11-18) over the fixed-point
// words at car+0xa0 / car+0xa8. Height and surface are IntersectPointWithMesh
// (build/decomp/fn/00206f28.c) over the PMP triangle strips in a loaded FLD or
// COURSE container — the mesh FindWorldMeshIntersection indexes on a 100-unit
// grid (build/decomp/fn/00207280.c:71). Pure: no GL, no disc I/O.

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::gameplay {

// DAT_00332bfc and DAT_00332c0c, both f32(2^25/1600). Ground contact divides
// fixed Y/X/Z by this (0021b3d0.c:55,64,69); the master tick writes the render
// floats the same way (0021ba70.c:423-426).
inline constexpr float kFixedUnitsPerGameUnit = 20971.51953125f;

// One tile is 1<<25 fixed units; the car stores the 8x8 torus with & 0x0fffffff
// (world-geography.md, 0021ba70.c:398-400).
inline constexpr std::int32_t kTileFixedUnits = 0x02000000;
inline constexpr std::uint32_t kTorusMask = 0x0fffffffu;
inline constexpr float kTileGameUnits = 1600.0f;

// FindWorldMeshIntersection / TestMeshIntersection cell size and grid.
// 00207280.c:71, 002084d0.c:21: ((int)(x/100) & 0xf) + ((int)(z/100) & 0xf) * 16.
inline constexpr float kCollisionCellUnits = 100.0f;
inline constexpr int kCollisionGrid = 16;

// Initial floor/ceiling for a mesh probe. DAT_00332b08 / DAT_00332b0c
// (00207280.c:102-104). A probe at +bound asks for the highest ground below
// the sky; a miss leaves the probe Y unchanged (00207280.c:109-116).
inline constexpr float kMeshHeightBound = 10000.0f;

// IntersectPointWithMesh miss sentinel (callers pass 0xffffffff, 00207280.c:79).
inline constexpr std::uint32_t kNoSurface = 0xffffffffu;

// Streaming id 0..63. Negative inputs get the decomp's asymmetric correction
// (Z += 0x01ffffff, X += 0x00ffffff) before the shift/mask tree.
int tileIdAt(std::int32_t fixedX, std::int32_t fixedZ) noexcept;

// Inverse of tileIdAt for id 0..63. Origins match world-geography.md and
// gameplay/world_layout.cpp (64/64 round-trip). Out-of-range ids return id -1.
struct TilePlacement {
    int id = -1;
    std::int32_t x0 = 0;
    std::int32_t z0 = 0;
    int row = 0;         // 0..7
    int halfColumn = 0;  // 0..15, odd rows sheared by one half-column
};

TilePlacement tilePlacementOf(int id) noexcept;

// FUN_0021ba70 render wrap: tile-local game units plus the streaming id.
// renderX applies the odd-row half-column shear before masking 0x01ffffff.
struct LocalGame {
    int tileId = 0;
    float x = 0.0f;
    float z = 0.0f;
};

LocalGame localGameAt(std::int32_t fixedX, std::int32_t fixedZ) noexcept;

// Packet word3 from the winning ground triangle. Bits 8-10 are the material
// class the tyre-dust path reads (config/game_state.toml ground-contact note):
// 0 asphalt, 1 dirt, 3 grass, 4 snow, 5 hard, 6 water. -1 when word is the
// miss sentinel.
int surfaceMaterialClass(std::uint32_t surface) noexcept;

struct SurfaceSample {
    bool hit = false;                       // a non-negative ground triangle recorded
    float height = 0.0f;                    // game units; probe Y when !hit
    std::uint32_t surface = kNoSurface;     // packet word3, or kNoSurface
};

// Collision strips from one FLD/COURSE container's PMP block [offB, offC).
// Packet grammar: [0x8000|count, 0x20224000, 0x41, surface] then N float4
// (x, height, z, q) and N-2 slope quads (nx, ny, nz, w). IntersectPointWithMesh
// walks those as a triangle strip with alternating winding, not the render fan
// field_converter.cpp builds for drawing.
class TerrainMesh {
public:
    bool load(std::span<const std::uint8_t> file);

    // referenceY is the probe (wheel/body Y). Default is the sky bound, so the
    // sample is the highest ground under (x, z). On a miss, height stays at
    // referenceY and hit is false — the game's "leave Y unchanged" fallback.
    SurfaceSample heightAt(float x, float z, float referenceY = kMeshHeightBound) const;

    int packetCount() const noexcept { return packetCount_; }
    int triangleCount() const noexcept { return triangleCount_; }

private:
    struct Tri {
        float x0 = 0, h0 = 0, z0 = 0;
        float x1 = 0, h1 = 0, z1 = 0;
        float x2 = 0, h2 = 0, z2 = 0;
        float nx = 0, nz = 0;
        std::uint32_t surface = 0;
        std::int8_t winding = 1;  // +1: all edge tests >= 0; -1: all <= 0
    };

    std::vector<Tri> triangles_;
    std::vector<std::uint32_t> binOfs_;   // kCollisionGrid^2 + 1 prefix
    std::vector<std::uint32_t> binTris_;  // triangle indices, grouped by cell
    int packetCount_ = 0;
    int triangleCount_ = 0;
};

}  // namespace rt::native::gameplay
