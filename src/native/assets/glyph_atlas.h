#pragma once

// FONTE glyph-atlas decoder for RoadTripAdventure native assets.
//
// Decodes SYS/FONTE.GSL (and the structurally identical SYS/FONT.GSL): a
// serialized PS2 DMA packet stream (docs/native-reference/sys-gsl-packs.md
// §1) whose unit 0 is a 512x384 PSMT4 glyph atlas (image data at file 0x70)
// and whose unit 1 is its CT16 16x16 CLUT (data at file 0x180e0). The decode
// conventions are the ones §4/§5 cross-check against field-textures.md:
// payload-linear rows (256 B per 512-px row, no GS swizzle on disc), PSMT4
// low nibble first, CSM1 CLUT (logical entry i -> physical
// (i & 7) | ((i & 8) << 1)), CT16 RGBA5551 channels expanded with <<3, TEXA
// opaque alpha 0x80 rescaled to 255, AEM making the all-zero CLUT entry
// transparent (same expansion as image_converter.cpp::rgba16; the atlas CLUT
// has every alpha bit clear, a documented OPEN item, so TEXA supplies alpha).
//
// image_converter.h is deliberately NOT reused for parsing: it parses the
// FLD REF-frame grammar whose CNT payload starts directly with GIF A+D
// packets, while a .GSL unit payload starts with a 16-byte tool-junk word
// ("04 00 00 00" + host pointers) that its strict A+D parse rejects. The GSL
// unit layout is fixed ([CNT][junk16][A+D quads 0x50-0x53][IMAGE][pixels]) so
// it is walked directly per sys-gsl-packs.md §1/§5. convertImage remains a
// byte-level oracle: run over a junk-stripped rewrite of the same chain it
// produces identical RGBA.
//
// DEVIATION from sys-gsl-packs.md §4 (verified against the disc extract
// 2026-10-16, see tests/native_assets/test_glyph_atlas.cpp): the slot grid is
// 16 px wide x 24 px tall (32 columns x 16 rows = 512 cells), not 16x12/1024.
// Under a 12-px pitch the ASCII caps row ('@ABCD...') lands two grid rows
// below where the doc's ASCII mapping predicts (doc slot 33 = 'A' would read
// an empty region). Under 16x24 the mapping is clean: slot i at
// (col = i % 32, row = i / 32), x0 = col*16, y0 = row*24; glyphs are
// ASCII-ordered from slot 0 = '!' (0x21) with 31 glyphs per row and a
// coloured icon (circled numerals, button glyphs) in each row's col-31 slot,
// i.e. ASCII c -> g = c - 0x21 -> slot (g / 31) * 32 + (g % 31)
// (verified: slot 33 'A', 62 '^', 64 '_', 65 '`'). The 16x12 advance figures
// in §4 are the text-layout metrics, not the atlas cell size.
//
// Failure contract: malformed, truncated, or wrongly-shaped input yields a
// default-constructed GlyphAtlas (width == height == 0, empty pixels); the
// error enum and errorOffset identify the fault. No partial output.

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::assets {

inline constexpr std::uint32_t kGlyphAtlasWidth = 512;
inline constexpr std::uint32_t kGlyphAtlasHeight = 384;
inline constexpr std::uint32_t kGlyphCellWidth = 16;
inline constexpr std::uint32_t kGlyphCellHeight = 24;
inline constexpr std::uint32_t kGlyphGridColumns = kGlyphAtlasWidth / kGlyphCellWidth;
inline constexpr std::uint32_t kGlyphGridRows = kGlyphAtlasHeight / kGlyphCellHeight;
inline constexpr std::uint32_t kGlyphSlotCount = kGlyphGridColumns * kGlyphGridRows;

struct GlyphAtlas {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> pixels; // width * height * 4, R8G8B8A8
    // slotNonEmpty[i] is true when the 16x24 cell of slot i contains at least
    // one pixel whose CLUT index is non-zero (index 0 is the atlas
    // background; detected pre-palette so a zeroed CLUT cannot fake empties).
    std::array<bool, kGlyphSlotCount> slotNonEmpty{};
};

enum class GlyphAtlasError : std::uint8_t {
    Ok = 0,
    Truncated,    // a tag, payload, or image range runs past the buffer
    ChainDecode,  // tag id not CNT/END, or the qwc == 6 + NLOOP invariant fails
    BadGeometry,  // no PSMT4 512x384 texture unit, or image bytes != w*h/2
    MissingClut,  // the unit after the atlas texture is not a CT16 16x16 CLUT
};

struct GlyphAtlasResult {
    GlyphAtlasError err = GlyphAtlasError::Ok;
    GlyphAtlas value;
    std::size_t errorOffset = 0;
};

// Optional walk diagnostics (disc-validation numbers; sys-gsl-packs.md §1).
struct GslUnitStats {
    std::uint32_t unitCount = 0;
    std::uint32_t qwcInvariantPassCount = 0; // units where qwc == 6 + NLOOP
};

// Walks the .GSL CNT...END chain, validates the unit grammar, and decodes the
// PSMT4 atlas with its CT16 CLUT into RGBA8 pixels plus per-slot occupancy.
GlyphAtlasResult decodeGlyphAtlas(std::span<const std::uint8_t> gsl,
                                  GslUnitStats* stats = nullptr);

// Extracts slot (0..kGlyphSlotCount-1) as a tightly packed 16x24 RGBA8 tile
// (row-major, kGlyphCellWidth * kGlyphCellHeight * 4 bytes). Returns an empty
// vector for an out-of-range slot or a default-constructed atlas.
std::vector<std::uint8_t> cellGlyph(const GlyphAtlas& atlas, std::uint32_t slot);

} // namespace rt::native::assets
