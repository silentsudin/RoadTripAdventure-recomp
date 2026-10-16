// Fixed-step waypoint traversal over a reconstructed navigation centerline.
#pragma once

#include "gameplay/nav_path.h"

#include <cstddef>
#include <vector>

namespace rt::native::gameplay {

struct WalkResult {
    float totalLength = 0.f;
    float visitedLength = 0.f;
    float visitedFraction = 0.f;
    bool completedLap = false;
    // Includes the starting position, each fixed arc-length cursor position,
    // and the exact final endpoint (or start again for a completed closed lap).
    std::vector<Pt> cursorPositions;
};

// Traverses the NavPath centerline by arc length.  Closed paths include the
// final edge back to centerline.front(); zero-length edges are ignored.
WalkResult walkNavPath(const NavPath& path, float progressStep);

struct NodeCoverage {
    std::size_t visitedNodes = 0;
    std::size_t totalNodes = 0;
    float fraction = 0.f;
};

// Diagnostic only: reports which unique transition nodes in path.nodeOrder
// have their midpoint visited by a fixed-step cursor within tolerance.  It
// never changes traversal or route-progress decisions.
NodeCoverage nodeCoverage(const NavPath& path, const assets::NavGraph& graph,
                          const WalkResult& walk, float tolerance);

} // namespace rt::native::gameplay
