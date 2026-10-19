// W12 host services, wave 5 (libgraph). Mirrors the oracle's ps2_stubs GS
// family (Kernel/Stubs/GS.cpp) for the 17 sceGs* entries in
// config/native_host_stubs.txt. State the game reads back (GParam via
// sceGsSyncV, GsImage structs, dbuff return parities) is faithful; the GS
// DEVICE itself does not exist in src/native yet — packets reach the DMA
// model (guest_libs_dma.cpp) and are walk-discarded there.
// TODO(deviation) [gs-sink]: until the native renderer lands (W12 MMIO/GS
// staging), apply/seed/submit bodies keep state only; scanout parity comes
// with that sink. Nothing here is compiled from ps2xRuntime (decomp rule).
#include <cstdio>
#include <cstring>
#include "guest_kernel.h"
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

// oracle GS.cpp:1487 GsGParam (interlace/omode/ffmode set by ResetGraph:848).
struct GsGParam { uint8_t interlace = 0, omode = 0, ffmode = 0, version = 0; } g_gparam;

// oracle Support.h:1574 GsImageMem (12 bytes, writeGsImage target layout).
struct GsImageMem {
    uint16_t x, y, width, height, vram_addr;
    uint8_t vram_width, psm;
};
static_assert(sizeof(GsImageMem) == 12, "GsImageMem size mismatch");

uint32_t rdReg(uint32_t reg) {  // oracle mem.readIORegister (kRam reg holes)
    uint32_t v = 0;
    std::memcpy(&v, rtguest::kRam + (reg & 0x01FFFFFFu), 4);
    return v;
}

void setDefImage(GsImageMem& img, uint32_t x, uint32_t y, uint32_t w,
                 uint32_t h, uint32_t vramAddr, uint32_t vramWidth, uint32_t psm) {
    img.x = (uint16_t)x; img.y = (uint16_t)y;
    img.width = (uint16_t)w; img.height = (uint16_t)h;
    img.vram_addr = (uint16_t)vramAddr;
    img.vram_width = (uint8_t)vramWidth;
    img.psm = (uint8_t)psm;
}

}  // namespace

// ---- sceGsResetGraph (GS.cpp:833): GParam state; GS-reset packet is the
// gs-sink deviation. Returns 0 (success path). ----
uint32_t sceGsResetGraph(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, ...) {
    g_gparam.interlace = (uint8_t)(q1 & 1u);
    g_gparam.omode = (uint8_t)(q2 & 0xFFu);
    g_gparam.ffmode = (uint8_t)(q3 & 1u);
    g_gparam.version = 0x12;  // GS12 (oracle writeGsGParamToScratch)
    return 0;
}
RT_STUB_FN(0x0026E250, guest::sceGsResetGraph)

// ---- sceGsResetPath (GS.cpp:903): 0 ----
uint32_t sceGsResetPath(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026E3F0, guest::sceGsResetPath)

// ---- sceGsSyncV (GS.cpp:1332): WaitVSyncTick(g_gparam.interlace ? -1 : 1);
// parks the green thread one vblank (kernel kWaitVSync). ----
uint32_t sceGsSyncV(uint32_t q0, ...) {
    const int32_t r = rtguest::kWaitVSync(g_gparam.interlace ? -1 : 1);
    return (uint32_t)r;
}
RT_STUB_FN(0x0026E458, guest::sceGsSyncV)

// ---- sceGsSyncPath (GS.cpp:1265): busy bits from the kRam DMAC/VIF reg
// holes; our DMA model completes synchronously, so nothing is busy. ----
uint32_t sceGsSyncPath(uint32_t mode, ...) {
    uint32_t result = 0;
    if (rdReg(0x10009000u) & 0x100u) result |= 1;   // MTO1 CHCR busy
    if (rdReg(0x1000A000u) & 0x100u) result |= 2;   // GIF CHCR busy
    if (rdReg(0x10003C00u) & 0x1F000003u) result |= 4;  // GIF SYNTR
    if (rdReg(0x10003020u) & 0xC00u) result |= 0x10;    // VIF1_STAT
    if (mode == 0) return 0;  // blocking: transfers already complete
    return result;
}
RT_STUB_FN(0x0026E4F0, guest::sceGsSyncPath)

// ---- sceGsSetDefLoadImage (GS.cpp:1159): writes the GsImage struct the
// game reads back (12 bytes at q0). ----
uint32_t sceGsSetDefLoadImage(uint32_t imgAddr, uint32_t q1, uint32_t q2,
                              uint32_t q3, uint32_t q4, uint32_t q5,
                              uint32_t q6, uint32_t q7, ...) {
    void* dst = rtguest::G(imgAddr);
    if (dst == rtguest::kScratch) return (uint32_t)-1;  // quarantine: bad addr
    GsImageMem img{};
    setDefImage(img, q1, q2, q3, q4, q5, q6, q7);
    std::memcpy(dst, &img, sizeof(img));
    return 0;
}
RT_STUB_FN(0x0026E808, guest::sceGsSetDefLoadImage)

// ---- sceGsSetDefStoreImage (GS.cpp:1177): same body (mirror image). ----
uint32_t sceGsSetDefStoreImage(uint32_t imgAddr, uint32_t q1, uint32_t q2,
                               uint32_t q3, uint32_t q4, uint32_t q5,
                               uint32_t q6, uint32_t q7, ...) {
    void* dst = rtguest::G(imgAddr);
    if (dst == rtguest::kScratch) return (uint32_t)-1;
    GsImageMem img{};
    setDefImage(img, q1, q2, q3, q4, q5, q6, q7);
    std::memcpy(dst, &img, sizeof(img));
    return 0;
}
RT_STUB_FN(0x0026E9F0, guest::sceGsSetDefStoreImage)

// ---- sceGsExecLoadImage (GS.cpp:640) / sceGsExecStoreImage (717): the
// oracle builds a BITBLT GIF packet and transfers pixels with the GS; the
// native gs-sink does not exist yet — accepted, state untouched (TODO above).
uint32_t sceGsExecLoadImage(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026EB30, guest::sceGsExecLoadImage)

uint32_t sceGsExecStoreImage(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026ECB0, guest::sceGsExecStoreImage)

// ---- sceGsSetDefDBuffDc (GS.cpp:916): seeds the double-buffer env packet
// struct (gs-sink deviation: not seeded). Returns 0. ----
uint32_t sceGsSetDefDBuffDc(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, uint32_t q6, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026F450, guest::sceGsSetDefDBuffDc)

// ---- sceGsSwapDBuffDc (GS.cpp:1182): parity return which^1 (the game
// threads its field flag on this); GS apply is the sink deviation. ----
uint32_t sceGsSwapDBuffDc(uint32_t envAddr, uint32_t which, ...) {
    return (which & 1u) ^ 1u;
}
RT_STUB_FN(0x0026F738, guest::sceGsSwapDBuffDc)

// ---- env struct builders: the game keeps these structs in its own data and
// reads them back; the oracle only forwards them to the GS, so returning
// success without touching them matches (GS.cpp:1050/1073/1116/908/807/820).
uint32_t sceGsSetDefDispEnv(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026F798, guest::sceGsSetDefDispEnv)

uint32_t sceGsPutDispEnv(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026FA08, guest::sceGsPutDispEnv)

// ---- sceGszbufaddr (GS.cpp:1370): pure block-count math on host-side
// gparam — mirror the arithmetic; no zbuf allocation exists natively. ----
uint32_t sceGszbufaddr(uint32_t q0, uint32_t q1, uint32_t q2, ...) {
    (void)q1;
    const int32_t w = (int32_t)(int16_t)(q1 & 0xFFFFu);
    const int32_t h = (int32_t)(int16_t)(q2 & 0xFFFFu);
    int32_t wb = (w + 63) >> 6;
    if (w + 63 < 0) wb = (w + 126) >> 6;
    int32_t hb;
    if ((q0 & 2) != 0) { hb = (h + 63) >> 6; if (h + 63 < 0) hb = (h + 126) >> 6; }
    else               { hb = (h + 31) >> 5; if (h + 31 < 0) hb = (h + 62) >> 5; }
    return (uint32_t)(wb * hb);  // oracle returns the block product + alloc state
}
RT_STUB_FN(0x0026FAC8, guest::sceGszbufaddr)

uint32_t sceGsSetDefDrawEnv(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026FB90, guest::sceGsSetDefDrawEnv)

uint32_t sceGsSetDefClear(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, uint32_t q6, uint32_t q7, uint32_t q8, uint32_t q9, uint32_t q10, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026FD78, guest::sceGsSetDefClear)

uint32_t sceGsPutDrawEnv(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026FE80, guest::sceGsPutDrawEnv)

uint32_t sceGsSetDefDrawEnv2(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026FF68, guest::sceGsSetDefDrawEnv2)

}  // namespace guest
