#pragma once

#include "preview/gauge_needle.h"
#include "preview/preview.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace rt::native::preview {

// CPU-side RGBA8 target used by the preview compositor after its textured HUD
// passes have been read back. It deliberately borrows the caller's pixels.
struct RgbaSurface {
    int width = 0;
    int height = 0;
    std::span<std::uint8_t> rgba;
};

// Triangle-strip order: upper-left, upper-right, lower-left, lower-right.
struct TexturedQuad {
    std::array<float, 4> x{};
    std::array<float, 4> y{};
    std::array<float, 4> u{};
    std::array<float, 4> v{};
};

struct GaugeDialOrigin {
    float x = 0.f;
    float y = 0.f;
    float scaleX = 1.f;
    float scaleY = 1.f;
};

namespace detail {

inline bool validSurface(const RgbaSurface& surface) {
    return surface.width > 0 && surface.height > 0 &&
           surface.rgba.size() >= static_cast<std::size_t>(surface.width) * surface.height * 4;
}

inline bool validTexture(const PreviewTexture& texture) {
    return texture.width > 0 && texture.height > 0 &&
           texture.rgba.size() >= static_cast<std::size_t>(texture.width) * texture.height * 4;
}

inline bool contains(const TexturedQuad& q, int a, int b, int c, float px, float py,
                     float& wa, float& wb, float& wc) {
    const float abx = q.x[b] - q.x[a], aby = q.y[b] - q.y[a];
    const float acx = q.x[c] - q.x[a], acy = q.y[c] - q.y[a];
    const float apx = px - q.x[a], apy = py - q.y[a];
    const float twiceArea = abx * acy - aby * acx;
    if (std::fabs(twiceArea) < 1e-6f) return false;
    wb = (apx * acy - apy * acx) / twiceArea;
    wc = (abx * apy - aby * apx) / twiceArea;
    wa = 1.f - wb - wc;
    constexpr float kEdgeTolerance = -1e-5f;
    return wa >= kEdgeTolerance && wb >= kEdgeTolerance && wc >= kEdgeTolerance;
}

inline std::array<float, 4> sampleBilinear(const PreviewTexture& texture, float u, float v) {
    const float tx = std::clamp(u, 0.f, 1.f) * static_cast<float>(texture.width - 1);
    const float ty = std::clamp(v, 0.f, 1.f) * static_cast<float>(texture.height - 1);
    const auto x0 = static_cast<std::uint32_t>(tx);
    const auto y0 = static_cast<std::uint32_t>(ty);
    const auto x1 = std::min(x0 + 1, texture.width - 1);
    const auto y1 = std::min(y0 + 1, texture.height - 1);
    const float fx = tx - static_cast<float>(x0), fy = ty - static_cast<float>(y0);
    const auto at = [&texture](std::uint32_t x, std::uint32_t y, int channel) {
        return texture.rgba[(static_cast<std::size_t>(y) * texture.width + x) * 4 + channel] / 255.f;
    };
    std::array<float, 4> result{};
    for (int channel = 0; channel < 4; ++channel) {
        const float top = at(x0, y0, channel) + (at(x1, y0, channel) - at(x0, y0, channel)) * fx;
        const float bottom = at(x0, y1, channel) + (at(x1, y1, channel) - at(x0, y1, channel)) * fx;
        result[channel] = top + (bottom - top) * fy;
    }
    return result;
}

}  // namespace detail

// Rasterizes a convex quad into `surface`, sampling normalized UVs bilinearly
// and compositing source-over. Returns false without writes for malformed input.
inline bool drawTexturedQuad(RgbaSurface surface, const TexturedQuad& quad, const PreviewTexture& texture) {
    if (!detail::validSurface(surface) || !detail::validTexture(texture)) return false;
    for (int i = 0; i < 4; ++i)
        if (!std::isfinite(quad.x[i]) || !std::isfinite(quad.y[i]) ||
            !std::isfinite(quad.u[i]) || !std::isfinite(quad.v[i])) return false;

    const auto [minXIt, maxXIt] = std::minmax_element(quad.x.begin(), quad.x.end());
    const auto [minYIt, maxYIt] = std::minmax_element(quad.y.begin(), quad.y.end());
    const int minX = std::max(0, static_cast<int>(std::floor(*minXIt)));
    const int maxX = std::min(surface.width - 1, static_cast<int>(std::ceil(*maxXIt)));
    const int minY = std::max(0, static_cast<int>(std::floor(*minYIt)));
    const int maxY = std::min(surface.height - 1, static_cast<int>(std::ceil(*maxYIt)));
    if (minX > maxX || minY > maxY) return false;

    bool wrote = false;
    for (int py = minY; py <= maxY; ++py) {
        for (int px = minX; px <= maxX; ++px) {
            const float sampleX = static_cast<float>(px) + .5f;
            const float sampleY = static_cast<float>(py) + .5f;
            float a = 0.f, b = 0.f, c = 0.f;
            int i0 = 0, i1 = 1, i2 = 2;
            if (!detail::contains(quad, i0, i1, i2, sampleX, sampleY, a, b, c)) {
                i0 = 2; i1 = 1; i2 = 3;
                if (!detail::contains(quad, i0, i1, i2, sampleX, sampleY, a, b, c)) continue;
            }
            const float u = a * quad.u[i0] + b * quad.u[i1] + c * quad.u[i2];
            const float v = a * quad.v[i0] + b * quad.v[i1] + c * quad.v[i2];
            const auto source = detail::sampleBilinear(texture, u, v);
            const std::size_t at = (static_cast<std::size_t>(py) * surface.width + px) * 4;
            const float alpha = source[3];
            for (int channel = 0; channel < 3; ++channel) {
                const float dst = surface.rgba[at + channel] / 255.f;
                surface.rgba[at + channel] = static_cast<std::uint8_t>(
                    std::clamp(std::lround((source[channel] * alpha + dst * (1.f - alpha)) * 255.f), 0l, 255l));
            }
            const float dstAlpha = surface.rgba[at + 3] / 255.f;
            surface.rgba[at + 3] = static_cast<std::uint8_t>(
                std::clamp(std::lround((alpha + dstAlpha * (1.f - alpha)) * 255.f), 0l, 255l));
            wrote = wrote || alpha > 0.f;
        }
    }
    return wrote;
}

// Draws the single-player tachometer needle described by gauge_needle.h.
// The caller provides the decoded 128x120 METER0 unit-2 CSA-12 page.
inline bool drawGaugeNeedle(RgbaSurface surface, float rpm, const PreviewTexture& texture,
                            GaugeDialOrigin dialOrigin) {
    if (!std::isfinite(rpm) || !std::isfinite(dialOrigin.x) || !std::isfinite(dialOrigin.y) ||
        !std::isfinite(dialOrigin.scaleX) || !std::isfinite(dialOrigin.scaleY) ||
        dialOrigin.scaleX <= 0.f || dialOrigin.scaleY <= 0.f) return false;
    const float clampedRpm = std::clamp(rpm, 0.f, 10000.f);
    constexpr float kPi = 3.14159265358979323846f;
    const float theta = kPi * (clampedRpm / 10000.f - .7f);
    const auto needle = gauge_needle::needleQuad(theta, 0, 0);
    TexturedQuad quad{};
    for (int i = 0; i < 4; ++i) {
        quad.x[i] = dialOrigin.x + needle.x[i] * dialOrigin.scaleX;
        quad.y[i] = dialOrigin.y + needle.y[i] * dialOrigin.scaleY;
        quad.u[i] = needle.u[i];
        quad.v[i] = needle.v[i];
    }
    return drawTexturedQuad(surface, quad, texture);
}

}  // namespace rt::native::preview
