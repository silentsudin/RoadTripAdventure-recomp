#include "../../src/native/assets/hud_art.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <vector>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
    if (!condition) { std::fprintf(stderr, "FAIL %s\n", name); ++failures; }
}

std::filesystem::path disc() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

std::uint64_t hash(const rt::native::preview::PreviewTexture& texture) {
    std::uint64_t value = 14695981039346656037ull;
    for (std::uint8_t byte : texture.rgba) { value ^= byte; value *= 1099511628211ull; }
    return value;
}

std::size_t opaque(const rt::native::preview::PreviewTexture& tex, int x, int y, int w, int h) {
    std::size_t count = 0;
    for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx)
            count += tex.rgba[(static_cast<std::size_t>(yy) * tex.width + xx) * 4 + 3] != 0;
    return count;
}

std::vector<std::uint8_t> readBytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

std::uint16_t u16(const std::vector<std::uint8_t>& bytes, std::size_t at) {
    return static_cast<std::uint16_t>(bytes[at] | (static_cast<std::uint16_t>(bytes[at + 1]) << 8));
}
std::uint64_t u64(const std::vector<std::uint8_t>& bytes, std::size_t at) {
    std::uint64_t value = 0;
    for (int i = 7; i >= 0; --i) value = (value << 8) | bytes[at + static_cast<std::size_t>(i)];
    return value;
}
bool gslImageAt(const std::vector<std::uint8_t>& bytes, std::size_t imageOffset,
                std::uint32_t expectedPsm, std::uint32_t expectedWidth, std::uint32_t expectedHeight) {
    for (std::size_t at = 0; at + 112 <= bytes.size() && bytes[at + 3] != 0x70;) {
        const std::size_t payload = at + 16;
        const std::uint64_t tag = u64(bytes, payload + 80);
        const std::uint64_t blit = u64(bytes, payload + 16);
        const std::uint64_t reg = u64(bytes, payload + 48);
        const std::size_t next = at + static_cast<std::size_t>(u16(bytes, at) + 1) * 16;
        if (payload + 96 == imageOffset)
            return ((blit >> 56) & 0x3f) == expectedPsm && (reg & 0xfff) == expectedWidth &&
                   ((reg >> 32) & 0xfff) == expectedHeight && (tag & 0x7fff) != 0;
        if (next <= at || next > bytes.size()) return false;
        at = next;
    }
    return false;
}

bool hasGreenGold(const rt::native::preview::PreviewTexture& tex, int x, int y, int w, int h) {
    for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx) {
            const std::uint8_t* p = &tex.rgba[(static_cast<std::size_t>(yy) * tex.width + xx) * 4];
            if (p[1] > 220 && p[0] > 160 && p[2] < 80) return true;
        }
    return false;
}

bool hasBrightNeutral(const rt::native::preview::PreviewTexture& tex, int x, int y, int w, int h) {
    for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx) {
            const std::uint8_t* p = &tex.rgba[(static_cast<std::size_t>(yy) * tex.width + xx) * 4];
            if (p[0] > 220 && p[1] > 220 && p[2] > 220) return true;
        }
    return false;
}
} // namespace

int main() {
    using namespace rt::native::assets;
    const auto root = disc();
    if (!std::filesystem::is_regular_file(root / "SYS/SCORE.GSL") ||
        !std::filesystem::is_regular_file(root / "ITEM/METER0.GSL")) {
        std::puts("SKIP (disc absent)");
        return 0;
    }

    const HudArt first = loadHudArt(root);
    const HudArt again = loadHudArt(root);
    check(gslImageAt(readBytes(root / "SYS/SCORE.GSL"), 0x8350, 0x14, 256, 256),
          "SCORE entry 1 image is at 0x8350 (PSMT4 256x256)");
    check(gslImageAt(readBytes(root / "ITEM/METER0.GSL"), 0x8550, 0x14, 128, 120),
          "METER0 gauge image is at 0x8550 (PSMT4 128x120)");
    check(first.lapDigits && first.lapDisc && first.gaugePage, "all W4-12 HUD pages load");
    check(first.lapDigits && first.lapDigits->width == 256 && first.lapDigits->height == 256,
          "SCORE entry 1 lap digit page dimensions");
    check(first.lapDisc && first.lapDisc->width == 256 && first.lapDisc->height == 256,
          "SCORE entry 1 lap disc page dimensions");
    check(first.gaugePage && first.gaugePage->width == 128 && first.gaugePage->height == 120,
          "METER0 gauge sub-page dimensions");
    if (first.lapDigits) {
        check(opaque(*first.lapDigits, 8, 200, 8, 16) > 20, "lap digit 1 has ink");
        check(opaque(*first.lapDigits, 0, 200, 80, 16) > 300, "lap digit bank is populated");
        check(hasGreenGold(*first.lapDigits, 0, 200, 80, 16), "CSA 7 lap palette has gold-green fill");
    }
    if (first.lapDisc) {
        check(opaque(*first.lapDisc, 232, 168, 24, 24) > 300, "lap disc has opaque texels");
        check(hasBrightNeutral(*first.lapDisc, 232, 168, 24, 24), "CSA 8 lap palette has bright disc fill");
    }
    if (first.gaugePage) {
        check(opaque(*first.gaugePage, 3, 53, 8, 59) > 250, "needle strip is tall and narrow");
        check(opaque(*first.gaugePage, 16, 80, 12, 12) > 60, "needle hub cap is roundish");
        check(opaque(*first.gaugePage, 40, 48, 20, 60) > 700, "boost arc cells have ink");
    }
    check(first.lapDigits && again.lapDigits && hash(*first.lapDigits) == hash(*again.lapDigits),
          "lap digit decode is byte stable");
    check(first.lapDisc && again.lapDisc && hash(*first.lapDisc) == hash(*again.lapDisc),
          "lap disc decode is byte stable");
    check(first.gaugePage && again.gaugePage && hash(*first.gaugePage) == hash(*again.gaugePage),
          "gauge page decode is byte stable");
    if (failures) return 1;
    std::puts("w12_hud_pages_test passed");
    return 0;
}
