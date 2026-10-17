#include "assets/title_screen.h"

#include <cstdint>
#include <fstream>
#include <limits>

namespace rt::native::assets {
namespace {
constexpr std::uint8_t kCnt = 0x10;
constexpr std::uint8_t kEnd = 0x70;
constexpr std::uint32_t kPsmCt32 = 0x00;
constexpr std::uint32_t kPsmT8 = 0x13;
constexpr std::size_t kUnitHeaderBytes = 112;

std::uint16_t u16(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint16_t>(b[o] | (static_cast<std::uint16_t>(b[o + 1]) << 8));
}
std::uint64_t u64(std::span<const std::uint8_t> b, std::size_t o) {
    std::uint64_t value = 0;
    for (int i = 7; i >= 0; --i) value = (value << 8) | b[o + static_cast<std::size_t>(i)];
    return value;
}
std::uint64_t bits(std::uint64_t v, unsigned first, unsigned count) {
    return (v >> first) & ((1ull << count) - 1ull);
}

struct Unit {
    std::uint32_t psm = 0, width = 0, height = 0;
    std::size_t pixels = 0, bytes = 0;
};

bool parseUnit(std::span<const std::uint8_t> data, std::size_t at, Unit& out, std::size_t& next) {
    if (at + kUnitHeaderBytes > data.size() || data[at + 3] != kCnt) return false;
    const std::uint64_t qwc = u16(data, at);
    if (qwc > (data.size() - at) / 16u - 1u) return false;
    const std::size_t payload = at + 16;
    const std::uint64_t imageTag = u64(data, payload + 80);
    const std::uint64_t nloop = imageTag & 0x7fffu;
    if (bits(imageTag, 58, 2) != 2 || qwc != 6u + nloop) return false;
    const std::uint64_t blit = u64(data, payload + 16);
    const std::uint64_t reg = u64(data, payload + 48);
    // The fixed A+D order is BITBLTBUF, TRXPOS, TRXREG, TRXDIR.
    if ((u64(data, payload + 24) & 0xffffffffu) != 0x50u ||
        (u64(data, payload + 40) & 0xffffffffu) != 0x51u ||
        (u64(data, payload + 56) & 0xffffffffu) != 0x52u ||
        (u64(data, payload + 72) & 0xffffffffu) != 0x53u ||
        (u64(data, payload + 64) & 3u) != 0) return false;
    if (nloop > std::numeric_limits<std::size_t>::max() / 16u) return false;
    out = {static_cast<std::uint32_t>(bits(blit, 56, 6)),
           static_cast<std::uint32_t>(bits(reg, 0, 12)),
           static_cast<std::uint32_t>(bits(reg, 32, 12)), payload + 96,
           static_cast<std::size_t>(nloop) * 16u};
    next = at + static_cast<std::size_t>(qwc + 1u) * 16u;
    return next <= data.size();
}
} // namespace

std::vector<std::filesystem::path> titleBackdropPaths(const std::filesystem::path& discRoot) {
    if (discRoot.empty()) return {};
    return {discRoot / "SYS" / "TITLE.GSL"};
}

bool decodeTitleBackdrop(std::span<const std::uint8_t> gsl, preview::PreviewTexture& out) {
    out = {};
    // The plate is TITLE.GSL's second T8 page.  The original title compositor
    // places its 440x220 transparent sprite over the 640x448 scene; baking
    // that exact shipped sprite into this runtime texture still leaves the
    // renderer with one full-window textured quad.
    constexpr std::size_t kPlateX = 50, kPlateY = 0;
    std::size_t at = 0;
    while (at + 16 <= gsl.size() && gsl[at + 3] != kEnd) {
        Unit texture, palette;
        std::size_t afterTexture = 0, afterPalette = 0;
        if (!parseUnit(gsl, at, texture, afterTexture)) return false;
        const bool isBackdrop = texture.psm == kPsmT8 && texture.width == 640 && texture.height == 448;
        const bool isPlate = texture.psm == kPsmT8 && texture.width == 440 && texture.height == 220;
        if (isBackdrop || isPlate) {
            if (!parseUnit(gsl, afterTexture, palette, afterPalette) ||
                palette.psm != kPsmCt32 || palette.width != 16 || palette.height != 16 ||
                texture.bytes != static_cast<std::size_t>(texture.width) * texture.height ||
                palette.bytes != 1024) return false;
            std::vector<std::uint8_t> decoded(static_cast<std::size_t>(texture.width) * texture.height * 4u);
            for (std::size_t i = 0; i < static_cast<std::size_t>(texture.width) * texture.height; ++i) {
                // CSM1 swaps palette-index bits 3 and 4 before the CT32
                // lookup (the same Extract.cpp rule used by image_converter).
                const std::uint32_t index = gsl[texture.pixels + i];
                const std::size_t c = static_cast<std::size_t>(
                    (index & ~0x18u) | ((index & 0x08u) << 1u) | ((index & 0x10u) >> 1u)) * 4u;
                // PSMCT32 data is little-endian RGBA; GS alpha 0x80 is opaque.
                decoded[i * 4 + 0] = gsl[palette.pixels + c + 0];
                decoded[i * 4 + 1] = gsl[palette.pixels + c + 1];
                decoded[i * 4 + 2] = gsl[palette.pixels + c + 2];
                const std::uint32_t alpha = gsl[palette.pixels + c + 3];
                decoded[i * 4 + 3] = static_cast<std::uint8_t>(alpha > 128 ? 255 : alpha * 255u / 128u);
            }
            if (isBackdrop) {
                out = {texture.width, texture.height, std::move(decoded)};
            } else if (out.width == 640 && out.height == 448) {
                for (std::size_t y = 0; y < texture.height; ++y) for (std::size_t x = 0; x < texture.width; ++x) {
                    const std::size_t src = (y * texture.width + x) * 4u;
                    const std::size_t dst = ((y + kPlateY) * out.width + x + kPlateX) * 4u;
                    const std::uint32_t a = decoded[src + 3];
                    if (!a) continue;
                    for (std::size_t c = 0; c < 3; ++c)
                        out.rgba[dst + c] = static_cast<std::uint8_t>((decoded[src + c] * a + out.rgba[dst + c] * (255u - a)) / 255u);
                    out.rgba[dst + 3] = 255;
                }
            }
        }
        at = afterTexture;
    }
    return out.width == 640 && out.height == 448 && out.rgba.size() == 640u * 448u * 4u;
}

std::optional<preview::PreviewTexture> loadTitleBackdrop(const std::filesystem::path& discRoot) {
    for (const auto& path : titleBackdropPaths(discRoot)) {
        std::ifstream input(path, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), {});
        preview::PreviewTexture decoded;
        if (decodeTitleBackdrop(bytes, decoded)) return decoded;
    }
    return std::nullopt;
}

} // namespace rt::native::assets
