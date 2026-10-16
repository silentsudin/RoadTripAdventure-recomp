#include "course_props.h"

#include <array>
#include <bit>
#include <utility>

namespace rt::native::assets {
namespace {

// Proven constants from docs/native-reference/course-prop-chains.md.
constexpr std::array<std::uint32_t, 8> kSetupRecord = {
    0x10000000u, 0u, 0x01000101u, 0u, 0x60000000u, 0u, 0u, 0u};
constexpr std::uint32_t kDmaCntByte3 = 0x10u; // DMA id 1 = CNT, in tag byte 3
constexpr std::uint32_t kDmaRetWord0 = 0x60000000u; // DMA id 6 = RET, QWC 0
constexpr std::uint32_t kVifStcycl = 0x01000101u;   // STCYCL wl=1 cl=1
constexpr std::uint32_t kVifUnpackV4 = 0x6C018000u; // UNPACK V4-32 NUM=1 imm 0x8000
constexpr std::uint32_t kVifUnpackV3Base = 0x68008001u; // UNPACK V3-32 imm 0x8001,
                                                        // NUM in bits 16..23
constexpr std::uint32_t kVifMscalf = 0x1500000Au;   // MSCALF imm 0x000A
constexpr std::uint64_t kGifRegs = 0x412u;          // ST, RGBAQ, XYZF2
constexpr std::uint32_t kGifPrim = 0x264u;          // TRISTRIP + ABE, flat, untextured
constexpr std::uint32_t kTailOverhead = 112u; // 0x10 prologue + 2*32 setup + 16 CNT + 16 RET
constexpr float kTileExtent = 1600.0f;

std::uint32_t readU32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

std::uint64_t readU64(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return static_cast<std::uint64_t>(readU32(bytes, offset)) |
           (static_cast<std::uint64_t>(readU32(bytes, offset + 4)) << 32);
}

float readF32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    return std::bit_cast<float>(readU32(bytes, offset));
}

// Proven course flavors: the normal lane and the alpha ride together, and
// one flavor holds for every vertex of a course.
enum class Flavor : std::uint8_t { Undecided, A, B };

Flavor flavorOf(float n1, float n2, float n3, float alpha) {
    if (n1 == 0.0f && n2 == 1.0f && n3 == 0.0f && alpha == 128.0f) return Flavor::A;
    if (n1 == 0.0f && n2 == 0.0f && n3 == 1.0f && alpha == 1.0f) return Flavor::B;
    return Flavor::Undecided;
}

} // namespace

expected<CoursePropChain, CoursePropError>
parseCourseProps(std::span<const std::uint8_t> data) {
    if (data.size() < 0x20) return CoursePropError::Truncated;

    const std::uint32_t headerSize = readU32(data, 0);
    if (headerSize != 0x20 && headerSize != 0x30) return CoursePropError::BadHeader;
    if (data.size() < headerSize) return CoursePropError::Truncated;
    const std::uint32_t offA = readU32(data, 4);
    const std::uint32_t offB = readU32(data, 8);
    const std::uint32_t offC = readU32(data, 12);
    // offA/offB are only sanity-checked: they delimit earlier container
    // regions whose contents are outside the prop grammar (course_data.cpp
    // exposes those spans without assigning semantics).
    if (offA < headerSize || offB < offA || offC < offB) return CoursePropError::BadHeader;
    if (offC > data.size()) return CoursePropError::BadHeader;

    CoursePropChain chain;
    chain.headerSize = headerSize;
    // Header words 4+ are the boundary list: packet-block end offsets,
    // strictly monotone, zero-padded, last boundary == file size.
    std::uint32_t previous = 0;
    bool sawZero = false;
    for (std::size_t offset = 4 * sizeof(std::uint32_t); offset < headerSize;
         offset += sizeof(std::uint32_t)) {
        const std::uint32_t boundary = readU32(data, offset);
        if (boundary == 0) {
            sawZero = true;
            continue;
        }
        if (sawZero || boundary <= previous ||
            boundary > data.size()) {
            return CoursePropError::BadHeader;
        }
        chain.boundaries.push_back(boundary);
        previous = boundary;
    }

    if (chain.boundaries.empty()) {
        // C16 form: offC == file size and the boundary word is zero — a
        // zero-length tail is the proven "no props" encoding.
        if (offC != data.size()) return CoursePropError::BadHeader;
        chain.tailBegin = offC;
        chain.tailEnd = offC;
        return chain;
    }
    if (chain.boundaries.back() != data.size()) return CoursePropError::BadHeader;
    if (offC < headerSize || offC >= chain.boundaries[0]) {
        return CoursePropError::BadHeader;
    }
    chain.tailBegin = offC;
    chain.tailEnd = chain.boundaries[0];
    chain.extraBlockCount = static_cast<std::uint32_t>(chain.boundaries.size() - 1);

    // Primary block: prologue u32 {0x30, 0x50, 0, N}.
    const std::size_t base = offC;
    if (data.size() - base < 0x60) return CoursePropError::Truncated;
    if (readU32(data, base) != 0x30u || readU32(data, base + 4) != 0x50u ||
        readU32(data, base + 8) != 0u) {
        return CoursePropError::BadPrologue;
    }
    const std::uint32_t propCount = readU32(data, base + 12);
    if (propCount == 0) return CoursePropError::BadPrologue;

    // Setup records A (base+0x10) and B (base+0x30): byte-identical constant.
    for (std::size_t record = 0; record < 2; ++record) {
        const std::size_t recordBase = base + 0x10 + 0x20 * record;
        for (std::size_t i = 0; i < kSetupRecord.size(); ++i) {
            if (readU32(data, recordBase + 4 * i) != kSetupRecord[i]) {
                return CoursePropError::BadSetupRecord;
            }
        }
    }

    // Main chain at base+0x50: CNT tag (word0 = 0x10000000|qwc, word1 = 0,
    // TTE VIF words {STCYCL, UNPACK V4-32 NUM=1}), then qwc*16 inline bytes,
    // then RET(QWC=0). Tail arithmetic: tailEnd - offC == 112 + 16*qwc.
    const std::uint32_t dmaWord0 = readU32(data, base + 0x50);
    if ((dmaWord0 >> 24) != kDmaCntByte3) return CoursePropError::BadDmaChain;
    const std::uint32_t qwc = dmaWord0 & 0xFFFFu;
    if (readU32(data, base + 0x54) != 0u) return CoursePropError::BadDmaChain;
    if (static_cast<std::uint64_t>(chain.tailEnd) - offC !=
        kTailOverhead + 16ull * qwc) {
        return CoursePropError::BadDmaChain;
    }

    // VIF stream covers the two TTE words plus the inline payload; it ends
    // exactly at the RET tag.
    const std::size_t streamEnd = base + 0x58 + 8 + 16ull * qwc;
    if (streamEnd + 16 > data.size()) return CoursePropError::Truncated;
    if (readU32(data, streamEnd) != kDmaRetWord0) return CoursePropError::BadDmaChain;
    if (readU32(data, base + 0x58) != kVifStcycl) return CoursePropError::BadVifStream;

    std::size_t cursor = base + 0x5C; // first draw's UNPACK V4 is the tag TTE word
    Flavor flavor = Flavor::Undecided;
    chain.draws.reserve(propCount);
    for (std::uint32_t draw = 0; draw < propCount; ++draw) {
        // Any overrun inside the loop means the stream and the prologue N
        // disagree (or the QWC is corrupt): report it as a stream mismatch.
        if (cursor + 28 > streamEnd) return CoursePropError::BadVifStream;
        if (readU32(data, cursor) != kVifUnpackV4) {
            return CoursePropError::BadVifStream;
        }

        CoursePropDraw prop;
        prop.tagOffset = static_cast<std::uint32_t>(cursor + 4);
        prop.gifTag = readU64(data, cursor + 4);
        prop.gifRegs = readU64(data, cursor + 12);
        prop.vertexCount = gifTagNloop(prop.gifTag);
        const bool goodTag = prop.vertexCount >= 3 && prop.vertexCount <= 50 &&
                             gifTagEop(prop.gifTag) && gifTagPre(prop.gifTag) &&
                             gifTagPrim(prop.gifTag) == kGifPrim &&
                             gifTagFlg(prop.gifTag) == 0 &&
                             gifTagNreg(prop.gifTag) == 3 &&
                             prop.gifRegs == kGifRegs;
        if (!goodTag) return CoursePropError::BadGifTag;

        if (readU32(data, cursor + 20) !=
            (kVifUnpackV3Base | (4u * prop.vertexCount) << 16)) {
            return CoursePropError::BadVifStream;
        }

        const std::size_t geomBytes = 48ull * prop.vertexCount;
        if (cursor + 24 + geomBytes + 4 > streamEnd) {
            return CoursePropError::BadVifStream;
        }
        prop.vertices.reserve(prop.vertexCount);
        for (std::size_t vertex = 0; vertex < prop.vertexCount; ++vertex) {
            const std::size_t at = cursor + 24 + 48 * vertex;
            const float x = readF32(data, at);
            const float y = readF32(data, at + 4);
            const float z = readF32(data, at + 8);
            const float n1 = readF32(data, at + 12);
            const float n2 = readF32(data, at + 16);
            const float n3 = readF32(data, at + 20);
            const float r = readF32(data, at + 24);
            const float g = readF32(data, at + 28);
            const float b = readF32(data, at + 32);
            const float pad0 = readF32(data, at + 36);
            const float pad1 = readF32(data, at + 40);
            const float alpha = readF32(data, at + 44);
            if (y != 0.0f || x <= 0.0f || x >= kTileExtent || z <= 0.0f ||
                z >= kTileExtent) {
                return CoursePropError::BadVertexUnit;
            }
            if (r != 255.0f || g != 255.0f || b != 255.0f || pad0 != 0.0f ||
                pad1 != 0.0f) {
                return CoursePropError::BadVertexUnit;
            }
            const Flavor vertexFlavor = flavorOf(n1, n2, n3, alpha);
            if (vertexFlavor == Flavor::Undecided) {
                return CoursePropError::BadVertexUnit;
            }
            if (flavor == Flavor::Undecided) {
                flavor = vertexFlavor;
                chain.flavorNormal = {n1, n2, n3};
                chain.alpha = alpha;
            } else if (flavor != vertexFlavor) {
                return CoursePropError::BadVertexUnit;
            }
            prop.vertices.push_back({x, z, static_cast<std::uint8_t>(alpha)});
        }
        cursor += 24 + geomBytes;
        if (readU32(data, cursor) != kVifMscalf) {
            return CoursePropError::BadVifStream;
        }
        cursor += 4;
        chain.draws.push_back(std::move(prop));
    }
    chain.propCount = propCount;

    // After the N draws only 0..3 trailing NOP words may pad the packet, and
    // the stream must end exactly at the RET — a loop that terminated early
    // (N smaller than the streamed draws) lands on an UNPACK word here.
    while (cursor < streamEnd) {
        if (readU32(data, cursor) != 0u) return CoursePropError::BadVifStream;
        cursor += 4;
    }
    return chain;
}

} // namespace rt::native::assets
