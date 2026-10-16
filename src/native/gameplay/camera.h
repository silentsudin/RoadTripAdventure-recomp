// Authored chase camera: plain C++, no guest RAM offsets, no PS2 types.
//
// Mirrors the documented original (docs/game-mechanics/car-ai-and-camera.md §2):
// a per-camera state with zoom/pitch/yaw channels and a mode field (the original
// keeps these as shorts at cam-state +0x14/+0x16/+0x1A/+0x1C), a spring block
// for the smoothed chase offset (original DAT_0172FF30: smoothed lateral offset
// and height plus their rate limits), pitch slewed at most a fixed step per
// frame (original ±0x80 in 0x10000 angle units), and a look-at view matrix
// built per tick (original buildCamera 0x21F698). All values here are authored
// floats/radians; no game data.
#pragma once

#include "gameplay/vec.h"

#include <cmath>
#include <cstdint>

namespace rt::native::gameplay {

// 4x4 matrix, row-major (m[row * 4 + col]).
struct Mat4 {
    float m[16] = {1.f, 0.f, 0.f, 0.f,
                   0.f, 1.f, 0.f, 0.f,
                   0.f, 0.f, 1.f, 0.f,
                   0.f, 0.f, 0.f, 1.f};
};

inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = 2.f * kPi;

// Wrap an angle to (-pi, pi].
inline float wrapAngle(float a) {
    a = std::fmod(a + kPi, kTwoPi);
    if (a < 0.f) a += kTwoPi;
    return a - kPi;
}

// ---------------------------------------------------------------- inputs

// Car pose for the camera. Heading convention: forward = (sin h, 0, cos h),
// i.e. heading 0 looks down +Z. Position is in world units.
struct CarPose {
    Vec3 position;
    float heading = 0.f;  // radians
};

inline Vec3 carForward(const CarPose& car) {
    return {std::sin(car.heading), 0.f, std::cos(car.heading)};
}

// ---------------------------------------------------------------- tuning

struct CameraParams {
    // Retained for source compatibility with callers that supply a speed. The
    // shipped chase task selects a camera preset; it does not ramp its default
    // preset from vehicle speed (RunCarCameraTask, 0x0021e690).
    float speedRef = 163840.f;

    // Default chase-preset offset (preset 0 at 0x0029ce00): (0, 2, -7, 1).
    // The host orbit represents its -Z component as distance and its Y
    // component as baseHeight.
    float distanceNear = 7.f;
    float distanceFar = 7.f;
    float zoomEase = 1.f;
    float zoomSlewPerTick = 7.f;

    // Default preset pitch is 0x0380 in a 0x10000-turn angle. The original
    // targets this fixed value and clamps movement to +/-0x80 each 60 Hz tick
    // (0x0021eb50..0x0021eb74), rather than scaling it by car speed.
    float pitchLow = 0.08590293f;
    float pitchHigh = 0.08590293f;
    float pitchSlewPerTick = 0.02454369f;

    // Yaw follows the car heading through the shortest arc.
    float yawEase = 0.15f;

    // Host-only look-at convention; the ELF builds basis vectors rather than a
    // host-style target point, so no direct source scalar exists for this.
    float lookAtHeight = 1.5f;
    // Preset-0 Y offset at 0x0029ce04 (instruction-verified by the 32-byte
    // copy in LoadCameraPreset, 0x0021fae8).
    float baseHeight = 2.0f;

    // DAT_00332c30..44 are +/-0.001f. ComputeChaseCameraBasis
    // (0x0021dc18) adds this signed amount to the previous delta and clips it
    // to the remaining displacement; this is an acceleration-limited slew,
    // not the previous guessed damped spring.
    float springAccelerationPerTick = 0.001f;

    // Projection values used by buildCamera (0x0021f698). They are exposed as
    // evidence-backed authored constants; this view-only module does not own
    // FrameSnapshot's host projection matrix.
    float projectionFocal = 500.f;          // preset 0 +0x10, 0x0029ce10
    float projectionHorizontalScale = 0.47f; // DAT_00332c80
    float projectionVerticalScale = 0.8f;    // DAT_00332c84
    float viewNear = 2048.f;                 // 0x45000000 -> 0x0021f910
    float viewFar = 2048.f;                  // 0x45000000 -> 0x0021f910
};

// ---------------------------------------------------------------- state

// Camera modes, mirroring the original cam-state mode int (+0x1C):
// mode 0 = fixed chase offset (spring bypassed), nonzero = spring-smoothed.
enum CameraMode : std::int32_t {
    kCameraModeFixed = 0,
    kCameraModeSpring = 1,
};

struct CameraState {
    // Eased channels (original: zoom float +0x10, pitch short +0x16, yaw-follow
    // short +0x1A, mode +0x1C).
    float distance = 9.f;  // current camera-to-target distance (zoom)
    float pitch = 0.0873f; // radians, camera elevation over the car
    float yaw = 0.f;       // radians, heading the camera orbits on
    std::int32_t mode = kCameraModeSpring;

    // Spring block: smoothed lateral offset / height + their rates.
    float springLateral = 0.f;
    float springLateralRate = 0.f;
    float springHeight = 0.f;
    float springHeightRate = 0.f;

    // Per-tick outputs (the original writes these into the camera block).
    Vec3 eye;       // camera position
    Vec3 target;    // look-at point
    Vec3 forward;   // normalized view direction (target - eye)
    Mat4 view;      // right-handed look-at view matrix (camera looks down -Z)

    bool initialized = false;  // first tick snaps instead of easing
};

// ---------------------------------------------------------------- update

// One camera tick: eases zoom/pitch/yaw toward speed-scaled targets, runs the
// spring block, and rebuilds the look-at basis and view matrix. The camera is
// kept behind (against carForward) and above the car.
void updateCamera(CameraState& cam, const CarPose& car, float speed,
                  const CameraParams& params = {});

// Right-handed look-at view matrix (OpenGL convention: camera looks down -Z).
Mat4 lookAtMatrix(Vec3 eye, Vec3 target, Vec3 up = {0.f, 1.f, 0.f});

}  // namespace rt::native::gameplay
