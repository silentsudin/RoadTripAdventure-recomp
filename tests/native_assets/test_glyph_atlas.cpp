// Synthetic contract tests for the FONTE glyph-atlas decoder, plus an
// optional local-disc integration check (SYS/FONTE.GSL via RT_DISC or the
// default extract path; no game data ships with this tree).
//
// Grammar under test (docs/native-reference/sys-gsl-packs.md §1/§4):
//   unit = [CNT tag (u16 qwc, byte3 0x10)][16 B tool junk][A+D quads
//          regs 0x50..0x53][GIF IMAGE tag (FLG=2, NLOOP=bytes/16)][pixels];
//   invariant qwc == 6 + NLOOP; PSMT4 512x384 atlas (payload-linear, low
//   nibble first) + immediately following CT16 16x16 CLUT (CSM1:
//   logical i -> physical (i & 7) | ((i & 8) << 1)).
//
// DEVIATION pinned here: the slot grid is 16x24 (32 cols x 16 rows = 512
// slots), not the 16x12/1024 the doc states — see glyph_atlas.h.

#include "../../src/native/assets/glyph_atlas.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using rt::native::assets::GlyphAtlas;
using rt::native::assets::GlyphAtlasError;
using rt::native::assets::GlyphAtlasResult;
using rt::native::assets::GslUnitStats;
using rt::native::assets::GlyphUv;
using rt::native::assets::fonteSlotForChar;
using rt::native::assets::fonteSlotUv;
using rt::native::assets::cellGlyph;
using rt::native::assets::decodeGlyphAtlas;
using rt::native::assets::kGlyphAtlasHeight;
using rt::native::assets::kGlyphAtlasWidth;
using rt::native::assets::kGlyphCellHeight;
using rt::native::assets::kGlyphCellWidth;
using rt::native::assets::kGlyphSlotCount;

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

void appendU16(Bytes& bytes, std::uint16_t value) {
    bytes.push_back(static_cast<std::uint8_t>(value & 0xFFu));
    bytes.push_back(static_cast<std::uint8_t>(value >> 8));
}

void appendU32(Bytes& bytes, std::uint32_t value) {
    for (std::size_t i = 0; i < 4; ++i) {
        bytes.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xFFu));
    }
}

void appendU64(Bytes& bytes, std::uint64_t value) {
    for (std::size_t i = 0; i < 8; ++i) {
        bytes.push_back(static_cast<std::uint8_t>((value >> (8 * i)) & 0xFFu));
    }
}

// One upload unit exactly as sys-gsl-packs.md §1 lays it out. `body` must
// have exactly dataBytes bytes of image payload appended by the caller after
// this returns (the builder leaves the IMAGE tag open at the end).
struct UnitSpec {
    std::uint32_t psm;
    std::uint32_t w;
    std::uint32_t h;
    std::uint32_t dataBytes;
    bool breakInvariant = false; // emit qwc = 5 + NLOOP instead of 6 + NLOOP
};

void appendUnit(Bytes& bytes, const UnitSpec& spec) {
    const std::uint32_t nloop = spec.dataBytes / 16u;
    const std::uint32_t qwc = (spec.breakInvariant ? 5u : 6u) + nloop;
    appendU16(bytes, static_cast<std::uint16_t>(qwc));
    bytes.push_back(0);
    bytes.push_back(0x10); // CNT
    appendU32(bytes, 0);
    appendU64(bytes, 0x0012FF6Cu); // qword-1 tool junk (ignored)
    appendU32(bytes, 4);           // junk16: "04 00 00 00" + host pointers
    appendU32(bytes, 0x1012B9A4u);
    appendU32(bytes, 0x00405BBEu);
    appendU32(bytes, 0x0012F9ECu);
    // A+D quads {u64 value, u32 reg, u32 junk}: BITBLTBUF, TRXPOS, TRXREG, TRXDIR.
    appendU64(bytes, (static_cast<std::uint64_t>(spec.psm) << 56) |
                         (8ull << 48));                       // dbp 0, dbw 8
    appendU32(bytes, 0x50);
    appendU32(bytes, 0);
    appendU64(bytes, 0);                                        // TRXPOS
    appendU32(bytes, 0x51);
    appendU32(bytes, 0);
    appendU64(bytes, (static_cast<std::uint64_t>(spec.h) << 32) | spec.w);
    appendU32(bytes, 0x52);
    appendU32(bytes, 0);
    appendU64(bytes, 0);                                        // TRXDIR (DIR 0)
    appendU32(bytes, 0x53);
    appendU32(bytes, 0);
    appendU64(bytes, nloop | (2ull << 58));                         // IMAGE tag
    appendU64(bytes, 0x0040648Du);                              // hi-qword junk
}

void appendEnd(Bytes& bytes) {
    appendU16(bytes, 0);
    bytes.push_back(0);
    bytes.push_back(0x70); // END
    appendU32(bytes, 0);
    appendU64(bytes, 0);
}

constexpr std::uint32_t kPsmCt16 = 0x02;
constexpr std::uint32_t kPsmT8 = 0x13;
constexpr std::uint32_t kPsmT4 = 0x14;

constexpr std::size_t kAtlasDataBytes = kGlyphAtlasWidth * kGlyphAtlasHeight / 2;
constexpr std::size_t kClutDataBytes = 512;

// Synthetic CLUT colours (CT16 RGBA5551). Physical entries hold the CSM1
// mapping targets: logical 1 -> phys 1 (dark gray), logical 2 -> phys 2
// (white), logical 8 -> phys 16 (pure green) so the bit-3/4 exchange is
// observable.
constexpr std::uint16_t kGray = 0x0842;  // (2,2,2) -> <<3 (16,16,16)
constexpr std::uint16_t kWhite = 0x7FFF; // (31,31,31) -> <<3 (248,248,248)
constexpr std::uint16_t kGreen = 0x03E0; // g=31 -> (0,248,0)

struct SyntheticGsl {
    Bytes bytes;
};

SyntheticGsl buildSyntheticGsl() {
    SyntheticGsl out;
    Bytes clutData(kClutDataBytes, 0);
    auto setClut = [&](std::uint32_t physical, std::uint16_t value) {
        clutData[physical * 2] = static_cast<std::uint8_t>(value & 0xFFu);
        clutData[physical * 2 + 1] = static_cast<std::uint8_t>(value >> 8);
    };
    setClut(1, kGray);
    setClut(2, kWhite);
    setClut(16, kGreen); // logical 8 under CSM1

    appendUnit(out.bytes, {kPsmT4, kGlyphAtlasWidth, kGlyphAtlasHeight, kAtlasDataBytes});
    out.bytes.insert(out.bytes.end(), kAtlasDataBytes, 0);
    appendUnit(out.bytes, {kPsmCt16, 16, 16, kClutDataBytes});
    out.bytes.insert(out.bytes.end(), clutData.begin(), clutData.end());
    appendEnd(out.bytes);
    return out;
}

// Byte offset of the PSMT4 image data inside the synthetic file: CNT tag 16 +
// payload header 96 = 112.
constexpr std::size_t kSyntheticAtlasOffset = 112;

void poke(SyntheticGsl& gsl, std::uint32_t x, std::uint32_t y, std::uint8_t byte) {
    const std::size_t dataIndex = static_cast<std::size_t>(y) * (kGlyphAtlasWidth / 2) + x / 2;
    gsl.bytes[kSyntheticAtlasOffset + dataIndex] = byte;
}

const std::uint8_t* pixel(const GlyphAtlas& atlas, std::uint32_t x, std::uint32_t y) {
    return &atlas.pixels[(static_cast<std::size_t>(y) * atlas.width + x) * 4];
}

bool pixelIs(const GlyphAtlas& atlas, std::uint32_t x, std::uint32_t y, std::uint8_t r,
             std::uint8_t g, std::uint8_t b, std::uint8_t a) {
    const std::uint8_t* p = pixel(atlas, x, y);
    return p[0] == r && p[1] == g && p[2] == b && p[3] == a;
}

SyntheticGsl buildDecoratedGsl() {
    SyntheticGsl gsl = buildSyntheticGsl();
    // Slot 1 (col 1, row 0): first byte 0x21 -> even px = index 1 (gray),
    // odd px = index 2 (white) under low-nibble-first.
    poke(gsl, 16, 0, 0x21);
    // Slot 3 (col 3): index 8 through the CSM1 exchange -> green.
    poke(gsl, 48, 0, 0x08);
    // Slot 511 (col 31, row 15): two index-1 pixels.
    poke(gsl, 496, 360, 0x11);
    return gsl;
}

void testSyntheticDecodeAndStats() {
    const SyntheticGsl gsl = buildSyntheticGsl();
    GslUnitStats stats;
    const GlyphAtlasResult result = decodeGlyphAtlas(gsl.bytes, &stats);
    check(result.err == GlyphAtlasError::Ok, "synthetic: decodes");
    if (result.err != GlyphAtlasError::Ok) {
        return;
    }
    check(stats.unitCount == 2, "synthetic: two units");
    check(stats.qwcInvariantPassCount == 2, "synthetic: qwc == 6 + NLOOP on both units");
    const GlyphAtlas& atlas = result.value;
    check(atlas.width == kGlyphAtlasWidth && atlas.height == kGlyphAtlasHeight,
          "synthetic: atlas dimensions");
    check(atlas.pixels.size() ==
              static_cast<std::size_t>(kGlyphAtlasWidth) * kGlyphAtlasHeight * 4,
          "synthetic: RGBA8 pixel count");
    check(atlas.slotNonEmpty.size() == kGlyphSlotCount, "synthetic: 512 slot flags");
}

void testNibbleOrderAndCsm1() {
    const SyntheticGsl gsl = buildDecoratedGsl();
    const GlyphAtlasResult result = decodeGlyphAtlas(gsl.bytes);
    check(result.err == GlyphAtlasError::Ok, "nibble: decodes");
    if (result.err != GlyphAtlasError::Ok) {
        return;
    }
    const GlyphAtlas& atlas = result.value;
    // Low nibble first: byte 0x21 puts index 1 on the even pixel, index 2 on
    // the odd pixel. High-first would swap the colours and fail these.
    check(pixelIs(atlas, 16, 0, 16, 16, 16, 255),
          "nibble: slot 1 even pixel takes the low nibble (gray, CT16 <<3)");
    check(pixelIs(atlas, 17, 0, 248, 248, 248, 255),
          "nibble: slot 1 odd pixel takes the high nibble (white, CT16 <<3)");
    // CSM1 exchange: logical 8 reads physical entry 16.
    check(pixelIs(atlas, 48, 0, 0, 248, 0, 255),
          "csm1: index 8 reads physical CLUT entry 16 (green)");
    // Index 0 is the AEM-transparent background.
    check(pixelIs(atlas, 49, 0, 0, 0, 0, 0),
          "csm1: index 0 is transparent (AEM on all-zero entry)");
}

void testSlotGridAndCellGlyph() {
    const SyntheticGsl gsl = buildDecoratedGsl();
    const GlyphAtlasResult result = decodeGlyphAtlas(gsl.bytes);
    check(result.err == GlyphAtlasError::Ok, "slots: decodes");
    if (result.err != GlyphAtlasError::Ok) {
        return;
    }
    const GlyphAtlas& atlas = result.value;
    std::size_t nonEmpty = 0;
    for (bool filled : atlas.slotNonEmpty) {
        nonEmpty += filled ? 1u : 0u;
    }
    check(nonEmpty == 3, "slots: exactly the three poked slots are non-empty");
    check(!atlas.slotNonEmpty[0] && atlas.slotNonEmpty[1] && !atlas.slotNonEmpty[2] &&
              atlas.slotNonEmpty[3] && atlas.slotNonEmpty[511],
          "slots: non-empty flags track index-0-free cells");

    const std::vector<std::uint8_t> tile1 = cellGlyph(atlas, 1);
    check(tile1.size() == kGlyphCellWidth * kGlyphCellHeight * 4,
          "cellglyph: 16x24x4 tile size");
    if (tile1.size() == kGlyphCellWidth * kGlyphCellHeight * 4) {
        check(tile1[0] == 16 && tile1[1] == 16 && tile1[2] == 16 && tile1[3] == 255,
              "cellglyph: slot 1 first pixel matches the atlas");
        check(tile1[4] == 248 && tile1[5] == 248 && tile1[6] == 248 && tile1[7] == 255,
              "cellglyph: slot 1 second pixel matches the atlas");
        check(tile1[8] == 0 && tile1[11] == 0,
              "cellglyph: untouched pixels stay transparent");
    }
    const std::vector<std::uint8_t> tileLast = cellGlyph(atlas, kGlyphSlotCount - 1);
    check(tileLast.size() == kGlyphCellWidth * kGlyphCellHeight * 4 &&
              tileLast[0] == 16 && tileLast[3] == 255,
          "cellglyph: slot 511 maps to col 31 row 15");
    check(cellGlyph(atlas, kGlyphSlotCount).empty(), "cellglyph: out-of-range slot rejected");
    check(cellGlyph(GlyphAtlas{}, 0).empty(), "cellglyph: empty atlas rejected");
}

void testErrorPaths() {
    check(decodeGlyphAtlas({}).err == GlyphAtlasError::Truncated,
          "errors: empty input is Truncated");

    const SyntheticGsl base = buildSyntheticGsl();

    Bytes badTag = base.bytes;
    badTag[3] = 0x20;
    check(decodeGlyphAtlas(badTag).err == GlyphAtlasError::ChainDecode,
          "errors: non-CNT/END tag id is ChainDecode");

    Bytes truncatedPayload = base.bytes;
    // Inflate unit 0's qwc so its payload runs past the file.
    const std::uint16_t qwc = static_cast<std::uint16_t>(0xFFFF);
    truncatedPayload[0] = static_cast<std::uint8_t>(qwc & 0xFFu);
    truncatedPayload[1] = static_cast<std::uint8_t>(qwc >> 8);
    check(decodeGlyphAtlas(truncatedPayload).err == GlyphAtlasError::Truncated,
          "errors: payload past the end is Truncated");

    GslUnitStats stats;
    Bytes brokenInvariant;
    appendUnit(brokenInvariant, {kPsmT4, kGlyphAtlasWidth, kGlyphAtlasHeight,
                                 kAtlasDataBytes, /*breakInvariant=*/true});
    brokenInvariant.insert(brokenInvariant.end(), kAtlasDataBytes, 0);
    appendUnit(brokenInvariant, {kPsmCt16, 16, 16, kClutDataBytes});
    brokenInvariant.insert(brokenInvariant.end(), kClutDataBytes, 0);
    appendEnd(brokenInvariant);
    check(decodeGlyphAtlas(brokenInvariant, &stats).err == GlyphAtlasError::ChainDecode,
          "errors: qwc != 6 + NLOOP is ChainDecode");
    check(stats.unitCount == 1 && stats.qwcInvariantPassCount == 0,
          "errors: stats count the reached unit and the failed invariant");

    Bytes noEnd = base.bytes;
    noEnd.resize(noEnd.size() - 16); // drop the END tag
    check(decodeGlyphAtlas(noEnd).err == GlyphAtlasError::Truncated,
          "errors: chain without END tag is Truncated");

    Bytes badImageTag = base.bytes;
    // Unit 0's IMAGE tag starts at file offset 16 + 80; its FLG field lives in
    // the tag's top byte (0x08 = FLG 2). Flip to FLG 1 (REGLIST).
    badImageTag[16 + 80 + 7] = 0x04;
    check(decodeGlyphAtlas(badImageTag).err == GlyphAtlasError::ChainDecode,
          "errors: IMAGE tag with FLG != 2 is ChainDecode");

    // A CLUT-only chain has no 512x384 PSMT4 texture.
    Bytes clutOnly;
    appendUnit(clutOnly, {kPsmCt16, 16, 16, kClutDataBytes});
    clutOnly.insert(clutOnly.end(), kClutDataBytes, 0);
    appendEnd(clutOnly);
    check(decodeGlyphAtlas(clutOnly).err == GlyphAtlasError::BadGeometry,
          "errors: missing atlas texture is BadGeometry");

    // Atlas geometry with a lying image size (NLOOP not w*h/2).
    Bytes lyingSize;
    appendUnit(lyingSize, {kPsmT4, kGlyphAtlasWidth, kGlyphAtlasHeight, 16});
    lyingSize.insert(lyingSize.end(), 16, 0);
    appendUnit(lyingSize, {kPsmCt16, 16, 16, kClutDataBytes});
    lyingSize.insert(lyingSize.end(), kClutDataBytes, 0);
    appendEnd(lyingSize);
    check(decodeGlyphAtlas(lyingSize).err == GlyphAtlasError::BadGeometry,
          "errors: image bytes != w*h/2 is BadGeometry");

    // Atlas texture followed by a non-CLUT upload.
    Bytes clutMissing;
    appendUnit(clutMissing, {kPsmT4, kGlyphAtlasWidth, kGlyphAtlasHeight, kAtlasDataBytes});
    clutMissing.insert(clutMissing.end(), kAtlasDataBytes, 0);
    appendUnit(clutMissing, {kPsmT8, 64, 64, 4096});
    clutMissing.insert(clutMissing.end(), 4096, 0);
    appendEnd(clutMissing);
    check(decodeGlyphAtlas(clutMissing).err == GlyphAtlasError::MissingClut,
          "errors: atlas without its CT16 16x16 CLUT is MissingClut");
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

std::uint64_t fnv1a64(std::span<const std::uint8_t> bytes) {
    std::uint64_t hash = 14695981039346656037ull;
    for (const std::uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 1099511628211ull;
    }
    return hash;
}

// Pins measured against the disc extract 2026-10-16 (FONTE.GSL, 99088 B):
// 2 units, qwc invariant 2/2, atlas 512x384, 438/512 non-empty slots under
// the evidence-verified 16x24 grid (the doc's 16x12/1024 grid reports 765
// "cells", but its slot 33 = 'A' region is empty — the 16x24 grid is the one
// where ASCII slot math, letterforms, and the dev-verified cell geometry
// agree). Low-nibble-first is pinned by 'A' interior pixels: white CLUT
// entry 2 core between gray entry-1/7 outline pixels, byte-level
// (pix[26*256+10] = 0x10 -> even px index 0, odd px index 1).
void testRealFonteWhenAvailable() {
    // FONTE.GSL sits under SYS/ (uppercase on this disc image).
    const std::filesystem::path path = discRoot() / "SYS/FONTE.GSL";
    if (path.empty() || !std::filesystem::exists(path)) {
        std::cout << "test_glyph_atlas: SKIP real SYS/FONTE.GSL (not found)\n";
        return;
    }
    std::ifstream input(path, std::ios::binary);
    const Bytes bytes((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());
    check(bytes.size() == 99088, "real FONTE.GSL: file size 99088");

    GslUnitStats stats;
    const GlyphAtlasResult result = decodeGlyphAtlas(bytes, &stats);
    check(result.err == GlyphAtlasError::Ok, "real FONTE.GSL: decodes");
    if (result.err != GlyphAtlasError::Ok) {
        return;
    }
    check(stats.unitCount == 2, "real FONTE.GSL: two units (texture + CLUT)");
    check(stats.qwcInvariantPassCount == 2, "real FONTE.GSL: qwc invariant 2/2");
    const GlyphAtlas& atlas = result.value;
    check(atlas.width == 512 && atlas.height == 384, "real FONTE.GSL: 512x384");
    check(atlas.pixels.size() == 512u * 384u * 4u, "real FONTE.GSL: RGBA size");

    std::size_t nonEmpty = 0;
    for (bool filled : atlas.slotNonEmpty) {
        nonEmpty += filled ? 1u : 0u;
    }
    check(nonEmpty == 438, "real FONTE.GSL: 438/512 non-empty slots");
    check(atlas.slotNonEmpty[0], "real FONTE.GSL: slot 0 = '!' (non-empty)");
    // ASCII spot checks under 16x24: slot 33 'A', 62 '^', 64 '_', 65 '`',
    // and 31/63 the circled-numeral icons.
    check(atlas.slotNonEmpty[33] && atlas.slotNonEmpty[62] && atlas.slotNonEmpty[64] &&
              atlas.slotNonEmpty[65] && atlas.slotNonEmpty[31] && atlas.slotNonEmpty[63],
          "real FONTE.GSL: ASCII anchor slots are non-empty");
    check(atlas.slotNonEmpty[511] == false, "real FONTE.GSL: trailing slot empty");

    // 'A' interior, low-nibble-first: px (21,26) index 1 -> gray
    // (16,16,16,255); px (23,26) index 2 -> white (248,248,248,255);
    // px (20,26) index 0 -> transparent. Under high-first the grays/whites
    // would swap pixels and these would fail.
    check(pixelIs(atlas, 21, 26, 16, 16, 16, 255) &&
              pixelIs(atlas, 23, 26, 248, 248, 248, 255) &&
              pixelIs(atlas, 20, 26, 0, 0, 0, 0),
          "real FONTE.GSL: 'A' core/outline pixels pin low-nibble-first");

    const std::vector<std::uint8_t> tileA = cellGlyph(atlas, 33);
    check(tileA.size() == kGlyphCellWidth * kGlyphCellHeight * 4,
          "real FONTE.GSL: 'A' tile size");

    // Stability + FNV pin (also cross-checked by the build-dir validation
    // tool's MD5 over the same RGBA stream).
    const GlyphAtlasResult again = decodeGlyphAtlas(bytes);
    check(again.err == GlyphAtlasError::Ok &&
              again.value.pixels == atlas.pixels &&
              again.value.slotNonEmpty == atlas.slotNonEmpty,
          "real FONTE.GSL: decode is deterministic across runs");
    check(fnv1a64(atlas.pixels) == 0x4F25554650A9DE8Dull,
          "real FONTE.GSL: RGBA FNV-1a pin");
    std::cout << "test_glyph_atlas: FONTE.GSL units=" << stats.unitCount
              << " qwc-pass=" << stats.qwcInvariantPassCount
              << " non-empty=" << nonEmpty << "/512"
              << " fnv=0x" << std::hex << fnv1a64(atlas.pixels) << std::dec << '\n';
}

// Char->slot + UV pins for the proven FONTE ordering (sys-gsl-packs.md §4
// corrected 2026-10-16: 31 glyphs/row from slot 0 = '!', col-31 icons).
void testSourceMapping() {
    check(fonteSlotForChar('!') == 0, "'!' slot 0");
    check(fonteSlotForChar('A') == 33, "'A' slot 33");
    check(fonteSlotForChar('^') == 62, "'^' slot 62");
    check(fonteSlotForChar('_') == 64, "'_' slot 64");
    check(fonteSlotForChar('`') == 65, "'`' slot 65");
    check(fonteSlotForChar('~') == 96, "'~' slot 96 (row 3 col 0)");
    check(fonteSlotForChar(' ') == -1, "space has no glyph");
    check(fonteSlotForChar('\x7f') == -1, "0x7f out of range");
    check(fonteSlotForChar('\x20') == -1, "0x20 out of range");
    const GlyphUv first = fonteSlotUv(0);
    check(first.u0 > 0.f && first.v0 > 0.f, "slot 0 uv inset > 0");
    check(first.u1 < 1.f && first.v1 < 1.f, "slot 0 uv within atlas");
    check(std::abs(first.u1 - first.u0 - 15.f / 512.f) < 1e-5f, "slot uv width = cell-1texel");
    check(std::abs(first.v1 - first.v0 - 23.f / 384.f) < 1e-5f, "slot uv height = cell-1texel");
    const GlyphUv a = fonteSlotUv(33);  // col 1, row 1
    check(std::abs(a.u0 - 16.5f / 512.f) < 1e-5f, "'A' u0 = col1 + inset");
    check(std::abs(a.v0 - 24.5f / 384.f) < 1e-5f, "'A' v0 = row1 + inset");
}

} // namespace

int main() {
    testSyntheticDecodeAndStats();
    testNibbleOrderAndCsm1();
    testSlotGridAndCellGlyph();
    testErrorPaths();
    testSourceMapping();
    testRealFonteWhenAvailable();

    std::cout << "test_glyph_atlas: " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}
