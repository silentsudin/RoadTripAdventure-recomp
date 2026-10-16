// Bridge ROM navigation centerlines to authored RaceRoute zones.  A zone
// strip is emitted per consecutive centerline pair (plus the closing pair
// when the path closed).  Centerline units are tile-local 0..1600 and are
// used 1:1 as authored world units until the authentic fixed-point
// geography mapping lands (docs/native-reference/nav-route-consumer.md).
// No speed semantic is transferred: the segment auxiliary floats remain
// OPEN, so callers keep choosing minArrivalSpeed themselves.
#pragma once

#include "gameplay/nav_path.h"
#include "gameplay/route.h"

#include <vector>

namespace rt::native::gameplay {

std::vector<RouteZone> buildRouteZones(const NavPath& path, float halfWidth = 55.f);

}  // namespace rt::native::gameplay
