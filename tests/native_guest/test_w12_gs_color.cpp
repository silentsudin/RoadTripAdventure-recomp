// W12 independent pin: rtguest::rgba5551To8888 (guest_gs.cpp) against the oracle
// contract, ps2xRuntime gs_cpu_backend.cpp:38-47 (read as spec, not copied):
// PSMCT16 texel -> RGBA8888 widens every 5-bit colour channel with a plain
// << 3 (NO low-bit replication: 0x1F -> 0xF8, not 0xFF) and the 1-bit alpha with
// << 7 (-> 0x80, the PS2 "opaque" value), packed R | G<<8 | B<<16 | A<<24.
#include <cstdint>
#include <cstdio>

namespace rtguest { uint32_t rgba5551To8888(uint16_t c); }

static int fails = 0, checks = 0;
static void check(bool ok, const char* what, int line) {
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL %s (line %d)\n", what, line); ++fails; }
}
#define CHECK(c) check((c), #c, __LINE__)

static void vec(uint16_t in, uint32_t expect, int line) {
    uint32_t got = rtguest::rgba5551To8888(in);
    if (got != expect) std::fprintf(stderr, "  in=%04x got=%08x expect=%08x\n", in, got, expect);
    check(got == expect, "vector", line);
}
#define VEC(i, e) vec((i), (e), __LINE__)

int main() {
    VEC(0x0000, 0x00000000u);            // transparent black
    VEC(0xFFFF, 0x80F8F8F8u);            // all ones: 0x1F<<3 = 0xF8, a = 0x80 (not 0xFF)
    VEC(0x8000, 0x80000000u);            // alpha alone
    VEC(0x001F, 0x000000F8u);            // R alone
    VEC(0x03E0, 0x0000F800u);            // G alone
    VEC(0x7C00, 0x00F80000u);            // B alone
    VEC(0x4210, 0x00808080u);            // mid-grey 0x10 each
    VEC(0xC210, 0x80808080u);            // mid-grey opaque (card band class)
    VEC(0x8421, 0x80080808u);            // one LSB per channel + alpha
    VEC(0x0001, 0x00000008u);            // shift, not replicate
    VEC(0x0020, 0x00000800u);
    VEC(0x0400, 0x00080000u);

    for (uint32_t v = 0; v < 32; ++v) {
        CHECK(rtguest::rgba5551To8888(uint16_t(v)) == (v << 3));
        CHECK(rtguest::rgba5551To8888(uint16_t(v << 5)) == ((v << 3) << 8));
        CHECK(rtguest::rgba5551To8888(uint16_t(v << 10)) == ((v << 3) << 16));
    }
    for (uint32_t c = 0; c < 0x10000; c += 0x111) {
        uint32_t o = rtguest::rgba5551To8888(uint16_t(c));
        CHECK(((o >> 24) & 0xFFu) == ((c & 0x8000u) ? 0x80u : 0u));
        CHECK((o & 0x00070707u) == 0u);
    }
    std::printf("w12_gs_color_test: %d checks, %d fails\n", checks, fails);
    return fails ? 1 : 0;
}
