#include "preview/crt_filter.h"

#include <array>

namespace rt::native::preview {
namespace {

// Every third row is a scanline gap; every third column carries one dominant
// channel of the aperture mask. Values are 8-bit so the pass is integer-only
// (deterministic across backends).
constexpr float kScanlineGain = 0.72f;
constexpr std::array<std::array<float, 3>, 3> kWireGain{{
    {1.00f, 0.86f, 0.74f},
    {0.86f, 1.00f, 0.86f},
    {0.74f, 0.86f, 1.00f},
}};

std::uint8_t gainByte(float value) {
    const int byte = static_cast<int>(value * 255.f + 0.5f);
    return static_cast<std::uint8_t>(byte < 0 ? 0 : byte > 255 ? 255 : byte);
}

}  // namespace

void applyCrtFilter(std::vector<std::uint8_t>& rgba, int width, int height) {
    const std::size_t need = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u;
    if (width <= 0 || height <= 0 || rgba.size() < need) return;

    // The app renders one size for a whole run; rebuild the table only when it
    // changes (one thread owns the render loop).
    static int cachedWidth = -1;
    static int cachedHeight = -1;
    static std::vector<std::uint8_t> gain;
    if (width != cachedWidth || height != cachedHeight) {
        cachedWidth = width;
        cachedHeight = height;
        gain.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 3u);
        for (int y = 0; y < height; ++y) {
            const float scan = (y % 3 == 2) ? kScanlineGain : 1.0f;
            for (int x = 0; x < width; ++x) {
                const auto& wire = kWireGain[static_cast<std::size_t>(x % 3)];
                std::uint8_t* g = gain.data() + (static_cast<std::size_t>(y) * width + x) * 3;
                g[0] = gainByte(wire[0] * scan);
                g[1] = gainByte(wire[1] * scan);
                g[2] = gainByte(wire[2] * scan);
            }
        }
    }

    for (int y = 0; y < height; ++y) {
        const std::uint8_t* g = gain.data() + static_cast<std::size_t>(y) * width * 3;
        std::uint8_t* p = rgba.data() + static_cast<std::size_t>(y) * width * 4;
        for (int x = 0; x < width; ++x, p += 4, g += 3) {
            p[0] = static_cast<std::uint8_t>((p[0] * g[0] + 127u) / 255u);
            p[1] = static_cast<std::uint8_t>((p[1] * g[1] + 127u) / 255u);
            p[2] = static_cast<std::uint8_t>((p[2] * g[2] + 127u) / 255u);
        }
    }
}

}  // namespace rt::native::preview
