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
// docs/native-reference/field-topology-status.md): a 65-entry chain
// directory relative to offA, plus a PMP model block (magic 0x600 old /
// 0x608 new) at offB with a relative section pointer table.
struct FieldTileCatalog {
    FieldTileHeader header{};
    std::vector<std::uint32_t> subBlocks;    // absolute file offsets of per-cell DMA tag words
    std::uint32_t attributeCount = 0;        // legacy drift pin: non-zero words after the
                                             // directory — NOT a table (the attribute-table
                                             // reading was rejected; this region is cell-0
                                             // chain payload, see cell-vif-chains.md)
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

// ── per-cell VIF strip chains ────────────────────────────────────────────
//
// Directory entries 0..63 at offA are tile-relative offsets of prebuilt
// inline DMA CNT chains, one per terrain cell (see docs/native-reference/
// cell-vif-chains.md and gif-material-streams.md).  Each chain is a run of
// byte3==0x10 CNT tags (TTE: the tag's upper 64 bits carry the first two
// VIF codes) terminated by a zero-QWC RET tag (byte3 == 0x60).  Inside,
// UNPACK V3-32 @TOPS+0x192 uploads a 4- or 5-vector GIF A+D material packet
// (TEX1_2/TEX0_2/CLAMP_2/MIPTBP1_2), and every strip group is UNPACK V4-32
// @TOPS+0 (the output GIF tag), UNPACK V3-32 @TOPS+1 with NUM = 5 *
// vertexCount of interleaved 60-byte records, then MSCALF imm 8 (WorldStrip).
// Cells may switch texture mid-chain: every @0x192 upload re-arms the
// material kick the next WorldStrip emits, so strips inherit the most recent
// preceding header.

// One submitted WorldStrip vertex: position, three authored colour sets and
// the UV/alphaAdd triple, exactly as the VIF V3-32 @TOP+1 stream carries it.
struct FieldVertex {
    float x = 0.0f;
    float height = 0.0f;
    float y = 0.0f;
    float colA[3] = {0.0f, 0.0f, 0.0f};
    float colB[3] = {0.0f, 0.0f, 0.0f};
    float colC[3] = {0.0f, 0.0f, 0.0f};
    float s = 0.0f;
    float t = 0.0f;
    float alphaAdd = 0.0f;
};

// GIFtag64 field decode, verified against jak-project gs.gc gif-tag64:
// nloop bits 0..14, eop 15, id 32..45, pre 46, prim 47..57, flg 58..59,
// nreg 60..63.  (The task-level guesses "PRE bit 16 / PRIM bits 17..20" do
// not hold on disc.)  FLG: 0 PACKED, 1 REGLIST, 2 IMAGE.
inline std::uint32_t gifTagNloop(std::uint64_t tag) noexcept { return static_cast<std::uint32_t>(tag & 0x7FFFu); }
inline bool gifTagEop(std::uint64_t tag) noexcept { return ((tag >> 15) & 1u) != 0; }
inline bool gifTagPre(std::uint64_t tag) noexcept { return ((tag >> 46) & 1u) != 0; }
inline std::uint32_t gifTagPrim(std::uint64_t tag) noexcept { return static_cast<std::uint32_t>((tag >> 47) & 0x7FFu); }
inline std::uint32_t gifTagFlg(std::uint64_t tag) noexcept { return static_cast<std::uint32_t>((tag >> 58) & 0x3u); }
inline std::uint32_t gifTagNreg(std::uint64_t tag) noexcept { return static_cast<std::uint32_t>((tag >> 60) & 0xFu); }
inline std::uint32_t gifRegsNibble(std::uint64_t regs, std::uint32_t index) noexcept {
    return static_cast<std::uint32_t>((regs >> (4 * (index & 0xFu))) & 0xFu);
}

// The @0x192 GIF A+D packet captured for one material upload.  regs[0] is
// the assembled header GIF tag qword; regs[1..qwordCount] are the A+D data
// qwords, paired with the GIF register addresses (0x15 TEX1_2, 0x07 TEX0_2,
// 0x09 CLAMP_2, 0x35 MIPTBP1_2; regs[0]'s address slot holds the 0xE A+D
// descriptor nibble).  A NUM=4 header carries three data qwords (NLOOP 3);
// unused tail slots stay zero.  TEX0 fields follow the jak gs-tex0 layout
// (tbp0@0:14 tbw@14:6 psm@20:6 tw@26 th@30 tcc@34 tfx@35 cbp@37:14
// cpsm@51 csm@55 csa@56 cld@61).
struct FieldCellMaterial {
    std::array<std::uint64_t, 5> regs{};
    std::array<std::uint32_t, 5> regAddrs{};
    std::uint32_t qwordCount = 0;  // A+D data qwords in the packet (header NLOOP: 3 or 4)
    bool valid = false;
    std::uint64_t gifTag = 0;      // regs[0]
    std::uint64_t tex0 = 0;        // raw register values (0 when absent)
    std::uint64_t tex1 = 0;
    std::uint64_t clamp = 0;
    std::uint64_t miptbp1 = 0;
    std::uint32_t tex0Tbp0 = 0;
    std::uint32_t tex0Tbw = 0;
    std::uint32_t tex0Psm = 0;
    std::uint32_t tex0Tw = 0;
    std::uint32_t tex0Th = 0;
    std::uint32_t tex0Tcc = 0;
    std::uint32_t tex0Tfx = 0;
    std::uint32_t tex0Cbp = 0;
    std::uint32_t tex0Cpsm = 0;
    std::uint32_t tex0Csm = 0;
    std::uint32_t tex0Csa = 0;
    std::uint32_t tex0Cld = 0;
    int textureIndex = -1;  // index into fieldTextureCatalog() with dbp == tex0Tbp0
    int paletteIndex = -1;  // CLUT-shaped upload with dbp == tex0Cbp
};

// One WorldStrip submission: the GIF tag uploaded to @TOP+0 plus the
// vertices from the paired @TOP+1 upload.  The output stream is ST/RGBAQ/
// XYZF2 (regs nibbles 0x412) with PRIM 0x27c throughout the FLD corpus.
struct FieldStrip {
    std::uint64_t gifTag = 0;   // low qword of the @TOP+0 upload
    std::uint64_t gifRegs = 0;  // upper regs-descriptor qword (e.g. 0x412 nibbles)
    std::uint32_t cell = 0;
    int materialIndex = -1;     // index into the owning cell's materials
    std::vector<FieldVertex> vertices;
};

// One terrain cell: every material packet seen in its chain, in stream
// order, plus the strips (each tagged with the material index in effect).
struct FieldCellStrips {
    std::uint32_t cell = 0;
    FieldCellMaterial material;              // == materials.front() when present
    std::vector<FieldCellMaterial> materials;
    std::vector<FieldStrip> strips;
    std::uint32_t unknownCommands = 0;     // VIF words with unrecognized cmds
    std::uint32_t malformedUploads = 0;    // well-formed cmds with unexpected dest/num/pairing
    std::uint32_t nloopMismatches = 0;    // strips whose GIF NLOOP != NUM/5
    bool terminated = false;               // chain ended at RET(qwc=0)/END
    bool truncated = false;               // payload ran past the chain or file end
};

// Walks cells 0..63 through the directory.  An unused (zero) directory entry
// or a chain whose first DMA tag is not a CNT is skipped without failing the
// tile; a malformed strip stops the cell gracefully (truncated = true).
// Texture binding uses fieldTextureCatalog(); a failing catalogue only marks
// the links unresolved.  Pinned on disc: 000.BIN yields 64 cells,
// 6,669 strips and 25,944 vertices; the 64-tile corpus yields 4,090 cells
// with strips (4,096 cells total; six empty cells in 023.BIN), 418,392
// strips and 1,749,827 vertices, with every cell's first-header TEX0 page
// resolving to a tile upload.
expected<std::vector<FieldCellStrips>, FieldTileError>
extractCellStrips(std::span<const std::uint8_t> tileFile);

// ── texture-page decode ──────────────────────────────────────────────────
//
// GS VRAM stores PSMT8/PSMT4 pages block-swizzled (page 8 KiB: PSMT8
// 128x64 px of 16x16-px blocks, PSMT4 128x128 px of 16x32-px blocks,
// PCSX2 GSTables _blockTable8/_blockTable4 + column tables).  The host never
// needs that layout: the tile file carries GIF IMAGE transfer payloads, and
// an IMAGE transfer's payload is linear row-major within its box, while the
// sampler that later reads the page walks the same swizzle the write used.
// The two round-trip to identity at payload level, so decoding straight from
// the payload is exact.  Pinned oracle-verified (rt_texture_extract against
// FLD/003.BIN): 45,056/45,056 pixels RGB-exact for the four indexed pages
// with payload-linear pixels (PSMT4 low nibble first) and CLUT words read at
// swappedBits34(csa * 16 + entry); applying the VRAM swizzle to the payload
// instead scores 1.07% on the same images.
struct FieldTexturePage {
    std::uint32_t tagOffset = 0;   // upload key; the oracle join key is
                                   // tagOffset + 16 == sources.csv offset
    std::uint32_t dbp = 0;
    std::uint32_t psm = 0;        // 0x13 PSMT8, 0x14 PSMT4, 0x01 PSMCT24, 0x00 PSMCT32
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t bank = 0;       // CLUT bank (TEX0.CSA) baked into rgba
    bool paletteResolved = false; // CLUT-shaped upload attached
    bool paletteAliased = false;  // CBP names a non-CLUT upload (232.BIN alias)
    std::vector<std::uint32_t> clutWords;  // raw CLUT upload payload (swappedBits34 positions)
    std::vector<std::uint8_t> rgba;        // width*height*4, R at byte 0, alpha 0..255
};

struct FieldTextureDecode {
    std::vector<FieldTexturePage> pages;  // every non-CLUT upload, chain order
    std::vector<std::int32_t> pageIndexByUpload;  // parallel to fieldTextureCatalog();
                                                  // -1 at CLUT-shaped uploads
    std::uint32_t clutHits = 0;      // indexed pages with a CLUT-shaped attach
    std::uint32_t clutAliasHits = 0; // CBP page resolves to a non-CLUT upload
    std::uint32_t clutMisses = 0;    // no CLUT at all: grayscale fallback
    std::uint32_t totalIndexPixels = 0;  // PSMT8/PSMT4 page texels
    std::uint32_t rgb8Pixels = 0;       // indexed texels whose CLUT resolved via
                                        // a CLUT-shaped upload (tag or material)
};

// Replays fieldTextureCatalog() and decodes every texture upload to RGBA8.
// CLUT attach order: the trailing CLUT recorded in the upload, else the CBP
// bound by the first cell material that names this texture (banks come from
// that material's TEX0.CSA), else any upload sitting on the CBP page (the
// 232.BIN cbp=0x31B7 aliasing), else a grayscale fallback (counted as a
// miss).  Alpha follows the reference export: byte0 = R, GS 0..128 alpha
// scaled by min(255, a * 255 / 128), AEM-empty entries fully transparent.
expected<FieldTextureDecode, FieldTileError>
decodeFieldTextures(std::span<const std::uint8_t> tileFile);

// ── textured terrain patch ───────────────────────────────────────────────

// One triangle-list terrain with per-vertex page-fraction UVs and one
// texture-page id per triangle (-1 = unresolved; renderers should fall back
// to a white dummy).  Positions map (x, height, y) * scale exactly like
// buildFieldRenderTerrain.  UVs are the raw STQ s/t floats — GS page
// fractions (1.0 spans a texture; PCSX2 GetTexCoordsImpl proves
// pixel = st / q * 2^TW|TH); the q slot carries the additive alpha
// (alphaAdd) and is 1.0 for 99.97% of the corpus, so the stored s/t are
// already the page fractions the sampler uses.  Measured over the 64-tile
// corpus (1,749,827 vertices): u spans [-39.5, 39], t spans [-21.32,
// 21.0], and 50.6% of vertices land inside the unit page — REPEAT
// wrapping at integer bounds reproduces what the GS sees.
struct FieldTexturedPatch {
    std::vector<std::array<float, 3>> positions;
    std::vector<std::array<float, 2>> uvs;              // parallel to positions
    std::vector<std::uint32_t> indices;                 // triangle list
    std::vector<std::int32_t> triangleTexture;          // index into pages
    std::vector<FieldTexturePage> pages;
};

struct FieldTexturedTerrain {
    FieldTexturedPatch patch;
    std::uint32_t cells = 0;              // cells contributing at least one strip
    std::uint32_t stripTriangles = 0;    // triangles after strip expansion
    std::uint32_t distinctTextureIds = 0;  // distinct material texture ids
    std::uint32_t uniqueTextureIds = 0;    // ids used by exactly one texture
    std::uint64_t textureArea = 0;       // sum of page w*h over distinct ids
    double projectedArea = 0.0;          // sum of |triangle area| in the XZ plane
    double unsignedCoverage = 0.0;      // textureArea / projectedArea
    float maxAbsU = 0.0f;
    float maxAbsT = 0.0f;
    std::uint32_t clutHits = 0;
    std::uint32_t clutAliasHits = 0;
    std::uint32_t clutMisses = 0;
    std::uint32_t unresolvedTriangles = 0;
};

// Builds the textured patch from the cell strip chains: each WorldStrip is a
// TRI_STRIP (GIF PRIM 0x27c) expanded into consecutive triples, its vertices
// carrying position, s/t and alphaAdd; the triangle takes the texture page of
// the strip's bound material (materials switch mid-chain; each @0x192 header
// re-arms the next strips).  Strips whose material names no uploaded page
// get page id -1.
expected<FieldTexturedTerrain, FieldTileError>
buildFieldTexturedTerrain(std::span<const std::uint8_t> tileFile, float scale);

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
