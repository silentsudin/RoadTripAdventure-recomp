// tests/native_assets/test_trigger_integration.cpp
//
// INTEGRATION + REGRESSION tests for the additive analog-trigger driving map
// (mapTriggersToDrive in gameplay/input.{h,cpp}, committed 73d7e80).
//
// The implementer's unit tests (test_trigger_map.cpp) pin the mapper in
// isolation; this file pins what they structurally miss: the map's output
// driven THROUGH the real consumer chain
//     TriggerDriveOutput.input (CarInput{gas,brake,reverse})
//         -> GameLoop::tickPlayer (game_loop.cpp consumer)
//         -> integrateCar -> tickCar (car_physics.cpp drive()),
// plus the gate-off compatibility pin: with mapTriggersToDrive never invoked,
// the legacy digital path (playerCarInput -> tickPlayer -> tickCar) must still
// produce exactly its documented digital outcomes.
//
// Construction convention (mirrors test_game_loop.cpp / test_car_physics.cpp):
// GameLoopParams.physicsTicksPerFrame is pinned to 1 so one tickPlayer call ==
// one fixed logic step == one mapTriggersToDrive step (the header's "one step
// per fixed logic step" contract). No GPU, no clock reads, no game data;
// fully deterministic.
//
// Pin style: exact == on floats captured from the same deterministic pipeline
// (identical inputs must produce bitwise-identical floats on this
// compiler/platform — that identity is precisely the regression being
// pinned). Struct comparison is field-wise rather than memcmp: CarInput/
// CarState carry padding bytes that are not part of any observable contract;
// field equality covers every value byte.

#include "../../src/native/gameplay/car_physics.h"
#include "../../src/native/gameplay/game_loop.h"
#include "../../src/native/gameplay/input.h"

#include <cmath>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <vector>

using namespace rt::native::gameplay;

namespace {
int gChecks = 0;
int gFailures = 0;

#define CHECK(c)                                                                      \
    do {                                                                             \
        ++gChecks;                                                                   \
        if (!(c)) {                                                                  \
            std::fprintf(stderr, "FAIL trigger_integration check #%d (%s:%d): %s\n", \
                         gChecks, __FILE__, __LINE__, #c);                           \
            ++gFailures;                                                             \
        }                                                                            \
    } while (0)

// One tickPlayer call == one fixed logic step == one map step.
GameLoopParams oneTickPerFrame() {
    GameLoopParams p;
    p.physicsTicksPerFrame = 1;
    return p;
}

bool sameCarInput(const CarInput& a, const CarInput& b) {
    return a.steer == b.steer && a.gas == b.gas && a.brake == b.brake && a.reverse == b.reverse;
}

bool sameCarInputSeq(const std::vector<CarInput>& a, const std::vector<CarInput>& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (!sameCarInput(a[i], b[i])) return false;
    return true;
}

bool sameCarState(const CarState& a, const CarState& b) {
    return a.steeringAngle == b.steeringAngle && a.brakeRamp == b.brakeRamp && a.gear == b.gear &&
           a.rpm == b.rpm && a.speed == b.speed && a.odometer == b.odometer &&
           a.airborne == b.airborne && a.fallSpeed[0] == b.fallSpeed[0] &&
           a.fallSpeed[1] == b.fallSpeed[1] && a.fallSpeed[2] == b.fallSpeed[2] &&
           a.contactWord[0] == b.contactWord[0] && a.contactWord[1] == b.contactWord[1] &&
           a.contactWord[2] == b.contactWord[2];
}

// Byte-level pin over every snapshot field: identical CarInputs through the
// same pipeline must reproduce every observable bit of the frame.
bool samePlayerSnapshot(const FrameSnapshot& a, const FrameSnapshot& b) {
    return a.carPosition.x == b.carPosition.x && a.carPosition.z == b.carPosition.z &&
           a.heading == b.heading && a.speed == b.speed && a.cameraEye.x == b.cameraEye.x &&
           a.cameraEye.y == b.cameraEye.y && a.cameraEye.z == b.cameraEye.z &&
           a.cameraTarget.x == b.cameraTarget.x && a.cameraTarget.y == b.cameraTarget.y &&
           a.cameraTarget.z == b.cameraTarget.z &&
           std::memcmp(a.view, b.view, sizeof a.view) == 0 &&
           std::memcmp(a.proj, b.proj, sizeof a.proj) == 0 &&
           a.visibleCells.size() == b.visibleCells.size() &&
           a.waypointIndex == b.waypointIndex && a.lapCount == b.lapCount &&
           a.waypointsPassed == b.waypointsPassed && a.frame == b.frame && a.clock == b.clock &&
           std::memcmp(&a.lighting, &b.lighting, sizeof a.lighting) == 0 && a.lap == b.lap &&
           a.elapsedFrames == b.elapsedFrames && a.bestLap == b.bestLap &&
           a.finished == b.finished && a.rank == b.rank;
}

// One integration step: read the live car through its real observable state
// (the FrameSnapshot the session/renderer consumes), map the analog triggers
// against it, then feed the mapped CarInput through the real consumer.
struct MapStep {
    TriggerDriveOutput out;
    FrameSnapshot snap;
};

MapStep stepTriggerDrive(GameLoop& loop, TriggerDriveState& st, const FrameSnapshot& last,
                         float leftTrigger, float rightTrigger, const TriggerMapConfig& cfg = {}) {
    const TriggerDriveInputs in{
        .leftTrigger = leftTrigger,
        .rightTrigger = rightTrigger,
        .speed = static_cast<int>(last.speed),
        .standstill = last.speed == 0.f,
    };
    const TriggerDriveOutput out = mapTriggersToDrive(in, st, cfg);
    MapStep m;
    m.out = out;
    m.snap = loop.tickPlayer(1.f / 60.f, MenuInput{}, out.input);
    return m;
}

// ---------------------------------------------------------------------------
// (a) Compatibility anchor at the integration level: a FULLY pulled gas
// trigger must reproduce the legacy digital gas hold through the whole
// consumer chain — same CarInput bits every step, therefore the same physics
// trajectory ("full trigger == legacy digital hold after settling"; this pins
// the stronger statement that they agree from the very first step).
// ---------------------------------------------------------------------------
void testFullTriggerEqualsLegacyDigitalHold() {
    GameLoop triggerLoop(oneTickPerFrame());
    GameLoop legacyLoop(oneTickPerFrame());
    TriggerDriveState st;
    const CarInput legacyHold{.steer = 0, .gas = true, .brake = false, .reverse = false};

    FrameSnapshot trigSnap;  // standstill seed (a fresh GameLoop starts stationary)
    FrameSnapshot legacySnap;
    for (int t = 0; t < 120; ++t) {
        const MapStep m = stepTriggerDrive(triggerLoop, st, trigSnap, 0.f, 1.f);
        legacySnap = legacyLoop.tickPlayer(1.f / 60.f, MenuInput{}, legacyHold);
        CHECK(sameCarInput(m.out.input, legacyHold));  // the mapped bits ARE the digital hold
        CHECK(m.out.input.gas);
        CHECK(samePlayerSnapshot(m.snap, legacySnap));  // ... so the physics agrees bit-for-bit
        trigSnap = m.snap;
    }

    // Qualitative forward outcome at full pull, same as digital gas:
    CHECK(trigSnap.speed > 0.f);
    const Vec2 start = triggerLoop.world().roadWaypoints[0];
    CHECK(lengthSq(trigSnap.carPosition - start) > 0.f);
    CHECK(trigSnap.speed == legacySnap.speed);
}

// ---------------------------------------------------------------------------
// (b) Half (and partial) triggers must integrate to LESS motion than a full
// trigger — asserted on the integrated outcome (terminal speed / distance
// through the real physics), never on duty arithmetic.
// ---------------------------------------------------------------------------
void testPartialTriggerAcceleratesSlowerThanFull() {
    // (b1) Default config: terminal speed and distance increase strictly with
    // the held gas trigger. The default pulsePeriod is 2, so partial triggers
    // hold the Gas bit on below their speed cap; the differentiated outcome is
    // the cap trigger*kGasTopSpeed, which must show up in the physics.
    const float levels[] = {0.25f, 0.5f, 0.75f, 1.0f};
    float prevSpeed = -1.f;
    float prevDist = -1.f;
    for (const float trig : levels) {
        GameLoop loop(oneTickPerFrame());
        TriggerDriveState st;
        FrameSnapshot snap;  // standstill seed
        for (int t = 0; t < 600; ++t) snap = stepTriggerDrive(loop, st, snap, 0.f, trig).snap;
        const float cap = trig * kGasTopSpeed;  // documented cap, physics speed units
        CHECK(snap.speed > prevSpeed);          // strictly monotone in the trigger level
        CHECK(snap.speed >= cap - 45.f);        // reaches its own cap neighbourhood
        CHECK(snap.speed <= cap + 45.f);        // ... and never meaningfully over it
        const float dist = std::sqrt(lengthSq(snap.carPosition - loop.world().roadWaypoints[0]));
        CHECK(dist > prevDist);  // strictly more ground covered than the weaker pull
        prevSpeed = snap.speed;
        prevDist = dist;
    }
    CHECK(prevSpeed > 19000.f);  // a full pull reaches the full-speed neighbourhood

    // (b2) Genuine duty pulsing through the physics: pulsePeriod pinned to 8
    // with a cap that never binds, so a half trigger is a real duty pulse. The
    // half-trigger car must accelerate, yet end up strictly slower and
    // strictly behind the full-trigger car over the same horizon.
    const TriggerMapConfig pulseCfg{
        .pulsePeriod = 8,
        .reverseHoldSteps = kReverseHoldSteps,
        .reverseHoldBrake = kReverseTriggerHold,
        .topSpeed = 2.f * kGasTopSpeed,
        .brakeRampMax = 32.f,
    };
    GameLoop halfLoop(oneTickPerFrame());
    GameLoop fullLoop(oneTickPerFrame());
    TriggerDriveState halfSt, fullSt;
    FrameSnapshot halfSnap, fullSnap;
    int halfGasSteps = 0, fullGasSteps = 0;
    for (int t = 0; t < 400; ++t) {
        const MapStep h = stepTriggerDrive(halfLoop, halfSt, halfSnap, 0.f, 0.5f, pulseCfg);
        const MapStep f = stepTriggerDrive(fullLoop, fullSt, fullSnap, 0.f, 1.f, pulseCfg);
        halfGasSteps += h.out.input.gas ? 1 : 0;
        fullGasSteps += f.out.input.gas ? 1 : 0;
        halfSnap = h.snap;
        fullSnap = f.snap;
    }
    CHECK(halfGasSteps > 0);            // the pulse is on part of the time...
    CHECK(halfGasSteps < fullGasSteps);  // ... but strictly less than a full pull
    CHECK(halfSnap.speed > 0.f);              // half trigger does drive the car forward
    CHECK(halfSnap.speed < fullSnap.speed);   // ... but strictly slower than full
    CHECK(lengthSq(halfSnap.carPosition - halfLoop.world().roadWaypoints[0]) <
          lengthSq(fullSnap.carPosition - fullLoop.world().roadWaypoints[0]));
}

// ---------------------------------------------------------------------------
// (c) The brake-at-standstill reverse latch must engage reverse IN THE
// PHYSICS: position actually decreases along the heading, and the latched
// trigger reverse reproduces the digital REVERSE-bit hold bit-for-bit.
// ---------------------------------------------------------------------------
void testStandstillBrakeReverseLatchDrivesPhysicsReverse() {
    GameLoop loop(oneTickPerFrame());
    TriggerDriveState st;

    // One idle step to observe the fresh loop's start pose.
    const FrameSnapshot seed = stepTriggerDrive(loop, st, FrameSnapshot{}, 0.f, 0.f).snap;
    const Vec2 start = seed.carPosition;
    const float h0 = seed.heading;
    const Vec2 fwd{std::sin(h0), std::cos(h0)};

    // Arming: full brake trigger at a standstill, gas at rest. No reverse
    // until the K-th step; braking at a standstill holds position.
    FrameSnapshot snap = seed;
    for (int t = 0; t < kReverseHoldSteps; ++t) {
        const MapStep m = stepTriggerDrive(loop, st, snap, 1.f, 0.f);
        if (t < kReverseHoldSteps - 1) {
            CHECK(!m.out.reverseIntent);
            CHECK(m.out.input.brake && !m.out.input.reverse && !m.out.input.gas);
            CHECK(m.snap.speed == 0.f);
            CHECK(m.snap.carPosition.x == start.x && m.snap.carPosition.z == start.z);
        } else {
            CHECK(m.out.reverseIntent);  // latched exactly on the K-th step
            CHECK(m.out.input.reverse && !m.out.input.brake);
        }
        snap = m.snap;
    }

    // A mirror digital loop with the IDENTICAL tick history (one idle step,
    // K-1 brake-at-standstill steps, then the digital REVERSE bit held) — the
    // trigger loop's arming step already ticked one reverse input, so mirror
    // it before the reverse-phase comparison below.
    GameLoop digital(oneTickPerFrame());
    FrameSnapshot dSnap = digital.tickPlayer(1.f / 60.f, MenuInput{}, CarInput{});  // idle
    const CarInput digitalBrake{.steer = 0, .gas = false, .brake = true, .reverse = false};
    for (int t = 1; t < kReverseHoldSteps; ++t)
        dSnap = digital.tickPlayer(1.f / 60.f, MenuInput{}, digitalBrake);
    const CarInput digitalReverse{.steer = 0, .gas = false, .brake = false, .reverse = true};
    dSnap = digital.tickPlayer(1.f / 60.f, MenuInput{}, digitalReverse);  // mirrors the latch tick

    // Reverse phase through the physics: the latch drives the car backwards
    // for real, identical tick-for-tick to the digital reverse hold.
    float minBack = 0.f;  // most-negative projection along the heading so far
    for (int t = 0; t < 20; ++t) {
        const MapStep m = stepTriggerDrive(loop, st, snap, 1.f, 0.f);
        dSnap = digital.tickPlayer(1.f / 60.f, MenuInput{}, digitalReverse);
        CHECK(m.out.input.reverse);
        CHECK(m.snap.speed < 0.f);  // actually reversing in the physics
        const float back = dot(m.snap.carPosition - start, fwd);
        CHECK(back < minBack);  // position strictly decreases along the heading
        CHECK(samePlayerSnapshot(m.snap, dSnap));  // == digital reverse bit hold
        minBack = back;
        snap = m.snap;
    }

    // Release: brake trigger off clears the latch on the next map step; the
    // car then coasts back to a standstill and freezes there.
    const MapStep first = stepTriggerDrive(loop, st, snap, 0.f, 0.f);
    CHECK(!first.out.reverseIntent);
    CHECK(!first.out.input.reverse && !first.out.input.brake && !first.out.input.gas);
    FrameSnapshot rel = first.snap;
    bool reachedStandstill = false;
    float frozenBack = 0.f;
    for (int t = 0; t < 240; ++t) {
        rel = stepTriggerDrive(loop, st, rel, 0.f, 0.f).snap;
        if (!reachedStandstill && rel.speed == 0.f) {
            reachedStandstill = true;
            frozenBack = dot(rel.carPosition - start, fwd);
        }
        if (reachedStandstill) {
            CHECK(rel.speed == 0.f);  // it stays stopped...
            CHECK(dot(rel.carPosition - start, fwd) == frozenBack);  // ... and frozen in place
        }
    }
    CHECK(reachedStandstill);
    CHECK(frozenBack <= minBack);  // never returned below the release point
}

// ---------------------------------------------------------------------------
// (d) GATE-OFF REGRESSION: with mapTriggersToDrive never invoked (the
// additive map unwired), the entire legacy digital path must still produce
// exactly its documented digital outcomes over a scripted 120-step pattern.
// Expectations are computed IN-TEST from the legacy contracts (menu.h default
// bindings, input.h threshold/deadzone constants) — nothing from the feature.
// ---------------------------------------------------------------------------
struct ScriptStep {
    std::uint16_t pad = 0;
    float stickX = 0.f;
    float triggerGas = 0.f;
    float triggerBrake = 0.f;
};

// Deterministic 120-step digital pad script: accelerate, steer while
// accelerating, brake to a standstill, hold the digital reverse bit, coast,
// then a gauntlet of analog probes (above-rest/below-threshold, exactly at
// the threshold, just under it, full pull, brake trigger).
ScriptStep legacyScript(int t) {
    ScriptStep s;
    if (t >= 15 && t < 55) s.pad |= buttonMask(PhysicalButton::CROSS);   // gas
    if (t >= 40 && t < 55) s.pad |= buttonMask(PhysicalButton::SQUARE);  // gas+brake
    if (t >= 55 && t < 75) s.pad = buttonMask(PhysicalButton::SQUARE);   // brake to standstill
    if (t >= 75 && t < 95) s.pad = buttonMask(PhysicalButton::CIRCLE);   // reverse bit
    if (t >= 100 && t < 105) s.stickX = 0.6f;   // steer while coasting
    if (t >= 105 && t < 110) s.stickX = -0.6f;
    if (t >= 110 && t < 120) {
        s.triggerGas = (t < 113) ? 0.05f             // above kTriggerRest, below threshold: OFF
                       : (t < 115) ? 0.50f           // exactly at the threshold: ON
                                   : (t == 115) ? 0.49f  // just under: OFF
                                                : 1.00f;  // full pull: ON
        s.triggerBrake = (t == 119) ? 0.51f : 0.f;
        s.stickX = (t % 2 == 0) ? 0.6f : -0.6f;
    }
    return s;
}

// The expectation oracle, derived from the documented legacy contract only:
// gas/brake from the default digital bindings (CROSS/SQUARE) OR the
// documented analog-axis threshold coupling; reverse only from its digital
// bit; steer from the documented stick deadzone. It restates the CONTRACT,
// not the implementation.
CarInput legacyExpected(const ScriptStep& s) {
    CarInput e;
    e.gas = (s.pad & buttonMask(PhysicalButton::CROSS)) != 0 || s.triggerGas >= kTriggerThreshold;
    e.brake = (s.pad & buttonMask(PhysicalButton::SQUARE)) != 0 ||
              s.triggerBrake >= kTriggerThreshold;
    e.reverse = (s.pad & buttonMask(PhysicalButton::CIRCLE)) != 0;
    e.steer = s.stickX >= kStickDeadzone ? 1 : (s.stickX <= -kStickDeadzone ? -1 : 0);
    return e;
}

void testGateOffLegacyPathByteExactRegression() {
    const MenuInput mi;
    ControllerConfig cfg;
    // Pin the default legacy bindings the oracle hardcodes, so an accidental
    // default change fails here loudly instead of silently redefining truth.
    CHECK(cfg.getButtonConfig(ControllerAction::ACCELERATE) == PhysicalButton::CROSS);
    CHECK(cfg.getButtonConfig(ControllerAction::BRAKE) == PhysicalButton::SQUARE);
    CHECK(cfg.getButtonConfig(ControllerAction::REVERSE) == PhysicalButton::CIRCLE);

    GameLoop legacyLoop(oneTickPerFrame());  // fed by the real playerCarInput
    GameLoop oracleLoop(oneTickPerFrame());  // fed by the in-test expectations
    GameLoop rerunLoop(oneTickPerFrame());   // determinism pin of the legacy path
    CarState gotChain, expChain;             // direct tickCar (car_physics.cpp) chains

    const Vec2 start = legacyLoop.world().roadWaypoints[0];
    FrameSnapshot snapGot, snapExp, snapRerun;
    for (int t = 0; t < 120; ++t) {
        const ScriptStep s = legacyScript(t);
        const PadState pad{.pressedButtons = s.pad};
        const AnalogAxes axes{.stickX = s.stickX, .triggerGas = s.triggerGas,
                              .triggerBrake = s.triggerBrake};
        const CarInput exp = legacyExpected(s);

        // The legacy digital path under test (mapTriggersToDrive is NEVER
        // called in this function; the new feature must be invisible here).
        const CarInput got = playerCarInput(pad, axes, cfg);
        CHECK(sameCarInput(got, exp));  // byte-exact CarInput sequence

        // Gate-off sharp edges: sub-threshold analog triggers — including ones
        // above the NEW kTriggerRest — must stay inert on the legacy path.
        if (s.triggerGas > 0.f && s.triggerGas < kTriggerThreshold) CHECK(!got.gas);
        if (s.triggerBrake > 0.f && s.triggerBrake < kTriggerThreshold) CHECK(!got.brake);

        snapGot = legacyLoop.tickPlayer(1.f / 60.f, mi, got);
        snapExp = oracleLoop.tickPlayer(1.f / 60.f, mi, exp);
        snapRerun = rerunLoop.tickPlayer(1.f / 60.f, mi, got);
        CHECK(samePlayerSnapshot(snapGot, snapExp));    // legacy == expectation, consumer-level
        CHECK(samePlayerSnapshot(snapGot, snapRerun));  // deterministic on a second run

        tickCar(gotChain, got);  // the physics consumer (car_physics.cpp drive())
        tickCar(expChain, exp);
        CHECK(sameCarState(gotChain, expChain));  // byte-exact CarState snapshots
        CHECK(snapGot.speed == static_cast<float>(gotChain.speed));  // loop/physics coherence
    }

    // The script actually exercised forward motion (otherwise the regression
    // above would be vacuous) and ran the full 120-step pattern.
    CHECK(snapGot.frame == 120);
    CHECK(lengthSq(snapGot.carPosition - start) > 0.f);
}

// ---------------------------------------------------------------------------
// (e) Multi-instance: two pads' TriggerDriveState (2 ports) must share
// NOTHING — stepping one must not shift the other's pulsePhase or stillSteps.
// ---------------------------------------------------------------------------
void testTwoPadStateIndependence() {
    // (e1) pulsePhase independence. With pulsePeriod 4 and a half trigger the
    // gas duty genuinely pulses (on 3 of every 4 steps). Step pad A alone,
    // then step pads A and B interleaved with IDENTICAL stimulus: B's
    // stepping between A's steps must not shift A's phase (and vice versa),
    // and both interleaved sequences must equal the alone-run sequence. A
    // shared/leaky phase would put A and B half a cycle apart and fail this.
    const TriggerMapConfig pulseCfg{
        .pulsePeriod = 4,
        .reverseHoldSteps = kReverseHoldSteps,
        .reverseHoldBrake = kReverseTriggerHold,
        .topSpeed = 2.f * kGasTopSpeed,  // the cap never binds at standstill speed
        .brakeRampMax = 32.f,
    };
    const TriggerDriveInputs half{
        .leftTrigger = 0.f, .rightTrigger = 0.5f, .speed = 0, .standstill = true};

    TriggerDriveState alone;
    std::vector<CarInput> aloneSeq;
    for (int t = 0; t < 16; ++t) aloneSeq.push_back(mapTriggersToDrive(half, alone, pulseCfg).input);
    // Self-validation: the pinned config really pulses (on,on,on,off per cycle).
    CHECK(aloneSeq.size() == 16);
    for (int t = 0; t < 16; ++t)
        CHECK(aloneSeq[static_cast<std::size_t>(t)].gas == ((t % 4) != 3));

    TriggerDriveState padA, padB;
    std::vector<CarInput> aSeq, bSeq;
    for (int t = 0; t < 16; ++t) {
        aSeq.push_back(mapTriggersToDrive(half, padA, pulseCfg).input);
        bSeq.push_back(mapTriggersToDrive(half, padB, pulseCfg).input);  // B between A's steps
    }
    CHECK(sameCarInputSeq(aSeq, bSeq));
    CHECK(sameCarInputSeq(aSeq, aloneSeq));  // interleaving another pad changes nothing

    // (e2) stillSteps/reverse-latch independence on two ports. Pad A holds
    // brake-at-standstill from frame 0 and must arm on its own K-th step.
    // Pad B starts holding at frame 4 and must arm K steps after ITS hold
    // begins — never on A's schedule. A shared stillSteps counter would arm B
    // early (at A's arming frame) and fail this.
    const TriggerDriveInputs rest{
        .leftTrigger = 0.f, .rightTrigger = 0.f, .speed = 0, .standstill = true};
    const TriggerDriveInputs brakeStandstill{
        .leftTrigger = 1.f, .rightTrigger = 0.f, .speed = 0, .standstill = true};
    TriggerDriveState portA, portB;
    TriggerDriveState lateAlone;  // what pad B does with NO other pad stepping
    std::vector<CarInput> bInterleaved, bAlone;
    for (int t = 0; t < 13; ++t) {
        const bool holds = t >= 4;  // pad B holds brake from frame 4
        const TriggerDriveOutput oa = mapTriggersToDrive(brakeStandstill, portA);
        const TriggerDriveOutput ob = mapTriggersToDrive(holds ? brakeStandstill : rest, portB);
        bInterleaved.push_back(ob.input);
        bAlone.push_back(mapTriggersToDrive(holds ? brakeStandstill : rest, lateAlone).input);

        if (t == kReverseHoldSteps - 1) {
            CHECK(oa.reverseIntent);   // A armed on its own K-th step
            CHECK(!ob.reverseIntent);  // B has held only 4 steps: must NOT be armed
        } else if (t > kReverseHoldSteps - 1 && t < 4 + kReverseHoldSteps - 1) {
            CHECK(oa.input.reverse);       // A stays latched (brake still held)
            CHECK(!ob.reverseIntent);      // B not yet on its own schedule
        } else if (t == 4 + kReverseHoldSteps - 1) {
            CHECK(ob.reverseIntent);  // B armed exactly K steps after IT began holding
            CHECK(ob.input.reverse && !ob.input.brake);
        }
    }
    CHECK(sameCarInputSeq(bInterleaved, bAlone));  // A's stepping never perturbed B
}

}  // namespace

int main() {
    testFullTriggerEqualsLegacyDigitalHold();
    testPartialTriggerAcceleratesSlowerThanFull();
    testStandstillBrakeReverseLatchDrivesPhysicsReverse();
    testGateOffLegacyPathByteExactRegression();
    testTwoPadStateIndependence();
    std::printf("trigger_integration: %d checks, %d failures\n", gChecks, gFailures);
    if (gFailures != 0) std::fprintf(stderr, "%d failure(s)\n", gFailures);
    return gFailures == 0 ? 0 : 1;
}
