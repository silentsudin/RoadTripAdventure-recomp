// W3 step 3b: native GS device model. Semantics transliterated with citations
// from the oracle host layer (never linked, decomp rule):
//   gs_frontend.cpp   writeRegisterUnlocked (register switch), writeRegisterPacked,
//                     vertexKick (needed/submit/trim), processImageData,
//                     writeRegister (reglist), TRXDIR/BeginTransfer
//   gs_cpu_backend.cpp BeginTransfer + UploadImage (host->local pixel copy)
//   ps2_gs_memory.h/.cpp PixelStorageTraits addressing + VRAM page layout tables
// The lookup-table DATA below is transliterated verbatim from
// ps2_gs_memory.cpp:45-205; the addressing is the same arithmetic inlined
// (PageId/BlockId/ColumnId), no page-lookup table materialized.
#include "guest_gs.h"

#include "guest_vif1.h"  // shares the RT_VIF_HIST census gate

#include <array>
#include <cstdio>
#include <cstring>
#include <map>

namespace rtguest {
namespace {

// ---------------------------------------------------------------- VRAM ----
constexpr uint32_t kVramSize = 4u << 20;  // the external 4 MiB GS allocation
alignas(64) uint8_t g_vram[kVramSize];

// Pixel storage modes (ps2_gs_memory.h PixelStorageMode).
enum : uint8_t {
    kPsmC32 = 0x00, kPsmC24 = 0x01, kPsmC16 = 0x02, kPsmC16S = 0x0A,
    kPsmP8 = 0x13, kPsmP4 = 0x14, kPsmP8H = 0x1B,
    kPsmP4HL = 0x24, kPsmP4HH = 0x2C,
    kPsmZ32 = 0x30, kPsmZ24 = 0x31, kPsmZ16 = 0x32, kPsmZ16S = 0x3A,
};

// Table type aliases matching the oracle's LookupTable<T, Width, Height>.
using C32BlockLookupTableT = std::array<std::array<uint8_t, 8>, 4>;
using Z32BlockLookupTableT = std::array<std::array<uint8_t, 8>, 4>;
using P8BlockLookupTable = std::array<std::array<uint8_t, 8>, 4>;
using C16BlockLookupTable = std::array<std::array<uint8_t, 4>, 8>;
using C16SBlockLookupTable = std::array<std::array<uint8_t, 4>, 8>;
using Z16BlockLookupTable = std::array<std::array<uint8_t, 4>, 8>;
using Z16SBlockLookupTable = std::array<std::array<uint8_t, 4>, 8>;
using P4BlockLookupTable = std::array<std::array<uint8_t, 4>, 8>;
using C32ColumnLookupTableT = std::array<std::array<uint16_t, 8>, 8>;
using C16ColumnLookupTable = std::array<std::array<uint16_t, 16>, 8>;
using P8ColumnLookupTable = std::array<std::array<uint16_t, 16>, 16>;
using P4ColumnLookupTable = std::array<std::array<uint16_t, 32>, 16>;

// ---- VRAM page layout tables, data transliterated from ps2_gs_memory.cpp:45-205
    static constexpr C32BlockLookupTableT BlockTableC32
    {{
        {  0,  1,  4,  5, 16, 17, 20, 21 },
        {  2,  3,  6,  7, 18, 19, 22, 23 },
        {  8,  9, 12, 13, 24, 25, 28, 29 },
        { 10, 11, 14, 15, 26, 27, 30, 31 }
    }};

    static constexpr Z32BlockLookupTableT BlockTableZ32
    {{
        { 24, 25, 28, 29,  8,  9, 12, 13 },
        { 26, 27, 30, 31, 10, 11, 14, 15 },
        { 16, 17, 20, 21,  0,  1,  4,  5 },
        { 18, 19, 22, 23,  2,  3,  6,  7 }
    }};

    static constexpr C16BlockLookupTable BlockTableC16
    {{
        {  0,  2,  8, 10 },
        {  1,  3,  9, 11 },
        {  4,  6, 12, 14 },
        {  5,  7, 13, 15 },
        { 16, 18, 24, 26 },
        { 17, 19, 25, 27 },
        { 20, 22, 28, 30 },
        { 21, 23, 29, 31 }
    }};

    static constexpr C16SBlockLookupTable BlockTableC16S
    {{
        {  0,  2, 16, 18 },
        {  1,  3, 17, 19 },
        {  8, 10, 24, 26 },
        {  9, 11, 25, 27 },
        {  4,  6, 20, 22 },
        {  5,  7, 21, 23 },
        { 12, 14, 28, 30 },
        { 13, 15, 29, 31 }
    }};

    static constexpr Z16BlockLookupTable BlockTableZ16
    {{
        { 24, 26, 16, 18 },
        { 25, 27, 17, 19 },
        { 28, 30, 20, 22 },
        { 29, 31, 21, 23 },
        {  8, 10,  0,  2 },
        {  9, 11,  1,  3 },
        { 12, 14,  4,  6 },
        { 13, 15,  5,  7 }
    }};

    static constexpr Z16SBlockLookupTable BlockTableZ16S
    {{
        { 24, 26,  8, 10 },
        { 25, 27,  9, 11 },
        { 16, 18,  0,  2 },
        { 17, 19,  1,  3 },
        { 28, 30, 12, 14 },
        { 29, 31, 13, 15 },
        { 20, 22,  4,  6 },
        { 21, 23,  5,  7 }
    }};

    static constexpr P8BlockLookupTable BlockTableP8
    {{
        {  0,  1,  4,  5, 16, 17, 20, 21},
        {  2,  3,  6,  7, 18, 19, 22, 23},
        {  8,  9, 12, 13, 24, 25, 28, 29},
        { 10, 11, 14, 15, 26, 27, 30, 31}
    }};

    static constexpr P4BlockLookupTable BlockTableP4
    {{
        {  0,  2,  8, 10 },
        {  1,  3,  9, 11 },
        {  4,  6, 12, 14 },
        {  5,  7, 13, 15 },
        { 16, 18, 24, 26 },
        { 17, 19, 25, 27 },
        { 20, 22, 28, 30 },
        { 21, 23, 29, 31 }
    }};

    // column layout is shared between c32 and z32
    static constexpr C32ColumnLookupTableT ColumnTable32
    {{
        {  0,  1,  4,  5,  8,  9, 12, 13 },
        {  2,  3,  6,  7, 10, 11, 14, 15 },
        { 16, 17, 20, 21, 24, 25, 28, 29 },
        { 18, 19, 22, 23, 26, 27, 30, 31 },
        { 32, 33, 36, 37, 40, 41, 44, 45 },
        { 34, 35, 38, 39, 42, 43, 46, 47 },
        { 48, 49, 52, 53, 56, 57, 60, 61 },
        { 50, 51, 54, 55, 58, 59, 62, 63 },
    }};

    static constexpr C16ColumnLookupTable ColumnTable16
    {{
        {   0,   2,   8,  10,  16,  18,  24,  26,   1,   3,   9,  11,  17,  19,  25,  27 },
        {   4,   6,  12,  14,  20,  22,  28,  30,   5,   7,  13,  15,  21,  23,  29,  31 },
        {  32,  34,  40,  42,  48,  50,  56,  58,  33,  35,  41,  43,  49,  51,  57,  59 },
        {  36,  38,  44,  46,  52,  54,  60,  62,  37,  39,  45,  47,  53,  55,  61,  63 },
        {  64,  66,  72,  74,  80,  82,  88,  90,  65,  67,  73,  75,  81,  83,  89,  91 },
        {  68,  70,  76,  78,  84,  86,  92,  94,  69,  71,  77,  79,  85,  87,  93,  95 },
        {  96,  98, 104, 106, 112, 114, 120, 122,  97,  99, 105, 107, 113, 115, 121, 123 },
        { 100, 102, 108, 110, 116, 118, 124, 126, 101, 103, 109, 111, 117, 119, 125, 127 }
    }};

    static constexpr P8ColumnLookupTable ColumnTable8
    {{
        {   0,   4,  16,  20,  32,  36,  48,  52,   2,   6,  18,  22,  34,  38,  50,  54 },
        {   8,  12,  24,  28,  40,  44,  56,  60,  10,  14,  26,  30,  42,  46,  58,  62 },
        {  33,  37,  49,  53,   1,   5,  17,  21,  35,  39,  51,  55,   3,   7,  19,  23 },
        {  41,  45,  57,  61,   9,  13,  25,  29,  43,  47,  59,  63,  11,  15,  27,  31 },
        {  96, 100, 112, 116,  64,  68,  80,  84,  98, 102, 114, 118,  66,  70,  82,  86 },
        { 104, 108, 120, 124,  72,  76,  88,  92, 106, 110, 122, 126,  74,  78,  90,  94 },
        {  65,  69,  81,  85,  97, 101, 113, 117,  67,  71,  83,  87,  99, 103, 115, 119 },
        {  73,  77,  89,  93, 105, 109, 121, 125,  75,  79,  91,  95, 107, 111, 123, 127 },
        { 128, 132, 144, 148, 160, 164, 176, 180, 130, 134, 146, 150, 162, 166, 178, 182 },
        { 136, 140, 152, 156, 168, 172, 184, 188, 138, 142, 154, 158, 170, 174, 186, 190 },
        { 161, 165, 177, 181, 129, 133, 145, 149, 163, 167, 179, 183, 131, 135, 147, 151 },
        { 169, 173, 185, 189, 137, 141, 153, 157, 171, 175, 187, 191, 139, 143, 155, 159 },
        { 224, 228, 240, 244, 192, 196, 208, 212, 226, 230, 242, 246, 194, 198, 210, 214 },
        { 232, 236, 248, 252, 200, 204, 216, 220, 234, 238, 250, 254, 202, 206, 218, 222 },
        { 193, 197, 209, 213, 225, 229, 241, 245, 195, 199, 211, 215, 227, 231, 243, 247 },
        { 201, 205, 217, 221, 233, 237, 249, 253, 203, 207, 219, 223, 235, 239, 251, 255 }
    }};

    static constexpr P4ColumnLookupTable ColumnTable4
    {{
        {   0,   8,  32,  40,  64,  72,  96, 104,   2,  10,  34,  42,  66,  74,  98, 106,   4,  12,  36,  44,  68,  76, 100, 108,   6,  14,  38,  46,  70,  78, 102, 110 },
        {  16,  24,  48,  56,  80,  88, 112, 120,  18,  26,  50,  58,  82,  90, 114, 122,  20,  28,  52,  60,  84,  92, 116, 124,  22,  30,  54,  62,  86,  94, 118, 126 },
        {  65,  73,  97, 105,   1,   9,  33,  41,  67,  75,  99, 107,   3,  11,  35,  43,  69,  77, 101, 109,   5,  13,  37,  45,  71,  79, 103, 111,   7,  15,  39,  47 },
        {  81,  89, 113, 121,  17,  25,  49,  57,  83,  91, 115, 123,  19,  27,  51,  59,  85,  93, 117, 125,  21,  29,  53,  61,  87,  95, 119, 127,  23,  31,  55,  63 },
        { 192, 200, 224, 232, 128, 136, 160, 168, 194, 202, 226, 234, 130, 138, 162, 170, 196, 204, 228, 236, 132, 140, 164, 172, 198, 206, 230, 238, 134, 142, 166, 174 },
        { 208, 216, 240, 248, 144, 152, 176, 184, 210, 218, 242, 250, 146, 154, 178, 186, 212, 220, 244, 252, 148, 156, 180, 188, 214, 222, 246, 254, 150, 158, 182, 190 },
        { 129, 137, 161, 169, 193, 201, 225, 233, 131, 139, 163, 171, 195, 203, 227, 235, 133, 141, 165, 173, 197, 205, 229, 237, 135, 143, 167, 175, 199, 207, 231, 239 },
        { 145, 153, 177, 185, 209, 217, 241, 249, 147, 155, 179, 187, 211, 219, 243, 251, 149, 157, 181, 189, 213, 221, 245, 253, 151, 159, 183, 191, 215, 223, 247, 255 },
        { 256, 264, 288, 296, 320, 328, 352, 360, 258, 266, 290, 298, 322, 330, 354, 362, 260, 268, 292, 300, 324, 332, 356, 364, 262, 270, 294, 302, 326, 334, 358, 366 },
        { 272, 280, 304, 312, 336, 344, 368, 376, 274, 282, 306, 314, 338, 346, 370, 378, 276, 284, 308, 316, 340, 348, 372, 380, 278, 286, 310, 318, 342, 350, 374, 382 },
        { 321, 329, 353, 361, 257, 265, 289, 297, 323, 331, 355, 363, 259, 267, 291, 299, 325, 333, 357, 365, 261, 269, 293, 301, 327, 335, 359, 367, 263, 271, 295, 303 },
        { 337, 345, 369, 377, 273, 281, 305, 313, 339, 347, 371, 379, 275, 283, 307, 315, 341, 349, 373, 381, 277, 285, 309, 317, 343, 351, 375, 383, 279, 287, 311, 319 },
        { 448, 456, 480, 488, 384, 392, 416, 424, 450, 458, 482, 490, 386, 394, 418, 426, 452, 460, 484, 492, 388, 396, 420, 428, 454, 462, 486, 494, 390, 398, 422, 430 },
        { 464, 472, 496, 504, 400, 408, 432, 440, 466, 474, 498, 506, 402, 410, 434, 442, 468, 476, 500, 508, 404, 412, 436, 444, 470, 478, 502, 510, 406, 414, 438, 446 },
        { 385, 393, 417, 425, 449, 457, 481, 489, 387, 395, 419, 427, 451, 459, 483, 491, 389, 397, 421, 429, 453, 461, 485, 493, 391, 399, 423, 431, 455, 463, 487, 495 },
        { 401, 409, 433, 441, 465, 473, 497, 505, 403, 411, 435, 443, 467, 475, 499, 507, 405, 413, 437, 445, 469, 477, 501, 509, 407, 415, 439, 447, 471, 479, 503, 511 },
    }};

    // ---- end transliterated tables ------------------------------------------------

// Per-PSM geometry (PixelStorageTraits<psm>::PageExtent/BlockExtent/
// ColumnExtent/UnpackedBitWidth/BitOffset + PackedT width + merge kind).
struct PsmGeom {
    uint16_t pageX, pageY;
    uint8_t blkX, blkY;       // blocks per page extent
    uint8_t colX, colY;       // pixels per block extent
    uint8_t unpackBits;       // UnpackedBitWidth
    uint8_t bitOffset;        // BitOffset
    uint8_t packedBytes;      // sizeof(PackedT)
    uint8_t merge;            // 0 copy, 1 24-in-32 RMW, 2 nibble RMW
    const void* blockTable;
    const void* columnTable;
};

const PsmGeom kGeoms[] = {
    // page     block  column  unpack off bytes merge  blockTbl        colTbl
    {64, 32, 8, 4, 8, 8, 32, 0, 4, 0, &BlockTableC32, &ColumnTable32},   // C32
    {64, 32, 8, 4, 8, 8, 32, 0, 4, 1, &BlockTableC32, &ColumnTable32},   // C24
    {64, 64, 4, 8, 16, 8, 16, 0, 2, 0, &BlockTableC16, &ColumnTable16},  // C16
    {64, 64, 4, 8, 16, 8, 16, 0, 2, 0, &BlockTableC16S, &ColumnTable16}, // C16S
    {64, 32, 8, 4, 8, 8, 32, 24, 1, 0, &BlockTableC32, &ColumnTable32},  // P8H (32-bit slots; oracle InitLookupTables has no P8H page table -> ReadTexture uses the C32 one)
    {128, 64, 8, 4, 16, 16, 8, 0, 1, 0, &BlockTableP8, &ColumnTable8},   // P8
    {128, 128, 4, 8, 32, 16, 4, 0, 1, 2, &BlockTableP4, &ColumnTable4},  // P4
    {64, 32, 8, 4, 8, 8, 32, 24, 1, 2, &BlockTableC32, &ColumnTable32},  // P4HL
    {64, 32, 8, 4, 8, 8, 32, 28, 1, 2, &BlockTableC32, &ColumnTable32},  // P4HH
    {64, 32, 8, 4, 8, 8, 32, 0, 4, 0, &BlockTableZ32, &ColumnTable32},   // Z32
    {64, 32, 8, 4, 8, 8, 32, 0, 4, 1, &BlockTableZ32, &ColumnTable32},   // Z24
    {64, 64, 4, 8, 16, 8, 16, 0, 2, 0, &BlockTableZ16, &ColumnTable16},  // Z16
    {64, 64, 4, 8, 16, 8, 16, 0, 2, 0, &BlockTableZ16S, &ColumnTable16}, // Z16S
};

int geomIndex(uint8_t psm) {
    switch (psm & 0x3Fu) {
    case kPsmC32: return 0;
    case kPsmC24: return 1;
    case kPsmC16: return 2;
    case kPsmC16S: return 3;
    case kPsmP8H: return 4;
    case kPsmP8: return 5;
    case kPsmP4: return 6;
    case kPsmP4HL: return 7;
    case kPsmP4HH: return 8;
    case kPsmZ32: return 9;
    case kPsmZ24: return 10;
    case kPsmZ16: return 11;
    case kPsmZ16S: return 12;
    default: return -1;
    }
}

// PixelStorageTraits<psm>::Address inlined: PageId + (block-in-page +
// BlockId(x,y)) * PixelsPerBlock + ColumnId(x,y).
uint32_t pixelAddress(const PsmGeom& g, uint32_t base, uint32_t bw, uint32_t x,
                      uint32_t y) {
    const uint32_t blocksPerPage = uint32_t(g.blkX) * g.blkY;
    const uint32_t pxPerBlock = uint32_t(g.colX) * g.colY;
    const uint32_t pxPerPage = uint32_t(g.pageX) * g.pageY;
    const uint32_t basePage = base / blocksPerPage;
    const uint32_t row = (y / g.pageY) * ((bw * 64u) / g.pageX);
    const uint32_t col = x / g.pageX;
    const uint64_t page = uint64_t(basePage) + row + col;
    const auto* bt = static_cast<const uint8_t*>(g.blockTable);
    const auto* ct = static_cast<const uint16_t*>(g.columnTable);
    const uint32_t bIdx =
        ((y / g.colY) % g.blkY) * g.blkX + ((x / g.colX) % g.blkX);
    const uint32_t cIdx = (y % g.colY) * g.colX + (x % g.colX);
    const uint32_t inPage =
        ((base % blocksPerPage) + bt[bIdx]) * pxPerBlock + ct[cIdx];
    return uint32_t(page * pxPerPage) + inPage;
}

// GSCpuBackend::WriteVramUnlocked + PixelStorageTraits::Write.
uint64_t g_vramWrites = 0;
void writeVram(uint8_t psm, uint32_t base, uint32_t bw, uint32_t x, uint32_t y,
               uint32_t value) {
    const int gi = geomIndex(psm);
    if (gi < 0) return;
    const PsmGeom& g = kGeoms[gi];
    const uint32_t addr = pixelAddress(g, base, bw, x, y);
    const uint64_t bits = uint64_t(addr) * g.unpackBits + g.bitOffset;
    const uint32_t byte = uint32_t(bits / 8) & (kVramSize - g.packedBytes);
    const uint32_t shift = uint32_t(bits % 8);
    uint8_t* ptr = g_vram + byte;
    switch (g.merge) {
    case 0:
        std::memcpy(ptr, &value, g.packedBytes);
        break;
    case 1: {  // C24/Z24: keep top byte
        uint32_t old;
        std::memcpy(&old, ptr, 4);
        value = (old & 0xFF000000u) | (value & 0x00FFFFFFu);
        std::memcpy(ptr, &value, 4);
        break;
    }
    default: {  // P4 family: nibble RMW
        const uint8_t old = *ptr;
        *ptr = uint8_t((old & ~(0x0Fu << shift)) | ((value & 0x0Fu) << shift));
        break;
    }
    }
    ++g_vramWrites;
}

// ------------------------------------------------------- register state ----
struct PrimBits {
    uint8_t type = 0;
    bool iip = false, tme = false, fge = false, abe = false, aa1 = false;
    bool fst = false, ctxt = false, fix = false;
};

// gs_frontend decodePrimRegister
PrimBits decodePrim(uint64_t value) {
    PrimBits p;
    p.type = uint8_t(value & 0x7u);
    p.iip = ((value >> 3) & 1u) != 0u;
    p.tme = ((value >> 4) & 1u) != 0u;
    p.fge = ((value >> 5) & 1u) != 0u;
    p.abe = ((value >> 6) & 1u) != 0u;
    p.aa1 = ((value >> 7) & 1u) != 0u;
    p.fst = ((value >> 8) & 1u) != 0u;
    p.ctxt = ((value >> 9) & 1u) != 0u;
    p.fix = ((value >> 10) & 1u) != 0u;
    return p;
}

struct GsCtx {  // gs_frontend GSDrawContext fields we model
    uint32_t fbp = 0, fbw = 0; uint8_t fpsm = 0; uint32_t fbmsk = 0;
    uint32_t zbp = 0; uint8_t zpsm = 0;
    uint32_t tbp0 = 0, cbp = 0;
    uint8_t tbw = 0, tpsm = 0, tw = 0, th = 0, tcc = 0, tfx = 0;
    uint8_t cpsm = 0, csm = 0, csa = 0, cld = 0;
    uint64_t clamp = 0, alpha = 0, test = 0, tex1 = 0, fba = 0;
    uint16_t ofx = 0, ofy = 0;
    uint16_t sc0x = 0, sc1x = 0, sc0y = 0, sc1y = 0;
};

uint64_t g_regs[0x100] = {};
GsCtx g_ctx[2];
PrimBits g_primRegister, g_prmodeRegister, g_prim;
bool g_prmodeCont = true;

uint8_t g_curR = 0, g_curG = 0, g_curB = 0, g_curA = 0;
float g_curQ = 1.0f, g_curS = 0.0f, g_curT = 0.0f;
uint16_t g_curU = 0, g_curV = 0;
uint8_t g_curFog = 0;
uint8_t g_fogR = 0, g_fogG = 0, g_fogB = 0;
uint64_t g_texclut = 0;

GsVertex g_vtxQueue[3] = {};  // kMaxVerts = 3
int g_vtxCount = 0, g_vtxIndex = 0;
uint64_t g_drawStateSerial = 0;

// BITBLT/transfer state (frontend m_bitbltbuf.. + cpu backend m_transferState).
struct TrxState {
    uint32_t sbp = 0, dbp = 0;
    uint8_t sbw = 0, dbw = 0, spsm = 0, dpsm = 0;
    uint16_t ssax = 0, ssay = 0, dsax = 0, dsay = 0;
    uint16_t rrw = 0, rrh = 0;
    uint32_t dir = 3;  // 3 = idle (backend convention)
    uint32_t totalPixels = 0, copiedPixels = 0, x = 0, y = 0;
};
TrxState g_trx;

std::vector<GsBatch> g_batches;
std::vector<GsStreamVertex> g_stream;
uint64_t g_batchesTotal = 0, g_streamTotal = 0;
uint64_t g_dispEnv[5] = {};  // pmode,smode2,dispfb1,display1,bgcolor (ctx2 same)

// census
std::map<uint32_t, uint64_t> g_regWrites;
std::map<uint32_t, uint64_t> g_trxDir;
std::map<uint32_t, uint64_t> g_kickProbe;  // [0]=kick drawing, [1]=kick !drawing, [2]=count<needed miss
uint64_t g_uploadPixels = 0, g_uploads = 0, g_undirSkipped = 0;

inline uint64_t le64(const uint8_t* p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return v;
}

GsDrawState snapshotState() {
    const GsCtx& c = g_ctx[g_prim.ctxt ? 1 : 0];
    GsDrawState s;
    s.fbp = c.fbp; s.fbw = c.fbw; s.fpsm = c.fpsm; s.fbmsk = c.fbmsk;
    s.zbp = c.zbp; s.zpsm = c.zpsm;
    s.tbp0 = c.tbp0; s.cbp = c.cbp; s.tbw = c.tbw; s.tpsm = c.tpsm;
    s.tw = c.tw; s.th = c.th; s.tcc = c.tcc; s.tfx = c.tfx;
    s.cpsm = c.cpsm; s.csa = c.csa; s.cld = c.cld;
    s.clamp = c.clamp; s.alpha = c.alpha; s.test = c.test; s.tex1 = c.tex1;
    s.ofx = c.ofx; s.ofy = c.ofy;
    s.sc0x = c.sc0x; s.sc1x = c.sc1x; s.sc0y = c.sc0y; s.sc1y = c.sc1y;
    return s;
}

void pushVertex(const GsVertex& v, bool kick) {
    g_vtxQueue[g_vtxCount % 3] = v;
    g_stream.push_back({v, g_prim.type, kick});
    ++g_streamTotal;
}

// gs_frontend GS::vertexKick (classifier/wide/skip/frame-skip parts are
// renderer-side; batch submit + queue trim transliterated).
void vertexKick(bool drawing) {
    ++g_vtxCount;
    ++g_vtxIndex;
    int needed;
    switch (g_prim.type) {
    case 0: needed = 1; break;              // POINT
    case 1: case 2: needed = 2; break;      // LINE, LINESTRIP
    case 3: case 4: case 5: needed = 3; break;  // TRI, TRISTRIP, TRIFAN
    case 6: needed = 2; break;              // SPRITE
    default: ++g_kickProbe[3]; return;
    }
    ++g_kickProbe[drawing ? 0u : 1u];
    if (g_vtxCount < needed) { ++g_kickProbe[2]; return; }
    if (drawing) {
        ++g_kickProbe[4];
        GsBatch b;
        b.primType = g_prim.type;
        b.ctx = g_prim.ctxt;
        b.vertexCount = uint8_t(needed < 3 ? needed : 3);
        for (int i = 0; i < b.vertexCount; ++i) b.verts[i] = g_vtxQueue[i];
        b.state = snapshotState();
        g_batches.push_back(b);
        ++g_batchesTotal;
    }
    switch (g_prim.type) {
    case 1: case 3: case 6:  // LINE, TRIANGLE, SPRITE (+ POINT falls below)
    case 0:
        g_vtxCount = 0;
        break;
    case 2:  // LINESTRIP
        g_vtxQueue[0] = g_vtxQueue[1];
        g_vtxCount = 1;
        break;
    case 4:  // TRISTRIP
        g_vtxQueue[0] = g_vtxQueue[1];
        g_vtxQueue[1] = g_vtxQueue[2];
        g_vtxCount = 2;
        break;
    case 5:  // TRIFAN
        g_vtxQueue[1] = g_vtxQueue[2];
        g_vtxCount = 2;
        break;
    default:
        g_vtxCount = 0;
        break;
    }
}

void beginTransfer() {
    // g_trx.dir was set from the TRXDIR register write before this call
    g_trx.x = g_trx.dsax;
    g_trx.y = g_trx.dsay;
    g_trx.totalPixels = uint32_t(g_trx.rrw) * g_trx.rrh;
    g_trx.copiedPixels = 0;
    // dir 1 (local->host) and 2 (local->local) are W14 (StoreImage/screen
    // copies); census counts them so the ledger shows what is still missing.
    ++g_trxDir[g_trx.dir];
}

// GSCpuBackend::UploadImage (host->local only; dir!=0 guards).
void uploadImage(const uint8_t* data, uint32_t sizeBytes) {
    if (!data || sizeBytes == 0u || g_trx.dir != 0u) { ++g_undirSkipped; return; }
    if (g_trx.rrw == 0u || g_trx.rrh == 0u || g_trx.totalPixels == 0u) return;
    const uint32_t dbp = g_trx.dbp, dbw = g_trx.dbw ? g_trx.dbw : 1u;
    const uint8_t dpsm = g_trx.dpsm;
    const uint32_t rrw = g_trx.rrw;
    const uint32_t dsax = g_trx.dsax;
    uint32_t offset = 0;
    auto advancePixel = [&](uint32_t count) {
        g_trx.copiedPixels =
            g_trx.totalPixels < g_trx.copiedPixels + count
                ? g_trx.totalPixels
                : g_trx.copiedPixels + count;
        if (g_trx.copiedPixels >= g_trx.totalPixels) {
            g_trx.dir = 3;
            g_trx.totalPixels = 0;
            return;
        }
        g_trx.x = dsax + (g_trx.copiedPixels % rrw);
        g_trx.y = g_trx.dsay + (g_trx.copiedPixels / rrw);
    };
    while (offset < sizeBytes && g_trx.dir == 0u) {
        switch (dpsm) {
        case kPsmC32:
        case kPsmZ32: {
            if (sizeBytes - offset < 4u) return;
            uint32_t v;
            std::memcpy(&v, data + offset, 4);
            writeVram(dpsm, dbp, dbw, g_trx.x, g_trx.y, v);
            offset += 4u;
            advancePixel(1u);
            break;
        }
        case kPsmC24:
        case kPsmZ24: {
            if (sizeBytes - offset < 3u) return;
            const uint32_t v = uint32_t(data[offset]) |
                               (uint32_t(data[offset + 1]) << 8u) |
                               (uint32_t(data[offset + 2]) << 16u);
            writeVram(dpsm, dbp, dbw, g_trx.x, g_trx.y, v);
            offset += 3u;
            advancePixel(1u);
            break;
        }
        case kPsmC16:
        case kPsmC16S:
        case kPsmZ16:
        case kPsmZ16S: {
            if (sizeBytes - offset < 2u) return;
            uint16_t v16;
            std::memcpy(&v16, data + offset, 2);
            writeVram(dpsm, dbp, dbw, g_trx.x, g_trx.y, v16);
            offset += 2u;
            advancePixel(1u);
            break;
        }
        case kPsmP8:
        case kPsmP8H:
            writeVram(dpsm, dbp, dbw, g_trx.x, g_trx.y, data[offset++]);
            advancePixel(1u);
            break;
        case kPsmP4:
        case kPsmP4HL:
        case kPsmP4HH: {
            const uint8_t packed = data[offset++];
            const uint32_t first = g_trx.copiedPixels;
            writeVram(dpsm, dbp, dbw, dsax + (first % rrw),
                      g_trx.dsay + (first / rrw), packed & 0x0Fu);
            if (first + 1u < g_trx.totalPixels)
                writeVram(dpsm, dbp, dbw, dsax + ((first + 1u) % rrw),
                          g_trx.dsay + ((first + 1u) / rrw),
                          (packed >> 4u) & 0x0Fu);
            const uint32_t take =
                g_trx.totalPixels - first < 2u ? g_trx.totalPixels - first : 2u;
            advancePixel(take);
            break;
        }
        default:
            return;
        }
        ++g_uploadPixels;
    }
    ++g_uploads;
}

}  // namespace

// --------------------------------------------------------------- public ----
uint64_t gsReg(uint8_t addr) { return g_regs[addr]; }
const uint8_t* gsVram() { return g_vram; }
uint32_t gsVramSize() { return kVramSize; }
const std::vector<GsBatch>& gsBatches() { return g_batches; }
void gsClearFrameData() { g_batches.clear(); g_stream.clear(); }
void gsSetDisplayEnv(const uint8_t* five) {
    for (int i = 0; i < 5; ++i) {
        uint64_t v;
        std::memcpy(&v, five + i * 8u, 8);
        g_dispEnv[i] = v;
    }
}
const uint64_t* gsDisplayEnv() { return g_dispEnv; }

// GSCpuBackend::ReadVramUnlocked analogue (packed read, for the fbmsk path).
uint32_t gsReadVramPsm(uint8_t psm, uint32_t base, uint32_t bw, uint32_t x,
                       uint32_t y) {
    const int gi = geomIndex(psm);
    if (gi < 0) return 0;
    const PsmGeom& g = kGeoms[gi];
    const uint32_t addr = pixelAddress(g, base, bw, x, y);
    const uint64_t bits = uint64_t(addr) * g.unpackBits + g.bitOffset;
    const uint32_t byte = uint32_t(bits / 8) & (kVramSize - g.packedBytes);
    uint32_t v = 0;
    std::memcpy(&v, g_vram + byte, g.packedBytes);
    return v >> uint32_t(bits % 8);
}

// GS::clearFramebufferContext + GSCpuBackend::ClearFramebuffer
// (gs_cpu_backend.cpp:1757): fill the scissor box over the frame buffer,
// FBMSK merge, FBA-up forces alpha opaque except CT24. fbp/zbp go through
// framePageBaseToBlock (ps2_gs_common.h:42: fbp << 5) exactly as the oracle.
uint64_t g_clears = 0;
void gsClearFramebuffer(uint32_t ctxIdx, uint32_t rgba) {
    const GsCtx& c = g_ctx[ctxIdx != 0u ? 1u : 0u];
    if (c.fbw == 0u) return;
    const uint32_t x0 = c.sc0x, x1 = x0 < c.sc1x ? c.sc1x : x0;
    const uint32_t y0 = c.sc0y, y1 = y0 < c.sc1y ? c.sc1y : y0;
    const uint8_t r = uint8_t(rgba), g8 = uint8_t(rgba >> 8),
                  b = uint8_t(rgba >> 16), a0 = uint8_t(rgba >> 24);
    const uint8_t a = ((c.fba & 1ull) != 0ull && c.fpsm != kPsmC24)
                          ? uint8_t(a0 | 0x80u) : a0;
    const uint32_t fbp = c.fbp << 5u, fbw = c.fbw ? c.fbw : 1u;
    if (c.fpsm == kPsmC32 || c.fpsm == kPsmC24) {
        const uint32_t source = uint32_t(r) | (uint32_t(g8) << 8) |
                                (uint32_t(b) << 16) | (uint32_t(a) << 24);
        for (uint32_t y = y0; y <= y1; ++y)
            for (uint32_t x = x0; x <= x1; ++x) {
                uint32_t pixel = source;
                if (c.fbmsk != 0u) {
                    const uint32_t old = gsReadVramPsm(c.fpsm, fbp, fbw, x, y);
                    pixel = (pixel & ~c.fbmsk) | (old & c.fbmsk);
                }
                writeVram(c.fpsm, fbp, fbw, x, y, pixel);
            }
    } else if (c.fpsm == kPsmC16 || c.fpsm == kPsmC16S) {
        // encodeFramePixelPSMCT16 (oracle gs_cpu_backend.cpp)
        const uint16_t source = uint16_t(((r >> 3) & 0x1Fu) |
                                         (((g8 >> 3) & 0x1Fu) << 5) |
                                         (((b >> 3) & 0x1Fu) << 10) |
                                         ((a >= 0x40u) ? 0x8000u : 0u));
        const uint16_t mask = uint16_t(c.fbmsk);
        for (uint32_t y = y0; y <= y1; ++y)
            for (uint32_t x = x0; x <= x1; ++x) {
                uint16_t pixel = source;
                if (mask != 0u)
                    pixel = uint16_t((pixel & ~mask) |
                                     (uint16_t(gsReadVramPsm(c.fpsm, fbp, fbw, x, y)) & mask));
                writeVram(c.fpsm, fbp, fbw, x, y, pixel);
            }
    }
    ++g_clears;
}
// Scanout geometry: DISPFB {fbp0-8,fbw9-14,psm15-19,dbx32-42,dby43-53},
// DISPLAY {dx,dy,magh23-26,magv27-28,dw32-43,dh44-54}; source rect =
// (dw+1)/(magh+1) x (dh+1)/(magv+1) at base+(dbx,dby), fbp<<5 blocks
// (framePageBaseToBlock). shortcut: CT32/CT24/CT16(S) only — the game's
// scanout psm set; other formats return bg-less black until needed.
bool gsScanoutRgba(std::vector<uint8_t>& out, uint32_t* outW, uint32_t* outH) {
    const uint64_t dispfb = g_dispEnv[2], display = g_dispEnv[3];
    if (dispfb == 0ull && display == 0ull) return false;
    const uint32_t fbw = uint32_t((dispfb >> 9) & 0x3Full);
    const uint8_t psm = uint8_t((dispfb >> 15) & 0x1Full);
    const uint32_t dbx = uint32_t((dispfb >> 32) & 0x7FFull);
    const uint32_t dby = uint32_t((dispfb >> 43) & 0x7FFull);
    const uint32_t magh = uint32_t((display >> 23) & 0xFull) + 1u;
    const uint32_t magv = uint32_t((display >> 27) & 0x3ull) + 1u;
    const uint32_t dw = uint32_t((display >> 32) & 0xFFFull);
    const uint32_t dh = uint32_t((display >> 44) & 0x7FFull);
    const uint32_t w = (dw + 1u) / magh, h = (dh + 1u) / magv;
    if (w == 0u || h == 0u || fbw == 0u) return false;
    const uint32_t base = (uint32_t(dispfb & 0x1FFull)) << 5u;
    out.assign(size_t(w) * size_t(h) * 4u, 0u);
    for (uint32_t y = 0; y < h; ++y)
        for (uint32_t x = 0; x < w; ++x) {
            const uint32_t v = gsReadVramPsm(psm, base, fbw, dbx + x, dby + y);
            uint8_t r, g, b, a;
            if (psm == kPsmC32 || psm == kPsmC24) {
                r = uint8_t(v); g = uint8_t(v >> 8); b = uint8_t(v >> 16);
                a = uint8_t(v >> 24);
            } else if (psm == kPsmC16 || psm == kPsmC16S) {
                const uint8_t r5 = uint8_t(v & 0x1F), g5 = uint8_t((v >> 5) & 0x1F),
                              b5 = uint8_t((v >> 10) & 0x1F);
                r = uint8_t((r5 << 3) | (r5 >> 2)); g = uint8_t((g5 << 3) | (g5 >> 2));
                b = uint8_t((b5 << 3) | (b5 >> 2)); a = (v & 0x8000u) ? 0x80u : 0u;
            } else {
                continue;
            }
            uint8_t* p = out.data() + (size_t(y) * w + x) * 4u;
            p[0] = r; p[1] = g; p[2] = b;
            p[3] = uint8_t(a >= 0x80u ? 0xFFu : a * 2u);  // GS alpha 0..128 -> 0..255
        }
    *outW = w; *outH = h;
    return true;
}

const std::vector<GsStreamVertex>& gsStream() { return g_stream; }
uint64_t gsBatchCount() { return g_batches.size(); }
bool gsCensusOn() { return vif1HistOn(); }

// gs_frontend GS::writeRegisterUnlocked (cases beyond the modelled set store
// the raw value only; LoadClut caching and survey/debug hooks omitted).
void gsWriteRegister(uint8_t regAddr, uint64_t value) {
    ++g_regWrites[regAddr];
    if (regAddr != 0x01 && regAddr != 0x02 && regAddr != 0x03 &&
        regAddr != 0x04 && regAddr != 0x05 && regAddr != 0x0A &&
        regAddr != 0x0C && regAddr != 0x0D)
        ++g_drawStateSerial;

    switch (regAddr) {
    case 0x00:  // PRIM
        g_primRegister = decodePrim(value);
        if (g_prmodeCont) {
            g_prim = g_primRegister;
        } else {
            g_prim.type = g_primRegister.type;
        }
        g_vtxCount = 0;
        g_vtxIndex = 0;
        break;
    case 0x01:  // RGBAQ
        g_curR = uint8_t(value & 0xFF);
        g_curG = uint8_t((value >> 8) & 0xFF);
        g_curB = uint8_t((value >> 16) & 0xFF);
        g_curA = uint8_t((value >> 24) & 0xFF);
        {
            uint32_t qBits = uint32_t((value >> 32) & 0xFFFFFFFF);
            std::memcpy(&g_curQ, &qBits, 4);
            if (g_curQ == 0.0f) g_curQ = 1.0f;
        }
        break;
    case 0x02: {  // ST
        uint32_t sBits = uint32_t(value & 0xFFFFFFFF);
        uint32_t tBits = uint32_t((value >> 32) & 0xFFFFFFFF);
        std::memcpy(&g_curS, &sBits, 4);
        std::memcpy(&g_curT, &tBits, 4);
        break;
    }
    case 0x03:  // UV
        g_curU = uint16_t(value & 0x3FFFu);
        g_curV = uint16_t((value >> 16) & 0x3FFFu);
        break;
    case 0x04:   // XYZF2
    case 0x0C: { // XYZF3
        GsVertex v;
        v.x = float(value & 0xFFFF) / 16.0f;
        v.y = float((value >> 16) & 0xFFFF) / 16.0f;
        v.z = float((value >> 32) & 0xFFFFFF);
        v.fog = uint8_t((value >> 56) & 0xFF);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        pushVertex(v, regAddr == 0x04);
        vertexKick(regAddr == 0x04);
        break;
    }
    case 0x05:   // XYZ2
    case 0x0D: { // XYZ3
        GsVertex v;
        v.x = float(value & 0xFFFF) / 16.0f;
        v.y = float((value >> 16) & 0xFFFF) / 16.0f;
        v.z = float((value >> 32) & 0xFFFFFFFF);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        v.fog = g_curFog;
        pushVertex(v, regAddr == 0x05);
        vertexKick(regAddr == 0x05);
        break;
    }
    case 0x06:   // TEX0_1
    case 0x07: { // TEX0_2
        GsCtx& t = g_ctx[regAddr == 0x07 ? 1 : 0];
        t.tbp0 = uint32_t(value & 0x3FFF);
        t.tbw = uint8_t((value >> 14) & 0x3F);
        t.tpsm = uint8_t((value >> 20) & 0x3F);
        t.tw = uint8_t((value >> 26) & 0xF);
        t.th = uint8_t((value >> 30) & 0xF);
        t.tcc = uint8_t((value >> 34) & 0x1);
        t.tfx = uint8_t((value >> 35) & 0x3);
        t.cbp = uint32_t((value >> 37) & 0x3FFF);
        t.cpsm = uint8_t((value >> 51) & 0xF);
        t.csm = uint8_t((value >> 55) & 0x1);
        t.csa = uint8_t((value >> 56) & 0x1F);
        t.cld = uint8_t((value >> 61) & 0x7);
        break;
    }
    case 0x08:   // CLAMP_1
    case 0x09:   // CLAMP_2
        g_ctx[regAddr == 0x09 ? 1 : 0].clamp = value;
        break;
    case 0x0A:  // FOG
        g_curFog = uint8_t((value >> 56) & 0xFF);
        break;
    case 0x14:   // TEX1_1
    case 0x15:   // TEX1_2
        g_ctx[regAddr == 0x15 ? 1 : 0].tex1 = value;
        break;
    case 0x16:   // TEX2_1
    case 0x17: { // TEX2_2
        GsCtx& t = g_ctx[regAddr == 0x17 ? 1 : 0];
        t.tpsm = uint8_t((value >> 20) & 0x3F);
        t.cbp = uint32_t((value >> 37) & 0x3FFF);
        t.cpsm = uint8_t((value >> 51) & 0xF);
        t.csm = uint8_t((value >> 55) & 0x1);
        t.csa = uint8_t((value >> 56) & 0x1F);
        t.cld = uint8_t((value >> 61) & 0x7);
        break;
    }
    case 0x18:   // XYOFFSET_1
    case 0x19: { // XYOFFSET_2
        GsCtx& c = g_ctx[regAddr == 0x19 ? 1 : 0];
        c.ofx = uint16_t(value & 0xFFFF);
        c.ofy = uint16_t((value >> 32) & 0xFFFF);
        break;
    }
    case 0x1A: {  // PRMODECONT
        g_prmodeCont = (value & 1) != 0;
        const uint8_t type = g_primRegister.type;
        g_prim = g_prmodeCont ? g_primRegister : g_prmodeRegister;
        g_prim.type = type;
        break;
    }
    case 0x1B: {  // PRMODE
        g_prmodeRegister = decodePrim(value);
        if (!g_prmodeCont) {
            const uint8_t type = g_primRegister.type;
            g_prim = g_prmodeRegister;
            g_prim.type = type;
        }
        break;
    }
    case 0x1C:  // TEXCLUT
        g_texclut = value;
        break;
    case 0x40:   // SCISSOR_1
    case 0x41: { // SCISSOR_2
        GsCtx& c = g_ctx[regAddr == 0x41 ? 1 : 0];
        c.sc0x = uint16_t(value & 0x7FF);
        c.sc1x = uint16_t((value >> 16) & 0x7FF);
        c.sc0y = uint16_t((value >> 32) & 0x7FF);
        c.sc1y = uint16_t((value >> 48) & 0x7FF);
        break;
    }
    case 0x42:   // ALPHA_1
    case 0x43:   // ALPHA_2
        g_ctx[regAddr == 0x43 ? 1 : 0].alpha = value;
        break;
    case 0x47:   // TEST_1
    case 0x48:   // TEST_2
        g_ctx[regAddr == 0x48 ? 1 : 0].test = value;
        break;
    case 0x4C:   // FRAME_1
    case 0x4D: { // FRAME_2
        GsCtx& c = g_ctx[regAddr == 0x4D ? 1 : 0];
        c.fbp = uint32_t(value & 0x1FF);
        c.fbw = uint32_t((value >> 16) & 0x3F);
        c.fpsm = uint8_t((value >> 24) & 0x3F);
        c.fbmsk = uint32_t((value >> 32) & 0xFFFFFFFF);  // gs_frontend.cpp:1613
        break;
    }
    case 0x4E:   // ZBUF_1
    case 0x4F: { // ZBUF_2
        GsCtx& c = g_ctx[regAddr == 0x4F ? 1 : 0];
        c.zbp = uint32_t(value & 0x1FF);
        c.zpsm = uint8_t(((value >> 24) & 0xF) | 0x30);
        break;
    }
    case 0x4A:   // FBA_1
    case 0x4B:   // FBA_2
        g_ctx[regAddr == 0x4B ? 1 : 0].fba = value;
        break;
    case 0x50:  // BITBLTBUF
        g_trx.sbp = uint32_t(value & 0x3FFF);
        g_trx.sbw = uint8_t((value >> 16) & 0x3F);
        g_trx.spsm = uint8_t((value >> 24) & 0x3F);
        g_trx.dbp = uint32_t((value >> 32) & 0x3FFF);
        g_trx.dbw = uint8_t((value >> 48) & 0x3F);
        g_trx.dpsm = uint8_t((value >> 56) & 0x3F);
        break;
    case 0x51:  // TRXPOS
        g_trx.ssax = uint16_t(value & 0x7FF);
        g_trx.ssay = uint16_t((value >> 16) & 0x7FF);
        g_trx.dsax = uint16_t((value >> 32) & 0x7FF);
        g_trx.dsay = uint16_t((value >> 48) & 0x7FF);
        break;
    case 0x52:  // TRXREG
        g_trx.rrw = uint16_t(value & 0xFFF);
        g_trx.rrh = uint16_t((value >> 32) & 0xFFF);
        break;
    case 0x53:  // TRXDIR: activate the transfer
        g_trx.dir = uint32_t(value & 0x3);
        beginTransfer();
        break;
    case 0x54: {  // HWREG: 8 bytes of pixel data
        uint8_t buf[8];
        std::memcpy(buf, &value, 8);
        uploadImage(buf, 8);
        break;
    }
    case 0x3D:  // FOGCOL
        g_fogR = uint8_t(value & 0xFFu);
        g_fogG = uint8_t((value >> 8) & 0xFFu);
        g_fogB = uint8_t((value >> 16) & 0xFFu);
        break;
    default:
        break;  // raw store only
    }
    g_regs[regAddr] = value;
}

// gs_frontend GS::writeRegisterPacked (cases transliterated).
void gsWritePacked(uint8_t regDesc, uint64_t lo, uint64_t hi) {
    switch (regDesc) {
    case 0x00:
        gsWriteRegister(0x00, lo & 0x7FF);
        break;
    case 0x01:  // RGBAQ packed: no Q (it comes with ST)
        g_curR = uint8_t(lo & 0xFF);
        g_curG = uint8_t((lo >> 32) & 0xFF);
        g_curB = uint8_t(hi & 0xFF);
        g_curA = uint8_t((hi >> 32) & 0xFF);
        break;
    case 0x02: {  // STQ
        uint32_t sBits = uint32_t(lo & 0xFFFFFFFF);
        uint32_t tBits = uint32_t((lo >> 32) & 0xFFFFFFFF);
        uint32_t qBits = uint32_t(hi & 0xFFFFFFFF);
        std::memcpy(&g_curS, &sBits, 4);
        std::memcpy(&g_curT, &tBits, 4);
        std::memcpy(&g_curQ, &qBits, 4);
        if (g_curQ == 0.0f) g_curQ = 1.0f;
        break;
    }
    case 0x03:  // UV
        g_curU = uint16_t(lo & 0x3FFFu);
        g_curV = uint16_t((lo >> 32) & 0x3FFFu);
        break;
    case 0x04: {  // XYZF2 quad; ADK = fog-word bit 15 (game repurposes it)
        const uint16_t x = uint16_t(lo & 0xFFFF);
        const uint16_t y = uint16_t((lo >> 32) & 0xFFFF);
        const uint32_t z = uint32_t((hi >> 4) & 0xFFFFFF);
        const uint8_t f = uint8_t((hi >> 36) & 0xFF);
        const bool adk = ((hi >> 47) & 1) != 0;
        GsVertex v;
        v.x = float(x) / 16.0f;
        v.y = float(y) / 16.0f;
        v.z = float(z);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        v.fog = f;
        pushVertex(v, !adk);
        vertexKick(!adk);
        break;
    }
    case 0x05: {  // XYZ2 quad
        const uint16_t x = uint16_t(lo & 0xFFFF);
        const uint16_t y = uint16_t((lo >> 32) & 0xFFFF);
        const uint32_t z = uint32_t(hi & 0xFFFFFFFF);
        const bool adk = ((hi >> 47) & 1) != 0;
        GsVertex v;
        v.x = float(x) / 16.0f;
        v.y = float(y) / 16.0f;
        v.z = float(z);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        v.fog = g_curFog;
        pushVertex(v, !adk);
        vertexKick(!adk);
        break;
    }
    case 0x0A:  // FOG packed
        g_curFog = uint8_t((hi >> 36) & 0xFF);
        break;
    case 0x0C: {  // XYZF3 packed: no kick
        GsVertex v;
        v.x = float(lo & 0xFFFF) / 16.0f;
        v.y = float((lo >> 32) & 0xFFFF) / 16.0f;
        v.z = float((hi >> 4) & 0xFFFFFF);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        v.fog = uint8_t((hi >> 36) & 0xFF);
        pushVertex(v, false);
        vertexKick(false);
        break;
    }
    case 0x0D: {  // XYZ3 packed: no kick
        GsVertex v;
        v.x = float(lo & 0xFFFF) / 16.0f;
        v.y = float((lo >> 32) & 0xFFFF) / 16.0f;
        v.z = float(hi & 0xFFFFFFFF);
        v.r = g_curR; v.g = g_curG; v.b = g_curB; v.a = g_curA;
        v.q = g_curQ; v.s = g_curS; v.t = g_curT;
        v.u = g_curU; v.v = g_curV;
        v.fog = g_curFog;
        pushVertex(v, false);
        vertexKick(false);
        break;
    }
    case 0x0E:  // DIRECT: addr in hi 7:0, value in lo
        gsWriteRegister(uint8_t(hi & 0xFF), lo);
        break;
    case 0x0F:  // AD: no state in this model
        break;
    default:  // TEX0/CLAMP/... codes pass through as register addresses
        gsWriteRegister(regDesc, lo);
        break;
    }
}

// gs_frontend GS::processImageData (host-transfer bit accounting omitted).
void gsImageTransfer(const uint8_t* data, uint32_t sizeBytes) {
    uploadImage(data, sizeBytes);
}

void gsCensusDump() {
    std::fprintf(stderr,
                 "[gscensus] batches=%llu batchesTotal=%llu streamTotal=%llu "
                 "vertsQueued=%llu vramWrites=%llu uploadPx=%llu uploads=%llu clears=%llu\n",
                 (unsigned long long)g_batches.size(),
                 (unsigned long long)g_batchesTotal, (unsigned long long)g_streamTotal,
                 (unsigned long long)g_vtxIndex,
                 (unsigned long long)g_vramWrites,
                 (unsigned long long)g_uploadPixels,
                 (unsigned long long)g_uploads,
                 (unsigned long long)g_clears);
    for (auto& [d, c] : g_trxDir)
        std::fprintf(stderr, "  trxdir=%u n=%llu\n", d, (unsigned long long)c);
    for (auto& [k, c] : g_kickProbe) {
        static const char* kName[] = {"drawKick", "noDrawKick", "belowNeeded",
                                      "badPrim", "submitted"};
        std::fprintf(stderr, "  kick %s=%llu\n",
                     k < 5 ? kName[k] : "?", (unsigned long long)c);
    }
    for (auto& [a, c] : g_regWrites)
        std::fprintf(stderr, "  wreg=%#x n=%llu\n", a, (unsigned long long)c);
    std::fprintf(stderr, "  ctx0 fbp=%u fbw=%u fpsm=%u zbp=%u zpsm=%u tbp0=%u tpsm=%u\n",
                 g_ctx[0].fbp, g_ctx[0].fbw, g_ctx[0].fpsm, g_ctx[0].zbp,
                 g_ctx[0].zpsm, g_ctx[0].tbp0, g_ctx[0].tpsm);
    std::fprintf(stderr, "  disp pmode=%llx smode2=%llx dispfb1=%llx display1=%llx bgcolor=%llx\n",
                 (unsigned long long)g_dispEnv[0], (unsigned long long)g_dispEnv[1],
                 (unsigned long long)g_dispEnv[2], (unsigned long long)g_dispEnv[3],
                 (unsigned long long)g_dispEnv[4]);
    std::fprintf(stderr, "  ctx1 fbp=%u fbw=%u fpsm=%u zbp=%u zpsm=%u tbp0=%u tpsm=%u\n",
                 g_ctx[1].fbp, g_ctx[1].fbw, g_ctx[1].fpsm, g_ctx[1].zbp,
                 g_ctx[1].zpsm, g_ctx[1].tbp0, g_ctx[1].tpsm);
}

}  // namespace rtguest
