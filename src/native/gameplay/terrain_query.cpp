#include "terrain_query.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

namespace rt::native::gameplay {
namespace {

std::uint32_t readU32(std::span<const std::uint8_t> bytes, std::size_t at) noexcept {
    return static_cast<std::uint32_t>(bytes[at]) |
           (static_cast<std::uint32_t>(bytes[at + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[at + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[at + 3]) << 24);
}

float readF32(std::span<const std::uint8_t> bytes, std::size_t at) noexcept {
    const std::uint32_t bits = readU32(bytes, at);
    float value = 0.0f;
    static_assert(sizeof(value) == sizeof(bits));
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

// 00207280.c:71. Truncate toward zero, then wrap into the 16-wide grid.
int collisionCell(float x, float z) noexcept {
    const int cx = static_cast<int>(x / kCollisionCellUnits) & 0xf;
    const int cz = static_cast<int>(z / kCollisionCellUnits) & 0xf;
    return cx + cz * kCollisionGrid;
}

bool finite3(float x, float y, float z) noexcept {
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

}  // namespace

int tileIdAt(std::int32_t fixedX, std::int32_t fixedZ) noexcept {
    // build/decomp/fn/0021c8d0.c:11-18, instruction immediates at 0x21c8d4..0x21c914.
    std::int32_t x = fixedX;
    std::int32_t z = fixedZ;
    if (z < 0) z += 0x01ffffff;
    if (x < 0) x += 0x00ffffff;
    const unsigned uz = static_cast<unsigned>(z);
    const unsigned ux = static_cast<unsigned>(x);
    const unsigned b = (uz >> 25) & 1u;
    return static_cast<int>(((uz >> 25) & 6u) * 8u + (((ux >> 24) - b) & 0x0eu) + b);
}

TilePlacement tilePlacementOf(int id) noexcept {
    // world-geography.md "Fixed coordinate to tile id", inverted. id bits:
    // A = bits 5:4, B = bits 3:2, swap2(C) = bits 1:0. Same origins as
    // gameplay/world_layout.cpp::tilePlacement (verified 64/64).
    TilePlacement out;
    if (id < 0 || id > 63) return out;
    const int A = (id >> 4) & 3;
    const int B = (id >> 2) & 3;
    const int swappedC = id & 3;
    const int C = (swappedC & 1) << 1 | (swappedC >> 1);  // swap2 inverse is swap2
    const int b = C >> 1;
    const int row = (A << 1) | b;
    const int col = (B << 1) | (C & 1);
    out.id = id;
    out.row = row;
    out.halfColumn = (col << 1) | b;
    out.x0 = out.halfColumn << 24;
    out.z0 = row << 25;
    return out;
}

LocalGame localGameAt(std::int32_t fixedX, std::int32_t fixedZ) noexcept {
    // 0021ba70.c:424-426. Y is not needed for the horizontal wrap.
    LocalGame out;
    out.tileId = tileIdAt(fixedX, fixedZ);
    const auto ux = static_cast<std::uint32_t>(fixedX);
    const auto uz = static_cast<std::uint32_t>(fixedZ);
    const std::uint32_t sheared = ux + ((uz & 0x02000000u) >> 1);
    out.x = static_cast<float>(static_cast<double>(sheared & 0x01ffffffu) /
                               static_cast<double>(kFixedUnitsPerGameUnit));
    out.z = static_cast<float>(static_cast<double>(uz & 0x01ffffffu) /
                               static_cast<double>(kFixedUnitsPerGameUnit));
    return out;
}

int surfaceMaterialClass(std::uint32_t surface) noexcept {
    if (surface == kNoSurface) return -1;
    return static_cast<int>((surface >> 8) & 7u);
}

bool TerrainMesh::load(std::span<const std::uint8_t> file) {
    triangles_.clear();
    binOfs_.clear();
    binTris_.clear();
    packetCount_ = 0;
    triangleCount_ = 0;
    if (file.size() < 20) return false;

    const std::uint32_t offB = readU32(file, 8);
    const std::uint32_t offC = readU32(file, 12);
    if (offB >= offC || static_cast<std::size_t>(offC) > file.size()) return false;

    // Anchors from field-topology-status.md "Section packet grammar". Collision
    // consumes the same packets as a strip (00206f28.c:31-114): header word0
    // low 15 bits = vertex count, word3 = surface/flags.
    constexpr std::uint32_t kHeaderFlag = 0x8000;
    constexpr std::uint32_t kHeaderTag = 0x20224000;
    constexpr std::uint32_t kHeaderKind = 0x41;
    constexpr std::uint32_t kMaxVertices = 0x400;

    std::size_t cursor = offB;
    const std::size_t limit = offC;
    while (cursor + 16 <= limit) {
        const std::uint32_t word0 = readU32(file, cursor);
        if ((word0 & 0xF000u) != kHeaderFlag || readU32(file, cursor + 4) != kHeaderTag ||
            readU32(file, cursor + 8) != kHeaderKind) {
            cursor += 4;
            continue;
        }
        const std::uint32_t vertexCount = word0 & 0x7fffu;
        if (vertexCount < 3 || vertexCount > kMaxVertices) {
            cursor += 4;
            continue;
        }
        const std::size_t bytes =
            16u + static_cast<std::size_t>(vertexCount) * 16u +
            static_cast<std::size_t>(vertexCount - 2) * 16u;
        if (cursor + bytes > file.size()) break;

        std::vector<float> vx(vertexCount), vh(vertexCount), vz(vertexCount);
        bool verticesOk = true;
        for (std::uint32_t i = 0; i < vertexCount; ++i) {
            const std::size_t at = cursor + 16 + static_cast<std::size_t>(i) * 16u;
            vx[i] = readF32(file, at);
            vh[i] = readF32(file, at + 4);
            vz[i] = readF32(file, at + 8);
            if (!finite3(vx[i], vh[i], vz[i])) {
                verticesOk = false;
                break;
            }
        }
        if (!verticesOk) {
            cursor += 4;
            continue;
        }
        const std::uint32_t surface = readU32(file, cursor + 12);
        const std::size_t normalAt = cursor + 16 + static_cast<std::size_t>(vertexCount) * 16u;
        const int fan = static_cast<int>(vertexCount) - 2;
        int tri = 0;
        int base = 0;
        while (tri < fan) {
            auto push = [&](int i0, int i1, int i2, int normalIndex, std::int8_t winding) {
                const std::size_t at = normalAt + static_cast<std::size_t>(normalIndex) * 16u;
                const float nx = readF32(file, at);
                const float nz = readF32(file, at + 8);
                if (!std::isfinite(nx) || !std::isfinite(nz)) return;
                Tri t;
                t.x0 = vx[static_cast<std::size_t>(i0)];
                t.h0 = vh[static_cast<std::size_t>(i0)];
                t.z0 = vz[static_cast<std::size_t>(i0)];
                t.x1 = vx[static_cast<std::size_t>(i1)];
                t.h1 = vh[static_cast<std::size_t>(i1)];
                t.z1 = vz[static_cast<std::size_t>(i1)];
                t.x2 = vx[static_cast<std::size_t>(i2)];
                t.h2 = vh[static_cast<std::size_t>(i2)];
                t.z2 = vz[static_cast<std::size_t>(i2)];
                t.nx = nx;
                t.nz = nz;
                t.surface = surface;
                t.winding = winding;
                triangles_.push_back(t);
            };
            push(base, base + 1, base + 2, tri, 1);
            ++tri;
            if (tri >= fan) break;
            push(base + 1, base + 2, base + 3, tri, -1);
            ++tri;
            base += 2;
        }
        ++packetCount_;
        cursor += bytes;
    }
    triangleCount_ = static_cast<int>(triangles_.size());

    constexpr int kCells = kCollisionGrid * kCollisionGrid;
    std::vector<std::vector<std::uint32_t>> bins(static_cast<std::size_t>(kCells));
    for (std::uint32_t index = 0; index < triangles_.size(); ++index) {
        const Tri& t = triangles_[index];
        const float minX = std::min(t.x0, std::min(t.x1, t.x2));
        const float maxX = std::max(t.x0, std::max(t.x1, t.x2));
        const float minZ = std::min(t.z0, std::min(t.z1, t.z2));
        const float maxZ = std::max(t.z0, std::max(t.z1, t.z2));
        int cx0 = static_cast<int>(std::floor(minX / kCollisionCellUnits));
        int cx1 = static_cast<int>(std::floor(maxX / kCollisionCellUnits));
        int cz0 = static_cast<int>(std::floor(minZ / kCollisionCellUnits));
        int cz1 = static_cast<int>(std::floor(maxZ / kCollisionCellUnits));
        if (cx1 - cx0 > kCollisionGrid) {
            cx0 = static_cast<int>(std::floor(minX / kCollisionCellUnits));
            cx1 = cx0;
        }
        if (cz1 - cz0 > kCollisionGrid) {
            cz0 = static_cast<int>(std::floor(minZ / kCollisionCellUnits));
            cz1 = cz0;
        }
        for (int cz = cz0; cz <= cz1; ++cz) {
            for (int cx = cx0; cx <= cx1; ++cx) {
                const int cell = (cx & 0xf) + (cz & 0xf) * kCollisionGrid;
                bins[static_cast<std::size_t>(cell)].push_back(index);
            }
        }
    }
    binOfs_.assign(static_cast<std::size_t>(kCells) + 1u, 0u);
    std::size_t total = 0;
    for (int cell = 0; cell < kCells; ++cell) {
        binOfs_[static_cast<std::size_t>(cell)] = static_cast<std::uint32_t>(total);
        total += bins[static_cast<std::size_t>(cell)].size();
    }
    binOfs_[static_cast<std::size_t>(kCells)] = static_cast<std::uint32_t>(total);
    binTris_.reserve(total);
    for (int cell = 0; cell < kCells; ++cell) {
        const auto& bin = bins[static_cast<std::size_t>(cell)];
        binTris_.insert(binTris_.end(), bin.begin(), bin.end());
    }
    return packetCount_ > 0;
}

SurfaceSample TerrainMesh::heightAt(float x, float z, float referenceY) const {
    // 00206f28.c:45-108, bounds from 00207280.c:102-116.
    // Negative surface words only move the floor/ceiling around the probe.
    // Bit 28 records a special height and is not a ground id. Everything else
    // raises the floor when it is the highest ground still under the ceiling.
    SurfaceSample sample;
    sample.height = referenceY;
    if (binOfs_.size() < 2 || !std::isfinite(x) || !std::isfinite(z)) return sample;

    const int cell = collisionCell(x, z);
    const std::uint32_t begin = binOfs_[static_cast<std::size_t>(cell)];
    const std::uint32_t end = binOfs_[static_cast<std::size_t>(cell) + 1u];
    float floorH = -kMeshHeightBound;
    float ceilH = kMeshHeightBound;
    bool hit = false;
    std::uint32_t surface = kNoSurface;

    for (std::uint32_t slot = begin; slot < end; ++slot) {
        const Tri& t = triangles_[binTris_[slot]];
        const float c1 = (t.z1 - t.z0) * (x - t.x1) - (t.x1 - t.x0) * (z - t.z1);
        const float c2 = (t.z2 - t.z1) * (x - t.x2) - (t.x2 - t.x1) * (z - t.z2);
        const float c3 = (t.z0 - t.z2) * (x - t.x0) - (t.x0 - t.x2) * (z - t.z0);
        const bool inside = t.winding > 0 ? (c1 >= 0.0f && c2 >= 0.0f && c3 >= 0.0f)
                                           : (c1 <= 0.0f && c2 <= 0.0f && c3 <= 0.0f);
        if (!inside) continue;
        const float h = t.h0 - ((x - t.x0) * t.nx + (z - t.z0) * t.nz);
        if (!std::isfinite(h)) continue;
        const auto signedWord = static_cast<std::int32_t>(t.surface);
        if (signedWord < 0) {
            if (referenceY < h && h < ceilH) ceilH = h;
            if (h < referenceY && floorH < h) floorH = h;
        } else if ((t.surface & 0x10000000u) == 0u) {
            if (h < ceilH && floorH < h) {
                floorH = h;
                surface = t.surface;
                hit = true;
            }
        }
    }
    if (hit) {
        sample.hit = true;
        sample.height = floorH;
        sample.surface = surface;
    }
    return sample;
}

}  // namespace rt::native::gameplay
