// FLD/COURSE field-tile header parsing and the currently-cracked field mesh
// descriptors.  Directory and texture conversion remain deferred.

#include "field_converter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace rt::native::assets {
namespace {

constexpr std::size_t kHeaderBytes = 0x20;
constexpr std::size_t kDigestBytes = 32;
constexpr std::size_t kDescriptorBytes = 5 * sizeof(std::uint32_t);
constexpr std::size_t kFieldVertexFloatCount = 6;
constexpr std::size_t kFieldVertexBytes = kFieldVertexFloatCount * sizeof(float);
constexpr std::size_t kIndexBufferOffset = 0x50A6;
constexpr std::uint32_t kMaterialDescriptorTag = 0x1012B42C;
constexpr std::size_t kSubBlockDirectoryEntries = 65;
constexpr std::size_t kAttributeTableEntries = 256;
constexpr std::uint32_t kPmpMagicOld = 0x600;
constexpr std::uint32_t kPmpMagicNew = 0x608;
constexpr std::size_t kPmpExtensionWordOffset = 0x180;
constexpr std::uint32_t kPacketHeaderFlag = 0x8000;
constexpr std::uint32_t kPacketHeaderTag = 0x20224000;
constexpr std::uint32_t kPacketHeaderKind = 0x41;
constexpr std::uint32_t kPacketMaxVertices = 0x400;
constexpr float kPacketGridSpacing = 40.0f;
constexpr std::uint8_t kChainTagRef = 0x10;   // DMA REF tag id (byte 3)
constexpr std::uint8_t kChainTagEnd = 0x70;   // DMA REF-End tag id ('p', byte 3)
constexpr std::uint32_t kGifRegAddrAndData = 0xE;  // GIF REGS field: A+D packed
constexpr std::uint32_t kGifRegBlitBuf = 0x50;
constexpr std::uint32_t kGifRegTransferPos = 0x51;
constexpr std::uint32_t kGifRegTransferReg = 0x52;
constexpr std::uint32_t kGifRegTransferDir = 0x53;
constexpr std::size_t kGsPageBytes = 256;

std::uint32_t readU32LE(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

std::uint64_t readU64LE(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return static_cast<std::uint64_t>(readU32LE(bytes, offset)) |
           (static_cast<std::uint64_t>(readU32LE(bytes, offset + 4)) << 32);
}

std::uint16_t readU16LE(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1]) << 8);
}

float readFloatLE(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::bit_cast<float>(readU32LE(bytes, offset));
}

bool isMeshDescriptor(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    const std::uint32_t tag = readU32LE(bytes, offset);
    // The fixed zero word is part of the mesh-header shape.  It is necessary
    // to avoid treating u16 index data that happens to resemble 0x1012xxxx as
    // a descriptor (FLD/000.BIN has one such false positive at 0x5094).
    return (tag >> 16) == 0x1012u && tag != kMaterialDescriptorTag &&
           readU32LE(bytes, offset + sizeof(std::uint32_t)) == 0;
}

bool isFiniteVertex(const Vertex& vertex) noexcept {
    return std::isfinite(vertex.px) && std::isfinite(vertex.py) &&
           std::isfinite(vertex.pz) && std::isfinite(vertex.u) &&
           std::isfinite(vertex.v);
}

bool isAllZeroDigest(const std::array<std::uint8_t, kDigestBytes>& digest) noexcept {
    return std::all_of(digest.begin(), digest.end(),
                       [](std::uint8_t byte) { return byte == 0; });
}

} // namespace

expected<FieldTileHeader, FieldTileError>
parseFieldTileHeader(std::span<const std::uint8_t> tileFile) {
    const std::uint64_t actualFileSize = static_cast<std::uint64_t>(tileFile.size());
    if (actualFileSize < kHeaderBytes) {
        return FieldTileError::Truncated;
    }

    // Word 0 is the header size: 0x20 canonical, 0x160 for 023.BIN.  The
    // declared fileSize may be smaller than the physical file (14 tiles carry
    // trailing padding).  Region ordering offA <= offB <= offC holds for all
    // 64 tiles — an earlier "13 reordered-dir variants" note was a hex
    // digit-count misread (0x1e800 < 0x1e3dc0); those tiles differ only in
    // padding.  See docs/native-reference/field-topology-status.md.
    const std::uint32_t headerSize = readU32LE(tileFile, 0x00);
    if (headerSize < kHeaderBytes || headerSize > 0x1000) {
        return FieldTileError::BadHeader;
    }

    FieldTileHeader header{
        .offA = readU32LE(tileFile, 0x04),
        .offB = readU32LE(tileFile, 0x08),
        .offC = readU32LE(tileFile, 0x0C),
        .fileSize = readU32LE(tileFile, 0x10),
    };
    if (static_cast<std::uint64_t>(header.fileSize) > actualFileSize ||
        header.offA < headerSize || header.offA > header.offB ||
        header.offB > header.offC || header.offC > header.fileSize) {
        return FieldTileError::BadHeader;
    }
    return header;
}

expected<FieldTileCatalog, FieldTileError>
catalogFieldTile(std::span<const std::uint8_t> tileFile) {
    const auto header = parseFieldTileHeader(tileFile);
    if (!header) {
        return header.error();
    }

    FieldTileCatalog catalog;
    catalog.header = *header;

    const std::size_t directory = header->offA;
    const std::size_t attributes =
        directory + kSubBlockDirectoryEntries * sizeof(std::uint32_t);
    if (attributes + kAttributeTableEntries * sizeof(std::uint32_t) > tileFile.size()) {
        return FieldTileError::Truncated;
    }

    std::uint32_t previous = 0;
    for (std::size_t entry = 0; entry < kSubBlockDirectoryEntries; ++entry) {
        const std::uint32_t relative =
            readU32LE(tileFile, directory + entry * sizeof(std::uint32_t));
        if (relative == 0) {
            continue;  // unused slot
        }
        if (relative < previous) {
            return FieldTileError::BadHeader;
        }
        previous = relative;
        const std::uint64_t absolute = static_cast<std::uint64_t>(directory) + relative;
        if (absolute + sizeof(std::uint32_t) > tileFile.size()) {
            return FieldTileError::Truncated;
        }
        catalog.subBlocks.push_back(static_cast<std::uint32_t>(absolute));
    }
    if (catalog.subBlocks.empty()) {
        return FieldTileError::BadHeader;
    }

    for (std::size_t entry = 0; entry < kAttributeTableEntries; ++entry) {
        if (readU32LE(tileFile, attributes + entry * sizeof(std::uint32_t)) != 0) {
            ++catalog.attributeCount;
        }
    }

    const std::size_t pmp = header->offB;
    if (pmp + sizeof(std::uint32_t) > tileFile.size()) {
        return FieldTileError::Truncated;
    }
    catalog.pmpMagic = readU32LE(tileFile, pmp);
    if (catalog.pmpMagic != kPmpMagicOld && catalog.pmpMagic != kPmpMagicNew) {
        return FieldTileError::BadHeader;
    }

    const std::uint32_t pmpSpan = header->offC - header->offB;
    std::uint32_t sectionPrevious = 0;
    for (std::size_t word = sizeof(std::uint32_t);
         word + sizeof(std::uint32_t) <= pmpSpan && pmp + word + sizeof(std::uint32_t) <= tileFile.size();
         word += sizeof(std::uint32_t)) {
        const std::uint32_t relative = readU32LE(tileFile, pmp + word);
        if (relative <= sectionPrevious || relative >= pmpSpan) {
            break;
        }
        sectionPrevious = relative;
        catalog.pmpSections.push_back(static_cast<std::uint32_t>(pmp + relative));
    }
    if (catalog.pmpSections.empty()) {
        return FieldTileError::BadHeader;
    }

    if (catalog.pmpMagic == kPmpMagicNew) {
        if (pmp + kPmpExtensionWordOffset + sizeof(std::uint32_t) > tileFile.size()) {
            return FieldTileError::Truncated;
        }
        const std::uint32_t relative = readU32LE(tileFile, pmp + kPmpExtensionWordOffset);
        if (relative >= pmpSpan) {
            return FieldTileError::BadHeader;
        }
        catalog.pmpExtensionOffset = static_cast<std::uint32_t>(pmp + relative);
    }

    return catalog;
}

namespace {

bool plausiblePacketVertex(std::span<const std::uint8_t> file, std::size_t at) noexcept {
    const float x = readFloatLE(file, at);
    const float z = readFloatLE(file, at + sizeof(float));
    const float y = readFloatLE(file, at + 2 * sizeof(float));
    const float q = readFloatLE(file, at + 3 * sizeof(float));
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(q) &&
           std::abs(x) < 5000.0f && std::abs(y) < 5000.0f && std::abs(z) < 5000.0f &&
           q > 0.25f && q < 4.0f;
}

bool plausibleFaceNormal(std::span<const std::uint8_t> file, std::size_t at) noexcept {
    const float nx = readFloatLE(file, at);
    const float ny = readFloatLE(file, at + sizeof(float));
    const float nz = readFloatLE(file, at + 2 * sizeof(float));
    const float w = readFloatLE(file, at + 3 * sizeof(float));
    return std::isfinite(nx) && std::isfinite(ny) && std::isfinite(nz) && std::isfinite(w) &&
           std::abs(nx) < 1e6f && std::abs(nz) < 1e6f;
}

} // namespace

std::vector<FieldPacket>
extractFieldPackets(std::span<const std::uint8_t> tileFile,
                   const FieldTileCatalog& catalog) {
    std::vector<FieldPacket> packets;
    const std::size_t sectionCount = catalog.pmpSections.size();
    for (std::size_t section = 0; section < sectionCount; ++section) {
        const std::size_t limit = section + 1 < sectionCount
                                     ? catalog.pmpSections[section + 1]
                                     : catalog.header.offC;
        // Anchoring on packet headers (word1/word2 constants are denormal-as-
        // float bit patterns that never collide with live vertex data) keeps
        // the walk drift-free even where attribute gaps deviate.
        std::vector<std::pair<std::size_t, std::uint32_t>> headers;
        for (std::size_t cursor = catalog.pmpSections[section]; cursor + 16 <= limit;
             cursor += 16) {
            const std::uint32_t word0 = readU32LE(tileFile, cursor);
            if ((word0 & 0xF000u) == kPacketHeaderFlag &&
                readU32LE(tileFile, cursor + 4) == kPacketHeaderTag &&
                readU32LE(tileFile, cursor + 8) == kPacketHeaderKind) {
                headers.emplace_back(cursor, word0 & 0x0FFFu);
            }
        }
        for (std::size_t index = 0; index < headers.size(); ++index) {
            const std::size_t start = headers[index].first;
            const std::uint32_t vertexCount = headers[index].second;
            const std::size_t nextHeader =
                index + 1 < headers.size() ? headers[index + 1].first : limit;
            if (vertexCount < 3 || vertexCount > kPacketMaxVertices ||
                start + 16 + static_cast<std::size_t>(vertexCount) * 16 > nextHeader) {
                continue;
            }
            FieldPacket packet;
            packet.sectionIndex = static_cast<std::uint32_t>(section);
            packet.materialRef = readU32LE(tileFile, start + 12) & 0x00FF'FFFFu;
            packet.vertices.reserve(vertexCount);
            bool verticesOk = true;
            std::size_t cursor = start + 16;
            for (std::uint32_t vertex = 0; vertex < vertexCount; ++vertex) {
                if (!plausiblePacketVertex(tileFile, cursor)) {
                    verticesOk = false;
                    break;
                }
                // Disc layout is (x, z, y, q); host vertices are x/y/z with a
                // planar UV at the observed grid spacing.
                Vertex decoded{};
                decoded.px = readFloatLE(tileFile, cursor);
                decoded.py = readFloatLE(tileFile, cursor + 2 * sizeof(float));
                decoded.pz = readFloatLE(tileFile, cursor + sizeof(float));
                decoded.u = decoded.px / kPacketGridSpacing;
                decoded.v = decoded.pz / kPacketGridSpacing;
                packet.vertices.push_back(decoded);
                cursor += 4 * sizeof(float);
            }
            if (!verticesOk) {
                continue;
            }
            // Trailing quads are per-face attributes; when the gap is exactly
            // N-2 they are the flat-shading face normals (nx, ny, nz, w).
            const std::size_t gap = (nextHeader - cursor) / (4 * sizeof(float));
            if (gap == vertexCount - 2) {
                packet.faceNormals.reserve(gap);
                for (std::uint32_t face = 0; face + 2 < vertexCount; ++face) {
                    if (!plausibleFaceNormal(tileFile, cursor)) {
                        packet.faceNormals.clear();
                        break;
                    }
                    packet.faceNormals.push_back(
                        {readFloatLE(tileFile, cursor),
                         readFloatLE(tileFile, cursor + sizeof(float)),
                         readFloatLE(tileFile, cursor + 2 * sizeof(float))});
                    cursor += 4 * sizeof(float);
                }
            }
            packets.push_back(std::move(packet));
        }
    }
    return packets;
}

FieldRenderTerrain buildFieldRenderTerrain(std::span<const std::uint8_t> tileFile,
                                          const FieldTileCatalog& catalog, float scale) {
    FieldRenderTerrain terrain;
    const std::vector<FieldPacket> packets = extractFieldPackets(tileFile, catalog);
    for (const FieldPacket& packet : packets) {
        if (packet.vertices.size() < 3) {
            continue;
        }
        const std::uint32_t base = static_cast<std::uint32_t>(terrain.positions.size());
        for (const Vertex& vertex : packet.vertices) {
            // FieldPacket vertices are (px=x, py=disc-y, pz=height); render
            // space is y-up.
            terrain.positions.push_back({vertex.px * scale, vertex.pz * scale,
                                        vertex.py * scale});
        }
        for (std::uint32_t fan = 1; fan + 2 <= packet.vertices.size(); ++fan) {
            terrain.indices.push_back(base);
            terrain.indices.push_back(base + fan);
            terrain.indices.push_back(base + fan + 1);
            terrain.triangleMaterials.push_back(packet.materialRef);
        }
    }
    return terrain;
}

namespace {

// Pixel-storage modes observed on disc (native-textures.md §1: CT32, CT24,
// CT16, T8, T4; the H/S variants are carried for completeness and never
// occur in the FLD corpus).
std::uint32_t psmBitsPerPixel(std::uint32_t psm) noexcept {
    switch (psm) {
    case 0x00: return 32;  // PSMCT32
    case 0x01: return 24;  // PSMCT24
    case 0x02:
    case 0x0A: return 16;  // PSMCT16 / PSMCT16S
    case 0x13:
    case 0x1B: return 8;   // PSMT8 / PSMT8H
    case 0x14:
    case 0x24:
    case 0x2C: return 4;   // PSMT4 / PSMT4HL / PSMT4HH
    default: return 0;
    }
}

// Palette-shaped upload (direct colour, at most 16 columns and 256 entries),
// the same predicate the image converter uses for CLUT uploads.
bool isClutShaped(std::uint32_t psm, std::uint32_t width, std::uint32_t height) noexcept {
    const bool directColour =
        psm == 0x00 || psm == 0x01 || psm == 0x02 || psm == 0x0A;
    return directColour && width <= 16 &&
           (height <= 2 || (width == 16 && height == 16) || (width == 8 && height == 8));
}

} // namespace

expected<std::vector<FieldTexture>, FieldTileError>
fieldTextureCatalog(std::span<const std::uint8_t> tileFile) {
    if (tileFile.size() < kHeaderBytes) {
        return FieldTileError::Truncated;
    }
    // Word0 is the header size: 0x20 canonical, 0x160 for the 023.BIN
    // variant; the chain always starts at the end of the header.  offB/offC
    // are deliberately not validated here: the chain region [headerSize,
    // offA) is unaffected by the 14 padded tiles (declared fileSize <
    // physical size).
    const std::uint32_t headerSize = readU32LE(tileFile, 0x00);
    const std::uint32_t offA = readU32LE(tileFile, 0x04);
    if (headerSize < kHeaderBytes || offA <= headerSize ||
        static_cast<std::uint64_t>(offA) > tileFile.size()) {
        return FieldTileError::BadHeader;
    }

    std::vector<FieldTexture> textures;
    bool terminated = false;
    std::size_t cursor = headerSize;
    while (cursor + 16 <= offA) {
        const std::uint32_t tagWord = readU32LE(tileFile, cursor);
        const std::uint8_t id = static_cast<std::uint8_t>(tagWord >> 24);
        if (id == kChainTagEnd) {
            terminated = true; // END payload is not entered (loader semantics)
            break;
        }
        if (id != kChainTagRef) {
            return FieldTileError::ChainDecode;
        }
        const std::size_t qwc = tagWord & 0xFFFFu;
        const std::size_t payloadStart = cursor + 16;
        const std::size_t payloadEnd = payloadStart + qwc * 16;
        if (payloadEnd > offA || payloadEnd > tileFile.size()) {
            return FieldTileError::Truncated;
        }

        std::size_t pos = payloadStart;
        while (pos + 16 <= payloadEnd) {
            const std::uint64_t gifTag = readU64LE(tileFile, pos);
            const std::uint32_t flg = static_cast<std::uint32_t>((gifTag >> 58) & 0x3u);
            const std::uint32_t nloop = static_cast<std::uint32_t>(gifTag & 0x7FFFu);
            const std::uint32_t nreg = static_cast<std::uint32_t>((gifTag >> 60) & 0xFu);
            if (flg != 0 || nreg != 1 ||
                (readU64LE(tileFile, pos + 8) & 0xFu) != kGifRegAddrAndData ||
                nloop == 0 || nloop > 64) {
                // IMAGE and REGLIST packets never stand alone in this chain.
                return FieldTileError::ChainDecode;
            }
            const std::size_t dataStart = pos + 16;
            const std::size_t packetEnd = dataStart + static_cast<std::size_t>(nloop) * 16;
            if (packetEnd > payloadEnd) {
                return FieldTileError::Truncated;
            }
            const std::size_t gifStart = pos;
            std::uint64_t blitBuf = 0;
            std::uint64_t transferPos = 0;
            std::uint64_t transferReg = 0;
            std::uint64_t transferDir = 0;
            bool haveBlit = false;
            bool havePos = false;
            bool haveReg = false;
            bool haveDir = false;
            for (std::uint32_t i = 0; i < nloop; ++i) {
                const std::size_t at = dataStart + static_cast<std::size_t>(i) * 16;
                const std::uint64_t value = readU64LE(tileFile, at);
                switch (readU32LE(tileFile, at + 8) & 0xFFu) {
                case kGifRegBlitBuf: blitBuf = value; haveBlit = true; break;
                case kGifRegTransferPos: transferPos = value; havePos = true; break;
                case kGifRegTransferReg: transferReg = value; haveReg = true; break;
                case kGifRegTransferDir: transferDir = value & 3u; haveDir = true; break;
                default: break; // other registers (texture state) are not part of an upload
                }
            }
            pos = packetEnd;
            if (!(haveBlit && havePos && haveReg && haveDir && transferDir == 0)) {
                continue; // not a host-to-local upload; no IMAGE data follows
            }

            FieldTexture texture;
            texture.tagOffset = static_cast<std::uint32_t>(cursor);
            texture.gifOffset = static_cast<std::uint32_t>(gifStart);
            texture.dbp = static_cast<std::uint32_t>((blitBuf >> 32) & 0x3FFFu);
            texture.dbw = static_cast<std::uint32_t>((blitBuf >> 48) & 0x3Fu);
            texture.psm = static_cast<std::uint32_t>((blitBuf >> 56) & 0x3Fu);
            texture.width = static_cast<std::uint32_t>(transferReg & 0xFFFu);
            texture.height = static_cast<std::uint32_t>((transferReg >> 32) & 0xFFFu);
            // DSAX/DSAY live in TRXPOS bits 32..42/48..58; every disc upload
            // is a whole image (native-textures.md §3.1).
            if (((transferPos >> 32) & 0x7FFu) != 0 || ((transferPos >> 48) & 0x7FFu) != 0) {
                return FieldTileError::ChainDecode; // partial uploads never occur on disc
            }
            const std::uint32_t bpp = psmBitsPerPixel(texture.psm);
            if (bpp == 0 || texture.width == 0 || texture.height == 0 || texture.dbw == 0) {
                return FieldTileError::ChainDecode;
            }
            const std::size_t pixelBytes =
                (static_cast<std::size_t>(texture.width) * texture.height * bpp + 7u) / 8u;
            texture.dataBytes = static_cast<std::uint32_t>(pixelBytes);
            texture.isClut = isClutShaped(texture.psm, texture.width, texture.height);

            std::size_t pixels = 0;
            while (pixels < pixelBytes) {
                if (pos + 16 > payloadEnd) {
                    return FieldTileError::Truncated;
                }
                const std::uint64_t imageTag = readU64LE(tileFile, pos);
                if (((imageTag >> 58) & 0x3u) != 2) {
                    return FieldTileError::ChainDecode; // upload without its IMAGE data
                }
                const std::size_t chunk = static_cast<std::size_t>(imageTag & 0x7FFFu) * 16;
                if (pos + 16 + chunk > payloadEnd) {
                    return FieldTileError::Truncated;
                }
                if (pixels == 0) {
                    texture.dataOffset = static_cast<std::uint32_t>(pos + 16);
                }
                pixels += chunk;
                pos += 16 + chunk;
            }
            if (pixels != pixelBytes) {
                // Byte-exact accounting held for all 3,097 disc uploads; a
                // padded IMAGE tail means this is not the proven grammar.
                return FieldTileError::ChainDecode;
            }
            textures.push_back(texture);
        }
        if (pos != payloadEnd) {
            return FieldTileError::ChainDecode; // dangling bytes inside the CNT payload
        }
        cursor = payloadEnd;
    }
    if (!terminated) {
        return FieldTileError::ChainDecode;
    }

    // Pair each texture with the CLUT upload whose page starts exactly where
    // the texture's pages end (disc rule: texture first, then its palette).
    for (std::size_t i = 0; i < textures.size(); ++i) {
        if (textures[i].isClut) {
            continue;
        }
        const std::uint32_t pages =
            static_cast<std::uint32_t>((textures[i].dataBytes + kGsPageBytes - 1) / kGsPageBytes);
        for (std::size_t j = i + 1; j < textures.size(); ++j) {
            if (textures[j].isClut && textures[j].dbp == textures[i].dbp + pages) {
                textures[i].clutOffset = textures[j].tagOffset;
                textures[i].clutDbp = textures[j].dbp;
                break;
            }
        }
    }
    return textures;
}

namespace {

// VIF command codes verified against jak-project common/dma/dma.h VifCode:
// cmd = (word >> 24) & 0x7f, num = (word >> 16) & 0xff (0 == 256),
// imm = word & 0xffff; UNPACK dest = imm & 0x3ff, TOPS = imm bit 15.
// The FLD cell corpus uses exactly NOP 0x00, STCYCL 0x01, MSCALF 0x15,
// UNPACK V3-32 0x68 and UNPACK V4-32 0x6C (the task-level guess "V3-32 =
// 0x64" is wrong; 0x64 is V2-32).
constexpr std::uint32_t kVifNop = 0x00;
constexpr std::uint32_t kVifStcycl = 0x01;
constexpr std::uint32_t kVifMscalf = 0x15;
constexpr std::uint32_t kVifMscnt = 0x17;
constexpr std::uint32_t kVifStmask = 0x20;
constexpr std::uint32_t kVifStrow = 0x30;
constexpr std::uint32_t kVifStcol = 0x31;
constexpr std::uint32_t kVifMpg = 0x4A;
constexpr std::uint32_t kVifDirect = 0x50;
constexpr std::uint32_t kVifDirectHl = 0x51;
constexpr std::uint32_t kVifUnpackV3V4_32 = 0x69;
constexpr std::uint32_t kVifUnpackV3_32 = 0x68;
constexpr std::uint32_t kVifUnpackV4_32 = 0x6C;
constexpr std::uint32_t kVifTopsBit = 0x8000;
constexpr std::uint32_t kVifUnpackMask = 0x60;
constexpr std::uint8_t kCellTagCnt = 0x10;  // CNT/REF with inline qwc payload (TTE)
constexpr std::uint8_t kCellTagRet = 0x60;  // RET terminator (qwc == 0 on disc)
constexpr std::uint32_t kMaterialVuDest = 0x192;
constexpr std::uint32_t kVertexVuDest = 1;
constexpr std::uint32_t kVertexVectorsPerVertex = 5;
constexpr std::uint32_t kWorldStripCall = 8;  // MSCALF imm for the WorldStrip entry
constexpr std::uint32_t kGifRegTex0 = 0x07;
constexpr std::uint32_t kGifRegTex1 = 0x15;
constexpr std::uint32_t kGifRegClamp = 0x09;
constexpr std::uint32_t kGifRegMiptbp1 = 0x35;

// Payload bytes for one UNPACK/mpg/direct command, `num` already resolved
// from the 0-means-256 encoding (jak model_dump byte accounting).
std::size_t vifPayloadBytes(std::uint32_t cmd, std::uint32_t num) noexcept {
    const std::size_t n = num == 0 ? 256 : num;
    if (cmd == kVifDirect) {
        return n * 16;
    }
    if (cmd == kVifDirectHl || cmd == kVifMpg) {
        return n * 8;
    }
    if (cmd == kVifUnpackV3V4_32) {
        return n * 28;
    }
    if ((cmd & kVifUnpackMask) != kVifUnpackMask) {
        return 0;
    }
    const std::size_t vectors = ((cmd >> 2) & 0x3u) + 1u;
    switch (cmd & 0x3u) {
    case 0x0: return n * vectors * 4;  // V?-32
    case 0x1: return n * vectors * 2;  // V?-16
    case 0x2: return n * vectors;      // V?-8
    default: return 0;
    }
}

// Decodes TEX0 fields (jak gs-tex0 layout) and resolves the page/palette
// against the tile's staged GIF IMAGE uploads.
void bindMaterial(FieldCellMaterial& material,
                 const std::vector<FieldTexture>& textures) {
    const std::uint64_t tex0 = material.tex0;
    material.tex0Tbp0 = static_cast<std::uint32_t>(tex0 & 0x3FFFu);
    material.tex0Tbw = static_cast<std::uint32_t>((tex0 >> 14) & 0x3Fu);
    material.tex0Psm = static_cast<std::uint32_t>((tex0 >> 20) & 0x3Fu);
    material.tex0Tw = static_cast<std::uint32_t>((tex0 >> 26) & 0xFu);
    material.tex0Th = static_cast<std::uint32_t>((tex0 >> 30) & 0xFu);
    material.tex0Tcc = static_cast<std::uint32_t>((tex0 >> 34) & 1u);
    material.tex0Tfx = static_cast<std::uint32_t>((tex0 >> 35) & 3u);
    material.tex0Cbp = static_cast<std::uint32_t>((tex0 >> 37) & 0x3FFFu);
    material.tex0Cpsm = static_cast<std::uint32_t>((tex0 >> 51) & 0xFu);
    material.tex0Csm = static_cast<std::uint32_t>((tex0 >> 55) & 1u);
    material.tex0Csa = static_cast<std::uint32_t>((tex0 >> 56) & 0x1Fu);
    material.tex0Cld = static_cast<std::uint32_t>((tex0 >> 61) & 7u);
    for (std::size_t index = 0; index < textures.size(); ++index) {
        const FieldTexture& texture = textures[index];
        if (texture.dbp == material.tex0Tbp0 && !texture.isClut && material.textureIndex < 0) {
            material.textureIndex = static_cast<int>(index);
        }
        if (texture.dbp == material.tex0Cbp && texture.isClut && material.paletteIndex < 0) {
            material.paletteIndex = static_cast<int>(index);
        }
    }
    material.valid = true;
}

// Captures one @0x192 material packet: `vectors` V3 vectors of source lanes,
// each [data_lo, data_hi, regAddr] (the GIF tag vector's third lane is the
// 0xE A+D regs descriptor).  The preserved W lanes never carry the fields we
// read, so every decoded value is complete.
FieldCellMaterial decodeMaterialPacket(const std::uint32_t* lanes, std::uint32_t vectors) {
    FieldCellMaterial material;
    material.qwordCount = static_cast<std::uint32_t>(vectors - 1);
    material.gifTag = static_cast<std::uint64_t>(lanes[0]) |
                      (static_cast<std::uint64_t>(lanes[1]) << 32);
    material.regs[0] = material.gifTag;
    material.regAddrs[0] = lanes[2];
    for (std::uint32_t vector = 1; vector < vectors; ++vector) {
        const std::uint64_t value = static_cast<std::uint64_t>(lanes[vector * 3]) |
                                   (static_cast<std::uint64_t>(lanes[vector * 3 + 1]) << 32);
        material.regs[vector] = value;
        material.regAddrs[vector] = lanes[vector * 3 + 2] & 0xFFu;
        switch (material.regAddrs[vector]) {
        case kGifRegTex1: material.tex1 = value; break;
        case kGifRegTex0: material.tex0 = value; break;
        case kGifRegClamp: material.clamp = value; break;
        case kGifRegMiptbp1: material.miptbp1 = value; break;
        default: break;
        }
    }
    return material;
}

FieldVertex decodeFieldVertex(const std::uint32_t* lanes) {
    const auto at = [lanes](std::size_t index) {
        return std::bit_cast<float>(lanes[index]);
    };
    FieldVertex vertex;
    vertex.x = at(0);
    vertex.height = at(1);
    vertex.y = at(2);
    vertex.colA[0] = at(3);
    vertex.colA[1] = at(4);
    vertex.colA[2] = at(5);
    vertex.colB[0] = at(6);
    vertex.colB[1] = at(7);
    vertex.colB[2] = at(8);
    vertex.colC[0] = at(9);
    vertex.colC[1] = at(10);
    vertex.colC[2] = at(11);
    vertex.s = at(12);
    vertex.t = at(13);
    vertex.alphaAdd = at(14);
    return vertex;
}

// Concatenates the chain's VIF word stream (every CNT tag's two TTE codes
// plus its qwc*16 inline bytes) and decodes it.  The chain occupies
// [start, limit); payload reaching past the limit marks the cell truncated
// and stops it.  Unknown commands are skipped word-wise and counted.
FieldCellStrips parseCellChain(std::span<const std::uint8_t> tileFile,
                               std::size_t start,
                               std::size_t limit,
                               const std::vector<FieldTexture>& textures,
                               std::uint32_t cellIndex) {
    FieldCellStrips cell;
    cell.cell = cellIndex;

    std::vector<std::uint32_t> words;
    std::size_t cursor = start;
    while (cursor + 16 <= limit) {
        const std::uint32_t tagWord = readU32LE(tileFile, cursor);
        const std::uint8_t id = static_cast<std::uint8_t>(tagWord >> 24);
        const std::size_t qwc = tagWord & 0xFFFFu;
        if (id == kCellTagCnt) {
            words.push_back(readU32LE(tileFile, cursor + 8));   // TTE VIF code 0
            words.push_back(readU32LE(tileFile, cursor + 12));  // TTE VIF code 1
            const std::size_t payloadEnd = cursor + 16 + qwc * 16;
            if (payloadEnd > limit) {
                cell.truncated = true;
                break;
            }
            for (std::size_t pos = cursor + 16; pos + 4 <= payloadEnd; pos += 4) {
                words.push_back(readU32LE(tileFile, pos));
            }
            cursor = payloadEnd;
            continue;
        }
        if ((id == kCellTagRet && qwc == 0) || id == kChainTagEnd) {
            cell.terminated = true;  // RET(qwc=0) / END terminator
        }
        break;
    }

    bool pendingTag = false;
    std::uint64_t pendingGifTag = 0;
    std::uint64_t pendingGifRegs = 0;
    std::size_t index = 0;
    while (index < words.size()) {
        const std::uint32_t word = words[index];
        const std::uint32_t cmd = (word >> 24) & 0x7Fu;
        const std::uint32_t num = (word >> 16) & 0xFFu;
        const std::uint32_t imm = word & 0xFFFFu;
        const bool isUnpack = (cmd & kVifUnpackMask) == kVifUnpackMask;
        const std::size_t payloadBytes = isUnpack || cmd == kVifMpg || cmd == kVifDirect ||
                                        cmd == kVifDirectHl
                                            ? vifPayloadBytes(cmd, num)
                                            : (cmd == kVifStrow || cmd == kVifStcol ? 16 : 0);
        if (index + 1 + payloadBytes / 4 > words.size()) {
            cell.truncated = true;  // upload runs past the chain end
            break;
        }
        if (cmd == kVifUnpackV3_32) {
            const std::uint32_t vectors = num == 0 ? 256 : num;
            const std::uint32_t dest = imm & 0x3FFu;
            const std::uint32_t* lanes = words.data() + index + 1;
            if (dest == kMaterialVuDest && (imm & kVifTopsBit) != 0 &&
                (vectors == 4 || vectors == 5)) {
                FieldCellMaterial material = decodeMaterialPacket(lanes, vectors);
                bindMaterial(material, textures);
                cell.materials.push_back(material);
            } else if (dest == kVertexVuDest && (imm & kVifTopsBit) != 0 &&
                       vectors % kVertexVectorsPerVertex == 0 && pendingTag) {
                FieldStrip strip;
                strip.gifTag = pendingGifTag;
                strip.gifRegs = pendingGifRegs;
                strip.cell = cellIndex;
                strip.materialIndex =
                    cell.materials.empty() ? -1 : static_cast<int>(cell.materials.size() - 1);
                const std::uint32_t vertexCount = vectors / kVertexVectorsPerVertex;
                strip.vertices.reserve(vertexCount);
                for (std::uint32_t vertex = 0; vertex < vertexCount; ++vertex) {
                    strip.vertices.push_back(decodeFieldVertex(lanes + vertex * 15));
                }
                if (gifTagNloop(strip.gifTag) != vertexCount) {
                    ++cell.nloopMismatches;
                }
                cell.strips.push_back(std::move(strip));
                pendingTag = false;
            } else {
                ++cell.malformedUploads;
            }
        } else if (cmd == kVifUnpackV4_32) {
            const std::uint32_t vectors = num == 0 ? 256 : num;
            const std::uint32_t dest = imm & 0x3FFu;
            if (dest == 0 && (imm & kVifTopsBit) != 0 && vectors >= 1) {
                pendingGifTag = static_cast<std::uint64_t>(words[index + 1]) |
                               (static_cast<std::uint64_t>(words[index + 2]) << 32);
                pendingGifRegs = static_cast<std::uint64_t>(words[index + 3]) |
                                (static_cast<std::uint64_t>(words[index + 4]) << 32);
                pendingTag = true;
                if (vectors != 1) {
                    ++cell.malformedUploads;  // only the first GIF tag qword applies
                }
            } else {
                ++cell.malformedUploads;
            }
        } else if (cmd == kVifMscalf) {
            // imm 8 == WorldStrip; other model entries never occur in cell
            // chains.  A call closes the stream position anyway.
            if (imm != kWorldStripCall) {
                ++cell.malformedUploads;
            }
        } else if (cmd == kVifNop || cmd == kVifStcycl || cmd == kVifStmask ||
                   cmd == kVifStrow || cmd == kVifStcol ||
                   (cmd >= 0x02 && cmd <= 0x05) || (cmd >= 0x08 && cmd <= 0x14) ||
                   cmd == kVifMscnt) {
            // Stateless commands (ITOP/OFFSET/BASE/STMOD/MSK3PATH/MARK/FUSH*/
            // MSCAL/MSCNT); STROW/STCOL payload rows are skipped via
            // payloadBytes.  Nothing to decode.
        } else if (payloadBytes > 0) {
            ++cell.unknownCommands;  // known shape, foreign command (MPG/DIRECT/…)
        } else {
            ++cell.unknownCommands;  // unrecognized command word
        }
        index += 1 + payloadBytes / 4;
    }

    if (!cell.materials.empty()) {
        cell.material = cell.materials.front();
    }
    return cell;
}

} // namespace

expected<std::vector<FieldCellStrips>, FieldTileError>
extractCellStrips(std::span<const std::uint8_t> tileFile) {
    const auto header = parseFieldTileHeader(tileFile);
    if (!header) {
        return header.error();
    }
    const std::size_t directory = header->offA;
    if (directory + kSubBlockDirectoryEntries * sizeof(std::uint32_t) > tileFile.size()) {
        return FieldTileError::Truncated;
    }

    // Cell chains live between the directory and the PMP block at offB.
    const std::size_t chainCeiling =
        header->offB != 0 && header->offB <= tileFile.size() ? header->offB : tileFile.size();

    // Texture binding is best-effort: a failing upload chain only leaves the
    // material links unresolved, it must not fail the strip extraction.
    const std::vector<FieldTexture> textures = [&tileFile] {
        const auto catalog = fieldTextureCatalog(tileFile);
        return catalog ? *catalog : std::vector<FieldTexture>{};
    }();

    std::vector<FieldCellStrips> cells;
    for (std::uint32_t cell = 0; cell < kSubBlockDirectoryEntries - 1; ++cell) {
        const std::uint32_t relative =
            readU32LE(tileFile, directory + cell * sizeof(std::uint32_t));
        if (relative == 0) {
            continue;  // unused directory slot
        }
        const std::size_t chainStart = directory + relative;
        if (chainStart + 16 > chainCeiling) {
            continue;  // directory entry points outside the chain region
        }
        // The chain physically ends where the next directory entry begins.
        std::size_t chainLimit = chainCeiling;
        for (std::uint32_t next = cell + 1; next < kSubBlockDirectoryEntries; ++next) {
            const std::uint32_t nextRelative =
                readU32LE(tileFile, directory + next * sizeof(std::uint32_t));
            if (nextRelative != 0 && directory + nextRelative > chainStart) {
                chainLimit = std::min(chainCeiling, directory + static_cast<std::size_t>(nextRelative));
                break;
            }
        }
        const std::uint8_t firstId =
            static_cast<std::uint8_t>(readU32LE(tileFile, chainStart) >> 24);
        if (firstId != kCellTagCnt) {
            continue;  // not a CNT-rooted chain: skip without failing the tile
        }
        cells.push_back(parseCellChain(tileFile, chainStart, chainLimit, textures, cell));
    }
    return cells;
}

FieldTileResult convertFieldTile(std::span<const std::uint8_t> tileFile,
                                 const std::string& sourceFile,
                                 const std::array<std::uint8_t, 32>& sha256) {
    FieldTileResult result{};
    if (sourceFile.empty() || isAllZeroDigest(sha256)) {
        result.err = FieldTileError::MissingProvenance;
        return result;
    }

    const auto header = parseFieldTileHeader(tileFile);
    if (!header) {
        result.err = header.error();
        return result;
    }

    if (header->offA < kIndexBufferOffset ||
        (static_cast<std::size_t>(header->offA) - kIndexBufferOffset) % sizeof(std::uint16_t) != 0) {
        result.err = FieldTileError::BadBatch;
        result.errorOffset = header->offA;
        return result;
    }

    std::vector<MeshRecord> meshes;
    for (std::size_t descriptorOffset = kHeaderBytes;
         descriptorOffset + kDescriptorBytes <= header->offA;
         descriptorOffset += sizeof(std::uint32_t)) {
        if (!isMeshDescriptor(tileFile, descriptorOffset)) {
            continue;
        }

        const std::uint32_t vertexOffset = readU32LE(tileFile, descriptorOffset + 12);
        const std::uint32_t count = readU32LE(tileFile, descriptorOffset + 16);
        const std::uint64_t vertexBytes =
            static_cast<std::uint64_t>(count) * kFieldVertexBytes;
        const std::uint64_t vertexEnd = static_cast<std::uint64_t>(vertexOffset) + vertexBytes;

        // Vertex sub-buffers are file-relative and live after directory A.
        // A zero count is not a drawable mesh and is treated as a malformed
        // descriptor rather than silently emitting a misleading empty mesh.
        if (count == 0 || vertexOffset < header->offA) {
            result.err = FieldTileError::BadBatch;
            result.errorOffset = descriptorOffset;
            return result;
        }
        if (vertexEnd > tileFile.size()) {
            result.err = FieldTileError::Truncated;
            result.errorOffset = vertexOffset;
            return result;
        }

        MeshRecord mesh;
        mesh.provenance.sourceFile = sourceFile;
        mesh.provenance.offset = descriptorOffset;
        mesh.provenance.length = kDescriptorBytes;
        mesh.provenance.sha256 = sha256;
        mesh.vertices.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index) {
            const std::size_t vertex = static_cast<std::size_t>(vertexOffset) +
                                       static_cast<std::size_t>(index) * kFieldVertexBytes;
            // On disc: [x, z, y, S, T, Q].  Host vertices use x/y/z.
            Vertex decoded{};
            decoded.px = readFloatLE(tileFile, vertex + 0 * sizeof(float));
            decoded.py = readFloatLE(tileFile, vertex + 2 * sizeof(float));
            decoded.pz = readFloatLE(tileFile, vertex + 1 * sizeof(float));
            decoded.u = readFloatLE(tileFile, vertex + 3 * sizeof(float));
            decoded.v = readFloatLE(tileFile, vertex + 4 * sizeof(float));
            if (!isFiniteVertex(decoded)) {
                result.err = FieldTileError::BadBatch;
                result.errorOffset = vertex;
                return result;
            }
            mesh.vertices.push_back(decoded);
        }
        meshes.push_back(std::move(mesh));
    }

    const std::size_t indexCount =
        (static_cast<std::size_t>(header->offA) - kIndexBufferOffset) / sizeof(std::uint16_t);
    if (!meshes.empty()) {
        // The field descriptor does not yet expose index-to-mesh grouping.  Do
        // not invent it: retain the complete raw u16 list on the first mesh.
        // It is deliberately not range-validated against that mesh here;
        // FLD/000.BIN's max index (64) proves that grouping is still unknown.
        meshes.front().indices.reserve(indexCount);
        for (std::size_t index = 0; index < indexCount; ++index) {
            meshes.front().indices.push_back(
                readU16LE(tileFile, kIndexBufferOffset + index * sizeof(std::uint16_t)));
        }
    }

    // Directory/attribute/PMP structure is validated by catalogFieldTile();
    // mesh extraction beyond vertex records still follows the legacy path here.
    result.value.version = kContainerVersion;
    result.value.meshes = std::move(meshes);
    return result;
}

namespace {

struct MaterialBind {
    std::int32_t palette = -1;   // CLUT-shaped catalog index at the bound CBP
    std::uint32_t cbp = 0;
    std::uint32_t csa = 0;
};

// Fetch the CLUT words backing one indexed texture page. Attach order:
// trailing CLUT upload, then the CBP bound by the first cell material naming
// this texture, then any (non-CLUT-shaped) upload sitting on the CBP page.
const FieldTexture* attachClut(const std::vector<FieldTexture>& uploads,
                               const std::unordered_map<std::uint32_t, std::size_t>& clutByTag,
                               const std::unordered_map<std::uint32_t, std::size_t>& anyByDbp,
                               const MaterialBind& bind, std::uint32_t clutOffset,
                               bool& aliased) {
    if (clutOffset != 0) {
        auto it = clutByTag.find(clutOffset);
        if (it != clutByTag.end()) {
            aliased = false;
            return &uploads[it->second];
        }
    }
    if (bind.palette >= 0) {
        aliased = false;
        return &uploads[static_cast<std::size_t>(bind.palette)];
    }
    if (bind.cbp != 0) {
        auto it = anyByDbp.find(bind.cbp);
        if (it != anyByDbp.end()) {
            aliased = true;
            return &uploads[it->second];
        }
    }
    aliased = false;
    return nullptr;
}

// CLUT CSM1 position within the upload payload: bits 3 and 4 of the flat
// entry index (bank * 16 + entry) are swapped (reference Extract.cpp:139;
// oracle-exact via rt_texture_extract, 45,056/45,056 px on FLD/003.BIN).
constexpr std::uint32_t swappedClutWordPos(std::uint32_t entry) noexcept {
    return (entry & ~0x18u) | ((entry & 0x08u) << 1) | ((entry & 0x10u) >> 1);
}

// GS alpha is 0..128 (0x80 opaque); host RGBA8 wants min(255, a*255/128)
// (Extract.cpp:346-347).
constexpr std::uint8_t gsAlphaTo8(std::uint32_t a) noexcept {
    return static_cast<std::uint8_t>(std::min(255u, a * 255u / 128u));
}

// One CLUT word (CT32 upload payload) to logical RGBA before alpha scaling.
// PSMCT24 storage carries no alpha: AEM makes an all-zero RGB entry
// transparent and everything else opaque 0x80 (Extract.h:20).
constexpr std::uint32_t clutWordRgba(std::uint32_t w, bool cpsmCt24) noexcept {
    if (!cpsmCt24) return w;
    const std::uint32_t rgb = w & 0x00FFFFFFu;
    return rgb | ((rgb == 0u ? 0u : 0x80u) << 24);
}

void storeRgba(std::uint8_t* out, std::uint32_t c) noexcept {
    out[0] = static_cast<std::uint8_t>(c & 0xFFu);
    out[1] = static_cast<std::uint8_t>((c >> 8) & 0xFFu);
    out[2] = static_cast<std::uint8_t>((c >> 16) & 0xFFu);
    out[3] = gsAlphaTo8((c >> 24) & 0xFFu);
}

} // namespace

expected<FieldTextureDecode, FieldTileError>
decodeFieldTextures(std::span<const std::uint8_t> tileFile) {
    auto uploadsResult = fieldTextureCatalog(tileFile);
    if (!uploadsResult) {
        return uploadsResult.error();
    }
    const auto& uploads = *uploadsResult;

    FieldTextureDecode out;
    out.pageIndexByUpload.assign(uploads.size(), -1);

    std::unordered_map<std::uint32_t, std::size_t> clutByTag;
    std::unordered_map<std::uint32_t, std::size_t> anyByDbp;
    for (std::size_t i = 0; i < uploads.size(); ++i) {
        anyByDbp.emplace(uploads[i].dbp, i);  // first upload on a page wins
        if (uploads[i].isClut) {
            clutByTag.emplace(uploads[i].tagOffset, i);
        }
    }

    // Material bindings from the cell chains; first binding wins. A failing
    // strip walk only leaves pages without CBP context.
    std::unordered_map<std::size_t, MaterialBind> bound;
    if (auto cells = extractCellStrips(tileFile)) {
        for (const FieldCellStrips& cell : *cells) {
            for (const FieldCellMaterial& material : cell.materials) {
                if (material.textureIndex >= 0 && material.tex0Cbp != 0) {
                    MaterialBind bind;
                    bind.palette = material.paletteIndex;
                    bind.cbp = material.tex0Cbp;
                    bind.csa = material.tex0Csa;
                    bound.emplace(static_cast<std::size_t>(material.textureIndex), bind);
                }
            }
        }
    }

    for (std::size_t i = 0; i < uploads.size(); ++i) {
        const FieldTexture& upload = uploads[i];
        if (upload.isClut) {
            continue;
        }
        out.pageIndexByUpload[i] = static_cast<std::int32_t>(out.pages.size());
        FieldTexturePage page;
        page.tagOffset = upload.tagOffset;
        page.dbp = upload.dbp;
        page.psm = upload.psm;
        page.width = upload.width;
        page.height = upload.height;
        const std::size_t texels = static_cast<std::size_t>(upload.width) * upload.height;
        page.rgba.resize(texels * 4);

        if (upload.psm == 0x00 || upload.psm == 0x01) {
            const bool ct24 = upload.psm == 0x01;
            for (std::size_t k = 0; k < texels; ++k) {
                std::uint32_t c;
                if (ct24) {
                    const std::uint8_t* px = tileFile.data() + upload.dataOffset + k * 3;
                    c = static_cast<std::uint32_t>(px[0]) | (static_cast<std::uint32_t>(px[1]) << 8) |
                        (static_cast<std::uint32_t>(px[2]) << 16);
                } else {
                    c = readU32LE(tileFile, upload.dataOffset + k * 4);
                }
                storeRgba(&page.rgba[k * 4], clutWordRgba(c, ct24));
            }
            out.pages.push_back(std::move(page));
            continue;
        }

        MaterialBind bind;
        auto bindIt = bound.find(i);
        if (bindIt != bound.end()) {
            bind = bindIt->second;
        }
        bool aliased = false;
        const FieldTexture* clut = attachClut(uploads, clutByTag, anyByDbp, bind,
                                             upload.clutOffset, aliased);
        if (clut != nullptr) {
            const std::size_t words = clut->dataBytes / 4;
            page.clutWords.reserve(words);
            for (std::size_t k = 0; k < words; ++k) {
                page.clutWords.push_back(readU32LE(tileFile, clut->dataOffset + k * 4));
            }
            page.paletteResolved = !aliased;
            page.paletteAliased = aliased;
            if (aliased) {
                ++out.clutAliasHits;
            } else {
                ++out.clutHits;
            }
        } else {
            ++out.clutMisses;
        }
        out.totalIndexPixels += texels;
        if (page.paletteResolved) {
            out.rgb8Pixels += texels;
        }
        const bool fourBit = upload.psm == 0x14;
        const bool cpsmCt24 = false;  // every observed CLUT writes PSMCT32
        for (std::size_t k = 0; k < texels; ++k) {
            std::uint32_t index;
            if (fourBit) {
                const std::uint8_t byte = tileFile[upload.dataOffset + k / 2];
                index = (k & 1u) == 0u ? (byte & 0x0Fu) : (byte >> 4);
            } else {
                index = tileFile[upload.dataOffset + k];
            }
            std::uint32_t pos;
            if (fourBit) {
                pos = bind.csa * 16u + index;
            } else {
                const std::uint32_t raw = (bind.csa * 16u + index) & 0x1FFu;
                pos = std::min(raw >> 4, 15u) * 16u + (raw & 0x0Fu);  // clamp to last bank
            }
            std::uint32_t c;
            if (clut != nullptr) {
                const std::uint32_t at = swappedClutWordPos(pos);
                c = at < page.clutWords.size() ? clutWordRgba(page.clutWords[at], cpsmCt24) : 0u;
            } else {
                const std::uint32_t gray = fourBit ? index * 17u : index;
                c = gray | (gray << 8) | (gray << 16) | (0x80u << 24);
            }
            storeRgba(&page.rgba[k * 4], c);
        }
        page.bank = bind.csa;
        out.pages.push_back(std::move(page));
    }
    return out;
}

expected<FieldTexturedTerrain, FieldTileError>
buildFieldTexturedTerrain(std::span<const std::uint8_t> tileFile, float scale) {
    auto cellsResult = extractCellStrips(tileFile);
    if (!cellsResult) {
        return cellsResult.error();
    }
    auto decodeResult = decodeFieldTextures(tileFile);
    if (!decodeResult) {
        return decodeResult.error();
    }

    FieldTexturedTerrain out;
    out.clutHits = decodeResult->clutHits;
    out.clutAliasHits = decodeResult->clutAliasHits;
    out.clutMisses = decodeResult->clutMisses;
    const std::vector<std::int32_t> pageIndexByUpload = decodeResult->pageIndexByUpload;
    auto& patch = out.patch;
    patch.pages = std::move(decodeResult->pages);

    std::unordered_map<std::uint32_t, std::uint32_t> textureUses;
    for (const FieldCellStrips& cell : *cellsResult) {
        bool contributed = false;
        for (const FieldStrip& strip : cell.strips) {
            const FieldCellMaterial* material = nullptr;
            if (strip.materialIndex >= 0 &&
                static_cast<std::size_t>(strip.materialIndex) < cell.materials.size()) {
                material = &cell.materials[static_cast<std::size_t>(strip.materialIndex)];
            } else if (!cell.materials.empty()) {
                material = &cell.materials.front();
            }
            std::int32_t pageId = -1;
            if (material != nullptr && material->textureIndex >= 0 &&
                static_cast<std::size_t>(material->textureIndex) < pageIndexByUpload.size()) {
                pageId = pageIndexByUpload[static_cast<std::size_t>(material->textureIndex)];
            }
            if (material != nullptr && material->textureIndex >= 0) {
                ++textureUses[static_cast<std::uint32_t>(material->textureIndex)];
            }
            if (strip.vertices.size() < 3) {
                continue;
            }
            contributed = true;
            const std::uint32_t base = static_cast<std::uint32_t>(patch.positions.size());
            for (const FieldVertex& vertex : strip.vertices) {
                patch.positions.push_back({vertex.x * scale, vertex.height * scale, vertex.y * scale});
                // WorldStrip's RGB output is the set-C lighting basis in the
                // current field streams.  The VU writes 0..128-ish values to
                // GS RGBAQ; Vulkan vertex colours are normalized instead.
                patch.colours.push_back({vertex.colC[0] / 128.0f,
                                         vertex.colC[1] / 128.0f,
                                         vertex.colC[2] / 128.0f,
                                         1.0f});
                patch.uvs.push_back({vertex.s, vertex.t});
                out.maxAbsU = std::max(out.maxAbsU, std::abs(vertex.s));
                out.maxAbsT = std::max(out.maxAbsT, std::abs(vertex.t));
            }
            // GIF PRIM 0x27c topic 4 = TriangleStrip (EmuTC PRIM bits 0..2);
            // expand to consecutive triples.
            for (std::size_t j = 2; j < strip.vertices.size(); ++j) {
                patch.indices.push_back(base + static_cast<std::uint32_t>(j - 2));
                patch.indices.push_back(base + static_cast<std::uint32_t>(j - 1));
                patch.indices.push_back(base + static_cast<std::uint32_t>(j));
                patch.triangleTexture.push_back(pageId);
                if (pageId < 0) {
                    ++out.unresolvedTriangles;
                }
                const auto& p0 = patch.positions[base + static_cast<std::uint32_t>(j - 2)];
                const auto& p1 = patch.positions[base + static_cast<std::uint32_t>(j - 1)];
                const auto& p2 = patch.positions[base + static_cast<std::uint32_t>(j)];
                const double ux = p1[0] - p0[0];
                const double uz = p1[2] - p0[2];
                const double vx = p2[0] - p0[0];
                const double vz = p2[2] - p0[2];
                out.projectedArea += std::abs(ux * vz - uz * vx) * 0.5;
                ++out.stripTriangles;
            }
        }
        if (contributed) {
            ++out.cells;
        }
    }
    out.distinctTextureIds = static_cast<std::uint32_t>(textureUses.size());
    for (const auto& entry : textureUses) {
        if (entry.second == 1) {
            ++out.uniqueTextureIds;
        }
        const std::size_t upload = entry.first;
        const std::int32_t page =
            upload < pageIndexByUpload.size() ? pageIndexByUpload[upload] : -1;
        if (page >= 0 && static_cast<std::size_t>(page) < patch.pages.size()) {
            out.textureArea += static_cast<std::uint64_t>(patch.pages[static_cast<std::size_t>(page)].width) *
                              patch.pages[static_cast<std::size_t>(page)].height;
        }
    }
    if (out.projectedArea > 0.0) {
        out.unsignedCoverage = static_cast<double>(out.textureArea) / out.projectedArea;
    }
    return out;
}

} // namespace rt::native::assets
