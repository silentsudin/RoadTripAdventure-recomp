// FLD/COURSE field-tile header parsing and the currently-cracked field mesh
// descriptors.  Directory and texture conversion remain deferred.

#include "field_converter.h"

#include <algorithm>
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

std::uint32_t readU32LE(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
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
