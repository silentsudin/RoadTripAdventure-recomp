// src/native/assets/hd_pack.cpp
//
// Implementation of the HD-texture-pack LOGIC module (hd_pack.h). Reviewer
// notes:
// - Pure host C++: no Vulkan, no PS2/guest/GS types, no third-party image
//   library. The only I/O is the pack-folder walk (std::filesystem) and the
//   dump writer (<fstream>), both caller-path-bound; nothing here reads the
//   disc extract or game data.
// - The key feed, the file-name grammar and the mip chain math are the
//   frozen contracts documented in hd_pack.h; every parse checks bounds
//   before it reads, mirroring the house style of car_models.cpp.
// - Determinism is a requirement, not a property: the same inputs produce
//   byte-identical keys, encodings and chains on every run and platform
//   (the scan sorts file names before indexing so last-wins is stable; the
//   box filter is integer math with a documented rounding rule).

#include "hd_pack.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iterator>

namespace rt::native::assets {
namespace {

// ── small deterministic helpers ────────────────────────────────────────────

void fnv1aMix64(std::uint64_t& h, const std::uint8_t* b, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        h ^= b[i];
        h *= kFnv1a64Prime;
    }
}

void putByte(std::vector<std::uint8_t>& v, std::uint8_t x) { v.push_back(x); }

int hexDigitValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isAlnum(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Parses 1..8 decimal digits, value in [1, kPackDimMax]; consumes [i, next).
// Returns false without touching the outputs on any violation.
bool parseDim(std::string_view s, std::size_t& i, std::uint32_t& out,
              std::size_t& next) {
    std::size_t j = i;
    std::uint64_t value = 0;
    int digits = 0;
    while (j < s.size() && isDigit(s[j])) {
        value = value * 10 + static_cast<std::uint64_t>(s[j] - '0');
        ++j;
        ++digits;
        if (value > kPackDimMax || digits > 8) return false;
    }
    if (digits == 0 || value < 1 || value > kPackDimMax) return false;
    out = static_cast<std::uint32_t>(value);
    next = j;
    return true;
}

bool hasPackImageExtension(std::string_view ext) {
    for (std::string_view e : kPackImageExtensions) {
        if (ext == e) return true;
    }
    return false;
}

// True when the name starts with 16 hex digits followed by '_' or '.' (or
// ends right after) — the cheap pre-check that separates pack-shaped names
// from foreign files like "readme.txt".
bool startsWithHex16Name(std::string_view name) {
    if (name.size() < 16) return false;
    for (std::size_t i = 0; i < 16; ++i) {
        if (hexDigitValue(name[i]) < 0) return false;
    }
    return name.size() == 16 || name[16] == '_' || name[16] == '.';
}

constexpr std::uint32_t kMinImageDim = 1;

} // namespace

// ── StableContentKey ───────────────────────────────────────────────────────

StableContentKey stableContentKey(std::uint32_t width, std::uint32_t height,
                                  std::span<const std::uint8_t> rgba,
                                  PixelFormat format) {
    if (width == 0 || height == 0) return StableContentKey{};
    const std::uint64_t area =
        static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height) * 4;
    if (static_cast<std::uint64_t>(rgba.size()) != area) return StableContentKey{};

    std::uint64_t h = kFnv1a64OffsetBasis;
    const std::array<std::uint8_t, 10> head{
        kStableKeyLayoutVersion,
        static_cast<std::uint8_t>(format),
        static_cast<std::uint8_t>(width & 0xFFu),
        static_cast<std::uint8_t>((width >> 8) & 0xFFu),
        static_cast<std::uint8_t>((width >> 16) & 0xFFu),
        static_cast<std::uint8_t>((width >> 24) & 0xFFu),
        static_cast<std::uint8_t>(height & 0xFFu),
        static_cast<std::uint8_t>((height >> 8) & 0xFFu),
        static_cast<std::uint8_t>((height >> 16) & 0xFFu),
        static_cast<std::uint8_t>((height >> 24) & 0xFFu),
    };
    fnv1aMix64(h, head.data(), head.size());
    if (!rgba.empty()) fnv1aMix64(h, rgba.data(), rgba.size());
    return StableContentKey{h};
}

StableContentKey stableContentKey(const ImageRecord& image) {
    if (image.format != PixelFormat::R8G8B8A8_UNORM) return StableContentKey{};
    return stableContentKey(image.width, image.height, image.pixels, image.format);
}

std::string stableKeyToString(StableContentKey key) {
    static constexpr char kHex[] = "0123456789abcdef";
    std::string out(16, '0');
    for (int i = 15; i >= 0; --i) {
        out[static_cast<std::size_t>(i)] = kHex[key.value & 0xFu];
        key.value >>= 4;
    }
    return out;
}

std::optional<StableContentKey> parseStableKey(std::string_view hex16) {
    if (hex16.size() != 16) return std::nullopt;
    std::uint64_t value = 0;
    for (char c : hex16) {
        const int d = hexDigitValue(c);
        if (d < 0) return std::nullopt;
        value = (value << 4) | static_cast<std::uint64_t>(d);
    }
    if (value == 0) return std::nullopt;  // the null key is never nameable
    return StableContentKey{value};
}

// ── Pack file-name grammar ──────────────────────────────────────────────────

ParsedPackFileName parsePackFileName(std::string_view name) {
    ParsedPackFileName parsed;
    std::optional<StableContentKey> key = parseStableKey(name.substr(
        0, name.size() < 16 ? name.size() : 16));
    if (!key || name.size() < 16) return parsed;
    if (!(name.size() == 16 || name[16] == '_' || name[16] == '.')) return parsed;

    std::size_t i = 16;
    if (i == name.size()) return parsed;  // no extension: grammar wants one
    if (name[i] == '.') {
        // Bare fallback naming: <hex16>.<ext> — no dims token.
        const std::size_t extStart = i + 1;
        if (extStart >= name.size()) return parsed;
        for (std::size_t j = extStart; j < name.size(); ++j) {
            if (!isAlnum(name[j])) return parsed;
        }
        if (name.size() - extStart > 15) return parsed;
        parsed.extension.assign(name.substr(extStart));
        std::transform(parsed.extension.begin(), parsed.extension.end(),
                       parsed.extension.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        parsed.ok = true;
        parsed.bare = true;
        parsed.key = *key;
        return parsed;
    }

    // Dims token: '_<W>x<H>'.
    ++i;  // consume '_'
    std::uint32_t w = 0, h = 0;
    std::size_t next = 0;
    if (!parseDim(name, i, w, next)) return parsed;
    i = next;
    if (i >= name.size() || (name[i] != 'x' && name[i] != 'X')) return parsed;
    ++i;
    if (!parseDim(name, i, h, next)) return parsed;
    i = next;

    parsed.width = w;
    parsed.height = h;

    // Optional source-dims token '_src<Wn>x<Hn>' (case-insensitive).
    const bool looksLikeSrc =
        name.size() - i >= 4 && name[i] == '_' &&
        (name[i + 1] == 's' || name[i + 1] == 'S') &&
        (name[i + 2] == 'r' || name[i + 2] == 'R') &&
        (name[i + 3] == 'c' || name[i + 3] == 'C');
    if (looksLikeSrc) {
        std::size_t j = i + 4;
        std::uint32_t sw = 0, sh = 0;
        std::size_t tail = 0;
        if (!parseDim(name, j, sw, tail)) return parsed;
        j = tail;
        if (j >= name.size() || (name[j] != 'x' && name[j] != 'X')) return parsed;
        ++j;
        if (!parseDim(name, j, sh, tail)) return parsed;
        i = tail;
        parsed.sourceDimsNamed = true;
        parsed.sourceWidth = sw;
        parsed.sourceHeight = sh;
    }

    // Any number of further '_token' pieces (psm tags, _4x markers, ...).
    while (i < name.size() && name[i] == '_') {
        ++i;
        std::size_t j = i;
        while (j < name.size() && isAlnum(name[j])) ++j;
        if (j == i) return parsed;  // empty token ("__" or trailing '_')
        i = j;
    }

    // Required '.' + extension.
    if (i >= name.size() || name[i] != '.') return parsed;
    const std::size_t extStart = i + 1;
    if (extStart >= name.size()) return parsed;
    for (std::size_t j = extStart; j < name.size(); ++j) {
        if (!isAlnum(name[j])) return parsed;
    }
    if (name.size() - extStart > 15) return parsed;
    parsed.extension.assign(name.substr(extStart));
    std::transform(parsed.extension.begin(), parsed.extension.end(),
                   parsed.extension.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    parsed.key = *key;
    parsed.ok = true;
    return parsed;
}

// ── PackIndex ───────────────────────────────────────────────────────────────

bool PackIndex::contains(StableContentKey key) const {
    return find(key) != nullptr;
}

const PackEntry* PackIndex::find(StableContentKey key) const {
    const auto it = std::lower_bound(
        entries.begin(), entries.end(), key.value,
        [](const PackEntry& e, std::uint64_t v) { return e.key.value < v; });
    if (it == entries.end() || it->key.value != key.value) return nullptr;
    return &*it;
}

bool indexPackFileName(PackIndex& index, const std::string& fileName,
                        const std::filesystem::path& path) {
    if (!startsWithHex16Name(fileName)) {
        ++index.warnings.ignoredForeign;
        return false;
    }
    const ParsedPackFileName parsed = parsePackFileName(fileName);
    if (!parsed.ok) {
        ++index.warnings.skippedUnparseable;
        return false;
    }
    if (!hasPackImageExtension(parsed.extension)) {
        ++index.warnings.skippedOtherExtension;
        return false;
    }

    const auto it = std::lower_bound(
        index.entries.begin(), index.entries.end(), parsed.key.value,
        [](const PackEntry& e, std::uint64_t v) { return e.key.value < v; });
    const bool collision = it != index.entries.end() && it->key.value == parsed.key.value;

    PackEntry entry;
    entry.key = parsed.key;
    entry.fileName = fileName;
    entry.path = path;
    entry.bare = parsed.bare;
    entry.width = parsed.width;
    entry.height = parsed.height;
    entry.sourceDimsNamed = parsed.sourceDimsNamed;
    entry.sourceWidth = parsed.sourceWidth;
    entry.sourceHeight = parsed.sourceHeight;
    entry.extension = parsed.extension;

    if (collision) {
        entry.hadCollision = true;  // and the loser's flag is dropped: the
        ++index.warnings.collisions;  // entry is the file that won, only
        *it = entry;               // the winner exists after the call
        return true;
    }
    entry.hadCollision = false;
    index.entries.insert(it, entry);
    return true;
}

PackIndex scanPackFolder(const std::filesystem::path& folder) {
    PackIndex index;
    std::error_code ec;
    if (!std::filesystem::exists(folder, ec) || ec ||
        !std::filesystem::is_directory(folder, ec) || ec) {
        index.err = PackScanError::NotADirectory;
        index.errorPath = folder.string();
        return index;
    }

    std::vector<std::pair<std::string, std::filesystem::path>> files;
    try {
        for (const std::filesystem::directory_entry& entry :
             std::filesystem::directory_iterator(folder)) {
            std::error_code entryEc;
            if (!entry.is_regular_file(entryEc) || entryEc) continue;
            files.emplace_back(entry.path().filename().string(), entry.path());
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Mid-walk failure: keep what was indexed for diagnosis; err != Ok
        // tells consumers not to use the index (hd_pack.h failure contract).
        index.err = PackScanError::WalkError;
        index.errorPath = folder.string();
        return index;
    }

    // Deterministic order (AUTHORED, hd_pack.h): byte-wise ascending leaf
    // name, so the same folder indexes identically on every platform and
    // "last wins" names a specific, reproducible file.
    std::sort(files.begin(), files.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& [name, path] : files) {
        indexPackFileName(index, name, path);
    }
    return index;
}

// ── ReplacementRequest ───────────────────────────────────────────────────────

ReplacementRequest requestReplacement(const PackIndex& index,
                                      StableContentKey key,
                                      std::uint32_t nativeWidth,
                                      std::uint32_t nativeHeight) {
    ReplacementRequest request;
    request.key = key;
    request.nativeWidth = nativeWidth;
    request.nativeHeight = nativeHeight;

    if (!index.valid() || key.isNull()) {
        request.verdict = ReplacementVerdict::Miss;
        if (!index.valid()) request.note = "pack index scan failed";
        return request;
    }

    const PackEntry* entry = index.find(key);
    if (entry == nullptr) {
        request.verdict = ReplacementVerdict::Miss;
        return request;
    }

    if (entry->sourceDimsNamed &&
        (entry->sourceWidth != nativeWidth || entry->sourceHeight != nativeHeight)) {
        request.verdict = ReplacementVerdict::StaleMismatch;
        request.note = "pack image names source dims " +
                       std::to_string(entry->sourceWidth) + "x" +
                       std::to_string(entry->sourceHeight) + " but live texture is " +
                       std::to_string(nativeWidth) + "x" + std::to_string(nativeHeight);
        return request;
    }

    request.verdict = ReplacementVerdict::Replace;
    request.path = entry->path;
    request.width = entry->width;
    request.height = entry->height;
    request.mipLevelCount = entry->bare ? 0 : mipLevelCount(entry->width, entry->height);
    if (entry->bare) {
        request.note = "bare fallback name: read the pack image dims at load, "
                       "then size the chain with mipLevelCount()";
    }
    if (entry->hadCollision) {
        request.note += request.note.empty() ? "" : "; ";
        request.note += "pack-key collision: last file wins: " + entry->fileName;
    }
    return request;
}

// ── Dump mode ───────────────────────────────────────────────────────────────

std::string_view dumpExtension(DumpFormat format) {
    return format == DumpFormat::PamRgba ? std::string_view{"pam"}
                                         : std::string_view{"ppm"};
}

std::string dumpFileName(StableContentKey key, std::uint32_t width,
                         std::uint32_t height, DumpFormat format) {
    return stableKeyToString(key) + "_" + std::to_string(width) + "x" +
           std::to_string(height) + "." + std::string{dumpExtension(format)};
}

std::vector<std::uint8_t> encodePpmRgb(std::uint32_t width, std::uint32_t height,
                                       std::span<const std::uint8_t> rgba) {
    if (width < kMinImageDim || height < kMinImageDim) return {};
    const std::uint64_t area =
        static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height);
    if (static_cast<std::uint64_t>(rgba.size()) != area * 4) return {};
    std::string header = "P6\n" + std::to_string(width) + " " + std::to_string(height) +
                         "\n255\n";
    std::vector<std::uint8_t> out;
    out.reserve(header.size() + static_cast<std::size_t>(area) * 3);
    out.insert(out.end(), header.begin(), header.end());
    for (std::size_t i = 0; i < rgba.size(); i += 4) {
        putByte(out, rgba[i]);
        putByte(out, rgba[i + 1]);
        putByte(out, rgba[i + 2]);  // snapshot-writer pattern: alpha dropped
    }
    return out;
}

std::vector<std::uint8_t> encodePamRgba(std::uint32_t width, std::uint32_t height,
                                        std::span<const std::uint8_t> rgba) {
    if (width < kMinImageDim || height < kMinImageDim) return {};
    const std::uint64_t area =
        static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height);
    if (static_cast<std::uint64_t>(rgba.size()) != area * 4) return {};
    std::string header = "P7\nWIDTH " + std::to_string(width) + "\nHEIGHT " +
                         std::to_string(height) +
                         "\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n";
    std::vector<std::uint8_t> out;
    out.reserve(header.size() + rgba.size());
    out.insert(out.end(), header.begin(), header.end());
    out.insert(out.end(), rgba.begin(), rgba.end());  // raw RGBA, alpha kept
    return out;
}

DumpPlan planDump(const std::filesystem::path& dir, StableContentKey key,
                 std::uint32_t width, std::uint32_t height, DumpFormat format) {
    DumpPlan plan;
    plan.fileName = dumpFileName(key, width, height, format);
    plan.path = dir / plan.fileName;
    return plan;
}

DumpWriteError writeDumpFile(const std::filesystem::path& dir, StableContentKey key,
                            std::uint32_t width, std::uint32_t height,
                            std::span<const std::uint8_t> rgba, DumpFormat format) {
    if (key.isNull() || width < kMinImageDim || height < kMinImageDim) {
        return DumpWriteError::BadSize;
    }
    // Encode first: the encoder's own size/dim guards stand in for the span
    // check without a second code path.
    const std::vector<std::uint8_t> bytes =
        format == DumpFormat::PamRgba ? encodePamRgba(width, height, rgba)
                                      : encodePpmRgb(width, height, rgba);
    if (bytes.empty()) return DumpWriteError::BadSize;

    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    if (ec) return DumpWriteError::WriteFailed;

    const DumpPlan plan = planDump(dir, key, width, height, format);
    std::ofstream out(plan.path, std::ios::binary | std::ios::trunc);
    if (!out) return DumpWriteError::WriteFailed;
    out.write(reinterpret_cast<const char*>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    out.flush();
    return out ? DumpWriteError::Ok : DumpWriteError::WriteFailed;
}

// ── CPU mip-chain builder ────────────────────────────────────────────────────

std::uint32_t mipLevelCount(std::uint32_t width, std::uint32_t height) {
    if (width == 0 || height == 0) return 0;
    std::uint32_t m = std::max(width, height);
    std::uint32_t levels = 1;
    while (m > 1) {
        m >>= 1;
        ++levels;
    }
    return levels;
}

MipChain buildMipChain(std::uint32_t width, std::uint32_t height,
                       std::span<const std::uint8_t> rgba) {
    MipChain chain;
    if (width == 0 || height == 0) return chain;
    const std::uint64_t area =
        static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height);
    if (static_cast<std::uint64_t>(rgba.size()) != area * 4) return chain;

    chain.levels.reserve(mipLevelCount(width, height));

    MipLevel base;
    base.width = width;
    base.height = height;
    base.rgba.assign(rgba.begin(), rgba.end());
    chain.levels.push_back(std::move(base));

    // Each step: dims max(1, prev >> 1); every output texel averages the 2x2
    // window it covers, taps clamped to the source edge (odd trailing
    // rows/cols duplicate their last tap); integer average, half rounds up:
    // (sum + 2) >> 2.
    while (chain.levels.back().width > 1 || chain.levels.back().height > 1) {
        const MipLevel& prev = chain.levels.back();
        const std::uint32_t w = std::max<std::uint32_t>(1, prev.width >> 1);
        const std::uint32_t h = std::max<std::uint32_t>(1, prev.height >> 1);
        MipLevel next;
        next.width = w;
        next.height = h;
        next.rgba.assign(static_cast<std::size_t>(w) * h * 4, 0);
        for (std::uint32_t y = 0; y < h; ++y) {
            const std::uint32_t y0 = std::min(y * 2, prev.height - 1);
            const std::uint32_t y1 = std::min(y * 2 + 1, prev.height - 1);
            for (std::uint32_t x = 0; x < w; ++x) {
                const std::uint32_t x0 = std::min(x * 2, prev.width - 1);
                const std::uint32_t x1 = std::min(x * 2 + 1, prev.width - 1);
                for (std::uint32_t c = 0; c < 4; ++c) {
                    const std::uint32_t sum =
                        prev.rgba[(static_cast<std::size_t>(y0) * prev.width + x0) * 4 + c] +
                        prev.rgba[(static_cast<std::size_t>(y0) * prev.width + x1) * 4 + c] +
                        prev.rgba[(static_cast<std::size_t>(y1) * prev.width + x0) * 4 + c] +
                        prev.rgba[(static_cast<std::size_t>(y1) * prev.width + x1) * 4 + c];
                    next.rgba[(static_cast<std::size_t>(y) * w + x) * 4 + c] =
                        static_cast<std::uint8_t>((sum + 2) >> 2);
                }
            }
        }
        chain.levels.push_back(std::move(next));
    }
    return chain;
}

} // namespace rt::native::assets
