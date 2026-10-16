#include "gameplay/nav_drive.h"

#include <cmath>

namespace rt::native::gameplay {
namespace {

float distance(Pt a, Pt b) {
    const float dx = b.x - a.x;
    const float dz = b.z - a.z;
    return std::sqrt(dx * dx + dz * dz);
}

// 0 = +Z: the authored render convention (preview's sin/cos heading basis).
float headingBetween(Pt a, Pt b) { return std::atan2(b.x - a.x, b.z - a.z); }

} // namespace

void NavDrive::buildSegments(const NavPath& path) {
    const auto append = [this](Pt start, Pt end) {
        const float length = distance(start, end);
        if (!(length > 0.f)) return;  // ignore zero-length edges
        segments_.push_back({start, end, length, totalLength_});
        totalLength_ += length;
    };
    for (std::size_t i = 1; i < path.centerline.size(); ++i)
        append(path.centerline[i - 1], path.centerline[i]);
    if (path.closed && path.centerline.size() > 1)
        append(path.centerline.back(), path.centerline.front());
}

NavDrive::NavDrive(const NavPath& path) {
    closed_ = path.closed;
    if (!path.centerline.empty()) start_ = path.centerline.front();
    buildSegments(path);
}

NavPose NavDrive::poseAt(float offset) const {
    NavPose pose;
    pose.lap = lap_;
    pose.arcLength = offset;
    if (segments_.empty()) {
        pose.position = start_;
        return pose;
    }
    float remaining = offset;
    for (const Segment& segment : segments_) {
        if (remaining <= segment.length) {
            const float t = remaining / segment.length;
            pose.position = {segment.start.x + (segment.end.x - segment.start.x) * t,
                             segment.start.z + (segment.end.z - segment.start.z) * t};
            pose.heading = headingBetween(segment.start, segment.end);
            return pose;
        }
        remaining -= segment.length;
    }
    // Offset at or past the last segment's end (open path clamped at length).
    const Segment& last = segments_.back();
    pose.position = last.end;
    pose.heading = headingBetween(last.start, last.end);
    return pose;
}

NavPose NavDrive::step(float speedUnitsPerFrame) {
    if (std::isfinite(speedUnitsPerFrame) && speedUnitsPerFrame > 0.f && totalLength_ > 0.f) {
        cursor_ += speedUnitsPerFrame;
        if (closed_) {
            while (cursor_ >= totalLength_) {
                cursor_ -= totalLength_;
                ++lap_;
            }
        } else if (cursor_ > totalLength_) {
            cursor_ = totalLength_;
        }
    }
    return poseAt(cursor_);
}

NavPose NavDrive::pose() const { return poseAt(cursor_); }

void NavDrive::reset() {
    cursor_ = 0.f;
    lap_ = 0;
}

} // namespace rt::native::gameplay
