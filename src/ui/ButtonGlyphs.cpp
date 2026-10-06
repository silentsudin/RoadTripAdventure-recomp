// The game's button glyphs for the pad in use (see ButtonGlyphs.h).

#include "ui/ButtonGlyphs.h"

#include "settings/Apply.h"
#include "ui/Theme.h"

#include "runtime/gs/gs_pgs_backend.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

namespace rt::ui
{
    namespace
    {
        using DecodedTexture = ps2x::gs::PgsControl::DecodedTexture;

        // The font atlas (a 4-bit, 512-wide texture): its index layout around the button glyphs,
        // the same in every palette the game draws it with.
        constexpr uint64_t kAtlasFingerprint = 0x31b63df03b69537aull;
        constexpr uint32_t kFpX0 = 104, kFpX1 = 248, kFpY0 = 224, kFpY1 = 248;

        uint64_t fingerprint(const DecodedTexture &d)
        {
            uint64_t h = 1469598103934665603ull;
            for (uint32_t y = kFpY0; y < kFpY1; ++y)
                for (uint32_t x = kFpX0; x < kFpX1; ++x)
                    h = (h ^ d.index(x, y)) * 1099511628211ull;
            return h;
        }

        // Pixel letters: 5x7 for the face buttons, 3x5 for the shoulders (rows top down, high bit left).
        struct Glyph
        {
            int w, h;
            uint8_t rows[7];
        };
        const std::map<char, Glyph> kBig = {
            {'A', {5, 7, {0b01110, 0b10001, 0b10001, 0b11111, 0b10001, 0b10001, 0b10001}}},
            {'B', {5, 7, {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110}}},
            {'X', {5, 7, {0b10001, 0b10001, 0b01010, 0b00100, 0b01010, 0b10001, 0b10001}}},
            {'Y', {5, 7, {0b10001, 0b10001, 0b01010, 0b00100, 0b00100, 0b00100, 0b00100}}},
        };
        const std::map<char, Glyph> kSmall = {
            {'L', {3, 5, {0b100, 0b100, 0b100, 0b100, 0b111}}}, {'R', {3, 5, {0b110, 0b101, 0b110, 0b101, 0b101}}},
            {'B', {3, 5, {0b110, 0b101, 0b110, 0b101, 0b110}}}, {'T', {3, 5, {0b111, 0b010, 0b010, 0b010, 0b010}}},
            {'Z', {3, 5, {0b111, 0b001, 0b010, 0b100, 0b111}}},
        };

        struct Canvas
        {
            DecodedTexture &d;
            uint32_t &at(int x, int y) { return d.rgba[static_cast<size_t>(y) * d.width + x]; }
        };

        int luma(uint32_t c) { return (c & 0xFF) * 3 + ((c >> 8) & 0xFF) * 6 + ((c >> 16) & 0xFF); }

        // Inside a region: the commonest colour (the face) and the one most unlike it (the symbol).
        void faceAndInk(Canvas &cv, int x0, int y0, int x1, int y1, bool disk, uint32_t &face, uint32_t &ink)
        {
            std::map<uint32_t, int> count;
            const float cx = (x0 + x1) * 0.5f, cy = (y0 + y1) * 0.5f, r = (x1 - x0) * 0.5f;
            for (int y = y0; y < y1; ++y)
                for (int x = x0; x < x1; ++x)
                    if (!disk || std::hypot(x + 0.5f - cx, y + 0.5f - cy) <= r)
                        ++count[cv.at(x, y)];
            face = std::max_element(count.begin(), count.end(), [](auto &a, auto &b) { return a.second < b.second; })->first;
            ink = face;
            int best = -1;
            for (auto &[c, n] : count)
                if (n >= 3 && (c >> 24) >= 0x40 && std::abs(luma(c) - luma(face)) > best)
                {
                    best = std::abs(luma(c) - luma(face));
                    ink = c;
                }
        }

        // `text` centred on (cx, cy), each font pixel `scale` texels square.
        void drawText(Canvas &cv, const std::map<char, Glyph> &font, const std::string &text, int cx, int cy, uint32_t ink, int scale)
        {
            int w = 0;
            for (char ch : text)
                w += (font.at(ch).w + 1) * scale;
            w -= scale;
            const int h = font.at(text[0]).h * scale;
            int x = cx - w / 2;
            const int y = cy - h / 2;
            for (char ch : text)
            {
                const Glyph &g = font.at(ch);
                for (int row = 0; row < g.h * scale; ++row)
                    for (int col = 0; col < g.w * scale; ++col)
                        if (g.rows[row / scale] & (1 << (g.w - 1 - col / scale)))
                            cv.at(x + col, y + row) = ink;
                x += (g.w + 1) * scale;
            }
        }

        // A round face button: the symbol inside the ring becomes a letter.
        void faceButton(Canvas &cv, float cx, float cy, float r, char letter, int scale)
        {
            const int x0 = static_cast<int>(cx - r), y0 = static_cast<int>(cy - r);
            const int x1 = static_cast<int>(cx + r + 1), y1 = static_cast<int>(cy + r + 1);
            uint32_t face, ink;
            faceAndInk(cv, x0, y0, x1, y1, true, face, ink);
            for (int y = y0; y < y1; ++y)
                for (int x = x0; x < x1; ++x)
                    if (std::hypot(x + 0.5f - cx, y + 0.5f - cy) <= r)
                        cv.at(x, y) = face;
            drawText(cv, kBig, std::string(1, letter), static_cast<int>(std::lround(cx)), static_cast<int>(std::lround(cy)), ink, scale);
        }

        // A shoulder button label (the grey plate's inside).
        void shoulder(Canvas &cv, int x, int y, const std::string &label)
        {
            const int x0 = x + 3, y0 = y + 3, x1 = x + 20, y1 = y + 9;
            uint32_t face, ink;
            faceAndInk(cv, x0, y0, x1, y1, false, face, ink);
            for (int yy = y0; yy < y1; ++yy)
                for (int xx = x0; xx < x1; ++xx)
                    cv.at(xx, yy) = face;
            drawText(cv, kSmall, label, (x0 + x1) / 2, (y0 + y1) / 2, ink, 1);
        }

        std::string g_family = "unset";

        void repaint(DecodedTexture &d, const std::string &family)
        {
            if (d.psm != 0x14u || d.width != 512 || d.height < 384 || !d.index)
                return;
            const uint64_t fp = fingerprint(d);
            static const bool debug = [] { const char *e = std::getenv("RT_GLYPH_DEBUG"); return e && *e == '1'; }();
            if (debug)
                std::fprintf(stderr, "[glyphs] 512-wide 4-bit texture, fingerprint %016llx%s\n", (unsigned long long)fp,
                             fp == kAtlasFingerprint ? " (the font atlas)" : "");
            if (fp != kAtlasFingerprint)
                return;
            Canvas cv{d};
            const bool xbox = family == "xbox";
            // The game's ✕ ○ □ △ as the pad's letters in its layout (input Mapping.h familyProfile):
            // ✕ (confirm) is A and △ (back) is B on both; ○ is Y on Xbox, X on Nintendo.
            const char cross = 'A', triangle = 'B', circle = xbox ? 'Y' : 'X', square = xbox ? 'X' : 'Y';
            faceButton(cv, 169.5f, 232.5f, 6.0f, circle, 1);
            faceButton(cv, 189.8f, 232.5f, 6.0f, cross, 1);
            faceButton(cv, 209.9f, 232.5f, 6.0f, square, 1);
            faceButton(cv, 230.0f, 232.5f, 6.0f, triangle, 1);
            faceButton(cv, 322.4f, 295.7f, 8.5f, triangle, 2); // "BACK △"
            faceButton(cv, 404.3f, 296.0f, 8.5f, cross, 2);    // "OK ✕"
            shoulder(cv, 112, 224, xbox ? "LB" : "L");
            shoulder(cv, 136, 224, xbox ? "LT" : "ZL");
            shoulder(cv, 112, 236, xbox ? "RB" : "R");
            shoulder(cv, 136, 236, xbox ? "RT" : "ZR");
        }
    }

    void updateButtonGlyphs()
    {
        ps2x::gs::PgsControl *gs = rt::settings::gsControl();
        if (!gs)
            return;
        // RT_GLYPH_FAMILY=xbox|nintendo|ps: as if that pad were connected (tests).
        const char *forced = std::getenv("RT_GLYPH_FAMILY");
        std::string family = forced && *forced ? forced : theme::padFamilyName();
        if (family == "keyboard")
            family = "ps"; // the game's own glyphs (keyboard players read the on-screen keys)
        if (family == g_family)
            return;
        g_family = family;
        if (family == "ps")
            gs->setDecodeHook({});
        else
            gs->setDecodeHook([family](DecodedTexture &d) { repaint(d, family); });
    }
}
