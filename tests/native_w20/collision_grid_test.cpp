// W7-03: town collision grid -> car-physics integration contract.
//
// Proves the W3-03 occupancy grid (gameplay/collision_grid.h) can feed
// gameplay car integration: it builds from the *town* nav graph, the known
// Peach Town spawn pose lands on a free cell, road corridors exist, and
// building-interior-ish samples are blocked.  Also settles the 84-vs-43
// segment-count discrepancy by reading the ELF's two independent directories
// directly (raw structure reads, not the reader) and cross-checking the
// NavRouteTables decode.  Reads the user's own SLUS_203.98 at run time only;
// its absence is an intentional SKIP (exit 0) and never a failure.
#include "assets/nav_routes.h"
#include "gameplay/collision_grid.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

int checks = 0;
int failures = 0;

void check(bool ok, const char* what) {
    ++checks;
    if (!ok) {
        ++failures;
        std::printf("FAIL %s\n", what);
    }
}

// --- raw ELF structure reads (independent of NavRouteTables) ---------------
std::uint16_t le16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint16_t{b[at]} | (std::uint16_t{b[at + 1]} << 8);
}
std::uint32_t le32(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint32_t{b[at]} | (std::uint32_t{b[at + 1]} << 8) |
           (std::uint32_t{b[at + 2]} << 16) | (std::uint32_t{b[at + 3]} << 24);
}
std::uint32_t elfOffset(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    if (b.size() < 0x34) return 0xffffffffu;
    const std::uint32_t phoff = le32(b, 0x1c);
    const std::uint16_t entsize = le16(b, 0x2a), count = le16(b, 0x2c);
    if (entsize < 32 || phoff + std::size_t{entsize} * count > b.size()) return 0xffffffffu;
    for (std::uint16_t i = 0; i < count; ++i) {
        const std::size_t ph = phoff + std::size_t{i} * entsize;
        if (le32(b, ph) != 1 /* PT_LOAD */) continue;
        const std::uint32_t off = le32(b, ph + 4), base = le32(b, ph + 8), size = le32(b, ph + 16);
        if (va >= base && va - base + 4 <= size) return off + va - base;
    }
    return 0xffffffffu;
}
std::uint32_t word(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    const std::uint32_t off = elfOffset(b, va);
    return off == 0xffffffffu || off + 4 > b.size() ? 0u : le32(b, off);
}

// --- disc discovery (mirrors tests/native_w2/collision_grid_test.cpp) -----
std::filesystem::path findElf() {
    if (const char* elf = std::getenv("RT_ELF"); elf && *elf && std::filesystem::exists(elf))
        return elf;
    if (const char* disc = std::getenv("RT_DISC"); disc && *disc) {
        const std::filesystem::path p = std::filesystem::path(disc) / "SLUS_203.98";
        if (std::filesystem::exists(p)) return p;
    }
    const std::filesystem::path canonical =
        std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "") /
        "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    if (std::filesystem::exists(canonical)) return canonical;
    const std::filesystem::path repo = "build/rom/SLUS_203.98";
    if (std::filesystem::exists(repo)) return repo;
    return {};
}

// --- geometry helpers ------------------------------------------------------
float pointSegmentDistance(rt::native::gameplay::collision::Point p,
                           const rt::native::assets::NavSegment& s) {
    const float dx = s.bx - s.ax, dz = s.bz - s.az;
    const float len2 = dx * dx + dz * dz;
    if (len2 <= 1e-12f) {
        const float px = p.x - s.ax, pz = p.z - s.az;
        return std::sqrt(px * px + pz * pz);
    }
    float t = ((p.x - s.ax) * dx + (p.z - s.az) * dz) / len2;
    t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
    const float px = p.x - (s.ax + t * dx), pz = p.z - (s.az + t * dz);
    return std::sqrt(px * px + pz * pz);
}

float nearestSegmentDistance(rt::native::gameplay::collision::Point p,
                             const rt::native::assets::NavGraph& g) {
    float best = 1e30f;
    for (const auto& s : g.segments) best = std::min(best, pointSegmentDistance(p, s));
    return best;
}

// Transition-record midpoint: the graph's node, as nav_walkable.cpp defines it.
rt::native::gameplay::collision::Point nodeMidpoint(const rt::native::assets::NavGraph& g,
                                                    std::size_t i) {
    const auto& n = g.transitions[i];
    const auto& a = g.segments[n.segmentA];
    const auto& b = g.segments[n.segmentB];
    return {(a.ax + a.bx + b.ax + b.bx) * 0.25f, (a.az + a.bz + b.az + b.bz) * 0.25f};
}

}  // namespace

int main() {
    using namespace rt::native::gameplay::collision;

    const std::filesystem::path elf = findElf();
    if (elf.empty()) {
        std::puts("SKIP collision_grid: no SLUS_203.98 (set RT_ELF or RT_DISC)");
        std::printf("checks=%d failed=%d\n", checks, failures);
        return 0;
    }

    std::ifstream in(elf, std::ios::binary);
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(in), {}};
    if (bytes.empty()) {
        std::puts("SKIP collision_grid: unreadable ELF");
        std::printf("checks=%d failed=%d\n", checks, failures);
        return 0;
    }

    auto tables = rt::native::assets::NavRouteTables::open(elf);
    check(tables.has_value(), "NavRouteTables::open succeeds on the boot ELF");
    if (!tables) return 1;

    // == 84-vs-43 segment-count discrepancy, settled from raw structure ====
    // Two independent ROM directories are being conflated:
    //   * outer directory @0x002d98a8 (course/entity/town mode) -> town graph
    //   * static scene directory @0x002bd578 (pack-type indexed)
    // outer[1] (Peach Town) car0 pair is 84 segments; static[1] is 43.
    const std::uint32_t outerEntry = word(bytes, rt::native::assets::kNavOuterDirectoryVaddr + 4u);
    const std::uint32_t outerSeg = word(bytes, outerEntry);
    const std::uint32_t outerTrans = word(bytes, outerEntry + 4u);
    const std::uint32_t staticSeg = word(bytes, rt::native::assets::kNavStaticDirectoryVaddr + 8u);
    const std::uint32_t staticTrans = word(bytes, rt::native::assets::kNavStaticDirectoryVaddr + 12u);
    const std::uint32_t rawTownSegments =
        outerTrans > outerSeg ? (outerTrans - outerSeg) / 24u : 0u;
    const std::uint32_t rawStaticSegments =
        staticTrans > staticSeg ? (staticTrans - staticSeg) / 24u : 0u;

    auto town = tables->townGraph(1);
    check(town.has_value(), "townGraph(1) decodes (Peach Town)");
    if (!town) return 1;

    std::printf("evidence outer[1]=0x%08x pair={0x%08x,0x%08x} raw_segments=%u\n", outerEntry,
                outerSeg, outerTrans, rawTownSegments);
    std::printf("evidence static[1] pair={0x%08x,0x%08x} raw_segments=%u\n", staticSeg, staticTrans,
                rawStaticSegments);
    check(town->segments.size() == rawTownSegments,
          "reader town segment count equals raw (transition-segment)/24");
    check(town->segments.size() == 84, "townGraph(1) is 84 street segments (raw pair span)");
    check(town->segmentVaddr == 0x002c5ff0u && town->transitionVaddr == 0x002c67d0u,
          "townGraph(1) pair pins 0x002c5ff0/0x002c67d0");
    check(rawStaticSegments == 43, "static[1] pair span is 43 segments");

    auto staticGraph = tables->staticGraph(1);
    check(staticGraph.has_value() && staticGraph->segments.size() == 43,
          "staticGraph(1) is the 43-segment graph (the survey's other number)");
    // Same-tile sanity: the town graph and the static-1 graph occupy different
    // footprints, so they cannot be the same street corpus.
    std::printf("evidence town_segments=%zu static1_segments=%u\n", town->segments.size(),
                rawStaticSegments);

    // == (a) grid builds non-empty ==========================================
    const OccupancyGrid grid = OccupancyGrid::fromTownGraph(*town);
    check(grid.width() > 0 && grid.height() > 0, "grid dimensions non-zero");
    check(grid.width() == town->segments.size() && grid.height() == town->segments.size(),
          "grid dimensions derive from the town graph segment count");
    check(grid.cellUnits() > 0.f, "grid cell size positive");

    std::size_t freeCells = 0;
    for (std::size_t z = 0; z < grid.height(); ++z) {
        for (std::size_t x = 0; x < grid.width(); ++x) {
            const Point centre{grid.cellUnits() * (static_cast<float>(x) + 0.5f),
                               grid.cellUnits() * (static_cast<float>(z) + 0.5f)};
            if (!grid.blocked(centre)) ++freeCells;
        }
    }
    check(freeCells > 0, "grid has free (driveable) cells");
    std::printf("evidence grid=%zux%zu cellUnits=%.4f freeCells=%zu\n", grid.width(), grid.height(),
                static_cast<double>(grid.cellUnits()), freeCells);

    // == (b) known town spawn pose maps to a FREE cell ======================
    const Point spawn{506.283f, 442.230f};
    const Point settled{504.297f, 450.594f};
    check(grid.cellX(spawn) >= 0 && grid.cellZ(spawn) >= 0, "spawn pose is inside the grid bounds");
    check(!grid.blocked(spawn), "town spawn (506.283,442.230) maps to a FREE cell");
    check(!grid.blocked(settled), "settled pose (504.297,450.594) maps to a FREE cell");

    // == (c) a road corridor of free cells crosses the grid =================
    // Flood fill the free component through the spawn; a real corridor spans a
    // long run of connected free cells, not an isolated pocket.
    const int w = static_cast<int>(grid.width()), h = static_cast<int>(grid.height());
    std::vector<char> seen(static_cast<std::size_t>(w) * h, 0);
    std::vector<std::pair<int, int>> stack;
    auto pushIfFree = [&](int x, int z) {
        if (x < 0 || z < 0 || x >= w || z >= h) return;
        const std::size_t i = static_cast<std::size_t>(z) * w + x;
        if (seen[i]) return;
        const Point c{grid.cellUnits() * (static_cast<float>(x) + 0.5f),
                      grid.cellUnits() * (static_cast<float>(z) + 0.5f)};
        if (grid.blocked(c)) return;
        seen[i] = 1;
        stack.push_back({x, z});
    };
    pushIfFree(grid.cellX(spawn), grid.cellZ(spawn));
    std::size_t corridorCells = 0;
    float minX = 1e9f, maxX = -1e9f, minZ = 1e9f, maxZ = -1e9f;
    while (!stack.empty()) {
        const auto [cx, cz] = stack.back();
        stack.pop_back();
        ++corridorCells;
        const float wx = grid.cellUnits() * (static_cast<float>(cx) + 0.5f);
        const float wz = grid.cellUnits() * (static_cast<float>(cz) + 0.5f);
        minX = std::min(minX, wx); maxX = std::max(maxX, wx);
        minZ = std::min(minZ, wz); maxZ = std::max(maxZ, wz);
        pushIfFree(cx + 1, cz); pushIfFree(cx - 1, cz);
        pushIfFree(cx, cz + 1); pushIfFree(cx, cz - 1);
    }
    const float extentX = maxX - minX, extentZ = maxZ - minZ;
    const float longestExtent = std::max(extentX, extentZ);
    check(corridorCells >= 10, "spawn free component is a connected corridor (>=10 cells)");
    check(longestExtent > 4.f * grid.cellUnits(),
          "corridor spans more than four cells along at least one axis");
    std::printf("evidence corridor_cells=%zu extent=%.1fx%.1f\n", corridorCells,
                static_cast<double>(extentX), static_cast<double>(extentZ));

    // == (d) off-ribbon samples are BLOCKED / out of range ===================
    // Ground truth update 2026-10-18 (W8-03): the drivable ribbon is 60 units
    // half-wide (drive-tuned, HANDOFF-W7-03), so the old ">2 cells (38 units)"
    // far-side test contradicted the road itself.  A point is guaranteed blocked
    // only past roadHalfWidth + two cell half-diagonals (free rule + the cell
    // containing the point).
    const float blockedFrom = OccupancyGrid::kTownRoadHalfWidth +
                              std::sqrt(2.f) * grid.cellUnits();
    std::size_t sampled = 0, blockedSamples = 0;
    float worstFreeDistance = 0.f;
    for (std::size_t i = 0; i < town->transitions.size(); ++i) {
        const Point node = nodeMidpoint(*town, i);
        for (float radius : {60.f, 90.f, 120.f}) {
            for (int k = 0; k < 8; ++k) {
                const float theta = static_cast<float>(k) * 3.14159265f / 4.f;
                const Point q{node.x + radius * std::cos(theta), node.z + radius * std::sin(theta)};
                const float d = nearestSegmentDistance(q, *town);
                if (d <= blockedFrom) continue;  // still road-adjacent; skip
                ++sampled;
                if (grid.blocked(q)) ++blockedSamples;
                else worstFreeDistance = std::max(worstFreeDistance, d);
            }
        }
    }
    check(sampled > 0, "produced off-ribbon samples far from every route segment");
    check(blockedSamples == sampled,
          "every sample past the road ribbon + raster margin is BLOCKED");
    std::printf("evidence interior_samples=%zu blocked=%zu\n", sampled, blockedSamples);
    // Out-of-range reads must report blocked, never free.
    check(grid.blocked(Point{-10.f, -10.f}), "point below the grid is out of range (blocked)");
    check(grid.blocked(Point{2000.f, 2000.f}), "point above the grid is out of range (blocked)");

    // == (e) ray between two known-nearby route nodes is mostly free ========
    std::vector<Point> nodes;
    nodes.reserve(town->transitions.size());
    for (std::size_t i = 0; i < town->transitions.size(); ++i) nodes.push_back(nodeMidpoint(*town, i));
    int raysTested = 0, raysMostlyFree = 0;
    float worstFreeFraction = 1.f;
    for (std::size_t i = 0; i < nodes.size() && raysTested < 24; ++i) {
        for (std::size_t j = i + 1; j < nodes.size() && raysTested < 24; ++j) {
            const float dx = nodes[i].x - nodes[j].x, dz = nodes[i].z - nodes[j].z;
            const float d = std::sqrt(dx * dx + dz * dz);
            if (!(d > 3.f && d < 30.f)) continue;
            if (grid.blocked(nodes[i]) || grid.blocked(nodes[j])) continue;
            ++raysTested;
            int free = 0;
            constexpr int kSteps = 64;
            for (int k = 0; k <= kSteps; ++k) {
                const float f = static_cast<float>(k) / kSteps;
                if (!grid.blocked(Point{nodes[i].x + dx * f, nodes[i].z + dz * f})) ++free;
            }
            const float frac = static_cast<float>(free) / (kSteps + 1);
            worstFreeFraction = std::min(worstFreeFraction, frac);
            if (frac >= 0.8f) ++raysMostlyFree;
        }
    }
    check(raysTested >= 8, "found >=8 nearby free-node pairs to ray-test");
    check(raysTested > 0 && raysMostlyFree == raysTested,
          "every near-node ray is mostly free (>=80% of samples)");
    std::printf("evidence rays_tested=%d mostly_free=%d worst_free_fraction=%.3f\n", raysTested,
                raysMostlyFree, static_cast<double>(worstFreeFraction));

    std::printf("checks=%d failed=%d\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
