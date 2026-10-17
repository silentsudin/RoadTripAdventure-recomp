// Authored chase-camera contract tests (src/native/gameplay/camera.*).
// Concrete numeric assertions; no game data, no framework.
// Additional GameLoop camera framing + setCarPose tests per camera-framing.md.
#include "../../src/native/gameplay/camera.h"
#include "../../src/native/gameplay/game_loop.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
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

// --- GameLoop chase-camera framing + setCarPose tests (camera-framing.md) ---

static MenuInput makeMi() { return MenuInput{}; }

// Compute flat-road ground strike distance ahead of car origin (y=0 ray cast).
// eye->target extended; diagnostic per camera-framing.md:30 .
static float groundStrikeRu(const Vec3& eye, const Vec3& target, float carX, float carZ) {
    const float dy = eye.y - target.y;
    if (std::fabs(dy) < 1e-9f) return 0.f;
    // parametric t where y=0: t = eye.y / (eye.y - target.y)
    const float t = eye.y / dy;
    const float strikeX = eye.x + t * (target.x - eye.x);
    const float strikeZ = eye.z + t * (target.z - eye.z);
    const float dx = strikeX - carX;
    const float dz = strikeZ - carZ;
    return std::sqrt(dx*dx + dz*dz);
}

static void testChaseCameraFramingTargets() {
    // Use GameLoopParams set to documented targets (wired to CameraParams inside).
    GameLoopParams p;
    p.chaseBackDistance = kChaseBackRu;
    p.chaseBaseHeight   = kChaseHeightRu;
    p.chasePitchRad     = kChasePitchRad;
    p.chaseLookAtHeight = kChaseLookAtRu;
    p.fovY = kChaseFovYRad;
    GameLoop g(p);

    // Place car at origin heading +Z for easy math.
    const float cx = 0.f, cz = 0.f, h = 0.f;
    g.setCarPose(cx, cz, h);
    // Tick with zero input keeps pose (speed 0, steer 0); camera already refreshed by set.
    const MenuInput mi = makeMi();
    const FrameSnapshot s = g.tickPlayer(0.f, mi, CarInput{});

    // Documented targets (camera-framing.md table).
    const float eyeH = 2.f + 7.f * std::sin(kChasePitchRad);  // ~2.601
    const float backH = 7.f * std::cos(kChasePitchRad);       // horizontal back component
    const float groundStrike = 16.47f;  // derived flat-road

    const float carRx = cx * p.renderScale;
    const float carRz = cz * p.renderScale;
    const float dx = s.cameraEye.x - carRx;
    const float dz = s.cameraEye.z - carRz;
    const float backDist = std::sqrt(dx*dx + dz*dz);
    const float eyeHeight = s.cameraEye.y;
    // Use the derived diagnostic formula from camera-framing.md:30 (not the
    // car-to-strike euclid, which is ~9.5; the cited 16.47 is the extension
    // factor applied to horiz back).
    const float strike = backH * eyeHeight / (eyeHeight - kChaseLookAtRu);

    std::printf("framing: eyeH=%g (want~%g) back=%g (want~%g) strike=%g (want~%g) eye=(%g,%g,%g) tgt=(%g,%g,%g)\n",
                eyeHeight, eyeH, backDist, backH, strike, groundStrike,
                s.cameraEye.x, s.cameraEye.y, s.cameraEye.z,
                s.cameraTarget.x, s.cameraTarget.y, s.cameraTarget.z);
    CHECK(nearEq(eyeHeight, eyeH, 0.01f));           // documented actual eye height
    CHECK(nearEq(backDist, backH, 0.01f));           // back distance along -forward
    CHECK(nearEq(strike, groundStrike, 0.1f));       // ground strike within deriv tol
    // Target at lookAt height, no lateral.
    CHECK(nearEq(s.cameraTarget.y, kChaseLookAtRu, 1e-6f));
    CHECK(nearEq(s.cameraTarget.x, carRx, 1e-6f));
    CHECK(nearEq(s.cameraTarget.z, carRz, 1e-6f));
}

static std::uint64_t hashCamera(const FrameSnapshot& s) {
    // Simple deterministic hash for re-anchor determinism (pose+camera).
    // Ignore frame (each tick increments) and lighting etc; pose+eye+target only.
    std::uint64_t h = 0xdeadbeefULL;
    auto mix = [&](float v) {
        h = h * 6364136223846793005ULL + static_cast<std::uint32_t>(std::fabs(v) * 1e6f + 0.5f);
    };
    mix(s.carPosition.x); mix(s.carPosition.z); mix(s.heading);
    mix(s.cameraEye.x); mix(s.cameraEye.y); mix(s.cameraEye.z);
    mix(s.cameraTarget.x); mix(s.cameraTarget.y); mix(s.cameraTarget.z);
    return h;
}

static void testSetCarPoseReanchorsDeterministically() {
    GameLoop g;
    const float x=123.4f, z=-56.7f, h=1.23f;
    g.setCarPose(x, z, h);
    const MenuInput mi = makeMi();
    const FrameSnapshot s1 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s1.carPosition.x, x, 1e-6f));
    CHECK(nearEq(s1.carPosition.z, z, 1e-6f));
    CHECK(nearEq(s1.heading, h, 1e-6f));

    // Second call with identical pose must produce identical camera state.
    g.setCarPose(x, z, h);
    const FrameSnapshot s2 = g.tickPlayer(0.f, mi, CarInput{});
    std::printf("setpose hashes: s1=%llu s2=%llu\n", (unsigned long long)hashCamera(s1), (unsigned long long)hashCamera(s2));
    std::printf("setpose s1 car=(%g,%g) h=%g eye=(%g,%g,%g)\n", s1.carPosition.x, s1.carPosition.z, s1.heading, s1.cameraEye.x,s1.cameraEye.y,s1.cameraEye.z);
    std::printf("setpose s2 car=(%g,%g) h=%g eye=(%g,%g,%g)\n", s2.carPosition.x, s2.carPosition.z, s2.heading, s2.cameraEye.x,s2.cameraEye.y,s2.cameraEye.z);
    CHECK(hashCamera(s1) == hashCamera(s2));
    // Eyes match (no drift).
    CHECK(nearEq(s1.cameraEye.x, s2.cameraEye.x, 1e-6f));
    CHECK(nearEq(s1.cameraEye.y, s2.cameraEye.y, 1e-6f));
    CHECK(nearEq(s1.cameraEye.z, s2.cameraEye.z, 1e-6f));
}

static void testAspectDoesNotMoveEye() {
    GameLoop g;
    g.setCarPose(10.f, 20.f, 0.1f);
    const MenuInput mi = makeMi();
    const FrameSnapshot s0 = g.tickPlayer(0.f, mi, CarInput{});
    const Vec3 eye0 = s0.cameraEye;

    // 4:3 vs 16:9 changes proj only (policy: pinned fovY, aspect reapplied).
    g.setProjectionAspect(4.f/3.f);
    const FrameSnapshot s43 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s43.cameraEye.x, eye0.x, 1e-9f));
    CHECK(nearEq(s43.cameraEye.y, eye0.y, 1e-9f));
    CHECK(nearEq(s43.cameraEye.z, eye0.z, 1e-9f));

    g.setProjectionAspect(16.f/9.f);
    const FrameSnapshot s169 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s169.cameraEye.x, eye0.x, 1e-9f));
    CHECK(nearEq(s169.cameraEye.y, eye0.y, 1e-9f));
    CHECK(nearEq(s169.cameraEye.z, eye0.z, 1e-9f));
    // Proj does change with aspect (but we don't assert specific matrix here).
}

static void testSetCarPoseFreezesMotion() {
    // Anchoring a ROLLING car must freeze speed/steering with the pose so no
    // idle tick can drift it off the anchor (Peach Town square hold;
    // GameSession::continueFromResults relies on this exact semantics).
    GameLoop g;
    const MenuInput mi = makeMi();
    CarInput gas{};
    gas.gas = true;
    for (int i = 0; i < 60; ++i) g.tickPlayer(0.f, mi, gas);
    g.setCarPose(506.283f, 442.230f, 0.f);
    const FrameSnapshot s1 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s1.carPosition.x, 506.283f, 1e-5f));
    CHECK(nearEq(s1.carPosition.z, 442.230f, 1e-5f));
    CHECK(s1.speed == 0.f);
    const FrameSnapshot s2 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(s2.carPosition.x == s1.carPosition.x);
    CHECK(s2.carPosition.z == s1.carPosition.z);

    // Elevation: anchoring at the Peach Town square ground (game y=30 ->
    // 3.0 RU) must lift the eye above the plaza (oracle y=30.000,
    // oracle-golden-frames.md:54), and reset() returns the datum to 0.
    g.setCarPose(0.f, 0.f, 0.f, 30.f);
    const FrameSnapshot s3 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s3.cameraEye.y, 3.f + 2.f + 7.f * std::sin(kChasePitchRad), 0.01f));
    g.reset();
    const FrameSnapshot s4 = g.tickPlayer(0.f, mi, CarInput{});
    CHECK(nearEq(s4.cameraEye.y, 2.f + 7.f * std::sin(kChasePitchRad), 0.01f));
}

int main() {
    testExtractedCameraConstants();
    testBehindAndAbove();
    testDefaultPresetDistanceIsSpeedIndependent();
    testDefaultPresetPitchIsSpeedIndependent();
    testViewMatrixOrthonormal();
    testYawFollow();
    testChaseCameraFramingTargets();
    testSetCarPoseReanchorsDeterministically();
    testSetCarPoseFreezesMotion();
    testAspectDoesNotMoveEye();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("camera: all checks passed");
    return 0;
}
