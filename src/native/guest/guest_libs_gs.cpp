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
#include "guest_vif1.h"
#include "guest_gs.h"
#include "guest_kernel.h"
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

// W12 census: which libgraph env entries the game actually calls (RT_VIF_HIST).
struct GsEnvCalls { uint64_t resetGraph = 0, putDrawEnv = 0, putDispEnv = 0, swapDc = 0,
                                  setDefDc = 0, loadImg = 0, storeImg = 0, syncV = 0; } g_envCalls;

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

// W12 census: which libgraph env entries the game actually calls at boot.
void gsEnvCensusDump() {
    if (!rtguest::vif1HistOn()) return;
    std::fprintf(stderr, "[gsenv] resetGraph=%llu putDrawEnv=%llu putDispEnv=%llu "
                         "swapDc=%llu setDefDc=%llu syncV=%llu\n",
                 (unsigned long long)g_envCalls.resetGraph,
                 (unsigned long long)g_envCalls.putDrawEnv,
                 (unsigned long long)g_envCalls.putDispEnv,
                 (unsigned long long)g_envCalls.swapDc,
                 (unsigned long long)g_envCalls.setDefDc,
                 (unsigned long long)g_envCalls.syncV);
}

// ---- sceGsResetGraph (GS.cpp:833): GParam state; GS-reset packet is the
// gs-sink deviation. Returns 0 (success path). ----
uint32_t sceGsResetGraph(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, ...) {
    ++g_envCalls.resetGraph;
    g_gparam.interlace = (uint8_t)(q1 & 1u);
    g_gparam.omode = (uint8_t)(q2 & 0xFFu);
    g_gparam.ffmode = (uint8_t)(q3 & 1u);
    g_gparam.version = 0x12;  // GS12 (oracle writeGsGParamToScratch)
    // oracle writeGsGParamToScratch (Support.h:1953): 4-byte GParam at
    // scratchpad 0x100; sceGszbufaddr (GS.cpp:1400) reads it back as u64.
    std::memcpy(rtguest::kScratch + 0x100, &g_gparam, sizeof(g_gparam));
    return 0;
}
RT_STUB_FN(0x0026E250, guest::sceGsResetGraph)

// ---- sceGsResetPath (GS.cpp:903): 0 ----
uint32_t sceGsResetPath(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x0026E3F0, guest::sceGsResetPath)

// ---- sceGsSyncV (GS.cpp:1332): WaitVSyncTick(g_gparam.interlace ? -1 : 1);
// parks the green thread one vblank (kernel kWaitVSync). ----
uint32_t sceGsSyncV(uint32_t q0, ...) {
    ++g_envCalls.syncV;
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

// ---- sceGsSetDefDBuffDc (GS.cpp:916): seed the 0x330 double-buffer env
// struct exactly as the oracle (display envs, four draw env pair sets,
// giftags, clear packets). Transliterated oracle host-infra (established
// practice: stub semantics = oracle, cited per encoder). Args: a0 env,
// a1 psm, a2 w, a3 h, a4 ztest, a5 zpsm, a6 clear. ----
namespace {
// Encoders mirrored from oracle Helpers/Support.h:1597-1684.
uint64_t makePmode(uint32_t en1, uint32_t en2, uint32_t mmod, uint32_t amod,
                   uint32_t slbg, uint32_t alp) {
    return (uint64_t(en1 & 1) << 0) | (uint64_t(en2 & 1) << 1) | (uint64_t(1) << 2) |
           (uint64_t(mmod & 1) << 5) | (uint64_t(amod & 1) << 6) |
           (uint64_t(slbg & 1) << 7) | (uint64_t(alp & 0xFF) << 8);
}
uint64_t makeDispFb(uint32_t fbp, uint32_t fbw, uint32_t psm, uint32_t dbx,
                    uint32_t dby) {
    return (uint64_t(fbp & 0x1FF) << 0) | (uint64_t(fbw & 0x3F) << 9) |
           (uint64_t(psm & 0x1F) << 15) | (uint64_t(dbx & 0x7FF) << 32) |
           (uint64_t(dby & 0x7FF) << 43);
}
uint64_t makeDisplay(uint32_t dx, uint32_t dy, uint32_t magh, uint32_t magv,
                     uint32_t dw, uint32_t dh) {
    return (uint64_t(dx & 0x0FFF) << 0) | (uint64_t(dy & 0x07FF) << 12) |
           (uint64_t(magh & 0x0F) << 23) | (uint64_t(magv & 0x03) << 27) |
           (uint64_t(dw & 0x0FFF) << 32) | (uint64_t(dh & 0x07FF) << 44);
}
uint64_t makeLibgraphDisplay(uint32_t dx, uint32_t dy, uint32_t w, uint32_t h) {
    w = w == 0u ? 640u : w;
    uint32_t magh = (2560u + w - 1u) / w - 1u;
    if (magh > 15u) magh = 15u;
    return makeDisplay(dx, dy, magh, 0u, (magh + 1u) * w - 1u, h - 1u);
}
uint64_t makeFrame(uint32_t fbp, uint32_t fbw, uint32_t psm, uint32_t fbmsk) {
    return (uint64_t(fbp & 0x1FFu) << 0) | (uint64_t(fbw & 0x3Fu) << 16) |
           (uint64_t(psm & 0x3Fu) << 24) | (uint64_t(fbmsk) << 32);
}
uint64_t makeZbuf(uint32_t zbp, uint32_t psm, bool zmsk) {
    return (uint64_t(zbp & 0x1FFu) << 0) | (uint64_t(psm & 0xFu) << 24) |
           (uint64_t(zmsk ? 1u : 0u) << 32);
}
uint64_t makeXYOffset(int32_t width, int32_t height) {
    const int32_t offX = 0x800 - (width >> 1), offY = 0x800 - (height >> 1);
    return (uint64_t(uint32_t(offY) & 0xFFFFu) << 36) |
           (uint64_t(uint32_t(offX) & 0xFFFFu) << 4);
}
uint64_t makeScissor(int32_t width, int32_t height) {
    return (uint64_t(uint32_t(width - 1) & 0x7FFu) << 16) |
           (uint64_t(uint32_t(height - 1) & 0x7FFu) << 48);
}
uint64_t makeTest(uint32_t ztest) {
    if ((ztest & 0x3u) == 0u) return 0x30000ULL;
    return (uint64_t(ztest & 0x3u) << 17) | 0x10000ULL;
}
uint64_t makeGiftagAplusD(uint32_t nloop) {
    return (uint64_t(nloop & 0x7FFFu) << 0) | (uint64_t(1u) << 15) |
           (uint64_t(1u) << 60);
}
// sceGszbufaddr (oracle GS.cpp:1370, product + scratch GParam scaling at
// GS.cpp:1398-1413).
uint32_t zbufProduct(uint32_t psm, int32_t w, int32_t h) {
    int32_t width_blocks = (w + 63) >> 6;
    if (w + 63 < 0) width_blocks = (w + 126) >> 6;
    int32_t height_blocks;
    if ((psm & 2u) != 0u) {
        int32_t v = (h + 63) >> 6;
        if (h + 63 < 0) v = (h + 126) >> 6;
        height_blocks = v;
    } else {
        int32_t v = (h + 31) >> 5;
        if (h + 31 < 0) v = (h + 62) >> 5;
        height_blocks = v;
    }
    int32_t product = width_blocks * height_blocks;
    uint64_t gparam_val = 0;
    std::memcpy(&gparam_val, rtguest::kScratch + 0x100, sizeof(gparam_val));
    if ((gparam_val & 0xFFFF0000FFFFULL) == 1ULL) product = (product * 0x10000) >> 16;
    else product = (product * 0x20000) >> 16;
    return uint32_t(product);
}
void writePair(uint8_t* p, uint64_t value, uint64_t reg) {
    std::memcpy(p, &value, 8);
    std::memcpy(p + 8, &reg, 8);
}
// seedGsDrawEnv1/2 (Support.h:1911/1930); ctx2 picks the _2 register codes.
void seedDrawPairs(uint8_t* p, int32_t w, int32_t h, uint32_t fbp, uint32_t fbw,
                   uint32_t psm, uint32_t zbp, uint32_t zpsm, uint32_t ztest,
                   bool ctx2) {
    writePair(p + 0,   makeFrame(fbp, fbw, psm, 0u),            ctx2 ? 0x4D : 0x4C);
    writePair(p + 16,  makeZbuf(zbp, zpsm, (ztest & 3u) == 0u), ctx2 ? 0x4F : 0x4E);
    writePair(p + 32,  makeXYOffset(w, h),                      ctx2 ? 0x19 : 0x18);
    writePair(p + 48,  makeScissor(w, h),                       ctx2 ? 0x41 : 0x40);
    writePair(p + 64,  1u, 0x1A);   // PRMODECONT
    writePair(p + 80,  1u, 0x46);   // COLCLAMP
    writePair(p + 96,  0u, 0x45);   // DTHE (oracle seeds dthe=false)
    writePair(p + 112, makeTest(ztest),                         ctx2 ? 0x48 : 0x47);
}
// seedGsClearPacket (GS.cpp:36): TEST/PRIM/RGBAQ/XYZ2/XYZ2/TEST pair packet.
void seedClearPairs(uint8_t* p, int32_t w, int32_t h, uint32_t rgba,
                    uint32_t ztest, bool ctx2) {
    const int32_t offX = 0x800 - (w >> 1), offY = 0x800 - (h >> 1);
    const auto xyz = [](int32_t x, int32_t y) {
        return uint64_t(uint16_t(x << 4)) | (uint64_t(uint16_t(y << 4)) << 16);
    };
    writePair(p + 0,   makeTest(0u), ctx2 ? 0x48 : 0x47);
    writePair(p + 16,  6ull | (ctx2 ? 1ull << 9 : 0ull), 0x00);  // SPRITE|ctx2
    writePair(p + 32,  rgba, 0x01);                              // RGBAQ
    writePair(p + 48,  xyz(offX, offY), 0x05);                   // XYZ2
    writePair(p + 64,  xyz(offX + w, offY + h), 0x05);           // XYZ2
    writePair(p + 80,  makeTest(ztest), ctx2 ? 0x48 : 0x47);
}
constexpr uint32_t kDbDc_disp0 = 0, kDbDc_giftag0 = 80, kDbDc_clear0 = 352,
                   kDbDc_giftag1 = 448, kDbDc_clear1 = 720,
                   kDbDc_draw01 = 96, kDbDc_draw02 = 224, kDbDc_draw11 = 464,
                   kDbDc_draw12 = 592;
}  // namespace
uint32_t sceGsSetDefDBuffDc(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3,
                            uint32_t q4, uint32_t q5, uint32_t q6, ...) {
    ++g_envCalls.setDefDc;
    uint8_t* env = static_cast<uint8_t*>(rtguest::G(q0));
    if (env == rtguest::kScratch) return (uint32_t)-1;  // quarantine: bad addr
    const uint32_t psm = q1, ztest = q4, zpsm = q5, clear = q6;
    uint32_t w = q2, h = q3;
    if (w == 0u) w = 640u;
    if (h == 0u) h = 448u;
    uint32_t fbw = (w + 63u) / 64u;
    if (fbw < 1u) fbw = 1u;
    const uint64_t pmode = makePmode(1u, 1u, 0u, 0u, 0u, 0x80u);
    const uint64_t smode2 = (uint64_t(g_gparam.interlace & 0x1u) << 0) |
                            (uint64_t(g_gparam.ffmode & 0x1u) << 1);
    const uint64_t display = makeLibgraphDisplay(636u, 32u, w, h);
    const uint32_t zbufAddr = zbufProduct(psm, int32_t(w), int32_t(h));
    // libgraph places buffer 1 after buffer 0 (Support.h page math).
    const uint32_t pageLines = (psm & 0x2u) != 0u ? 64u : 32u;
    const uint32_t fbp1 = fbw * ((h + pageLines - 1u) / pageLines);
    std::memset(env, 0, 0x330);
    auto put = [&](uint32_t off, uint64_t v) { std::memcpy(env + off, &v, 8); };
    put(kDbDc_disp0 + 0, pmode);  put(kDbDc_disp0 + 8, smode2);
    put(kDbDc_disp0 + 16, makeDispFb(fbp1, fbw, psm, 0u, 0u));
    put(kDbDc_disp0 + 24, display);  put(kDbDc_disp0 + 32, 0u);
    put(40 + 0, pmode);  put(40 + 8, smode2);
    put(40 + 16, makeDispFb(0u, fbw, psm, 0u, 0u));
    put(40 + 24, display);  put(40 + 32, 0u);
    const bool seedClear = clear != 0u;
    put(kDbDc_giftag0, makeGiftagAplusD(seedClear ? 22u : 16u));
    put(kDbDc_giftag0 + 8, 0xEULL);
    put(kDbDc_giftag1, makeGiftagAplusD(seedClear ? 22u : 16u));
    put(kDbDc_giftag1 + 8, 0xEULL);
    seedDrawPairs(env + kDbDc_draw01, int32_t(w), int32_t(h), 0u, fbw, psm,
                  zbufAddr, zpsm, ztest, false);
    seedDrawPairs(env + kDbDc_draw02, int32_t(w), int32_t(h), 0u, fbw, psm,
                  zbufAddr, zpsm, ztest, true);
    seedDrawPairs(env + kDbDc_draw11, int32_t(w), int32_t(h), fbp1, fbw, psm,
                  zbufAddr, zpsm, ztest, false);
    seedDrawPairs(env + kDbDc_draw12, int32_t(w), int32_t(h), fbp1, fbw, psm,
                  zbufAddr, zpsm, ztest, true);
    if (seedClear) {
        seedClearPairs(env + kDbDc_clear0, int32_t(w), int32_t(h), 0u, ztest, false);
        seedClearPairs(env + kDbDc_clear1, int32_t(w), int32_t(h), 0u, ztest, true);
    }
    return 0;
}
RT_STUB_FN(0x0026F450, guest::sceGsSetDefDBuffDc)

// ---- sceGsSwapDBuffDc (GS.cpp:1182): parity return which^1 (the game
// threads its field flag on this) + full oracle mirror of the apply chain:
// applyGsDispEnv, applyGsRegPairs x2, clearFramebufferContext, then
// applyGsClearPacket (hasSeeded guard; its markFrameStart is our frame
// boundary). ----
namespace {
void applyDrawEnvPairs(const uint8_t* p) {  // GsDrawEnv1Mem: 8 x {value,reg} u64
    for (int i = 0; i < 8; ++i) {
        uint64_t value, reg;
        std::memcpy(&value, p + i * 16u, 8);
        std::memcpy(&reg, p + i * 16u + 8, 8);
        rtguest::gsWriteRegister(static_cast<uint8_t>(reg), value);
    }
}
}  // namespace
uint32_t sceGsSwapDBuffDc(uint32_t envAddr, uint32_t which, ...) {
    ++g_envCalls.swapDc;
    const uint8_t* db = static_cast<const uint8_t*>(rtguest::G(envAddr));
    if (db == rtguest::kScratch) return (uint32_t)-1;
    const uint32_t w = which & 1u;
    const bool one = w == 1u;
    rtguest::gsSetDisplayEnv(db + w * 40u);  // GsDispEnvMem {pmode,smode2,dispfb,display,bgcolor}
    applyDrawEnvPairs(db + (one ? kDbDc_draw11 : kDbDc_draw01));
    applyDrawEnvPairs(db + (one ? kDbDc_draw12 : kDbDc_draw02));
    const uint8_t* cl = db + (one ? kDbDc_clear1 : kDbDc_clear0);
    // hasSeededGsClearPacket (GS.cpp:63): rgbaq/xyz2a/xyz2b regs in place.
    uint64_t regQ = 0, regA = 0, regB = 0;
    std::memcpy(&regQ, cl + 40, 8);
    std::memcpy(&regA, cl + 56, 8);
    std::memcpy(&regB, cl + 72, 8);
    if (regQ == 0x01u && regA == 0x05u && regB == 0x05u) {
        uint64_t prim = 0, rgba = 0;
        std::memcpy(&prim, cl + 16, 8);
        std::memcpy(&rgba, cl + 32, 8);
        rtguest::gsClearFramebuffer(uint32_t((prim >> 9) & 1u), uint32_t(rgba));
        rtguest::gsClearFrameData();  // applyGsClearPacket's markFrameStart
        for (int i = 0; i < 6; ++i) {
            uint64_t value, reg;
            std::memcpy(&value, cl + i * 16u, 8);
            std::memcpy(&reg, cl + i * 16u + 8, 8);
            rtguest::gsWriteRegister(static_cast<uint8_t>(reg), value);
        }
    }
    return w ^ 1u;
}
RT_STUB_FN(0x0026F738, guest::sceGsSwapDBuffDc)

// ---- env struct builders: the game keeps these structs in its own data and
// reads them back; the oracle only forwards them to the GS, so returning
// success without touching them matches (GS.cpp:1050/1073/1116/908/807/820).
uint32_t sceGsSetDefDispEnv(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026F798, guest::sceGsSetDefDispEnv)

uint32_t sceGsPutDispEnv(uint32_t q0, ...) { ++g_envCalls.putDispEnv; return 0; }
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

uint32_t sceGsPutDrawEnv(uint32_t q0, ...) { ++g_envCalls.putDrawEnv; return 0; }
RT_STUB_FN(0x0026FE80, guest::sceGsPutDrawEnv)

uint32_t sceGsSetDefDrawEnv2(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4, uint32_t q5, ...) { (void)q1; return 0; }
RT_STUB_FN(0x0026FF68, guest::sceGsSetDefDrawEnv2)

}  // namespace guest
