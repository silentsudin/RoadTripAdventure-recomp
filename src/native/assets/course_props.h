#pragma once

// Parser for the Road Trip Adventure COURSE/*.BIN prop draw chains.
//
// Grammar proven in docs/native-reference/course-prop-chains.md: the tail
// region of a COURSE tile (starting at header word offC) is exactly one
// primary packet block of N prop draws. Each draw is one untextured,
// flat-shaded, alpha-blended GS tristrip (GIFtag PRIM 0x264, NREG=3,
// REGS=0x412) of NLOOP vertices, 3..50, carried by the VIF triplet
// UNPACK V4-32 NUM=1 (GIFtag) -> UNPACK V3-32 NUM=4*NLOOP (vertex units) ->
// MSCALF imm 0xA. Each 48-byte vertex unit bakes tile-local float (x, 0, z)
// plus fixed lanes (normal (0,1,0)/(0,0,1), colour (255,255,255),
// (0,0,alpha)) with a single alpha/normal flavor per course. Additional
// packet blocks after the primary block (C02/C03/C05/C13/C20) are skipped
// strictly via the header boundary list, as the document requires. C16
// carries a zero-length tail (offC == file size, boundary word 0) and parses
// to an empty chain.
//
// No game data ships here: callers pass a disc file buffer at run time.

#include "field_converter.h" // expected<>, GIFtag bit decode helpers

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::assets {

enum class CoursePropError : std::uint8_t {
    Ok = 0,
    Truncated,      // file too small for the declared structure
    BadHeader,      // unsupported header size, broken boundary list, bad offsets
    BadPrologue,    // prologue words != {0x30, 0x50, 0, N>=1}
    BadSetupRecord, // setup record A/B != the proven constant 8-tuple
    BadDmaChain,    // CNT/RET tag shape or QWC<->boundary arithmetic mismatch
    BadVifStream,   // wrong UNPACK/MSCALF/NOP grammar, or prologue N != stream
    BadGifTag,      // PRIM/PRE/EOP/FLG/NREG/REGS/NLOOP outside the proven shape
    BadVertexUnit,  // y != 0, x/z out of (0,1600), lane/alpha/flavor corrupt
};

// One prop vertex: tile-local x/z on the y == 0 ground plane with the GS
// 0..128-range alpha baked by the course flavor (128 = 50% ABE blend, 1 is
// the C14 opaque flavor). Position range proven corpus-wide: x, z in
// (0, 1600) — strictly inside one 1600-unit tile (world-geography.md).
struct CoursePropVertex {
    float x = 0.0f;
    float z = 0.0f;
    std::uint8_t alpha = 0; // 128 (flavor A) or 1 (flavor B)
};

// One prop draw: the decoded GIFtag plus its NLOOP-vertex tristrip.
struct CoursePropDraw {
    std::uint64_t gifTag = 0;      // low qword (NLOOP/EOP/PRE/PRIM/FLG/NREG)
    std::uint64_t gifRegs = 0;     // regs descriptor qword (== 0x412)
    std::uint32_t vertexCount = 0; // == GIFtag NLOOP, 3..50
    std::uint32_t tagOffset = 0;   // absolute byte offset of the GIFtag
    std::vector<CoursePropVertex> vertices;
};

// The parsed primary prop block of one COURSE tile: the header boundary
// list, the prologue draw count N, and every decoded draw. Extras are only
// counted; their payload is outside this grammar (doc: OPEN) and is never
// dereferenced.
struct CoursePropChain {
    std::uint32_t headerSize = 0;  // 0x20 or 0x30
    std::uint32_t tailBegin = 0;   // == header offC; start of the primary block
    std::uint32_t tailEnd = 0;     // == boundaries[0]; end of the primary block
    std::vector<std::uint32_t> boundaries;    // packet-block end offsets; last == file size
    std::uint32_t propCount = 0;              // prologue N (== draws.size())
    std::uint32_t extraBlockCount = 0;        // opaque blocks after the primary block
    std::array<float, 3> flavorNormal{};      // baked lane1: (0,1,0) or (0,0,1)
    float alpha = 0.0f;                       // baked course alpha: 128.0 or 1.0
    std::vector<CoursePropDraw> draws;
};

expected<CoursePropChain, CoursePropError>
parseCourseProps(std::span<const std::uint8_t> data);

} // namespace rt::native::assets
