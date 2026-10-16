// Authored render-frame interpolation implementation. See interp.h for the
// model and its frame-timing.md grounding. Pure host C++, no game data.
#include "gameplay/interp.h"

#include "gameplay/camera.h"

#include <algorithm>
#include <cmath>

namespace rt::native::gameplay {

RenderPose renderPoseOf(const FrameSnapshot& snapshot) {
    RenderPose pose;
    pose.carPosition = snapshot.carPosition;
    pose.heading = snapshot.heading;
    pose.cameraEye = snapshot.cameraEye;
    pose.cameraTarget = snapshot.cameraTarget;
    return pose;
}

FrameSnapshot snapshotWithRenderPose(const FrameSnapshot& snapshot, const RenderPose& pose) {
    FrameSnapshot out = snapshot;
    out.carPosition = pose.carPosition;
    out.heading = pose.heading;
    out.cameraEye = pose.cameraEye;
    out.cameraTarget = pose.cameraTarget;
    // Rebuild the view matrix from the pose exactly like
    // GameLoop::makeSnapshot: right-handed look-at, transposed into the
    // column-major Vulkan upload order.
    const Mat4 v = lookAtMatrix(pose.cameraEye, pose.cameraTarget);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            out.view[c * 4 + r] = v.m[r * 4 + c];
    return out;
}

float lerpAngleShortestArc(float a, float b, float t) {
    const float delta = wrapAngle(b - a);  // shortest signed arc in [-pi, pi]
    return a + delta * t;
}

RenderPose lerpRenderPose(const RenderPose& prev, const RenderPose& cur, float t) {
    t = std::clamp(t, 0.f, 1.f);
    RenderPose pose;
    pose.carPosition.x = prev.carPosition.x + (cur.carPosition.x - prev.carPosition.x) * t;
    pose.carPosition.z = prev.carPosition.z + (cur.carPosition.z - prev.carPosition.z) * t;
    pose.heading = lerpAngleShortestArc(prev.heading, cur.heading, t);
    pose.cameraEye.x = prev.cameraEye.x + (cur.cameraEye.x - prev.cameraEye.x) * t;
    pose.cameraEye.y = prev.cameraEye.y + (cur.cameraEye.y - prev.cameraEye.y) * t;
    pose.cameraEye.z = prev.cameraEye.z + (cur.cameraEye.z - prev.cameraEye.z) * t;
    pose.cameraTarget.x = prev.cameraTarget.x + (cur.cameraTarget.x - prev.cameraTarget.x) * t;
    pose.cameraTarget.y = prev.cameraTarget.y + (cur.cameraTarget.y - prev.cameraTarget.y) * t;
    pose.cameraTarget.z = prev.cameraTarget.z + (cur.cameraTarget.z - prev.cameraTarget.z) * t;
    return pose;
}

FixedStepClock::FixedStepClock(int catchUpCap)
    : catchUpCap_(std::max(1, catchUpCap)) {}

void FixedStepClock::addElapsed(double seconds) {
    if (seconds > 0.0) accumulatorSeconds_ += seconds;
}

bool FixedStepClock::stepDue() const {
    return accumulatorSeconds_ >= kStepSeconds;
}

int FixedStepClock::stepsReady() const {
    if (accumulatorSeconds_ < kStepSeconds) return 0;
    const double owed = std::floor(accumulatorSeconds_ / kStepSeconds);
    return static_cast<int>(std::min(owed, static_cast<double>(catchUpCap_)));
}

void FixedStepClock::takeStep() {
    if (accumulatorSeconds_ < kStepSeconds) return;
    accumulatorSeconds_ -= kStepSeconds;
    ++stepsTaken_;
}

float FixedStepClock::alpha() const {
    const double alpha = accumulatorSeconds_ / kStepSeconds;
    return static_cast<float>(std::clamp(alpha, 0.0, 1.0));
}

std::uint64_t FixedStepClock::stepsTaken() const { return stepsTaken_; }
double FixedStepClock::accumulatorSeconds() const { return accumulatorSeconds_; }
int FixedStepClock::catchUpCap() const { return catchUpCap_; }

}  // namespace rt::native::gameplay
