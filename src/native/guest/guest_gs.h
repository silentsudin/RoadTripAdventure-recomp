#ifndef RTGUEST_GUEST_GS_H
#define RTGUEST_GUEST_GS_H
// W3 step 3b: native GS device model (docs/native-reference/w13-gs-sink-design.md).
// Register-file, vertex/batch pipeline, VRAM and IMAGE-transfer semantics mirror
// the oracle gs_frontend.cpp (writeRegisterUnlocked/writeRegisterPacked/
// vertexKick/processImageData) + GSCpuBackend (BeginTransfer/UploadImage) +
// ps2_gs_memory.{h,cpp} (VRAM page addressing). The oracle is the reference and
// is NOT linked (decomp rule).
#include <cstdint>
#include <vector>

namespace rtguest {

// Vertex as the queue carries it (gs_frontend GSVertex fields we model).
struct GsVertex {
    float x, y, z;
    uint8_t r, g, b, a;
    float q, s, t;
    uint16_t u, v;
    uint8_t fog;
};

// Draw-relevant state snapshot at batch time (buildDrawBatch analogue).
struct GsDrawState {
    uint32_t fbp, fbw; uint8_t fpsm; uint32_t fbmsk;
    uint32_t zbp; uint8_t zpsm;
    uint32_t tbp0, cbp; uint8_t tbw, tpsm, tw, th, tcc, tfx, cpsm, csa, cld;
    uint64_t clamp, alpha, test, tex1;
    uint16_t ofx, ofy;
    uint16_t sc0x, sc1x, sc0y, sc1y;
    // 3c-2 rasterizer fields (gs_frontend buildDrawBatch: prim flags, TEXA,
    // FBA, ZMSK, CSM, fog colour, texture size and filter derived here).
    bool iip, tme, fst, fge, abe, pabe, linear;
    uint8_t zmask, csm;
    uint8_t fogR, fogG, fogB;
    uint64_t texa, fba;
    uint16_t texW, texH;
};

struct GsBatch {
    uint8_t primType;   // PRIM bits 2:0
    bool ctx;           // PRIM ctxt (context 0/1)
    uint8_t vertexCount;
    GsVertex verts[3];
    GsDrawState state;
};

// Every vertex write in stream order (the pgs packet-mirror analogue: the
// oracle front end batches only ~2.7k/boot; the game's XGKICK geometry flows
// as PATH1 packets that parallel-gs decodes itself — the native renderer must
// see the full stream, not just the batch windows).
struct GsStreamVertex {
    GsVertex v;
    uint8_t primType;
    bool kick;  // drawing flag at this write (ADK clear / XYZF2)
};

void gsWriteRegister(uint8_t regAddr, uint64_t value);  // writeRegisterUnlocked
void gsWritePacked(uint8_t regDesc, uint64_t lo, uint64_t hi);  // writeRegisterPacked
void gsImageTransfer(const uint8_t* data, uint32_t sizeBytes);  // processImageData -> UploadImage

// Display environment (applyGsDispEnv): {pmode,smode2,dispfb,display,bgcolor}
// u64 x5 — scanout config, not the draw register file.
void gsSetDisplayEnv(const uint8_t* five);
const uint64_t* gsDisplayEnv();

// GS::clearFramebufferContext + GSCpuBackend::ClearFramebuffer (gs_cpu_backend
// .cpp:1757): fill the context's scissor box over its frame buffer.
void gsClearFramebuffer(uint32_t ctxIdx, uint32_t rgba);
uint32_t gsReadVramPsm(uint8_t psm, uint32_t base, uint32_t bw, uint32_t x,
                       uint32_t y);  // ReadVramUnlocked analogue

// W3 3c-1 scanout readback: convert the applied display env's FB rect
// (DISPFB base/dbx/dby + DISPLAY (dw+1)/(magh+1) x (dh+1)/(magv+1)) to RGBA8
// rows. Oracle scanout geometry (gs_pgs_backend + framePageBaseToBlock);
// compositing/bgcolor stay with the golden grader.
bool gsScanoutRgba(std::vector<uint8_t>& out, uint32_t* w, uint32_t* h,
                   int fbpOverride = -1);

uint64_t gsReg(uint8_t addr);              // raw register file
const uint8_t* gsVram(); uint32_t gsVramSize();
void gsDebugDumpState(const char* path);  // GS text dump seam
const std::vector<GsBatch>& gsBatches();
const std::vector<GsStreamVertex>& gsStream();
void gsClearFrameData();  // after a frame's batches+stream were consumed

bool gsCensusOn();
void gsCensusDump();
uint64_t gsBatchCount();

// W3 3c-2/3c-3: rasterize the frame's batches into VRAM (GSCpuBackend: every
// modelled prim type now draws inline at the kick; the drain counts strays).
void gsRenderBatches();
void drawSpriteNative(const GsBatch& b);   // inline kick draw (3c-2)
void drawTriangleNative(const GsBatch& b); // inline kick draw (3c-3)
void drawLineNative(const GsBatch& b);
void drawPointNative(const GsBatch& b);

}  // namespace rtguest
#endif
