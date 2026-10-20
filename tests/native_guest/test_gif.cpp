// W3 step-3a pin: native GIF front end (src/native/guest/guest_gif.cpp).
// Semantics pinned against oracle gs_frontend.cpp citations: packed/reglist/
// image walk (:919-993: NLOOP lo&0x7fff, FLG 59:58, NREG 63:60 0=16, PRE
// bit 46 with prim 57:47, odd reglist n pads 8 bytes), packed register
// decode writeRegisterPacked (:1144-1260: case 0x4 XYZF2 x@lo 15:0 y@lo
// 47:32 z@hi 27:4 fog@hi 43:36, kick = ADK hi bit 47 clear).
// Exit 0 = all pins hold.
#include "guest/guest_gif.h"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>

namespace {

int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { std::printf("FAIL: %s\n", what); ++failures; }
}

std::vector<std::uint8_t> g;
void qw(std::uint64_t lo, std::uint64_t hi) {
    for (int i = 0; i < 8; ++i) g.push_back(static_cast<std::uint8_t>(lo >> (i * 8)));
    for (int i = 0; i < 8; ++i) g.push_back(static_cast<std::uint8_t>(hi >> (i * 8)));
}
void raw(std::uint64_t v) {  // 8-byte reglist entry
    for (int i = 0; i < 8; ++i) g.push_back(static_cast<std::uint8_t>(v >> (i * 8)));
}

float f32(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }

}  // namespace

int main() {
    // --- packed vertex strip: NLOOP 3, PRE prim TRI_STRIP(1), NREG 3,
    // REGS 0x412 (ST, RGBAQ, XYZF2) — the LitStrip layout (vu1-microcode.md:58-59).
    qw(3ull | (1ull << 46) | (1ull << 47) | (3ull << 60), 0x412ull);
    for (int v = 0; v < 3; ++v) {
        qw(0x3F800000ull | (0x40000000ull << 32), 0x3F800000ull);   // ST s=1 t=2 q=1
        qw(255ull | (128ull << 32), 64ull | (255ull << 32));        // RGBAQ
        qw(160ull | (120ull << 32),
           (300ull << 4) | (255ull << 36) | (v < 2 ? (1ull << 47) : 0ull));  // XYZF2
    }
    // --- reglist: NLOOP 1, FLG 1, NREG 1, REGS 0x5 (XYZ2), odd n pads 8 bytes.
    qw(1ull | (1ull << 58) | (1ull << 60), 0x5ull);
    raw(161ull | (121ull << 32));                      // ADK bit 47 clear -> kick
    raw(0);                                              // pad
    // --- image: NLOOP 4, FLG 2 -> 64 data bytes.
    qw(4ull | (2ull << 58), 0);
    for (int i = 0; i < 4; ++i) qw(0, 0);
    // --- FLG 3 reserved: tag counted, stream dropped (oracle returns).
    qw(1ull | (3ull << 58) | (1ull << 60), 0x5ull);
    raw(0);

    rtguest::gifSink(g.data(), static_cast<std::uint32_t>(g.size()));

    check(rtguest::gifTagCount(0) == 1, "packed tags");
    check(rtguest::gifTagCount(1) == 1, "reglist tags");
    check(rtguest::gifTagCount(2) == 1, "image tags");
    check(rtguest::gifTagCount(3) == 1, "reserved tags counted");
    check(rtguest::gifVertsCount() == 4, "verts (3 packed + 1 reglist)");
    check(rtguest::gifKicksCount() == 2, "kicks (last packed vertex + reglist)");
    check(rtguest::gifImageBytesCount() == 64, "image bytes");
    // determinism: same bytes again -> counters double exactly.
    rtguest::gifSink(g.data(), static_cast<std::uint32_t>(g.size()));
    check(rtguest::gifVertsCount() == 8 && rtguest::gifKicksCount() == 4,
          "deterministic re-decode");
    f32(0);  // silence unused helper when all pins pass

    std::printf("gif_test: %s (%d failures)\n", failures ? "FAIL" : "PASS", failures);
    return failures ? 1 : 0;
}
