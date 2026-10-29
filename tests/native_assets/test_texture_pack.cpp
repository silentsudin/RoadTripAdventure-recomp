// tests/native_assets/test_texture_pack.cpp
//
// Contract tests for the app-side HD-pack glue
// (src/native/assets/png_decode.{h,cpp}, src/native/assets/texture_pack.{h,cpp})
// and for the decoder they share. Everything is built here: hand-made PNG
// streams, PPM/PAM rasters and a temp pack folder — no game bytes, no disc, no
// GPU. Deterministic end to end (the pack scan sorts file names).
//
// Covered contracts:
//   - inflate: stored block, fixed-Huffman block, and a real dynamic-Huffman
//     stream (the embedded PNG below), Adler-32 rejection;
//   - PNG: 8-bit colour types 2/6 with row filters 0..4, paletted 3 with PLTE
//     + tRNS, and refusals (16-bit, interlaced, CRC damage, short raster);
//   - PNM: P6 RGB and P7 PAM depths 1..4, header refusals;
//   - TexturePack: a pack folder serves a replacement by content key at ANY
//     pack-image size; a miss/stale `_src`/unreadable/refused file falls back
//     to the native texture (never a wrong image); load() bumps generation and
//     drops cached verdicts; clear() deactivates; a dump written by
//     dumpTexture() is directly pack-usable (the sibling "dump folder is a
//     pack" contract).
//
// Failure accounting is plain counters and if-statements (no assert()), so
// behaviour is NDEBUG-independent; any failed check exits nonzero.

#include "../../src/native/assets/hd_pack.h"
#include "../../src/native/assets/png_decode.h"
#include "../../src/native/assets/texture_pack.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

int g_failures = 0;
int g_checks = 0;

void check(bool ok, const std::string& what) {
    ++g_checks;
    if (!ok) {
        ++g_failures;
        std::cout << "FAIL: " << what << '\n';
    }
}

using rt::native::assets::DecodedImage;
using rt::native::assets::TexturePack;

std::vector<std::uint8_t> readFile(const std::filesystem::path& path) {
    std::ifstream f(path, std::ios::binary);
    return std::vector<std::uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

void writeFile(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

// ── PNG built by hand: one RGB image whose rows use filters 0..4 ────────────
std::uint32_t be32(std::uint32_t v) { return v; }  // byte order handled below

std::vector<std::uint8_t> chunk(const char type[5], const std::vector<std::uint8_t>& data) {
    std::vector<std::uint8_t> out;
    const auto pushBE = [&out](std::uint32_t v) {
        out.push_back(static_cast<std::uint8_t>(v >> 24));
        out.push_back(static_cast<std::uint8_t>(v >> 16));
        out.push_back(static_cast<std::uint8_t>(v >> 8));
        out.push_back(static_cast<std::uint8_t>(v));
    };
    pushBE(static_cast<std::uint32_t>(data.size()));
    std::vector<std::uint8_t> td(type, type + 4);
    td.insert(td.end(), data.begin(), data.end());
    out.insert(out.end(), td.begin(), td.end());
    // CRC-32, mirrored here so the test does not trust the decoder's own table.
    static std::uint32_t table[256];
    static bool ready = false;
    if (!ready) {
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
        ready = true;
    }
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::uint8_t b : td) crc = table[(crc ^ b) & 0xFFu] ^ (crc >> 8);
    pushBE(crc ^ 0xFFFFFFFFu);
    return out;
}

// Deflate with one fixed-Huffman block of literals only (exercises the Huffman
// decoder; the dynamic-table path is covered by the embedded real PNG below).
std::uint32_t reverseBits(std::uint32_t v, int n) {
    std::uint32_t r = 0;
    for (int i = 0; i < n; ++i) { r = (r << 1) | (v & 1u); v >>= 1; }
    return r;
}

std::vector<std::uint8_t> deflateFixed(const std::vector<std::uint8_t>& raw) {
    struct Out {
        std::vector<std::uint8_t> bytes;
        std::uint32_t buf = 0;
        int bits = 0;
        void put(std::uint32_t code, int n) {  // LSB-first
            buf |= (code & ((1u << n) - 1u)) << bits;
            bits += n;
            while (bits >= 8) { bytes.push_back(static_cast<std::uint8_t>(buf & 0xFFu)); buf >>= 8; bits -= 8; }
        }
        void flush() { if (bits) { bytes.push_back(static_cast<std::uint8_t>(buf & 0xFFu)); buf = 0; bits = 0; } }
    } o;
    o.put(1, 1);  // final block
    o.put(1, 2);  // fixed Huffman
    // Canonical fixed-Huffman codes (RFC 1951 3.2.6), packed MSB-first.
    const auto emit = [&o](std::uint32_t code, int n) { o.put(reverseBits(code, n), n); };
    const auto literal = [&emit](int sym) {
        if (sym < 144) emit(0x30 + static_cast<std::uint32_t>(sym), 8);
        else if (sym < 256) emit(0x190 + static_cast<std::uint32_t>(sym - 144), 9);
        else if (sym < 280) emit(static_cast<std::uint32_t>(sym - 256), 7);
        else emit(0xC0 + static_cast<std::uint32_t>(sym - 280), 8);
    };
    for (std::uint8_t b : raw) literal(b);
    literal(256);  // end of block
    o.flush();
    return o.bytes;
}

std::vector<std::uint8_t> makePng(std::vector<std::uint8_t> idat, std::uint32_t w, std::uint32_t h,
                                  std::uint8_t depth, std::uint8_t colour, std::uint8_t interlace,
                                  const std::vector<std::uint8_t>& palette = {},
                                  const std::vector<std::uint8_t>& trns = {}) {
    std::vector<std::uint8_t> out{0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    std::vector<std::uint8_t> ihdr;
    const auto pushBE = [&ihdr](std::uint32_t v) {
        ihdr.push_back(static_cast<std::uint8_t>(v >> 24));
        ihdr.push_back(static_cast<std::uint8_t>(v >> 16));
        ihdr.push_back(static_cast<std::uint8_t>(v >> 8));
        ihdr.push_back(static_cast<std::uint8_t>(v));
    };
    pushBE(w); pushBE(h);
    ihdr.push_back(depth); ihdr.push_back(colour); ihdr.push_back(0); ihdr.push_back(0);
    ihdr.push_back(interlace);
    std::vector<std::uint8_t> head = chunk("IHDR", ihdr);
    out.insert(out.end(), head.begin(), head.end());
    if (!palette.empty()) {
        std::vector<std::uint8_t> c = chunk("PLTE", palette);
        out.insert(out.end(), c.begin(), c.end());
    }
    if (!trns.empty()) {
        std::vector<std::uint8_t> c = chunk("tRNS", trns);
        out.insert(out.end(), c.begin(), c.end());
    }
    std::vector<std::uint8_t> data = chunk("IDAT", idat);
    out.insert(out.end(), data.begin(), data.end());
    std::vector<std::uint8_t> end = chunk("IEND", {});
    out.insert(out.end(), end.begin(), end.end());
    return out;
}

// Rows of a WxH 8-bit RGB image, filtered by `filterOf(row)`.
std::vector<std::uint8_t> filterRows(std::uint32_t w, std::uint32_t h, int channels,
                                     int (*filterOf)(std::uint32_t), bool paletted,
                                     std::vector<std::uint8_t>& pixelsOut) {
    const std::size_t stride = static_cast<std::size_t>(w) * channels;
    pixelsOut.assign(stride * h, 0);
    for (std::uint32_t y = 0; y < h; ++y) {
        for (std::uint32_t x = 0; x < w; ++x) {
            std::uint8_t* px = pixelsOut.data() + stride * y + static_cast<std::size_t>(x) * channels;
            if (paletted) px[0] = static_cast<std::uint8_t>((x + y) % 4);
            else if (channels == 1) px[0] = static_cast<std::uint8_t>((x * 37 + y * 91) & 0xFF);
            else if (channels == 2) { px[0] = static_cast<std::uint8_t>((x * 37 + y * 91) & 0xFF); px[1] = static_cast<std::uint8_t>(y * 60); }
            else if (channels == 3) {
                px[0] = static_cast<std::uint8_t>((x * 37 + y * 11) & 0xFF);
                px[1] = static_cast<std::uint8_t>((y * 91 + x * 3) & 0xFF);
                px[2] = static_cast<std::uint8_t>((x * 7 + y * 13) & 0xFF);
            } else {
                px[0] = px[1] = px[2] = static_cast<std::uint8_t>((x * 37 + y * 91) & 0xFF);
                px[3] = static_cast<std::uint8_t>(255 - x * 30);
            }
        }
    }
    // Filter in place against the reconstructed previous row.
    std::vector<std::uint8_t> out((stride + 1) * h);
    for (std::uint32_t y = 0; y < h; ++y) {
        const std::uint8_t* row = pixelsOut.data() + stride * y;
        const std::uint8_t* up = y ? pixelsOut.data() + stride * (y - 1) : nullptr;
        const int filter = filterOf(y);
        out[(stride + 1) * y] = static_cast<std::uint8_t>(filter);
        for (std::size_t x = 0; x < stride; ++x) {
            const int a = x >= static_cast<std::size_t>(channels) ? row[x - channels] : 0;
            const int b = up ? up[x] : 0;
            const int c = (up && x >= static_cast<std::size_t>(channels)) ? up[x - channels] : 0;
            int v = row[x];
            if (filter == 1) v -= a;
            else if (filter == 2) v -= b;
            else if (filter == 3) v -= (a + b) / 2;
            else if (filter == 4) {
                const int p = a + b - c;
                const int pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
                v -= (pa <= pb && pa <= pc) ? a : (pb <= pc ? b : c);
            }
            out[(stride + 1) * y + 1 + x] = static_cast<std::uint8_t>(v & 0xFF);
        }
    }
    return out;
}

int cycleFilter(std::uint32_t y) { return static_cast<int>(y % 5); }
int filter0(std::uint32_t) { return 0; }
int filter2(std::uint32_t) { return 2; }

// ── PNM builders ───────────────────────────────────────────────────────────
std::vector<std::uint8_t> makePpm(std::uint32_t w, std::uint32_t h) {
    std::string header = "P6\n" + std::to_string(w) + " " + std::to_string(h) + "\n255\n";
    std::vector<std::uint8_t> out(header.begin(), header.end());
    for (std::uint32_t y = 0; y < h; ++y)
        for (std::uint32_t x = 0; x < w; ++x) {
            out.push_back(static_cast<std::uint8_t>((x * 37 + y * 11) & 0xFF));
            out.push_back(static_cast<std::uint8_t>((y * 91 + x * 3) & 0xFF));
            out.push_back(static_cast<std::uint8_t>((x * 7 + y * 13) & 0xFF));
        }
    return out;
}

std::vector<std::uint8_t> makePam(std::uint32_t w, std::uint32_t h, int depth) {
    const char* tuple = depth == 4 ? "RGB_ALPHA" : depth == 3 ? "RGB" : depth == 2 ? "GRAYSCALE_ALPHA" : "GRAYSCALE";
    std::string header = "P7\nWIDTH " + std::to_string(w) + "\nHEIGHT " + std::to_string(h) +
                         "\nDEPTH " + std::to_string(depth) + "\nMAXVAL 255\nTUPLTYPE " + tuple + "\nENDHDR\n";
    std::vector<std::uint8_t> out(header.begin(), header.end());
    for (std::uint32_t y = 0; y < h; ++y)
        for (std::uint32_t x = 0; x < w; ++x) {
            const std::uint8_t v = static_cast<std::uint8_t>((x * 37 + y * 91) & 0xFF);
            if (depth == 1) out.push_back(v);
            else if (depth == 2) { out.push_back(v); out.push_back(static_cast<std::uint8_t>(255 - x * 40)); }
            else if (depth == 3) { out.push_back(v); out.push_back(static_cast<std::uint8_t>((y * 17) & 0xFF)); out.push_back(static_cast<std::uint8_t>((x * 5) & 0xFF)); }
            else { out.push_back(v); out.push_back(v); out.push_back(v); out.push_back(static_cast<std::uint8_t>(255 - x * 40)); }
        }
    return out;
}

// ── The real dynamic-Huffman PNG (zlib level 9, 4x4 RGBA, filter 0) ─────────
// Encoded once with Python's zlib so the decoder's dynamic-table path is
// covered by a stream it did not write. Expected pixels are the formula in
// the generator: (y*16, y*16, x*37+y*91, 255-x*40).
const std::uint8_t kDynamicPng[] = {
    137, 80, 78, 71, 13, 10, 26, 10, 0, 0, 0, 13, 73, 72, 68, 82, 0, 0, 0, 4, 0, 0, 0, 4, 8, 6, 0, 0, 0,
    169, 241, 158, 126, 0, 0, 0, 72, 73, 68, 65, 84, 120, 218, 5, 193, 49, 1, 128, 48, 16, 3, 192, 72, 8,
    14, 178, 116, 207, 132, 8, 44, 84, 4, 22, 186, 189, 18, 230, 223, 16, 81, 3, 63, 32, 230, 185, 3, 128,
    6, 70, 1, 87, 2, 119, 128, 156, 77, 174, 34, 159, 36, 119, 64, 122, 91, 250, 74, 66, 74, 35, 96, 31,
    109, 159, 101, 207, 180, 87, 252, 155, 14, 21, 49, 90, 219, 74, 153, 0, 0, 0, 0, 73, 69, 78, 68, 174,
    66, 96, 130,
};

void testInflate() {
    std::vector<std::uint8_t> out;
    std::string why;
    // Stored block: the app's own PNG writer layout.
    const std::vector<std::uint8_t> payload{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<std::uint8_t> z{0x78, 0x01, 0x01, 10, 0, 0xF5, 0xFF};
    z.insert(z.end(), payload.begin(), payload.end());
    std::uint32_t a = 1, b = 0;
    for (std::uint8_t c : payload) { a = (a + c) % 65521u; b = (b + a) % 65521u; }
    const std::uint32_t adler = (b << 16) | a;
    z.push_back(static_cast<std::uint8_t>(adler >> 24));
    z.push_back(static_cast<std::uint8_t>(adler >> 16));
    z.push_back(static_cast<std::uint8_t>(adler >> 8));
    z.push_back(static_cast<std::uint8_t>(adler));
    check(rt::native::assets::inflateZlib(z, out, why) && out == payload, "inflate stored block");

    // Fixed Huffman: written by this test's own literal encoder.
    std::vector<std::uint8_t> raw(200);
    for (std::size_t i = 0; i < raw.size(); ++i) raw[i] = static_cast<std::uint8_t>((i * 37) & 0xFF);
    check(rt::native::assets::inflateZlib(deflateFixed(raw), out, why) && out == raw,
          "inflate fixed-Huffman block");

    // Dynamic Huffman: the embedded PNG's IDAT decoded by the PNG path below,
    // and the raw stream through inflateZlib.
    const DecodedImage png = rt::native::assets::decodePng(
        std::span<const std::uint8_t>(kDynamicPng, sizeof(kDynamicPng)));
    check(png.ok && png.width == 4 && png.height == 4, "dynamic-Huffman PNG decodes");
    if (png.ok) {
        for (std::uint32_t y = 0; y < 4; ++y)
            for (std::uint32_t x = 0; x < 4; ++x) {
                const std::uint8_t* p = png.rgba.data() + (static_cast<std::size_t>(y) * 4 + x) * 4;
                check(p[0] == y * 16 && p[1] == y * 16 && p[2] == ((x * 37 + y * 91) & 0xFF) &&
                          p[3] == 255 - x * 40,
                      "dynamic-Huffman PNG pixels");
            }
    }
    // A stream that expands past the caller's limit is refused before the
    // memory is committed (pack images are untrusted input).
    std::vector<std::uint8_t> big(400, 7);
    check(!rt::native::assets::inflateZlib(deflateFixed(big), out, why, 10),
          "inflate refuses a stream past its size limit");

    // Adler mismatch is refused.
    std::vector<std::uint8_t> bad = z;
    bad.back() ^= 0xFF;
    check(!rt::native::assets::inflateZlib(bad, out, why), "inflate rejects a bad Adler-32");
}

void testPng() {
    std::vector<std::uint8_t> pixels;
    const std::uint32_t w = 7, h = 5;

    // RGB8, every filter type.
    std::vector<std::uint8_t> rows = filterRows(w, h, 3, cycleFilter, false, pixels);
    const DecodedImage rgb = rt::native::assets::decodePng(makePng(deflateFixed(rows), w, h, 8, 2, 0));
    check(rgb.ok, "PNG RGB8 (filters 0..4) decodes");
    if (rgb.ok) {
        bool same = rgb.width == w && rgb.height == h;
        for (std::uint32_t y = 0; same && y < h; ++y)
            for (std::uint32_t x = 0; same && x < w; ++x) {
                const std::uint8_t* a = rgb.rgba.data() + (static_cast<std::size_t>(y) * w + x) * 4;
                const std::uint8_t* e = pixels.data() + (static_cast<std::size_t>(y) * w + x) * 3;
                same = a[0] == e[0] && a[1] == e[1] && a[2] == e[2] && a[3] == 255;
            }
        check(same, "PNG RGB8 pixels + implicit opaque alpha");
    }

    // RGBA8 with the alpha channel kept.
    rows = filterRows(w, h, 4, cycleFilter, false, pixels);
    const DecodedImage rgba = rt::native::assets::decodePng(makePng(deflateFixed(rows), w, h, 8, 6, 0));
    check(rgba.ok, "PNG RGBA8 decodes");
    if (rgba.ok) check(rgba.rgba[3] == 255 && rgba.rgba[4 * (w + 1) + 3] == 255 - 30,
                       "PNG RGBA8 alpha kept as stored");

    // Paletted with PLTE + tRNS.
    const std::vector<std::uint8_t> plte{10, 20, 30, 200, 100, 50, 0, 0, 0, 255, 255, 255};
    const std::vector<std::uint8_t> trns{255, 128, 0};
    rows = filterRows(w, h, 1, filter2, true, pixels);
    const DecodedImage pal = rt::native::assets::decodePng(makePng(deflateFixed(rows), w, h, 8, 3, 0, plte, trns));
    check(pal.ok, "PNG paletted decodes");
    if (pal.ok) {
        check(pal.rgba[0] == 10 && pal.rgba[1] == 20 && pal.rgba[2] == 30 && pal.rgba[3] == 255,
              "PNG palette entry 0 + opaque tRNS default");
        check(pal.rgba[4 + 3] == 128, "PNG tRNS alpha applies per index");
        check(pal.rgba[2 * 4 + 3] == 0, "PNG tRNS zero alpha (index 2)");
    }

    // Refusals.
    rows = filterRows(w, h, 3, filter0, false, pixels);
    std::span<const std::uint8_t> dummy;
    (void)dummy;
    check(!rt::native::assets::decodePng(makePng(deflateFixed(rows), w, h, 16, 2, 0)).ok,
          "16-bit PNG refused");
    check(!rt::native::assets::decodePng(makePng(deflateFixed(rows), w, h, 8, 2, 1)).ok,
          "interlaced PNG refused");
    std::vector<std::uint8_t> damaged = makePng(deflateFixed(rows), w, h, 8, 2, 0);
    damaged[30] ^= 0xFF;  // inside IHDR data
    check(!rt::native::assets::decodePng(damaged).ok, "PNG CRC damage refused");
    std::vector<std::uint8_t> shortRaster = rows;
    shortRaster.resize(shortRaster.size() - 3);
    check(!rt::native::assets::decodePng(makePng(deflateFixed(shortRaster), w, h, 8, 2, 0)).ok,
          "short PNG raster refused");
    check(!rt::native::assets::decodePng(std::vector<std::uint8_t>{1, 2, 3}).ok, "non-PNG refused");
    check(!rt::native::assets::decodePng(makePng(deflateFixed(rows), 8192, 8192, 8, 2, 0)).ok,
          "PNG past the pack pixel cap refused before allocating");
    check(!rt::native::assets::decodePnm(makePpm(8192, 8192)).ok,
          "PPM past the pack pixel cap refused");
    // A small header with a huge IDAT: refused before the stream is kept.
    const std::vector<std::uint8_t> junk(200000, 0);
    check(!rt::native::assets::decodePng(makePng(junk, 1, 1, 8, 2, 0)).ok,
          "PNG whose IDAT exceeds what its raster needs is refused");

    // Image data before the IHDR is refused (either length), rather than
    // being buffered until a later header makes it look legal.
    const auto ihdr13 = [](std::uint32_t iw, std::uint32_t ih, std::uint8_t depth_, std::uint8_t colour_) {
        std::vector<std::uint8_t> d;
        const auto push = [&d](std::uint32_t v) {
            d.push_back(static_cast<std::uint8_t>(v >> 24)); d.push_back(static_cast<std::uint8_t>(v >> 16));
            d.push_back(static_cast<std::uint8_t>(v >> 8)); d.push_back(static_cast<std::uint8_t>(v));
        };
        push(iw); push(ih);
        d.push_back(depth_); d.push_back(colour_); d.push_back(0); d.push_back(0); d.push_back(0);
        return d;
    };
    const auto pngFrom = [](const std::vector<std::vector<std::uint8_t>>& chunks) {
        std::vector<std::uint8_t> out{0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
        for (const auto& c : chunks) out.insert(out.end(), c.begin(), c.end());
        return out;
    };
    const std::uint32_t ew = 2, eh = 2;
    std::vector<std::uint8_t> epixels;
    const std::vector<std::uint8_t> erows = filterRows(ew, eh, 3, filter0, false, epixels);
    check(!rt::native::assets::decodePng(pngFrom({chunk("IDAT", {}),
                                                  chunk("IHDR", ihdr13(ew, eh, 8, 2)),
                                                  chunk("IEND", {})}))
               .ok,
          "zero-length IDAT before IHDR refused");
    check(!rt::native::assets::decodePng(pngFrom({chunk("IDAT", deflateFixed(erows)),
                                                  chunk("IHDR", ihdr13(ew, eh, 8, 2)),
                                                  chunk("IEND", {})}))
               .ok,
          "IDAT before IHDR refused");
}

void testPnm() {
    const std::uint32_t w = 5, h = 3;
    const DecodedImage ppm = rt::native::assets::decodePnm(makePpm(w, h));
    check(ppm.ok && ppm.width == w && ppm.height == h, "PPM P6 decodes");
    if (ppm.ok) {
        const std::uint8_t* p = ppm.rgba.data() + (2 * w + 3) * 4;
        check(p[0] == ((3 * 37 + 2 * 11) & 0xFF) && p[1] == ((2 * 91 + 3 * 3) & 0xFF) &&
                  p[2] == ((3 * 7 + 2 * 13) & 0xFF) && p[3] == 255,
              "PPM P6 pixels");
    }
    for (int depth = 1; depth <= 4; ++depth) {
        const DecodedImage pam = rt::native::assets::decodePnm(makePam(w, h, depth));
        check(pam.ok && pam.width == w && pam.rgba.size() == static_cast<std::size_t>(w) * h * 4,
              "PAM depth " + std::to_string(depth) + " decodes");
        if (pam.ok && depth == 4)
            check(pam.rgba[4 * 2 + 3] == 255 - 2 * 40, "PAM RGB_ALPHA alpha kept");
        if (pam.ok && depth == 3) check(pam.rgba[4 * 2 + 3] == 255, "PAM RGB alpha defaults opaque");
    }
    check(!rt::native::assets::decodePnm(std::vector<std::uint8_t>{}).ok, "empty PNM refused");
    std::vector<std::uint8_t> badHeader = makePpm(w, h);
    badHeader[3] = 'x';
    check(!rt::native::assets::decodePnm(badHeader).ok, "malformed PPM header refused");
}

// A small 8x8 texture the pack tests key on.
struct Tex {
    std::uint32_t w = 8, h = 8;
    std::vector<std::uint8_t> rgba;
    explicit Tex(int seed = 0) {
        rgba.resize(static_cast<std::size_t>(w) * h * 4);
        for (std::uint32_t y = 0; y < h; ++y)
            for (std::uint32_t x = 0; x < w; ++x) {
                std::uint8_t* p = rgba.data() + (static_cast<std::size_t>(y) * w + x) * 4;
                p[0] = static_cast<std::uint8_t>(x * 31 + seed);
                p[1] = static_cast<std::uint8_t>(y * 31);
                p[2] = static_cast<std::uint8_t>((x + y) * 16);
                p[3] = 255;
            }
    }
};

void testTexturePack() {
    namespace fs = std::filesystem;
    using namespace rt::native::assets;
    const fs::path root = fs::temp_directory_path() / "rt_texture_pack_test";
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root / "packA");
    fs::create_directories(root / "packB");

    const Tex tex;
    const StableContentKey key = stableContentKey(tex.w, tex.h, tex.rgba);
    check(!key.isNull(), "texture keys");

    // Pack A: a 2x upscaled PPM under the dump-style name.
    writeFile(root / "packA" / (stableKeyToString(key) + "_16x16.ppm"), makePpm(16, 16));
    // A stale `_src` image (named against a shape this texture is not) proves
    // the guard falls back instead of binding the wrong image; it lives in its
    // own folder so it cannot collide with pack A's real entry.
    fs::create_directories(root / "packStale");
    writeFile(root / "packStale" / (stableKeyToString(key) + "_16x16_src4x4.png"), makePpm(16, 16));
    const Tex other{7};
    const StableContentKey otherKey = stableContentKey(other.w, other.h, other.rgba);

    TexturePack pack;
    std::string error;
    check(pack.load(root / "packA", error), "pack loads");
    check(pack.imageCount() == 1, "pack index count");
    const std::uint32_t genA = pack.generation();
    check(genA != 0, "load bumps generation");

    const DecodedImage* rep = pack.replacement(tex.w, tex.h, tex.rgba);
    check(rep != nullptr && rep->width == 16 && rep->height == 16, "replacement served at pack size");
    if (rep) {
        // The 16x16 PPM's pixel (3,2) is the same formula at that size.
        const std::uint8_t* p = rep->rgba.data() + (2 * 16 + 3) * 4;
        check(p[0] == ((3 * 37 + 2 * 11) & 0xFF) && p[3] == 255, "replacement pixels decoded");
    }
    check(pack.replacement(tex.w, tex.h, tex.rgba) == rep, "replacement is cached");
    check(pack.replacement(other.w, other.h, other.rgba) == nullptr, "miss falls back to native");
    check(pack.replacement(tex.w, tex.h, tex.rgba) == rep, "the same content keys the same verdict");
    check(pack.replacement(tex.w, tex.h + 1, tex.rgba) == nullptr, "shape mismatch is a miss");

    // Switch: generation changes and the old verdicts are gone.
    writeFile(root / "packB" / (stableKeyToString(key) + "_16x16.pam"), makePam(16, 16, 4));
    check(pack.load(root / "packB", error), "second pack loads");
    check(pack.generation() != genA, "switch bumps generation");
    const DecodedImage* repB = pack.replacement(tex.w, tex.h, tex.rgba);
    check(repB != nullptr && repB->width == 16, "second pack serves");
    if (repB) check(repB->rgba[3] == 255, "PAM replacement alpha kept");

    // Stale `_src` naming: refused, native texture drawn.
    check(pack.load(root / "packStale", error), "stale pack loads");
    check(pack.replacement(tex.w, tex.h, tex.rgba) == nullptr, "stale _src image falls back");

    // Unreadable file: refused, no crash, still a miss.
    fs::create_directories(root / "packC");
    writeFile(root / "packC" / (stableKeyToString(otherKey) + "_8x8.png"), {1, 2, 3, 4, 5});
    check(pack.load(root / "packC", error), "third pack loads");
    check(pack.replacement(other.w, other.h, other.rgba) == nullptr, "unreadable pack image falls back");

    // A file bigger than a legal pack image is never read.
    fs::create_directories(root / "packBig");
    {
        std::ofstream big(root / "packBig" / (stableKeyToString(key) + "_8x8.png"),
                          std::ios::binary);
        big.seekp(static_cast<std::streamoff>(kMaxPackFileBytes + 1));
        big.put('x');
    }
    check(pack.load(root / "packBig", error), "oversize pack loads");
    check(pack.replacement(tex.w, tex.h, tex.rgba) == nullptr, "oversize pack image is not read");

    // A dump is directly pack-usable.
    fs::create_directories(root / "dumps");
    setDumpDirectory(root / "dumps");
    const fs::path dumped = dumpTexture(tex.w, tex.h, tex.rgba);
    check(!dumped.empty() && fs::exists(dumped), "dump written");
    check(dumpTexture(tex.w, tex.h, tex.rgba).empty(), "a texture is dumped once");
    setDumpDirectory({});
    check(pack.load(root / "dumps", error), "dump folder acts as a pack");
    const DecodedImage* fromDump = pack.replacement(tex.w, tex.h, tex.rgba);
    check(fromDump != nullptr && fromDump->width == tex.w && fromDump->height == tex.h,
          "dump round-trips into a replacement");
    if (fromDump)
        check(fromDump->rgba == tex.rgba, "dump round-trip pixels identical (dumping changes no pixel)");

    pack.clear();
    check(!pack.active() && pack.replacement(tex.w, tex.h, tex.rgba) == nullptr, "clear deactivates");
    check(!pack.replacement(0, 0, std::span<const std::uint8_t>{}), "null key is never replaced");

    fs::remove_all(root, ec);
}

}  // namespace

int main() {
    testInflate();
    testPng();
    testPnm();
    testTexturePack();
    std::cout << (g_failures ? "FAILED " : "ok ") << g_checks - g_failures << "/" << g_checks << " checks\n";
    return g_failures ? 1 : 0;
}
