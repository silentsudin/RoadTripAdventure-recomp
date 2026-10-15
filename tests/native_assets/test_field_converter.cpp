// Synthetic contract tests plus an optional local-disc integration check.

#include "../../src/native/assets/field_converter.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using rt::native::assets::FieldTexture;
using rt::native::assets::FieldTileError;
using rt::native::assets::FieldTileResult;
using rt::native::assets::convertFieldTile;
using rt::native::assets::extractFieldPackets;
using rt::native::assets::fieldTextureCatalog;
using rt::native::assets::parseFieldTileHeader;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool condition, const std::string& description) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cerr << "FAIL: " << description << '\n';
    }
}

using Bytes = std::vector<std::uint8_t>;
using Digest = std::array<std::uint8_t, 32>;

Digest testDigest() {
    Digest digest{};
    digest.fill(0xA5);
    return digest;
}

void setU32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
    for (std::size_t i = 0; i < 4; ++i) {
        bytes[offset + i] = static_cast<std::uint8_t>(value >> (8 * i));
    }
}

void setFloat(Bytes& bytes, std::size_t offset, float value) {
    setU32(bytes, offset, std::bit_cast<std::uint32_t>(value));
}

FieldTileResult convert(const Bytes& bytes) {
    return convertFieldTile(bytes, "synthetic.fld", testDigest());
}

void testTruncatedHeader() {
    const Bytes bytes(0x1F, 0);
    const FieldTileResult result = convert(bytes);
    check(result.err == FieldTileError::Truncated, "truncated header: returns Truncated");
}

void testOffsetPastFileIsBadHeader() {
    Bytes bytes(0x20, 0);
    setU32(bytes, 0x00, 0x20);
    setU32(bytes, 0x04, 0x21);  // offA exceeds file size
    setU32(bytes, 0x08, 0x21);
    setU32(bytes, 0x0C, 0x21);
    setU32(bytes, 0x10, 0x20);
    const FieldTileResult result = convert(bytes);
    check(result.err == FieldTileError::BadHeader, "out-of-file offA: returns BadHeader");
}

void testConvertsDescriptorVerticesAndSharedIndices() {
    // A mesh header's file positions are x,z,y.  A converter that leaves y/z
    // unswapped, ignores STQ, or drops the shared index buffer must fail this.
    constexpr std::size_t offA = 0x50AE;
    constexpr std::size_t vertexOffset = offA;
    Bytes bytes(vertexOffset + 2 * 6 * sizeof(float), 0);
    setU32(bytes, 0x00, 0x20);
    setU32(bytes, 0x04, offA);
    setU32(bytes, 0x08, offA);
    setU32(bytes, 0x0C, offA);
    setU32(bytes, 0x10, static_cast<std::uint32_t>(bytes.size()));

    setU32(bytes, 0x20, 0x10120106);
    setU32(bytes, 0x24, 0);
    setU32(bytes, 0x28, 0x00A80D6C);
    setU32(bytes, 0x2C, vertexOffset);
    setU32(bytes, 0x30, 2);

    const std::array<float, 12> fields{
        10.0f, 30.0f, 20.0f, 0.25f, 0.50f, 1.0f,
        -4.0f,  6.0f,  5.0f, 0.75f, 0.00f, 1.0f,
    };
    for (std::size_t i = 0; i < fields.size(); ++i) {
        setFloat(bytes, vertexOffset + i * sizeof(float), fields[i]);
    }
    bytes[0x50A6] = 1;
    bytes[0x50A8] = 1;

    const FieldTileResult result = convert(bytes);
    check(result.err == FieldTileError::Ok, "descriptor mesh: converts");
    if (result.err != FieldTileError::Ok) {
        return;
    }
    check(result.value.meshes.size() == 1, "descriptor mesh: one mesh");
    if (result.value.meshes.empty()) {
        return;
    }
    const auto& mesh = result.value.meshes.front();
    check(mesh.vertices.size() == 2, "descriptor mesh: vertex count");
    check(mesh.indices == std::vector<std::uint32_t>({1, 1, 0, 0}),
          "descriptor mesh: shared u16 index buffer");
    if (mesh.vertices.size() == 2) {
        const auto& first = mesh.vertices[0];
        const auto& second = mesh.vertices[1];
        check(first.px == 10.0f && first.py == 20.0f && first.pz == 30.0f,
              "descriptor mesh: swaps file x,z,y to authored x,y,z");
        check(first.u == 0.25f && first.v == 0.50f,
              "descriptor mesh: copies STQ s,t to UV");
        check(second.px == -4.0f && second.py == 5.0f && second.pz == 6.0f,
              "descriptor mesh: decodes subsequent vertex");
    }
}

void testRejectsMeshVertexRangeOutsideFile() {
    constexpr std::size_t offA = 0x50A6;
    Bytes bytes(offA, 0);
    setU32(bytes, 0x00, 0x20);
    setU32(bytes, 0x04, offA);
    setU32(bytes, 0x08, offA);
    setU32(bytes, 0x0C, offA);
    setU32(bytes, 0x10, static_cast<std::uint32_t>(bytes.size()));
    setU32(bytes, 0x20, 0x10120106);
    setU32(bytes, 0x24, 0);
    setU32(bytes, 0x2C, static_cast<std::uint32_t>(offA));
    setU32(bytes, 0x30, 1);

    const FieldTileResult result = convert(bytes);
    check(result.err == FieldTileError::Truncated,
          "descriptor mesh: vertex range outside file is rejected");
    check(result.errorOffset == offA,
          "descriptor mesh: out-of-file vertex range identifies vertex offset");
}

// Minimal synthetic tile honouring the decoded layout: header, 65-entry
// sub-block directory relative to offA, 256-entry attribute table, DMA-tag
// sub-block region, and a PMP block (magic + strictly increasing relative
// section table + extension word at +0x180 for 0x608).
Bytes buildCatalogTile(std::uint32_t magic, bool monotonicDirectory) {
    const auto pushU32 = [](Bytes& bytes, std::uint32_t value) {
        bytes.push_back(static_cast<std::uint8_t>(value & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
        bytes.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    };
    const std::uint32_t offA = 0x20;
    const std::uint32_t subRegionBytes = 0x80;
    const std::uint32_t offB = offA + 65 * 4 + 256 * 4 + subRegionBytes;
    const std::uint32_t pmpSpan = 0x320;
    const std::uint32_t offC = offB + pmpSpan;

    Bytes bytes;
    pushU32(bytes, 0x20);
    pushU32(bytes, offA);
    pushU32(bytes, offB);
    pushU32(bytes, offC);
    pushU32(bytes, offC + 8);
    pushU32(bytes, 0);  // header reserved words (region is 0x20 bytes)
    pushU32(bytes, 0);
    pushU32(bytes, 0);
    for (std::uint32_t entry = 0; entry < 65; ++entry) {
        std::uint32_t value = 0;
        if (entry == 0) value = monotonicDirectory ? 0x40 : 0x80;
        if (entry == 1) value = monotonicDirectory ? 0x80 : 0x40;
        pushU32(bytes, value);
    }
    for (std::uint32_t entry = 0; entry < 256; ++entry) {
        pushU32(bytes, entry < 2 ? 0xABC000u + entry : 0);
    }
    while (bytes.size() < offB) pushU32(bytes, 0x10000259);
    pushU32(bytes, magic);
    pushU32(bytes, 0x20);
    pushU32(bytes, 0xB0);
    pushU32(bytes, 0x220);
    while (bytes.size() < offB + 0x20) pushU32(bytes, 0);
    // One packet in section 0: header (5 verts) + 5 vertices + 3 normals.
    pushU32(bytes, 0x8005);
    pushU32(bytes, 0x20224000);
    pushU32(bytes, 0x41);
    pushU32(bytes, 0x00100651);
    for (int v = 0; v < 5; ++v) {
        pushU32(bytes, 0x41200000u + static_cast<std::uint32_t>(v));  // x=10.x
        pushU32(bytes, 0xc1200000u - static_cast<std::uint32_t>(v));  // z=-10.x
        pushU32(bytes, 0x41200000u + static_cast<std::uint32_t>(v));  // y=10.x
        pushU32(bytes, 0x3f800000u);                                   // q=1
    }
    for (int n = 0; n < 3; ++n) {
        pushU32(bytes, 0x3d4ccccdu);  // nx=0.05
        pushU32(bytes, 0x3f800000u);  // ny=1
        pushU32(bytes, 0x3d4ccccdu);  // nz=0.05
        pushU32(bytes, 0x3f800000u);  // w=1
    }
    while (bytes.size() < offB + 0x180) pushU32(bytes, 0);
    pushU32(bytes, 0x184);  // extension offset, relative to offB
    while (bytes.size() < offC + 8) pushU32(bytes, 0);
    return bytes;
}

void testCatalogValid() {
    const Bytes bytes = buildCatalogTile(0x608, true);
    const auto catalog = rt::native::assets::catalogFieldTile(bytes);
    check(catalog.has_value(), "catalog: valid synthetic tile parses");
    if (!catalog) return;
    check(catalog->subBlocks.size() == 2, "catalog: two non-zero directory entries found");
    check(catalog->subBlocks[0] == 0x20 + 0x40 && catalog->subBlocks[1] == 0x20 + 0x80,
          "catalog: sub-block targets are directory-relative");
    check(catalog->attributeCount == 2, "catalog: attribute count");
    check(catalog->pmpMagic == 0x608, "catalog: new-style magic");
    check(catalog->pmpSections.size() == 3, "catalog: three sections");
    const std::uint32_t offB = 0x20 + 65 * 4 + 256 * 4 + 0x80;
    check(catalog->pmpSections[0] == offB + 0x20 &&
          catalog->pmpSections[2] == offB + 0x220,
          "catalog: section targets are offB-relative");
    check(catalog->pmpExtensionOffset == offB + 0x184,
          "catalog: new-style extension offset resolved");

    const Bytes oldStyle = buildCatalogTile(0x600, true);
    const auto oldCatalog = rt::native::assets::catalogFieldTile(oldStyle);
    check(oldCatalog.has_value() && oldCatalog->pmpMagic == 0x600 &&
          oldCatalog->pmpExtensionOffset == 0,
          "catalog: old-style magic accepted with no extension");
}

void testCatalogRejectsBadShapes() {
    const Bytes badMagic = buildCatalogTile(0x601, true);
    check(rt::native::assets::catalogFieldTile(badMagic).error() ==
              FieldTileError::BadHeader,
          "catalog: unknown PMP magic rejected");
    const Bytes nonMonotonic = buildCatalogTile(0x608, false);
    check(rt::native::assets::catalogFieldTile(nonMonotonic).error() ==
              FieldTileError::BadHeader,
          "catalog: non-monotonic directory rejected");
}

void testExtractsPacketsFromSyntheticTile() {
    const Bytes bytes = buildCatalogTile(0x608, true);
    const auto catalog = rt::native::assets::catalogFieldTile(bytes);
    check(catalog.has_value(), "packets: synthetic tile catalogs");
    if (!catalog) return;
    const auto packets = rt::native::assets::extractFieldPackets(bytes, *catalog);
    check(packets.size() == 1, "packets: one packet extracted");
    if (packets.empty()) return;
    check(packets[0].vertices.size() == 5, "packets: five vertices");
    check(packets[0].faceNormals.size() == 3, "packets: N-2 face normals");
    check(packets[0].materialRef == 0x100651, "packets: material ref");
    check(packets[0].sectionIndex == 0, "packets: section index");
    const auto& v = packets[0].vertices[0];
    check(std::abs(v.px - 10.0f) < 0.01f && std::abs(v.pz + 10.0f) < 0.01f,
          "packets: vertex float decode");
    check(std::abs(v.u - 0.25f) < 0.01f, "packets: planar UV at grid spacing");
}

void testBuildsRenderTerrain() {
    const Bytes bytes = buildCatalogTile(0x608, true);
    const auto catalog = rt::native::assets::catalogFieldTile(bytes);
    check(catalog.has_value(), "terrain: synthetic tile catalogs");
    if (!catalog) return;
    const auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
    check(terrain.positions.size() == 5, "terrain: five positions");
    check(terrain.indices.size() == 9, "terrain: fan expanded to triangle list");
    check(terrain.triangleMaterials.size() == 3 &&
          terrain.triangleMaterials[0] == 0x100651, "terrain: per-triangle materials");
    // Height (disc float index 1) lands in y: x=10, h=-10, depth=10 at 0.1.
    check(std::abs(terrain.positions[0][0] - 1.0f) < 0.01f &&
          std::abs(terrain.positions[0][1] + 1.0f) < 0.01f &&
          std::abs(terrain.positions[0][2] - 1.0f) < 0.01f,
          "terrain: y-up coordinate mapping at scale");
}

// ── fieldTextureCatalog ──────────────────────────────────────────────────
//
// Grammar proven on all 3,097 uploads of the 64 disc FLD tiles (see
// docs/native-reference/field-textures.md): within [headerSize, offA) a DMA
// REF-tag chain (byte3 0x10, inline payload qwc*16) of A+D GIF packets with
// BITBLTBUF/TRXPOS/TRXREG/TRXDIR (DIR 0, DSAX/DSAY 0) plus IMAGE data,
// terminated by an END tag (byte3 0x70).

std::filesystem::path discRoot();

void appendWord(Bytes& bytes, std::uint32_t value) {
    bytes.push_back(static_cast<std::uint8_t>(value & 0xFF));
    bytes.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    bytes.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
}

void appendDword(Bytes& bytes, std::uint64_t value) {
    for (std::size_t i = 0; i < 8; ++i) {
        bytes.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xFF));
    }
}

// One texture upload as its own REF frame; returns the frame's tag offset.
std::size_t appendUpload(Bytes& bytes, std::uint32_t dbp, std::uint32_t dbw,
                        std::uint32_t psm, std::uint32_t width, std::uint32_t height,
                        std::uint32_t dataBytes, std::size_t& imageTagOffsetOut) {
    const std::size_t dataQuads = dataBytes / 16;
    const std::uint32_t qwc = 5 + static_cast<std::uint32_t>(1 + dataQuads);
    const std::size_t tagOffset = bytes.size();
    appendWord(bytes, (0x10u << 24) | qwc);  // REF tag, inline payload
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    // A+D setup: NLOOP 4, EOP, NREG 1; REGS descriptor low nibble 0xE.
    appendDword(bytes, 4ull | (1ull << 15) | (1ull << 60));
    appendDword(bytes, 0xEull);
    appendDword(bytes, (static_cast<std::uint64_t>(dbw) << 48) |
                           (static_cast<std::uint64_t>(dbp) << 32) |
                           (static_cast<std::uint64_t>(psm) << 56));
    appendDword(bytes, 0x50);  // BITBLTBUF
    appendDword(bytes, 0);
    appendDword(bytes, 0x51);  // TRXPOS (whole image: DSAX/DSAY zero)
    appendDword(bytes, (static_cast<std::uint64_t>(height) << 32) | width);
    appendDword(bytes, 0x52);  // TRXREG
    appendDword(bytes, 0);
    appendDword(bytes, 0x53);  // TRXDIR (DIR = 0)
    imageTagOffsetOut = bytes.size();
    appendDword(bytes, dataQuads | (1ull << 15) | (2ull << 58));  // IMAGE mode
    appendDword(bytes, 0);
    for (std::size_t quad = 0; quad < dataQuads; ++quad) {
        appendDword(bytes, 0x1122334455667700ull + quad);
        appendDword(bytes, 0x8899AABBCCDDEEFFull + quad);
    }
    return tagOffset;
}

// header + uploads + END + trailing strip up to offA.
Bytes makeChainFile(const Bytes& uploads, std::size_t stripBytes, std::size_t& offAOut) {
    Bytes bytes(0x20, 0);
    setU32(bytes, 0x00, 0x20);  // header size
    offAOut = 0x20 + uploads.size() + 16 + stripBytes;
    setU32(bytes, 0x04, static_cast<std::uint32_t>(offAOut));
    bytes.insert(bytes.end(), uploads.begin(), uploads.end());
    appendWord(bytes, 0x70u << 24);  // END tag
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    bytes.insert(bytes.end(), stripBytes, 0xAA);
    return bytes;
}

void testTextureCatalogSyntheticChain() {
    Bytes uploads;
    std::size_t image0 = 0, image1 = 0, image2 = 0;
    // T8 8x8 (64 B) at page 0x100 + its trailing CT32 16x2 palette (128 B)
    // at page 0x101; then a T4 8x8 (32 B) at 0x200 whose palette is shared
    // (no trailing CLUT upload) and so stays unlinked.
    const std::size_t tag0 = appendUpload(uploads, 0x100, 1, 0x13, 8, 8, 64, image0);
    const std::size_t tag1 = appendUpload(uploads, 0x101, 1, 0x00, 16, 2, 128, image1);
    const std::size_t tag2 = appendUpload(uploads, 0x200, 1, 0x14, 8, 8, 32, image2);
    std::size_t offA = 0;
    const Bytes bytes = makeChainFile(uploads, 0x40, offA);

    const auto catalog = fieldTextureCatalog(bytes);
    check(catalog.has_value(), "texcatalog: synthetic chain parses");
    if (!catalog) {
        return;
    }
    check(catalog->size() == 3, "texcatalog: three uploads reported");
    if (catalog->size() != 3) {
        return;
    }
    // Offsets returned by the fixture builder are into `uploads`; the catalog
    // reports absolute file offsets behind the 0x20 header.
    const std::size_t base = 0x20;
    const FieldTexture& t0 = (*catalog)[0];
    check(t0.tagOffset == tag0 + base && t0.gifOffset == tag0 + base + 16 &&
              t0.dataOffset == image0 + base + 16,
          "texcatalog: tag/gif/data offsets");
    check(!t0.isClut && t0.psm == 0x13 && t0.width == 8 && t0.height == 8 &&
              t0.dataBytes == 64 && t0.dbp == 0x100 && t0.dbw == 1,
          "texcatalog: T8 8x8 descriptor");
    check(t0.clutDbp == 0x101 && t0.clutOffset == tag1 + base,
          "texcatalog: texture paired with trailing palette");
    const FieldTexture& t1 = (*catalog)[1];
    check(t1.isClut && t1.psm == 0x00 && t1.width == 16 && t1.height == 2 &&
              t1.dataBytes == 128 && t1.tagOffset == tag1 + base,
          "texcatalog: CT32 palette record flagged");
    const FieldTexture& t2 = (*catalog)[2];
    check(!t2.isClut && t2.psm == 0x14 && t2.dataBytes == 32 && t2.tagOffset == tag2 + base,
          "texcatalog: T4 8x8 descriptor");
    check(t2.clutOffset == 0 && t2.clutDbp == 0,
          "texcatalog: shared palette stays unlinked");
}

void testTextureCatalogErrors() {
    Bytes uploads;
    std::size_t image = 0;
    appendUpload(uploads, 0x100, 1, 0x13, 8, 8, 64, image);
    std::size_t offA = 0;
    const Bytes good = makeChainFile(uploads, 0x10, offA);
    check(fieldTextureCatalog(good).has_value(), "texcatalog: valid chain parses");

    Bytes tooShort(12, 0);
    check(fieldTextureCatalog(tooShort).error() == FieldTileError::Truncated,
          "texcatalog: file shorter than header is Truncated");

    Bytes badHeader = good;
    setU32(badHeader, 0x00, 0x10);  // header size below 0x20
    check(fieldTextureCatalog(badHeader).error() == FieldTileError::BadHeader,
          "texcatalog: implausible header size is BadHeader");

    Bytes offPastFile = good;
    setU32(offPastFile, 0x04, static_cast<std::uint32_t>(good.size() + 0x10));
    check(fieldTextureCatalog(offPastFile).error() == FieldTileError::BadHeader,
          "texcatalog: offA past file is BadHeader");

    Bytes badTag = good;
    badTag[0x23] = 0x20;  // first frame tag id is not REF/END
    check(fieldTextureCatalog(badTag).error() == FieldTileError::ChainDecode,
          "texcatalog: unknown tag id is ChainDecode");

    Bytes noEnd = good;
    setU32(noEnd, 0x20 + uploads.size(), 0x10u << 24);  // END replaced by an empty REF
    check(fieldTextureCatalog(noEnd).error() == FieldTileError::ChainDecode,
          "texcatalog: chain without END tag is ChainDecode");

    Bytes overrun = good;
    setU32(overrun, 0x20, (0x10u << 24) | 0xFFFFu);  // payload reaches far past offA
    check(fieldTextureCatalog(overrun).error() == FieldTileError::Truncated,
          "texcatalog: payload past offA is Truncated");

    // IMAGE data overshooting w*h*bpp/8 breaks the byte-exact accounting that
    // holds for every disc upload, so the strict walk must reject it.
    Bytes lyingUploads;
    appendUpload(lyingUploads, 0x100, 1, 0x13, 8, 4, 64, image);  // 8x4 needs 32 B
    std::size_t lyingOffA = 0;
    const Bytes paddedTail = makeChainFile(lyingUploads, 0x10, lyingOffA);
    check(fieldTextureCatalog(paddedTail).error() == FieldTileError::ChainDecode,
          "texcatalog: IMAGE byte count must equal w*h*bpp/8 exactly");
}

void testTextureCatalogRealTiles() {
    const std::filesystem::path dir = discRoot() / "FLD";
    if (dir.empty() || !std::filesystem::exists(dir)) {
        std::cout << "test_field_converter: SKIP real FLD texture catalog (not found)\n";
        return;
    }

    std::size_t tiles = 0, uploads = 0, textures = 0, cluts = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() != ".BIN") {
            continue;
        }
        std::ifstream input(entry.path(), std::ios::binary);
        const Bytes bytes((std::istreambuf_iterator<char>(input)),
                         std::istreambuf_iterator<char>());
        const auto catalog = fieldTextureCatalog(bytes);
        check(catalog.has_value(), "texcatalog corpus: " + entry.path().filename().string());
        if (!catalog) {
            continue;
        }
        ++tiles;
        uploads += catalog->size();
        std::uint32_t minDbp = UINT32_MAX;
        for (const auto& texture : *catalog) {
            minDbp = std::min(minDbp, texture.dbp);
            if (texture.isClut) {
                ++cluts;
            } else {
                ++textures;
                if (texture.clutOffset != 0) {
                    // A linked CLUT must be a palette upload in this tile.
                    const FieldTexture* linked = nullptr;
                    for (const auto& candidate : *catalog) {
                        if (candidate.tagOffset == texture.clutOffset) {
                            linked = &candidate;
                            break;
                        }
                    }
                    check(linked != nullptr && linked->isClut,
                          "texcatalog corpus: clutOffset names a palette upload");
                }
                if (texture.clutDbp != 0) {
                    check(texture.clutDbp ==
                              texture.dbp + (texture.dataBytes + 255) / 256,
                          "texcatalog corpus: palette page follows texture pages");
                }
            }
        }
        // VRAM placement rule: chain base page 0x285E + lastDigit * 0x420.
        const int tileIndex = std::stoi(entry.path().stem().string());
        check(minDbp == 0x285Eu + static_cast<std::uint32_t>(tileIndex % 10) * 0x420u,
              "texcatalog corpus: min dbp matches tile base rule");
    }
    // Corpus-wide proven counts across all 64 tiles.
    check(tiles == 64, "texcatalog corpus: all 64 tiles parse");
    check(uploads == 3097, "texcatalog corpus: 3097 uploads");
    check(textures == 1685, "texcatalog corpus: 1685 textures");
    check(cluts == 1412, "texcatalog corpus: 1412 CLUT uploads");

    std::ifstream input0(dir / "000.BIN", std::ios::binary);
    const Bytes bytes0((std::istreambuf_iterator<char>(input0)),
                       std::istreambuf_iterator<char>());
    const auto catalog0 = fieldTextureCatalog(bytes0);
    check(catalog0.has_value() && catalog0->size() == 10, "texcatalog 000: 10 uploads");
    if (catalog0 && catalog0->size() == 10) {
        const FieldTexture& t = catalog0->at(0);
        check(t.dbp == 0x285E && t.psm == 0x13 && t.width == 64 && t.height == 64 &&
                  t.dataBytes == 4096 && t.clutDbp == 0x286E && !t.isClut,
              "texcatalog 000: first upload is the T8 64x64 tile at 0x285E");
        std::size_t tex = 0, clut = 0;
        std::uint64_t textureBytes = 0, uploadBytes = 0;
        for (const auto& texture : *catalog0) {
            uploadBytes += texture.dataBytes;
            if (texture.isClut) {
                ++clut;
            } else {
                ++tex;
                textureBytes += texture.dataBytes;
            }
        }
        check(tex == 7 && clut == 3, "texcatalog 000: 7 textures + 3 palettes");
        check(textureBytes == 18176 && uploadBytes == 19456,
              "texcatalog 000: 18176 texture bytes, 19456 bytes with palettes");
    }
    std::cout << "test_field_converter: texcatalog corpus tiles=" << tiles
              << " uploads=" << uploads << " textures=" << textures << " cluts=" << cluts
              << '\n';
}

std::filesystem::path discRoot() {
    if (const char* root = std::getenv("RT_DISC")) {
        return root;
    }
    if (const char* home = std::getenv("HOME")) {
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    }
    return {};
}

void testRealFieldTileWhenAvailable() {
    const std::filesystem::path path = discRoot() / "FLD/000.BIN";
    if (path.empty() || !std::filesystem::exists(path)) {
        std::cout << "test_field_converter: SKIP real FLD/000.BIN (not found)\n";
        return;
    }

    std::ifstream input(path, std::ios::binary);
    const Bytes bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    check(!bytes.empty(), "real FLD/000.BIN: file read");
    const auto header = parseFieldTileHeader(bytes);
    check(header.has_value(), "real FLD/000.BIN: header parses");
    if (!header) {
        return;
    }
    check(header->offA == 22528, "real FLD/000.BIN: offA");
    check(header->offB == 1778832, "real FLD/000.BIN: offB");
    check(header->offC == 2505600, "real FLD/000.BIN: offC");
    check(header->fileSize == 2506352, "real FLD/000.BIN: fileSize");

    const auto catalog = rt::native::assets::catalogFieldTile(bytes);
    check(catalog.has_value(), "real FLD/000.BIN: catalog parses");
    if (catalog) {
        check(catalog->subBlocks.size() == 65, "real FLD/000.BIN: 65 sub-blocks");
        check(catalog->attributeCount == 248, "real FLD/000.BIN: 248 attributes");
        check(catalog->pmpMagic == 0x608, "real FLD/000.BIN: new-style PMP magic");
        check(catalog->pmpSections.size() >= 16, "real FLD/000.BIN: section table");
        check(catalog->pmpExtensionOffset == header->offB + 0x2d838,
              "real FLD/000.BIN: extension offset");
        std::cout << "test_field_converter: FLD/000.BIN catalog sub-blocks="
                  << catalog->subBlocks.size() << " attributes="
                  << catalog->attributeCount << " pmp-sections="
                  << catalog->pmpSections.size() << '\n';
    }

    const auto packets = extractFieldPackets(bytes, *catalog);
    check(packets.size() == 6185, "real FLD/000.BIN: packet count");
    std::size_t packetVertices = 0;
    std::size_t normals = 0;
    for (const auto& packet : packets) {
        packetVertices += packet.vertices.size();
        normals += packet.faceNormals.size();
    }
    check(packetVertices == 25590, "real FLD/000.BIN: packet vertex count");
    check(normals == 13220, "real FLD/000.BIN: face normal count");
    std::cout << "test_field_converter: FLD/000.BIN packets=" << packets.size()
              << " vertices=" << packetVertices << " tris=" << normals << '\n';

    const auto terrain = rt::native::assets::buildFieldRenderTerrain(bytes, *catalog, 0.1f);
    check(terrain.indices.size() == 13220 * 3, "real FLD/000.BIN: render triangle count");
    bool extentsOk = !terrain.positions.empty();
    float minH = 1e9f, maxH = -1e9f;
    for (const auto& p : terrain.positions) {
        extentsOk = extentsOk && p[0] >= -0.1f && p[0] <= 160.1f &&
                    p[2] >= -0.1f && p[2] <= 160.1f;
        minH = std::min(minH, p[1]);
        maxH = std::max(maxH, p[1]);
    }
    check(extentsOk, "real FLD/000.BIN: terrain fits the 160-unit render field");
    check(minH > -10.f && maxH < 10.f, "real FLD/000.BIN: heights sane in render units");
    std::cout << "test_field_converter: FLD/000.BIN render tris=" << terrain.indices.size() / 3
              << " height range=[" << minH << ", " << maxH << "]\n";

    const FieldTileResult result =
        convertFieldTile(bytes, "FLD/000.BIN", testDigest());
    check(result.err == FieldTileError::Ok, "real FLD/000.BIN: converts");
    if (result.err != FieldTileError::Ok) {
        return;
    }

    std::size_t vertexCount = 0;
    bool sanePositions = true;
    for (const auto& mesh : result.value.meshes) {
        vertexCount += mesh.vertices.size();
        for (const auto& vertex : mesh.vertices) {
            sanePositions = sanePositions && std::isfinite(vertex.px) &&
                            std::isfinite(vertex.py) && std::isfinite(vertex.pz) &&
                            std::abs(vertex.px) <= 2000.0f && std::abs(vertex.py) <= 500.0f &&
                            std::abs(vertex.pz) <= 2000.0f;
        }
    }
    check(result.value.meshes.size() == 7, "real FLD/000.BIN: mesh count");
    check(vertexCount == 28, "real FLD/000.BIN: vertex count");
    check(sanePositions, "real FLD/000.BIN: decoded positions are sane");
    if (!result.value.meshes.empty() && !result.value.meshes.front().vertices.empty()) {
        const auto& vertex = result.value.meshes.front().vertices.front();
        std::cout << "test_field_converter: FLD/000.BIN meshes="
                  << result.value.meshes.size() << " vertices=" << vertexCount
                  << " first=(" << vertex.px << ", " << vertex.py << ", " << vertex.pz
                  << ") uv=(" << vertex.u << ", " << vertex.v << ")\n";
        const auto& vertices = result.value.meshes.front().vertices;
        for (std::size_t i = 1; i < std::min<std::size_t>(vertices.size(), 3); ++i) {
            const auto& sample = vertices[i];
            std::cout << "test_field_converter: FLD/000.BIN vertex[" << i << "]=("
                      << sample.px << ", " << sample.py << ", " << sample.pz << ") uv=("
                      << sample.u << ", " << sample.v << ")\n";
        }
    }
    if (!result.value.meshes.empty()) {
        const auto& indices = result.value.meshes.front().indices;
        const auto max = indices.empty() ? 0u : *std::max_element(indices.begin(), indices.end());
        const bool inRange = std::all_of(indices.begin(), indices.end(),
                                        [&result](std::uint32_t index) {
                                            return index < result.value.meshes.front().vertices.size();
                                        });
        check(indices.size() == 941, "real FLD/000.BIN: index count");
        check(max == 64, "real FLD/000.BIN: index maximum");
        std::cout << "test_field_converter: FLD/000.BIN indices=" << indices.size()
                  << " max=" << max << " first-mesh-in-range=" << (inRange ? "yes" : "no")
                  << " (shared grouping remains unresolved)\n";
    }
}

void testCatalogCorpusWhenAvailable() {
    const std::filesystem::path dir = discRoot() / "FLD";
    if (dir.empty() || !std::filesystem::exists(dir)) {
        std::cout << "test_field_converter: SKIP FLD catalog corpus (not found)\n";
        return;
    }
    std::size_t total = 0;
    std::size_t parsed = 0;
    std::size_t variantLayout = 0;
    std::size_t variantHeader = 0;
    std::size_t tilesWithGeometry = 0;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() != ".BIN") {
            continue;
        }
        std::ifstream input(entry.path(), std::ios::binary);
        const Bytes bytes((std::istreambuf_iterator<char>(input)),
                         std::istreambuf_iterator<char>());
        ++total;
        const auto tileCatalog = rt::native::assets::catalogFieldTile(bytes);
        if (tileCatalog) {
            ++parsed;
            if (!extractFieldPackets(bytes, *tileCatalog).empty()) {
                ++tilesWithGeometry;
            }
            continue;
        }
        // Distinguish the known variant population: files whose first word is
        // not the 0x20 header marker versus files with the marker but a
        // region-offset ordering that differs (offB < offA observed).
        if (bytes.size() >= 8 && rt::native::assets::parseFieldTileHeader(bytes).error() ==
                                     FieldTileError::BadHeader &&
            bytes[0] == 0x20) {
            ++variantLayout;
        } else {
            ++variantHeader;
        }
    }
    // 50/64 tiles are canonical; 13 use an alternate region ordering and one
    // (023.BIN) carries a 0x160 header.  Variants are documented in
    // docs/native-reference/field-topology-status.md and parsed later.
    check(total == 64, "catalog corpus: 64 tiles present");
    check(parsed == 50 && variantLayout == 13 && variantHeader == 1,
          "catalog corpus: 50 canonical / 13 layout variants / 1 header variant");
    check(tilesWithGeometry == parsed,
          "geometry corpus: every canonical tile yields packets");
    std::cout << "test_field_converter: catalog corpus canonical=" << parsed
              << " layout-variants=" << variantLayout
              << " header-variants=" << variantHeader
              << " with-geometry=" << tilesWithGeometry << " of " << total << '\n';
}

} // namespace

int main() {
    testTruncatedHeader();
    testOffsetPastFileIsBadHeader();
    testConvertsDescriptorVerticesAndSharedIndices();
    testRejectsMeshVertexRangeOutsideFile();
    testCatalogValid();
    testCatalogRejectsBadShapes();
    testExtractsPacketsFromSyntheticTile();
    testBuildsRenderTerrain();
    testTextureCatalogSyntheticChain();
    testTextureCatalogErrors();
    testTextureCatalogRealTiles();
    testRealFieldTileWhenAvailable();
    testCatalogCorpusWhenAvailable();

    std::cout << "test_field_converter: " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
