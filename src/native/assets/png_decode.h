#pragma once

// src/native/assets/png_decode.h
//
// Image decoding for HD texture packs: PNG (the format upscalers emit) and the
// Netpbm formats our own dumps write (PAM P7 / PPM P6), all to RGBA8 with
// alpha kept as the file stores it. Pure C++ (no zlib, no libpng): PNG needs
// inflate, so the decoder carries its own (assets/png_decode.cpp) — the
// sibling recomp links an encoder for the same reason: no third-party image
// dependency ships with the app.
//
// Accepted PNG: 8-bit depth, colour types 0/2/3/4/6, non-interlaced, with
// PLTE + tRNS honoured for paletted files. Refused (ok=false, `why` says so):
// 16-bit, interlaced, unknown colour type, truncated/corrupt streams, a
// palette index outside PLTE. Packs are authored images; refusing beats
// guessing at a pack maker's file.

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace rt::native::assets {

struct DecodedImage {
    bool ok = false;
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;  // width*height*4, R at byte 0
    std::string why;                 // empty iff ok
};

// Picks the decoder by file signature (PNG) or magic ("P6"/"P7"): what a pack
// scan hands over, whatever the extension claimed.
DecodedImage decodePackImage(std::span<const std::uint8_t> bytes);

// Format-specific entry points (each refuses anything outside its spec).
DecodedImage decodePng(std::span<const std::uint8_t> bytes);
DecodedImage decodePnm(std::span<const std::uint8_t> bytes);

// Raw DEFLATE/zlib inflate, exposed for its own test. `bytes` may start with a
// zlib header (0x78 ..) or be bare deflate; the Adler-32 is checked when a
// zlib header is present. `maxOut` bounds the output: a stream that expands
// past it is refused before the memory is committed (a pack file is untrusted
// input), and PNG decoding passes the raster size it expects exactly.
bool inflateZlib(std::span<const std::uint8_t> bytes, std::vector<std::uint8_t>& out,
                 std::string& why, std::size_t maxOut = 1u << 26);

// Largest accepted pack image (16.7M pixels = 4096x4096 RGBA8 at 64 MB): far
// past any texture the game has, and small enough that a corrupt or hostile
// file cannot ask for gigabytes.
inline constexpr std::uint64_t kMaxPackPixels = 1ull << 24;

// Largest pack file read (72 MB): a 4096x4096 RGBA8 raster is 64 MB and its
// compressed form cannot exceed the bound decodePng applies, so nothing a pack
// may legally contain is refused - a bigger file is not a pack image.
inline constexpr std::size_t kMaxPackFileBytes = std::size_t(72) << 20;

}  // namespace rt::native::assets
