#include "gameplay/vehicle_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace rt::native::gameplay {
namespace {

std::int32_t scaledPositionDelta(std::int32_t velocity) {
    // EE div is signed quotient toward zero; C++20 integer division has that rule.
    return (velocity << 4) / kPositionVelocityDenominator;
}

std::int16_t clampS16(std::int32_t value) {
    return static_cast<std::int16_t>(std::clamp(value,
        std::int32_t{std::numeric_limits<std::int16_t>::min()},
        std::int32_t{std::numeric_limits<std::int16_t>::max()}));
}

}  // namespace

double rawSpeedFromF15(const F15Vector& velocity) {
    const double x = static_cast<double>(velocity.x) / kF15One;
    const double y = static_cast<double>(velocity.y) / kF15One;
    const double z = static_cast<double>(velocity.z) / kF15One;
    return std::sqrt(x * x + y * y + z * z) * kF15ToRawSpeed;
}

SpeedSample sampleSpeed(const F15Vector& velocity, bool clampToDial) {
    const double raw = rawSpeedFromF15(velocity);
    int mph = static_cast<int>(raw / kMphDivisor);
    if (clampToDial) mph = std::min(mph, kSpeedometerMaxMph);
    return {raw, mph};
}

int speedometerMph(const F15Vector& velocity, bool clampToDial) {
    return sampleSpeed(velocity, clampToDial).mph;
}

void tickKinematics(VehicleKinematics& state) {
    if (state.airborne) state.velocity.y += kGravityF15PerTick;
    else state.velocity.y = 0;
    state.position.x += scaledPositionDelta(state.velocity.x);
    state.position.y += scaledPositionDelta(state.velocity.y);
    state.position.z += scaledPositionDelta(state.velocity.z);
}

void steerTick(SteeringState& state, std::int32_t longitudinalVelocity,
               std::int32_t lateralForce, std::int32_t speed) {
    const std::int32_t yaw = state.yaw;
    if (yaw >= -0xfff && yaw < 0x1000) {
        if (lateralForce < 0) {
            state.command = 0;
        } else if (speed < -lateralForce || speed > lateralForce) {
            std::int32_t target = speed < 0 ? (speed - lateralForce) / 3 : (speed + lateralForce) / 3;
            target = std::clamp(target, -0x200, 0x200);
            if (target - state.command < 9) state.command = clampS16(target);
            else state.command = clampS16(state.command + (speed < 0 ? -8 : 8));
        } else {
            state.command = 0;
        }
    }
    if (longitudinalVelocity == 0) state.command = 0;
    state.yaw = clampS16(yaw + state.command);
}

}  // namespace rt::native::gameplay
