// gs_test: W3 step 3b pins for the native GS model (guest_gs.cpp) with
// synthetic register writes/packets: BITBLT IMAGE uploads into VRAM
// (CT32 + P4 addressing), the vertex queue/batch pipeline (TRISTRIP trim,
// ADK no-draw kicks) and the raw register file.
#include "guest/guest_gs.h"

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

int main() {
    assert(gsVramSize() == 4u * 1024u * 1024u);
    testUploadCt32();
    testUploadP4();
    testStripBatches();
    testRawRegFile();
    std::printf("gs_test: all pins OK\n");
    return 0;
}
