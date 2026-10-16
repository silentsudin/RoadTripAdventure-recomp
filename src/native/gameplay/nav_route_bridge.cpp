#include "gameplay/nav_route_bridge.h"

#include <algorithm>
#include <cmath>

namespace rt::native::gameplay {
namespace {

float pointPointDist(const Pt& a, const Pt& b) {
    const float dx = a.x - b.x, dz = a.z - b.z;
    return std::sqrt(dx * dx + dz * dz);
}

// Distance from point p to segment s0->s1 (clamped projection).
float pointSegmentDist(const Pt& p, const Pt& s0, const Pt& s1) {
    const float dx = s1.x - s0.x, dz = s1.z - s0.z;
    const float len2 = dx * dx + dz * dz;
    if (len2 <= 0.f) return pointPointDist(p, s0);
    const float t = std::clamp(((p.x - s0.x) * dx + (p.z - s0.z) * dz) / len2, 0.f, 1.f);
    return pointPointDist(p, {s0.x + t * dx, s0.z + t * dz});
}

} // namespace

std::vector<RouteZone> buildRouteZones(const NavPath& path, float halfWidth) {
    std::vector<RouteZone> zones;
    const auto& c = path.centerline;
    if (c.size() < 2) return zones;
    const std::size_t pairs = path.closed ? c.size() : c.size() - 1;

    // Leg endpoints/midpoints for separation queries.
    std::vector<Pt> starts(pairs), ends(pairs), mids(pairs);
    for (std::size_t i = 0; i < pairs; ++i) {
        starts[i] = c[i];
        ends[i] = c[(i + 1) % c.size()];
        mids[i] = {(starts[i].x + ends[i].x) * 0.5f, (starts[i].z + ends[i].z) * 0.5f};
    }

    // Tiny floor: own-midpoint containment holds at any positive width; the
    // floor keeps genuinely crossing graphs (separation ~0) from producing
    // zero-width degenerate zones.
    constexpr float kMinHalf = 0.25f;
    zones.reserve(pairs);
    for (std::size_t i = 0; i < pairs; ++i) {
        const Pt& a = starts[i];
        const Pt& b = ends[i];
        const float dx = b.x - a.x, dz = b.z - a.z;
        const float len = std::sqrt(dx * dx + dz * dz);
        if (len < 1e-3f) continue;  // degenerate pair: no zone, no progress
        // Clamp width so no *other* leg's midpoint can land inside this zone
        // under RaceRoute::zoneOf's first-match semantics.  Midpoint-based
        // distance to this leg's axis covers switchbacks into adjacent zones
        // too (leg midpoints stay clear of shared endpoints, so unlike full
        // leg separation this never collapses for legitimate neighbours).
        float width = halfWidth;
        for (std::size_t j = 0; j < pairs; ++j) {
            if (j == i) continue;
            width = std::min(width, 0.5f * pointSegmentDist(mids[j], starts[i], ends[i]));
        }
        width = std::max(width, kMinHalf);
        const float nx = -dz / len * width, nz = dx / len * width;
        // Vertex order satisfying pointInConvexPolygon's z-first cross
        // convention (its "CCW").  Perpendicular end caps keep each zone's
        // own leg midpoint strictly inside and the next midpoint outside, so
        // progress stays monotonic along the path.
        zones.push_back(RouteZone{{
            Vec2{a.x + nx, a.z + nz}, Vec2{b.x + nx, b.z + nz},
            Vec2{b.x - nx, b.z - nz}, Vec2{a.x - nx, a.z - nz}}});
    }
    return zones;
}

} // namespace rt::native::gameplay
