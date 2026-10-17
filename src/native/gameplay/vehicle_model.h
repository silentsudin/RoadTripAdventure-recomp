// Fixed-point vehicle kinematics reconstructed from SLUS-203.98 vehicle tick.
#pragma once

#include <cstdint>

namespace rt::native::gameplay {

// VU f15 values held by car+0xf0..+0xfc. One unit is 1/32768 of a velocity unit.
struct F15Vector {
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t z = 0;
    constexpr bool operator==(const F15Vector&) const = default;
};

// Car position words at +0xa0/+0xa4/+0xa8, before conversion by 20971.51953125.
struct VehiclePosition {
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t z = 0;
    constexpr bool operator==(const VehiclePosition&) const = default;
};

struct VehicleKinematics {
    VehiclePosition position{};
    F15Vector velocity{};
    bool airborne = false;
};

struct SpeedSample {
    double rawUnitsPerFrame = 0.0;  // f15 vector length * 216, before mph conversion.
    int mph = 0;                    // trunc(raw / 1.6089999676), capped at 186.
};

struct SteeringState {
    std::int16_t command = 0;  // car+0x1d2
    std::int16_t yaw = 0;      // car+0x1d6
};

constexpr std::int32_t kF15One = 32768;
constexpr std::int32_t kGravityF15PerTick = -0x59;
constexpr std::int32_t kPositionVelocityNumerator = 16;
constexpr std::int32_t kPositionVelocityDenominator = 0x19;
constexpr double kF15ToRawSpeed = 216.0;
constexpr double kMphDivisor = 1.6089999675750732;
constexpr int kSpeedometerMaxMph = 186;
constexpr double kWorldUnitsPerPositionWord = 1.0 / 20971.51953125;
constexpr double kWorldUnitsPerRawSpeedFrame =
    (double(kF15One) * kPositionVelocityNumerator) /
    (double(kPositionVelocityDenominator) * 20971.51953125 * kF15ToRawSpeed);

// Exact f15 length conversion from GetSpeedFromF15Vector (0x21d358).
double rawSpeedFromF15(const F15Vector& velocity);
SpeedSample sampleSpeed(const F15Vector& velocity, bool clampToDial = true);
int speedometerMph(const F15Vector& velocity, bool clampToDial = true);

// Applies the -0x59 airborne vertical force and the velocity << 4 / 0x19
// position commit from UpdateVehiclePhysics (0x21ba70). Ground-contact resolution
// has already zeroed vertical velocity when airborne is false.
void tickKinematics(VehicleKinematics& state);

// The integer command/yaw portion of UpdateSteeringDamping (0x21a088).
// lateralForce is its param_3 and speed is its param_5.
void steerTick(SteeringState& state, std::int32_t longitudinalVelocity,
               std::int32_t lateralForce, std::int32_t speed);

}  // namespace rt::native::gameplay
