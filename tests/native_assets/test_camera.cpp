// Authored chase-camera contract tests (src/native/gameplay/camera.*).
// Concrete numeric assertions; no game data, no framework.
#include "../../src/native/gameplay/camera.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static bool nearEq(float a, float b, float eps) { return std::fabs(a - b) <= eps; }

// Binary pins: preset 0 at 0x29ce00 and the projection constants consumed by
// buildCamera (0x21f698).  These are not presentation guesses.
static void testExtractedCameraConstants() {
    const CameraParams p;
    CHECK(nearEq(p.distanceNear, 7.f, 1e-6f));
    CHECK(nearEq(p.distanceFar, 7.f, 1e-6f));
    CHECK(nearEq(p.baseHeight, 2.f, 1e-6f));
    CHECK(nearEq(p.pitchLow, 0.08590293f, 1e-6f));  // 0x0380 turns
    CHECK(nearEq(p.pitchHigh, p.pitchLow, 1e-6f));
    CHECK(nearEq(p.pitchSlewPerTick, 0.02454369f, 1e-6f));  // 0x80 turns
    CHECK(nearEq(p.springAccelerationPerTick, 0.001f, 1e-8f));
    CHECK(nearEq(p.projectionFocal, 500.f, 1e-6f));
    CHECK(nearEq(p.projectionHorizontalScale, 0.47f, 1e-6f));  // 0x00332c80
    CHECK(nearEq(p.projectionVerticalScale, 0.8f, 1e-6f));     // 0x00332c84
    CHECK(nearEq(p.viewNear, 2048.f, 1e-6f));
    CHECK(nearEq(p.viewFar, 2048.f, 1e-6f));
}

// Run the camera to a settled state at a constant speed/heading.
static CameraState settle(const CarPose& car, float speed, int ticks = 600) {
    CameraState cam;
    for (int i = 0; i < ticks; ++i) updateCamera(cam, car, speed);
    return cam;
}

// The camera sits behind the car (view direction agrees with car forward, and
// the eye is on the negative-forward side) and above it.
static void testBehindAndAbove() {
    const float headings[] = {0.f, 0.7f, 2.5f, -1.3f, 3.0f};
    for (float h : headings) {
        CarPose car{{10.f, 0.f, -4.f}, h};
        CameraState cam = settle(car, 60000.f);
        const Vec3 fwd = carForward(car);
        // View direction roughly along car forward.
        CHECK(dot(cam.forward, fwd) > 0.95f);
        // Eye strictly behind: (carPos - eye) has a positive forward component
        // and a negative backward one.
        const Vec3 toCar = car.position - cam.eye;
        CHECK(dot(toCar, fwd) > 1.f);          // car is well in front of the eye
        CHECK(cam.eye.y > car.position.y);      // camera rides above the car
        CHECK(dot(normalized(toCar), fwd) > 0.3f);  // mostly ahead, not sideways
    }
}

// The shipped task chooses a preset rather than a speed ramp; the default
// preset's chase distance remains fixed for every host speed input.
static void testDefaultPresetDistanceIsSpeedIndependent() {
    CarPose car{{0.f, 0.f, 0.f}, 0.4f};
    const float speeds[] = {0.f, 20000.f, 60000.f, 110000.f, 163840.f};
    float prev = -1.f;
    for (float s : speeds) {
        CameraState cam = settle(car, s);
        const float dist = length(cam.eye - cam.target);
        if (prev >= 0.f) CHECK(nearEq(dist, prev, 1e-4f));
        prev = dist;
        // Distance stays inside the authored ramp bounds.
        CHECK(dist >= 6.9f && dist <= 7.2f);
        // Matches the speed-scaled target distance (horizontal component).
        CHECK(nearEq(cam.distance, 7.f, 1e-3f));
    }
    // Speeds beyond the reference clamp: distance stops growing.
    CameraState a = settle(car, 163840.f);
    CameraState b = settle(car, 400000.f);
    CHECK(nearEq(a.distance, b.distance, 1e-6f));
}

// The selected default preset's pitch does not change for a speed jump.
static void testDefaultPresetPitchIsSpeedIndependent() {
    CarPose car{{0.f, 0.f, 0.f}, 0.f};
    CameraState cam = settle(car, 0.f);
    const float startPitch = cam.pitch;
    CHECK(nearEq(startPitch, 0.08590293f, 1e-5f));  // preset-0 pitch target

    const float slew = 0.02454369f;
    float prev = cam.pitch;
    float maxDelta = 0.f;
    int ticksToSettle = -1;
    for (int i = 0; i < 400; ++i) {
        updateCamera(cam, car, 163840.f);  // sudden full-speed jump
        const float d = std::fabs(cam.pitch - prev);
        maxDelta = std::max(maxDelta, d);
        CHECK(nearEq(cam.pitch, prev, 1e-7f));  // speed cannot alter the preset pitch
        prev = cam.pitch;
        if (ticksToSettle < 0 && nearEq(cam.pitch, 0.08590293f, 1e-4f)) ticksToSettle = i;
    }
    CHECK(maxDelta <= slew + 1e-6f);        // bounded per-tick delta
    CHECK(maxDelta < 1e-7f);
    CHECK(ticksToSettle == 0);
    CHECK(nearEq(cam.pitch, 0.08590293f, 1e-4f));
}

// The rotation part of the view matrix is orthonormal (rows unit-length and
// pairwise orthogonal), and the last row/col are the rigid identity tail.
static void testViewMatrixOrthonormal() {
    CarPose car{{3.f, 1.f, -7.f}, 1.1f};
    const float speeds[] = {0.f, 50000.f, 163840.f};
    for (float s : speeds) {
        CameraState cam = settle(car, s, 300);
        const float* m = cam.view.m;
        const Vec3 r{m[0], m[1], m[2]};
        const Vec3 u{m[4], m[5], m[6]};
        const Vec3 f{m[8], m[9], m[10]};
        CHECK(nearEq(length(r), 1.f, 1e-5f));
        CHECK(nearEq(length(u), 1.f, 1e-5f));
        CHECK(nearEq(length(f), 1.f, 1e-5f));
        CHECK(nearEq(dot(r, u), 0.f, 1e-5f));
        CHECK(nearEq(dot(r, f), 0.f, 1e-5f));
        CHECK(nearEq(dot(u, f), 0.f, 1e-5f));
        // Right-handed: row2 (=-viewDir) == row0 x row1, since the camera
        // looks down -Z in its own frame.
        const Vec3 xru = cross(r, u);
        CHECK(nearEq(f.x, xru.x, 1e-5f));
        CHECK(nearEq(f.y, xru.y, 1e-5f));
        CHECK(nearEq(f.z, xru.z, 1e-5f));
        CHECK(m[12] == 0.f && m[13] == 0.f && m[14] == 0.f && m[15] == 1.f);
        // Translation column: transforming the eye lands at the origin.
        const Vec3 eyeT{dot(r, cam.eye) + m[3], dot(u, cam.eye) + m[7],
                        dot(f, cam.eye) + m[11]};
        CHECK(nearEq(eyeT.x, 0.f, 1e-3f));
        CHECK(nearEq(eyeT.y, 0.f, 1e-3f));
        CHECK(nearEq(eyeT.z, 0.f, 1e-3f));
    }
}

// Yaw follows the car heading through the shortest arc; a hard heading flip
// doesn't teleport the camera in front of the car once re-settled.
static void testYawFollow() {
    CarPose car{{0.f, 0.f, 0.f}, 0.f};
    CameraState cam = settle(car, 40000.f, 200);
    car.heading = 3.0f;  // big turn
    for (int i = 0; i < 300; ++i) updateCamera(cam, car, 40000.f);
    CHECK(nearEq(wrapAngle(cam.yaw - car.heading), 0.f, 1e-3f));
    // At 40000 speed the camera looks ~10 deg down at the car, so the view
    // direction sits ~0.976 along the car forward — well inside 0.95.
    CHECK(dot(cam.forward, carForward(car)) > 0.95f);
}

int main() {
    testExtractedCameraConstants();
    testBehindAndAbove();
    testDefaultPresetDistanceIsSpeedIndependent();
    testDefaultPresetPitchIsSpeedIndependent();
    testViewMatrixOrthonormal();
    testYawFollow();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("camera: all checks passed");
    return 0;
}
