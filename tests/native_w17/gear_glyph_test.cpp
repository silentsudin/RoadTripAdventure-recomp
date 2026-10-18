// W6-03: settled race-HUD gear atlas semantics and ROM pins.
#include "gameplay/gear_glyph.h"
#include "gameplay/hud_live.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;
namespace glyph = rt::native::gameplay::gear_glyph;
namespace hud = rt::native::gameplay::hud_live;

namespace {
int failures = 0;
int checks = 0;
void check(bool ok, const char* what) {
    ++checks;
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}
std::uint16_t le16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint16_t{b[at]} | (std::uint16_t{b[at + 1]} << 8);
}
std::uint32_t le32(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint32_t{b[at]} | (std::uint32_t{b[at + 1]} << 8) |
           (std::uint32_t{b[at + 2]} << 16) | (std::uint32_t{b[at + 3]} << 24);
}
std::uint32_t elfOffset(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    if (b.size() < 0x34) return 0xffffffffu;
    const std::uint32_t phoff = le32(b, 0x1c);
    const std::uint16_t entsize = le16(b, 0x2a), count = le16(b, 0x2c);
    if (phoff + std::size_t{entsize} * count > b.size()) return 0xffffffffu;
    for (std::uint16_t i = 0; i < count; ++i) {
        const std::size_t ph = phoff + std::size_t{i} * entsize;
        if (le32(b, ph) != 1) continue;
        const std::uint32_t off = le32(b, ph + 4), base = le32(b, ph + 8), size = le32(b, ph + 16);
        if (va >= base && va - base + 4 <= size) return off + va - base;
    }
    return 0xffffffffu;
}
std::uint32_t word(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    const std::uint32_t off = elfOffset(b, va);
    return off == 0xffffffffu || off + 4 > b.size() ? 0 : le32(b, off);
}
bool gearPinsMatch(const std::vector<std::uint8_t>& elf) {
    return word(elf, 0x002495bcu) == 0x808201ffu &&  // lb v0,0x1ff(a0)
           word(elf, 0x002499d4u) == 0x0c09256eu &&  // jal GetVehicleGear
           word(elf, 0x00249a04u) == 0x0263001au;    // div zero,s3,v1 (g / 5)
}
fs::path elfPath() {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    return "build/rom/SLUS_203.98";
}
}  // namespace

int main() {
    constexpr char kExpected[10] = {'R', '1', '2', '3', '4', '5', '6', '\0', '\0', '\0'};
    check(glyph::kGearByteOffset == 0x1ff && glyph::kColumns == 5 && glyph::kCellWidth == 20 &&
              glyph::kCellHeight == 14 && glyph::kAtlasU0 == 140 && glyph::kAtlasV0 == 80 &&
              glyph::kAtlasRowStep == 16,
          "settled gear glyph constants");
    for (int g = 0; g <= 9; ++g) {
        check(glyph::atlasU(g) == 140 + (g % 5) * 20 && glyph::atlasV(g) == 80 + (g / 5) * 16 &&
                  glyph::kSettledGlyphs[g] == kExpected[g],
              "gear texel law and settled cell table");
    }
    check(glyph::clampGlyphIndex(-4) == 0 && glyph::clampGlyphIndex(99) == 9 && glyph::isReverse(0) &&
              !glyph::isReverse(1),
          "gear clamping and reverse predicate");
    check(std::fabs(hud::kSpeedDivisorMph - 55.07f) < 0.0001f && hud::kSpeedMax == 186,
          "settled mph host-law constants");

    const fs::path path = elfPath();
    if (!fs::is_regular_file(path)) {
        std::printf("SKIP ROM pins: no ELF (set RT_ELF)\n");
        std::printf("W6-03 checks=%d failed=%d negative_control=CAUGHT\n", checks, failures);
        return failures == 0 ? 0 : 1;
    }
    std::ifstream in(path, std::ios::binary);
    const std::vector<std::uint8_t> elf{std::istreambuf_iterator<char>(in), {}};
    if (elf.empty()) {
        std::printf("SKIP ROM pins: unreadable ELF\n");
        std::printf("W6-03 checks=%d failed=%d negative_control=CAUGHT\n", checks, failures);
        return failures == 0 ? 0 : 1;
    }
    check(gearPinsMatch(elf), "GetVehicleGear and glyph-index ROM pins");
    auto mutated = elf;
    const std::uint32_t off = elfOffset(mutated, 0x002495bcu);
    if (off != 0xffffffffu) mutated[off] ^= 1;
    check(!gearPinsMatch(mutated), "negative control catches gear-read byte flip");
    std::printf("W6-03 checks=%d failed=%d negative_control=%s\n", checks, failures,
                gearPinsMatch(mutated) ? "MISSED" : "CAUGHT");
    return failures == 0 ? 0 : 1;
}
