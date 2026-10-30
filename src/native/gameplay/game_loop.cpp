#include "gameplay/game_loop.h"

#include "gameplay/car_physics.h"
#include "gameplay/gauge_rpm.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace rt::native::gameplay {
namespace {

constexpr int kGearboxRow = 0;
constexpr int kAutoShiftPolicy = 0;
constexpr std::int32_t kCoastGoldenSpeedWord = 2148;  // floor(39 mph * 55.07)
constexpr std::int32_t kCoastGoldenMotion = 106;
constexpr std::int32_t kLoadGoldenSpeedWord = 3469;   // floor(63 mph * 55.07)
constexpr std::int32_t kLoadGoldenMotion = 5634;

std::int32_t motionFromSpeedWord(std::int32_t speedWord, bool loaded) {
    if (speedWord <= 0) return 0;
    const std::int64_t numerator = static_cast<std::int64_t>(speedWord) *
        (loaded ? kLoadGoldenMotion : kCoastGoldenMotion);
    const std::int32_t denominator = loaded ? kLoadGoldenSpeedWord : kCoastGoldenSpeedWord;
    return static_cast<std::int32_t>(std::min<std::int64_t>(
        numerator / denominator, std::numeric_limits<std::int32_t>::max()));
}

int highestForwardGear() {
    int highest = 1;
    for (int gear = 1; gear < static_cast<int>(gauge_rpm::kGearRatioTable[kGearboxRow].size()); ++gear) {
        if (gauge_rpm::kGearRatioTable[kGearboxRow][gear] <= 0) break;
        highest = gear;
    }
    return highest;
}

int projectedRpm(std::int32_t motion, int gear) {
    return gauge_rpm::rpmFromMotionState(
        motion, gauge_rpm::kGearRatioTable[kGearboxRow][gear]);
}

// Oracle race-car static config, identical in all 600 capture blanks
// (build/scratch/w9/oracle_step600.csv): accel 1800, response 25, grip and
// gear-row words at car+0x21C/0x22C, damp words at 0x1DC..0x1E4. Gear starts
// 1 like the oracle's byte at +0x1FF (race cars are in gear before GO).
GuestCarBlock makeRaceGuestBlock() {
    GuestCarBlock c{};
    static const int16_t kGrip[8] = {196, 168, 128, 168, 96, 64, 0, 0};
    static const int16_t kGear[8] = {-95, 116, 162, 227, 318, 446, 0, 0};
    for (int i = 0; i < 8; ++i) { c.tireGrip[i] = kGrip[i]; c.gearRow[i] = kGear[i]; }
    c.tireIdx = 0x2000;  // asphalt slot 0 (grip 196)
    c.gear = 1;
    c.damp1DC = c.damp1E0 = c.damp1E4 = 0x1000;
    c.accel = 1800;
    c.response = 25;
    return c;
}

}  // namespace

void advanceDriveline(DrivelineState& state, std::int32_t speedWord, const CarInput& input) {
    if (input.reverse || speedWord < 0) {
        state.longitudinalMotion = 0;
        state.gear = 0;
        state.rpm = 0;
        state.boostTargetRpm = 0;
        return;
    }

    const int maxGear = highestForwardGear();
    if (state.gear < 1 || state.gear > maxGear) state.gear = 1;
    state.longitudinalMotion = motionFromSpeedWord(speedWord, input.gas);
    int rpm = projectedRpm(state.longitudinalMotion, state.gear);
    if (state.gear < maxGear && rpm >= gauge_rpm::kAutoUpshiftRpm[kAutoShiftPolicy]) {
        ++state.gear;
        rpm = projectedRpm(state.longitudinalMotion, state.gear);
    } else if (state.gear > 1 && rpm < gauge_rpm::kAutoDownshiftCurrentRpm[kAutoShiftPolicy] &&
               projectedRpm(state.longitudinalMotion, state.gear - 1) <
                   gauge_rpm::kAutoDownshiftProjectedRpm[kAutoShiftPolicy]) {
        --state.gear;
        rpm = projectedRpm(state.longitudinalMotion, state.gear);
    }
    state.rpm = rpm;
    // AdvanceVehicleMotion's 3000/9000 literals sit on a flag path whose
    // selector is not yet recovered. Preserve the target deterministically
    // for future integration without allowing an unproven target to perturb
    // the ROM-derived coupling projection.
    state.boostTargetRpm = rpm < 9501 ? (input.gas ? 9000 : 3000) : 0;
}

GameLoop::GameLoop(const GameLoopParams& params) : params_(params) { reset(); }

void GameLoop::reset() {
    tasks_ = TaskPool();
    groundY_ = 0.f;  // race course starts at datum; anchors re-set it
    car_ = CarState();
    driveline_ = DrivelineState();
    guest_ = makeRaceGuestBlock();
    camera_ = CameraState();
    raceTracker_ = RaceTracker(0);
    lapCount_ = 0;
    waypointsPassed_ = 0;
    frame_ = 0;
    clock_ = params_.initialClock;
    lighting_ = DayNightOutput();
    raceFinished_ = false;
    raceStarted_ = false;
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
    if (params_.raceMode) {
        // Translated guest longitudinal model (0x21A310/0x218EE0, gear policy
        // A; proven 600/600 oracle blanks in w26_guest_vehicle_test).
        // shortcut: motion is fed from the placeholder speed integrator and
        // tireIdx from static asphalt; both upgrade when the physics task
        // (0x21BA70) velocity/contact chain port lands.
        const uint32_t pad = (input.gas ? 1u : 0u) | (input.brake ? 2u : 0u) |
                             (input.reverse ? 4u : 0u);
        const int motion = car_.speed < 0 ? 0 : car_.speed;
        int outMotion = 0;
        gsCarUpdateMotion(guest_, gearPolicyA, pad, motion, 0, true, &outMotion);
        driveline_.longitudinalMotion = motion;
        driveline_.gear = guest_.gear;
        driveline_.rpm = guest_.rpm;
        driveline_.boostTargetRpm = 0;
    } else {
        advanceDriveline(driveline_, car_.speed, input);
    }
    // Legacy CarState fields remain a coherent presentation mirror. Neither
    // field feeds the existing physics integration, so this cannot perturb it.
    car_.gear = driveline_.gear;
    car_.rpm = driveline_.rpm;
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
    // Analog camera: orbit the look target around the eye by the right stick.
    // Zero stick runs no rotation math at all, so the default view is exact.
    if (camStickX_ != 0.f || camStickY_ != 0.f) {
        const auto beyondDeadzone = [](float v) {
            const float a = std::fabs(v);
            if (a <= kCameraLookDeadzone) return 0.f;
            return std::copysign((a - kCameraLookDeadzone) / (1.f - kCameraLookDeadzone), v);
        };
        Vec3 dir = camera_.target - camera_.eye;
        // Stick right looks toward screen right (+X yaw is screen left in this
        // look-at basis); stick down looks down.
        const float yaw = -beyondDeadzone(camStickX_) * kCameraYawMaxRad;
        const float pitch = -beyondDeadzone(camStickY_) * kCameraPitchMaxRad;
        if (yaw != 0.f) dir = rotateAboutAxis(dir, {0.f, 1.f, 0.f}, yaw);
        if (pitch != 0.f)
            dir = rotateAboutAxis(dir, normalized(cross(dir, {0.f, 1.f, 0.f})), pitch);
        camera_.target = camera_.eye + dir;
    }
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
    s.rpm = driveline_.rpm;
    s.gear = driveline_.gear;
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

void GameLoop::beginRace() { raceStarted_ = true; }

FrameSnapshot GameLoop::tick(float /*dt*/, const MenuInput& /*menuInput*/) {
    AiParams aiParams;
    aiParams.arrivalRadius = params_.arrivalRadius;
    aiParams.steerSlewPerTick = params_.steerSlewPerTick;
    aiParams.maxSpeedStraight = params_.maxSpeedStraight;
    aiParams.brakeLookahead = params_.brakeLookahead;
    const int n = static_cast<int>(world_.roadWaypoints.size());

    tasks_.tick();  // (a) task walker pass
    if (params_.raceMode) raceTracker_.tick();
    // ROM RunCountdownTimerTask (0x22DAE8): the GO bit goes into the race
    // word at tick 0xF0 and SetVehicleMode switches to the free mode 6 there
    // (tick/0x3c + 2), so even the attract-demo AI is held before GO.
    // shortcut: input blanked for 240 race frames; upgrade when the READY/GO
    // banner state machine (hud_layout RaceStart*) lands and can gate on it.
    const bool held =
        params_.raceMode && raceStarted_ && raceTracker_.elapsedFrames() <= 240;

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
        integrateCar(held ? CarInput{} : in);  // (c) car physics, heading, and position integration
    }
    return makeSnapshot();
}

FrameSnapshot GameLoop::tickPlayer(float /*dt*/, const MenuInput& /*menuInput*/, const CarInput& input) {
    tasks_.tick();  // (a) task walker pass
    if (params_.raceMode) raceTracker_.tick();
    // ROM RunCountdownTimerTask (0x22DAE8): the GO bit goes into the race
    // word at tick 0xF0 and SetVehicleMode switches to the free mode 6 there
    // (tick/0x3c + 2).  Blank the driver until then; the car holds at speed 0.
    // shortcut: input blanked for 240 race frames; upgrade when the READY/GO
    // banner state machine (hud_layout RaceStart*) lands and can gate on it.
    const bool held =
        params_.raceMode && raceStarted_ && raceTracker_.elapsedFrames() <= 240;
    for (int k = 0; k < params_.physicsTicksPerFrame; ++k)
        integrateCar(held ? CarInput{} : input);
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
