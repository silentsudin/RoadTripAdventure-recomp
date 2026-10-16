// Render-frame interpolation contract tests (src/native/gameplay/interp.*):
// the fixed-timestep accumulator (step counts at synthetic elapsed times,
// catch-up cap, alpha clamping), shortest-arc angle lerp wrap cases, and
// pose snapshot/lerp correctness — including the step-boundary identity the
// deterministic RT_APP_SNAPSHOT capture relies on. Pure arithmetic; no
// clocks, no GPU, no game data.
#include "../../src/native/gameplay/interp.h"

#include <cmath>
#include <cstdio>
#include <cstdint>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static bool nearEq(float a, float b, float eps) { return std::fabs(a - b) <= eps; }
static bool nearEqD(double a, double b, double eps) { return std::fabs(a - b) <= eps; }

// ------------------------------------------------------------ accumulator

static void testAccumulatorStepCounts() {
    const double step = FixedStepClock::kStepSeconds;
    CHECK(FixedStepClock::kDefaultCatchUpCap == 2);

    // Below one quantum: nothing owed, alpha tracks the fraction.
    FixedStepClock c;
    CHECK(!c.stepDue());
    CHECK(c.stepsReady() == 0);
    CHECK(c.stepsTaken() == 0);
    c.addElapsed(step * 0.5);
    CHECK(!c.stepDue());
    CHECK(c.stepsReady() == 0);
    CHECK(nearEq(c.alpha(), 0.5f, 1e-6f));

    // Crossing the quantum boundary owes exactly one step.
    c.addElapsed(step * 0.5);
    CHECK(c.stepDue());
    CHECK(c.stepsReady() == 1);
    c.takeStep();
    CHECK(c.stepsTaken() == 1);
    CHECK(!c.stepDue());
    CHECK(c.stepsReady() == 0);
    CHECK(nearEq(c.accumulatorSeconds(), 0.0, 1e-15));  // 1/60 - 1/60 == 0
    CHECK(nearEq(c.alpha(), 0.f, 1e-6f));

    // A 120-Hz host renders every half-quantum: no step owed until two
    // renders bank a full one.
    FixedStepClock half;
    half.addElapsed(step / 2.0);
    CHECK(half.stepsReady() == 0);
    half.addElapsed(step / 2.0);
    CHECK(half.stepsReady() == 1);
    half.takeStep();
    CHECK(nearEqD(half.accumulatorSeconds(), 0.0, 1e-15));

    // Zero or negative elapsed time must not advance the accumulator.
    FixedStepClock z;
    z.addElapsed(0.0);
    z.addElapsed(-0.05);
    CHECK(z.stepsReady() == 0);
    CHECK(nearEq(z.accumulatorSeconds(), 0.0, 0.0));
}

static void testCatchUpCap() {
    const double step = FixedStepClock::kStepSeconds;

    // A stall banks more than the cap; stepsReady clamps to the cap and the
    // backlog drains across successive host frames.
    FixedStepClock c;  // default cap 2
    c.addElapsed(0.1);  // 6 quanta worth (the host's dt clamp ceiling)
    CHECK(c.stepsReady() == 2);
    c.takeStep();
    c.takeStep();
    CHECK(c.stepsTaken() == 2);
    CHECK(c.stepsReady() == 2);  // 4 quanta still banked
    c.takeStep();
    c.takeStep();
    CHECK(c.stepsReady() == 2);  // 2 remain
    c.takeStep();
    c.takeStep();
    CHECK(c.stepsReady() == 0);
    CHECK(c.stepsTaken() == 6);

    // While capped, alpha clamps to 1 — rendering never extrapolates past
    // the newest completed step.
    FixedStepClock behind(1);
    behind.addElapsed(step * 3.0);
    CHECK(behind.stepsReady() == 1);
    CHECK(behind.alpha() == 1.f);
    behind.takeStep();
    CHECK(behind.stepsReady() == 1);
    CHECK(behind.alpha() == 1.f);

    // Explicit cap (RT_APP_INTERP_CAP parsing lives in main.cpp; the clock
    // itself honours any >= 1 value and floors smaller ones to 1).
    FixedStepClock one(1);
    CHECK(one.catchUpCap() == 1);
    one.addElapsed(step * 2.5);
    CHECK(one.stepsReady() == 1);
    FixedStepClock zero(0);
    CHECK(zero.catchUpCap() == 1);
    CHECK(zero.stepsReady() == 0);

    // takeStep without a banked quantum is a deterministic no-op.
    FixedStepClock none;
    none.takeStep();
    CHECK(none.stepsTaken() == 0);
    CHECK(none.stepsReady() == 0);
}

// Drains the accumulator exactly like the rt-app host loop
// (src/native/app/main.cpp): cap-bounded per host frame, stopped at a fixed
// step budget so the drained count equals RT_APP_FRAMES.
static std::uint64_t hostDrain(FixedStepClock& clock, double perRenderSeconds,
                               int maxSteps) {
    std::uint64_t steps = 0;
    for (int frame = 0; (maxSteps == 0 || steps < static_cast<std::uint64_t>(maxSteps)) && frame < 100000; ++frame) {
        clock.addElapsed(perRenderSeconds);
        for (int owed = clock.stepsReady(); owed > 0; --owed) {
            if (maxSteps && steps >= static_cast<std::uint64_t>(maxSteps)) break;
            clock.takeStep();
            ++steps;
        }
    }
    return steps;
}

static void testDrainMatchesHostCadence() {
    // 120-Hz host: one step per two renders; the 60-Hz simulation rate is
    // preserved because the drained total matches elapsed-time steps.
    {
        FixedStepClock c;
        CHECK(hostDrain(c, FixedStepClock::kStepSeconds / 2.0, 60) == 60);
        CHECK(c.stepsTaken() == 60);
    }
    // 30-Hz host (two quanta per render): the default cap drains both per
    // render, so one wall second still yields exactly 60 logic steps.
    {
        FixedStepClock c;
        CHECK(hostDrain(c, FixedStepClock::kStepSeconds * 2.0, 60) == 60);
    }
    // Thirty 120-Hz host frames represent 0.25 s and drain 15 steps exactly.
    // (A zero budget means unbounded, so hostDrain's safety bound is not a
    // wall-clock duration.)
    {
        FixedStepClock c;
        CHECK(hostDrain(c, FixedStepClock::kStepSeconds / 2.0, 15) == 15);
    }
    // The step budget never overshoots, even under a cap-throttled backlog.
    {
        FixedStepClock c(2);
        CHECK(hostDrain(c, 0.1, 40) == 40);   // dt at the 0.1 s clamp
        CHECK(c.stepsTaken() == 40);
    }
}

// ------------------------------------------------------------ angle lerp

static void testAngleLerp() {
    const float kPiHalf = kPi * 0.5f;

    // Plain arcs behave like a plain lerp.
    CHECK(nearEq(lerpAngleShortestArc(0.f, kPiHalf, 0.5f), kPiHalf * 0.5f, 1e-6f));
    CHECK(nearEq(lerpAngleShortestArc(-0.2f, 0.4f, 0.25f), -0.05f, 1e-6f));

    // Shortest-arc wrap: 3.0 -> -3.0 crosses the +/-pi seam (~0.283 rad),
    // never the long way through 0 (which a naive lerp would hit at 0.0).
    CHECK(nearEq(lerpAngleShortestArc(3.0f, -3.0f, 0.5f), kPi, 1e-5f));
    CHECK(nearEq(lerpAngleShortestArc(-3.0f, 3.0f, 0.5f), -kPi, 1e-5f));
    CHECK(std::fabs(lerpAngleShortestArc(3.0f, -3.0f, 0.1f) - 3.0f) < 0.1f);

    // Angles outside one turn wrap their difference first.
    CHECK(nearEq(lerpAngleShortestArc(0.f, kTwoPi + 0.5f, 0.5f), 0.25f, 1e-6f));
    CHECK(nearEq(lerpAngleShortestArc(-kTwoPi, 0.25f, 0.5f), -kTwoPi + 0.125f, 1e-6f));

    // t = 0 returns the start angle bit-exactly (no re-wrap side effect).
    const float start = 0.7f;
    CHECK(lerpAngleShortestArc(start, -0.7f, 0.f) == start);
    CHECK(lerpAngleShortestArc(start, 7.3f, 0.f) == start);

    // t = 1 lands on the (2-pi-equivalent) target through the short way.
    CHECK(nearEq(lerpAngleShortestArc(1.0f, 2.0f, 1.f), 2.0f, 1e-6f));
    CHECK(nearEq(wrapAngle(lerpAngleShortestArc(3.0f, -3.0f, 1.f) - (-3.0f)), 0.f, 1e-6f));

    // Identical endpoints are stable at every fraction.
    CHECK(nearEq(lerpAngleShortestArc(1.25f, 1.25f, 0.75f), 1.25f, 1e-6f));
}

// ------------------------------------------------------------ pose lerp

static void testPoseLerp() {
    RenderPose prev;
    prev.carPosition = {10.f, 20.f};
    prev.heading = 0.f;
    prev.cameraEye = {1.f, 2.f, 3.f};
    prev.cameraTarget = {4.f, 5.f, 6.f};
    RenderPose cur;
    cur.carPosition = {30.f, 60.f};
    cur.heading = 1.f;
    cur.cameraEye = {2.f, 4.f, 6.f};
    cur.cameraTarget = {8.f, 10.f, 12.f};

    const RenderPose mid = lerpRenderPose(prev, cur, 0.5f);
    CHECK(nearEq(mid.carPosition.x, 20.f, 1e-6f));
    CHECK(nearEq(mid.carPosition.z, 40.f, 1e-6f));
    CHECK(nearEq(mid.heading, 0.5f, 1e-6f));
    CHECK(nearEq(mid.cameraEye.x, 1.5f, 1e-6f));
    CHECK(nearEq(mid.cameraEye.y, 3.f, 1e-6f));
    CHECK(nearEq(mid.cameraEye.z, 4.5f, 1e-6f));
    CHECK(nearEq(mid.cameraTarget.x, 6.f, 1e-6f));
    CHECK(nearEq(mid.cameraTarget.y, 7.5f, 1e-6f));
    CHECK(nearEq(mid.cameraTarget.z, 9.f, 1e-6f));

    // t = 0 reproduces the previous pose exactly.
    const RenderPose atZero = lerpRenderPose(prev, cur, 0.f);
    CHECK(atZero.carPosition.x == prev.carPosition.x);
    CHECK(atZero.carPosition.z == prev.carPosition.z);
    CHECK(atZero.heading == prev.heading);
    CHECK(atZero.cameraEye.x == prev.cameraEye.x && atZero.cameraEye.y == prev.cameraEye.y &&
          atZero.cameraEye.z == prev.cameraEye.z);
    CHECK(atZero.cameraTarget.x == prev.cameraTarget.x && atZero.cameraTarget.y == prev.cameraTarget.y &&
          atZero.cameraTarget.z == prev.cameraTarget.z);

    // Heading blends through the shortest arc across the seam.
    RenderPose wrapPrev, wrapCur;
    wrapPrev.heading = 3.0f;
    wrapCur.heading = -3.0f;
    CHECK(nearEq(lerpRenderPose(wrapPrev, wrapCur, 0.5f).heading, kPi, 1e-5f));

    // Positions are monotone along the blend.
    for (float t = 0.f; t <= 1.f; t += 0.25f) {
        const RenderPose p = lerpRenderPose(prev, cur, t);
        const float expected = 10.f + 20.f * t;
        CHECK(nearEq(p.carPosition.x, expected, 1e-5f));
    }

    // Render callers may be behind the catch-up cap.  The blend is bounded
    // rather than extrapolated even if a future caller supplies an out-of-
    // contract fraction.
    const RenderPose below = lerpRenderPose(prev, cur, -0.25f);
    const RenderPose above = lerpRenderPose(prev, cur, 1.25f);
    CHECK(below.carPosition.x == prev.carPosition.x);
    CHECK(below.heading == prev.heading);
    CHECK(above.carPosition.x == cur.carPosition.x);
    CHECK(nearEq(wrapAngle(above.heading - cur.heading), 0.f, 1e-6f));
}

// ------------------------------------------------- snapshot pose plumbing

static FrameSnapshot boundarySnapshot() {
    FrameSnapshot s;
    s.carPosition = {123.f, -45.f};
    s.heading = -2.5f;
    s.cameraEye = {3.f, 9.f, -1.f};
    s.cameraTarget = {1.f, 2.f, 7.f};
    s.frame = 60;
    s.clock = 4321;
    // View matrix built exactly the way GameLoop::makeSnapshot does.
    const Mat4 v = lookAtMatrix(s.cameraEye, s.cameraTarget);
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            s.view[c * 4 + r] = v.m[r * 4 + c];
    // Arbitrary non-identity projection that must survive untouched.
    for (int i = 0; i < 16; ++i) s.proj[i] = static_cast<float>(i) + 0.5f;
    return s;
}

static void testRenderPoseExtraction() {
    const FrameSnapshot s = boundarySnapshot();
    const RenderPose pose = renderPoseOf(s);
    CHECK(pose.carPosition.x == s.carPosition.x && pose.carPosition.z == s.carPosition.z);
    CHECK(pose.heading == s.heading);
    CHECK(pose.cameraEye.x == s.cameraEye.x && pose.cameraEye.y == s.cameraEye.y &&
          pose.cameraEye.z == s.cameraEye.z);
    CHECK(pose.cameraTarget.x == s.cameraTarget.x && pose.cameraTarget.y == s.cameraTarget.y &&
          pose.cameraTarget.z == s.cameraTarget.z);
}

// The determinism contract at module level: applying the pose round-trip to
// a step-boundary snapshot changes nothing — bit-identical view matrix,
// untouched frame/clock/projection. This is what lets the RT_APP_INTERP=1
// capture path record the same bytes the RT_APP_INTERP=0 path records at
// the same logic step (proved byte-for-byte by the matrix's
// interp_determinism row for the full P6 capture).
static void testBoundaryPoseRoundTripIsIdentity() {
    const FrameSnapshot s = boundarySnapshot();
    const FrameSnapshot out = snapshotWithRenderPose(s, renderPoseOf(s));
    for (int i = 0; i < 16; ++i) {
        CHECK(out.view[i] == s.view[i]);   // bit-exact rebuild
        CHECK(out.proj[i] == s.proj[i]);
    }
    CHECK(out.carPosition.x == s.carPosition.x && out.carPosition.z == s.carPosition.z);
    CHECK(out.heading == s.heading);
    CHECK(out.cameraEye.x == s.cameraEye.x && out.cameraEye.y == s.cameraEye.y &&
          out.cameraEye.z == s.cameraEye.z);
    CHECK(out.cameraTarget.x == s.cameraTarget.x && out.cameraTarget.y == s.cameraTarget.y &&
          out.cameraTarget.z == s.cameraTarget.z);
    CHECK(out.frame == s.frame);
    CHECK(out.clock == s.clock);

    // A blended pose rebuilds a view matrix consistent with its own camera:
    // transforming the lerped eye lands at the view-space origin.
    const RenderPose prev = renderPoseOf(s);
    RenderPose moved;
    moved.carPosition = {140.f, -30.f};
    moved.heading = -2.2f;
    moved.cameraEye = {4.f, 9.5f, -0.5f};
    moved.cameraTarget = {1.5f, 2.5f, 6.f};
    for (int i = 0; i <= 4; ++i) {
        const float t = static_cast<float>(i) / 4.f;
        const FrameSnapshot blended = snapshotWithRenderPose(s, lerpRenderPose(prev, moved, t));
        const float* m = blended.view;
        const RenderPose p = lerpRenderPose(prev, moved, t);
        const float ex = m[0] * p.cameraEye.x + m[4] * p.cameraEye.y + m[8] * p.cameraEye.z + m[12];
        const float ey = m[1] * p.cameraEye.x + m[5] * p.cameraEye.y + m[9] * p.cameraEye.z + m[13];
        const float ez = m[2] * p.cameraEye.x + m[6] * p.cameraEye.y + m[10] * p.cameraEye.z + m[14];
        CHECK(nearEq(ex, 0.f, 1e-4f));
        CHECK(nearEq(ey, 0.f, 1e-4f));
        CHECK(nearEq(ez, 0.f, 1e-4f));
        // The lerped pose fields land verbatim in the snapshot.
        CHECK(blended.carPosition.x == p.carPosition.x);
        CHECK(blended.heading == p.heading);
    }
}

int main() {
    testAccumulatorStepCounts();
    testCatchUpCap();
    testDrainMatchesHostCadence();
    testAngleLerp();
    testPoseLerp();
    testRenderPoseExtraction();
    testBoundaryPoseRoundTripIsIdentity();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("interp: all checks passed");
    return 0;
}
