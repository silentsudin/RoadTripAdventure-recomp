// W5-01: RPM/driveline ROM-law contract. No ELF => SKIP (exit 0).
#include "gameplay/gauge_rpm.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;
namespace gr = rt::native::gameplay::gauge_rpm;

namespace {
int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}
std::uint16_t le16(const std::vector<std::uint8_t>& b, std::size_t o) {
    return std::uint16_t{b[o]} | (std::uint16_t{b[o + 1]} << 8);
}
std::uint32_t le32(const std::vector<std::uint8_t>& b, std::size_t o) {
    return std::uint32_t{b[o]} | (std::uint32_t{b[o + 1]} << 8) |
           (std::uint32_t{b[o + 2]} << 16) | (std::uint32_t{b[o + 3]} << 24);
}
struct Segment { std::uint32_t offset, vaddr, filesz; };
bool vaddrOffset(const std::vector<std::uint8_t>& elf, std::uint32_t va, std::size_t* out) {
    if (elf.size() < 0x34) return false;
    const auto phoff = le32(elf, 0x1c);
    const auto phentsize = le16(elf, 0x2a);
    const auto phnum = le16(elf, 0x2c);
    if (phentsize < 32 || phoff + std::size_t(phentsize) * phnum > elf.size()) return false;
    for (std::uint16_t i = 0; i < phnum; ++i) {
        const auto p = phoff + std::size_t(i) * phentsize;
        if (le32(elf, p) != 1) continue;  // PT_LOAD
        const auto offset = le32(elf, p + 4), vaddr = le32(elf, p + 8), filesz = le32(elf, p + 16);
        if (va >= vaddr && std::uint64_t(va) < std::uint64_t(vaddr) + filesz) {
            *out = std::size_t(offset) + va - vaddr;
            return *out < elf.size();
        }
    }
    return false;
}
std::uint32_t wordAt(const std::vector<std::uint8_t>& elf, std::uint32_t va) {
    std::size_t o{};
    return vaddrOffset(elf, va, &o) && o + 4 <= elf.size() ? le32(elf, o) : 0;
}
bool pinsMatch(const std::vector<std::uint8_t>& elf) {
    // AdvanceVehicleMotion: rpm load, threshold, selected-ratio load, store/clamps.
    if (wordAt(elf, 0x218f24) != 0x864201d0u || wordAt(elf, 0x218f34) != 0x2842251du ||
        wordAt(elf, 0x218f94) != 0x8453022cu || wordAt(elf, 0x2193c0) != 0xa64201d0u ||
        wordAt(elf, 0x2193c4) != 0x24022710u || wordAt(elf, 0x2193d4) != 0xa64001d0u)
        return false;
    // LoadCarPerformanceSpecs: gearbox-table base and 7 halfwords into car+0x21c..+0x226.
    if (wordAt(elf, 0x2180cc) != 0x244dca7cu || wordAt(elf, 0x218104) != 0x94460000u ||
        wordAt(elf, 0x21810c) != 0xa4e60000u || wordAt(elf, 0x21823c) != 0x3c01002fu)
        return false;
    // SelectScene3dHandler targets 0x21ce48/0x21cf80; each compares Q7
    // longitudinal motion / selected ratio before automatic gear changes.
    if (wordAt(elf, 0x21cea4) != 0x28632009u || wordAt(elf, 0x21cf00) != 0x28631388u ||
        wordAt(elf, 0x21cf20) != 0x28631f40u || wordAt(elf, 0x21cfdc) != 0x28631389u ||
        wordAt(elf, 0x21d038) != 0x28630bb8u || wordAt(elf, 0x21d058) != 0x28630fa0u)
        return false;
    std::size_t t{};
    if (!vaddrOffset(elf, 0x2ed334, &t) || t + 12 * 0x1c > elf.size()) return false;
    constexpr std::int16_t expected[6][7] = {
        {-95,116,162,227,318,446,0}, {-95,116,182,291,408,490,0},
        {-95,128,220,276,387,464,557}, {-95,128,260,327,458,550,660},
        {-95,144,300,414,539,647,711}, {-95,156,350,446,550,625,750},
    };
    for (std::size_t row = 0; row < 6; ++row)
        for (std::size_t col = 0; col < 7; ++col)
            if (static_cast<std::int16_t>(le16(elf, t + row * 0x1c + col * 2)) != expected[row][col]) return false;
    return true;
}
fs::path elfPath() {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    return "build/rom/SLUS_203.98";
}
}  // namespace

int main() {
    const auto path = elfPath();
    if (!fs::is_regular_file(path)) {
        std::printf("SKIP gauge RPM: no ELF (set RT_ELF or provide build/rom/SLUS_203.98)\n");
        return 0;
    }
    std::ifstream in(path, std::ios::binary);
    const std::vector<std::uint8_t> elf{std::istreambuf_iterator<char>(in), {}};
    if (elf.empty()) { std::printf("SKIP gauge RPM: unreadable ELF %s\n", path.c_str()); return 0; }

    check(pinsMatch(elf), "ROM instruction and gearbox-table pins");
    check(gr::kGearRatioTable[0][1] == 116 && gr::kGearRatioTable[5][6] == 750,
          "six gearbox ratio rows");
    check(gr::rpmFromMotionState(106, 116) == 116, "Q7 driveline to rpm truncates toward zero");
    check(gr::rpmFromMotionState(5634, 116) == 6216, "driving coupling check");
    check(gr::rpmFromMotionState(999999, 116) == 10000, "redline clamp");
    check(gr::rpmFromMotionState(-1, 116) == 0 && gr::rpmFromMotionState(1, 0) == 0,
          "negative and neutral clamp");
    check(gr::kAutoUpshiftRpm == decltype(gr::kAutoUpshiftRpm){8201, 5001} &&
          gr::kAutoDownshiftCurrentRpm == decltype(gr::kAutoDownshiftCurrentRpm){5000, 3000} &&
          gr::kAutoDownshiftProjectedRpm == decltype(gr::kAutoDownshiftProjectedRpm){8000, 4000},
          "automatic shift thresholds");

    auto mutated = elf;
    std::size_t pin{};
    check(vaddrOffset(mutated, 0x218f24, &pin), "vaddr mapping for negative control");
    mutated[pin] ^= 1;
    check(!pinsMatch(mutated), "negative control catches one-byte input flip");
    std::printf("W5-01 SCORE checks=%d failed=%d negative_control=%s\n", 9, failures,
                pinsMatch(mutated) ? "MISSED" : "CAUGHT");
    return failures == 0 ? 0 : 1;
}
