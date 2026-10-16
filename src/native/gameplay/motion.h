// Render motion-vector math for the authored native renderer.
// Pure C++: it neither depends on Vulkan nor submits GPU work.
#pragma once

#include "gameplay/vec.h"

#include <array>
#include <cstdint>

namespace rt::native::gameplay {

// The position/yaw fields correspond directly to RenderPose's car placement
// snapshots in interp.h.  Pitch is available for renderables that need it;
// current car placement is yaw-only.
struct EntityMotion {
    Vec3 prevPos{};
    Vec3 curPos{};
    float prevYaw = 0.f;
    float curYaw = 0.f;
    float prevPitch = 0.f;
    float curPitch = 0.f;
    bool hasPrevious = true;
};

struct MotionSettings {
    // Render/game units; world-geography.md defines the native terrain/nav
    // unit. A larger per-logic-step displacement is a discontinuity, not a
    // velocity suitable for temporal reprojection.
    float teleportDistance = 30.f;
    // Large per-step orientation changes produce unreliable per-vertex motion
    // on coarse meshes, so preserve only entity translation for that draw.
    float spinTranslationOnlyDegrees = 45.f;
};

enum MotionFlags : std::uint32_t {
    MotionNone = 0,
    MotionNoPrevious = 1u << 0,
    MotionTeleport = 1u << 1,
    MotionTranslationOnly = 1u << 2,
    MotionProjectionInvalid = 1u << 3,
};

struct MotionVector {
    Vec3 world{};  // current world point - previous world point
    float yawDeltaRadians = 0.f;  // shortest signed arc
    std::uint32_t flags = MotionNone;
    [[nodiscard]] bool valid() const {
        return (flags & (MotionNoPrevious | MotionTeleport | MotionProjectionInvalid)) == 0;
    }
};

// Vulkan/GLSL column-major matrix layout, matching FrameSnapshot::view/proj
// uploads in preview.cpp. It is intentionally just data so this module stays
// host-only.
using MotionMatrix = std::array<float, 16>;

struct ScreenMotion {
    // Vec2 is the native ground/screen pair type: x is horizontal and z is
    // the second component (vertical here, not a world-axis claim).
    Vec2 previousNdc{};
    Vec2 currentNdc{};
    Vec2 ndc{};  // currentNdc - previousNdc
    Vec2 uv{};   // ndc * 0.5, suitable for a texture UV reprojection delta
    std::uint32_t flags = MotionNone;
    [[nodiscard]] bool valid() const {
        return (flags & (MotionNoPrevious | MotionTeleport | MotionProjectionInvalid)) == 0;
    }
};

// Transforms a local mesh point by the entity's yaw then pitch and translates
// it. For a root-only vector use the default local point {0,0,0}.
[[nodiscard]] MotionVector worldMotion(const EntityMotion& entity, Vec3 localPoint = {},
                                       MotionSettings settings = {});

// Projects one world point to normalized device coordinates. `out` is not
// modified on a non-projectable point (w == 0 or non-finite values).
[[nodiscard]] bool projectToNdc(const MotionMatrix& viewProjection, Vec3 worldPoint, Vec2& out);

// Per-entity vector including object motion and camera motion. The two
// matrices are the previous and current jittered view-projection matrices.
[[nodiscard]] ScreenMotion projectEntityMotion(const EntityMotion& entity, Vec3 localPoint,
                                               const MotionMatrix& previousViewProjection,
                                               const MotionMatrix& currentViewProjection,
                                               MotionSettings settings = {});

// Camera-only vector: project the same static world point through successive
// view-projection matrices. Terrain can use this where it has no entity pose.
[[nodiscard]] ScreenMotion cameraOnlyMotion(Vec3 worldPoint,
                                            const MotionMatrix& previousViewProjection,
                                            const MotionMatrix& currentViewProjection);

// GPU wave reference (column-major GLSL, with the same validity decisions
// supplied per draw by CPU):
//
//   vec4 prevClip = uPrevViewProjection * (uPrevModel * vec4(inPosition, 1.0));
//   vec4 curClip  = uCurViewProjection  * (uCurModel  * vec4(inPosition, 1.0));
//   vec2 prevNdc = prevClip.xy / prevClip.w;
//   vec2 curNdc  = curClip.xy / curClip.w;
//   outMotionUv = 0.5 * (curNdc - prevNdc); // current minus previous
//
// On MotionNoPrevious, MotionTeleport, or invalid clip w, output zero and
// mark the pixel/history invalid rather than attempting temporal reprojection.

}  // namespace rt::native::gameplay
