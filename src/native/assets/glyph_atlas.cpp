// FONTE glyph-atlas decoder — implementation.
//
// Host-native, read-only, std-only. Implements the .GSL chain walk of
// docs/native-reference/sys-gsl-packs.md §1 verbatim (it is the game's own
// validation): byte 3 of each 16-byte DMA tag is 0x10 (CNT) or 0x70 (END);
// every unit's payload is [junk16][four A+D quads regs 0x50..0x53][IMAGE tag]
// [image bytes], and the invariant qwc == 6 + NLOOP holds per unit
// (proven 1142/1142 across the 49-file corpus).
//
// Strictness: every read is bounds-checked; anything malformed, truncated, or
// wrongly shaped rejects with an empty GlyphAtlas (no partial output), the
// same contract as image_converter.cpp.

#include "glyph_atlas.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rt::native::assets {
namespace {

// Evidence constants (sys-gsl-packs.md §1/§4).
constexpr std::uint8_t kDmaTagCnt = 0x10;
constexpr std::uint8_t kDmaTagEnd = 0x70;

// Pixel storage modes (BITBLTBUF destination format, bits 56..61).
constexpr std::uint32_t kPsmCt16 = 0x02;
constexpr std::uint32_t kPsmT4 = 0x14;

// Fixed unit layout, payload-relative offsets (sys-gsl-packs.md §1 table:
// unit start T = CNT tag; T+16 junk; T+32 A+D quads; T+96 IMAGE tag;
// T+112 image data). The payload starts at T+16.
constexpr std::size_t kPayloadAdQuads = 16; // four {u64 value, u32 reg, u32 junk} quads (64 B)
constexpr std::size_t kPayloadImageTag = 80;
constexpr std::size_t kPayloadImageData = 96;

// A+D register addresses.
constexpr std::uint64_t kRegBlitBuf = 0x50;
constexpr std::uint64_t kRegTransferPos = 0x51;
constexpr std::uint64_t kRegTransferReg = 0x52;
constexpr std::uint64_t kRegTransferDir = 0x53;

// Walk cap (defence in depth; the walk always advances by >= 112 bytes).
constexpr int kMaxUnits = 4096;

std::uint64_t readU64Le(std::span<const std::uint8_t> b, std::size_t o) {
    std::uint64_t v = 0;
    for (int i = 7; i >= 0; --i) {
        v = (v << 8) | static_cast<std::uint64_t>(b[o + static_cast<std::size_t>(i)]);
    }
    return v;
}

std::uint16_t readU16Le(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(b[o]) | (static_cast<std::uint16_t>(b[o + 1]) << 8));
}

std::uint64_t field(std::uint64_t v, unsigned lo, unsigned n) {
    return (v >> lo) & ((1ull << n) - 1ull);
}

// One parsed upload unit: destination format/geometry plus the image span.
struct Unit {
    std::size_t tagOffset = 0;  // unit start (CNT tag) in the file
    std::size_t dataOffset = 0; // image payload offset in the file
    std::size_t dataBytes = 0;  // NLOOP * 16
    std::uint32_t psm = 0;
    std::uint32_t w = 0;
    std::uint32_t h = 0;
};

// Walks the CNT...END chain and parses every unit's fixed layout. Returns
// false (with *err/*errAt set) on any malformed structure; fills `units` in
// file order. `stats` receives the walk counters as units parse, so a file
// that later rejects still reports how far the walk got.
bool walkUnits(std::span<const std::uint8_t> gsl, std::vector<Unit>& units,
               GslUnitStats* stats, GlyphAtlasError* err, std::size_t* errAt) {
    const std::size_t n = gsl.size();
    std::size_t cur = 0;
    for (int tag = 0; tag < kMaxUnits; ++tag) {
        if (cur + 16 > n) {
            *err = GlyphAtlasError::Truncated;
            *errAt = cur;
            return false;
        }
        const std::uint8_t id = gsl[cur + 3];
        if (id == kDmaTagEnd) {
            return true;
        }
        if (id != kDmaTagCnt) {
            *err = GlyphAtlasError::ChainDecode;
            *errAt = cur;
            return false;
        }
        const std::uint64_t qwc = readU16Le(gsl, cur);
        const std::uint64_t payloadBytes = qwc * 16u;
        if (payloadBytes > n - (cur + 16)) {
            *err = GlyphAtlasError::Truncated;
            *errAt = cur;
            return false;
        }
        if (payloadBytes < kPayloadImageData + 16u) {
            *err = GlyphAtlasError::ChainDecode; // too small for the unit layout
            *errAt = cur;
            return false;
        }
        const std::size_t payload = cur + 16;

        // IMAGE tag: FLG (bits 58..59) must be 2; NLOOP is the low 15 bits.
        const std::uint64_t imageTag = readU64Le(gsl, payload + kPayloadImageTag);
        const std::uint64_t nloop = imageTag & 0x7FFFu;
        if (field(imageTag, 58, 2) != 2) {
            *err = GlyphAtlasError::ChainDecode;
            *errAt = payload + kPayloadImageTag;
            return false;
        }
        if (stats != nullptr) {
            ++stats->unitCount;
            if (qwc == 6 + nloop) {
                ++stats->qwcInvariantPassCount;
            }
        }
        // The corpus invariant (sys-gsl-packs.md §1, 1142/1142 units):
        // payload = 96 header bytes + NLOOP*16 image bytes exactly.
        if (qwc != 6 + nloop) {
            *err = GlyphAtlasError::ChainDecode;
            *errAt = cur;
            return false;
        }

        // A+D quads at payload+16, +32, +48, +64: {u64 value, u32 reg, u32 junk}.
        std::uint64_t blitBuf = 0, transferReg = 0, transferDir = 0;
        bool haveBlit = false, haveReg = false, haveDir = false, havePos = false;
        for (std::size_t q = 0; q < 4; ++q) {
            const std::uint64_t value = readU64Le(gsl, payload + kPayloadAdQuads + q * 16);
            const std::uint64_t reg = field(readU64Le(gsl, payload + kPayloadAdQuads + q * 16 + 8),
                                            0, 32);
            switch (reg) {
            case kRegBlitBuf: blitBuf = value; haveBlit = true; break;
            case kRegTransferPos: havePos = true; break; // DSAX/DSAY are 0 for full uploads
            case kRegTransferReg: transferReg = value; haveReg = true; break;
            case kRegTransferDir: transferDir = value & 3u; haveDir = true; break;
            default:
                *err = GlyphAtlasError::ChainDecode; // §1: regs are exactly 0x50..0x53
                *errAt = payload + kPayloadAdQuads + q * 16 + 8;
                return false;
            }
        }
        if (!(haveBlit && havePos && haveReg && haveDir) || transferDir != 0) {
            *err = GlyphAtlasError::ChainDecode; // not a host-to-local upload unit
            *errAt = cur;
            return false;
        }

        Unit u;
        u.tagOffset = cur;
        u.dataOffset = payload + kPayloadImageData;
        u.dataBytes = static_cast<std::size_t>(nloop) * 16u;
        u.psm = static_cast<std::uint32_t>(field(blitBuf, 56, 6));
        u.w = static_cast<std::uint32_t>(field(transferReg, 0, 12));
        u.h = static_cast<std::uint32_t>(field(transferReg, 32, 12));
        units.push_back(u);
        cur = payload + static_cast<std::size_t>(payloadBytes);
    }
    *err = GlyphAtlasError::ChainDecode; // no END within the cap
    *errAt = cur;
    return false;
}

// CT16 colour to RGBA8, mirroring image_converter.cpp::rgba16 exactly: 5-bit
// channels expand with <<3; every CLUT entry's alpha bit is clear in this
// file (sys-gsl-packs.md §4 OPEN item), so TEXA supplies opaque alpha 0x80
// which the container scale (min(255, a*255/128)) maps to 255; AEM makes the
// all-zero entry transparent.
void expandCt16(std::uint16_t c, std::uint8_t* out) {
    out[0] = static_cast<std::uint8_t>((c & 0x1Fu) << 3);
    out[1] = static_cast<std::uint8_t>(((c >> 5) & 0x1Fu) << 3);
    out[2] = static_cast<std::uint8_t>(((c >> 10) & 0x1Fu) << 3);
    out[3] = c == 0 ? std::uint8_t{0} : std::uint8_t{255};
}

} // namespace

GlyphAtlasResult decodeGlyphAtlas(std::span<const std::uint8_t> gsl,
                                  GslUnitStats* stats) {
    GlyphAtlasResult result;
    std::vector<Unit> units;
    if (!walkUnits(gsl, units, stats, &result.err, &result.errorOffset)) {
        return result;
    }

    // Locate the atlas texture: the single PSMT4 upload at full atlas
    // geometry (sys-gsl-packs.md §3: FONTE unit 0 is T4 512x384, its CT16
    // 16x16 CLUT is the immediately following unit).
    const std::size_t atlasIndex = [&] {
        for (std::size_t i = 0; i < units.size(); ++i) {
            if (units[i].psm == kPsmT4 && units[i].w == kGlyphAtlasWidth &&
                units[i].h == kGlyphAtlasHeight) {
                return i;
            }
        }
        return units.size();
    }();
    if (atlasIndex == units.size()) {
        result.err = GlyphAtlasError::BadGeometry;
        return result;
    }
    const Unit& tex = units[atlasIndex];
    if (tex.dataBytes != static_cast<std::size_t>(tex.w) * tex.h / 2u) {
        // Image bytes are w*h*bpp/8 exactly, never padded (sys-gsl-packs.md §1).
        result.err = GlyphAtlasError::BadGeometry;
        result.errorOffset = tex.dataOffset;
        return result;
    }
    if (atlasIndex + 1 >= units.size() || units[atlasIndex + 1].psm != kPsmCt16 ||
        units[atlasIndex + 1].w != 16 || units[atlasIndex + 1].h != 16 ||
        units[atlasIndex + 1].dataBytes != 512u) {
        result.err = GlyphAtlasError::MissingClut;
        result.errorOffset = tex.tagOffset;
        return result;
    }
    const Unit& clutUnit = units[atlasIndex + 1];

    // Logical PSMT4 entry i reads physical (i & 7) | ((i & 8) << 1) — the GS
    // CSM1 bit-3/4 exchange (sys-gsl-packs.md §4, field-textures.md).
    std::uint16_t clut[16];
    for (std::uint32_t i = 0; i < 16; ++i) {
        const std::uint32_t physical = (i & 7u) | ((i & 8u) << 1);
        clut[i] = readU16Le(gsl, clutUnit.dataOffset + physical * 2u);
    }

    GlyphAtlas atlas;
    atlas.width = kGlyphAtlasWidth;
    atlas.height = kGlyphAtlasHeight;
    atlas.pixels.resize(static_cast<std::size_t>(atlas.width) * atlas.height * 4u);

    std::array<std::uint32_t, kGlyphSlotCount> slotInk{};
    std::span<const std::uint8_t> texData =
        gsl.subspan(tex.dataOffset, tex.dataBytes);
    for (std::uint32_t y = 0; y < atlas.height; ++y) {
        const std::size_t rowBase = static_cast<std::size_t>(y) * (atlas.width / 2u);
        for (std::uint32_t x = 0; x < atlas.width; ++x) {
            // PSMT4 on disc is payload-linear, low nibble first
            // (sys-gsl-packs.md §4; white-core-inside-outline letterforms pin
            // the order against the high-first alternative).
            const std::uint8_t byte = texData[rowBase + x / 2u];
            const std::uint32_t index =
                (byte >> ((x & 1u) * 4u)) & 0xFu;
            if (index != 0) {
                ++slotInk[(y / kGlyphCellHeight) * kGlyphGridColumns +
                          (x / kGlyphCellWidth)];
            }
            std::uint8_t* out =
                &atlas.pixels[(static_cast<std::size_t>(y) * atlas.width + x) * 4u];
            expandCt16(clut[index], out);
        }
    }
    for (std::uint32_t i = 0; i < kGlyphSlotCount; ++i) {
        atlas.slotNonEmpty[i] = slotInk[i] != 0;
    }
    result.value = std::move(atlas);
    return result;
}

std::vector<std::uint8_t> cellGlyph(const GlyphAtlas& atlas, std::uint32_t slot) {
    if (slot >= kGlyphSlotCount || atlas.width != kGlyphAtlasWidth ||
        atlas.height != kGlyphAtlasHeight ||
        atlas.pixels.size() !=
            static_cast<std::size_t>(kGlyphAtlasWidth) * kGlyphAtlasHeight * 4u) {
        return {};
    }
    const std::size_t x0 = static_cast<std::size_t>(slot % kGlyphGridColumns) *
                           kGlyphCellWidth;
    const std::size_t y0 = static_cast<std::size_t>(slot / kGlyphGridColumns) *
                           kGlyphCellHeight;
    std::vector<std::uint8_t> tile(kGlyphCellWidth * kGlyphCellHeight * 4u);
    for (std::size_t y = 0; y < kGlyphCellHeight; ++y) {
        const std::uint8_t* src =
            &atlas.pixels[((y0 + y) * kGlyphAtlasWidth + x0) * 4u];
        std::copy(src, src + kGlyphCellWidth * 4u,
                  tile.begin() + static_cast<std::ptrdiff_t>(y * kGlyphCellWidth * 4u));
    }
    return tile;
}

} // namespace rt::native::assets
