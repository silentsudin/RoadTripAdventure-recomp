#pragma once

#include "gameplay/daynight.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace rt::native::preview {

// Preview's persistent fallback is retained for callers that provide a
// default-constructed FrameSnapshot rather than the GameLoop's authored tick.
inline constexpr std::array<float, 4> kPreviewFallbackClear{.08f, .09f, .12f, 1.f};

struct SceneLighting {
    std::array<float, 4> clear = kPreviewFallbackClear;
    // RGB plus render-space fog distance. A huge distance disables fog for
    // uninitialized snapshots, preserving the legacy preview appearance.
    std::array<float, 4> fog{0.f, 0.f, 0.f, 1e30f};
    float fogFarRender = 1e30f;
    bool enabled = false;
};

inline std::array<float, 4> argbToRgba(std::uint32_t argb) {
    return {
        static_cast<float>((argb >> 16) & 0xffu) / 255.f,
        static_cast<float>((argb >> 8) & 0xffu) / 255.f,
        static_cast<float>(argb & 0xffu) / 255.f,
        1.f,
    };
}

// updateTimeOfDayLight always writes the final matrix element as 128. The
// default DayNightOutput leaves it zero, which gives preview a deterministic
// initialization sentinel without changing the simulation data model.
inline bool hasSceneLighting(const gameplay::DayNightOutput& lighting) {
    return lighting.fogFar > 0.f && lighting.lightMatrix[15] == 128.f;
}

// Matches the shader's smooth fog blend in game-space distances. Render
// positions are game positions scaled by 0.1, so the dimensionless factor is
// the same in either space.
inline float fogFactor(float distance, float fogFar) {
    const float edge0 = fogFar * .5f;
    const float edge1 = fogFar;
    if (edge1 <= edge0) return distance >= edge1 ? 1.f : 0.f;
    const float t = std::clamp((distance - edge0) / (edge1 - edge0), 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

inline SceneLighting selectSceneLighting(const gameplay::DayNightOutput& lighting) {
    SceneLighting selected;
    if (!hasSceneLighting(lighting)) return selected;
    selected.enabled = true;
    selected.clear = argbToRgba(lighting.skyColour);
    selected.fog = argbToRgba(lighting.fogColourRGBA);
    selected.fogFarRender = lighting.fogFar * .1f;
    selected.fog[3] = selected.fogFarRender;
    return selected;
}

}  // namespace rt::native::preview
