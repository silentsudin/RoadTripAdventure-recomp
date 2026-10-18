// W3-03 town-road occupancy collision.  The road geometry comes from the
// user's SLUS_203.98 through NavRouteTables; this file embeds no game data.
//
// Grounding: NavRouteTables::townGraph(1) resolves outer-directory vaddr
// 0x002D98A8 (FUN_0025FBC0).  In SLUS_203.98, outer[1] @0x002D98AC is
// 0x002D9310 and its pair is [0x002C5FF0, 0x002C67D0): 84 * 24-byte records.
// Each is {AX, AZ, BX, BZ, ...} f32, proven by FUN_00250068.  The town-tile
// [0, 1600] coordinate extent is prior-verified in
// docs/native-reference/nav-tables.md §Scale verdict.
#pragma once

#include "assets/nav_routes.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace rt::native::gameplay::collision {

struct Point {
    float x = 0.f;
    float z = 0.f;
};

struct CollisionResult {
    Point position{};
    Point velocity{};
    bool contact = false;
};

class OccupancyGrid {
public:
    // Bound values are the documented native tile-local coordinate extent;
    // they are not an authored world scale.  Their route-table source is the
    // f32 segments selected from 0x002D98A8 as described above.
    static constexpr float kTownMinimum = 0.f;
    static constexpr float kTownMaximum = 1600.f;
    // game_loop.h's drive-tuned evidence records a 60-unit road half-width.
    static constexpr float kTownRoadHalfWidth = 60.f;

    [[nodiscard]] static OccupancyGrid fromTownGraph(const assets::NavGraph& graph,
                                                     float roadHalfWidth = kTownRoadHalfWidth,
                                                     float margin = 0.f) {
        OccupancyGrid grid;
        // W3-03 deliberately derives resolution from the loaded graph's 84
        // segment records rather than embedding a second geometry table.
        grid.width_ = graph.segments.size();
        grid.height_ = graph.segments.size();
        if (grid.width_ == 0) return grid;
        grid.cellUnits_ = (kTownMaximum - kTownMinimum) / static_cast<float>(grid.width_);
        grid.blocked_.assign(grid.width_ * grid.height_, true);

        // Mark every cell which can contain a point inside the road ribbon.
        // The half-cell diagonal is a rasterization margin, so a point exactly
        // roadHalfWidth from a segment cannot map to a blocked cell.
        const float cellCoverageMargin = std::sqrt(2.f) * grid.cellUnits_ * .5f;
        const float freeDistance = std::max(0.f, roadHalfWidth + margin) + cellCoverageMargin;
        const float freeDistanceSquared = freeDistance * freeDistance;
        for (std::size_t z = 0; z < grid.height_; ++z) {
            for (std::size_t x = 0; x < grid.width_; ++x) {
                const Point centre{kTownMinimum + (static_cast<float>(x) + 0.5f) * grid.cellUnits_,
                                   kTownMinimum + (static_cast<float>(z) + 0.5f) * grid.cellUnits_};
                for (const assets::NavSegment& segment : graph.segments) {
                    if (distanceSquaredToSegment(centre, segment) <= freeDistanceSquared) {
                        grid.blocked_[grid.index(x, z)] = false;
                        break;
                    }
                }
            }
        }
        return grid;
    }

    [[nodiscard]] std::size_t width() const { return width_; }
    [[nodiscard]] std::size_t height() const { return height_; }
    [[nodiscard]] float cellUnits() const { return cellUnits_; }

    [[nodiscard]] int cellX(Point p) const { return cellCoordinate(p.x); }
    [[nodiscard]] int cellZ(Point p) const { return cellCoordinate(p.z); }

    [[nodiscard]] bool blocked(Point p) const {
        const int x = cellX(p);
        const int z = cellZ(p);
        return x < 0 || z < 0 || static_cast<std::size_t>(x) >= width_ ||
               static_cast<std::size_t>(z) >= height_ || blocked_.empty() ||
               blocked_[index(static_cast<std::size_t>(x), static_cast<std::size_t>(z))];
    }

    // Move from the known prior free position toward desiredPosition.  The
    // first blocked sample returns the prior free point and damps velocity by
    // the caller-provided restitution (the coordinator should pass 0.3f from
    // the W3-03 collision policy).  Sampling is integer-counted and grid-sized
    // so a fixed-step replay gets the same occupied cells across hosts.
    [[nodiscard]] CollisionResult resolve(Point previousPosition, Point desiredPosition, Point velocity,
                                          float restitution) const {
        CollisionResult result{desiredPosition, velocity, false};
        if (blocked(previousPosition)) {
            result.position = previousPosition;
            result.velocity = Point{};
            result.contact = true;
            return result;
        }
        const float dx = desiredPosition.x - previousPosition.x;
        const float dz = desiredPosition.z - previousPosition.z;
        const float largestAxis = std::max(std::fabs(dx), std::fabs(dz));
        const int steps = cellUnits_ > 0.f ? std::max(1, static_cast<int>(std::ceil(largestAxis / cellUnits_))) : 1;
        Point lastFree = previousPosition;
        for (int step = 1; step <= steps; ++step) {
            const float fraction = static_cast<float>(step) / static_cast<float>(steps);
            const Point candidate{previousPosition.x + dx * fraction, previousPosition.z + dz * fraction};
            if (blocked(candidate)) {
                result.position = lastFree;
                result.velocity = Point{velocity.x * restitution, velocity.z * restitution};
                result.contact = true;
                return result;
            }
            lastFree = candidate;
        }
        return result;
    }

private:
    [[nodiscard]] static float distanceSquaredToSegment(Point p, const assets::NavSegment& segment) {
        const float dx = segment.bx - segment.ax;
        const float dz = segment.bz - segment.az;
        const float lengthSquared = dx * dx + dz * dz;
        if (lengthSquared <= std::numeric_limits<float>::epsilon()) {
            const float px = p.x - segment.ax;
            const float pz = p.z - segment.az;
            return px * px + pz * pz;
        }
        const float projection = std::clamp(((p.x - segment.ax) * dx + (p.z - segment.az) * dz) / lengthSquared,
                                            0.f, 1.f);
        const float px = p.x - (segment.ax + projection * dx);
        const float pz = p.z - (segment.az + projection * dz);
        return px * px + pz * pz;
    }

    [[nodiscard]] std::size_t index(std::size_t x, std::size_t z) const { return z * width_ + x; }
    [[nodiscard]] int cellCoordinate(float coordinate) const {
        if (coordinate < kTownMinimum || coordinate >= kTownMaximum || cellUnits_ <= 0.f) return -1;
        return static_cast<int>((coordinate - kTownMinimum) / cellUnits_);
    }

    std::size_t width_ = 0;
    std::size_t height_ = 0;
    float cellUnits_ = 0.f;
    std::vector<bool> blocked_;
};

}  // namespace rt::native::gameplay::collision
