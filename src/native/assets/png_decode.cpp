// src/native/assets/png_decode.cpp — see png_decode.h for the contract.
#include "assets/png_decode.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace rt::native::assets {
namespace {

// ── CRC-32 (PNG/IEEE), table built once ─────────────────────────────────────
std::uint32_t crc32(const std::uint8_t* p, std::size_t n) {
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
    std::uint32_t c = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < n; ++i) c = table[(c ^ p[i]) & 0xFFu] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}

// ── bit reader (LSB first, as deflate wants) ────────────────────────────────
struct Bits {
    const std::uint8_t* data = nullptr;
    std::size_t size = 0, pos = 0;
    std::uint32_t buf = 0;
    int count = 0;

    bool get(int n, std::uint32_t& out) {
        while (count < n) {
            if (pos >= size) return false;
            buf |= static_cast<std::uint32_t>(data[pos++]) << count;
            count += 8;
        }
        out = buf & ((n == 32) ? 0xFFFFFFFFu : ((1u << n) - 1u));
        buf >>= n;
        count -= n;
        return true;
    }
    bool bit(int& out) {
        std::uint32_t v = 0;
        if (!get(1, v)) return false;
        out = static_cast<int>(v);
        return true;
    }
    bool alignToByte() {
        const int drop = count % 8;
        buf >>= drop;
        count -= drop;
        return true;
    }
};

// ── canonical Huffman ───────────────────────────────────────────────────────
struct Huffman {
    std::vector<int> count;    // count[len], len 1..15
    std::vector<int> firstCode;  // first code of each length
    std::vector<int> offset;   // index into symbols of the first symbol of that length
    std::vector<int> symbols;

    bool build(const std::vector<std::uint8_t>& lengths, int maxLen) {
        count.assign(static_cast<std::size_t>(maxLen) + 1, 0);
        for (std::uint8_t l : lengths) {
            if (l > maxLen) return false;
            if (l) ++count[l];
        }
        firstCode.assign(count.size(), 0);
        offset.assign(count.size(), 0);
        int code = 0, idx = 0;
        for (int len = 1; len <= maxLen; ++len) {
            firstCode[len] = code;
            offset[len] = idx;
            code += count[len];
            idx += count[len];
            code <<= 1;
        }
        symbols.assign(static_cast<std::size_t>(idx), 0);
        std::vector<int> cursor = offset;
        for (std::size_t s = 0; s < lengths.size(); ++s)
            if (lengths[s]) symbols[cursor[lengths[s]]++] = static_cast<int>(s);
        return true;
    }

    bool decode(Bits& in, int& sym) const {
        int code = 0;
        for (int len = 1; len < static_cast<int>(count.size()); ++len) {
            int b = 0;
            if (!in.bit(b)) return false;
            code = (code << 1) | b;
            const int cnt = count[len];
            if (cnt && code - firstCode[len] < cnt) {
                sym = symbols[offset[len] + (code - firstCode[len])];
                return true;
            }
        }
        return false;
    }
};

constexpr int kLengthBase[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27,
                                 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
constexpr int kLengthExtra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
                                  2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
constexpr int kDistBase[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129,
                               193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097,
                               6145, 8193, 12289, 16385, 24577};
constexpr int kDistExtra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
                                6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

bool fixedTables(Huffman& lit, Huffman& dist) {
    std::vector<std::uint8_t> l(288);
    for (int i = 0; i < 144; ++i) l[static_cast<std::size_t>(i)] = 8;
    for (int i = 144; i < 256; ++i) l[static_cast<std::size_t>(i)] = 9;
    for (int i = 256; i < 280; ++i) l[static_cast<std::size_t>(i)] = 7;
    for (int i = 280; i < 288; ++i) l[static_cast<std::size_t>(i)] = 8;
    if (!lit.build(l, 15)) return false;
    std::vector<std::uint8_t> d(30, 5);
    return dist.build(d, 15);
}

bool dynamicTables(Bits& in, Huffman& lit, Huffman& dist) {
    std::uint32_t hlit = 0, hdist = 0, hclen = 0;
    if (!in.get(5, hlit) || !in.get(5, hdist) || !in.get(4, hclen)) return false;
    hlit += 257; hdist += 1; hclen += 4;
    if (hlit > 286 || hdist > 30) return false;
    static constexpr int order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    std::vector<std::uint8_t> cl(19, 0);
    for (std::uint32_t i = 0; i < hclen; ++i) {
        std::uint32_t v = 0;
        if (!in.get(3, v)) return false;
        cl[static_cast<std::size_t>(order[i])] = static_cast<std::uint8_t>(v);
    }
    Huffman clh;
    if (!clh.build(cl, 15)) return false;
    std::vector<std::uint8_t> lengths;
    lengths.reserve(hlit + hdist);
    const std::size_t total = hlit + hdist;
    while (lengths.size() < total) {
        int sym = 0;
        if (!clh.decode(in, sym)) return false;
        if (sym < 16) {
            lengths.push_back(static_cast<std::uint8_t>(sym));
        } else if (sym == 16) {
            if (lengths.empty()) return false;
            std::uint32_t rep = 0;
            if (!in.get(2, rep)) return false;
            const std::uint8_t prev = lengths.back();
            for (std::uint32_t i = 0; i < 3 + rep; ++i) lengths.push_back(prev);
        } else if (sym == 17) {
            std::uint32_t rep = 0;
            if (!in.get(3, rep)) return false;
            for (std::uint32_t i = 0; i < 3 + rep; ++i) lengths.push_back(0);
        } else if (sym == 18) {
            std::uint32_t rep = 0;
            if (!in.get(7, rep)) return false;
            for (std::uint32_t i = 0; i < 11 + rep; ++i) lengths.push_back(0);
        } else {
            return false;
        }
    }
    if (lengths.size() != total) return false;
    std::vector<std::uint8_t> litLen(lengths.begin(), lengths.begin() + hlit);
    std::vector<std::uint8_t> distLen(lengths.begin() + hlit, lengths.end());
    if (!lit.build(litLen, 15) || !dist.build(distLen, 15)) return false;
    return true;
}

bool inflateBlockData(Bits& in, const Huffman& lit, const Huffman& dist, std::vector<std::uint8_t>& out,
                      std::size_t maxOut) {
    for (;;) {
        int sym = 0;
        if (!lit.decode(in, sym)) return false;
        if (sym < 256) {
            if (out.size() >= maxOut) return false;
            out.push_back(static_cast<std::uint8_t>(sym));
            continue;
        }
        if (sym == 256) return true;  // end of block
        if (sym > 285) return false;
        const int li = sym - 257;
        std::uint32_t extra = 0;
        if (kLengthExtra[li] && !in.get(kLengthExtra[li], extra)) return false;
        const std::size_t length = static_cast<std::size_t>(kLengthBase[li] + static_cast<int>(extra));
        int dsym = 0;
        if (!dist.decode(in, dsym)) return false;
        if (dsym > 29) return false;
        extra = 0;
        if (kDistExtra[dsym] && !in.get(kDistExtra[dsym], extra)) return false;
        const std::size_t distance = static_cast<std::size_t>(kDistBase[dsym] + static_cast<int>(extra));
        if (distance == 0 || distance > out.size()) return false;
        if (length > maxOut - out.size()) return false;
        const std::size_t start = out.size() - distance;
        for (std::size_t i = 0; i < length; ++i) out.push_back(out[start + i % distance]);
    }
}

bool inflateRaw(const std::uint8_t* data, std::size_t size, std::vector<std::uint8_t>& out, std::string& why,
                std::size_t maxOut) {
    Huffman lit, dist;
    Bits in{data, size, 0, 0, 0};
    for (;;) {
        std::uint32_t final_bit = 0, type = 0;
        if (!in.get(1, final_bit) || !in.get(2, type)) { why = "deflate header truncated"; return false; }
        if (type == 0) {
            in.alignToByte();
            std::uint32_t len = 0, nlen = 0;
            if (!in.get(16, len) || !in.get(16, nlen)) { why = "stored block header truncated"; return false; }
            if ((len ^ 0xFFFFu) != nlen) { why = "stored block length mismatch"; return false; }
            if (len > maxOut - out.size()) { why = "stored block expands past the size limit"; return false; }
            for (std::uint32_t i = 0; i < len; ++i) {
                std::uint32_t b = 0;
                if (!in.get(8, b)) { why = "stored block data truncated"; return false; }
                out.push_back(static_cast<std::uint8_t>(b));
            }
        } else if (type == 1 || type == 2) {
            if (type == 1) {
                if (!fixedTables(lit, dist)) { why = "bad fixed tables"; return false; }
            } else if (!dynamicTables(in, lit, dist)) {
                why = "bad dynamic Huffman tables";
                return false;
            }
            if (!inflateBlockData(in, lit, dist, out, maxOut)) { why = "deflate stream is not decodable"; return false; }
        } else {
            why = "reserved deflate block type";
            return false;
        }
        if (final_bit) return true;
    }
}

// ── PNG helpers ────────────────────────────────────────────────────────────
std::uint32_t be32(const std::uint8_t* p) {
    return (static_cast<std::uint32_t>(p[0]) << 24) | (static_cast<std::uint32_t>(p[1]) << 16) |
           (static_cast<std::uint32_t>(p[2]) << 8) | static_cast<std::uint32_t>(p[3]);
}

int paeth(int a, int b, int c) {
    const int p = a + b - c;
    const int pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    return pb <= pc ? b : c;
}

// Netpbm header tokenizer (whitespace separated, '#' comments).
struct PnmTokens {
    const std::uint8_t* p = nullptr;
    std::size_t n = 0, i = 0;
    bool next(long long& out) {
        while (i < n) {
            while (i < n && (p[i] == ' ' || p[i] == '\t' || p[i] == '\r' || p[i] == '\n')) ++i;
            if (i < n && p[i] == '#') { while (i < n && p[i] != '\n') ++i; continue; }
            break;
        }
        if (i >= n) return false;
        long long v = 0;
        bool any = false;
        while (i < n && p[i] >= '0' && p[i] <= '9') { v = v * 10 + (p[i] - '0'); ++i; any = true; }
        out = v;
        return any;
    }
};

}  // namespace

bool inflateZlib(std::span<const std::uint8_t> bytes, std::vector<std::uint8_t>& out, std::string& why,
                 std::size_t maxOut) {
    out.clear();
    std::size_t offset = 0;
    bool hadZlibHeader = false;
    if (bytes.size() >= 2 && (bytes[0] & 0x0Fu) == 8 && ((bytes[0] << 8) | bytes[1]) % 31 == 0) {
        hadZlibHeader = true;
        offset = 2;
    }
    if (!inflateRaw(bytes.data() + offset, bytes.size() - offset, out, why, maxOut)) return false;
    if (hadZlibHeader) {
        const std::size_t tail = bytes.size();
        if (tail >= offset + 4) {
            std::uint32_t a = 1, b = 0;
            for (std::uint8_t c : out) { a = (a + c) % 65521u; b = (b + a) % 65521u; }
            const std::uint32_t expected = (b << 16) | a;
            const std::uint32_t stored = be32(bytes.data() + tail - 4);
            if (expected != stored) { why = "zlib Adler-32 mismatch"; out.clear(); return false; }
        }
    }
    return true;
}

DecodedImage decodePng(std::span<const std::uint8_t> bytes) {
    DecodedImage out;
    static constexpr std::uint8_t sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    if (bytes.size() < 8 || std::memcmp(bytes.data(), sig, 8) != 0) { out.why = "not a PNG"; return out; }
    std::size_t pos = 8;
    std::uint32_t width = 0, height = 0;
    int depth = 0, colour = -1, interlace = 0, channels = 0;
    std::size_t maxIdat = 0;  // cap on the compressed stream for this raster
    std::vector<std::uint8_t> palette, transparency, idat;
    bool sawHeader = false;
    while (pos + 8 <= bytes.size()) {
        const std::uint32_t len = be32(bytes.data() + pos);
        const char* type = reinterpret_cast<const char*>(bytes.data() + pos + 4);
        if (pos + 12 + len > bytes.size()) { out.why = "PNG chunk runs past the file"; return out; }
        const std::uint8_t* data = bytes.data() + pos + 8;
        const std::uint32_t stored = be32(data + len);
        if (crc32(bytes.data() + pos + 4, static_cast<std::size_t>(len) + 4) != stored) {
            out.why = "PNG chunk CRC mismatch";
            return out;
        }
        if (std::memcmp(type, "IHDR", 4) == 0) {
            if (len != 13) { out.why = "bad IHDR"; return out; }
            width = be32(data); height = be32(data + 4);
            depth = data[8]; colour = data[9]; interlace = data[12];
            if (data[10] != 0 || data[11] != 0) { out.why = "unknown PNG compression/filter method"; return out; }
            // Validate the header the moment it is read: a hostile IHDR or IDAT
            // must be refused before it can drive an allocation, and this
            // raster size bounds the compressed stream allowed below.
            if (width == 0 || height == 0 ||
                static_cast<std::uint64_t>(width) * height > kMaxPackPixels) {
                out.why = "PNG size out of range";
                return out;
            }
            if (interlace != 0) { out.why = "interlaced PNG is not supported"; return out; }
            if (depth != 8) { out.why = "only 8-bit PNGs are supported"; return out; }
            switch (colour) {
                case 0: channels = 1; break;
                case 2: channels = 3; break;
                case 3: channels = 1; break;
                case 4: channels = 2; break;
                case 6: channels = 4; break;
                default: out.why = "unknown PNG colour type"; return out;
            }
            const std::size_t raster =
                (static_cast<std::size_t>(width) * channels + 1) * height;
            // Deliberate RESOURCE cap, not a claim about legal streams: a
            // deflate literal costs at most 15 bits (1.875 bytes per output
            // byte), so 2*raster + 64 KiB leaves room for any image plus its
            // block and chunk headers. A stream above it is refused rather
            // than kept, which can also refuse a legal stream padded with
            // empty blocks - a pad makes no image, so that is intended.
            maxIdat = 2 * raster + 65536;
            sawHeader = true;
        } else if (std::memcmp(type, "PLTE", 4) == 0) {
            palette.assign(data, data + len);
        } else if (std::memcmp(type, "tRNS", 4) == 0) {
            transparency.assign(data, data + len);
        } else if (std::memcmp(type, "IDAT", 4) == 0) {
            if (!sawHeader) { out.why = "PNG image data before IHDR"; return out; }
            if (idat.size() + len > maxIdat) { out.why = "PNG IDAT is larger than the raster can need"; return out; }
            idat.insert(idat.end(), data, data + len);
        } else if (std::memcmp(type, "IEND", 4) == 0) {
            break;
        }
        pos += 12 + len;
    }
    if (!sawHeader) { out.why = "PNG has no IHDR"; return out; }
    if (colour == 3 && palette.empty()) { out.why = "paletted PNG has no PLTE"; return out; }
    const std::size_t stride = static_cast<std::size_t>(width) * channels;
    std::vector<std::uint8_t> raw;
    std::string why;
    // The raster size is exactly (stride + 1) * height: the inflate refuses a
    // stream that would expand past it, so a lying IDAT cannot allocate.
    if (!inflateZlib(idat, raw, why, (stride + 1) * height)) { out.why = "PNG IDAT: " + why; return out; }
    if (raw.size() < (stride + 1) * height) { out.why = "PNG IDAT is short"; return out; }
    // Unfilter in place into `pixels`.
    std::vector<std::uint8_t> pixels(stride * height);
    for (std::uint32_t y = 0; y < height; ++y) {
        const std::uint8_t* src = raw.data() + (stride + 1) * y;
        const int filter = src[0];
        const std::uint8_t* in = src + 1;
        std::uint8_t* dst = pixels.data() + stride * y;
        const std::uint8_t* up = y ? pixels.data() + stride * (y - 1) : nullptr;
        for (std::size_t x = 0; x < stride; ++x) {
            const int a = x >= static_cast<std::size_t>(channels) ? dst[x - channels] : 0;
            const int b = up ? up[x] : 0;
            const int c = (up && x >= static_cast<std::size_t>(channels)) ? up[x - channels] : 0;
            int v = in[x];
            switch (filter) {
                case 0: break;
                case 1: v += a; break;
                case 2: v += b; break;
                case 3: v += (a + b) / 2; break;
                case 4: v += paeth(a, b, c); break;
                default: out.why = "unknown PNG row filter"; return out;
            }
            dst[x] = static_cast<std::uint8_t>(v & 0xFF);
        }
    }
    out.width = width;
    out.height = height;
    out.rgba.resize(static_cast<std::size_t>(width) * height * 4);
    const auto lookupPalette = [&](std::size_t index, std::uint8_t rgba[4]) {
        if (index * 3 + 2 >= palette.size()) return false;
        rgba[0] = palette[index * 3];
        rgba[1] = palette[index * 3 + 1];
        rgba[2] = palette[index * 3 + 2];
        rgba[3] = index < transparency.size() ? transparency[index] : 255;
        return true;
    };
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            const std::uint8_t* px = pixels.data() + (static_cast<std::size_t>(y) * width + x) * channels;
            std::uint8_t* dst = out.rgba.data() + (static_cast<std::size_t>(y) * width + x) * 4;
            switch (colour) {
                case 0: dst[0] = dst[1] = dst[2] = px[0]; dst[3] = 255; break;
                case 2: dst[0] = px[0]; dst[1] = px[1]; dst[2] = px[2]; dst[3] = 255; break;
                case 3:
                    if (!lookupPalette(px[0], dst)) { out.why = "PNG palette index outside PLTE"; return out; }
                    break;
                case 4: dst[0] = dst[1] = dst[2] = px[0]; dst[3] = px[1]; break;
                case 6: std::memcpy(dst, px, 4); break;
                default: break;
            }
        }
    }
    out.ok = true;
    return out;
}

DecodedImage decodePnm(std::span<const std::uint8_t> bytes) {
    DecodedImage out;
    if (bytes.size() < 2 || bytes[0] != 'P') { out.why = "not a Netpbm image"; return out; }
    PnmTokens tok{bytes.data(), bytes.size(), 0};
    if (bytes[1] == '6') {  // PPM P6
        tok.i = 2;
        long long w = 0, h = 0, maxval = 0;
        if (!tok.next(w) || !tok.next(h) || !tok.next(maxval)) { out.why = "bad PPM header"; return out; }
        if (maxval != 255 || w <= 0 || h <= 0 ||
            static_cast<std::uint64_t>(w) * h > kMaxPackPixels) { out.why = "unsupported PPM header"; return out; }
        ++tok.i;  // the single header whitespace byte before the raster
        const std::size_t need = static_cast<std::size_t>(w) * h * 3;
        if (tok.i + need > bytes.size()) { out.why = "PPM raster is short"; return out; }
        out.width = static_cast<std::uint32_t>(w);
        out.height = static_cast<std::uint32_t>(h);
        out.rgba.resize(static_cast<std::size_t>(w) * h * 4);
        for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i) {
            out.rgba[i * 4 + 0] = bytes[tok.i + i * 3 + 0];
            out.rgba[i * 4 + 1] = bytes[tok.i + i * 3 + 1];
            out.rgba[i * 4 + 2] = bytes[tok.i + i * 3 + 2];
            out.rgba[i * 4 + 3] = 255;
        }
        out.ok = true;
        return out;
    }
    if (bytes[1] != '7') { out.why = "unsupported Netpbm magic"; return out; }
    // P7 PAM: header lines up to ENDHDR, then the raster.
    std::size_t line = 0;
    int depth = 0, maxval = 0;
    long long w = 0, h = 0;
    bool ended = false;
    std::string tuple;
    while (line < bytes.size()) {
        std::size_t eol = line;
        while (eol < bytes.size() && bytes[eol] != '\n') ++eol;
        const std::string text(reinterpret_cast<const char*>(bytes.data() + line),
                               eol - line + (eol < bytes.size() ? 1 : 0));
        auto value = [&text](const char* key, long long& v) {
            const std::size_t n = std::strlen(key);
            if (text.compare(0, n, key) != 0) return false;
            const char* p = text.c_str() + n;
            while (*p == ' ') ++p;
            char* end = nullptr;
            v = std::strtoll(p, &end, 10);
            return end != p;
        };
        long long v = 0;
        if (value("WIDTH", v)) w = v;
        else if (value("HEIGHT", v)) h = v;
        else if (value("DEPTH", v)) depth = static_cast<int>(v);
        else if (value("MAXVAL", v)) maxval = static_cast<int>(v);
        else if (text.compare(0, 8, "TUPLTYPE") == 0) tuple = text;
        if (text.compare(0, 6, "ENDHDR") == 0) { ended = true; line = eol + 1; break; }
        if (eol >= bytes.size()) break;
        line = eol + 1;
    }
    if (!ended) { out.why = "PAM has no ENDHDR"; return out; }
    if (maxval != 255 || w <= 0 || h <= 0 || depth < 1 || depth > 4 ||
        static_cast<std::uint64_t>(w) * h > kMaxPackPixels) {
        out.why = "unsupported PAM header";
        return out;
    }
    const std::size_t need = static_cast<std::size_t>(w) * h * static_cast<std::size_t>(depth);
    if (line + need > bytes.size()) { out.why = "PAM raster is short"; return out; }
    out.width = static_cast<std::uint32_t>(w);
    out.height = static_cast<std::uint32_t>(h);
    out.rgba.assign(static_cast<std::size_t>(w) * h * 4, 255);
    for (std::size_t i = 0; i < static_cast<std::size_t>(w) * h; ++i) {
        const std::uint8_t* s = bytes.data() + line + i * depth;
        std::uint8_t* d = out.rgba.data() + i * 4;
        if (depth == 1) { d[0] = d[1] = d[2] = s[0]; d[3] = 255; }
        else if (depth == 2) { d[0] = d[1] = d[2] = s[0]; d[3] = s[1]; }
        else if (depth == 3) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = 255; }
        else { std::memcpy(d, s, 4); }
    }
    out.ok = true;
    return out;
}

DecodedImage decodePackImage(std::span<const std::uint8_t> bytes) {
    if (bytes.size() >= 8 && bytes[0] == 0x89 && bytes[1] == 'P' && bytes[2] == 'N' && bytes[3] == 'G')
        return decodePng(bytes);
    return decodePnm(bytes);
}

}  // namespace rt::native::assets
