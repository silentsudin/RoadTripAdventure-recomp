#include "assets/hud_art.h"

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <limits>

namespace rt::native::assets {
namespace {

constexpr std::uint8_t kCnt = 0x10;
constexpr std::uint8_t kEnd = 0x70;
constexpr std::uint32_t kPsmCt32 = 0x00;
constexpr std::uint32_t kPsmCt16 = 0x02;
constexpr std::uint32_t kPsmT8 = 0x13;
constexpr std::uint32_t kPsmT4 = 0x14;
constexpr std::size_t kUnitHeaderBytes = 112;

std::uint16_t u16(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint16_t>(b[o] | (static_cast<std::uint16_t>(b[o + 1]) << 8));
}
std::uint32_t u32(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint32_t>(b[o] | (static_cast<std::uint32_t>(b[o + 1]) << 8) |
                                      (static_cast<std::uint32_t>(b[o + 2]) << 16) |
                                      (static_cast<std::uint32_t>(b[o + 3]) << 24));
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

std::uint32_t csm1Index(std::uint8_t idx) {
    return static_cast<std::uint32_t>((idx & ~0x18u) | ((idx & 0x08u) << 1u) | ((idx & 0x10u) >> 1u));
}

bool decodePsmt8Ct32(std::span<const std::uint8_t> gsl, const Unit& tex, const Unit& pal, preview::PreviewTexture& out) {
    if (tex.bytes != static_cast<std::size_t>(tex.width) * tex.height || pal.bytes != 1024) return false;
    std::vector<std::uint8_t> decoded(static_cast<std::size_t>(tex.width) * tex.height * 4u);
    for (std::size_t i = 0; i < static_cast<std::size_t>(tex.width) * tex.height; ++i) {
        const std::uint32_t idx = gsl[tex.pixels + i];
        const std::size_t c = static_cast<std::size_t>(csm1Index(static_cast<std::uint8_t>(idx))) * 4u;
        decoded[i * 4 + 0] = gsl[pal.pixels + c + 0];
        decoded[i * 4 + 1] = gsl[pal.pixels + c + 1];
        decoded[i * 4 + 2] = gsl[pal.pixels + c + 2];
        const std::uint32_t alpha = gsl[pal.pixels + c + 3];
        decoded[i * 4 + 3] = static_cast<std::uint8_t>(alpha > 128 ? 255 : alpha * 255u / 128u);
    }
    out = {tex.width, tex.height, std::move(decoded)};
    return true;
}

bool decodePsmt8Ct16(std::span<const std::uint8_t> gsl, const Unit& tex, const Unit& pal, preview::PreviewTexture& out) {
    if (tex.bytes != static_cast<std::size_t>(tex.width) * tex.height || pal.bytes != 512) return false;
    std::vector<std::uint8_t> decoded(static_cast<std::size_t>(tex.width) * tex.height * 4u);
    for (std::size_t i = 0; i < static_cast<std::size_t>(tex.width) * tex.height; ++i) {
        const std::uint32_t idx = gsl[tex.pixels + i];
        const std::size_t c = static_cast<std::size_t>(csm1Index(static_cast<std::uint8_t>(idx))) * 2u;
        const std::uint16_t p16 = static_cast<std::uint16_t>(gsl[pal.pixels + c] | (static_cast<std::uint16_t>(gsl[pal.pixels + c + 1]) << 8));
        decoded[i * 4 + 0] = static_cast<std::uint8_t>((p16 & 0x1Fu) << 3);
        decoded[i * 4 + 1] = static_cast<std::uint8_t>(((p16 >> 5) & 0x1Fu) << 3);
        decoded[i * 4 + 2] = static_cast<std::uint8_t>(((p16 >> 10) & 0x1Fu) << 3);
        decoded[i * 4 + 3] = (p16 == 0 ? 0 : 255);
    }
    out = {tex.width, tex.height, std::move(decoded)};
    return true;
}

bool decodePsmt4Ct16(std::span<const std::uint8_t> gsl, const Unit& tex, const Unit& pal, preview::PreviewTexture& out) {
    const std::size_t expected = (static_cast<std::size_t>(tex.width) * tex.height + 1u) / 2u;
    if (tex.bytes != expected || pal.bytes != 512) return false;
    std::vector<std::uint8_t> decoded(static_cast<std::size_t>(tex.width) * tex.height * 4u);
    for (std::size_t i = 0; i < static_cast<std::size_t>(tex.width) * tex.height; ++i) {
        const std::uint8_t byte = gsl[tex.pixels + (i / 2)];
        const std::uint32_t idx = (byte >> ((i & 1u) * 4u)) & 0xFu;
        const std::uint32_t phys = (idx & 7u) | ((idx & 8u) << 1);
        const std::size_t c = phys * 2u;
        const std::uint16_t p16 = static_cast<std::uint16_t>(gsl[pal.pixels + c] | (static_cast<std::uint16_t>(gsl[pal.pixels + c + 1]) << 8));
        decoded[i * 4 + 0] = static_cast<std::uint8_t>((p16 & 0x1Fu) << 3);
        decoded[i * 4 + 1] = static_cast<std::uint8_t>(((p16 >> 5) & 0x1Fu) << 3);
        decoded[i * 4 + 2] = static_cast<std::uint8_t>(((p16 >> 10) & 0x1Fu) << 3);
        decoded[i * 4 + 3] = (p16 == 0 ? 0 : 255);
    }
    out = {tex.width, tex.height, std::move(decoded)};
    return true;
}

std::filesystem::path packagedRoot() {
    if (const char* a = std::getenv("RT_ASSET_DIR"); a && *a) return std::filesystem::path(a);
    if (const char* d = std::getenv("RT_DATA_DIR"); d && *d) return std::filesystem::path(d) / "assets";
    if (!std::getenv("RT_DISC")) {
        if (const char* home = std::getenv("HOME"))
            return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp" / "assets";
    }
    return {};
}

} // namespace

std::vector<std::filesystem::path> meterDialPaths(const std::filesystem::path& discRoot) {
    std::vector<std::filesystem::path> out;
    // Two distinct gauge pages ship in the METER family (pixel-scanned
    // 2026-10-17, build/scratch/hud-art + sim_dial_v2_vs_golden.png):
    // METER0 unit 0 = baked dial ring (digits 0-10 around the rim + mph),
    // METER10 unit 0 = dynamic parts incl. the §1.2 speed-digit row v=102.
    if (!discRoot.empty()) {
        out.push_back(discRoot / "ITEM" / "METER0.GSL");
        out.push_back(discRoot / "ITEM" / "METER10.GSL");
    }
    const auto root = packagedRoot();
    if (!root.empty()) {
        for (const char* leaf : {"METER0.GSL", "METER10.GSL"}) {
            out.push_back(root / "item" / leaf);
            out.push_back(root / "sys" / leaf);
            out.push_back(root / "ITEM" / leaf);
        }
    }
    return out;
}

std::vector<std::filesystem::path> scoreAtlasPaths(const std::filesystem::path& discRoot) {
    std::vector<std::filesystem::path> out;
    if (!discRoot.empty()) out.push_back(discRoot / "SYS" / "SCORE.GSL");
    const auto root = packagedRoot();
    if (!root.empty()) {
        out.push_back(root / "sys" / "SCORE.GSL");
        out.push_back(root / "SYS" / "SCORE.GSL");
    }
    return out;
}

// The runtime gauge page must carry the speed-digit row: ten 20x26 glyph
// cells at (d*20,102) (hud-layout.md §1.2 CONFIRMED).  Pixel-scanning the
// decoded corpus (2026-10-17, build/scratch/hud-art/{meter10_verify,
// contact_v9_check}.png): METER10.GSL unit 0 matches the documented layout
// exactly — dial arc at origin, mph label, gear badge, digits at v=102 —
// while METER0.GSL unit 0 bakes digits along the page top with a
// full-height dial (its v=102 row is empty ground), so it is NOT the page
// the §1.2 records sample.  Require the digit row to validate a candidate.
bool hasSpeedDigitRow(const preview::PreviewTexture& t) {
    if (t.width != 256 || t.height != 128 || t.rgba.size() < static_cast<std::size_t>(t.width) * t.height * 4)
        return false;
    int good = 0;
    constexpr int kW = 20, kH = 26;
    for (int d = 0; d < 10; ++d) {
        double sum = 0, sum2 = 0;
        for (int y = 0; y < kH; ++y) {
            for (int x = 0; x < kW; ++x) {
                const std::uint8_t* p = &t.rgba[(static_cast<std::size_t>(102 + y) * 256 + d * 20 + x) * 4];
                const double lum = .3 * p[0] + .6 * p[1] + .1 * p[2];
                sum += lum;
                sum2 += lum * lum;
            }
        }
        const double mean = sum / (kW * kH);
        const double var = sum2 / (kW * kH) - mean * mean;
        if (var > 30.0 * 30.0) ++good;  // PIL scan threshold (stddev ~35) w/ margin
    }
    return good >= 8;
}

bool decodeMeterDial(std::span<const std::uint8_t> gsl, preview::PreviewTexture& out) {
    out = {};
    std::size_t at = 0;
    while (at + 16 <= gsl.size() && gsl[at + 3] != kEnd) {
        Unit texture, palette;
        std::size_t after = 0;
        if (!parseUnit(gsl, at, texture, after)) return false;
        const bool isDial = (texture.psm == kPsmT8 || texture.psm == kPsmT4) &&
                            texture.width == 256 && texture.height == 128;
        if (isDial) {
            if (!parseUnit(gsl, after, palette, after) ||
                palette.width != 16 || palette.height != 16) return false;
            bool ok = false;
            if (texture.psm == kPsmT8 && palette.psm == kPsmCt32)
                ok = decodePsmt8Ct32(gsl, texture, palette, out);
            else if (texture.psm == kPsmT8 && palette.psm == kPsmCt16)
                ok = decodePsmt8Ct16(gsl, texture, palette, out);
            else if (texture.psm == kPsmT4 && palette.psm == kPsmCt16)
                ok = decodePsmt4Ct16(gsl, texture, palette, out);
            if (ok && out.width == 256 && out.height == 128) return true;
            out = {};
        }
        at = after;
    }
    return false;
}

bool decodeRaceAtlas(std::span<const std::uint8_t> gsl, preview::PreviewTexture& out) {
    out = {};
    std::size_t at = 0;
    while (at + 16 <= gsl.size() && gsl[at + 3] != kEnd) {
        Unit texture, palette;
        std::size_t after = 0;
        if (!parseUnit(gsl, at, texture, after)) return false;
        const bool isAtlas = (texture.psm == kPsmT4) && texture.width == 256 && texture.height == 256;
        if (isAtlas) {
            if (!parseUnit(gsl, after, palette, after) ||
                palette.psm != kPsmCt16 || palette.width != 16 || palette.height != 16 ||
                texture.bytes != ((static_cast<std::size_t>(texture.width) * texture.height + 1u) / 2u) ||
                palette.bytes != 512) return false;
            if (decodePsmt4Ct16(gsl, texture, palette, out) &&
                out.width == 256 && out.height == 256) return true;
            out = {};
        }
        at = after;
    }
    return false;
}

HudArt loadHudArt(const std::filesystem::path& discRoot) {
    HudArt art;
    // Base dial ring + dynamic digit page ride different METER containers;
    // scan candidates once, filling whichever slot each page validates for.
    for (const auto& path : meterDialPaths(discRoot)) {
        std::ifstream input(path, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        preview::PreviewTexture tex;
        if (!art.speedDial && decodeMeterDial(bytes, tex)) art.speedDial = std::move(tex);
        preview::PreviewTexture digitsTex;
        if (!art.speedDigits && decodeMeterDial(bytes, digitsTex) && hasSpeedDigitRow(digitsTex))
            art.speedDigits = std::move(digitsTex);
        if (art.speedDial && art.speedDigits) break;
    }
    for (const auto& path : scoreAtlasPaths(discRoot)) {
        std::ifstream input(path, std::ios::binary);
        if (!input) continue;
        const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        preview::PreviewTexture tex;
        if (decodeRaceAtlas(bytes, tex)) {
            art.raceAtlas = std::move(tex);
            break;
        }
    }
    return art;
}

} // namespace rt::native::assets
