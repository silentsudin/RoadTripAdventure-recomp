// Authored AI controller contract tests (src/native/gameplay/ai.*).
// Concrete numeric assertions; no game data, no framework.
#include "../../src/native/gameplay/ai.h"
#include "../../src/native/gameplay/car_physics.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c)                                                            \
    do {                                                                    \
        if (!(c)) {                                                         \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); \
            ++g_fail;                                                       \
        }                                                                   \
    } while (0)

static bool nearEq(float a, float b, float eps) { return std::fabs(a - b) <= eps; }

// ---------------------------------------------------------------- kinematics
// Simple integration model used only by these tests. `kSpeedFactor` converts
// car-physics speed units into world units per tick; `kTurnRate` is the angular
// change produced by a full left/right steer command.
static constexpr float kSpeedFactor = 0.0001f;
static constexpr float kTurnRate = 0.04f;

struct Sim {
    AiState ai;
    CarState car;
};

static AiOutput step(Sim& s, const WaypointPath& path, const AiParams& params) {
    AiOutput out = tickAi(s.ai, s.car, path, params);

    CarInput in;
    in.steer = out.steer;
    in.gas = out.throttle;
    in.brake = out.brake;
    tickCar(s.car, in);

    // Turn command -> heading change.
    s.ai.heading += out.steer * kTurnRate;

    // Forward motion along the current heading.
    s.ai.position.x += std::sin(s.ai.heading) * s.car.speed * kSpeedFactor;
    s.ai.position.z += std::cos(s.ai.heading) * s.car.speed * kSpeedFactor;

    return out;
}

// ---------------------------------------------------------------- tests

// The AI follows a straight northward line of waypoints: it converges its
// heading to the target, advances through the waypoints in order, and keeps
// the car within a narrow corridor.
static void testStraightLine() {
    WaypointPath path = {{0.f, 100.f}, {0.f, 200.f}, {0.f, 300.f}, {0.f, 400.f}};

    Sim s;
    s.ai.position = {0.f, 0.f};
    s.ai.heading = 0.02f;  // slightly off target
    s.car.speed = 8000;

    AiParams params;
    params.maxSpeedStraight = 12000.f;
    params.maxSpeedCorner = 3000.f;
    params.arrivalRadius = 8.f;
    params.brakeLookahead = 20.f;

    int reached = 0;
    float maxCorridor = 0.f;
    for (int i = 0; i < 350; ++i) {
        step(s, path, params);
        maxCorridor = std::max(maxCorridor, std::fabs(s.ai.position.x));
        if (s.ai.waypointIndex > reached) reached = s.ai.waypointIndex;
    }

    CHECK(reached >= 2);
    CHECK(maxCorridor < 3.f);
    CHECK(nearEq(std::sin(s.ai.heading), 0.f, 0.05f));
}

// The AI slows for a sharp 90-degree left turn: after entering the turn
// segment, speed drops well below the peak speed seen on the straight.
static void testSharpTurnSlows() {
    // North then a long west segment so the AI has room to decelerate.
    WaypointPath path = {{0.f, 50.f}, {0.f, 100.f}, {-300.f, 100.f}};

    Sim s;
    s.ai.position = {0.f, 0.f};
    s.ai.heading = 0.f;
    s.car.speed = 15000;

    AiParams params;
    params.maxSpeedStraight = 20000.f;
    params.maxSpeedCorner = 2000.f;
    params.arrivalRadius = 8.f;
    params.brakeLookahead = 20.f;

    float maxStraightSpeed = 0.f;
    float minTurnSpeedAfterStart = 1e9f;
    bool enteredTurn = false;

    for (int i = 0; i < 1600; ++i) {
        step(s, path, params);

        if (s.ai.waypointIndex < 2) {
            maxStraightSpeed = std::max(maxStraightSpeed, static_cast<float>(s.car.speed));
        } else {
            enteredTurn = true;
            // Ignore the first part of the turn where the car is still carrying
            // straight-line speed; measure the minimum once it is committed.
            if (s.ai.position.x < -20.f) {
                minTurnSpeedAfterStart =
                    std::min(minTurnSpeedAfterStart, static_cast<float>(s.car.speed));
            }
        }
    }

    CHECK(enteredTurn);
    CHECK(maxStraightSpeed > 15000.f);
    CHECK(minTurnSpeedAfterStart < maxStraightSpeed - 10000.f);
}

// The AI brakes when approaching a waypoint zone and advances the waypoint
// index once the car is inside the arrival radius.
static void testArrivalRadiusAndBrake() {
    WaypointPath path = {{0.f, 30.f}};

    Sim s;
    s.ai.position = {0.f, 0.f};
    s.ai.heading = 0.f;
    s.car.speed = 6000;

    AiParams params;
    params.maxSpeedStraight = 10000.f;
    params.maxSpeedCorner = 3000.f;
    params.arrivalRadius = 10.f;
    params.brakeLookahead = 20.f;

    bool braked = false;
    int reachedTick = -1;
    for (int i = 0; i < 400; ++i) {
        AiOutput out = step(s, path, params);
        if (out.brake) braked = true;

        const float dx = path[0].x - s.ai.position.x;
        const float dz = path[0].z - s.ai.position.z;
        const float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < params.arrivalRadius && reachedTick < 0) reachedTick = i;
    }

    CHECK(braked);
    CHECK(reachedTick >= 0);
    CHECK(s.ai.waypointIndex == 1);
}

// The internal steering command is bounded per tick by the slew cap, even
// when the car is instantly reversed relative to the waypoint (180 deg error).
// This catches a regression in the camera-style slew; the discrete output is
// just a thresholded view of the slewed accumulator.
static void testSteerBoundedPerTick() {
    WaypointPath path = {{0.f, 50.f}};

    Sim s;
    s.ai.position = {0.f, 0.f};
    s.ai.heading = 3.14159f;  // facing south, waypoint is north: ~180 deg error
    s.car.speed = 0;

    AiParams params;
    params.steerSlewPerTick = 0.1f;

    float prevAccum = s.ai.steerAccum;
    bool bounded = true;
    bool becameFullSteer = false;
    for (int i = 0; i < 60; ++i) {
        AiOutput out = tickAi(s.ai, s.car, path, params);
        const float delta = std::fabs(s.ai.steerAccum - prevAccum);
        if (delta > params.steerSlewPerTick + 1e-6f) bounded = false;
        prevAccum = s.ai.steerAccum;
        if (out.steer != 0) becameFullSteer = true;
    }

    CHECK(bounded);
    CHECK(becameFullSteer);
    CHECK(std::fabs(s.ai.steerAccum) > 0.99f);
}

// CONFIRMED speed quantization/thresholds: 0x250978..0x250c9c.
// Changing strict comparisons, masking, or using the cap divisor for both
// paths must fail these boundary pins. Synthetic inputs, not disc data.
static void testTrafficSpeedBands() {
    CHECK(TrafficAiConstants::speedNumerator == 216);
    CHECK(TrafficAiConstants::routeSpeedDivisor == 0x28000);
    CHECK(TrafficAiConstants::capSpeedDivisor == 0x14000);
    CHECK(TrafficAiConstants::residentSpeedCap == 20);
    CHECK(TrafficAiConstants::flaggedResidentSpeedCap == 40);
    CHECK(trafficSpeedInput(0, 0) == 1); // zero target means gas, not stop
    CHECK(trafficSpeedInput(200000, 0x80) == 1); // low seven bits only
    // 8192 * 216 / 163840 = 10 (integer truncation).
    CHECK(trafficSpeedInput(8192, 11) == 1);
    CHECK(trafficSpeedInput(8192, 10) == 1);
    CHECK(trafficSpeedInput(8192, 9) == 0);
    CHECK(trafficSpeedInput(8192, 8) == 2);
    CHECK(trafficSpeedInput(8192, 0x8a) == 1);
    CHECK(trafficSpeedInput(7585, 8) == 0); // index 9, just below 10 boundary
    CHECK(trafficSpeedInput(7586, 8) == 2); // index 10
    CHECK(trafficSpeedInput(-8192, 10) == 1);
    // Cap index is 21: d<=0 preserves base, d=1/2 coasts, d>=3 brakes.
    CHECK(trafficSpeedInput(8192, 8, 21) == 2);
    CHECK(trafficSpeedInput(8192, 0, 20) == 0);
    CHECK(trafficSpeedInput(8192, 0, 19) == 0);
    CHECK(trafficSpeedInput(8192, 0, 18) == 2);
    CHECK(trafficSpeedInput(8192, 0, 22) == 1);
}

static void testTrafficBranchSide() {
    // CONFIRMED 0x250144..0x2501a4 and 0x250244..0x2502c0:
    // the third point in two segment records defines the dividing line.
    const Vec2 a{0.f, 0.f}, b{0.f, 10.f};
    CHECK(chooseTrafficBranch(a, b, {-1.f, 5.f}, 3, 7) == 3);
    CHECK(chooseTrafficBranch(a, b, {1.f, 5.f}, 3, 7) == 7);
    CHECK(chooseTrafficBranch(a, b, {0.f, 5.f}, 3, 7) == 7);
    CHECK(chooseTrafficBranch(a, b, {1.f, 5.f}, 3, 3) == 3);
}

static void testTrafficSpeedOptIn() {
    AiState ai;
    CarState car;
    car.speed = 8192;
    AiParams params;
    params.routeTargetSpeed = 0;
    params.trafficSpeedCap = 18;
    const auto out = tickAi(ai, car, {{0.f, 15.f}}, params);
    CHECK(out.brake);
    CHECK(!out.throttle);
    params.trafficSpeedCap = 0;
    const auto gas = tickAi(ai, car, {{0.f, 15.f}}, params);
    CHECK(gas.throttle); // no authored corner/lookahead override in this mode
    CHECK(!gas.brake);
}

int main() {
    testStraightLine();
    testSharpTurnSlows();
    testArrivalRadiusAndBrake();
    testSteerBoundedPerTick();
    testTrafficSpeedBands();
    testTrafficBranchSide();
    testTrafficSpeedOptIn();

    if (g_fail) {
        std::fprintf(stderr, "%d failure(s)\n", g_fail);
        return 1;
    }
    std::puts("ai: all checks passed");
    return 0;
}
