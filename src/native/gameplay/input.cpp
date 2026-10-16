#include "input.h"

#include <algorithm>
#include <cmath>

namespace rt::native::gameplay {

namespace {
bool down(const PadState& pad, PhysicalButton b) {
    return b != PhysicalButton::NONE && (pad.pressedButtons & buttonMask(b)) != 0;
}

float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
}  // namespace

CarInput playerCarInput(const PadState& pad, const AnalogAxes& axes, const ControllerConfig& cfg) {
    CarInput in{};
    in.steer = axes.stickX >= kStickDeadzone ? 1 : (axes.stickX <= -kStickDeadzone ? -1 : 0);
    in.gas = down(pad, cfg.getButtonConfig(ControllerAction::ACCELERATE)) || axes.triggerGas >= kTriggerThreshold;
    in.brake = down(pad, cfg.getButtonConfig(ControllerAction::BRAKE)) || axes.triggerBrake >= kTriggerThreshold;
    in.reverse = down(pad, cfg.getButtonConfig(ControllerAction::REVERSE));
    return in;
}

void setPressed(PadState& pad, PhysicalButton button, bool isDown) {
    const std::uint16_t m = buttonMask(button);
    if (isDown) pad.pressedButtons = static_cast<std::uint16_t>(pad.pressedButtons | m);
    else pad.pressedButtons = static_cast<std::uint16_t>(pad.pressedButtons & ~m);
}

PadState padWith(PhysicalButton a, PhysicalButton b, PhysicalButton c) {
    PadState p;
    setPressed(p, a); setPressed(p, b); setPressed(p, c);
    return p;
}

float triggerGasDuty(float trigger) { return 0.5f + 0.5f * clamp01(trigger); }

int triggerBrakeRampLimit(float trigger, float rampMax) {
    const float lo = rampMax > 0.f ? rampMax : 0.f;
    return std::clamp(static_cast<int>(std::ceil(clamp01(trigger) * lo)), 1,
                      static_cast<int>(lo > 0.f ? lo : 32.f));
}

bool triggerPulseOn(float duty, int period, int phase) {
    if (period < 1) period = 1;
    if (phase < 0 || phase >= period) phase = 0;
    const int on = static_cast<int>(std::ceil(clamp01(duty) * static_cast<float>(period)));
    return phase < on;
}

TriggerDriveOutput mapTriggersToDrive(const TriggerDriveInputs& in,
                                      TriggerDriveState& state,
                                      const TriggerMapConfig& cfg) {
    const int period = cfg.pulsePeriod > 0 ? cfg.pulsePeriod : 1;
    const int holdSteps = cfg.reverseHoldSteps > 0 ? cfg.reverseHoldSteps : 1;

    const float gas = clamp01(in.rightTrigger);
    const float brake = clamp01(in.leftTrigger);
    const bool gasAtRest = gas <= kTriggerRest;
    const bool brakeAtRest = brake <= kTriggerRest;
    const bool reverseHold = brake >= cfg.reverseHoldBrake;

    TriggerDriveOutput out;

    // Reverse latch (DriveFeel.cpp:267-284): the brake held at a standstill for
    // K steps selects reverse; gas or releasing the brake clears it. The latch is
    // evaluated against the pre-step speed so it holds on the step it arms.
    if (state.reversing && (!reverseHold || !gasAtRest)) {
        state.reversing = false;
        state.stillSteps = 0;
    } else if (!state.reversing) {
        if (reverseHold && gasAtRest && in.standstill) {
            if (state.stillSteps < holdSteps) ++state.stillSteps;
        } else {
            state.stillSteps = 0;
        }
        if (state.stillSteps >= holdSteps) state.reversing = true;
    }
    out.reverseIntent = state.reversing;

    // Gas: pulsed at 0.5 + 0.5*trigger under a speed cap of trigger*topSpeed.
    // A fully pressed trigger has duty 1 and stays on unconditionally, so it is
    // bit-for-bit the legacy digital hold (the compatibility anchor).
    out.gasDuty = triggerGasDuty(gas);
    out.gasSpeedCap = gas * cfg.topSpeed;
    bool pulse = false;
    if (!gasAtRest) {
        if (out.gasDuty >= 1.f) {
            pulse = true;
        } else {
            const bool capped =
                static_cast<float>(std::abs(in.speed)) >= out.gasSpeedCap;
            pulse = !capped && triggerPulseOn(out.gasDuty, period, state.pulsePhase);
        }
    }
    state.pulsePhase = (state.pulsePhase + 1) % period;
    out.gasPulse = pulse;

    // Brake: hold the +0x1FE ramp at ceil(32*trigger) while the bit is set.
    out.brakeRampLimit = brakeAtRest ? 0 : triggerBrakeRampLimit(brake, cfg.brakeRampMax);

    // Compose the CarInput the session/physics read. Reverse replaces the other
    // two bits while latched, as the recomp hook does (DriveFeel.cpp:283).
    out.input = CarInput{};
    if (state.reversing) {
        out.input.reverse = true;
        out.input.gas = pulse;  // push out of reverse under a pulsed gas
    } else {
        out.input.gas = pulse;
        out.input.brake = !brakeAtRest;
    }
    return out;
}

} // namespace rt::native::gameplay
