// Authored simulation frame: the host equivalent of gameMainLoop (0x2003B8)'s
// per-frame update — task walker (0x204F98) -> car/AI/camera update -> field
// cull (cellVisible 0x21FA38 via World::visibleCells) -> snapshot for the
// renderer. Pure C++, deterministic, no game data, no GPU. The draw stays in
// preview/. Lighting/day-night is not part of this slice.
//
// Wiring is extracted from tools/drive_main.cpp (rt-asset-drive).
#pragma once

#include "gameplay/ai.h"
#include "gameplay/camera.h"
#include "gameplay/car_state.h"
#include "gameplay/daynight.h"
#include "gameplay/menu.h"
#include "gameplay/race.h"
#include "gameplay/task.h"
#include "gameplay/world.h"

#include <cstdint>
#include <vector>

namespace rt::native::gameplay {

// Chase-camera framing targets (docs/native-reference/camera-framing.md:22-31,
// derived from oracle-golden-frames + ROM preset-0 at 0x0029ce00).
// Render units (RU); preserve current native eye/target math.
inline constexpr float kChaseBackRu     = 7.000f;      // ROM CONFIRMED: preset-0 (0,2,-7)
inline constexpr float kChaseHeightRu   = 2.000f;      // ROM CONFIRMED: base Y component
inline constexpr float kChasePitchRad   = 0.08590293f; // ROM CONFIRMED: 0x0380 turns (+4.921875 deg)
inline constexpr float kChaseLookAtRu   = 1.500f;      // UNDOCUMENTED host convention (car centre + up*1.5)
inline constexpr float kChaseFovYRad    = 1.030f;      // UNDOCUMENTED-assumed (see camera-framing.md:31)

// Drive-tuned defaults (rt-asset-drive, CAR0/Q00: 2 laps/240 frames, max
// centreline deviation 43.8 vs road half width 60).
struct GameLoopParams {
    // AI (AiParams)
    float arrivalRadius = 30.f;
    float steerSlewPerTick = 0.5f;
    float maxSpeedStraight = 20000.f;
    float brakeLookahead = 0.f;

    // Integration
    int physicsTicksPerFrame = 8;
    float unitsPerSpeedTick = 4.f / 20000.f;  // world units/tick at kSpeedMax (20000)
    float turnRate = 0.2f;                    // rad/tick at full lock & full speed
    float speedRef = 20000.f;                 // speed that scales the turn rate

    // Camera / projection
    float renderScale = 0.1f;   // world units -> render units
    float cameraSpeedScale = 8.f;
    float fovY = kChaseFovYRad;  // 1.03 per camera-framing.md (UNDOCUMENTED-assumed host)
    float aspect = 800.f / 600.f;
    float nearZ = 0.1f;
    float farZ = 500.f;

    // Chase camera (wired to updateCamera; overridable for app/tests without
    // recompiling per-scene; defaults = camera-framing.md targets).
    float chaseBackDistance = kChaseBackRu;
    float chaseBaseHeight   = kChaseHeightRu;
    float chasePitchRad     = kChasePitchRad;
    float chaseLookAtHeight = kChaseLookAtRu;

    // Time of day: starting clock (vblanks since midnight, wraps at kDayTicks).
    std::uint32_t initialClock = 0;

    // Race timing is opt-in so the default remains a free drive.
    bool raceMode = false;
    int totalLaps = 3;

    // Cull
    float viewDistance = kDefaultViewDistance;
};

struct FrameSnapshot {
    Vec2 carPosition;        // world units (x east, z north)
    float heading = 0.f;     // radians, 0 = +Z
    float speed = 0.f;       // car-physics speed units
    Vec3 cameraEye;          // render units
    Vec3 cameraTarget;       // render units
    float view[16] = {};     // column-major (Vulkan upload order), render units
    float proj[16] = {};     // column-major, Vulkan clip conventions (y down, depth 0..1)
    std::vector<Cell> visibleCells;
    int waypointIndex = 0;
    int lapCount = 0;
    int waypointsPassed = 0;
    std::uint64_t frame = 0;  // frames simulated since reset()
    std::uint32_t clock = 0;  // time-of-day clock after this frame's advance
    DayNightOutput lighting;  // updateTimeOfDayLight(0, clock) for this frame
    int lap = 0;                       // completed race laps (race mode only)
    std::uint32_t elapsedFrames = 0;   // race frames elapsed (race mode only)
    std::uint32_t bestLap = 0;         // best completed lap (race mode only)
    bool finished = false;             // race crossed the configured lap count
    int rank = 0;                      // getRaceTimeRank for the current/final time
};

class GameLoop {
public:
    explicit GameLoop(const GameLoopParams& params = {});

    void reset();

    // One fixed-step simulation frame. dt is accepted for the frame-pipeline
    // interface but the step is fixed (determinism); menuInput is reserved for
    // the menu/pause layer and does not steer the self-driving car.
    FrameSnapshot tick(float dt, const MenuInput& menuInput);

    // One fixed-step simulation frame driven by host/controller input instead
    // of the waypoint AI. Menu input remains reserved for a future pause layer.
    FrameSnapshot tickPlayer(float dt, const MenuInput& menuInput, const CarInput& input);

    const World& world() const { return world_; }
    const GameLoopParams& params() const { return params_; }
    void setProjectionAspect(float aspect) { params_.aspect = aspect; }
    const TaskPool& tasks() const { return tasks_; }
    // Distance from a world point to the closed road centreline.
    float roadDistance(Vec2 p) const;

    // Sets the AI/authoritative car pose (world units; same fields read by
    // makeSnapshot and passed to updateCamera). Refreshes internal camera
    // state from the pose so the next snapshot continues smoothly (no snap
    // from stale prior camera). Thread affinity: simulation thread only.
    void setCarPose(float x, float z, float headingRadians);

private:
    GameLoopParams params_;
    TaskPool tasks_;
    World world_;
    CarState car_;
    AiState ai_;
    CameraState camera_;
    WaypointPath path_;
    RaceTracker raceTracker_{0};
    int lapCount_ = 0;
    int waypointsPassed_ = 0;
    std::uint64_t frame_ = 0;
    std::uint32_t clock_ = 0;
    DayNightOutput lighting_;
    bool raceFinished_ = false;
    std::uint32_t pipelineTicks_ = 0;  // bumped by the spawned frame task

    void integrateCar(const CarInput& input);
    void completeLap();
    FrameSnapshot makeSnapshot();
};

}  // namespace rt::native::gameplay
