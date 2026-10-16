// Deterministic kinematic follower over a NavPath centerline.  Unlike the
// diagnostic fixed-step `nav_walkable` walk (which materializes every sampled
// cursor at once), this keeps a single arc-length cursor that a host advances
// by `speed` units per frame, wrapping on a closed lap and counting laps.  It
// is pure geometry: no car dynamics, no collision, no branch selection, and no
// game data.  Its purpose is to drive the render-visible demo car along an
// authentic constructed centerline.
#pragma once

#include "gameplay/nav_path.h"

#include <cstddef>
#include <vector>

namespace rt::native::gameplay {

// Pose at the current cursor.  Units match the NavPath centerline (tile-local);
// heading is radians with 0 = +Z, matching the authored render convention.
struct NavPose {
    Pt position;           // centerline units
    float heading = 0.f;   // radians, 0 = +Z (atan2(dx, dz))
    float arcLength = 0.f; // cursor tracked within the current lap
    int lap = 0;           // completed laps (closed paths only)
};

// A single arc-length cursor over the centerline polyline.  Zero-length edges
// are ignored (as in `nav_walkable`); a closed path includes the closing edge.
// Every method is deterministic and free of host time, randomness, or I/O.
class NavDrive {
public:
    explicit NavDrive(const NavPath& path);

    // Advances the cursor by `speedUnitsPerFrame` and returns the new pose.
    // Non-finite or non-positive speed is a deterministic no-op.  On a closed
    // path the cursor wraps past the total length and `lap` increments once
    // per full traversal; on an open path it clamps at the final point.
    NavPose step(float speedUnitsPerFrame);

    // Pose at the current cursor without advancing.
    NavPose pose() const;

    // Restores the initial cursor (start of the polyline, zero laps).
    void reset();

    std::size_t segmentCount() const { return segments_.size(); }
    float totalLength() const { return totalLength_; }
    bool closed() const { return closed_; }
    bool empty() const { return segments_.empty(); }

private:
    struct Segment {
        Pt start;
        Pt end;
        float length = 0.f;
        float startDistance = 0.f;
    };

    void buildSegments(const NavPath& path);
    NavPose poseAt(float offset) const;

    std::vector<Segment> segments_;
    Pt start_;             // fallback pose for an empty/degenerate centerline
    float totalLength_ = 0.f;
    bool closed_ = false;
    float cursor_ = 0.f;
    int lap_ = 0;
};

} // namespace rt::native::gameplay
