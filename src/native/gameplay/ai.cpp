// Authored AI traffic brain implementation. See ai.h for the model and the
// evidence boundary (docs/native-reference/traffic-ai.md). Pure host C++, no
// game data. Default path following remains an authored approximation.
#include "gameplay/ai.h"

#include <algorithm>
#include <bit>
#include <numbers>

namespace rt::native::gameplay {
namespace {

constexpr float kPi = std::numbers::pi_v<float>;
constexpr float kTwoPi = 2.f * kPi;

// Same shortest-arc wrapping used by the chase camera.
float wrapAngle(float a) {
    a = std::fmod(a + kPi, kTwoPi);
    if (a < 0.f) a += kTwoPi;
    return a - kPi;
}

// Camera-style bounded approach: move `value` toward `target` by at most
// `maxStep` per tick.
float slewToward(float value, float target, float maxStep) {
    const float d = target - value;
    if (std::fabs(d) <= maxStep) return target;
    return value + (d > 0.f ? maxStep : -maxStep);
}

}  // namespace

std::uint8_t trafficSpeedInput(std::int32_t speed, std::uint8_t routeTarget,
                               std::uint8_t cap) {
    // CONFIRMED 0x250980..0x2509c0: 32-bit shift/subtract computes 216*s.
    // Preserve low-word wrap without signed-overflow UB; div truncates to zero.
    const auto product = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(speed) *
        static_cast<std::uint32_t>(TrafficAiConstants::speedNumerator));
    const int target = routeTarget & 0x7f;
    const int delta = product / TrafficAiConstants::routeSpeedDivisor - target;
    std::uint8_t input = target == 0 || delta <= 0 ? 1 : (delta == 1 ? 0 : 2);
    // CONFIRMED 0x250c48..0x250c9c: cap uses half the divisor, and
    // nonpositive delta preserves the base command (does not force gas).
    if (cap != 0) {
        const int capDelta = product / TrafficAiConstants::capSpeedDivisor - cap;
        if (capDelta > 0) input = capDelta > 2 ? 2 : 0;
    }
    return input;
}

std::uint8_t chooseTrafficBranch(Vec2 a, Vec2 b, Vec2 position,
                                 std::uint8_t first, std::uint8_t second) {
    // CONFIRMED 0x250284..0x2502c0: equality goes to the second candidate.
    if (first == second) return first;
    const float side = (b.z - a.z) * (position.x - b.x) -
                       (b.x - a.x) * (position.z - b.z);
    return side < 0.f ? first : second;
}

AiOutput tickAi(AiState& ai, const CarState& car, const WaypointPath& path,
                const AiParams& params) {
    AiOutput out{};

    // No path or already past the end: coast to a stop.
    if (ai.waypointIndex >= static_cast<int>(path.size())) {
        if (car.speed > params.speedMargin) out.brake = true;
        return out;
    }

    const Vec2 target = path[ai.waypointIndex];
    const Vec2 toTarget = target - ai.position;
    const float dist = length(toTarget);

    // Authored arrival policy: the original graph uses segment-side tests.
    if (dist < params.arrivalRadius) {
        ++ai.waypointIndex;
        if (ai.waypointIndex >= static_cast<int>(path.size())) {
            // Final waypoint reached: brake to stop.
            out.brake = true;
            return out;
        }
    }

    // Authored single-target heading (not original packed endpoint angles).
    const Vec2 currentTarget = path[ai.waypointIndex];
    const Vec2 toCurrent = currentTarget - ai.position;
    const float targetHeading = std::atan2(toCurrent.x, toCurrent.z);
    const float headingError = wrapAngle(targetHeading - ai.heading);
    const float distCurrent = length(toCurrent);

    // Steering: slew the internal command toward the sign of the heading error.
    // This authored smoothing prevents the command snapping between extremes;
    // it is not the original's angle-gated discrete steering.
    float desiredSteer = 0.f;
    if (headingError > 0.005f) desiredSteer = 1.f;
    else if (headingError < -0.005f) desiredSteer = -1.f;

    ai.steerAccum = slewToward(ai.steerAccum, desiredSteer, params.steerSlewPerTick);
    if (ai.steerAccum > 0.5f) out.steer = 1;
    else if (ai.steerAccum < -0.5f) out.steer = -1;

    if (params.routeTargetSpeed >= 0) {
        const auto input = trafficSpeedInput(
            car.speed, static_cast<std::uint8_t>(params.routeTargetSpeed),
            params.trafficSpeedCap);
        out.throttle = input == 1;
        out.brake = input == 2;
        return out;
    }

    // Authored default: speed cap lowers for sharp
    // heading deltas. The cap interpolates from straight to corner speed as
    // the absolute heading error grows to cornerErrorRad.
    const float errorRatio =
        std::clamp(std::fabs(headingError) / params.cornerErrorRad, 0.f, 1.f);
    const float speedCap =
        params.maxSpeedStraight +
        (params.maxSpeedCorner - params.maxSpeedStraight) * errorRatio;

    // Authored hysteresis, separate from the verified integer speed law above.
    bool wantThrottle = car.speed < speedCap - params.speedMargin;
    bool wantBrake = car.speed > speedCap + params.speedMargin;

    // Additional zone braking: if we are approaching the waypoint too fast,
    // brake so we can make the next segment cleanly.
    if (distCurrent < params.brakeLookahead &&
        car.speed > params.maxSpeedCorner + params.speedMargin) {
        wantBrake = true;
    }

    // Navigator fix: gas and brake must be mutually exclusive. When both are
    // requested (e.g. below the speed cap but inside the brake zone), slowing
    // wins and we pick brake.
    if (wantThrottle && wantBrake) wantThrottle = false;
    out.throttle = wantThrottle;
    out.brake = wantBrake;

    return out;
}

}  // namespace rt::native::gameplay
