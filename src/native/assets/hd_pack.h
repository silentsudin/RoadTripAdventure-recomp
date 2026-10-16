#pragma once

// src/native/assets/hd_pack.h
//
// HD-texture-pack LOGIC for the Road Trip native rebuild — pure C++, no GPU:
// everything here is content keys, folder indexing, replacement requests, a
// dump writer and a CPU mip-chain builder. The module deliberately knows
// nothing about Vulkan/samplers; it produces the CONTRACT the GPU wave
// consumes (path, dims, key, mip count, filter/clamp policy) and the GPU
// wave decides how to upload it (docs/native-reference/hd-texture-packs.md
// is the design ledger for this split).
//
// Provenance labels (the repo's convention, see car_models.h):
//   CONFIRMED — ported ideas from the sibling recomp app's HD-pack system
//   (CLAUDE.md, "Texture dumps and HD packs"): hash-of-content naming so the
//   same texture keeps its name across runs wherever it sits; packs are plain
//   folders of images whose names start with the 16-hex key, at ANY size with
//   their own alpha; replacements get a full mip chain and an anisotropic
//   sampler (default 16); clamped axes must not bleed the pack image's edge;
//   a pack must pack-switch atomically (drop everything, re-decode).
//   AUTHORED — native-first decisions documented in the header and the
//   ledger: WHICH bytes feed the key (the sibling hashes GS-readback pixels;
//   native keys come from OUR decode path — the post-palette RGBA8 a disc
//   pack decodes to via image_converter, never from GS uploads), the
//   optional `_src<W>x<H>` mismatch guard, the deterministic lexicographic
//   last-wins collision rule, the unpadded floor-halving mip chain, and the
//   PPM/PAM dump formats (the repo's committed snapshot writer is P6 PPM —
//   src/native/app/main.cpp RT_APP_CARMODEL_DUMP / RT_APP_SNAPSHOT writers —
//   so a portable dump needs no new third-party image dependency).
//
// The native decode path this module keys (READ-ONLY references, the only
// texture sources the app has today):
//   - assets/image_converter.{h,cpp}  convertImage / convertImageBank —
//     disc upload chain -> ImageRecord (RGBA8, palette applied, CSM1 order,
//     byte0 = R). The key's texel bytes are exactly this record's pixels.
//   - assets/car_models.cpp:183      extractCarLivery -> one 128x128 livery
//                                     ImageRecord per CARn/Qnn.BIN.
//   - assets/field_converter.cpp     decodeFieldTextures -> FieldTexturePage
//                                     rgba (w*h*4, R at byte 0).
//   - assets/glyph_atlas.{h,cpp}     decodeGlyphAtlas -> FONTE 512x384 atlas.
//   - src/native/app/main.cpp wraps each of these into preview::PreviewTexture
//     (e.g. 1008-1009 for the car livery, 915-950 for the atlas) — the
//     integration seam where an HD-pack lookup slots in.
//
// Failure contract (mirrors container.h / car_models.h): a rejected input
// produces the default-constructed value, never partial output:
//   - StableContentKey{0} (parseStableContentKey / stableContentKey) means
//     "not a keyable texture" (wrong byte count or zero dims);
//   - an unparseable pack file name is refused entry and only counted;
//   - a ReplacementRequest with verdict != Replace never names a file.

#include "container.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace rt::native::assets {

// ── StableContentKey ───────────────────────────────────────────────────────

// 64-bit FNV-1a (basis 0xcbf29ce484222325, prime 0x100000001b3) — the exact
// constants the repo already uses for content identity (preview.cpp
// hashTexture, main.cpp textureContentHash), so pack keys share the family
// readers can cross-check. Not cryptographic: collisions are ~1 in 2^32 at
// birthday bound, and every appearance is double-checked against the live
// decoded content (a replacement never forces pixels, see "no pixel changes"
// contract in the ledger), so FNV-1a 64 is adequate at pack scale (~10^4
// textures). Documented deliberately: swap the feed only behind a bump of
// kStableKeyLayoutVersion (below), never silently.
inline constexpr std::uint64_t kFnv1a64OffsetBasis = 1469598103934665603ull;
inline constexpr std::uint64_t kFnv1a64Prime = 1099511628211ull;

// Feed layout revision. Bumping it re-keys every content name (all packs and
// dumps must be re-dumped); it exists so a future feed change is a DELIBERATE
// key-space break, not an accident. 1 = [layout][format][W LE][H LE][texels].
inline constexpr std::uint8_t kStableKeyLayoutVersion = 1;

struct StableContentKey {
    std::uint64_t value = 0;  // 0 = null key (not keyable), never a hash

    bool operator==(const StableContentKey&) const = default;
    bool isNull() const { return value == 0; }
};

// Computes the key of one decoded native texture: FNV-1a over
//   byte 0      kStableKeyLayoutVersion
//   byte 1      format tag (PixelFormat byte; only R8G8B8A8_UNORM exists,
//               but the tag is hashed so a future format can never alias a
//               same-dims RGBA8 texture)
//   bytes 2..5  width, little-endian u32
//   bytes 6..9  height, little-endian u32
//   bytes 10..  the recorded texels, row-major, EXACTLY width*height*4 bytes.
//
// Key semantics (CONFIRMED design, sibling precedent "the same texture gets
// the same name across runs, wherever it sits ... a palette swap is a
// different texture"):
//   - content, not location: provenance/offset/variantId are NOT in the
//     feed. Two decodes of the same texels — from different Q-files, folder
//     shuffles, or sessions — get the same key. When OUR decode is ever
//     corrected (palette application, alpha expansion), the texels change,
//     so the key changes: old pack entries simply stop matching; they are
//     inert, never mis-bound (that is how pack keys survive re-decodes —
//     document this in every pack README).
//   - palette-swapped content is a DIFFERENT key: the hash is applied AFTER
//     the palette, over the decoded RGBA the native renderer would sample
//     (the same texels image_converter already emits).
// Returns the null key when rgba.size() != width*height*4 or a dim is zero
// (the shape of a rejected ImageRecord; callers never key failed decodes).
StableContentKey stableContentKey(std::uint32_t width, std::uint32_t height,
                                  std::span<const std::uint8_t> rgba,
                                  PixelFormat format = PixelFormat::R8G8B8A8_UNORM);

// StableContentKey of a decoded ImageRecord (see container.h). Conveniences
// the decode integration points: convertImage / extractCarLivery /
// decodeFieldTextures output ImageRecords with format R8G8B8A8_UNORM and
// pixels.size() == width*height*4, so a good record always keys.
StableContentKey stableContentKey(const ImageRecord& image);

// 16 lowercase hex digits, zero-padded ("0123456789abcdef..."). This is the
// canonical spelling in file names; plain uint64 formatting must go through
// here so parse/print stay inverse functions.
std::string stableKeyToString(StableContentKey key);

// Inverse of stableKeyToString; case-insensitive hex, requires exactly 16
// digits. Null key input is rejected (nullopt) — the null key is never valid
// as a file-name key.
std::optional<StableContentKey> parseStableKey(std::string_view hex16);

// ── Pack file-name grammar ──────────────────────────────────────────────────

// Primary pack naming (mirrors the dump naming, so a dump folder is directly
// pack-usable):
//
//   <key16hex>_<W>x<H>[ _src<Wn>x<Hn> ][ _token ]*. <ext>
//
//   - 16 hex digits (case-insensitive) = StableContentKey, always first.
//   - _<W>x<H>    the PACK IMAGE's own dimensions (what the file decodes to;
//                 any size, any scale, non-power-of-two fine). 'X' tolerated.
//   - _src<Wn>x<Hn>  optional: the NATIVE texture's dimensions the key was
//                 dumped from (AUTHORED mismatch guard — see
//                 requestReplacement; conversion tools preserve what the
//                 dump folder's PPM doesn't have, so this token only exists
//                 when a tool writes it).
//   - further _token pieces (alphanumeric) are tolerated and ignored
//                 (sibling names carry _psmNN tags; upscalers append _4x).
//   - <ext>       png / pam / ppm (png is what GPU waves ultimately decode;
//                 pam/ppm are what OUR dumps write). Extension is required,
//                 lowercased, max 15 chars. Parse checks the grammar only —
//                 which extensions are ACCEPTED is scan policy, one layer up.
//
// Documented fallback naming: a bare <key16hex>.<ext> (no dims token) is
// accepted; the pack image's dims are then unknown to the index (entry
// width/height 0) and the replacement request defers them to load time.
// The GPU wave must read such an image's header before sizing the chain.
//
// Anything else is refused: wrong hex count, empty/zero/oversized (over
// 0xFFFFFF) dims, empty token, double separators, whitespace, or embedded
// path separators.

// Upper bound a dims field may name before the parse refuses (guards
// against overflow while decoding; 16.7M texels a side is far past any pack
// image anyway).
inline constexpr std::uint32_t kPackDimMax = 0xFFFFFFu;

struct ParsedPackFileName {
    bool ok = false;              // grammar accepted
    StableContentKey key{};      // valid iff ok
    bool bare = false;           // fallback naming: no dims token
    std::uint32_t width = 0;    // pack image dims as named (0 when bare)
    std::uint32_t height = 0;
    bool sourceDimsNamed = false;  // an explicit _src<Wn>x<Hn> token
    std::uint32_t sourceWidth = 0;
    std::uint32_t sourceHeight = 0;
    std::string extension;       // lowercased; non-empty iff ok
};

ParsedPackFileName parsePackFileName(std::string_view name);

// ── PackIndex ───────────────────────────────────────────────────────────────

// Extensions the scan accepts as pack images. Anything with a parse-valid
// name but another extension is counted skippedOtherExtension; anything not
// starting with 16 hex digits is foreign (readme.txt, thumbs) and counted
// ignoredForeign — real pack folders carry such files, they are not warnings.
inline constexpr std::string_view kPackImageExtensions[] = {"png", "pam", "ppm"};

struct PackEntry {
    StableContentKey key{};
    std::string fileName;      // leaf name as scanned
    std::filesystem::path path;  // folder / fileName
    bool bare = false;          // fallback naming
    std::uint32_t width = 0;    // pack image dims as named (0 when bare)
    std::uint32_t height = 0;
    bool sourceDimsNamed = false;
    std::uint32_t sourceWidth = 0;
    std::uint32_t sourceHeight = 0;
    std::string extension;      // "png" | "pam" | "ppm"
    bool hadCollision = false;  // more than one file carried this key; this
                                // entry won the walk (see collision rule)
};

enum class PackScanError : std::uint8_t {
    Ok = 0,
    // The path does not exist or is not a directory.
    NotADirectory,
    // The walk failed mid-iteration (permissions / OS error). Entries
    // indexed so far are kept for diagnosis; err != Ok means "do not use".
    WalkError,
};

struct PackWarnings {
    // Key collisions seen (how many indexPackFileName calls replaced an
    // existing entry). SURFACED deliberately: sibling precedent — two PNGs
    // of one hash used to share a cache file and "one was always missing";
    // the count lets the UI warn pack makers instead of silently guessing.
    std::uint32_t collisions = 0;
    // 16-hex-prefixed names whose tail failed the grammar.
    std::uint32_t skippedUnparseable = 0;
    // Grammar-valid names with an extension outside kPackImageExtensions.
    std::uint32_t skippedOtherExtension = 0;
    // Names that do not start with 16 hex digits (foreign files; benign).
    std::uint32_t ignoredForeign = 0;
};

struct PackIndex {
    PackScanError err = PackScanError::Ok;
    std::string errorPath;      // the folder that failed ("" when Ok)
    PackWarnings warnings;
    // One entry per key, SORTED by key (stable, binary-searchable). The
    // COLLISION RULE: when several files carry one key the LAST one wins.
    // The sibling picks "the last file the walk meets" (OS order); this
    // module processes names in lexicographic byte order (AUTHORED: makes
    // last-wins deterministic across platforms, which tests and the cache
    // need; the rule itself — one file per key, one cache file per hash —
    // is the ported CONFIRMED idea).
    std::vector<PackEntry> entries;

    bool contains(StableContentKey key) const;
    const PackEntry* find(StableContentKey key) const;
    std::size_t size() const { return entries.size(); }
    bool valid() const { return err == PackScanError::Ok; }
};

// Adds one file name to the index (used by scanPackFolder, and directly by
// tests / callers that walk their own directory order — the LAST call for a
// key wins). Returns true when the name became (or replaced) an entry.
// Refuses names outside the pack grammar or extension policy, bumping the
// matching warning counter instead.
bool indexPackFileName(PackIndex& index, const std::string& fileName,
                        const std::filesystem::path& path);

// Scans one pack folder: regular files only, names sorted lexicographically,
// each through indexPackFileName (last-wins per key). Not recursive; a pack
// is one flat folder (sibling precedent). The index is a SNAPSHOT — files
// dropped in later need a re-scan (the GPU wave's opts are documented in the
// ledger; simplest correct glue re-scans on pack switch and on demand).
PackIndex scanPackFolder(const std::filesystem::path& folder);

// ── ReplacementRequest ───────────────────────────────────────────────────────

enum class ReplacementVerdict : std::uint8_t {
    // No entry carries this key: draw the native texture unchanged. (This
    // is also the answer for a null key and a failed index scan — a pack
    // problem must never change a drawn pixel, sibling "dumping changes no
    // pixel" contract.)
    Miss = 0,
    // The entry is usable as-is (any dims, any scale, non-power-of-two fine,
    // the pack image's alpha kept as the file stored it).
    Replace,
    // The entry explicitly names the native dims it was built against
    // (_src token) and they do not match this live texture: the pack was
    // authored against a different materialisation of this content (a
    // decode-revision drift visible in naming, or a mis-renamed file). Do
    // not replace; draw native; surface the reason. AUTHORED guard — warns
    // instead of guessing, because the key alone cannot prove dims.
    StaleMismatch,
};

// Filter / addressing contract the GPU wave must honor for a replacement
// (CONFIRMED policies ported from the sibling; the numbers are its proven
// production values):
//   - anisotropic filtering, default 16 (costs next to nothing; user-tunable
//     through the same knob as the sibling's pack_anisotropy, 1..16);
//   - trilinear minification as the base (linear-mipmap-linear + aniso);
//   - clamped axes (PS2 CLAMP / REGION_CLAMP) sample through clamp-to-edge
//     so the pack image never bleeds its neighbours at any mip level;
//     wrapping axes keep repeat addressing — the policy is per draw, hence
//     a bool here plus the note that the GPU wave resolves it per axis;
//   - the pack image's own alpha is used as-is (a replacement is NOT run
//     through the PS2 0x80 alpha expansion).
struct PackSamplePolicy {
    std::uint16_t maxAnisotropy = 16;
    bool trilinearMin = true;
    bool clampToEdgeOnClampedAxes = true;
    bool keepPackAlpha = true;
};

// The GPU-wave contract struct for one resolved texture. Verdict != Replace
// means every other field is default (empty path, zero dims, zero mips).
struct ReplacementRequest {
    ReplacementVerdict verdict = ReplacementVerdict::Miss;
    StableContentKey key{};
    std::filesystem::path path;   // the pack image to load (Replace only)
    std::uint32_t width = 0;      // pack image dims as NAMED (0 = unknown,
    std::uint32_t height = 0;     // bare fallback: the GPU wave reads the
                                  // header at load and sizes the chain)
    std::uint32_t mipLevelCount = 0;  // floor(log2(max(W,H))) + 1, 0 unknown
    PackSamplePolicy policy;      // see PackSamplePolicy
    std::uint32_t nativeWidth = 0;   // echo of the live texture, kept because
    std::uint32_t nativeHeight = 0;  // the GPU wave addresses a replacement
                                     // by the ORIGINAL texture's size
                                     // (sibling precedent: same UV space)
    std::string note;             // collision / fallback / stale reason
};

// Resolves one decoded native texture against a pack index. nativeWidth and
// nativeHeight are the live texture's dims (used only for the _src guard and
// the echo — the KEY match itself is what proves content identity, since the
// dims are already inside the key feed).
ReplacementRequest requestReplacement(const PackIndex& index,
                                      StableContentKey key,
                                      std::uint32_t nativeWidth,
                                      std::uint32_t nativeHeight);

// ── Dump mode ───────────────────────────────────────────────────────────────

// What writeDumpFile emits. Both are plain stdio-friendly formats (a short
// ASCII header then raw bytes) following the committed snapshot writer's
// pattern (main.cpp RT_APP_CARMODEL_DUMP / RT_APP_SNAPSHOT write "P6\nW H\n255\n"
// + RGB triplets) — no third-party image dependency, no game bytes.
//   PamRgba (default) = Netpbm P7 PAM, DEPTH 4 TUPLTYPE RGB_ALPHA: keeps the
//     alpha channel (HUD sprites / font atlas need it; "upscale alpha
//     separately" is sibling advice for AI pack makers, and alpha must
//     SURVIVE the dump for that to be possible at all).
//   PpmRgb = Netpbm P6 PPM: RGB only, byte-identical in spirit to the
//     snapshot writer; eyeball-friendly, drops alpha.
inline constexpr std::string_view kDumpInfoNote =
    "dumps are PPM/PAM (no PNG encoder in the pure C++ layer); convert or "
    "upscale to PNG keeping the 16-hex key, and name the result with the "
    "OUTPUT image's dims: <key>_<newW>x<newH>.png (see hd-texture-packs.md)";

enum class DumpFormat : std::uint8_t {
    PamRgba = 0,
    PpmRgb = 1,
};

// The dump format's file extension ("pam" / "ppm").
std::string_view dumpExtension(DumpFormat format);

// The full dump file name: "<key16>_<W>x<H>.pam|ppm", where W/H are the
// DECODED NATIVE texture's dims (the dump is 1x, so the pack-name dims equal
// the pack image's own dims — one grammar, dump folder directly pack-usable).
std::string dumpFileName(StableContentKey key, std::uint32_t width,
                         std::uint32_t height, DumpFormat format);

// In-memory encoders (pure, deterministic; exactly what the writer emits):
// "P6\n<W> <H>\n255\n" + W*H RGB triplets  /  "P7\nWIDTH <W>\nHEIGHT <H>\n"
// "DEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n" + W*H RGBA quads.
// Reject (return empty) on a span not exactly W*H*4 bytes or a zero dim.
std::vector<std::uint8_t> encodePpmRgb(std::uint32_t width, std::uint32_t height,
                                       std::span<const std::uint8_t> rgba);
std::vector<std::uint8_t> encodePamRgba(std::uint32_t width, std::uint32_t height,
                                        std::span<const std::uint8_t> rgba);

enum class DumpWriteError : std::uint8_t {
    Ok = 0,
    // rgba.size() != width*height*4, a zero dim, or a null key.
    BadSize,
    // The output stream could not be opened or the write was short.
    WriteFailed,
};

struct DumpPlan {
    std::filesystem::path path;  // dir / dumpFileName(...)
    std::string fileName;
};

// Where writeDumpFile would put one image (pure; does not touch the disk).
DumpPlan planDump(const std::filesystem::path& dir, StableContentKey key,
                 std::uint32_t width, std::uint32_t height, DumpFormat format);

// Writes one decoded native texture as a dump file (creates the directory;
// overwrites an existing file of the same name — dumps key by content, so a
// re-dump of the same texture is the same file, regardless of which disc
// location packed it).
DumpWriteError writeDumpFile(const std::filesystem::path& dir, StableContentKey key,
                            std::uint32_t width, std::uint32_t height,
                            std::span<const std::uint8_t> rgba, DumpFormat format);

// ── CPU mip-chain builder ────────────────────────────────────────────────────

// Level count of a full chain over a base of W x H, matching the GPUs' own
// extent rule max(1, dim >> level): floor(log2(max(W,H))) + 1, minimum 1.
// 0 when either dim is zero (unknown/unsized — the GPU wave reads the header
// first for bare-fallback entries).
std::uint32_t mipLevelCount(std::uint32_t width, std::uint32_t height);

struct MipLevel {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> rgba;  // width*height*4, row-major RGBA8
};

struct MipChain {
    // levels[0] is the base level (a copy of the input), then each step is
    // a clamped 2x2 box filter of the previous level. Empty when rejected.
    std::vector<MipLevel> levels;

    bool valid() const { return !levels.empty(); }
    std::size_t levelCount() const { return levels.size(); }
};

// Builds the full chain INCLUDING the base level (so the GPU wave uploads
// levels as-is). AUTHORED chain rule (documented, matches mipLevelCount and
// Vulkan floor-halving exactly):
//   - level dims: max(1, prev >> 1) per axis; the chain ends at 1x1, so a
//     100x75 base gives [100x75, 50x37, 25x18, 12x9, 6x4, 3x2, 1x1] (7
//     levels = floor(log2(100)) + 1);
//   - no power-of-two padding, ever: padding would alter sampled content
//     (invented border texels) and Vulkan/Metal accept non-PoT chains;
//   - each output texel = the box average of the up-to-2x2 window of input
//     texels it maps onto, with taps CLAMPED to the previous level's edge
//     when the window runs off it (odd rows/cols duplicate their last tap
//     rather than being skipped); an odd trailing column can fall outside
//     every window (floor halving drops 5->2, sampling columns 0..3) — that
//     matches the GPU's own level-extent math and loses at most half a texel
//     of one column at one level;
//   - straight RGBA average (no premultiplied alpha; the premultiply->
//     filter->unpremultiply variant stays a GPU-wave A/B note for AI packs,
//     see the ledger).
// Rejects (empty chain) a span not exactly width*height*4 or a zero dim.
MipChain buildMipChain(std::uint32_t width, std::uint32_t height,
                       std::span<const std::uint8_t> rgba);

} // namespace rt::native::assets
