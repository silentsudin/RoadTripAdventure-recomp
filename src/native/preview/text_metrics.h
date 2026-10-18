#pragma once

// Proportional metrics and atlas cells for the live FONTE preview font.
//
// The original's `uiGlyphWidths` is 96 contiguous unsigned-byte advances for
// ASCII 0x20..0x7f at ELF vaddr 0x0029A880..0x0029A8DF (file offset is
// vaddr - 0x001ff000).  Runtime loading keeps an installed user's original
// disc authoritative.  The numeric fallback below is the same table copied as
// measurements (not extracted texture data), so a preview remains deterministic
// when no disc is configured.

#include <array>
#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <vector>

namespace rt::native::preview {

struct Glyph {
    float advance = 16.f;
    int cellX = -1;
    int cellY = -1;
    int w = 0;
    int h = 0;
    bool operator==(const Glyph&) const = default;
};

struct GlyphPlacement {
    Glyph glyph{};
    float penX = 0.f;
    float advance = 0.f;
    bool operator==(const GlyphPlacement&) const = default;
};

class GlyphTable {
public:
    static constexpr unsigned kFirstAscii = 0x20;
    static constexpr unsigned kLastAscii = 0x7f;
    static constexpr std::size_t kAdvanceCount = kLastAscii - kFirstAscii + 1;
    static constexpr std::uint32_t kAdvanceTableVaddr = 0x0029A880;
    static constexpr std::uint32_t kElfBaseVaddr = 0x001FF000;

    // Numeric fallback measured from uiGlyphWidths at 0x0029A880.  These are
    // width constants only; no disc or decoded atlas pixels are stored here.
    static constexpr std::array<std::uint8_t, kAdvanceCount> kFallbackAdvances{
        8, 16, 6, 16, 16, 16, 16, 6, 8, 10, 16, 16, 10, 10, 12, 16,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 16, 16, 16, 16, 16, 16,
        16, 15, 14, 15, 16, 12, 12, 16, 14, 12, 16, 16, 16, 16, 16, 16,
        14, 16, 14, 14, 14, 16, 16, 16, 16, 14, 16, 16, 16, 16, 16, 16,
        16, 14, 14, 14, 14, 14, 14, 14, 14, 8, 14, 14, 8, 16, 12, 13,
        14, 14, 12, 12, 12, 12, 12, 14, 14, 14, 13, 16, 16, 16, 16, 16,
    };

    GlyphTable() { setAdvances(kFallbackAdvances, false); }

    // Loads only the documented 96-byte metric range.  A failed/truncated ROM
    // or values outside the conservative 4..24-pixel sanity range leaves the
    // authored numeric fallback in place.
    static GlyphTable fromRomPath(const std::filesystem::path& path) {
        GlyphTable result;
        std::ifstream rom(path, std::ios::binary);
        if (!rom) return result;
        constexpr std::streamoff kOffset = static_cast<std::streamoff>(kAdvanceTableVaddr - kElfBaseVaddr);
        rom.seekg(kOffset);
        std::array<std::uint8_t, kAdvanceCount> advances{};
        rom.read(reinterpret_cast<char*>(advances.data()), static_cast<std::streamsize>(advances.size()));
        if (rom.gcount() != static_cast<std::streamsize>(advances.size())) return result;
        for (const std::uint8_t value : advances)
            if (value < 4 || value > 24) return result;
        result.setAdvances(advances, true);
        return result;
    }

    // RT_ELF is the explicit runtime override.  The conventional local-disc
    // path is tried only when it is unset, never shipped or copied by native.
    static GlyphTable fromRuntime() {
        if (const char* elf = std::getenv("RT_ELF"); elf && *elf) return fromRomPath(elf);
        if (const char* home = std::getenv("HOME"); home && *home)
            return fromRomPath(std::filesystem::path(home) /
                               "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98");
        return {};
    }

    [[nodiscard]] bool usesRomAdvances() const { return usesRomAdvances_; }
    [[nodiscard]] const Glyph& glyph(unsigned char c) const { return glyphs_[c]; }

    [[nodiscard]] float measure(std::string_view text, float pxPerUnit = 1.f) const {
        float width = 0.f;
        for (const unsigned char c : text) width += glyphs_[c].advance * pxPerUnit;
        return width;
    }

    // One placement per source byte, including spaces and unsupported bytes;
    // callers skip atlas sampling when cellX/cellY are negative but retain the
    // original pen advance.
    [[nodiscard]] std::vector<GlyphPlacement> layoutRun(std::string_view text,
                                                         float pxPerUnit = 1.f) const {
        std::vector<GlyphPlacement> result;
        result.reserve(text.size());
        float pen = 0.f;
        for (const unsigned char c : text) {
            const Glyph& g = glyphs_[c];
            const float advance = g.advance * pxPerUnit;
            result.push_back({g, pen, advance});
            pen += advance;
        }
        return result;
    }

    bool operator==(const GlyphTable&) const = default;

private:
    void setAdvances(const std::array<std::uint8_t, kAdvanceCount>& advances, bool fromRom) {
        for (Glyph& glyph : glyphs_) glyph = {};
        for (unsigned c = kFirstAscii; c <= kLastAscii; ++c) {
            Glyph& glyph = glyphs_[c];
            glyph.advance = static_cast<float>(advances[c - kFirstAscii]);
            // FONTE: ASCII '!'..'~' uses 31 text cells per atlas row; physical
            // column 31 is an icon cell.  See glyph_atlas.h and ELF uiDrawGlyph
            // at 0x002017B0; lowercase follows this same c - 0x21 mapping.
            if (c >= 0x21 && c <= 0x7e) {
                const unsigned g = c - 0x21;
                glyph.cellX = static_cast<int>(g % 31);
                glyph.cellY = static_cast<int>(g / 31);
                glyph.w = 16;
                glyph.h = 24;
            }
        }
        usesRomAdvances_ = fromRom;
    }

    std::array<Glyph, 256> glyphs_{};
    bool usesRomAdvances_ = false;
};

}  // namespace rt::native::preview
