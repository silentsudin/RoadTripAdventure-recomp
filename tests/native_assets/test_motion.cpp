// Motion-vector arithmetic contract tests (src/native/gameplay/motion.*).
#include "../../src/native/gameplay/motion.h"

#include <cmath>
#include <cstdio>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static bool nearEq(float a, float b, float eps = 1e-5f) { return std::fabs(a - b) <= eps; }

static MotionMatrix identity() {
    return {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
}

static void testStraightLineWorldMotion() {
    EntityMotion e;
    e.prevPos = {1.f, 2.f, 3.f};
    e.curPos = {4.f, 6.f, 9.f};
    const MotionVector mv = worldMotion(e);
    CHECK(mv.valid());
    CHECK(nearEq(mv.world.x, 3.f));
    CHECK(nearEq(mv.world.y, 4.f));
    CHECK(nearEq(mv.world.z, 6.f));
}

static void testYawWrapUsesShortestArc() {
    EntityMotion e;
    e.prevYaw = 3.10f;
    e.curYaw = -3.10f;
    const MotionVector mv = worldMotion(e, {1.f, 0.f, 0.f});
    CHECK(mv.valid());
    CHECK(std::fabs(mv.yawDeltaRadians) < 0.1f);
    CHECK(!(mv.flags & MotionTranslationOnly));
}

static void testTeleportIsRejected() {
    EntityMotion e;
    e.prevPos = {0.f, 0.f, 0.f};
    e.curPos = {31.f, 0.f, 0.f};
    const MotionVector mv = worldMotion(e);
    CHECK(!mv.valid());
    CHECK(mv.flags & MotionTeleport);
    CHECK(nearEq(mv.world.x, 0.f));
}

static void testNoPreviousIsZeroMotion() {
    EntityMotion e;
    e.hasPrevious = false;
    e.curPos = {10.f, 2.f, -3.f};
    const MotionVector mv = worldMotion(e);
    CHECK(!mv.valid());
    CHECK(mv.flags & MotionNoPrevious);
    CHECK(nearEq(mv.world.x, 0.f) && nearEq(mv.world.y, 0.f) && nearEq(mv.world.z, 0.f));
}

static void testProjectionMatchesHandComputedOffset() {
    MotionMatrix vp = identity();
    EntityMotion e;
    e.prevPos = {0.f, 0.f, 0.f};
    e.curPos = {0.5f, -0.25f, 0.f};
    const ScreenMotion mv = projectEntityMotion(e, {0.f, 0.f, 0.f}, vp, vp);
    CHECK(mv.valid());
    CHECK(nearEq(mv.previousNdc.x, 0.f));
    CHECK(nearEq(mv.currentNdc.x, 0.5f));
    CHECK(nearEq(mv.ndc.x, 0.5f));
    CHECK(nearEq(mv.ndc.z, -0.25f));
    CHECK(nearEq(mv.uv.x, 0.25f));  // NDC-to-UV scale is one half.
    CHECK(nearEq(mv.uv.z, -0.125f));
}

static void testSpinUsesTranslationOnly() {
    EntityMotion e;
    e.prevPos = {2.f, 0.f, 0.f};
    e.curPos = {3.f, 0.f, 0.f};
    e.prevYaw = 0.f;
    e.curYaw = 3.14159265358979323846f;
    const MotionVector mv = worldMotion(e, {2.f, 0.f, 0.f});
    CHECK(mv.valid());
    CHECK(mv.flags & MotionTranslationOnly);
    CHECK(nearEq(mv.world.x, 1.f));
    CHECK(nearEq(mv.world.y, 0.f));
    CHECK(nearEq(mv.world.z, 0.f));
}

int main() {
    testStraightLineWorldMotion();
    testYawWrapUsesShortestArc();
    testTeleportIsRejected();
    testNoPreviousIsZeroMotion();
    testProjectionMatchesHandComputedOffset();
    testSpinUsesTranslationOnly();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("motion: all checks passed");
    return 0;
}
