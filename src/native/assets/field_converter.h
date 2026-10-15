#pragma once

// FLD/COURSE field-tile header parsing and deferred mesh conversion.

#include "container.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <vector>

namespace rt::native::assets {

enum class FieldTileError : std::uint8_t {
    Ok = 0,
    Truncated,
    BadHeader,
    ChainDecode,
    BadBatch,
    MissingProvenance,
};

// C++20 expected-like result used for independently testable binary parsing.
template <class Value, class Error>
class expected {
public:
    expected(Value value) : storage_(std::move(value)) {}
    expected(Error error) : storage_(error) {}

    bool has_value() const noexcept { return std::holds_alternative<Value>(storage_); }
    explicit operator bool() const noexcept { return has_value(); }
    Value& operator*() { return std::get<Value>(storage_); }
    const Value& operator*() const { return std::get<Value>(storage_); }
    Value* operator->() { return &std::get<Value>(storage_); }
    const Value* operator->() const { return &std::get<Value>(storage_); }
    Error error() const { return std::get<Error>(storage_); }

private:
    std::variant<Value, Error> storage_;
};

struct FieldTileHeader {
    std::uint32_t offA = 0;
    std::uint32_t offB = 0;
    std::uint32_t offC = 0;
    std::uint32_t fileSize = 0;
};

// Decoded tile catalog mirroring the runtime loader semantics (see
// docs/native-reference/field-topology-status.md): a 65-entry sub-block
// directory relative to offA, a 256-entry attribute table, and a PMP model
// block (magic 0x600 old / 0x608 new) at offB with a relative section
// pointer table.
struct FieldTileCatalog {
    FieldTileHeader header{};
    std::vector<std::uint32_t> subBlocks;    // absolute file offsets of sub-block DMA tag words
    std::uint32_t attributeCount = 0;        // non-zero entries in the 256-entry attribute table
    std::uint32_t pmpMagic = 0;              // 0x600 or 0x608
    std::vector<std::uint32_t> pmpSections;  // absolute file offsets, strictly increasing
    std::uint32_t pmpExtensionOffset = 0;    // absolute; 0 for old-style (0x600) blocks
};

expected<FieldTileCatalog, FieldTileError>
catalogFieldTile(std::span<const std::uint8_t> tileFile);

// One decoded GPU packet from the PMP sections: header
// [0x8000|vertexCount, 0x20224000, 0x41, materialRef] followed by N
// float4 vertices (x, z, y, q) and N-2 face normals (nx, 1, nz).  Vertices
// form a triangle fan; UVs are a planar projection at the observed 40-unit
// grid spacing (authentic UV provenance still lives in the VU0 path).
struct FieldPacket {
    std::uint32_t sectionIndex = 0;
    std::uint32_t materialRef = 0;  // header word3, low 24 bits
    std::vector<Vertex> vertices;
    std::vector<std::array<float, 3>> faceNormals;
};

std::vector<FieldPacket>
extractFieldPackets(std::span<const std::uint8_t> tileFile,
                   const FieldTileCatalog& catalog);

// Y-up render-space terrain for the preview scene: packet fans converted to
// a triangle list, positions scaled (e.g. 0.1 for the app's render units),
// disc (x, height, depth) mapped to (x, y=height*scale, z=depth*scale).
struct FieldRenderTerrain {
    std::vector<std::array<float, 3>> positions;
    std::vector<std::uint32_t> indices;           // triangle list
    std::vector<std::uint32_t> triangleMaterials; // one materialRef per triangle
};

FieldRenderTerrain buildFieldRenderTerrain(std::span<const std::uint8_t> tileFile,
                                          const FieldTileCatalog& catalog, float scale);

// One host-to-local texture upload in the tile's DMA REF-tag chain at
// [headerSize, offA) (headerSize = header word0: 0x20 canonical, 0x160 for
// the 023.BIN header variant).  `psm` is the GS pixel-storage mode declared
// by BITBLTBUF — the proven FLD inventory is CLUT4 (0x14), CLUT8 (0x13),
// PSMCT24 (0x01) textures plus PSMCT32 (0x00) CLUTs; see
// docs/native-reference/field-textures.md.  Chain uploads carry absolute GS
// pages: base 0x285E + (tileName.lastDigit * 0x420), never rebased at load.
// CLUT-shaped uploads (CT32 16x2 / 16x16) are reported with isClut = true;
// every texture whose CLUT is the upload starting on the page right after
// its own pixels records that CLUT's tag offset in clutOffset (else 0 —
// textures sharing an earlier palette resolve via material-block CBP).
struct FieldTexture {
    std::uint32_t tagOffset = 0;   // absolute file offset of the DMA REF tag
    std::uint32_t gifOffset = 0;   // absolute offset of the A+D GIF tag
    std::uint32_t dataOffset = 0;  // absolute offset of the first pixel byte
    std::uint32_t dataBytes = 0;   // width * height * bpp / 8 (exact, never padded)
    std::uint32_t dbp = 0;        // GS destination page (256-byte blocks)
    std::uint32_t dbw = 0;        // GS destination buffer width (64-byte units)
    std::uint32_t psm = 0;        // GS pixel-storage mode (pixel format)
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t clutOffset = 0; // tagOffset of the trailing CLUT upload, 0 if none
    std::uint32_t clutDbp = 0;    // its GS page, 0 if none
    bool isClut = false;          // palette-shaped direct-colour upload
};

// Strict walk of the upload chain: REF tags (byte3 == 0x10) with inline
// payloads terminated by an END tag (byte3 == 0x70); every payload must be an
// A+D packet carrying BITBLTBUF/TRXPOS/TRXREG/TRXDIR (DIR == 0, DSAX/DSAY 0)
// followed by IMAGE packets whose byte count equals width*height*bpp/8
// exactly.  Malformed structure yields ChainDecode, overrun of offA or of the
// file yields Truncated, an implausible header yields BadHeader.  Proven
// byte-exact on all 3,097 uploads of the 64 disc FLD tiles.
expected<std::vector<FieldTexture>, FieldTileError>
fieldTextureCatalog(std::span<const std::uint8_t> tileFile);

expected<FieldTileHeader, FieldTileError>
parseFieldTileHeader(std::span<const std::uint8_t> tileFile);

struct FieldTileResult {
    FieldTileError err = FieldTileError::Ok;
    HostAssetContainer value;
    std::uint64_t errorOffset = 0;
};

FieldTileResult convertFieldTile(std::span<const std::uint8_t> tileFile,
                                 const std::string& sourceFile,
                                 const std::array<std::uint8_t, 32>& sha256);

} // namespace rt::native::assets
