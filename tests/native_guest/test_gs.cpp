// gs_test: W3 step 3b pins for the native GS model (guest_gs.cpp) with
// synthetic register writes/packets: BITBLT IMAGE uploads into VRAM
// (CT32 + P4 addressing), the vertex queue/batch pipeline (TRISTRIP trim,
// ADK no-draw kicks) and the raw register file.
#include "guest/guest_gs.h"
#include "guest/guest_shim.h"  // rtguest::G for the env struct at 0x335fb0

#include <cassert>
#include <cstdio>
#include <cstring>

using namespace rtguest;

static void testUploadCt32() {
    gsWriteRegister(0x50, (uint64_t(1) << 48) | uint64_t(0));  // dbp=0 dbw=1 dpsm=C32
    gsWriteRegister(0x51, 0);                                  // dsax=dsay=0
    gsWriteRegister(0x52, 2ull | (2ull << 32));                // rrw=2 rrh=2
    gsWriteRegister(0x53, 0);                                  // activate host->local
    const uint8_t px[16] = {0xDD, 0xCC, 0xBB, 0xAA,  // (0,0) -> addr 0
                            0x44, 0x33, 0x22, 0x11,  // (1,0) -> addr 1
                            0x88, 0x77, 0x66, 0x55,  // (0,1) -> addr 2
                            0xCC, 0xBB, 0xAA, 0x99};  // (1,1) -> addr 3
    gsImageTransfer(px, 16);
    assert(std::memcmp(gsVram(), px, 16) == 0);  // CT32 column order is linear here
    // Transfer consumed: a second IMAGE with no active transfer must not write.
    const uint8_t junk[16] = {0xFF, 0xFF, 0xFF, 0xFF};
    gsImageTransfer(junk, 16);
    assert(gsVram()[0] == 0xDD);
}

static void testUploadP4() {
    gsWriteRegister(0x50, (uint64_t(1) << 56) | (uint64_t(1) << 32) |
                              uint64_t(1));  // dbp=1 dpsm=P4 dbw=1
    gsWriteRegister(0x51, 0);
    gsWriteRegister(0x52, 4ull | (1ull << 32));  // 4x1
    gsWriteRegister(0x53, 0);
    const uint8_t pair[2] = {0x21, 0x43};  // px0=1 px1=2 px2=3 px3=4
    gsImageTransfer(pair, 2);
    // ColumnTable4 row0 = {0,8,32,40,...}: unpacked nibble slots at
    // bytes 0,4,16,20 of the block (dbp=1 -> block 1 of the page).
    const uint8_t* v = gsVram();
    assert((v[256 + 0] & 0xFu) == 1u);
    assert((v[256 + 4] & 0xFu) == 2u);
    assert((v[256 + 16] & 0xFu) == 3u);
    assert((v[256 + 20] & 0xFu) == 4u);
}

static uint64_t xyz2(uint16_t x, uint16_t y, uint32_t z) {
    return uint64_t(x) | (uint64_t(y) << 16) | (uint64_t(z) << 32);
}

static void testStripBatches() {
    gsWriteRegister(0x4C, 1ull | (10ull << 16) | (3ull << 24));  // FRAME: fbp=1 fbw=10 psm=3
    gsWriteRegister(0x00, 4u | (1u << 4));  // TRISTRIP, TME
    gsClearFrameData();
    // Three XYZ2 writes (reglist route: kick on XYZ2) -> one batch.
    gsWriteRegister(0x01, 0x11223344ull);          // RGBAQ r=0x44 g=0x33 b=0x22 a=0x11
    gsWriteRegister(0x05, xyz2(160, 32, 5));       // x = 10.0f
    assert(gsBatches().empty());                   // 1 vertex: nothing yet
    gsWriteRegister(0x05, xyz2(480, 64, 6));
    assert(gsBatches().empty());                   // 2 vertices
    gsWriteRegister(0x05, xyz2(800, 96, 7));
    assert(gsBatches().size() == 1);
    const GsBatch& b = gsBatches()[0];
    assert(b.primType == 4 && b.vertexCount == 3);
    assert(b.verts[0].x == 10.0f && b.verts[2].x == 50.0f);
    assert(b.verts[0].r == 0x44 && b.verts[0].fog == 0);
    assert(b.state.fbp == 1 && b.state.fbw == 10 && b.state.fpsm == 3);
    // Fourth vertex: strip trim slides the window.
    gsWriteRegister(0x05, xyz2(1120, 128, 8));
    assert(gsBatches().size() == 2);
    const GsBatch& b2 = gsBatches()[1];
    assert(b2.verts[0].x == 30.0f && b2.verts[1].x == 50.0f && b2.verts[2].x == 70.0f);
    // Packed XYZF2 with the ADK/fog-word flag set: queued but no batch.
    const uint64_t lo = 160ull | (16ull << 32);
    const uint64_t hi = (5ull << 4) | (0xFFull << 36) | (1ull << 47);
    const size_t before = gsBatches().size();
    gsWritePacked(0x04, lo, hi);
    assert(gsBatches().size() == before);
    // Same quad with the flag clear: closes a triangle from the slid window.
    gsWritePacked(0x04, lo, hi & ~(1ull << 47));
    assert(gsBatches().size() == before + 1);
    const GsBatch& b3 = gsBatches()[before];
    assert(b3.verts[2].x == 10.0f && b3.verts[2].fog == 0xFF);
}

static void testRawRegFile() {
    gsWriteRegister(0x42, 0x123456789ABCull);  // ALPHA_1 raw
    assert(gsReg(0x42) == 0x123456789ABCull);
    gsWritePacked(0x0E, 0x77ull, 0x47ull);  // DIRECT: TEST_1 = 0x77
    assert(gsReg(0x47) == 0x77ull);
}

// ---- 3b-2: the libgs env seed chain, oracle-faithful. ----
namespace guest {  // host stubs (guest_libs_gs.cpp), called directly here
uint32_t sceGsResetGraph(uint32_t, uint32_t, uint32_t, uint32_t, ...);
uint32_t sceGsSetDefDBuffDc(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                            uint32_t, uint32_t, ...);
uint32_t sceGsSwapDBuffDc(uint32_t, uint32_t, ...);
}  // namespace guest

static uint64_t rd(const uint8_t* p, uint32_t off) {
    uint64_t v;
    std::memcpy(&v, p + off, 8);
    return v;
}

static void testDbuffDcSeed() {
    // The game's ResetGraph(0, interlace=1, omode=2, ffmode=1) then
    // SetDefDBuffDc(0x335fb0, psm=0 CT32, 640, 224, ztest=2, zpsm=1, clear=1);
    // expected bytes = the oracle dump @0x335fb0 (eerm_o1800).
    assert(guest::sceGsResetGraph(0, 1, 2, 1) == 0);
    assert(guest::sceGsSetDefDBuffDc(0x335fb0u, 0u, 640u, 224u, 2u, 1u, 1u) == 0);
    const uint8_t* e = static_cast<const uint8_t*>(rtguest::G(0x335fb0u));
    assert(rd(e, 0) == 0x8007ull && rd(e, 8) == 3ull);
    assert(rd(e, 16) == 0x1446ull);                       // dispfb0 fbp=70 fbw=10
    assert(rd(e, 24) == 0xdf9ff0182027call);              // display
    assert(rd(e, 32) == 0ull && rd(e, 56) == 0x1400ull);  // dispfb1 fbp=0
    assert(rd(e, 80) == 0x1000000000008016ull && rd(e, 88) == 0xEull);
    assert(rd(e, 96) == 0xA0000ull && rd(e, 104) == 0x4Cull);   // FRAME_1
    assert(rd(e, 112) == 0x0100008Cull && rd(e, 120) == 0x4Eull);  // ZBUF zbp=140
    assert(rd(e, 128) == 0x790000006C00ull && rd(e, 136) == 0x18ull);
    assert(rd(e, 144) == 0xDF0000027F0000ull && rd(e, 152) == 0x40ull);
    assert(rd(e, 160) == 1ull && rd(e, 168) == 0x1Aull);
    assert(rd(e, 192) == 0ull && rd(e, 200) == 0x45ull);  // DTHE off
    assert(rd(e, 208) == 0x50000ull && rd(e, 216) == 0x47ull);
    assert(rd(e, 232) == 0x4Dull && rd(e, 248) == 0x4Full);  // ctx2 pair set
    assert(rd(e, 344) == 0x48ull);
    assert(rd(e, 464) == 0xA0046ull && rd(e, 592) == 0xA0046ull);  // fb1 frames
    assert(rd(e, 352) == 0x30000ull && rd(e, 360) == 0x47ull);     // clear0 TEST
    assert(rd(e, 368) == 6ull && rd(e, 376) == 0ull);              // SPRITE
    assert(rd(e, 384) == 0ull && rd(e, 392) == 1ull);              // RGBAQ
    assert(rd(e, 400) == 0x79006C00ull && rd(e, 416) == 0x87009400ull);  // XYZ2s
    assert(rd(e, 432) == 0x50000ull);
    assert(rd(e, 736) == 0x206ull);   // clear1 SPRITE|ctx2
    assert(rd(e, 712) == 0x48ull);
}

static void testDbuffDcClearFill() {
    // swap which=0 applies draw01/02 + the clear packet: FB fill of frame 0
    // (FBP raw 0 -> blocks 0) with the struct's RGBAQ; FBMSK merges old bits.
    uint8_t* e = static_cast<uint8_t*>(rtguest::G(0x335fb0u));
    const uint64_t white = 0x80FFFFFFull;
    std::memcpy(e + 384, &white, 8);
    assert(guest::sceGsSwapDBuffDc(0x335fb0u, 0u) == 1u);
    assert(gsReadVramPsm(0x00, 0u, 10u, 10u, 20u) == 0x80FFFFFFu);
    assert(gsReadVramPsm(0x00, 0u, 10u, 639u, 223u) == 0x80FFFFFFu);
    // which=1: draw11 FRAME fbp 0x46 -> blocks 0x46<<5 (page base, oracle
    // framePageBaseToBlock) filled too.
    assert(guest::sceGsSwapDBuffDc(0x335fb0u, 1u) == 0u);
    assert(gsReadVramPsm(0x00, 0x46u << 5u, 10u, 10u, 20u) == 0x80FFFFFFu);
    // FBMSK = 0x00FF0000 keeps the old blue from the fill, new green/red in.
    const uint64_t fbmskFrame = 0xA0000ull | (0x00FF0000ull << 32);
    std::memcpy(e + 96, &fbmskFrame, 8);
    const uint64_t src = 0x80112233ull;
    std::memcpy(e + 384, &src, 8);
    assert(guest::sceGsSwapDBuffDc(0x335fb0u, 0u) == 1u);
    assert(gsReadVramPsm(0x00, 0u, 10u, 10u, 20u) == 0x8011FF33u);
}

int main() {
    assert(gsVramSize() == 4u * 1024u * 1024u);
    testUploadCt32();
    testUploadP4();
    testStripBatches();
    testRawRegFile();
    testDbuffDcSeed();
    testDbuffDcClearFill();
    std::printf("gs_test: all pins OK\n");
    return 0;
}
