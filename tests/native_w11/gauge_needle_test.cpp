// W4-11: ROM-pinned 1P METER0 tachometer needle law.  No ELF => SKIP (exit 0).
#include "gameplay/gauge_needle.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace fs = std::filesystem;
namespace gn = rt::native::gameplay::gauge_needle;
namespace hl = rt::native::gameplay::hud_live;

namespace {
constexpr std::uint32_t kElfBase = 0x1ff000;
int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { ++failures; std::printf("FAIL %s\n", what); }
}
std::uint32_t le32(const std::vector<std::uint8_t>& b, std::uint32_t va) {
    const std::size_t o = va - kElfBase;
    if (o + 4 > b.size()) return 0;
    return std::uint32_t{b[o]} | (std::uint32_t{b[o + 1]} << 8) |
           (std::uint32_t{b[o + 2]} << 16) | (std::uint32_t{b[o + 3]} << 24);
}
bool pinsMatch(const std::vector<std::uint8_t>& b) {
    // handler: tach helper call, divide by 10000, add -0.7, record z rotation.
    return le32(b, 0x249dc0) == 0x8fa4009cu && le32(b, 0x249dd0) == 0x0c092548u &&
           le32(b, 0x249de4) == 0xc7828460u && le32(b, 0x249de8) == 0xc7818464u &&
           le32(b, 0x249e04) == 0x46020003u && le32(b, 0x249e0c) == 0x46010500u &&
           le32(b, 0x249edc) == 0xe7b40088u &&
           le32(b, 0x332f50) == 0x461c4000u && le32(b, 0x332f54) == 0xbf333333u &&
           le32(b, 0x332f84) == 0x40490fdbu &&
           // CalculateTachometerValue: signed halfword car+0x1d0, high clamp 10000.
           le32(b, 0x249520) == 0x848201d0u && le32(b, 0x249524) == 0x24032710u &&
           le32(b, 0x249538) == 0x0064100bu;
}
fs::path elfPath() {
    if (const char* e = std::getenv("RT_ELF"); e && *e) return e;
    return "build/rom/SLUS_203.98";
}
}  // namespace

int main() {
    const fs::path p = elfPath();
    if (!fs::is_regular_file(p)) {
        std::printf("SKIP gauge needle: no ELF (set RT_ELF or provide build/rom/SLUS_203.98)\n");
        return 0;
    }
    std::ifstream in(p, std::ios::binary);
    const std::vector<std::uint8_t> elf{std::istreambuf_iterator<char>(in), {}};
    if (elf.empty()) { std::printf("SKIP gauge needle: unreadable ELF %s\n", p.c_str()); return 0; }

    check(pinsMatch(elf), "ROM rotation/tach instruction pins");
    check(std::fabs(gn::angleRadForRpm(0) + .7f * gn::kPi) < 1e-6f, "rpm=0 angle");
    check(std::fabs(gn::angleRadForRpm(3500) + .35f * gn::kPi) < 1e-6f, "rpm=3500 angle");
    check(std::fabs(gn::angleRadForRpm(10000) - .3f * gn::kPi) < 1e-6f, "rpm=10000 angle");
    check(std::fabs(gn::angleRadForRpm(12000) - .3f * gn::kPi) < 1e-6f, "rpm high clamp");
    check(std::fabs(gn::angleRadForRpm(-100) + .71f * gn::kPi) < 1e-6f, "ROM high-only clamp");
    const auto q = gn::buildNeedleQuad({484, 294}, 3500);
    check(q.rotated && q.x == 554 && q.y == 362 && q.u == 3 && q.v == 53 && q.w == 8 && q.h == 59 &&
          q.texEntry == 1 && q.csa == 12 && q.verts == decltype(q.verts){-4, -59, 4, -59, -4, 0, 4, 0},
          "needle quad geometry");

    auto mutated = elf;
    mutated[0x249dc0 - kElfBase] ^= 1;  // negative control: flip one pinned instruction byte.
    check(!pinsMatch(mutated), "negative control catches one-byte input flip");
    std::printf("W4-11 SCORE checks=%d failed=%d negative_control=%s\n", 8, failures,
                pinsMatch(mutated) ? "MISSED" : "CAUGHT");
    return failures == 0 ? 0 : 1;
}
