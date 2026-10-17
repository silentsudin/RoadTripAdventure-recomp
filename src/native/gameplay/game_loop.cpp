#include "gameplay/game_loop.h"

#include "gameplay/car_physics.h"

#include <algorithm>
#include <cmath>

namespace rt::native::gameplay {

GameLoop::GameLoop(const GameLoopParams& params) : params_(params) { reset(); }

void GameLoop::reset() {
    tasks_ = TaskPool();
    groundY_ = 0.f;  // race course starts at datum; anchors re-set it
    car_ = CarState();
    camera_ = CameraState();
    raceTracker_ = RaceTracker(0);
    lapCount_ = 0;
    waypointsPassed_ = 0;
    frame_ = 0;
    clock_ = params_.initialClock;
    lighting_ = DayNightOutput();
    raceFinished_ = false;
    pipelineTicks_ = 0;

    path_ = world_.roadWaypoints;
    const int n = static_cast<int>(world_.roadWaypoints.size());
    ai_ = AiState();
    if (n > 0) {
        ai_.position = world_.roadWaypoints[0];
        ai_.waypointIndex = 1 % n;
        const Vec2 d = world_.roadWaypoints[ai_.waypointIndex] - world_.roadWaypoints[0];
        ai_.heading = std::atan2(d.x, d.z);
    }
    // Frame task stands in for the walker's per-frame handlers.
    tasks_.spawn([this](Task&, TaskPool&) { ++pipelineTicks_; });
}

float GameLoop::roadDistance(Vec2 p) const {
    const auto& w = world_.roadWaypoints;
    const std::size_t n = w.size();
    float best = 1e30f;
    for (std::size_t i = 0; i < n; ++i) {
        const Vec2 a = w[i], b = w[(i + 1) % n];
        const Vec2 ab = b - a, ap = p - a;
        const float denom = std::max(dot(ab, ab), 1e-12f);
        const float t = std::clamp(dot(ap, ab) / denom, 0.f, 1.f);
        best = std::min(best, std::sqrt(lengthSq(p - (a + ab * t))));
    }
    return best;
}

void GameLoop::integrateCar(const CarInput& input) {
    tickCar(car_, input, world_.surfaceMaterialAt(ai_.position.x, ai_.position.z));
    const float sp = static_cast<float>(car_.speed);
    const float steer = static_cast<float>(car_.steeringAngle) / static_cast<float>(kSteerLock);
    ai_.heading = wrapAngle(ai_.heading + steer * (sp / params_.speedRef) * params_.turnRate);
    ai_.position.x += std::sin(ai_.heading) * sp * params_.unitsPerSpeedTick;
    ai_.position.z += std::cos(ai_.heading) * sp * params_.unitsPerSpeedTick;
}

FrameSnapshot GameLoop::makeSnapshot() {
    ++frame_;
    clock_ = advanceTimeOfDayCounter(clock_);
    updateTimeOfDayLight(0.0f, clock_, lighting_);

    // Camera and projection are in render space.
    // Chase-camera construction parameterized here (via GameLoopParams wired
    // to CameraParams) so callers/tests tune without per-scene rebuilds.
    // See docs/native-reference/camera-framing.md for targets + citations.
    CarPose pose;
    pose.position = {ai_.position.x * params_.renderScale, groundY_,
                     ai_.position.z * params_.renderScale};
    pose.heading = ai_.heading;
    CameraParams cp{};
    cp.distanceNear = params_.chaseBackDistance;
    cp.distanceFar  = params_.chaseBackDistance;
    cp.baseHeight   = params_.chaseBaseHeight;
    cp.pitchLow     = params_.chasePitchRad;
    cp.pitchHigh    = params_.chasePitchRad;
    cp.lookAtHeight = params_.chaseLookAtHeight;
    updateCamera(camera_, pose, static_cast<float>(car_.speed) * params_.cameraSpeedScale, cp);
    const Mat4 v = lookAtMatrix(camera_.eye, camera_.target);

    FrameSnapshot s;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) s.view[c * 4 + r] = v.m[r * 4 + c];
    const float t = 1.f / std::tan(params_.fovY * 0.5f);
    s.proj[0] = t / params_.aspect;
    s.proj[5] = -t;
    s.proj[10] = params_.farZ / (params_.nearZ - params_.farZ);
    s.proj[11] = -1.f;
    s.proj[14] = params_.nearZ * params_.farZ / (params_.nearZ - params_.farZ);

    s.carPosition = ai_.position;
    s.heading = ai_.heading;
    s.speed = static_cast<float>(car_.speed);
    s.cameraEye = camera_.eye;
    s.cameraTarget = camera_.target;
    s.visibleCells = world_.visibleCells(s.carPosition, params_.viewDistance);
    s.waypointIndex = ai_.waypointIndex;
    s.lapCount = lapCount_;
    s.waypointsPassed = waypointsPassed_;
    s.frame = frame_;
    s.clock = clock_;
    s.lighting = lighting_;
    if (params_.raceMode) {
        s.lap = static_cast<int>(raceTracker_.lapCount());
        s.elapsedFrames = raceTracker_.elapsedFrames();
        s.bestLap = raceTracker_.bestLap();
        s.finished = raceFinished_;
        s.rank = getRaceTimeRank(raceFinished_ ? raceTracker_.totalFrames() : raceTracker_.elapsedFrames());
    }
    return s;
}

void GameLoop::completeLap() {
    ++lapCount_;
    if (!params_.raceMode) return;

    raceTracker_.completeLap();
    if (!raceFinished_ && lapCount_ >= params_.totalLaps) {
        raceTracker_.finishRace();
        raceFinished_ = true;
    }
}

FrameSnapshot GameLoop::tick(float /*dt*/, const MenuInput& /*menuInput*/) {
    AiParams aiParams;
    aiParams.arrivalRadius = params_.arrivalRadius;
    aiParams.steerSlewPerTick = params_.steerSlewPerTick;
    aiParams.maxSpeedStraight = params_.maxSpeedStraight;
    aiParams.brakeLookahead = params_.brakeLookahead;
    const int n = static_cast<int>(world_.roadWaypoints.size());

    tasks_.tick();  // (a) task walker pass
    if (params_.raceMode) raceTracker_.tick();

    for (int k = 0; k < params_.physicsTicksPerFrame; ++k) {
        const int before = ai_.waypointIndex;
        if (n > 0 && ai_.waypointIndex >= n) {  // tickAi stops at the last waypoint: wrap = lap
            ai_.waypointIndex = 0;
            completeLap();
        }
        const AiOutput cmd = tickAi(ai_, car_, path_, aiParams);  // (b)
        if (ai_.waypointIndex != before) ++waypointsPassed_;
        CarInput in;
        in.gas = cmd.throttle;
        in.brake = cmd.brake;
        in.steer = cmd.steer;
        integrateCar(in);  // (c) car physics, heading, and position integration
    }
    return makeSnapshot();
}

FrameSnapshot GameLoop::tickPlayer(float /*dt*/, const MenuInput& /*menuInput*/, const CarInput& input) {
    tasks_.tick();  // (a) task walker pass
    if (params_.raceMode) raceTracker_.tick();
    for (int k = 0; k < params_.physicsTicksPerFrame; ++k) integrateCar(input);
    return makeSnapshot();
}

void GameLoop::setCarPose(float x, float z, float headingRadians, float yGameUnits) {
    // Authoritative pose for AI/explore (world units; matches ai_ fields read
    // by makeSnapshot and CarPose passed to updateCamera). FREEZES motion:
    // speed and steering-angle clear with the pose, so an anchored car (e.g.
    // the Peach Town square) does not drift on the next idle tick. Gear/rpm/
    // odometer are left alone (presentation state, no integration effect).
    // yGameUnits is the authoritative ground elevation (game units): the
    // oracle Peach Town square sits at y=30 (oracle-golden-frames.md:54),
    // so a y=0 chase camera would render UNDERNEATH the town plaza.
    ai_.position = {x, z};
    ai_.heading = headingRadians;
    groundY_ = yGameUnits * params_.renderScale;
    car_.speed = 0;
    car_.steeringAngle = 0;
    car_.brakeRamp = 0;
    // Refresh camera to the new pose so the next snapshot does not snap from
    // a stale prior camera state (sets initialized=false then update snaps).
    // Thread affinity: simulation thread only (no locking).
    camera_.initialized = false;
    CarPose pose;
    pose.position = {x * params_.renderScale, groundY_, z * params_.renderScale};
    pose.heading = headingRadians;
    CameraParams cp{};
    cp.distanceNear = params_.chaseBackDistance;
    cp.distanceFar  = params_.chaseBackDistance;
    cp.baseHeight   = params_.chaseBaseHeight;
    cp.pitchLow     = params_.chasePitchRad;
    cp.pitchHigh    = params_.chasePitchRad;
    cp.lookAtHeight = params_.chaseLookAtHeight;
    updateCamera(camera_, pose, static_cast<float>(car_.speed) * params_.cameraSpeedScale, cp);
}

}  // namespace rt::native::gameplay
