// W5-03: ROM-law pins for the live race-HUD inputs. No ROM => SKIP (exit 0).
#include "gameplay/race_state_probe.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;
namespace probe = rt::native::gameplay::race_state_probe;

namespace {
int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}

std::uint16_t le16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint16_t{b[at]} | (std::uint16_t{b[at + 1]} << 8);
}
std::uint32_t le32at(const std::vector<std::uint8_t>& b, std::size_t at) {
    return std::uint32_t{b[at]} | (std::uint32_t{b[at + 1]} << 8) |
           (std::uint32_t{b[at + 2]} << 16) | (std::uint32_t{b[at + 3]} << 24);
}
std::uint32_t elfOffset(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    if (b.size() < 0x34 || le32at(b, 0x1c) + le16(b, 0x2a) * le16(b, 0x2c) > b.size()) return 0xffffffffu;
    const std::uint32_t phoff = le32at(b, 0x1c);
    const std::uint16_t entsize = le16(b, 0x2a);
    for (std::uint16_t i = 0; i < le16(b, 0x2c); ++i) {
        const std::size_t ph = phoff + std::size_t{i} * entsize;
        if (le32at(b, ph) != 1) continue;  // PT_LOAD
        const std::uint32_t off = le32at(b, ph + 4);
        const std::uint32_t base = le32at(b, ph + 8);
        const std::uint32_t size = le32at(b, ph + 16);
        if (va >= base && va - base + 4 <= size) return off + va - base;
    }
    return 0xffffffffu;
}
std::uint32_t word(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    const std::uint32_t off = elfOffset(b, va);
    return off == 0xffffffffu || off + 4 > b.size() ? 0 : le32at(b, off);
}
bool pinsMatch(const std::vector<std::uint8_t>& b) {
    return word(b, probe::kRacePlaceRankWriteVa) == 0xa0cb0247u &&
           word(b, probe::kLapFramesIncrementLoadVa) == 0x8e020260u &&
           word(b, probe::kLapFramesIncrementStoreVa) == 0xae020260u &&
           word(b, probe::kLapFramesResetVa) == 0xae000260u &&
           word(b, probe::kLapFramesHudReadVa) == 0x8e480260u &&
           word(b, probe::kGearHudReadVa) == 0x808201ffu &&
           word(b, probe::kGearFrameWriteVa) == 0xa20201ffu;
}
fs::path elfPath() {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    return "build/rom/SLUS_203.98";
}
}  // namespace

int main() {
    const fs::path path = elfPath();
    if (!fs::is_regular_file(path)) {
        std::printf("SKIP race state probe: no ELF (set RT_ELF)\n");
        return 0;
    }
    std::ifstream in(path, std::ios::binary);
    const std::vector<std::uint8_t> elf{std::istreambuf_iterator<char>(in), {}};
    if (elf.empty()) { std::printf("SKIP race state probe: unreadable ELF\n"); return 0; }

    check(probe::kCarStride == 0x270u, "car stride");
    check(probe::kRacePlaceOffset == 0x247u, "race place offset");
    check(probe::kCurrentLapFramesOffset == 0x260u, "lap frames offset");
    check(probe::kGearOffset == 0x1ffu, "gear offset");
    check(pinsMatch(elf), "ROM HUD input read/write instruction pins");
    auto mutated = elf;
    const std::uint32_t off = elfOffset(mutated, probe::kRacePlaceRankWriteVa);
    if (off != 0xffffffffu) mutated[off] ^= 1;
    check(!pinsMatch(mutated), "negative control catches byte flip");
    std::printf("W5-03 checks=%d failed=%d negative_control=%s\n", 6, failures,
                pinsMatch(mutated) ? "MISSED" : "CAUGHT");
    return failures == 0 ? 0 : 1;
}
