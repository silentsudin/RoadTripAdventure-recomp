// W8-03: a road-width occupancy grid must cover, rather than narrow, Peach Town's
// 84-segment navigation ribbon.  The ELF remains user-supplied at run time.
#include "assets/nav_routes.h"
#include "gameplay/collision_grid.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <limits>
#include <string>

namespace {

int checks = 0;
int failures = 0;

void check(bool ok, const char* name) {
    ++checks;
    if (!ok) {
        ++failures;
        std::printf("FAIL %s\n", name);
    }
}

std::filesystem::path findElf() {
    if (const char* elf = std::getenv("RT_ELF"); elf && *elf && std::filesystem::exists(elf)) return elf;
    const std::filesystem::path canonical =
        std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "") /
        "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    return std::filesystem::exists(canonical) ? canonical : std::filesystem::path{};
}

using Point = rt::native::gameplay::collision::Point;
using Segment = rt::native::assets::NavSegment;

float distanceToSegment(Point p, const Segment& s) {
    const float dx = s.bx - s.ax;
    const float dz = s.bz - s.az;
    const float lengthSquared = dx * dx + dz * dz;
    if (lengthSquared <= std::numeric_limits<float>::epsilon()) {
        const float px = p.x - s.ax;
        const float pz = p.z - s.az;
        return std::sqrt(px * px + pz * pz);
    }
    const float t = std::clamp(((p.x - s.ax) * dx + (p.z - s.az) * dz) / lengthSquared, 0.f, 1.f);
    const float px = p.x - (s.ax + t * dx);
    const float pz = p.z - (s.az + t * dz);
    return std::sqrt(px * px + pz * pz);
}

float nearestDistance(Point p, const rt::native::assets::NavGraph& graph) {
    float best = std::numeric_limits<float>::infinity();
    for (const Segment& segment : graph.segments) best = std::min(best, distanceToSegment(p, segment));
    return best;
}

}  // namespace

int main() {
    using namespace rt::native::gameplay::collision;

    const std::filesystem::path elf = findElf();
    if (elf.empty()) {
        std::puts("SKIP collision_sizing: no SLUS_203.98 (set RT_ELF)");
        std::printf("checks=%d failed=%d\n", checks, failures);
        return 0;
    }
    auto tables = rt::native::assets::NavRouteTables::open(elf);
    check(tables.has_value(), "NavRouteTables opens ELF");
    if (!tables) return 1;
    auto town = tables->townGraph(1);
    check(town.has_value() && town->segments.size() == 84, "Peach Town resolves to 84 segments");
    if (!town) return 1;

    const OccupancyGrid grid = OccupancyGrid::fromTownGraph(*town);
    std::size_t freeCells = 0;
    for (std::size_t z = 0; z < grid.height(); ++z) {
        for (std::size_t x = 0; x < grid.width(); ++x) {
            if (!grid.blocked({(static_cast<float>(x) + .5f) * grid.cellUnits(),
                               (static_cast<float>(z) + .5f) * grid.cellUnits()})) ++freeCells;
        }
    }

    float minX = std::numeric_limits<float>::infinity();
    float maxX = -minX;
    float minZ = minX;
    float maxZ = -minX;
    for (const Segment& segment : town->segments) {
        minX = std::min({minX, segment.ax, segment.bx});
        maxX = std::max({maxX, segment.ax, segment.bx});
        minZ = std::min({minZ, segment.az, segment.bz});
        maxZ = std::max({maxZ, segment.az, segment.bz});
    }
    constexpr float kRoadSampleOffset = 40.f;
    constexpr float kOutsideRoadDistance = 200.f;
    std::size_t ribbonSamples = 0;
    std::size_t ribbonFree = 0;
    std::size_t outsideSamples = 0;
    std::size_t outsideBlocked = 0;
    for (const Segment& segment : town->segments) {
        const float dx = segment.bx - segment.ax;
        const float dz = segment.bz - segment.az;
        const float length = std::sqrt(dx * dx + dz * dz);
        const float nx = length > 0.f ? -dz / length : 1.f;
        const float nz = length > 0.f ? dx / length : 0.f;
        for (int i = 0; i <= 16; ++i) {
            const float t = static_cast<float>(i) / 16.f;
            const Point centre{segment.ax + dx * t, segment.az + dz * t};
            for (float offset : {-kRoadSampleOffset, 0.f, kRoadSampleOffset}) {
                const Point sample{centre.x + nx * offset, centre.z + nz * offset};
                if (sample.x < 0.f || sample.x >= OccupancyGrid::kTownMaximum || sample.z < 0.f ||
                    sample.z >= OccupancyGrid::kTownMaximum)
                    continue;
                ++ribbonSamples;
                if (!grid.blocked(sample)) ++ribbonFree;
            }
        }
    }
    for (float z = 0.f; z < OccupancyGrid::kTownMaximum; z += grid.cellUnits()) {
        for (float x = 0.f; x < OccupancyGrid::kTownMaximum; x += grid.cellUnits()) {
            const Point sample{x + .5f * grid.cellUnits(), z + .5f * grid.cellUnits()};
            if (nearestDistance(sample, *town) <= kOutsideRoadDistance) continue;
            ++outsideSamples;
            if (grid.blocked(sample)) ++outsideBlocked;
        }
    }

    const Point spawn{506.283f, 442.230f};
    check(!grid.blocked(spawn), "Peach Town spawn is free");
    check(ribbonSamples > 0 && ribbonFree * 100 >= ribbonSamples * 99,
          "at least 99 percent of dense +/-40 route ribbon samples are free");
    check(outsideSamples > 0 && outsideBlocked == outsideSamples,
          "points farther than 200 units from every segment are blocked");
    constexpr float kGridBuffer = 60.f;
    check(minX - kGridBuffer >= OccupancyGrid::kTownMinimum && maxX + kGridBuffer < OccupancyGrid::kTownMaximum &&
              minZ - kGridBuffer >= OccupancyGrid::kTownMinimum && maxZ + kGridBuffer < OccupancyGrid::kTownMaximum,
          "grid covers the full route bounding box plus the 60-unit road half-width");

    const CollisionResult spawnResult = grid.resolve(spawn, spawn, {}, .3f);
    check(!spawnResult.contact, "resolve has no collision at spawn");
    std::size_t routeSteps = 0;
    std::size_t routeCollisions = 0;
    for (const Segment& segment : town->segments) {
        const Point a{segment.ax, segment.az};
        const Point b{segment.bx, segment.bz};
        const CollisionResult result = grid.resolve(a, b, {b.x - a.x, b.z - a.z}, .3f);
        ++routeSteps;
        if (result.contact) ++routeCollisions;
    }
    check(routeCollisions == 0, "resolve has no collision along sampled route segments");

    std::printf("evidence grid=%zux%zu cellUnits=%.4f freeCells=%zu\n", grid.width(), grid.height(),
                static_cast<double>(grid.cellUnits()), freeCells);
    std::printf("evidence ribbon_samples=%zu free=%zu fraction=%.6f offset=%.1f\n", ribbonSamples, ribbonFree,
                ribbonSamples ? static_cast<double>(ribbonFree) / ribbonSamples : 0., kRoadSampleOffset);
    std::printf("evidence outside_samples=%zu blocked=%zu minimum_distance=%.1f\n", outsideSamples, outsideBlocked,
                kOutsideRoadDistance);
    std::printf("evidence segment_bbox=x[%.1f,%.1f] z[%.1f,%.1f] grid=[%.1f,%.1f]^2 buffer=%.1f\n", minX, maxX,
                minZ, maxZ, OccupancyGrid::kTownMinimum, OccupancyGrid::kTownMaximum, kGridBuffer);
    std::printf("evidence resolve_route_steps=%zu collisions=%zu\n", routeSteps, routeCollisions);
    std::printf("checks=%d failed=%d\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
