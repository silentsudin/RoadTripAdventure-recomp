#include "gameplay/motion.h"

#include <cmath>

namespace rt::native::gameplay {
namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.f * kPi;
constexpr float kDegreesToRadians = kPi / 180.f;

float wrapAngle(float radians) {
    while (radians > kPi) radians -= kTwoPi;
    while (radians < -kPi) radians += kTwoPi;
    return radians;
}

Vec3 rotateYawPitch(Vec3 p, float yaw, float pitch) {
    const float sy = std::sin(yaw), cy = std::cos(yaw);
    const Vec3 yawed{p.x * cy + p.z * sy, p.y, -p.x * sy + p.z * cy};
    const float sp = std::sin(pitch), cp = std::cos(pitch);
    return {yawed.x, yawed.y * cp - yawed.z * sp, yawed.y * sp + yawed.z * cp};
}

Vec3 transformPoint(Vec3 position, float yaw, float pitch, Vec3 local) {
    return position + rotateYawPitch(local, yaw, pitch);
}

ScreenMotion makeScreenMotion(Vec2 previous, Vec2 current, std::uint32_t flags) {
    ScreenMotion result;
    result.previousNdc = previous;
    result.currentNdc = current;
    result.ndc = current - previous;
    result.uv = result.ndc * .5f;
    result.flags = flags;
    return result;
}
}  // namespace

MotionVector worldMotion(const EntityMotion& entity, Vec3 localPoint, MotionSettings settings) {
    MotionVector result;
    if (!entity.hasPrevious) {
        result.flags = MotionNoPrevious;
        return result;
    }

    const Vec3 translation = entity.curPos - entity.prevPos;
    if (length(translation) > settings.teleportDistance) {
        result.flags = MotionTeleport;
        return result;
    }

    result.yawDeltaRadians = wrapAngle(entity.curYaw - entity.prevYaw);
    const float pitchDeltaRadians = wrapAngle(entity.curPitch - entity.prevPitch);
    const float rotationMagnitude = std::sqrt(result.yawDeltaRadians * result.yawDeltaRadians +
                                              pitchDeltaRadians * pitchDeltaRadians);
    if (rotationMagnitude > settings.spinTranslationOnlyDegrees * kDegreesToRadians) {
        result.world = translation;
        result.flags = MotionTranslationOnly;
        return result;
    }
    result.world = transformPoint(entity.curPos, entity.curYaw, entity.curPitch, localPoint) -
                   transformPoint(entity.prevPos, entity.prevYaw, entity.prevPitch, localPoint);
    return result;
}

bool projectToNdc(const MotionMatrix& m, Vec3 point, Vec2& out) {
    const float x = m[0] * point.x + m[4] * point.y + m[8] * point.z + m[12];
    const float y = m[1] * point.x + m[5] * point.y + m[9] * point.z + m[13];
    const float w = m[3] * point.x + m[7] * point.y + m[11] * point.z + m[15];
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(w) || std::fabs(w) < 1e-8f)
        return false;
    out = {x / w, y / w};
    return std::isfinite(out.x) && std::isfinite(out.z);
}

ScreenMotion projectEntityMotion(const EntityMotion& entity, Vec3 localPoint,
                                 const MotionMatrix& previousViewProjection,
                                 const MotionMatrix& currentViewProjection,
                                 MotionSettings settings) {
    const MotionVector world = worldMotion(entity, localPoint, settings);
    if (!world.valid()) return makeScreenMotion({}, {}, world.flags);

    const bool translationOnly = (world.flags & MotionTranslationOnly) != 0;
    const Vec3 previous = translationOnly ? entity.prevPos + localPoint
                                          : transformPoint(entity.prevPos, entity.prevYaw,
                                                           entity.prevPitch, localPoint);
    const Vec3 current = translationOnly ? entity.curPos + localPoint
                                         : transformPoint(entity.curPos, entity.curYaw,
                                                          entity.curPitch, localPoint);
    Vec2 previousNdc, currentNdc;
    if (!projectToNdc(previousViewProjection, previous, previousNdc) ||
        !projectToNdc(currentViewProjection, current, currentNdc))
        return makeScreenMotion({}, {}, world.flags | MotionProjectionInvalid);
    return makeScreenMotion(previousNdc, currentNdc, world.flags);
}

ScreenMotion cameraOnlyMotion(Vec3 worldPoint, const MotionMatrix& previousViewProjection,
                              const MotionMatrix& currentViewProjection) {
    Vec2 previousNdc, currentNdc;
    if (!projectToNdc(previousViewProjection, worldPoint, previousNdc) ||
        !projectToNdc(currentViewProjection, worldPoint, currentNdc))
        return makeScreenMotion({}, {}, MotionProjectionInvalid);
    return makeScreenMotion(previousNdc, currentNdc, MotionNone);
}

}  // namespace rt::native::gameplay
