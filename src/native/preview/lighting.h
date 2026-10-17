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

// The visible daytime dome in the shipped game is a medium blue — the ROM
// skyColour field in updateTimeOfDayLight is the SUN light colour (0xf05800
// orange for day, daynight.cpp:189), not the sky background. The golden
// frames' dome samples at (43,154,216) in build/scratch/oracle-golden/
// title.png near (100,10). AUTHORED blend: pull the computed clear colour
// toward the measured blue by the (clamped) day weight, leaving dusk/night
// computed colours untouched at zero weight.
inline constexpr std::array<float, 4> kOracleDaySkyBlue{43.f / 255.f, 154.f / 255.f, 216.f / 255.f, 1.f};

inline std::array<float, 4> blendClearToOracleSky(std::array<float, 4> clear, float dayWeight) {
    const float t = std::clamp(dayWeight, 0.f, 1.f);
    for (int i = 0; i < 3; ++i)
        clear[i] = clear[i] * (1.f - t) + kOracleDaySkyBlue[i] * t;
    return clear;
}

inline SceneLighting selectSceneLighting(const gameplay::DayNightOutput& lighting) {
    SceneLighting selected;
    if (!hasSceneLighting(lighting)) return selected;
    selected.enabled = true;
    selected.clear = blendClearToOracleSky(argbToRgba(lighting.skyColour), lighting.dayWeight);
    selected.fog = argbToRgba(lighting.fogColourRGBA);
    selected.fogFarRender = lighting.fogFar * .1f;
    selected.fog[3] = selected.fogFarRender;
    return selected;
}

// ---- Sky gradient calibration (sky-ground-calibration.md §3) ------------
// Vertical sky stops measured on the oracle goldens' open-sky columns
// (§1.3 town x=320, §1.4 race x=1240): {uiY0, uiY1, RGBA} in 480-line UI
// space, y down from the viewport top. The fill interpolates linearly
// between consecutive stops and the final stop holds its colour to uiY1
// (the measured horizon row). These are DAY measurements; the day-weight
// gate of blendClearToOracleSky applies (see previewRenderScene).
struct SkyStop {
    float uiY0, uiY1;
    std::array<float, 4> rgba;
};

// Horizon uiY: town transition 80->81 px / 0.93125 (§1.3), race 80->81 (§1.4).
inline constexpr float kSkyHorizonUiY = 87.0f;

// residual 5.53/255 (§3 town)
inline constexpr SkyStop kTownSkyStops[] = {
    {  0.0f, 73.0f, {  12.f / 255.f, 121.f / 255.f, 253.f / 255.f, 1.f } },
    { 73.0f, 85.9f, {  16.f / 255.f, 124.f / 255.f, 253.f / 255.f, 1.f } },
    { 85.9f, 87.0f, {   1.f / 255.f, 113.f / 255.f, 248.f / 255.f, 1.f } },
};

// residual 3.60/255 (§3 race)
inline constexpr SkyStop kRaceSkyStops[] = {
    {  0.0f, 56.9f, {  34.f / 255.f, 133.f / 255.f, 253.f / 255.f, 1.f } },
    { 56.9f, 74.1f, {  42.f / 255.f, 137.f / 255.f, 253.f / 255.f, 1.f } },
    { 74.1f, 85.9f, {  71.f / 255.f, 153.f / 255.f, 253.f / 255.f, 1.f } },
    { 85.9f, 87.0f, { 115.f / 255.f, 176.f / 255.f, 249.f / 255.f, 1.f } },
};

}  // namespace rt::native::preview
