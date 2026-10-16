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

// ── extractCellStrips ────────────────────────────────────────────────────
//
// Grammar proven on disc (docs/native-reference/cell-vif-chains.md and
// gif-material-streams.md): directory entries at offA are tile-relative chain
// starts; chains are byte3==0x10 CNT tags (TTE upper VIF codes) terminated
// by RET(qwc=0) (byte3==0x60); material packets are UNPACK V3-32 NUM 4/5
// @TOPS+0x192; strips are UNPACK V4-32 NUM=1 @TOPS+0 (GIF tag quad) +
// UNPACK V3-32 NUM=5*V @TOPS+1 + MSCALF imm 8.

using rt::native::assets::FieldCellMaterial;
using rt::native::assets::FieldCellStrips;
using rt::native::assets::FieldStrip;
using rt::native::assets::FieldVertex;
using rt::native::assets::extractCellStrips;
using rt::native::assets::gifTagEop;
using rt::native::assets::gifTagFlg;
using rt::native::assets::gifTagNloop;
using rt::native::assets::gifTagNreg;
using rt::native::assets::gifTagPre;
using rt::native::assets::gifTagPrim;
using rt::native::assets::gifRegsNibble;

// TEX0 selecting the synthetic T8 8x8 upload at page 0x100 with the 16x2
// palette at 0x101 (gs-tex0: tbp0@0:14 tbw@14:6 psm@20:6 tw@26 th@30
// tcc@34 tfx@35 cbp@37:14 cpsm@51 csm@55 csa@56 cld@61).
constexpr std::uint64_t kTestTex0 =
    0x100ull | (1ull << 14) | (0x13ull << 20) | (3ull << 26) | (3ull << 30) |
    (1ull << 34) | (0x101ull << 37) | (1ull << 61);

struct SyntheticCellTile {
    Bytes bytes;
    std::size_t offA = 0;
    std::size_t chainTag = 0;   // absolute offset of the cell-0 CNT tag
};

SyntheticCellTile buildCellTileBase() {
    Bytes uploads;
    std::size_t image0 = 0;
    std::size_t image1 = 0;
    appendUpload(uploads, 0x100, 1, 0x13, 8, 8, 64, image0);
    appendUpload(uploads, 0x101, 1, 0x00, 16, 2, 128, image1);

    SyntheticCellTile tile;
    tile.offA = 0x20 + uploads.size() + 16;  // after header, uploads and END
    Bytes bytes(0x20, 0);
    bytes.insert(bytes.end(), uploads.begin(), uploads.end());
    appendWord(bytes, 0x70u << 24);  // END tag closes the upload chain
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    // 65-entry directory relative to offA; entry 0 places the cell-0 chain
    // right after the directory itself.
    const std::uint32_t chainRel = 65 * 4;
    for (std::uint32_t entry = 0; entry < 65; ++entry) {
        appendWord(bytes, entry == 0 ? chainRel : 0);
    }
    tile.chainTag = tile.offA + chainRel;
    // CNT tag: TTE carries STCYCL 0x0101 + UNPACK V3-32 NUM=5 @TOPS+0x192.
    appendWord(bytes, (0x10u << 24) | 17u);
    appendWord(bytes, 0);
    appendWord(bytes, 0x01000101u);
    appendWord(bytes, 0x68058192u);
    // Material header: GIF tag, TEX1_2, TEX0_2, CLAMP_2, MIPTBP1_2.
    appendWord(bytes, 0x00008004u);
    appendWord(bytes, 0x10000000u);
    appendWord(bytes, 0x0000000Eu);
    appendWord(bytes, 0x00080128u);
    appendWord(bytes, 0x00000F80u);
    appendWord(bytes, 0x00000015u);
    appendWord(bytes, static_cast<std::uint32_t>(kTestTex0 & 0xFFFFFFFFu));
    appendWord(bytes, static_cast<std::uint32_t>(kTestTex0 >> 32));
    appendWord(bytes, 0x00000007u);
    appendWord(bytes, 0x00000000u);
    appendWord(bytes, 0x00000000u);
    appendWord(bytes, 0x00000009u);
    appendWord(bytes, 0x8A90A8A1u);
    appendWord(bytes, 0x2000000Au);
    appendWord(bytes, 0x00000035u);
    // Strip group: GIF tag (NLOOP 3, EOP, PRIM 0x27c, PACKED, NREG 3,
    // regs ST/RGBAQ/XYZF2), 15-vector vertex upload, MSCALF 8, NOP pad.
    appendWord(bytes, 0x6C018000u);
    appendWord(bytes, 0x00008003u);
    appendWord(bytes, 0x313E0000u);
    appendWord(bytes, 0x00000412u);
    appendWord(bytes, 0x313EC000u);
    appendWord(bytes, 0x680F8001u);
    // Three 15-float records: (x, height, y), colA, colB, colC, (s, t, alphaAdd).
    const std::array<float, 45> vertices{
        1.5f,    2.5f,    3.5f,    0.25f,  0.5f,    0.75f,   0.0625f, 0.125f,  0.1875f,
        0.2f,    0.4f,    0.6f,    0.3125f, 0.625f,  0.0f,
        -4.5f,   -3.5f,   -2.5f,   1.0f,   1.0f,    1.0f,    0.0f,    0.0f,    0.0f,
        0.8f,    0.6f,    0.4f,    1.0f,   -0.5f,   0.5f,
        40.0f,   50.0f,   60.0f,   0.125f, 0.375f,  0.625f,  0.0625f, 0.0625f, 0.0625f,
        0.2f,    0.4f,    0.6f,    0.75f,  -0.25f,  0.125f,
    };
    for (const float value : vertices) {
        appendWord(bytes, std::bit_cast<std::uint32_t>(value));
    }
    appendWord(bytes, 0x15000008u);  // MSCALF imm 8 (WorldStrip)
    appendWord(bytes, 0x00000000u);  // NOP FIFO padding (qword alignment)
    appendWord(bytes, 0x60000000u);  // RET(qwc = 0) terminator
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    const std::uint32_t fileSize = static_cast<std::uint32_t>(bytes.size());
    setU32(bytes, 0x00, 0x20);
    setU32(bytes, 0x04, static_cast<std::uint32_t>(tile.offA));
    setU32(bytes, 0x08, fileSize);  // offB: chains end with the file
    setU32(bytes, 0x0C, fileSize);  // offC
    setU32(bytes, 0x10, fileSize);
    tile.bytes = std::move(bytes);
    return tile;
}

void testCellStripsSynthetic() {
    const SyntheticCellTile tile = buildCellTileBase();
    const auto cells = extractCellStrips(tile.bytes);
    check(cells.has_value(), "cellstrips: synthetic tile parses");
    if (!cells) {
        return;
    }
    check(cells->size() == 1, "cellstrips: one populated cell entry");
    if (cells->size() != 1) {
        return;
    }
    const FieldCellStrips& cell = (*cells)[0];
    check(cell.cell == 0 && cell.terminated && !cell.truncated,
          "cellstrips: cell 0 terminated cleanly");
    check(cell.unknownCommands == 0 && cell.malformedUploads == 0 &&
              cell.nloopMismatches == 0,
          "cellstrips: diagnostics stay zero on the clean chain");
    check(cell.materials.size() == 1, "cellstrips: one material header");
    const FieldCellMaterial& material = cell.material;
    check(material.valid && material.qwordCount == 4,
          "cellstrips: material packet carries four A+D qwords");
    check(material.regs[0] == 0x1000000000008004ull &&
              material.regAddrs[0] == 0x0E && material.regAddrs[1] == 0x15 &&
              material.regAddrs[2] == 0x07 && material.regAddrs[3] == 0x09 &&
              material.regAddrs[4] == 0x35,
          "cellstrips: GIF tag plus TEX1/TEX0/CLAMP/MIPTBP1 addresses");
    check(material.tex1 == 0x00000F8000080128ull &&
              material.tex0 == kTestTex0 && material.clamp == 0 &&
              material.miptbp1 == 0x2000000A8A90A8A1ull,
          "cellstrips: raw register qwords");
    check(material.tex0Tbp0 == 0x100 && material.tex0Tbw == 1 &&
              material.tex0Psm == 0x13 && material.tex0Tw == 3 &&
              material.tex0Th == 3 && material.tex0Tcc == 1 &&
              material.tex0Tfx == 0 && material.tex0Cbp == 0x101 &&
              material.tex0Cpsm == 0 && material.tex0Csm == 0 &&
              material.tex0Csa == 0 && material.tex0Cld == 1,
          "cellstrips: TEX0 field decode");
    const auto textures = fieldTextureCatalog(tile.bytes);
    check(textures.has_value() && material.textureIndex >= 0 &&
              material.paletteIndex >= 0 &&
              (*textures)[material.textureIndex].dbp == 0x100 &&
              !(*textures)[material.textureIndex].isClut &&
              (*textures)[material.paletteIndex].dbp == 0x101 &&
              (*textures)[material.paletteIndex].isClut,
          "cellstrips: TEX0 page/palette bound to catalog uploads");

    check(cell.strips.size() == 1, "cellstrips: one strip");
    if (cell.strips.size() != 1) {
        return;
    }
    const FieldStrip& strip = cell.strips[0];
    check(strip.cell == 0 && strip.materialIndex == 0,
          "cellstrips: strip carries cell and material indices");
    check(gifTagNloop(strip.gifTag) == 3 && gifTagEop(strip.gifTag) &&
              !gifTagPre(strip.gifTag) && gifTagPrim(strip.gifTag) == 0x27C &&
              gifTagFlg(strip.gifTag) == 0 && gifTagNreg(strip.gifTag) == 3,
          "cellstrips: GIF tag field decode");
    check(gifRegsNibble(strip.gifRegs, 0) == 2 &&
              gifRegsNibble(strip.gifRegs, 1) == 1 &&
              gifRegsNibble(strip.gifRegs, 2) == 4,
          "cellstrips: regs descriptor is ST/RGBAQ/XYZF2 (0x412)");
    check(strip.vertices.size() == 3, "cellstrips: NUM 15 yields three vertices");
    if (strip.vertices.size() != 3) {
        return;
    }
    const FieldVertex& v0 = strip.vertices[0];
    check(v0.x == 1.5f && v0.height == 2.5f && v0.y == 3.5f,
          "cellstrips: position (x, height, y) decode");
    check(v0.colA[0] == 0.25f && v0.colA[1] == 0.5f && v0.colA[2] == 0.75f &&
              v0.colB[0] == 0.0625f && v0.colB[1] == 0.125f && v0.colB[2] == 0.1875f &&
              std::abs(v0.colC[0] - 0.2f) < 1e-6f && std::abs(v0.colC[1] - 0.4f) < 1e-6f &&
              std::abs(v0.colC[2] - 0.6f) < 1e-6f,
          "cellstrips: three colour-set decode");
    check(v0.s == 0.3125f && v0.t == 0.625f && v0.alphaAdd == 0.0f,
          "cellstrips: authentic UV/alphaAdd decode");
    const FieldVertex& v1 = strip.vertices[1];
    check(v1.x == -4.5f && v1.s == 1.0f && v1.t == -0.5f &&
              v1.colA[0] == 1.0f && v1.colB[0] == 0.0f,
          "cellstrips: second vertex record decode (60-byte stride)");
    const FieldVertex& v2 = strip.vertices[2];
    check(v2.x == 40.0f && v2.y == 60.0f && std::abs(v2.s - 0.75f) < 1e-6f &&
              v2.t == -0.25f,
          "cellstrips: third vertex record decode");
}

void testCellStripsErrorsAndDrift() {
    const Bytes tooShort(0x1F, 0);
    check(extractCellStrips(tooShort).error() == FieldTileError::Truncated,
          "cellstrips: file shorter than header is Truncated");

    SyntheticCellTile tile = buildCellTileBase();
    Bytes badHeader = tile.bytes;
    setU32(badHeader, 0x00, 0x10);  // implausible header size
    check(extractCellStrips(badHeader).error() == FieldTileError::BadHeader,
          "cellstrips: implausible header size is BadHeader");

    Bytes dirPastFile = tile.bytes;
    setU32(dirPastFile, 0x04, static_cast<std::uint32_t>(tile.bytes.size() - 8));
    check(extractCellStrips(dirPastFile).error() == FieldTileError::Truncated,
          "cellstrips: directory past end of file is Truncated");

    // A CNT tag whose payload overshoots the chain region truncates the cell
    // without failing the tile.
    Bytes paddedQwc = tile.bytes;
    setU32(paddedQwc, tile.chainTag, (0x10u << 24) | 0x4000u);
    const auto truncated = extractCellStrips(paddedQwc);
    check(truncated.has_value() && truncated->size() == 1,
          "cellstrips: over-large qwc keeps the tile parseable");
    if (truncated && truncated->size() == 1) {
        check((*truncated)[0].truncated && !(*truncated)[0].terminated &&
                  (*truncated)[0].strips.empty(),
              "cellstrips: over-large qwc truncates the cell gracefully");
    }

    // An honest qwc whose final UNPACK overruns the remaining stream keeps
    // the material header but stops the strip.
    Bytes overrunUpload = tile.bytes;
    const std::size_t v3Cmd = tile.chainTag + 16 + 60 + 4 + 16;
    setU32(overrunUpload, v3Cmd, 0x68288001u);  // NUM 40 needs 480 bytes
    setU32(overrunUpload, tile.chainTag, (0x10u << 24) | 17u);
    const auto stopped = extractCellStrips(overrunUpload);
    check(stopped.has_value() && stopped->size() == 1,
          "cellstrips: payload past chain end keeps the tile parseable");
    if (stopped && stopped->size() == 1) {
        const FieldCellStrips& cell = (*stopped)[0];
        check(cell.truncated && cell.strips.empty() && cell.materials.size() == 1,
              "cellstrips: malformed strip stops the cell, header survives");
    }

    // A second directory entry pointing at garbage is skipped, not fatal.
    Bytes bogusEntry = tile.bytes;
    setU32(bogusEntry, 0x08, static_cast<std::uint32_t>(tile.bytes.size()));  // offB slack
    setU32(bogusEntry, 0x0C, static_cast<std::uint32_t>(tile.bytes.size()));
    setU32(bogusEntry, 0x10, static_cast<std::uint32_t>(tile.bytes.size()));
    setU32(bogusEntry, tile.offA + 4 * sizeof(std::uint32_t),
           static_cast<std::uint32_t>(tile.bytes.size()) + 0x40 - static_cast<std::uint32_t>(tile.offA));
    const auto skipped = extractCellStrips(bogusEntry);
    check(skipped.has_value() && skipped->size() == 1 && (*skipped)[0].cell == 0,
          "cellstrips: unparseable directory entry is skipped without failing the tile");

    // Without staged uploads the material stays unbound but extraction works:
    // rebuild the tile with the upload frames (and their offA shift) removed.
    const SyntheticCellTile bare = buildCellTileBase();
    Bytes unbound;
    unbound.insert(unbound.end(), bare.bytes.begin(), bare.bytes.begin() + 0x20);
    unbound.insert(unbound.end(), bare.bytes.begin() + bare.offA - 16, bare.bytes.end());
    const std::uint32_t newOffA = 0x20 + 16;  // just the END tag
    setU32(unbound, 0x04, newOffA);
    setU32(unbound, 0x08, static_cast<std::uint32_t>(unbound.size()));
    setU32(unbound, 0x0C, static_cast<std::uint32_t>(unbound.size()));
    setU32(unbound, 0x10, static_cast<std::uint32_t>(unbound.size()));
    const auto loose = extractCellStrips(unbound);
    check(loose.has_value() && loose->size() == 1 &&
              (*loose)[0].strips.size() == 1 &&
              (*loose)[0].material.textureIndex == -1 &&
              (*loose)[0].material.paletteIndex == -1,
          "cellstrips: missing upload chain leaves material unbound but extracts strips");
}

void testCellStripsRealTile() {
    const std::filesystem::path path = discRoot() / "FLD/000.BIN";
    if (path.empty() || !std::filesystem::exists(path)) {
        std::cout << "test_field_converter: SKIP real FLD/000.BIN cell strips (not found)\n";
        return;
    }
    std::ifstream input(path, std::ios::binary);
    const Bytes bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const auto cells = extractCellStrips(bytes);
    check(cells.has_value(), "cellstrips 000: tile parses");
    if (!cells) {
        return;
    }
    const auto textures = fieldTextureCatalog(bytes);

    std::size_t strips = 0, vertices = 0, headers = 0, switching = 0, withMat = 0;
    std::size_t texBinds = 0, palBinds = 0, unknown = 0, malformed = 0, nloopMiss = 0;
    bool allTerminated = cells->size() == 64;
    for (const FieldCellStrips& cell : *cells) {
        strips += cell.strips.size();
        for (const FieldStrip& strip : cell.strips) {
            vertices += strip.vertices.size();
        }
        headers += cell.materials.size();
        if (!cell.materials.empty()) {
            ++withMat;
            if (cell.materials.size() > 1) {
                ++switching;
            }
            if (cell.material.textureIndex >= 0) {
                ++texBinds;
            }
            if (cell.material.paletteIndex >= 0) {
                ++palBinds;
            }
        }
        unknown += cell.unknownCommands;
        malformed += cell.malformedUploads;
        nloopMiss += cell.nloopMismatches;
        allTerminated = allTerminated && cell.terminated && !cell.truncated;
    }
    // Pinned by the /tmp probe over the disc copy (2026-10-16).
    check(cells->size() == 64, "cellstrips 000: 64 cells");
    check(strips == 6669, "cellstrips 000: 6669 strips");
    check(vertices == 25944, "cellstrips 000: 25944 vertices");
    check(headers == 162, "cellstrips 000: 162 material headers");
    check(withMat == 64, "cellstrips 000: every cell has a material header");
    check(switching == 64, "cellstrips 000: every cell switches material at least once");
    check(texBinds == 64 && palBinds == 64,
          "cellstrips 000: first-header TEX0 page+palette bind 64/64");
    check(unknown == 0 && malformed == 0 && nloopMiss == 0 && allTerminated,
          "cellstrips 000: clean diagnostics, every chain RET-terminated");

    // Cell 0 end-to-end: 146 strips / 564 vertices / 3 headers, the first
    // selecting the PSMT4 128x128 page 0x2880 with palette 0x28a0.
    const FieldCellStrips& cell0 = cells->at(0);
    check(cell0.strips.size() == 146, "cellstrips 000 cell0: 146 strips");
    std::size_t cell0Verts = 0;
    for (const FieldStrip& strip : cell0.strips) {
        cell0Verts += strip.vertices.size();
    }
    check(cell0Verts == 564, "cellstrips 000 cell0: 564 vertices");
    check(cell0.materials.size() == 3, "cellstrips 000 cell0: 3 headers");
    const FieldCellMaterial& mat0 = cell0.material;
    check(mat0.tex0Tbp0 == 0x2880 && mat0.tex0Psm == 0x14 && mat0.tex0Tw == 7 &&
              mat0.tex0Th == 7 && mat0.tex0Cbp == 0x28A0 &&
              textures && (*textures)[mat0.textureIndex].dbp == 0x2880 &&
              (*textures)[mat0.paletteIndex].dbp == 0x28A0 &&
              (*textures)[mat0.textureIndex].psm == 0x14 &&
              (*textures)[mat0.textureIndex].width == 128,
          "cellstrips 000 cell0: first header binds PSMT4 page 0x2880 + CLUT 0x28a0");
    check(cell0.strips.front().vertices.size() == gifTagNloop(cell0.strips.front().gifTag) &&
              gifTagPrim(cell0.strips.front().gifTag) == 0x27C &&
              cell0.strips.front().materialIndex == 0,
          "cellstrips 000 cell0: first strip NLOOP 6, PRIM 0x27c, material 0");
    // The last header re-selects PSMT8 page 0x285e; the tail strips follow.
    check(cell0.materials.back().tex0Tbp0 == 0x285E && cell0.materials.back().tex0Psm == 0x13,
          "cellstrips 000 cell0: tail header is the PSMT8 page 0x285e");
    check(cell0.strips.back().materialIndex == 2,
          "cellstrips 000 cell0: tail strips inherit material index 2");

    std::cout << "test_field_converter: cellstrips FLD/000.BIN cells=" << cells->size()
              << " strips=" << strips << " vertices=" << vertices << " headers=" << headers
              << " switching=" << switching << " binds=" << texBinds << "/" << palBinds << '\n';
}

void testCellStripsCorpus() {
    const std::filesystem::path dir = discRoot() / "FLD";
    if (dir.empty() || !std::filesystem::exists(dir)) {
        std::cout << "test_field_converter: SKIP FLD cellstrips corpus (not found)\n";
        return;
    }
    std::size_t tiles = 0, cellsTotal = 0, cellsWithStrips = 0, strips = 0, vertices = 0;
    std::size_t matCells = 0, headers = 0, switching = 0, texBinds = 0, palBinds = 0;
    std::size_t unknown = 0, malformed = 0, nloopMiss = 0, unterminated = 0;
    std::vector<std::string> distinctRecords;
    std::vector<std::string> emptyCells;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() != ".BIN") {
            continue;
        }
        std::ifstream input(entry.path(), std::ios::binary);
        const Bytes bytes((std::istreambuf_iterator<char>(input)),
                         std::istreambuf_iterator<char>());
        const auto cells = extractCellStrips(bytes);
        check(cells.has_value(), "cellstrips corpus: " + entry.path().filename().string());
        if (!cells) {
            continue;
        }
        ++tiles;
        for (const FieldCellStrips& cell : *cells) {
            ++cellsTotal;
            if (!cell.strips.empty()) {
                ++cellsWithStrips;
            } else {
                emptyCells.push_back(entry.path().filename().string() + ":" +
                                   std::to_string(cell.cell));
            }
            strips += cell.strips.size();
            for (const FieldStrip& strip : cell.strips) {
                vertices += strip.vertices.size();
            }
            headers += cell.materials.size();
            if (!cell.materials.empty()) {
                ++matCells;
                if (cell.materials.size() > 1) {
                    ++switching;
                }
                if (cell.material.textureIndex >= 0) {
                    ++texBinds;
                }
                if (cell.material.paletteIndex >= 0) {
                    ++palBinds;
                }
                auto hex = [](std::uint64_t v) {
                    std::string out;
                    for (int nibble = 15; nibble >= 0; --nibble) {
                        out += "0123456789abcdef"[(v >> (4 * nibble)) & 0xF];
                    }
                    return out;
                };
                std::string record;
                for (const std::uint64_t reg : cell.material.regs) {
                    record += hex(reg);
                }
                if (std::find(distinctRecords.begin(), distinctRecords.end(), record) ==
                    distinctRecords.end()) {
                    distinctRecords.push_back(record);
                }
            }
            unknown += cell.unknownCommands;
            malformed += cell.malformedUploads;
            nloopMiss += cell.nloopMismatches;
            if (!cell.terminated || cell.truncated) {
                ++unterminated;
            }
        }
    }
    // Pinned by the /tmp probe over the disc copy (2026-10-16): all 4,096
    // directory cells parse; the six empty cells (STCYCL + NOP + RET, no
    // strips) are all in 023.BIN.
    check(tiles == 64, "cellstrips corpus: 64 tiles");
    check(cellsTotal == 4096, "cellstrips corpus: 4096 cells");
    check(cellsWithStrips == 4090, "cellstrips corpus: 4090 cells with strips (4096 - 6 documented misses)");
    check(strips == 418392, "cellstrips corpus: 418392 strips");
    check(vertices == 1749827, "cellstrips corpus: 1749827 vertices");
    check(matCells == 4090 && headers == 16463 && switching == 3431,
          "cellstrips corpus: 4090 material cells, 16463 headers, 3431 mid-cell switches");
    check(texBinds == 4090, "cellstrips corpus: TBP0 binds 4090/4090 (100%)");
    check(palBinds == 4078, "cellstrips corpus: CBP binds 4078/4090 (99.6%)");
    check(distinctRecords.size() == 299, "cellstrips corpus: 299 distinct first-material qword records");
    check(unknown == 0 && malformed == 0 && nloopMiss == 0 && unterminated == 0,
          "cellstrips corpus: clean diagnostics across all chains");
    std::sort(emptyCells.begin(), emptyCells.end());
    const std::vector<std::string> expectedEmpty{"023.BIN:29", "023.BIN:30", "023.BIN:37",
                                                 "023.BIN:38", "023.BIN:45", "023.BIN:46"};
    check(emptyCells == expectedEmpty,
          "cellstrips corpus: empty cells are exactly the six 023.BIN cells");
    std::cout << "test_field_converter: cellstrips corpus tiles=" << tiles
              << " cells=" << cellsTotal << " with-strips=" << cellsWithStrips
              << " strips=" << strips << " vertices=" << vertices
              << " headers=" << headers << " switching=" << switching
              << " tbp-binds=" << texBinds << " cbp-binds=" << palBinds
              << " distinct-materials=" << distinctRecords.size() << '\n';
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
    std::size_t variantPadding = 0;
    std::size_t variantHeader = 0;
    std::size_t tilesWithGeometry = 0;
    std::size_t parseFailures = 0;
    const auto read32 = [](const Bytes& b, std::size_t o) {
        return static_cast<std::uint32_t>(b[o]) |
               (static_cast<std::uint32_t>(b[o + 1]) << 8) |
               (static_cast<std::uint32_t>(b[o + 2]) << 16) |
               (static_cast<std::uint32_t>(b[o + 3]) << 24);
    };
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
            // Classification only: 14 tiles declare fileSize < physical size
            // (trailing padding) and one (023.BIN, also padded) a 0x160-byte
            // header; the offA <= offB <= offC ordering holds for all 64.
            if (bytes.size() >= 0x14) {
                if (read32(bytes, 0x10) < static_cast<std::uint32_t>(bytes.size())) {
                    ++variantPadding;
                }
                if (read32(bytes, 0x00) != 0x20) {
                    ++variantHeader;
                }
            }
            if (!extractFieldPackets(bytes, *tileCatalog).empty()) {
                ++tilesWithGeometry;
            }
            continue;
        }
        ++parseFailures;
    }
    // All 64 tiles parse with the proven relaxations (header-size range and
    // fileSize <= physical size) — see docs/native-reference/
    // field-topology-status.md "Corpus behaviour".
    check(total == 64, "catalog corpus: 64 tiles present");
    check(parseFailures == 0 && parsed == 64, "catalog corpus: 64/64 parse");
    check(variantPadding == 14 && variantHeader == 1,
          "catalog corpus: 14 padded / 1 extended-header, all parsed");
    check(tilesWithGeometry == parsed,
          "geometry corpus: every tile yields packets");
    std::cout << "test_field_converter: catalog corpus parsed=" << parsed
              << " (padded=" << variantPadding
              << ", extended-header=" << variantHeader
              << ") with-geometry=" << tilesWithGeometry << " of " << total << '\n';
}

} // namespace

// ── decodeFieldTextures / buildFieldTexturedTerrain ──────────────────────
//
// Decode rules pinned against the reference oracle (rt_texture_extract):
// payload-linear row-major pixels (applying the GS VRAM swizzle to the
// payload scores 1.07% — it round-trips away between IMAGE write and
// sampler), PSMT4 low nibble first, CLUT entry colour at
// swappedBits34(bank * 16 + entry), word byte0 = R, GS alpha 0..128 scaled
// by min(255, a * 255 / 128), PSMCT24 alpha via AEM (rgb == 0 transparent,
// else opaque 0x80).  See docs/native-reference/field-texture-rendering.md.

namespace {

// One upload whose payload is caller-supplied payload bytes (size must be a
// multiple of 16); returns the tag offset within `bytes`.
std::size_t appendPayloadUpload(Bytes& bytes, std::uint32_t dbp, std::uint32_t dbw,
                               std::uint32_t psm, std::uint32_t width, std::uint32_t height,
                               const Bytes& payload) {
    const std::size_t dataQuads = payload.size() / 16;
    const std::uint32_t qwc = 5 + static_cast<std::uint32_t>(1 + dataQuads);
    const std::size_t tagOffset = bytes.size();
    appendWord(bytes, (0x10u << 24) | qwc);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendWord(bytes, 0);
    appendDword(bytes, 4ull | (1ull << 15) | (1ull << 60));
    appendDword(bytes, 0xEull);
    appendDword(bytes, (static_cast<std::uint64_t>(dbw) << 48) |
                           (static_cast<std::uint64_t>(dbp) << 32) |
                           (static_cast<std::uint64_t>(psm) << 56));
    appendDword(bytes, 0x50);
    appendDword(bytes, 0);
    appendDword(bytes, 0x51);
    appendDword(bytes, (static_cast<std::uint64_t>(height) << 32) | width);
    appendDword(bytes, 0x52);
    appendDword(bytes, 0);
    appendDword(bytes, 0x53);
    appendDword(bytes, dataQuads | (1ull << 15) | (2ull << 58));
    appendDword(bytes, 0);
    bytes.insert(bytes.end(), payload.begin(), payload.end());
    return tagOffset;
}

constexpr std::uint32_t fixtureClutWord(std::uint32_t p) noexcept {
    return (0x80u << 24) | ((p * 3u) << 16) | ((p * 2u) << 8) | p;
}

std::uint8_t gsAlpha(std::uint32_t a) {
    return static_cast<std::uint8_t>(std::min(255u, a * 255u / 128u));
}

void expectRgba(const std::vector<std::uint8_t>& rgba, std::size_t pixel,
                std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a,
                const std::string& what) {
    const bool ok = rgba[pixel * 4 + 0] == r && rgba[pixel * 4 + 1] == g &&
                    rgba[pixel * 4 + 2] == b && rgba[pixel * 4 + 3] == a;
    check(ok, what);
    if (!ok) {
        std::cout << "  expected (" << int(r) << "," << int(g) << "," << int(b) << ","
                  << int(a) << ") got (" << int(rgba[pixel * 4 + 0]) << ","
                  << int(rgba[pixel * 4 + 1]) << "," << int(rgba[pixel * 4 + 2]) << ","
                  << int(rgba[pixel * 4 + 3]) << ")\n";
    }
}

void testDecodeSyntheticExactRgba() {
    Bytes uploads;
    // T8 8x8 with linearly increasing indices and its trailing CLUT8 16x2.
    Bytes t8(64);
    for (std::size_t k = 0; k < 64; ++k) t8[k] = static_cast<std::uint8_t>(k);
    appendPayloadUpload(uploads, 0x100, 1, 0x13, 8, 8, t8);
    Bytes clut8(128);
    for (std::uint32_t p = 0; p < 32; ++p) {
        std::uint32_t w = fixtureClutWord(p);
        if (p == 5) w = (w & 0x00FFFFFFu) | (0x40u << 24);  // sub-opaque alpha pin
        clut8[p * 4 + 0] = static_cast<std::uint8_t>(w);
        clut8[p * 4 + 1] = static_cast<std::uint8_t>(w >> 8);
        clut8[p * 4 + 2] = static_cast<std::uint8_t>(w >> 16);
        clut8[p * 4 + 3] = static_cast<std::uint8_t>(w >> 24);
    }
    appendPayloadUpload(uploads, 0x101, 1, 0x00, 16, 2, clut8);
    // T4 8x8: byte[i] high nibble = i (odd pixel), low nibble = i ^ 3 (even).
    Bytes t4(32);
    for (std::size_t i = 0; i < 32; ++i) {
        t4[i] = static_cast<std::uint8_t>(((i & 0xFu) << 4) | ((i ^ 0x3u) & 0xFu));
    }
    appendPayloadUpload(uploads, 0x200, 1, 0x14, 8, 8, t4);
    Bytes clut4(128);
    for (std::uint32_t p = 0; p < 32; ++p) {
        const std::uint32_t w = (0x80u << 24) | (((0xF0u + p) & 0xFFu) << 16) |
                                (((0x0Fu + p) & 0xFFu) << 8) | ((0x10u + 2u * p) & 0xFFu);
        clut4[p * 4 + 0] = static_cast<std::uint8_t>(w);
        clut4[p * 4 + 1] = static_cast<std::uint8_t>(w >> 8);
        clut4[p * 4 + 2] = static_cast<std::uint8_t>(w >> 16);
        clut4[p * 4 + 3] = static_cast<std::uint8_t>(w >> 24);
    }
    appendPayloadUpload(uploads, 0x201, 1, 0x00, 16, 2, clut4);
    // PSMCT24 8x4 (96 B; 8x8 CT24 would be CLUT-shaped): RGB-zero first texel
    // (AEM transparent).
    Bytes ct24(96);
    for (std::size_t k = 1; k < 32; ++k) {
        ct24[k * 3 + 0] = static_cast<std::uint8_t>(k + 1);
        ct24[k * 3 + 1] = static_cast<std::uint8_t>(k + 2);
        ct24[k * 3 + 2] = static_cast<std::uint8_t>(k + 3);
    }
    appendPayloadUpload(uploads, 0x300, 1, 0x01, 8, 4, ct24);
    // PSMCT32 4x4 with per-texel alpha.
    Bytes ct32(64);
    for (std::uint32_t k = 0; k < 16; ++k) {
        const std::uint32_t w = (k << 24) | ((k + 1) << 16) | ((k + 2) << 8) | (k + 3);
        ct32[k * 4 + 0] = static_cast<std::uint8_t>(w);
        ct32[k * 4 + 1] = static_cast<std::uint8_t>(w >> 8);
        ct32[k * 4 + 2] = static_cast<std::uint8_t>(w >> 16);
        ct32[k * 4 + 3] = static_cast<std::uint8_t>(w >> 24);
    }
    appendPayloadUpload(uploads, 0x400, 1, 0x00, 4, 4, ct32);
    // Indexed page without any palette: grayscale fallback.
    appendPayloadUpload(uploads, 0x500, 1, 0x13, 8, 8, Bytes(64, 0x11));
    std::size_t offA = 0;
    const Bytes bytes = makeChainFile(uploads, 0x40, offA);

    const auto decode = rt::native::assets::decodeFieldTextures(bytes);
    check(decode.has_value(), "decode: synthetic tile decodes");
    if (!decode) return;
    check(decode->pages.size() == 5, "decode: five texture pages (CLUTs excluded)");
    check(decode->clutHits == 2 && decode->clutAliasHits == 0 && decode->clutMisses == 1,
          "decode: two tag-attached palettes, one miss");
    check(decode->totalIndexPixels == 192 && decode->rgb8Pixels == 128,
          "decode: 192 indexed px, 128 palette-resolved");
    if (decode->pages.size() != 5) return;
    const auto& p0 = decode->pages[0];  // T8
    check(p0.paletteResolved && p0.psm == 0x13 && p0.width == 8,
          "decode: T8 page flags");
    expectRgba(p0.rgba, 0, 0, 0, 0, 255, "decode T8: index 0 identity position");
    expectRgba(p0.rgba, 3, 3, 6, 9, 255, "decode T8: bits34 identity for entry 3");
    expectRgba(p0.rgba, 8, 16, 32, 48, 255, "decode T8: entry 8 reads swapped word 16");
    expectRgba(p0.rgba, 16, 8, 16, 24, 255, "decode T8: entry 16 reads swapped word 8");
    expectRgba(p0.rgba, 18, 10, 20, 30, 255, "decode T8: entry 18 swaps to position 10");
    expectRgba(p0.rgba, 5, 5, 10, 15, gsAlpha(0x40), "decode T8: GS alpha 0x40 scales to 127");
    const auto& p1 = decode->pages[1];  // T4
    expectRgba(p1.rgba, 0, 22, 18, 243, 255, "decode T4: low nibble first, index 3");
    expectRgba(p1.rgba, 1, 16, 15, 240, 255, "decode T4: odd pixel takes high nibble");
    expectRgba(p1.rgba, 2, 20, 17, 242, 255, "decode T4: index 2");
    expectRgba(p1.rgba, 16, 54, 34, 3, 255, "decode T4: index 11 swaps to word 19");
    const auto& p2 = decode->pages[2];  // CT24
    expectRgba(p2.rgba, 0, 0, 0, 0, 0, "decode CT24: RGB-zero texel transparent (AEM)");
    expectRgba(p2.rgba, 1, 2, 3, 4, 255, "decode CT24: opaque 0x80 scales to 255");
    expectRgba(p2.rgba, 31, 32, 33, 34, 255, "decode CT24: last texel");
    const auto& p3 = decode->pages[3];  // CT32
    expectRgba(p3.rgba, 0, 3, 2, 1, 0, "decode CT32: byte0 is R, alpha passthrough scale");
    expectRgba(p3.rgba, 8, 11, 10, 9, 15, "decode CT32: alpha 8 scales to 15");
    const auto& p4 = decode->pages[4];  // miss
    check(!p4.paletteResolved && !p4.paletteAliased, "decode: unresolved palette flags");
    expectRgba(p4.rgba, 10, 0x11, 0x11, 0x11, 255, "decode miss: grayscale fallback");
    check(decode->pageIndexByUpload.size() == 7 &&
              decode->pageIndexByUpload[1] == -1 && decode->pageIndexByUpload[3] == -1 &&
              decode->pageIndexByUpload[0] == 0,
          "decode: pageIndexByUpload marks CLUT slots as -1");
}

void testTexturedTerrainSyntheticEmpty() {
    Bytes uploads;
    appendPayloadUpload(uploads, 0x100, 1, 0x13, 8, 8, Bytes(64, 0x02));
    std::size_t offA = 0;
    Bytes bytes = makeChainFile(uploads, 0x40, offA);
    // A zeroed 64-slot cell directory sits at offA; offB/offC/fileSize close
    // the strict ordering offA <= offB <= offC <= fileSize at the file end.
    bytes.resize(offA + 65 * 4, 0);
    setU32(bytes, 0x08, static_cast<std::uint32_t>(bytes.size()));  // offB
    setU32(bytes, 0x0C, static_cast<std::uint32_t>(bytes.size()));  // offC
    setU32(bytes, 0x10, static_cast<std::uint32_t>(bytes.size()));  // fileSize
    const auto built = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
    check(built.has_value(), "textured builder: tile without cell chains still succeeds");
    if (!built) return;
    check(built->patch.positions.empty() && built->patch.indices.empty(),
          "textured builder: no vertices without cells");
    check(built->clutMisses == 1 && built->unresolvedTriangles == 0,
          "textured builder: decode stats propagate");
}

std::vector<std::uint8_t> readFileBytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return std::vector<std::uint8_t>((std::istreambuf_iterator<char>(input)),
                                    std::istreambuf_iterator<char>());
}

const rt::native::assets::FieldTexturePage* pageAt(const rt::native::assets::FieldTextureDecode& d,
                                                  std::uint32_t tagOffset) {
    for (const auto& page : d.pages) {
        if (page.tagOffset == tagOffset) return &page;
    }
    return nullptr;
}

void testFieldTextureDiscPins() {
    const std::filesystem::path dir = discRoot() / "FLD";
    if (dir.empty() || !std::filesystem::exists(dir)) {
        std::cout << "test_field_converter: SKIP FLD textured disc pins (no disc)\n";
        return;
    }
    {  // FLD/000.BIN: counts + byte-exact page pins (oracle-verified decode).
        const auto bytes = readFileBytes(dir / "000.BIN");
        const auto d = rt::native::assets::decodeFieldTextures(bytes);
        const auto t = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
        check(d.has_value() && t.has_value(), "disc 000.BIN: decode and build succeed");
        if (!d || !t) return;
        check(d->pages.size() == 7 && d->clutHits == 3 && d->clutAliasHits == 0 &&
                  d->clutMisses == 4,
              "disc 000.BIN: 7 pages, 3 CLUT hits, 4 unresolved");
        check(d->totalIndexPixels == 30976 && d->rgb8Pixels == 24576,
              "disc 000.BIN: 30,976 indexed px, 24,576 palette-resolved");
        check(t->patch.positions.size() == 25944 && t->patch.indices.size() / 3 == 12606 &&
                  t->cells == 64 && t->unresolvedTriangles == 0,
              "disc 000.BIN: 25,944 verts / 12,606 tris / 64 cells / 0 unresolved");
        check(t->distinctTextureIds == 3, "disc 000.BIN: three distinct material textures");
        const auto cells = rt::native::assets::extractCellStrips(bytes);
        std::size_t strips = 0, verts = 0, withStrips = 0;
        for (const auto& cell : *cells) {
            if (!cell.strips.empty()) ++withStrips;
            for (const auto& strip : cell.strips) {
                ++strips;
                verts += strip.vertices.size();
            }
        }
        check(withStrips == 64 && strips == 6669 && verts == 25944,
              "disc 000.BIN: strip walk 64 cells / 6,669 strips / 25,944 vertices");
        const auto* sky = pageAt(*d, 0x20);
        check(sky != nullptr, "disc 000.BIN: page tag 0x20 decoded");
        if (sky != nullptr) {
            check(sky->width == 64 && sky->height == 64 && sky->psm == 0x13,
                  "disc 000.BIN: 0x20 page geometry");
            expectRgba(sky->rgba, sky->width, 0, 128, 240, 255, "disc 000.BIN: page 0x20 pixel row 1 col 0");
            expectRgba(sky->rgba, 64 * 64 - 1, 0, 160, 240, 255, "disc 000.BIN: page 0x20 last pixel");
        }
        const auto* dirt = pageAt(*d, 0x1ae0);
        if (dirt != nullptr) {
            expectRgba(dirt->rgba, dirt->width, 125, 108, 95, 255, "disc 000.BIN: page 0x1ae0 pixel row 1 col 0");
        } else {
            check(false, "disc 000.BIN: page 0x1ae0 decoded");
        }
    }
    {  // FLD/003.BIN: the oracle-exact tile (45,056/45,056 px vs rt_texture_extract).
        const auto bytes = readFileBytes(dir / "003.BIN");
        const auto d = rt::native::assets::decodeFieldTextures(bytes);
        const auto t = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
        check(d.has_value() && t.has_value(), "disc 003.BIN: decode and build succeed");
        if (!d || !t) return;
        check(d->pages.size() == 10 && d->clutHits == 6 && d->clutMisses == 4,
              "disc 003.BIN: 10 pages, 6 hits, 4 unresolved");
        check(d->totalIndexPixels == 59648 && d->rgb8Pixels == 53248,
              "disc 003.BIN: 59,648 indexed px, 53,248 palette-resolved");
        check(t->patch.positions.size() == 25750 && t->patch.indices.size() / 3 == 12844 &&
                  t->cells == 64 && t->distinctTextureIds == 6 && t->unresolvedTriangles == 0,
              "disc 003.BIN: 25,750 verts / 12,844 tris / 6 pages used");
        const auto* p0 = pageAt(*d, 0x20);
        check(p0 != nullptr, "disc 003.BIN: page tag 0x20 decoded");
        if (p0 != nullptr) {
            expectRgba(p0->rgba, p0->width, 152, 144, 112, 255, "disc 003.BIN: page 0x20 pixel row 1 col 0");
            expectRgba(p0->rgba, 128 * 64 - 1, 8, 8, 8, 255, "disc 003.BIN: page 0x20 last pixel");
        }
        const auto* tex = pageAt(*d, 0x84a0);
        if (tex != nullptr) {
            expectRgba(tex->rgba, tex->width, 161, 108, 20, 255, "disc 003.BIN: page 0x84a0 pixel row 1 col 0");
        } else {
            check(false, "disc 003.BIN: page 0x84a0 decoded");
        }
        // Cell-0 vertex 0 UV triple (s, t, alphaAdd) pinned at stream offset
        // 0x5a24; alphaAdd == 1 so the raw s/t are the page fractions.
        check(std::abs(t->patch.uvs[0][0] - 0.13528818f) < 1e-6f &&
                  std::abs(t->patch.uvs[0][1] - 0.95435876f) < 1e-6f,
              "disc 003.BIN: cell-0 vertex UV = (0.13528818, 0.95435876)");
    }
    {  // FLD/232.BIN: the CBP-aliasing tile.  Its cells bind CBP 0x31B7, which
        // names page 0x31B7 itself — a texture page, not a palette — but the
        // texture bound with that CBP is PSMCT24 (direct colour, no palette),
        // and every indexed page in the tile carries its own trailing CLUT, so
        // no page ever needs the alias and the decode shows zero alias hits.
        const auto bytes = readFileBytes(dir / "232.BIN");
        const auto d = rt::native::assets::decodeFieldTextures(bytes);
        const auto t = rt::native::assets::buildFieldTexturedTerrain(bytes, 0.1f);
        check(d.has_value() && t.has_value(), "disc 232.BIN: decode and build succeed");
        if (!d || !t) return;
        check(d->pages.size() == 25 && d->clutHits == 18 && d->clutAliasHits == 0 &&
                  d->clutMisses == 6,
              "disc 232.BIN: 25 pages, 18 hits, 0 aliases, 6 unresolved");
        check(t->patch.indices.size() / 3 == 6919 && t->cells == 64 &&
                  t->distinctTextureIds == 14 && t->uniqueTextureIds == 1 &&
                  t->unresolvedTriangles == 0,
              "disc 232.BIN: 6,919 tris over 14 material textures, all resolve");
    }
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
    testCellStripsSynthetic();
    testCellStripsErrorsAndDrift();
    testCellStripsRealTile();
    testCellStripsCorpus();
    testDecodeSyntheticExactRgba();
    testTexturedTerrainSyntheticEmpty();
    testFieldTextureDiscPins();
    testRealFieldTileWhenAvailable();
    testCatalogCorpusWhenAvailable();

    std::cout << "test_field_converter: " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
