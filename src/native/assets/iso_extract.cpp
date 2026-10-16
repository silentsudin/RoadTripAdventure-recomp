// Implementation of the first-run ISO9660 reader/gate/extractor.
//
// Wire-format facts here are ECMA-119/ISO9660; the sibling's proven reader
// semantics (src/rom/IsoReader.{h,cpp}) are AUTHORED ports where noted,
// without libchdr (coordinator decision, see
// docs/native-reference/first-run-extraction.md).  Dev-time statements
// marked [disc-verified] were observed read-only against the user's own
// dump; no disc bytes are embedded or committed.

#include "iso_extract.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <numeric>
#include <system_error>
#include <utility>

namespace rt::native::assets {
namespace {

constexpr std::uint8_t kPvdType = 1;                // primary volume descriptor type
constexpr std::uint32_t kPvdRootRecordOffset = 156;  // root directory record inside the PVD
constexpr std::size_t kIsoRecordHeaderBytes = 33;   // everything before the name field
constexpr double kEtaWindowSeconds = 40.0;          // ProgressEta port: sample window
constexpr double kEtaMinSpanSeconds = 1.0;          // ProgressEta port: shortest trusted span
constexpr double kEtaWarmupSeconds = 3.0;           // ProgressEta port: phase warm-up
constexpr std::uint64_t kEtaMinDoneTwentieth = 20;  // ProgressEta port: need at least 5% done

std::uint32_t le32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

std::string toUpperFold(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return s;
}

std::string hex8(std::uint32_t v) {
    static const char* digits = "0123456789ABCDEF";
    std::string out(8, '0');
    for (int i = 0; i < 8; ++i) out[7 - i] = digits[(v >> (4 * i)) & 0xF];
    return out;
}

double steadySeconds() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

const std::array<std::uint32_t, 256>& crc32Table() {
    static const std::array<std::uint32_t, 256> table = [] {
        std::array<std::uint32_t, 256> t{};
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            t[i] = c;
        }
        return t;
    }();
    return table;
}

// Follows a .cue sheet's FILE line to its backing file (double-quoted or
// bare).  TRACK/INDEX lines are ignored: the single data-track .cue/.bin
// pair players keep is the only shape handled (sibling
// IsoReader::resolveCue precedent, AUTHORED port).
bool resolveCueDataFile(const std::filesystem::path& cue, std::filesystem::path& out) {
    std::ifstream in(cue);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line)) {
        const std::size_t fileAt = line.find("FILE");
        if (fileAt == std::string::npos) continue;
        std::size_t p = fileAt + 4;
        while (p < line.size() && std::isspace(static_cast<unsigned char>(line[p]))) ++p;
        std::string name;
        if (p < line.size() && line[p] == '"') {
            const std::size_t end = line.find('"', p + 1);
            if (end == std::string::npos) return false;
            name = line.substr(p + 1, end - p - 1);
        } else {
            std::size_t end = p;
            while (end < line.size() && !std::isspace(static_cast<unsigned char>(line[end]))) ++end;
            name = line.substr(p, end - p);
        }
        if (name.empty()) return false;
        out = cue.parent_path() / name;
        return true;
    }
    return false;
}

bool looksLikeChd(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    char magic[8] = {};
    return in.read(magic, 8) && std::memcmp(magic, "MComprHD", 8) == 0;
}

// The walk's pupil names come from the image; a damaged or hostile dump
// must not escape the extraction root ('.'/'..' segments, separators,
// control bytes, absurd lengths).  `leaf` arrives uppercased
// (canonicalization); a nested '/' (no real PS2 dump has one) is allowed
// and stays a relative path inside the target folder.
bool safeLeafName(std::string_view leaf) {
    if (leaf.empty() || leaf.size() > 200) return false;
    std::size_t segmentStart = 0;
    while (segmentStart <= leaf.size()) {
        const std::size_t segmentEnd = leaf.find('/', segmentStart);
        const std::string_view segment =
            leaf.substr(segmentStart, (segmentEnd == std::string_view::npos ? leaf.size() : segmentEnd) - segmentStart);
        if (segment.empty() || segment == "." || segment == "..") return false;
        for (char c : segment) {
            const auto u = static_cast<unsigned char>(c);
            if (u < 0x20 || u == 0x7F || c == '\\') return false;
        }
        if (segmentEnd == std::string_view::npos) break;
        segmentStart = segmentEnd + 1;
    }
    return true;
}

bool writeSmallFile(const std::filesystem::path& path, std::string_view text) {
    std::FILE* out = std::fopen(path.string().c_str(), "wb");
    if (!out) return false;
    const bool ok = text.empty() || std::fwrite(text.data(), 1, text.size(), out) == text.size();
    return std::fclose(out) == 0 && ok;
}

std::string jsonEscape(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (char c : text) {
        if (c == '"' || c == '\\') {
            out += '\\';
            out += c;
        } else if (static_cast<unsigned char>(c) < 0x20) {
            out += ' ';
        } else {
            out += c;
        }
    }
    return out;
}

// ---------------------------------------------------------------- manifest matching

// Pattern forms (matched case-insensitively): "<rootfile>" = one root
// file; "<folder>" = the whole folder; "<folder>/*" = the same, explicit;
// "<folder>/*.EXT" = an extension subset.
struct PatternParts {
    bool rootFile = false;
    std::string rootFolded;   // root files: uppercased whole pattern
    std::string folder;        // canonical folder spelling (manifest's own case)
    std::string prefix;        // uppercased folder + "/"
    std::string leafSpec;      // "" (whole folder), "*", "*.EXT" or an exact leaf
};

PatternParts splitPattern(const ManifestEntrySpec& spec) {
    const std::string_view pattern(spec.pattern);
    PatternParts parts;
    const std::size_t slash = pattern.find('/');
    if (slash == std::string_view::npos) {
        parts.rootFile = true;
        parts.rootFolded = toUpperFold(std::string(pattern));
        parts.folder = std::string(pattern);
        parts.prefix = parts.rootFolded + "/";  // dual reading: see patternMatches
        return parts;
    }
    parts.folder = std::string(pattern.substr(0, slash));
    parts.prefix = toUpperFold(parts.folder) + "/";
    parts.leafSpec = std::string(pattern.substr(slash + 1));
    return parts;
}

bool patternMatches(const PatternParts& parts, const std::string& foldedEntry) {
    // Slash-free pattern (SLUS_203.98, SYSTEM.CNF, SYS, CAR0..): dual reading
    // per the grammar comment — a root file when the image holds that exact
    // name, the whole folder when the image holds a folder by that name
    // (folder rules carry an empty leafSpec).  A root-file-only reading made
    // plain folder names match nothing, since walk() yields files only under
    // their own "FOLDER/" prefixed paths.
    if (parts.rootFile) return foldedEntry == parts.rootFolded || foldedEntry.starts_with(parts.prefix);
    if (!foldedEntry.starts_with(parts.prefix)) return false;
    if (parts.leafSpec.empty() || parts.leafSpec == "*") return true;
    const std::string_view leaf =
        std::string_view(foldedEntry).substr(parts.prefix.size());
    if (parts.leafSpec.starts_with("*.")) {
        const std::string ext = toUpperFold(parts.leafSpec.substr(1));
        return leaf.size() >= ext.size() && leaf.ends_with(ext);
    }
    return leaf == toUpperFold(parts.leafSpec);
}

}  // namespace

// ---------------------------------------------------------------- CRC32

void Crc32::update(const std::uint8_t* data, std::size_t size) noexcept {
    const auto& table = crc32Table();
    std::uint32_t c = m_crc;
    for (std::size_t i = 0; i < size; ++i)
        c = table[(c ^ data[i]) & 0xFFu] ^ (c >> 8);
    m_crc = c;
}

// ---------------------------------------------------------------- errors

const char* describeIsoError(IsoError error) noexcept {
    switch (error) {
        case IsoError::NotReadable: return "NotReadable";
        case IsoError::NotISO: return "NotISO";
        case IsoError::NotThisGame: return "NotThisGame";
        case IsoError::CorruptFile: return "CorruptFile";
        case IsoError::Io: return "Io";
    }
    return "?";
}

// ---------------------------------------------------------------- IsoImage

IsoImage::~IsoImage() { close(); }

IsoImage::IsoImage(IsoImage&& other) noexcept { *this = std::move(other); }

IsoImage& IsoImage::operator=(IsoImage&& other) noexcept {
    if (this != &other) {
        close();
        m_file = std::exchange(other.m_file, nullptr);
        m_stride = other.m_stride;
        m_dataOffset = other.m_dataOffset;
        m_volumeId = std::move(other.m_volumeId);
        m_volumeSectors = other.m_volumeSectors;
        m_root = std::move(other.m_root);
    }
    return *this;
}

void IsoImage::close() noexcept {
    if (m_file) std::fclose(m_file);
    m_file = nullptr;
    m_stride = IsoStride::Cooked2048;
    m_dataOffset = 0;
    m_volumeId.clear();
    m_volumeSectors = 0;
    m_root = IsoEntry{};
}

bool IsoImage::readSectors(std::uint32_t lba, std::uint32_t count, std::uint8_t* out) const {
    if (!m_file || count == 0) return false;
    if (m_stride == IsoStride::Cooked2048) {
        if (std::fseek(m_file, static_cast<long>(lba) * kIsoSectorBytes, SEEK_SET) != 0) return false;
        return std::fread(out, kIsoSectorBytes, count, m_file) == count;
    }
    for (std::uint32_t i = 0; i < count; ++i) {
        const long pos = static_cast<long>(lba + i) * kIsoRawSectorBytes + static_cast<long>(m_dataOffset);
        if (std::fseek(m_file, pos, SEEK_SET) != 0 ||
            std::fread(out + static_cast<std::size_t>(i) * kIsoSectorBytes, kIsoSectorBytes, 1, m_file) != 1)
            return false;
    }
    return true;
}

expected<IsoVolume, IsoError> IsoImage::open(const std::filesystem::path& image, IsoOpenOptions options) {
    close();
    std::filesystem::path data = image;
    if (toUpperFold(image.extension().string()) == ".CUE") {
        if (!options.allowCue || !resolveCueDataFile(image, data)) {
            close();
            return IsoError::NotReadable;  // a cue we cannot resolve reads as no image at all
        }
    }

    m_file = std::fopen(data.string().c_str(), "rb");
    if (!m_file) {
        close();
        return IsoError::NotReadable;
    }

    // An image must at least hold the PVD sector.
    if (std::fseek(m_file, 0, SEEK_END) != 0) {
        close();
        return IsoError::NotReadable;
    }
    const long long end = std::ftell(m_file);
    std::fseek(m_file, 0, SEEK_SET);
    if (end < 0 || static_cast<std::uint64_t>(end) < (kIsoPvdLba + 1) * kIsoSectorBytes) {
        close();
        return IsoError::NotReadable;
    }

    {  // CHD containers are deliberately out of scope here (first-run-extraction.md
       // documents the deferral; libchdr is a permitted optional dep): the
       // magic is refused up-front with a typed NotReadable instead of being
       // misdetected as a bad ISO.
        std::uint8_t magic[8] = {};
        static const std::uint8_t kChd[8] = {'M', 'C', 'o', 'm', 'p', 'r', 'H', 'D'};
        if (std::fseek(m_file, 0, SEEK_SET) == 0 &&
            std::fread(magic, 1, sizeof(magic), m_file) == sizeof(magic) &&
            std::memcmp(magic, kChd, sizeof(kChd)) == 0) {
            close();
            return IsoError::NotReadable;
        }
    }

    // Stride detection (documented heuristic): validate the PVD at its three
    // possible file positions, in order cooked (16*2048), raw Mode 2
    // (16*2352+24) and raw Mode 1 (16*2352+16).  A real image matches
    // exactly one of them: on the good dump (raw Mode 2) the cooked probe
    // lands in all-zero system-area bytes [disc-verified], so the order is
    // unambiguous.  Probes that cannot be read count as non-matches; none
    // matching means NotISO.
    struct StrideProbe {
        IsoStride stride;
        std::uint64_t fileOffset;
        std::uint32_t dataOffset;
    };
    const StrideProbe probes[3] = {
        {IsoStride::Cooked2048, std::uint64_t(kIsoPvdLba) * kIsoSectorBytes, 0},
        {IsoStride::RawMode2, std::uint64_t(kIsoPvdLba) * kIsoRawSectorBytes + kRawSectorDataOffsetMode2, kRawSectorDataOffsetMode2},
        {IsoStride::RawMode1, std::uint64_t(kIsoPvdLba) * kIsoRawSectorBytes + kRawSectorDataOffsetMode1, kRawSectorDataOffsetMode1},
    };
    const int probeCount = options.allowRawBin ? 3 : 1;
    std::array<std::uint8_t, kIsoSectorBytes> pvd{};
    bool found = false;
    for (int i = 0; i < probeCount; ++i) {
        if (std::fseek(m_file, static_cast<long>(probes[i].fileOffset), SEEK_SET) != 0) continue;
        if (std::fread(pvd.data(), 1, pvd.size(), m_file) != pvd.size()) continue;
        if (pvd[0] != kPvdType || std::memcmp(pvd.data() + 1, "CD001", 5) != 0 || pvd[6] != 1) continue;
        const std::uint8_t* root = pvd.data() + kPvdRootRecordOffset;
        if (root[0] < 34) continue;
        const std::uint32_t rootLba = le32(root + 2);
        const std::uint32_t rootBytes = le32(root + 10);
        if (rootLba == 0 || rootBytes == 0 || (root[25] & 0x02) == 0) continue;
        m_stride = probes[i].stride;
        m_dataOffset = probes[i].dataOffset;
        found = true;
        break;
    }
    if (!found) {
        close();
        return IsoError::NotISO;
    }

    m_volumeId.assign(reinterpret_cast<const char*>(pvd.data() + 40), 32);
    while (!m_volumeId.empty() && m_volumeId.back() == ' ') m_volumeId.pop_back();
    m_volumeSectors = le32(pvd.data() + 80);
    const std::uint8_t* rootRec = pvd.data() + kPvdRootRecordOffset;
    m_root = IsoEntry{"", le32(rootRec + 2), le32(rootRec + 10), true};

    IsoVolume volume;
    volume.volumeId = m_volumeId;
    volume.stride = m_stride;
    volume.volumeSpaceSectors = m_volumeSectors;
    volume.root = m_root;
    return volume;
}

expected<std::vector<IsoEntry>, IsoError> IsoImage::listDir(const IsoEntry& dir) const {
    std::vector<IsoEntry> out;
    if (!m_file || !dir.isDir || dir.bytes == 0) return out;

    const std::uint32_t sectors =
        static_cast<std::uint32_t>((dir.bytes + kIsoSectorBytes - 1) / kIsoSectorBytes);
    std::vector<std::uint8_t> extent(static_cast<std::size_t>(sectors) * kIsoSectorBytes, 0);
    if (!readSectors(dir.lba, sectors, extent.data())) return IsoError::Io;
    const std::size_t dataSize = static_cast<std::size_t>(dir.bytes);

    std::size_t i = 0;
    while (i < dataSize) {
        const std::uint8_t len = extent[i];
        if (len == 0) {
            // Records never span a logical sector: zero length here means
            // "the rest of this sector is padding" (ECMA-119 padding rule).
            i = (i / kIsoSectorBytes + 1) * kIsoSectorBytes;
            continue;
        }
        if (len < kIsoRecordHeaderBytes || len > dataSize - i) break;  // damaged tail: stop this extent
        const std::uint8_t* rec = extent.data() + i;
        const std::uint32_t flags = rec[25];
        const std::uint8_t nameLen = rec[32];
        if (nameLen >= 1 && !(nameLen == 1 && (rec[33] == 0x00 || rec[33] == 0x01))) {
            // '\x00' self and '\x01' parent records are skipped; everything
            // else is kept, hidden flag included (some dumps mark data).
            std::string name(reinterpret_cast<const char*>(rec + 33), nameLen);
            const std::size_t semi = name.find(';');
            if (semi != std::string::npos) name.resize(semi);      // ";1" version suffix
            if (!name.empty() && name.back() == '.') name.pop_back();  // level-1 lone trailing dot
            if (!name.empty()) {
                out.push_back(IsoEntry{dir.name.empty() ? name : dir.name + "/" + name,
                                       le32(rec + 2), le32(rec + 10), (flags & 0x02) != 0});
            }
        }
        i += len;
    }
    return out;
}

expected<std::vector<IsoEntry>, IsoError> IsoImage::walk() const {
    if (!m_file) return IsoError::Io;
    std::vector<IsoEntry> entries;
    std::vector<IsoEntry> stack{m_root};
    while (!stack.empty()) {
        IsoEntry dir = stack.back();
        stack.pop_back();
        auto children = listDir(dir);
        if (!children.has_value()) return children;
        for (IsoEntry& child : *children) {
            entries.push_back(child);
            if (child.isDir) stack.push_back(child);
        }
    }
    return entries;
}

expected<IsoEntry, IsoError> IsoImage::locate(std::string_view name) const {
    if (!m_file) return IsoError::Io;
    auto entries = walk();
    if (!entries.has_value()) return entries.error();
    const std::string exact(name);
    const std::string folded = toUpperFold(exact);
    // Contract (test_locateExactAndFolded): directories match folded; the
    // query's LEAF wins by exact spelling wherever it sits; with no exact
    // leaf, the first fold match in walk order applies.  (Full-path exact
    // comparison would never fire when directory spellings differ by case,
    // e.g. mastered "DATA/" queried as "data/".)
    const std::string_view queryLeaf =
        std::string_view(exact).substr(exact.find_last_of('/') + 1);
    const IsoEntry* foldedMatch = nullptr;
    for (const IsoEntry& e : *entries) {
        if (e.isDir) continue;
        if (toUpperFold(e.name) != folded) continue;
        const std::string_view leaf =
            std::string_view(e.name).substr(e.name.find_last_of('/') + 1);
        if (leaf == queryLeaf) return e;  // exact leaf spelling wins, wherever it sits
        if (foldedMatch == nullptr) foldedMatch = &e;
    }
    if (foldedMatch != nullptr) return *foldedMatch;
    return IsoError::NotThisGame;
}

bool IsoImage::readFile(const IsoEntry& entry,
                        const std::function<bool(const std::uint8_t*, std::size_t)>& sink,
                        const std::function<void(std::uint64_t, std::uint64_t)>& progress,
                        std::uint32_t chunkSectors) const {
    if (entry.bytes == 0) return true;
    if (!m_file || chunkSectors == 0) return false;
    chunkSectors = std::min<std::uint32_t>(chunkSectors, 1024);
    std::vector<std::uint8_t> buf(static_cast<std::size_t>(chunkSectors) * kIsoSectorBytes);
    std::uint64_t done = 0;
    std::uint32_t lba = entry.lba;
    std::uint64_t remaining = entry.bytes;
    while (remaining > 0) {
        const std::uint32_t sectors = static_cast<std::uint32_t>(
            std::min<std::uint64_t>(chunkSectors, (remaining + kIsoSectorBytes - 1) / kIsoSectorBytes));
        if (!readSectors(lba, sectors, buf.data())) return false;
        const std::size_t n =
            static_cast<std::size_t>(std::min<std::uint64_t>(remaining, std::uint64_t(sectors) * kIsoSectorBytes));
        done += n;
        if (progress) progress(done, entry.bytes);
        if (sink && !sink(buf.data(), n)) return false;
        remaining -= n;
        lba += sectors;
    }
    return true;
}

// ---------------------------------------------------------------- gate

expected<GateResult, IsoError> runGateChain(const IsoImage& image, const GateConfig& config,
                                           const std::function<void(GateStage, std::uint64_t, std::uint64_t)>& progress,
                                           std::uint32_t* actualCrcOut) {
    auto gateFile = image.locate(config.bootElfPath);
    if (!gateFile.has_value()) return gateFile.error();  // stage 3: NotThisGame
    if (progress) progress(GateStage::Located, 0, gateFile->bytes);

    Crc32 crc;
    const bool read = image.readFile(
        *gateFile,
        [&](const std::uint8_t* data, std::size_t n) {
            crc.update(data, n);
            return true;
        },
        [&](std::uint64_t done, std::uint64_t total) {
            if (progress) progress(GateStage::Hashing, done, total);
        });
    const std::uint32_t actual = crc.value();
    if (actualCrcOut) *actualCrcOut = actual;
    if (!read) return IsoError::Io;
    if (actual != config.expectedBootCrc) return IsoError::CorruptFile;

    GateResult result;
    result.bootElf = *gateFile;
    result.actualCrc = actual;
    result.volume.volumeId = image.volumeId();
    result.volume.stride = image.stride();
    result.volume.volumeSpaceSectors = image.volumeSpaceSectors();
    result.volume.root = image.root();
    return result;
}

// ---------------------------------------------------------------- manifest

const std::vector<ManifestEntrySpec>& nativeAssetManifest() {
    // Data-driven list, not guesswork: every pattern was verified against the
    // reference inventory of the good dump (the sibling extract's
    // .lbn_map.tsv, name/lba/BYTES, 376 entries; folders ACTION, CAR0..CAR4,
    // CARS, COURSE, FLD, ITEM, SHOP, SOUND, SYS [disc-verified]; CAR5 is
    // absent there), and every provenance string names the native consumer
    // or docs/native-reference page that needs it.
    static const std::vector<ManifestEntrySpec> manifest = {
        {"SLUS_203.98", AssetRule::Required,
         "gate + runtime ELF reads: nav graphs (nav_routes.cpp), field-tile LBA index "
         "(field_tile_index.cpp), hud-text data; name/size/CRC are CLAUDE.md facts"},
        {"SYSTEM.CNF", AssetRule::Optional,
         "disc identity metadata (57 B on the good dump); no native consumer, kept for a complete install"},
        {"SYS", AssetRule::Required,
         "44 files: 37 UI GSL packs + fonts + E3D boot-logo models (sys-gsl-packs.md sections 3/5; "
         "glyph_atlas.cpp FONTE, stamp art) and asset-map.md table B (request byte index)"},
        {"FLD", AssetRule::Required,
         "64 field tiles ABC.BIN (field-topology-status.md; field_tile_index.cpp via .lbn_map provenance)"},
        {"COURSE", AssetRule::Required,
         "19 course containers C00..C20 with C15/C17 absent (course-data-status.md; course_data.cpp)"},
        {"CAR0", AssetRule::Required,
         "car packs Q00..Q29.BIN (asset-map.md table E loadCarModel; car_models.cpp and the "
         "RT_APP_CARMODEL lookup in main.cpp)"},
        {"CAR1", AssetRule::Required, "car packs Q30..Q59.BIN (asset-map.md table E)"},
        {"CAR2", AssetRule::Required, "car packs Q60..Q89.BIN (asset-map.md table E)"},
        {"CAR3", AssetRule::Required, "car packs Q90..Q119.BIN (asset-map.md table E)"},
        {"CAR4", AssetRule::Required, "car packs Q120..Q149.BIN (asset-map.md table E)"},
        {"CAR5", AssetRule::Optional,
         "tolerated by main.cpp's car-model walk (CAR0..CAR5, CARS); ABSENT on the good dump "
         "(.lbn_map.tsv lists CAR0..CAR4 + CARS only)"},
        {"CARS", AssetRule::Required,
         "shared parts Q150/TIRE/PARTS/WHEEL.BIN (asset-map.md tables B and E)"},
        {"ITEM", AssetRule::Required,
         "12 files: METER0..10.GSL gauges + PARTS.GSL shop icons (hud_layout.cpp texture table "
         "0x2A9B08; asset-map.md table D)"},
        {"ACTION", AssetRule::Optional,
         "A00..A20.BIN action containers: asset-map.md table F proves the disc's own loader "
         "(request types 18..38) consumes them; native consumer not wired yet"},
        {"SHOP", AssetRule::Optional,
         "GARAGE.BIN + T00..T20.BIN shop packs (asset-map.md table D); the native shop module "
         "(gameplay/shop.cpp) is not wired to them yet"},
        {"SOUND/*.TSQ", AssetRule::Required,
         "20 sequence files (bgm_player.cpp SOUND/*.TSQ, tsq_sequencer.cpp, music_seq.cpp parseTsq)"},
        {"SOUND/*.TVB", AssetRule::Required,
         "3 sample banks BGM.TVB / CQ_MAIN.TVB / ACTION.TVB (music_seq.cpp parseTvb, bgm_player.cpp)"},
        {"SOUND/*.VAG", AssetRule::Required,
         "6 radio/demo streams 1CH/2CH/3CH _L/_R.VAG, ~148 MB (music_library.cpp injected paths; "
         "asset-map.md table A sound-RPC path)"},
    };
    return manifest;
}

// ---------------------------------------------------------------- plan

expected<ExtractionPlan, IsoError> buildExtractionPlan(const std::vector<IsoEntry>& entries,
                                                       std::vector<std::string>* missingOut) {
    std::vector<PatternParts> parts;
    for (const ManifestEntrySpec& spec : nativeAssetManifest()) parts.push_back(splitPattern(spec));

    ExtractionPlan plan;
    std::map<std::string, std::size_t> byCanonical;  // canonical path -> plan index (fold dedupe)
    std::vector<int> matchedByRule(parts.size(), 0);

    for (std::size_t walkIndex = 0; walkIndex < entries.size(); ++walkIndex) {
        const IsoEntry& entry = entries[walkIndex];
        if (entry.isDir) continue;
        const std::string folded = toUpperFold(entry.name);
        for (std::size_t r = 0; r < parts.size(); ++r) {
            if (!patternMatches(parts[r], folded)) continue;
            // The first matching rule (manifest order) fixes the canonical
            // target: the manifest's own folder spelling plus the uppercased
            // remainder, so uppercase- and lowercase-mastered dumps land on
            // identical host paths and a case-sensitive filesystem is safe.
            const std::string leaf =
                parts[r].rootFile ? folded : folded.substr(parts[r].prefix.size());
            if (!safeLeafName(leaf)) break;  // damaged/hostile name: excluded
            const std::string target = parts[r].rootFile
                                          ? leaf
                                          : parts[r].folder + "/" + leaf;
            if (byCanonical.find(target) != byCanonical.end()) break;  // fold-duplicate: first in walk order kept
            ExtractionPlanItem item;
            item.path = target;
            item.imagePath = entry.name;
            item.lba = entry.lba;
            item.bytes = entry.bytes;
            item.walkIndex = walkIndex;
            item.rule = &nativeAssetManifest()[r];
            byCanonical[target] = plan.items.size();
            plan.items.push_back(std::move(item));
            break;
        }
    }

    // Required-rule coverage on a gate-passed image: a miss blames the
    // image (the good dump matches every rule; 384/384 descriptor sweep in
    // asset-map.md).
    std::vector<std::string> missingRequired;
    for (const ExtractionPlanItem& item : plan.items) ++matchedByRule[item.rule - nativeAssetManifest().data()];
    for (std::size_t r = 0; r < parts.size(); ++r)
        if (nativeAssetManifest()[r].rule == AssetRule::Required && matchedByRule[r] == 0)
            missingRequired.push_back(nativeAssetManifest()[r].pattern);
    if (!missingRequired.empty()) {
        if (missingOut) *missingOut = std::move(missingRequired);
        return IsoError::NotThisGame;
    }

    // Deterministic order: heaviest first (weight = bytes), ties by path —
    // the sibling installer's scheduling precedent (GameBuilder compiles
    // units heaviest first; AUTHORED-for-native).
    std::sort(plan.items.begin(), plan.items.end(), [](const ExtractionPlanItem& a, const ExtractionPlanItem& b) {
        if (a.bytes != b.bytes) return a.bytes > b.bytes;
        return a.path < b.path;
    });

    for (const ExtractionPlanItem& item : plan.items) plan.totalBytes += item.bytes;
    return plan;
}

// ---------------------------------------------------------------- eta

void ThroughputEta::begin(std::uint64_t totalBytes, double nowSeconds) {
    m_total = totalBytes;
    m_phaseStart = nowSeconds;
    m_samples.clear();
}

void ThroughputEta::sample(std::uint64_t doneBytes, double nowSeconds) {
    m_samples.emplace_back(nowSeconds, doneBytes);
    while (m_samples.size() > 1 && nowSeconds - m_samples.front().first > kEtaWindowSeconds)
        m_samples.pop_front();
}

double ThroughputEta::secondsLeft(std::uint64_t doneBytes, double nowSeconds) const {
    if (m_total == 0 || m_samples.empty()) return -1.0;
    if (nowSeconds - m_phaseStart < kEtaWarmupSeconds) return -1.0;
    if (doneBytes * kEtaMinDoneTwentieth < m_total) return -1.0;  // < 5 % done
    const auto& [t0, d0] = m_samples.front();
    if (nowSeconds - t0 < kEtaMinSpanSeconds || doneBytes <= d0) return -1.0;
    const double rate = static_cast<double>(doneBytes - d0) / (nowSeconds - t0);
    if (rate <= 0.0) return -1.0;
    return static_cast<double>(m_total - doneBytes) / rate;
}

std::string ThroughputEta::describe(double seconds) {
    // AUTHORED port of sibling src/platform/ProgressEta.h describe() so the
    // native setup screens say exactly what players already see.
    if (seconds < 0.0) return {};
    if (seconds < 10.0) return "Almost done";
    if (seconds < 55.0)
        return "About " + std::to_string((static_cast<int>(seconds) + 5) / 10 * 10) + " seconds left";
    const int minutes = static_cast<int>(seconds / 60.0 + 0.5);
    return minutes <= 1 ? "About a minute left" : "About " + std::to_string(minutes) + " minutes left";
}

// ------------------------------------------------------------- installed

InstallStatus validateInstalledAssets(const std::filesystem::path& discDir) {
    InstallStatus status;
    std::ifstream in(discDir / ".lbn_map.tsv");
    if (!in) {
        // No map at all: report the full required set (an empty extract dir is
        // not installed and must say what is missing, not nothing).
        for (const ManifestEntrySpec& spec : nativeAssetManifest())
            if (spec.rule == AssetRule::Required)
                status.missingRules.push_back(std::string(spec.pattern) + " (no manifest map)");
        return status;
    }

    struct Row {
        std::string name;
        std::uint64_t bytes;
    };
    std::vector<Row> rows;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const std::size_t t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        const std::size_t t2 = line.find('\t', t1 + 1);
        if (t2 == std::string::npos) continue;
        try {
            std::size_t used = 0;
            const std::uint64_t bytes = std::stoull(line.substr(t2 + 1), &used, 10);
            if (used == 0) continue;
            rows.push_back(Row{line.substr(0, t1), bytes});
        } catch (...) {
            continue;
        }
    }

    status.files = rows.size();
    for (const Row& row : rows) status.bytes += row.bytes;

    std::vector<PatternParts> parts;
    for (const ManifestEntrySpec& spec : nativeAssetManifest()) parts.push_back(splitPattern(spec));

    for (std::size_t r = 0; r < parts.size(); ++r) {
        if (nativeAssetManifest()[r].rule != AssetRule::Required) continue;
        std::size_t matched = 0;
        for (const Row& row : rows) {
            if (!patternMatches(parts[r], toUpperFold(row.name))) continue;
            ++matched;
            std::error_code ec;
            const std::filesystem::path onDisk = discDir / row.name;
            if (!std::filesystem::is_regular_file(onDisk, ec)) {
                status.missingRules.push_back(row.name + " (in the map, not on disk)");
                continue;
            }
            const std::uintmax_t size = std::filesystem::file_size(onDisk, ec);
            if (ec || size != row.bytes) status.missingRules.push_back(row.name + " (size differs)");
        }
        if (matched == 0)
            status.missingRules.push_back(std::string(nativeAssetManifest()[r].pattern) + " (no files matching this rule)");
    }
    status.installed = !rows.empty() && status.missingRules.empty();
    return status;
}

// ---------------------------------------------------------------- flow

FirstRunExtraction::FirstRunExtraction(GateConfig config) : m_config(config) {}

ExtractionSnapshot FirstRunExtraction::snapshot() const {
    std::lock_guard lock(m_mutex);
    return m_snapshot;
}

void FirstRunExtraction::fail(IsoError error, std::string message) {
    std::lock_guard lock(m_mutex);
    m_snapshot.phase = ExtractionPhase::Failed;
    m_snapshot.error = error;
    m_snapshot.message = std::move(message);
    m_snapshot.etaSeconds = -1.0;
}

void FirstRunExtraction::setPhase(ExtractionPhase phase, std::string detail) {
    std::lock_guard lock(m_mutex);
    m_snapshot.phase = phase;
    m_snapshot.currentItem = std::move(detail);
    switch (phase) {
        case ExtractionPhase::Verifying: m_snapshot.stepIndex = 0; break;
        case ExtractionPhase::Extracting: m_snapshot.stepIndex = 1; break;
        case ExtractionPhase::Validating: m_snapshot.stepIndex = 2; break;
        case ExtractionPhase::Ready: m_snapshot.stepIndex = m_snapshot.stepCount - 1; break;
        case ExtractionPhase::Failed:
        case ExtractionPhase::Cancelled: break;
    }
    m_snapshot.fraction = 0.0;
}

void FirstRunExtraction::setFraction(double fraction) {
    std::lock_guard lock(m_mutex);
    m_snapshot.fraction = fraction < 0.0 ? 0.0 : (fraction > 1.0 ? 1.0 : fraction);
}

void FirstRunExtraction::setDetail(std::string detail) {
    std::lock_guard lock(m_mutex);
    m_snapshot.currentItem = std::move(detail);
}

void FirstRunExtraction::pushProgress(std::uint64_t done, std::uint64_t total) {
    const double now = steadySeconds();
    std::lock_guard lock(m_mutex);
    m_eta.sample(done, now);
    m_snapshot.bytesDone = done;
    m_snapshot.bytesTotal = total;
    m_snapshot.fraction = total > 0 ? static_cast<double>(done) / static_cast<double>(total) : 1.0;
    m_snapshot.etaSeconds = m_eta.secondsLeft(done, now);
}

std::string FirstRunExtraction::buildLbnMapText(const std::vector<ExtractionPlanItem>& itemsInWalkOrder) {
    std::string out;
    for (const ExtractionPlanItem& item : itemsInWalkOrder) {
        out += item.path;
        out += '\t';
        out += std::to_string(item.lba);
        out += '\t';
        out += std::to_string(item.bytes);
        out += '\n';
    }
    return out;
}

bool FirstRunExtraction::run(const std::filesystem::path& imagePath, const std::filesystem::path& discDir) {
    // A stop requested before run() cancels it (flag consumed so a plain
    // retry on the same object proceeds).
    if (m_stop.exchange(false, std::memory_order_relaxed)) {
        setPhase(ExtractionPhase::Cancelled, "Cancelled before start");
        return false;
    }
    m_stop.store(false, std::memory_order_relaxed);
    {
        std::lock_guard lock(m_mutex);
        m_snapshot = ExtractionSnapshot{};  // Verifying, step 0 of 3
    }
    setDetail("Checking for an existing extract");

    // Fast path: the disc dir may already hold a complete install (the
    // sibling installer's full-disc extract satisfies the same manifest);
    // then the image is never even opened.
    const InstallStatus installed = validateInstalledAssets(discDir);
    if (installed.installed) {
        std::lock_guard lock(m_mutex);
        m_snapshot.phase = ExtractionPhase::Ready;
        m_snapshot.stepIndex = m_snapshot.stepCount - 1;
        m_snapshot.fraction = 1.0;
        m_snapshot.bytesDone = installed.bytes;
        m_snapshot.bytesTotal = installed.bytes;
        m_snapshot.currentItem = "Assets already installed";
        m_snapshot.etaSeconds = -1.0;
        return true;
    }

    // ---- step 1: the gate chain -----------------------------------------
    setPhase(ExtractionPhase::Verifying, "Opening disc image");
    setFraction(0.02);
    IsoImage image;
    auto volume = image.open(imagePath);
    if (!volume.has_value()) {
        if (volume.error() == IsoError::NotReadable && looksLikeChd(imagePath)) {
            fail(IsoError::NotReadable,
                 "CHD images are not supported yet; extract the disc to a .bin/.iso first "
                 "(docs/native-reference/first-run-extraction.md)");
            return false;
        }
        switch (volume.error()) {
            case IsoError::NotReadable: fail(IsoError::NotReadable, "The disc image could not be read: " + imagePath.string()); return false;
            case IsoError::Io: fail(IsoError::Io, "The disc image could not be read: " + imagePath.string()); return false;
            case IsoError::NotISO: fail(IsoError::NotISO, imagePath.string() + " is not an ISO9660 disc image (no volume descriptor at sector 16)"); return false;
            default: fail(volume.error(), "The disc image could not be opened"); return false;
        }
    }
    setDetail("Verifying this is Road Trip (" + volume->volumeId + ")");
    setFraction(0.06);

    std::uint32_t actualCrc = 0;
    const double gateSpanStart = 0.08;
    auto gate = runGateChain(
        image, m_config,
        [&](GateStage stage, std::uint64_t done, std::uint64_t total) {
            if (stage == GateStage::Located) {
                setDetail("Checking the disc executable");
                setFraction(gateSpanStart);
            } else {
                setFraction(gateSpanStart + 0.84 * (total > 0 ? static_cast<double>(done) / static_cast<double>(total) : 1.0));
            }
        },
        &actualCrc);
    if (!gate.has_value()) {
        switch (gate.error()) {
            case IsoError::NotThisGame:
                fail(IsoError::NotThisGame,
                     "This is not Road Trip: no " + std::string(m_config.bootElfPath) +
                         " at the disc root");
                return false;
            case IsoError::CorruptFile:
                fail(IsoError::CorruptFile,
                     "The disc executable is damaged or from another build (CRC32 " + hex8(actualCrc) +
                         ", expected " + hex8(m_config.expectedBootCrc) + ")");
                return false;
            default:
                fail(gate.error(), "The disc executable could not be read from the image");
                return false;
        }
    }
    setDetail("Reading the disc contents");
    setFraction(0.95);

    auto entries = image.walk();
    if (!entries.has_value()) {
        fail(IsoError::Io, "The disc's directory records could not be read from the image");
        return false;
    }
    std::vector<std::string> missing;
    auto plan = buildExtractionPlan(*entries, &missing);
    if (!plan.has_value()) {
        fail(IsoError::NotThisGame,
             "This disc is missing assets Road Trip needs: " +
                 std::accumulate(missing.begin(), missing.end(), std::string(),
                                 [](const std::string& a, const std::string& b) {
                                     return a.empty() ? b : a + ", " + b;
                                 }));
        return false;
    }
    setFraction(1.0);

    // ---- step 2: weighted extraction ------------------------------------
    setPhase(ExtractionPhase::Extracting, "Extracting Road Trip's assets");
    {
        const double now = steadySeconds();
        std::lock_guard lock(m_mutex);
        m_eta.begin(plan->totalBytes, now);
        m_snapshot.bytesDone = 0;
        m_snapshot.bytesTotal = plan->totalBytes;
        m_snapshot.etaSeconds = -1.0;
    }
    {
        std::error_code ec;
        std::filesystem::create_directories(discDir, ec);
        if (!std::filesystem::is_directory(discDir)) {
            fail(IsoError::Io, "Could not create the asset directory: " + discDir.string());
            return false;
        }
    }
    std::uint64_t planBytesDone = 0;
    for (const ExtractionPlanItem& item : plan->items) {
        if (m_stop.load(std::memory_order_relaxed)) {
            std::lock_guard lock(m_mutex);
            m_snapshot.phase = ExtractionPhase::Cancelled;
            m_snapshot.etaSeconds = -1.0;
            return false;
        }
        setDetail(item.path);
        const std::filesystem::path target = discDir / item.path;
        const std::filesystem::path parent = target.parent_path();
        {
            std::error_code ec;
            if (!parent.empty()) std::filesystem::create_directories(parent, ec);
            if (!parent.empty() && !std::filesystem::is_directory(parent)) {
                fail(IsoError::Io, "Could not create the directory: " + parent.string());
                return false;
            }
        }
        std::FILE* out = std::fopen(target.string().c_str(), "wb");
        if (!out) {
            fail(IsoError::Io, "Could not write: " + target.string());
            return false;
        }
        std::uint64_t itemDone = 0;
        bool writeFailed = false;
        const IsoEntry source{item.imagePath, item.lba, item.bytes, false};
        const bool readOk = image.readFile(
            source,
            [&](const std::uint8_t* data, std::size_t n) {
                if (m_stop.load(std::memory_order_relaxed)) return false;
                if (std::fwrite(data, 1, n, out) != n) {
                    writeFailed = true;
                    return false;
                }
                itemDone += n;
                return true;
            },
            [&](std::uint64_t done, std::uint64_t) {
                pushProgress(planBytesDone + done, plan->totalBytes);
            });
        const bool closeOk = std::fclose(out) == 0;
        if (m_stop.load(std::memory_order_relaxed)) {
            std::lock_guard lock(m_mutex);
            m_snapshot.phase = ExtractionPhase::Cancelled;
            m_snapshot.etaSeconds = -1.0;
            return false;
        }
        if (!readOk || !closeOk) {
            fail(IsoError::Io, writeFailed ? "The destination could not be written: " + target.string()
                                           : "The image ended early while reading: " + item.imagePath);
            return false;
        }
        planBytesDone += item.bytes;
        pushProgress(planBytesDone, plan->totalBytes);
    }

    // Sidecar for native consumers ("name\tlba\tbytes", files only, walk
    // order — the format field_tile_index.cpp prefers and the sibling
    // installer writes).
    {
        std::vector<ExtractionPlanItem> walkOrder = plan->items;
        std::sort(walkOrder.begin(), walkOrder.end(),
                  [](const ExtractionPlanItem& a, const ExtractionPlanItem& b) { return a.walkIndex < b.walkIndex; });
        if (!writeSmallFile(discDir / ".lbn_map.tsv", buildLbnMapText(walkOrder))) {
            fail(IsoError::Io, "Could not write: " + (discDir / ".lbn_map.tsv").string());
            return false;
        }
    }

    // ---- step 3: validate the extracted asset set -----------------------
    setPhase(ExtractionPhase::Validating, "Validating the extracted assets");
    for (std::size_t i = 0; i < plan->items.size(); ++i) {
        const ExtractionPlanItem& item = plan->items[i];
        std::error_code ec;
        const std::filesystem::path onDisk = discDir / item.path;
        if (!std::filesystem::is_regular_file(onDisk, ec)) {
            fail(IsoError::CorruptFile, "Extracted file is missing: " + item.path);
            return false;
        }
        const std::uintmax_t size = std::filesystem::file_size(onDisk, ec);
        if (ec || size != item.bytes) {
            fail(IsoError::CorruptFile,
                 "Extracted file has the wrong size: " + item.path + " (" + std::to_string(size) +
                     " bytes, expected " + std::to_string(item.bytes) + ")");
            return false;
        }
        setDetail(std::to_string(i + 1) + "/" + std::to_string(plan->items.size()));
        setFraction(static_cast<double>(i + 1) / static_cast<double>(plan->items.size()));
    }

    // The flow's own install marker (the sibling's .installed.json is never
    // touched); carries only names, counts and the verified CRC.
    {
        const std::string marker =
            "{\n"
            "  \"source\": \"" + jsonEscape(imagePath.filename().string()) + "\",\n"
            "  \"volume_id\": \"" + jsonEscape(volume->volumeId) + "\",\n"
            "  \"boot_elf\": \"" + jsonEscape(std::string(m_config.bootElfPath)) + "\",\n"
            "  \"boot_crc32\": \"" + hex8(gate->actualCrc) + "\",\n"
            "  \"files\": " + std::to_string(plan->items.size()) + ",\n"
            "  \"bytes\": " + std::to_string(plan->totalBytes) + "\n"
            "}\n";
        if (!writeSmallFile(discDir / ".native_assets.json", marker)) {
            fail(IsoError::Io, "Could not write: " + (discDir / ".native_assets.json").string());
            return false;
        }
    }

    std::lock_guard lock(m_mutex);
    m_snapshot.phase = ExtractionPhase::Ready;
    m_snapshot.fraction = 1.0;
    m_snapshot.stepIndex = m_snapshot.stepCount - 1;
    m_snapshot.currentItem = "Assets ready";
    m_snapshot.etaSeconds = -1.0;
    m_snapshot.bytesDone = plan->totalBytes;
    m_snapshot.bytesTotal = plan->totalBytes;
    return true;
}

}  // namespace rt::native::assets
