// W12 independent pins for guest shim contracts landed in 9400315:
//  (a) setPendingRA stores jal PC + 8 (oracle residue: jal@0x2089c0 -> ra 0x2089c8),
//  (b) FrameGuard/framePush/framePop stack mirroring,
//  (c) loadImage stages PT_LOAD segments only (zeroed bss, gap bytes untouched,
//      malformed headers rejected) using a SYNTHETIC ELF32 built here (no game data),
//  (d) sceSifBindRpc dummy-server pool in 0x01F10000..0x01F20000, stride 0x80,
//      sid at +0, client+0x24 = server.
#include "guest/guest_shim.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace guest { uint32_t sceSifBindRpc(uint32_t, uint32_t, uint64_t, ...); }

static int fails = 0, checks = 0;
static void check(bool ok, const char* what, int line) {
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL %s (line %d)\n", what, line); ++fails; }
}
#define CHECK(c) check((c), #c, __LINE__)

using namespace rtguest;

static void testPendingRA() {
    setPendingRA(0x2089c0u);
    CHECK(pendingRA() == 0x2089c8u);
    setPendingRA(0);
    CHECK(pendingRA() == 8u);
    setPendingRA(0x00200008u);
    CHECK(pendingRA() == 0x00200010u);
    int v = RT_CALLRA(0x100000u, 7);
    CHECK(v == 7 && pendingRA() == 0x100008u);
}

static void testFrames() {
    setFrameFloor(RT_EE_STACK_FLOOR);
    setFrameTop(RT_EE_STACK_TOP);
    CHECK(frameTop() == RT_EE_STACK_TOP);
    setPendingRA(0x2089c0u);
    {
        FrameGuard a(48);
        CHECK(a.saved == RT_EE_STACK_TOP);
        CHECK(a.rtRA == 0x2089c8u);
        CHECK(frameTop() == RT_EE_STACK_TOP - 48);
        {
            FrameGuard b(0);
            CHECK(frameTop() == RT_EE_STACK_TOP - 48);
            FrameGuard c(128);
            CHECK(frameTop() == RT_EE_STACK_TOP - 48 - 128);
        }
        CHECK(frameTop() == RT_EE_STACK_TOP - 48);
    }
    CHECK(frameTop() == RT_EE_STACK_TOP);
    uint32_t saved = frameTop();
    framePush(16);
    CHECK(frameTop() == saved - 16);
    framePop(saved);
    CHECK(frameTop() == saved);
}

struct Seg { uint32_t off, addr, filesz, memsz; };
static std::vector<uint8_t> makeElf(const std::vector<Seg>& segs, size_t total,
                                    bool setMagic = true) {
    std::vector<uint8_t> f(total, 0);
    if (setMagic) { f[0] = 0x7f; f[1] = 'E'; f[2] = 'L'; f[3] = 'F'; f[4] = 1; f[5] = 1; f[6] = 1; }
    auto w32 = [&](size_t o, uint32_t v) { std::memcpy(&f[o], &v, 4); };
    auto w16 = [&](size_t o, uint16_t v) { std::memcpy(&f[o], &v, 2); };
    w32(0x1c, 0x34);
    w16(0x2a, 32);
    w16(0x2c, uint16_t(segs.size()));
    for (size_t i = 0; i < segs.size(); ++i) {
        size_t e = 0x34 + i * 32;
        w32(e, 1);
        w32(e + 4, segs[i].off); w32(e + 8, segs[i].addr);
        w32(e + 0x10, segs[i].filesz); w32(e + 0x14, segs[i].memsz);
    }
    return f;
}

static void testLoadImage() {
    const uint32_t A = 0x00300000u, B = 0x00300100u, C = 0x00300200u;
    std::memset(kRam + A, 0xAA, 0x300);
    std::vector<uint8_t> f = makeElf({{0x100, A, 16, 32},
                                      {0x120, B, 8, 8},
                                      {0x130, 0x80000000u + C, 4, 4}},
                                     0x200);
    for (int i = 0; i < 16; ++i) f[0x100 + i] = uint8_t(0x10 + i);
    for (int i = 0; i < 8; ++i) f[0x120 + i] = uint8_t(0x50 + i);
    for (int i = 0; i < 4; ++i) f[0x130 + i] = uint8_t(0x70 + i);
    CHECK(loadImage(f.data(), f.size()));
    for (int i = 0; i < 16; ++i) CHECK(kRam[A + i] == 0x10 + i);
    for (int i = 16; i < 32; ++i) CHECK(kRam[A + i] == 0);          // bss zeroed
    for (int i = 32; i < 0x100; ++i) CHECK(kRam[A + i] == 0xAA);    // gap between segments not staged
    for (int i = 0; i < 8; ++i) CHECK(kRam[B + i] == 0x50 + i);
    CHECK(kRam[B + 8] == 0xAA);
    for (int i = 0; i < 4; ++i) CHECK(kRam[C + i] == 0x70 + i);     // 0x80xxxxxx alias stripped

    CHECK(!loadImage(f.data(), 0x20));
    auto bad = makeElf({}, 0x40, false);
    CHECK(!loadImage(bad.data(), bad.size()));
    auto fgm = makeElf({{0x100, A, 32, 16}}, 0x200);
    CHECK(!loadImage(fgm.data(), fgm.size()));
    auto oob = makeElf({{0x100, RT_GUEST_RAM_SIZE - 8, 4, 16}}, 0x200);
    CHECK(!loadImage(oob.data(), oob.size()));
    auto trunc = makeElf({{0x1F8, A, 16, 16}}, 0x200);
    CHECK(!loadImage(trunc.data(), trunc.size()));
    auto nl = makeElf({{0x100, A, 4, 4}}, 0x200);
    uint32_t t = 4; std::memcpy(&nl[0x34], &t, 4);                  // PT_NOTE: skipped
    std::memset(kRam + A, 0xAA, 4);
    CHECK(loadImage(nl.data(), nl.size()));
    CHECK(kRam[A] == 0xAA);
}

static void testSdRpcPool() {
    const uint32_t client = 0x00400000u;
    uint32_t seen[4];
    for (int i = 0; i < 4; ++i) {
        uint32_t id = 0x80007F00u + uint32_t(i);
        CHECK(guest::sceSifBindRpc(client, id, 0) == 0);
        uint32_t srv; std::memcpy(&srv, G(client + 0x24), 4);
        seen[i] = srv;
        CHECK(srv >= 0x01F10000u && srv < 0x01F20000u);
        CHECK(((srv - 0x01F10000u) % 0x80u) == 0u);
        uint32_t sid; std::memcpy(&sid, G(srv), 4);
        CHECK(sid == id);
    }
    CHECK(seen[1] == seen[0] + 0x80u && seen[2] == seen[1] + 0x80u && seen[3] == seen[2] + 0x80u);
    CHECK(guest::sceSifBindRpc(client, 0x80007F00u, 0) == 0);
    uint32_t again; std::memcpy(&again, G(client + 0x24), 4);
    CHECK(again == seen[0]);
}

int main() {
    testPendingRA();
    testFrames();
    testLoadImage();
    testSdRpcPool();
    std::printf("w12_shim_contracts_test: %d checks, %d fails\n", checks, fails);
    return fails ? 1 : 0;
}
