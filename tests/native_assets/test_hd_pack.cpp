// tests/native_assets/test_hd_pack.cpp
//
// Synthetic contract tests for the HD-texture-pack LOGIC module
// (src/native/assets/hd_pack.{h,cpp}). Every input is hand-built here — no
// game bytes, no disc reads, no GPU. Deterministic end to end: the pack-scan
// test writes its fixtures into a temp folder it owns (grammar is exercised
// against the real directory walker; the walk order is deterministic because
// scanPackFolder sorts file names before indexing).
//
// Covered contracts (mirroring hd_pack.h docs):
//   - StableContentKey stability: same decoded texels + dims => same key
//     across runs and across disc locations; one texel differs => different
//     key; palette-swap content (hash AFTER palette application, exactly the
//     texels image_converter emits) => different key; dims differ => different
//     key; rejected shapes => null key.
//   - Pack file-name grammar: valid primary/fallback names parse, malformed
//     names are refused, key string round-trips.
//   - PackIndex collision rule: last index wins, with a surfaced warning.
//   - ReplacementRequest: Miss/Replace/StaleMismatch; any pack image size
//     (non-power-of-two, any scale, larger or smaller than the native
//     texture) is a valid replacement; mipLevelCount math.
//   - Dump mode: PPM/PAM encoders byte-exact, dump names mirror pack names,
//     a written dump folder REFILLS an index and serves a request (the
//     "dump folder is directly pack-usable" statement).
//   - Mip chain: level count and sizes, last level 1x1, clamped-box spot
//     checks (2x2->1x1 average, half rounds up, odd-edge duplication),
//     straight-alpha passthrough, rejection of malformed input.
//
// Failure accounting is plain counters and if-statements (no assert()), so
// behaviour is NDEBUG-independent; any failed check exits nonzero.

#include "../../src/native/assets/container.h"
#include "../../src/native/assets/hd_pack.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <vector>

using rt::native::assets::DumpFormat;
using rt::native::assets::DumpPlan;
using rt::native::assets::DumpWriteError;
using rt::native::assets::ImageRecord;
using rt::native::assets::MipChain;
using rt::native::assets::PackIndex;
using rt::native::assets::PackScanError;
using rt::native::assets::ParsedPackFileName;
using rt::native::assets::PixelFormat;
using rt::native::assets::ReplacementRequest;
using rt::native::assets::ReplacementVerdict;
using rt::native::assets::StableContentKey;
using rt::native::assets::dumpFileName;
using rt::native::assets::encodePamRgba;
using rt::native::assets::encodePpmRgb;
using rt::native::assets::indexPackFileName;
using rt::native::assets::mipLevelCount;
using rt::native::assets::parsePackFileName;
using rt::native::assets::parseStableKey;
using rt::native::assets::planDump;
using rt::native::assets::requestReplacement;
using rt::native::assets::scanPackFolder;
using rt::native::assets::stableContentKey;
using rt::native::assets::stableKeyToString;
using rt::native::assets::buildMipChain;
using rt::native::assets::writeDumpFile;

namespace {

int g_checks = 0;
int g_failures = 0;

void check(bool cond, const std::string& what) {
    ++g_checks;
    if (!cond) {
        ++g_failures;
        std::cerr << "FAIL: " << what << '\n';
    }
}

using Bytes = std::vector<std::uint8_t>;

// Fills a w*h*4 RGBA buffer deterministically: a gradient the tests can
// perturb single bytes in.
Bytes gradientTexels(std::uint32_t w, std::uint32_t h) {
    Bytes rgba(static_cast<std::size_t>(w) * h * 4, 0);
    for (std::uint32_t y = 0; y < h; ++y) {
        for (std::uint32_t x = 0; x < w; ++x) {
            const std::size_t at = (static_cast<std::size_t>(y) * w + x) * 4;
            rgba[at + 0] = static_cast<std::uint8_t>(x * 3);
            rgba[at + 1] = static_cast<std::uint8_t>(y * 5);
            rgba[at + 2] = static_cast<std::uint8_t>((x + y) * 2);
            rgba[at + 3] = 0xFF;
        }
    }
    return rgba;
}

// The palette-application step of the native decode, expressed directly:
// indices (one byte per texel) expanded through a 256-entry RGBA palette,
// the byte layout image_converter already emits. Used to encode the
// "hash AFTER palette application" contract without any image_converter
// dependency here.
Bytes applyPalette(const Bytes& indices, const Bytes& palette) {
    Bytes out(indices.size() * 4, 0);
    for (std::size_t i = 0; i < indices.size(); ++i) {
        const std::size_t p = static_cast<std::size_t>(indices[i]) * 4;
        out[i * 4 + 0] = palette[p + 0];
        out[i * 4 + 1] = palette[p + 1];
        out[i * 4 + 2] = palette[p + 2];
        out[i * 4 + 3] = palette[p + 3];
    }
    return out;
}

ImageRecord recordOf(std::uint32_t w, std::uint32_t h, const Bytes& rgba,
                     const std::string& sourceFile, std::uint64_t offset,
                     const std::string& variant) {
    ImageRecord r;
    r.width = w;
    r.height = h;
    r.mipCount = 1;
    r.format = PixelFormat::R8G8B8A8_UNORM;
    r.pixels = rgba;
    r.provenance.sourceFile = sourceFile;
    r.provenance.offset = offset;
    r.provenance.length = rgba.size();
    r.sourceVariantId = variant;
    return r;
}

ParsedPackFileName parseOk(const std::string& name) {
    const ParsedPackFileName parsed = parsePackFileName(name);
    check(parsed.ok, "parse ok: " + name);
    return parsed;
}

void parseFails(const std::string& name) {
    const ParsedPackFileName parsed = parsePackFileName(name);
    check(!parsed.ok, "parse refuses: " + name);
}

// ---------------------------------------------------------------------------
// StableContentKey
// ---------------------------------------------------------------------------

void testKeyStability() {
    const Bytes texels = gradientTexels(4, 4);

    const StableContentKey a = stableContentKey(4, 4, texels);
    const StableContentKey b = stableContentKey(4, 4, texels);
    check(!a.isNull(), "key: gradient texels key non-null");
    check(a == b, "key: same texels + dims => same key across calls");

    Bytes copy = texels;
    const StableContentKey same = stableContentKey(4, 4, copy);
    check(same == a, "key: copied buffer hashes identically");

    copy[7] ^= 0x01;  // one texel's green channel differs
    const StableContentKey differs = stableContentKey(4, 4, copy);
    check(differs != a, "key: one differing texel => different key");

    // Dims are part of the feed: same bytes reshaped are different content.
    const StableContentKey otherDims = stableContentKey(16, 1, texels);
    check(otherDims != a, "key: same bytes at different dims differ");
    const StableContentKey wide = stableContentKey(1, 16, texels);
    check(wide != otherDims, "key: 16x1 differs from 1x16");

    // Location independence: provenance and variant ids are NOT in the feed.
    const ImageRecord fromQ00 = recordOf(4, 4, texels, "CAR0/Q00.BIN", 0x219a0, "livery");
    const ImageRecord fromQ99 = recordOf(4, 4, texels, "CAR3/Q99.BIN", 0x33fd0, "livery");
    check(stableContentKey(fromQ00) == a, "key: record path keys like raw texels");
    check(stableContentKey(fromQ00) == stableContentKey(fromQ99),
          "key: same content from different disc locations => same key");

    // Rejected shapes never key.
    check(stableContentKey(0, 4, texels).isNull(), "key: zero width => null");
    check(stableContentKey(4, 0, texels).isNull(), "key: zero height => null");
    check(stableContentKey(4, 4, Bytes(texels.size() - 1)).isNull(), "key: short span => null");
    check(stableContentKey(4, 4, Bytes(texels.size() + 1)).isNull(), "key: long span => null");
    check(stableContentKey(recordOf(0, 0, Bytes{}, "broken", 0, "")).isNull(),
          "key: rejected decode record => null");
}

void testKeyPaletteSwap() {
    // Same index texel pattern under two palettes: the native decode applies
    // the palette, then the key sees the RESULT — so a palette swap (fades,
    // flashes, alternate CSA banks) is a different texture, exactly matching
    // image_converter's applied output.
    Bytes indices(16 * 16);
    for (std::size_t i = 0; i < indices.size(); ++i) {
        indices[i] = static_cast<std::uint8_t>(i & 0xFFu);
    }
    Bytes paletteA(256 * 4), paletteB;
    for (int i = 0; i < 256; ++i) {
        paletteA[static_cast<std::size_t>(i) * 4 + 0] = static_cast<std::uint8_t>(i);
        paletteA[static_cast<std::size_t>(i) * 4 + 1] = static_cast<std::uint8_t>(255 - i);
        paletteA[static_cast<std::size_t>(i) * 4 + 2] = 0x40;
        paletteA[static_cast<std::size_t>(i) * 4 + 3] = 0xFF;
    }
    paletteB = paletteA;
    paletteB[7 * 4 + 0] = 0xE0;   // one palette entry repaints 16 texels here
    paletteB[7 * 4 + 1] = 0x11;

    const Bytes decodedA = applyPalette(indices, paletteA);
    const Bytes decodedB = applyPalette(indices, paletteB);
    check(decodedA != decodedB, "palette swap: decoded texels differ");

    const StableContentKey keyA = stableContentKey(16, 16, decodedA);
    const StableContentKey keyB = stableContentKey(16, 16, decodedB);
    check(keyA != keyB, "key: palette-swapped content => different key");
    check(keyA == stableContentKey(16, 16, decodedA), "key: re-apply of same palette stable");
}

void testKeyStringRoundTrip() {
    check(stableKeyToString(StableContentKey{0x0123456789abcdefull}) == "0123456789abcdef",
          "key string: canonical lowercase hex");
    for (std::uint64_t v : {0xFull, 0xFEDCBA9876543210ull, 0xFFFFFFFFFFFFFFF0ull}) {
        const std::string s = stableKeyToString(StableContentKey{v});
        check(s.size() == 16, "key string: 16 digits");
        const auto parsed = parseStableKey(s);
        check(parsed.has_value() && parsed->value == v, "key string: round trip " + s);
    }
    check(!parseStableKey("0123456789abcde").has_value(), "key parse: 15 digits rejected");
    check(!parseStableKey("0123456789abcdef0").has_value(), "key parse: 17 digits rejected");
    check(!parseStableKey("0123456789abcdeg").has_value(), "key parse: non-hex rejected");
    check(!parseStableKey("0000000000000000").has_value(),
          "key parse: null key is not nameable");
    check(parseStableKey("FFFFFFFFFFFFFFFF").has_value(), "key parse: uppercase accepted");
}

// ---------------------------------------------------------------------------
// Pack file-name grammar
// ---------------------------------------------------------------------------

void testParsePackFileNameValid() {
    const ParsedPackFileName primary = parseOk("0123456789abcdef_128x64.png");
    check(primary.key == StableContentKey{0x0123456789abcdefull},
          "parse: key equals the hex digits");
    check(!primary.bare && primary.width == 128 && primary.height == 64,
          "parse: primary dims");
    check(primary.extension == "png", "parse: extension");
    check(!primary.sourceDimsNamed, "parse: no src token by default");

    const ParsedPackFileName upper = parseOk("0123456789ABCDEF_128X64.PAM");
    check(upper.key == primary.key, "parse: hex case-insensitive");
    check(upper.extension == "pam", "parse: extension lowercased");
    check(upper.width == 128 && upper.height == 64, "parse: 'X' tolerated");

    const ParsedPackFileName tagged = parseOk("0123456789abcdef_128x64_psm13.png");
    check(tagged.width == 128 && tagged.height == 64, "parse: suffix token tolerated");
    check(!tagged.sourceDimsNamed, "parse: psm token is not a src token");

    const ParsedPackFileName scaleTag = parseOk("0123456789abcdef_512x512_4x.png");
    check(scaleTag.width == 512 && scaleTag.height == 512, "parse: upscaler tag tolerated");

    const ParsedPackFileName src = parseOk("fedcba9876543210_64x64_src128x128.ppm");
    check(src.sourceDimsNamed && src.sourceWidth == 128 && src.sourceHeight == 128,
          "parse: src token records native dims");
    check(src.width == 64 && src.height == 64, "parse: src keeps pack dims separate");

    const ParsedPackFileName srcUpper = parseOk("fedcba9876543210_64x64_SRC64X64.PNG");
    check(srcUpper.sourceDimsNamed && srcUpper.sourceWidth == 64 && srcUpper.sourceHeight == 64,
          "parse: SRC token case-insensitive");

    const ParsedPackFileName bare = parseOk("0123456789abcdef.png");
    check(bare.bare && bare.width == 0 && bare.height == 0,
          "parse: bare fallback naming accepted, dims unknown");
    check(bare.extension == "png", "parse: bare extension");
}

void testParsePackFileNameInvalid() {
    parseFails("");                       // empty
    parseFails("123456789abcdef_64x64.png");    // 15 hex digits
    parseFails("0123456789abcdefg_64x64.png");  // non-hex at position 16
    parseFails("128x64_0123456789abcdef.png");  // key is not first
    parseFails("0123456789abcdef_64x64");       // no extension
    parseFails("0123456789abcdef");             // bare but extensionless
    parseFails("0123456789abcdef_.png");       // empty dims token
    parseFails("0123456789abcdef_0x64.png");   // zero width refused
    parseFails("0123456789abcdef_64x0.png");   // zero height refused
    parseFails("0123456789abcdef_64.png");     // missing xH
    parseFails("0123456789abcdef_x64.png");    // missing W
    parseFails("0123456789abcdef_64x_128.png");// trailing empty token
    parseFails("0123456789abcdef_64x64..png"); // empty extension chars
    parseFails("0123456789abcdef_64x64.png.bak");  // embedded dot in tail
    parseFails("0123456789abcdef_64x6 4.png");  // whitespace
    parseFails(" 0123456789abcdef_64x64.png");  // leading space (not hex16)
    parseFails("0123456789abcdef_99999999x64.png");  // dim over 0xFFFFFF
    parseFails("0123456789abcdef_64x64_src.png");     // src without dims
    parseFails("0123456789abcdef_64x64_src0x0.png");  // src with zero dims
    parseFails("0123456789abcdef_64x64__4x.png");     // empty token
    parseFails("0123456789abcdef_64x64.tar.gz");      // non-alnum in extension
    const std::string longExt = "0123456789abcdef_64x64.png" + std::string(16, 'a');
    parseFails(longExt);  // extension longer than 15 chars
}

// ---------------------------------------------------------------------------
// PackIndex indexing + collision rule
// ---------------------------------------------------------------------------

void testIndexAndCollision() {
    PackIndex index;
    const std::string keyHex = "0123456789abcdef";
    const std::filesystem::path dir = "/packs/demo";

    check(indexPackFileName(index, keyHex + "_64x64.png", dir / (keyHex + "_64x64.png")),
          "index: first file accepted");
    check(index.size() == 1, "index: one entry");

    check(indexPackFileName(index, keyHex + "_128x128_4x.png",
                            dir / (keyHex + "_128x128_4x.png")),
          "index: second file of same key accepted (collision)");
    check(index.size() == 1, "index: collision does not grow the entry count");
    check(index.warnings.collisions == 1, "index: collision surfaced in warnings");

    const StableContentKey key = *parseStableKey(keyHex);
    const auto* entry = index.find(key);
    check(entry != nullptr, "index: entry findable by key");
    if (entry != nullptr) {
        check(entry->width == 128 && entry->height == 128, "index: last file wins");
        check(entry->hadCollision, "index: winner marked as collision");
        check(entry->fileName == keyHex + "_128x128_4x.png", "index: winner name kept");
    }

    // A different key adds, keeping the vector sorted (find is a binary search).
    check(indexPackFileName(index, "fedcba9876543210_8x8.png", dir / "fedcba9876543210_8x8.png"),
          "index: second key accepted");
    check(index.size() == 2, "index: two distinct keys");
    bool sorted = true;
    for (std::size_t i = 1; i < index.entries.size(); ++i) {
        if (!(index.entries[i - 1].key.value < index.entries[i].key.value)) sorted = false;
    }
    check(sorted, "index: entries stay key-sorted");

    // Scan-policy counters (no entries created for refusal reasons).
    const std::size_t entriesBefore = index.size();
    check(!indexPackFileName(index, "readme.txt", dir / "readme.txt"),
          "index: foreign file refused");
    check(index.warnings.ignoredForeign == 1, "index: foreign file counted");
    check(!indexPackFileName(index, keyHex + "_64x.png", dir / (keyHex + "_64x.png")),
          "index: malformed hex-prefixed name refused");
    check(index.warnings.skippedUnparseable == 1, "index: unparseable counted");
    check(!indexPackFileName(index, keyHex + "_64x64.txt", dir / (keyHex + "_64x64.txt")),
          "index: non-image extension refused");
    check(index.warnings.skippedOtherExtension == 1, "index: other-extension counted");
    check(index.size() == entriesBefore, "index: refusals add no entries");
}

// ---------------------------------------------------------------------------
// ReplacementRequest
// ---------------------------------------------------------------------------

void testReplacementVerdicts() {
    const StableContentKey key = *parseStableKey("0123456789abcdef");
    const std::filesystem::path dir = "/packs/demo";

    PackIndex index;
    check(indexPackFileName(index, "0123456789abcdef_512x512.png", dir / "0123456789abcdef_512x512.png"),
          "request setup: 4x entry");
    check(indexPackFileName(index, "0123456789abcde1_100x75.png", dir / "0123456789abcde1_100x75.png"),
          "request setup: non-pot entry");
    check(indexPackFileName(index, "0123456789abcde2_127x63.png", dir / "0123456789abcde2_127x63.png"),
          "request setup: smaller-than-native entry");
    check(indexPackFileName(index, "0123456789abcde3_129x4097.png", dir / "0123456789abcde3_129x4097.png"),
          "request setup: odd-aspect entry");
    check(indexPackFileName(index, "0123456789abcde4_128x128_src256x256.png",
                            dir / "0123456789abcde4_128x128_src256x256.png"),
          "request setup: stale src entry");
    check(indexPackFileName(index, "0123456789abcde5_128x128_src128x128.png",
                            dir / "0123456789abcde5_128x128_src128x128.png"),
          "request setup: matching src entry");

    // Miss: no entry for the key.
    const ReplacementRequest miss =
        requestReplacement(index, *parseStableKey("ffffffffffffffff"), 128, 128);
    check(miss.verdict == ReplacementVerdict::Miss, "request: unknown key => Miss");
    check(miss.path.empty() && miss.mipLevelCount == 0, "request: Miss carries no contract");

    // Miss on null key (failed decode must never bind a pack entry).
    check(requestReplacement(index, StableContentKey{}, 128, 128).verdict ==
                  ReplacementVerdict::Miss,
          "request: null key => Miss");

    // 4x PoT replacement.
    const ReplacementRequest fourX = requestReplacement(index, key, 128, 128);
    check(fourX.verdict == ReplacementVerdict::Replace, "request: 4x entry replaces");
    check(fourX.width == 512 && fourX.height == 512, "request: pack dims carried");
    check(fourX.mipLevelCount == 10, "request: 512 mip count floor(log2)+1 = 10");
    check(fourX.nativeWidth == 128 && fourX.nativeHeight == 128,
          "request: native dims echoed (address by original size)");
    check(fourX.policy.maxAnisotropy == 16, "request: aniso default 16");
    check(fourX.policy.clampToEdgeOnClampedAxes && fourX.policy.keepPackAlpha,
          "request: clamp/alpha policies defaulted");

    // Non-power-of-two, any scale: 100x75 replacing 128x128.
    const ReplacementRequest nonPot =
        requestReplacement(index, *parseStableKey("0123456789abcde1"), 128, 128);
    check(nonPot.verdict == ReplacementVerdict::Replace, "request: non-pot entry replaces");
    check(nonPot.mipLevelCount == 7, "request: 100x75 mip count 7");
    check(nonPot.width == 100 && nonPot.height == 75, "request: non-pot dims carried");

    // Smaller than native and odd aspect both fine ("at any size").
    const ReplacementRequest smaller =
        requestReplacement(index, *parseStableKey("0123456789abcde2"), 128, 128);
    check(smaller.verdict == ReplacementVerdict::Replace && smaller.mipLevelCount == 7,
          "request: smaller-than-native entry replaces (127x63)");
    const ReplacementRequest odd =
        requestReplacement(index, *parseStableKey("0123456789abcde3"), 128, 128);
    check(odd.verdict == ReplacementVerdict::Replace && odd.mipLevelCount == 13,
          "request: 129x4097 replaces (mip 13)");

    // Stale mismatch: src token disagrees with the live native dims.
    const ReplacementRequest stale =
        requestReplacement(index, *parseStableKey("0123456789abcde4"), 128, 128);
    check(stale.verdict == ReplacementVerdict::StaleMismatch,
          "request: mismatched src dims => StaleMismatch");
    check(stale.path.empty() && stale.mipLevelCount == 0,
          "request: StaleMismatch carries no contract");
    check(stale.note.find("256x256") != std::string::npos &&
                  stale.note.find("128x128") != std::string::npos,
          "request: stale note names both dim pairs");

    // Matching src token -> Replace.
    const ReplacementRequest matching =
        requestReplacement(index, *parseStableKey("0123456789abcde5"), 128, 128);
    check(matching.verdict == ReplacementVerdict::Replace,
          "request: matching src dims => Replace");

    // Bare fallback entry: dims unknown until load.
    PackIndex bareIndex;
    check(indexPackFileName(bareIndex, "0123456789abcdef.png", dir / "0123456789abcdef.png"),
          "request setup: bare entry");
    const ReplacementRequest bare = requestReplacement(bareIndex, key, 128, 128);
    check(bare.verdict == ReplacementVerdict::Replace, "request: bare entry replaces");
    check(bare.width == 0 && bare.height == 0 && bare.mipLevelCount == 0,
          "request: bare entry defers dims to load time");
    check(!bare.note.empty(), "request: bare entry carries a load-time note");

    // Failed scan: never a replacement, and never a pixel change.
    PackIndex broken;
    broken.err = PackScanError::NotADirectory;
    broken.errorPath = "/nope";
    check(requestReplacement(broken, key, 128, 128).verdict == ReplacementVerdict::Miss,
          "request: broken index => Miss");
}

// ---------------------------------------------------------------------------
// Mip chain
// ---------------------------------------------------------------------------

void testMipLevelCount() {
    check(mipLevelCount(1, 1) == 1, "mip count: 1x1 -> 1");
    check(mipLevelCount(2, 2) == 2, "mip count: 2x2 -> 2");
    check(mipLevelCount(4, 4) == 3, "mip count: 4x4 -> 3");
    check(mipLevelCount(8, 4) == 4, "mip count: 8x4 -> 4");
    check(mipLevelCount(1, 128) == 8, "mip count: 1x128 -> 8");
    check(mipLevelCount(128, 128) == 8, "mip count: 128x128 -> 8");
    check(mipLevelCount(512, 512) == 10, "mip count: 512x512 -> 10");
    check(mipLevelCount(100, 75) == 7, "mip count: 100x75 (non-pot) -> 7");
    check(mipLevelCount(0, 64) == 0, "mip count: unknown dims -> 0");
    check(mipLevelCount(64, 0) == 0, "mip count: unknown dims -> 0 (h)");
    check(mipLevelCount(3, 1) == 2, "mip count: 3x1 -> 2");
}

void testBuildMipChain() {
    // 2x2 -> 1x1 box average: R = (10+20+30+40)/4 = 25; alpha stays opaque.
    std::vector<std::uint8_t> quad{
        10, 0, 0, 255,  20, 0, 0, 255,
        30, 0, 0, 255,  40, 0, 0, 255,
    };
    const MipChain quadChain = buildMipChain(2, 2, quad);
    check(quadChain.valid() && quadChain.levelCount() == 2, "chain: 2x2 has 2 levels");
    check(quadChain.levels[1].width == 1 && quadChain.levels[1].height == 1,
          "chain: 2x2 last level 1x1");
    check(quadChain.levels[1].rgba[0] == 25, "chain: box spot-check R = 25");
    check(quadChain.levels[1].rgba[3] == 255, "chain: alpha averaged, stays opaque");
    check(quadChain.levels[0].rgba == quad, "chain: level 0 is the input copy");

    // Half rounds up: (0+0+0+255)/4 = 63.75 -> 64.
    const std::vector<std::uint8_t> odd255{
        0, 0, 0, 255,  0, 0, 0, 255,
        0, 0, 0, 255,  255, 0, 0, 255,
    };
    const MipChain roundChain = buildMipChain(2, 2, odd255);
    check(roundChain.levels[1].rgba[0] == 64, "chain: box average rounds half up");

    // Odd edge clamps (3x1 -> 1x1): taps duplicate the row's edge, so the
    // average of {10,20,30} saturates to the (10,20) window => 15.
    const std::vector<std::uint8_t> row{
        10, 0, 0, 255,  20, 0, 0, 255,  30, 0, 0, 255,
    };
    const MipChain rowChain = buildMipChain(3, 1, row);
    check(rowChain.levelCount() == 2 && rowChain.levels[1].rgba[0] == 15,
          "chain: odd column edge-clamped to 15");

    // Sizes: 8x4 -> (8,4),(4,2),(2,1),(1,1); 5x3 -> (5,3),(2,1),(1,1).
    const Bytes big = gradientTexels(8, 4);
    const MipChain bigChain = buildMipChain(8, 4, big);
    check(bigChain.levelCount() == mipLevelCount(8, 4), "chain: 8x4 count matches helper");
    const std::pair<std::uint32_t, std::uint32_t> expected8x4[] = {
        {8, 4}, {4, 2}, {2, 1}, {1, 1}};
    bool dims8x4 = bigChain.levelCount() == 4;
    for (std::size_t i = 0; i < 4 && i < bigChain.levelCount(); ++i) {
        if (bigChain.levels[i].width != expected8x4[i].first ||
            bigChain.levels[i].height != expected8x4[i].second) {
            dims8x4 = false;
        }
    }
    check(dims8x4, "chain: 8x4 level dims floor-halve, last level 1x1");
    bool sizes8x4 = true;
    for (const auto& level : bigChain.levels) {
        if (level.rgba.size() != static_cast<std::size_t>(level.width) * level.height * 4) {
            sizes8x4 = false;
        }
    }
    check(sizes8x4, "chain: every level is exactly W*H*4 bytes");

    const Bytes nonPot = gradientTexels(5, 3);
    const MipChain nonPotChain = buildMipChain(5, 3, nonPot);
    check(nonPotChain.levelCount() == 3, "chain: 5x3 non-pot count 3");
    check(nonPotChain.levels[2].width == 1 && nonPotChain.levels[2].height == 1,
          "chain: non-pot ends at 1x1");

    // 1x1 base: single level, byte-identical.
    const std::vector<std::uint8_t> one{7, 8, 9, 255};
    const MipChain oneChain = buildMipChain(1, 1, one);
    check(oneChain.levelCount() == 1 && oneChain.levels[0].rgba == one,
          "chain: 1x1 is a single level");

    // Rejections.
    check(!buildMipChain(0, 4, Bytes{1}).valid(), "chain: zero w rejected");
    check(!buildMipChain(4, 0, Bytes{1}).valid(), "chain: zero h rejected");
    check(!buildMipChain(2, 2, Bytes(quad.size() - 1)).valid(),
          "chain: short span rejected");
    check(!buildMipChain(2, 2, Bytes(quad.size() + 1)).valid(),
          "chain: long span rejected");
}

// ---------------------------------------------------------------------------
// Dump mode
// ---------------------------------------------------------------------------

void testDumpEncoders() {
    const Bytes texels = gradientTexels(2, 2);

    const Bytes ppm = encodePpmRgb(2, 2, texels);
    const std::string ppmHeader = "P6\n2 2\n255\n";
    check(ppm.size() == ppmHeader.size() + 12, "ppm: header + 2*2*3 bytes");
    check(ppm.size() >= ppmHeader.size() &&
                  std::equal(ppmHeader.begin(), ppmHeader.end(), ppm.begin()),
          "ppm: snapshot-writer P6 header");
    // Alpha is dropped by the P6 format: the last stored byte is the final
    // texel's blue channel, not its alpha.
    check(ppm[ppm.size() - 1] == texels[2 * 2 * 4 - 2],
          "ppm: last byte is the last texel's B (alpha dropped)");
    bool rgbOnly = true;
    for (std::size_t i = ppmHeader.size(); i + 2 < ppm.size(); i += 3) {
        if (ppm[i] != texels[(i - ppmHeader.size()) / 3 * 4] ||
            ppm[i + 1] != texels[(i - ppmHeader.size()) / 3 * 4 + 1] ||
            ppm[i + 2] != texels[(i - ppmHeader.size()) / 3 * 4 + 2]) {
            rgbOnly = false;
        }
    }
    check(rgbOnly, "ppm: RGB triplets in row-major source order");

    const Bytes pam = encodePamRgba(2, 2, texels);
    const std::string pamHeader =
        "P7\nWIDTH 2\nHEIGHT 2\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n";
    check(pam.size() == pamHeader.size() + 16, "pam: header + raw RGBA kept");
    check(std::equal(pamHeader.begin(), pamHeader.end(), pam.begin()),
          "pam: P7 RGB_ALPHA header");
    check(std::equal(texels.begin(), texels.end(), pam.begin() + pamHeader.size()),
          "pam: alpha channel kept verbatim");

    check(encodePpmRgb(0, 2, texels).empty(), "ppm: zero dim rejected");
    check(encodePamRgba(2, 0, texels).empty(), "pam: zero dim rejected");
    check(encodePpmRgb(2, 2, Bytes(texels.size() - 1)).empty(), "ppm: span mismatch rejected");
    check(encodePamRgba(2, 2, Bytes(texels.size() + 4)).empty(), "pam: span mismatch rejected");
}

void testDumpNameRoundTrip() {
    const Bytes texels = gradientTexels(128, 128);
    const StableContentKey key = stableContentKey(128, 128, texels);
    check(!key.isNull(), "dump round trip: fixture keyed");

    const std::string name = dumpFileName(key, 128, 128, DumpFormat::PamRgba);
    check(name == stableKeyToString(key) + "_128x128.pam", "dump name: key_dims.ext");
    check(name.find(stableKeyToString(key)) == 0, "dump name: starts with the key");

    // Mirrors the pack grammar exactly (a dump is a trivial 1x pack image).
    const ParsedPackFileName parsed = parseOk(name);
    check(parsed.key == key, "dump name: key round-trips");
    check(parsed.width == 128 && parsed.height == 128 && !parsed.bare,
          "dump name: native dims are the pack-image dims");
    check(parsed.extension == "pam", "dump name: pam extension");

    const ParsedPackFileName parsedPpm = parseOk(dumpFileName(key, 128, 128, DumpFormat::PpmRgb));
    check(parsedPpm.extension == "ppm", "dump name: ppm extension");
    check(parsedPpm.key == key, "dump name: format does not change the key");

    // ... and it fills an index like any pack file.
    PackIndex index;
    check(indexPackFileName(index, name, std::filesystem::path("/dumps") / name),
          "dump name: indexed as a pack entry");
    check(index.contains(key), "dump name: index finds the dump key");

    const ReplacementRequest request = requestReplacement(index, key, 128, 128);
    check(request.verdict == ReplacementVerdict::Replace, "dump name: serves a request");
    check(request.mipLevelCount == 8, "dump name: 128x128 request sizes a full chain");
}

void testPlanAndWrite() {
    const Bytes texels = gradientTexels(4, 4);
    const StableContentKey key = stableContentKey(4, 4, texels);

    const DumpPlan plan = planDump("/dumps", key, 4, 4, DumpFormat::PamRgba);
    check(plan.fileName == dumpFileName(key, 4, 4, DumpFormat::PamRgba),
          "plan: names the dump file");
    check(plan.path == std::filesystem::path("/dumps") / plan.fileName,
          "plan: path is dir / name");

    // Error paths that never touch the disk.
    check(writeDumpFile("/dumps", key, 4, 4, Bytes(texels.size() - 1), DumpFormat::PamRgba) ==
                  DumpWriteError::BadSize,
          "write: span mismatch refused");
    check(writeDumpFile("/dumps", key, 0, 4, texels, DumpFormat::PpmRgb) ==
                  DumpWriteError::BadSize,
          "write: zero dim refused");
    check(writeDumpFile("/dumps", StableContentKey{}, 4, 4, texels, DumpFormat::PamRgba) ==
                  DumpWriteError::BadSize,
          "write: null key refused");
}

// ---------------------------------------------------------------------------
// Dump folder usability: a written dump folder serves as a pack
// ---------------------------------------------------------------------------

void testDumpFolderUsability() {
    const std::filesystem::path root =
        std::filesystem::temp_directory_path() / "rt_native_hd_pack_test";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);
    const std::filesystem::path dir = root / "dumps";

    const Bytes texels = gradientTexels(16, 16);
    const StableContentKey dumpKey = stableContentKey(16, 16, texels);
    check(writeDumpFile(dir, dumpKey, 16, 16, texels, DumpFormat::PamRgba) == DumpWriteError::Ok,
          "folder: dump written");
    check(writeDumpFile(dir, dumpKey, 16, 16, texels, DumpFormat::PpmRgb) ==
                  DumpWriteError::Ok,
          "folder: ppm twin written (same key, second format)");

    // The written bytes are exactly the encoder's (the file is decodable PAM).
    const DumpPlan plan = planDump(dir, dumpKey, 16, 16, DumpFormat::PamRgba);
    std::ifstream in(plan.path, std::ios::binary);
    check(in.good(), "folder: dump file readable");
    const Bytes onDisk((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    check(onDisk == encodePamRgba(16, 16, texels), "folder: PAM bytes deterministic");

    // Folder fixtures for the scan-policy counters and the collision rule.
    const std::string dumpHex = stableKeyToString(dumpKey);
    auto writeFile = [](const std::filesystem::path& p, const std::string& content) {
        std::ofstream f(p, std::ios::binary | std::ios::trunc);
        f << content;
    };
    writeFile(dir / "readme.txt", "not a texture");
    writeFile(dir / (dumpHex + "_16x16.png"), "fake png");  // 3rd file of dumpKey
    writeFile(dir / (dumpHex + "_16x1x.png"), "bad name");  // hex-prefixed, malformed
    writeFile(dir / (dumpHex + "_16x16.txt"), "wrong extension");

    const PackIndex index = scanPackFolder(dir);
    check(index.err == PackScanError::Ok, "folder: scan ok");
    check(index.warnings.collisions == 2, "folder: 3 files of one key => 2 collisions");
    check(index.warnings.skippedUnparseable == 1, "folder: malformed name counted");
    check(index.warnings.skippedOtherExtension == 1, "folder: other extension counted");
    check(index.warnings.ignoredForeign == 1, "folder: readme counted as foreign");
    check(index.size() == 1, "folder: one key indexed");

    const auto* entry = index.find(dumpKey);
    check(entry != nullptr, "folder: dump key found");
    if (entry != nullptr) {
        // Lexicographic last-wins: "_16x1x.png" (malformed) never indexed;
        // of the three valid
        // names the extensions sort "pam" < "png" < "ppm", so the ppm twin
        // wins. Deterministic on every platform by the sorted walk.
        check(entry->fileName == dumpHex + "_16x16.ppm",
              "folder: lexicographically last file wins");
        check(entry->hadCollision, "folder: winner flagged");
        check(entry->width == 16 && entry->height == 16, "folder: dims indexed");
    }

    // THE usability statement: the dump folder, untouched, serves requests.
    const ReplacementRequest request = requestReplacement(index, dumpKey, 16, 16);
    check(request.verdict == ReplacementVerdict::Replace,
          "folder: dump folder serves a replacement as-is");
    check(request.path == dir / (dumpHex + "_16x16.ppm"),
          "folder: request names the indexed winning file");
    const ReplacementRequest same = requestReplacement(index, dumpKey, 16, 16);
    check(request.path == same.path, "folder: requests are stable across calls");
    check(request.mipLevelCount == 5, "folder: 16x16 chain sized (mip 5)");
    check(!request.note.empty() && request.note.find("collision") != std::string::npos,
          "folder: collision surfaced in the request note");

    // Indexes stay snapshots: a late file needs a re-scan.
    writeFile(dir / (stableKeyToString(stableContentKey(2, 2, gradientTexels(2, 2))) +
                     "_2x2.png"),
              "late png");
    check(!index.contains(stableContentKey(2, 2, gradientTexels(2, 2))),
          "folder: index is a snapshot; unre-scanned file absent");
    const PackIndex rescan = scanPackFolder(dir);
    check(rescan.contains(stableContentKey(2, 2, gradientTexels(2, 2))),
          "folder: re-scan picks the late file up");

    // Negative scans.
    const PackIndex missing = scanPackFolder(root / "does-not-exist");
    check(missing.err == PackScanError::NotADirectory, "folder: missing dir refused");
    const PackIndex fileScan = scanPackFolder(plan.path);  // a file, not a folder
    check(fileScan.err == PackScanError::NotADirectory, "folder: non-directory refused");

    std::filesystem::remove_all(root, ec);
}

} // namespace

int main() {
    testKeyStability();
    testKeyPaletteSwap();
    testKeyStringRoundTrip();
    testParsePackFileNameValid();
    testParsePackFileNameInvalid();
    testIndexAndCollision();
    testReplacementVerdicts();
    testMipLevelCount();
    testBuildMipChain();
    testDumpEncoders();
    testDumpNameRoundTrip();
    testPlanAndWrite();
    testDumpFolderUsability();

    if (g_failures > 0) {
        std::cerr << "test_hd_pack: " << g_failures << " of " << g_checks
                  << " checks FAILED\n";
        return EXIT_FAILURE;
    }
    std::cout << "test_hd_pack: " << g_checks << " checks passed\n";
    return EXIT_SUCCESS;
}
