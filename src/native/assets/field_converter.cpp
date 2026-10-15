// FLD/COURSE field-tile header parsing and the currently-cracked field mesh
// descriptors.  Directory and texture conversion remain deferred.

#include "field_converter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

    if (readU32LE(tileFile, 0x00) != kHeaderBytes) {
        return FieldTileError::BadHeader;
    }

    FieldTileHeader header{
        .offA = readU32LE(tileFile, 0x04),
        .offB = readU32LE(tileFile, 0x08),
        .offC = readU32LE(tileFile, 0x0C),
        .fileSize = readU32LE(tileFile, 0x10),
    };
    if (static_cast<std::uint64_t>(header.fileSize) != actualFileSize ||
        header.offA < kHeaderBytes || header.offA > header.offB ||
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
    // offA) is unaffected by the 13 layout-variant tiles (offB < offA).
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

} // namespace rt::native::assets
