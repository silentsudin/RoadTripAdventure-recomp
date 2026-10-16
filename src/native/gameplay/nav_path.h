// Deterministic host-side traversal of ROM navigation graphs.  This is a
// centerline reconstruction aid, not a replacement for the game's position-
// driven FUN_00250068 follower.
#pragma once

#include "assets/nav_routes.h"

#include <cstddef>
#include <vector>

namespace rt::native::gameplay {

struct Pt {
    float x = 0.f;
    float z = 0.f;
};

// The graph stores two candidate pairs.  SideA is the default convention;
// SideB traverses the opposite pair and reverses segment-pair headings.
enum class NavPathDirection { SideA, SideB };

struct NavPath {
    std::vector<Pt> centerline;
    std::vector<int> nodeOrder;
    bool closed = false;

    float totalLength() const;
};

// Starts at transition zero by default.  A path stops at a dangling candidate,
// an already visited nonzero transition, or exhausted graph.  Returning to the
// initial node closes the path.
NavPath buildNavPath(const assets::NavGraph& graph, std::size_t startTransition = 0,
                     NavPathDirection direction = NavPathDirection::SideA);

} // namespace rt::native::gameplay
