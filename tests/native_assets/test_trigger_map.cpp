// Deterministic contract test for the additive analog-trigger map
// (mapTriggersToDrive in src/native/gameplay/input.*). No game data.
//
// Compatibility anchor: a fully pressed gas trigger must produce the same
// CarInput gas bit as the legacy digital playerCarInput hold, and a fully
// pressed brake trigger the same brake bit — so callers that pass 1.0/0.0 see
// the original behaviour unchanged.
#include "../../src/native/gameplay/input.h"

#include <cmath>
#include <cstdio>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

// Advance the map with fixed inputs; standstill/speed can be recomputed per step.
TriggerDriveOutput step(TriggerDriveState& st, float brake, float gas, int speed,
                        bool standstill, const TriggerMapConfig& cfg = {}) {
    return mapTriggersToDrive({.leftTrigger = brake, .rightTrigger = gas,
                               .speed = speed, .standstill = standstill}, st, cfg);
}

void testZeroTriggersAreZeroOutput() {
    TriggerDriveState st;
    const TriggerDriveOutput o = step(st, 0.f, 0.f, 0, true);
    CHECK(!o.input.gas && !o.input.brake && !o.input.reverse);
    CHECK(!o.reverseIntent);
    CHECK(!o.gasPulse);
    CHECK(o.brakeRampLimit == 0);
    // Duty at rest is reported but the Gas bit stays off (the bit, not the duty,
    // is what the physics reads).
    CHECK(std::fabs(o.gasDuty - 0.5f) < 1e-6f);
    // Holding zero triggers across many steps never raises a command.
    for (int i = 0; i < 32; ++i) {
        const TriggerDriveOutput z = step(st, 0.f, 0.f, 0, true);
        CHECK(!z.input.gas && !z.input.brake && !z.input.reverse);
    }
}

void testFullGasEqualsLegacyDigitalHold() {
    // Legacy: a pad with the ACCELERATE button held reads gas == true each step.
    ControllerConfig cfg;
    const AnalogAxes none;
    const PadState heldPad = padWith(cfg.getButtonConfig(ControllerAction::ACCELERATE));

    TriggerDriveState st;
    for (int i = 0; i < 64; ++i) {
        const CarInput legacy = playerCarInput(heldPad, none, cfg);
        const TriggerDriveOutput o = step(st, 0.f, 1.f, 0, true);
        CHECK(legacy.gas == o.input.gas);   // gas bit matches every step
        CHECK(o.input.gas);                 // and it is on
        CHECK(!o.input.brake && !o.input.reverse);
        CHECK(o.gasPulse);
    }
    // Even at/above the speed cap a full trigger still holds gas on (duty 1).
    TriggerDriveState fast;
    for (int i = 0; i < 16; ++i)
        CHECK(step(fast, 0.f, 1.f, 20000, false).input.gas);
}

void testFullBrakeEqualsLegacyDigitalHold() {
    ControllerConfig cfg;
    const AnalogAxes none;
    const PadState heldPad = padWith(cfg.getButtonConfig(ControllerAction::BRAKE));

    TriggerDriveState st;
    for (int i = 0; i < 64; ++i) {
        const CarInput legacy = playerCarInput(heldPad, none, cfg);
        const TriggerDriveOutput o = step(st, 1.f, 0.f, 5000, false);
        CHECK(legacy.brake == o.input.brake);  // brake bit matches every step
        CHECK(o.input.brake);
        CHECK(!o.input.reverse);
        CHECK(o.brakeRampLimit == 32);
    }
}

void testDutyIsMonotonicInTrigger() {
    float prev = -1.f;
    for (int i = 0; i <= 100; ++i) {
        const float d = triggerGasDuty(static_cast<float>(i) / 100.f);
        CHECK(d >= prev);
        prev = d;
    }
    CHECK(triggerGasDuty(0.f) == 0.5f);
    CHECK(triggerGasDuty(1.f) == 1.f);
    // Clamped outside 0..1.
    CHECK(triggerGasDuty(-3.f) == 0.5f);
    CHECK(triggerGasDuty(5.f) == 1.f);

    // The realised on-fraction of the pulse cycle rises with the trigger. Use a
    // wide N so ceil() resolves the intermediate duties.
    const TriggerMapConfig cfg{.pulsePeriod = 20};
    float prevOn = -1.f;
    for (float t : {0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.0f}) {
        TriggerDriveState st;
        int on = 0;
        for (int i = 0; i < 400; ++i)
            on += step(st, 0.f, t, 0, false, cfg).input.gas;  // no cap: standstill
        const float frac = static_cast<float>(on) / 400.f;
        CHECK(frac >= prevOn - 1e-6f);
        prevOn = frac;
    }
    CHECK(prevOn > 0.95f);  // full trigger is on every step
}

void testDutyNeverExceedsOneAndRampStaysInRange() {
    for (int i = -5; i <= 105; ++i) {
        const float t = static_cast<float>(i) / 100.f;
        const float d = triggerGasDuty(t);
        CHECK(d >= 0.f && d <= 1.f);
        for (int period = 1; period <= 8; ++period)
            for (int phase = 0; phase < period; ++phase)
                (void)triggerPulseOn(d, period, phase);  // must not assert/overflow
        // Ramp limit is always within its range and monotone in the trigger.
        const int r = triggerBrakeRampLimit(t);
        CHECK(r >= 1 && r <= 32);
    }
    CHECK(triggerBrakeRampLimit(0.f) == 1);
    CHECK(triggerBrakeRampLimit(0.5f) == 16);
    CHECK(triggerBrakeRampLimit(1.f) == 32);
}

void testStandstillReverseLatchAndRelease() {
    const TriggerMapConfig cfg;  // K = 8
    TriggerDriveState st;
    // Full brake, no gas, at a standstill: no reverse until K steps.
    for (int i = 0; i < cfg.reverseHoldSteps - 1; ++i) {
        const TriggerDriveOutput o = step(st, 1.f, 0.f, 0, true);
        CHECK(!o.reverseIntent);
        CHECK(!o.input.reverse);
    }
    const TriggerDriveOutput armed = step(st, 1.f, 0.f, 0, true);
    CHECK(armed.reverseIntent);
    CHECK(armed.input.reverse);
    // While reversing the gas/brake bits are replaced by reverse (plus a push).
    CHECK(!armed.input.brake);
    // Releasing the brake clears reverse intent.
    const TriggerDriveOutput released = step(st, 0.f, 0.f, 0, true);
    CHECK(!released.reverseIntent);
    CHECK(!released.input.reverse);
    // ... and a single-frame release re-arms the full K-step hold (the counter
    // resets), so reverse does not flicker back on at once.
    for (int i = 0; i < cfg.reverseHoldSteps - 1; ++i)
        CHECK(!step(st, 1.f, 0.f, 0, true).reverseIntent);
    CHECK(step(st, 1.f, 0.f, 0, true).reverseIntent);
    // Gas also clears it: re-arm, then press gas.
    TriggerDriveState st3;
    for (int i = 0; i < cfg.reverseHoldSteps; ++i) step(st3, 1.f, 0.f, 0, true);
    CHECK(step(st3, 1.f, 0.f, 0, true).reverseIntent);
    CHECK(!step(st3, 1.f, 0.8f, 0, false).reverseIntent);  // gas clears the latch
    // Moving is not a standstill: the latch never arms in gear.
    TriggerDriveState st4;
    for (int i = 0; i < 50; ++i)
        CHECK(!step(st4, 1.f, 0.f, 5000, false).reverseIntent);
}

void testBoundaryValues() {
    // Exactly 0.0 and 1.0 on each trigger, with and without the reverse latch.
    TriggerDriveState st;
    const TriggerDriveOutput both = step(st, 1.f, 1.f, 3000, false);
    CHECK(both.input.gas && both.input.brake);  // both bits set (not reversing: gas present)
    CHECK(!both.input.reverse);

    TriggerDriveState st2;
    const TriggerDriveOutput zeros = step(st2, 0.f, 0.f, 0, true);
    CHECK(!zeros.input.gas && !zeros.input.brake && !zeros.input.reverse);

    // Threshold boundaries: at/below rest is off; just above rest commands.
    TriggerDriveState st3;
    CHECK(!step(st3, kTriggerRest, kTriggerRest, 0, true).input.brake);
    CHECK(step(st3, kTriggerRest + 0.01f, 0.f, 5000, false).input.brake);
    CHECK(!step(st3, 0.f, kTriggerRest, 0, true).input.gas);
}

}  // namespace

int main() {
    testZeroTriggersAreZeroOutput();
    testFullGasEqualsLegacyDigitalHold();
    testFullBrakeEqualsLegacyDigitalHold();
    testDutyIsMonotonicInTrigger();
    testDutyNeverExceedsOneAndRampStaysInRange();
    testStandstillReverseLatchAndRelease();
    testBoundaryValues();
    if (g_fail != 0) {
        std::fprintf(stderr, "%d check(s) failed\n", g_fail);
        return 1;
    }
    std::puts("trigger_map: all checks passed");
    return 0;
}
