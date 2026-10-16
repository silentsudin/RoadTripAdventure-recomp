// Authored player input: pad + analog axes -> CarInput (replaces IOP/pad path).
#pragma once
#include "car_state.h"
#include "menu.h"

namespace rt::native::gameplay {

struct AnalogAxes {
    float stickX = 0.f;       // -1..1
    float stickY = 0.f;       // -1..1
    float triggerGas = 0.f;   // 0..1
    float triggerBrake = 0.f; // 0..1
};

constexpr float kStickDeadzone = 0.2f;
constexpr float kTriggerThreshold = 0.5f;

CarInput playerCarInput(const PadState& pad, const AnalogAxes& axes, const ControllerConfig& cfg);

// Test/helper: set or clear a button bit in a PadState.
void setPressed(PadState& pad, PhysicalButton button, bool down = true);
PadState padWith(PhysicalButton a, PhysicalButton b = PhysicalButton::NONE,
                 PhysicalButton c = PhysicalButton::NONE);

// ---------------------------------------------------------------------------
// Analog-trigger driving map (additive).
//
// The original car physics reads only digital control bits — gas, brake and
// reverse (docs/game-mechanics/car-physics.md:29; the per-port action masks at
// `0x1780A48`, docs/game-mechanics/car-physics.md:189-191). Modern pads give
// analog triggers instead, so this converts {brake,gas} trigger travel into the
// bits the session/physics consume, exactly as the sibling recomp app does in
// src/game/DriveFeel.{h,cpp}:
//   gas   = the Gas bit pulsed at duty 0.5 + 0.5*trigger (DriveFeel.cpp:40),
//           under a speed cap of trigger * top speed (DriveFeel.cpp:42-51);
//   brake = the +0x1FE ramp held at ceil(32*trigger) (DriveFeel.cpp:53-56;
//           the ramp itself is 0..32, docs/game-mechanics/car-physics.md:165);
//   reverse = the brake held at a standstill for K steps selects reverse
//           (DriveFeel.cpp:267-284), the trigger analogue of the reverse bit.
//
// AUTHORED-for-native: this is a pure, deterministic port of that authored
// behaviour. It reads no guest memory and touches none of the existing
// playerCarInput()/PadState/AnalogAxes symbols, so callers that ignore it see
// the old behaviour byte-for-byte. Grounding for the bit/ramp semantics is
// cited above; the mapping arithmetic is authored-for-native.
// ---------------------------------------------------------------------------

// A trigger at or below this is at rest; at or above kTriggerFull it is fully
// pressed (the sibling app's kTriggerRest / kTriggerFull, DriveFeel.h:12-14).
constexpr float kTriggerRest = 0.04f;
constexpr float kTriggerFull = 0.97f;

// Reverse arming: the brake trigger held at/above this, at a standstill, for
// kReverseHoldSteps consecutive logic steps selects reverse.
constexpr float kReverseTriggerHold = 0.90f;
constexpr int   kReverseHoldSteps = 8;

// Gas duty cycle width (N): the Gas bit is on for ceil(duty * N) frames of each
// N-frame cycle. N = 2 keeps full-trigger gas permanently on, matching a legacy
// digital hold (the compatibility anchor).
constexpr int   kGasPulsePeriod = 2;
// Default gas speed cap denominator: the starter car's top speed (CarState
// physics units; matches DriveFeel.h:42's default 20000).
constexpr float kGasTopSpeed = 20000.f;

// Tuning for mapTriggersToDrive. Defaults are the constants above; every field
// is a plain value so a caller can pin a deterministic variant.
struct TriggerMapConfig {
    int   pulsePeriod = kGasPulsePeriod;        // N >= 1
    int   reverseHoldSteps = kReverseHoldSteps; // K >= 1
    float reverseHoldBrake = kReverseTriggerHold;
    float topSpeed = kGasTopSpeed;              // trigger * topSpeed = gas cap
    float brakeRampMax = 32.f;                  // ceil(brakeRampMax * trigger)
};

// Per-port persistent state: the pulse phase and the reverse latch. One step
// per fixed logic step; deterministic (same inputs + state -> same output).
struct TriggerDriveState {
    int  pulsePhase = 0;   // 0 .. pulsePeriod-1
    int  stillSteps = 0;   // consecutive standstill-brake steps
    bool reversing = false;
};

// Runtime inputs for one step. Left trigger is brake, right trigger is gas (the
// standard pad convention, matching AnalogAxes.triggerBrake/triggerGas).
struct TriggerDriveInputs {
    float leftTrigger = 0.f;   // brake travel, 0..1
    float rightTrigger = 0.f;  // gas travel, 0..1
    int   speed = 0;           // CarState::speed (signed physics units)
    bool  standstill = true;   // true while the car is stationary (speed == 0)
};

// One step's result. `input` carries exactly the fields the session/physics
// read (CarInput.gas/brake/reverse); the rest is for glue, HUD and tests.
struct TriggerDriveOutput {
    CarInput input;            // the gas/brake/reverse bits for this step
    bool  reverseIntent = false;
    bool  gasPulse = false;    // the duty-pulsed Gas bit this step
    float gasDuty = 0.f;       // 0.5 + 0.5*rightTrigger (0 at rest / capped)
    float gasSpeedCap = 0.f;   // rightTrigger * topSpeed
    int   brakeRampLimit = 0;  // ceil(32*leftTrigger), 1..32, 0 when not braking
};

// Pure helpers, no state.
float triggerGasDuty(float trigger);       // 0.5 + 0.5*clamp01(trigger)
int   triggerBrakeRampLimit(float trigger, float rampMax = 32.f); // ceil(t*rampMax)
// Whether the Gas bit is on at `phase` of an N-frame cycle for this duty: on for
// the first ceil(duty * period) frames.
bool  triggerPulseOn(float duty, int period, int phase);

// One mapping step: analog triggers + speed/standstill -> the CarInput bits the
// session and physics read. Full triggers reproduce the legacy digital hold; a
// partial gas trigger is duty-pulsed. `state` is advanced in place.
TriggerDriveOutput mapTriggersToDrive(const TriggerDriveInputs& in,
                                      TriggerDriveState& state,
                                      const TriggerMapConfig& cfg = {});

} // namespace rt::native::gameplay
