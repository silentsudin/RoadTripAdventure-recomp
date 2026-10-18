// W3-01 proportional FONTE text-metrics acceptance (w3_text_metrics_test).
// The ROM is read only at run time; no game bytes are embedded here.

#include "preview/text_metrics.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;
namespace metrics = rt::native::preview;

namespace {

constexpr std::uint32_t kGlyphWidthsVaddr = 0x0029A880;
constexpr std::uint32_t kElfBaseVaddr = 0x001FF000;

fs::path romPath() {
    if (const char* elf = std::getenv("RT_ELF"); elf && *elf) return elf;
    if (const char* home = std::getenv("HOME"); home && *home)
        return fs::path(home) / "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    return {};
}

bool readAdvance(const fs::path& path, unsigned char c, unsigned char& value) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    const std::streamoff offset = static_cast<std::streamoff>(kGlyphWidthsVaddr - kElfBaseVaddr + c - 0x20u);
    in.seekg(offset);
    char byte = 0;
    if (!in.get(byte)) return false;
    value = static_cast<unsigned char>(byte);
    return true;
}

}  // namespace

int main() {
    const fs::path elf = romPath();
    if (elf.empty() || !fs::is_regular_file(elf)) {
        std::puts("NO_ELF");
        return 0;
    }

    const metrics::GlyphTable first = metrics::GlyphTable::fromRomPath(elf);
    const metrics::GlyphTable second = metrics::GlyphTable::fromRomPath(elf);
    if (!first.usesRomAdvances() || !second.usesRomAdvances()) {
        std::puts("NO_ELF");
        return 0;
    }

    constexpr std::array<unsigned char, 14> kPinned{'A', 'a', ' ', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int advances = 0;
    for (const unsigned char c : kPinned) {
        unsigned char romValue = 0;
        if (readAdvance(elf, c, romValue) && first.glyph(c).advance == static_cast<float>(romValue)) ++advances;
    }

    bool monotonic = true;
    for (unsigned c = 0x20; c <= 0x7f; ++c) {
        const auto& glyph = first.glyph(static_cast<unsigned char>(c));
        monotonic = monotonic && glyph.advance >= 4.f && glyph.advance <= 24.f;
    }

    constexpr std::string_view kText = "Aa .0123456789";
    const auto run = first.layoutRun(kText, 1.25f);
    float sum = 0.f;
    bool layout = monotonic && run.size() == kText.size();
    // FONTE reserves physical column 31 for an icon: ASCII cells use 31 text
    // columns, so 'A' (0x41) is (1,1) and lowercase 'a' (0x61) is (2,2).
    layout = layout && first.glyph('A').cellX == 1 && first.glyph('A').cellY == 1 &&
             first.glyph('a').cellX == 2 && first.glyph('a').cellY == 2 &&
             first.glyph(' ').cellX < 0;
    for (const auto& placed : run) {
        sum += placed.glyph.advance * 1.25f;
        layout = layout && placed.advance > 0.f;
    }
    layout = layout && first.measure(kText, 1.25f) == sum;

    const bool deterministic = first == second && first.layoutRun(kText) == second.layoutRun(kText);
    std::printf("SCORE advances=%d/%zu layout=%s deterministic=%d\n", advances, kPinned.size(),
                layout ? "ok" : "fail", deterministic ? 1 : 0);
    return advances == static_cast<int>(kPinned.size()) && layout && deterministic ? 0 : 1;
}
