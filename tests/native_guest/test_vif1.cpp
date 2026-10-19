// W3 step-1 pin: native VIF1 interpreter (src/native/guest/guest_vif1.cpp).
// Semantics pinned against the oracle worker citations (ps2_vif1_interpreter.cpp
// control ops :516-628, MSCAL DBF rotation :560-611, generic UNPACK :688-986,
// MPG :630-650, DIRECT :652-686). Exit 0 = all pins hold.
#include "guest/guest_vif1.h"
#include "guest/guest_shim.h"

#include <cstdio>
#include <cstring>

namespace {

int failures = 0;
void check(bool ok, const char* what) {
    if (!ok) { std::printf("FAIL: %s\n", what); ++failures; }
}

constexpr std::uint32_t kSeg = 0x00400000u;
std::uint32_t wpos = 0;

void put(std::uint32_t w) {
    std::memcpy(rtguest::kRam + kSeg + wpos, &w, 4);
    wpos += 4;
}
void putBytes(const std::uint8_t* b, std::uint32_t n) {
    std::memcpy(rtguest::kRam + kSeg + wpos, b, n);
    wpos += n;
}
void padQw() { while (wpos % 16) put(0); }

int g_mscalCalls = 0;
std::uint32_t g_pc, g_top, g_itop;
void mscalHook(std::uint32_t pc, std::uint32_t top, std::uint32_t itop) {
    ++g_mscalCalls; g_pc = pc; g_top = top; g_itop = itop;
}
int g_directCalls = 0;
std::uint32_t g_directBytes;
bool g_directHl;
void directHook(const std::uint8_t*, std::uint32_t bytes, bool hl) {
    ++g_directCalls; g_directBytes = bytes; g_directHl = hl;
}

std::uint32_t vuW(std::uint32_t vf, std::uint32_t lane) {
    std::uint32_t v;
    std::memcpy(&v, rtguest::vu1Data() + vf * 16u + lane * 4u, 4);
    return v;
}

}  // namespace

int main() {
    rtguest::vif1SetHooks(mscalHook, directHook);
    rtguest::Vif1Regs& r = rtguest::vif1();

    // Control ops + DBF rotation.
    put((0x01u << 24) | 0x0101u);           // STCYCL cl=1 wl=1
    put((0x03u << 24) | 0x0010u);           // BASE 0x10
    put((0x02u << 24) | 0x0020u);           // OFFSET 0x20 -> tops=BASE, DBF clear
    put((0x04u << 24) | 0x0005u);           // ITOP 5
    put((0x17u << 24));                     // MSCNT -> top=0x10, tops=0x30, DBF set
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(r.cycle == 0x0101u, "STCYCL");
    check(r.base == 0x10u && r.ofst == 0x20u, "BASE/OFFSET");
    check(r.top == 0x10u && r.tops == 0x30u && (r.stat & (1u << 7)), "MSCNT latch+DBF");
    check(r.itop == 5u, "ITOP");

    wpos = 0;
    put((0x15u << 24) | 0x002Fu);           // MSCALF entry 0x2F (pc 0x178)
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(g_mscalCalls == 1 && g_pc == 0x178u && g_top == 0x30u, "MSCALF hook pc/top");
    check(r.tops == 0x10u && !(r.stat & (1u << 7)), "MSCALF DBF wrap to BASE");

    // UNPACK V4-32, num=2 -> vf0x100.
    wpos = 0;
    put((0x6Cu << 24) | (2u << 16) | 0x0100u);
    std::uint32_t vec[8];
    for (int i = 0; i < 8; ++i) vec[i] = 0x1000u + i;
    putBytes(reinterpret_cast<const std::uint8_t*>(vec), sizeof(vec));
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(vuW(0x100, 0) == 0x1000u && vuW(0x100, 3) == 0x1003u, "UNPACK V4-32 vec0");
    check(vuW(0x101, 1) == 0x1005u, "UNPACK V4-32 vec1");

    // UNPACK V4-16 signed extend (Z off) at vf0x102.
    wpos = 0;
    put((0x6Du << 24) | (1u << 16) | 0x0102u);
    const std::int16_t h[4] = {-1, 2, -3, 4};
    putBytes(reinterpret_cast<const std::uint8_t*>(h), sizeof(h));
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(vuW(0x102, 0) == 0xFFFFFFFFu && vuW(0x102, 2) == 0xFFFFFFFDu, "V4-16 sign extend");

    // STROW + UNPACK V4-32 mask-enable all-ROW (mask spec 1 per field, cycle 0).
    wpos = 0;
    put((0x30u << 24));                     // STROW
    const std::uint32_t rowv[4] = {7, 8, 9, 10};
    putBytes(reinterpret_cast<const std::uint8_t*>(rowv), 16);
    put((0x20u << 24));                     // STMASK
    put(0x55555555u);                       // spec 01 (ROW) on every field of cycle 0..3
    put((0x7Cu << 24) | (1u << 16) | 0x0103u);  // UNPACK V4-32|mask, num=1
    putBytes(reinterpret_cast<const std::uint8_t*>(vec), 16);
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(vuW(0x103, 0) == 7u && vuW(0x103, 3) == 10u, "STMASK row fill");

    // MPG uploads microcode at imm*8.
    wpos = 0;
    put((0x4Au << 24) | (2u << 16) | 0x0001u);   // 2 instructions at pc 8
    const std::uint8_t prog[16] = {0x11,2,3,4,5,6,7,8, 9,10,11,12,13,14,15,16};
    putBytes(prog, 16);
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(std::memcmp(rtguest::vu1Code() + 8, prog, 16) == 0, "MPG upload at 8");

    // DIRECT: 1 qword payload handed to the hook.
    wpos = 0;
    put((0x50u << 24) | 1u);
    putBytes(prog, 16);
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    check(g_directCalls == 1 && g_directBytes == 16 && !g_directHl, "DIRECT hook");

    // UNPACK with W bit: dest = vuAddr + TOP.
    wpos = 0;
    put((0x6Cu << 24) | (1u << 16) | 0x8100u);   // vf (TOP + 0x100)
    putBytes(reinterpret_cast<const std::uint8_t*>(vec), 16);
    padQw();
    rtguest::vif1Consume(kSeg, wpos / 16u);
    // W-bit adds TOPS (oracle :736), not the latched TOP.
    check(vuW(0x100 + (r.tops & 0x3FFu), 0) == 0x1000u, "UNPACK W-bit TOPS offset");

    std::printf("vif1_test: %s (%d failures)\n", failures ? "FAIL" : "PASS", failures);
    return failures ? 1 : 0;
}
