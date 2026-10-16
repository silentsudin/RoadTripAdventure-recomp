// Synthetic contract test for src/native/assets/iso_extract.* — the
// first-run ISO9660 reader / verification gate / weighted extractor.
//
// Every image below is a SYNTHETIC ISO9660 fixture built in test code
// (PVD 'CD001' at sector 16, tiny directory extents, pattern-ASCII
// contents); zero game bytes appear anywhere in this file or in the
// fixtures it writes.  CRC32 constants are external-oracle values
// computed with python's zlib.crc32 over the identical fill recipes, so
// the Crc32 implementation is pinned against something outside this
// repo.  Names like "SLUS_203.98" are committed identifier facts, not
// disc content.

#include "../../src/native/assets/iso_extract.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

using rt::native::assets::AssetRule;
using rt::native::assets::Crc32;
using rt::native::assets::ExtractionPhase;
using rt::native::assets::ExtractionSnapshot;
using rt::native::assets::FirstRunExtraction;
using rt::native::assets::GateConfig;
using rt::native::assets::GateStage;
using rt::native::assets::InstallStatus;
using rt::native::assets::IsoEntry;
using rt::native::assets::IsoError;
using rt::native::assets::IsoImage;
using rt::native::assets::IsoOpenOptions;
using rt::native::assets::IsoStride;
using rt::native::assets::ThroughputEta;
using rt::native::assets::kBootElfCrc32;
using rt::native::assets::kBootElfName;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)
#define CHECK_EQ(a, b) do { const long long va_ = static_cast<long long>(a); const long long vb_ = static_cast<long long>(b); if (va_ != vb_) { std::fprintf(stderr, "FAIL %s:%d: %s == %s (got %lld vs %lld)\n", __FILE__, __LINE__, #a, #b, va_, vb_); ++g_fail; } } while (0)

namespace {

// ------------------------------------------------------------- content

std::vector<std::uint8_t> fill(std::string_view pattern, std::size_t size) {
    std::vector<std::uint8_t> out;
    out.reserve(size);
    while (out.size() < size) {
        const std::size_t n = std::min(pattern.size(), size - out.size());
        out.insert(out.end(), pattern.begin(), pattern.begin() + static_cast<std::ptrdiff_t>(n));
    }
    return out;
}

std::string lower(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

// Fixture A contents (recipes mirrored by the python zlib.crc32 oracle).
const std::vector<std::uint8_t> kWelcomeBytes = fill("Road Trip native first-run synthetic welcome.\n", 2880);
const std::vector<std::uint8_t> kBootBytes = fill("SYNTHETIC-NATIVE-BOOT-ELF-NO-GAME-BYTES\n", 6144);
const std::vector<std::uint8_t> kModelBytes = fill("SYNTHETIC-NATIVE-MODEL-PAYLOAD-BYTES\n", 8192);
const std::vector<std::uint8_t> kPageBytes = fill("PAGE", 2048);
const std::vector<std::uint8_t> kGreeterUpperBytes = fill("GREETER-UPPERCASE.", 100);
const std::vector<std::uint8_t> kGreeterLowerBytes = fill("greeter-lowercase.", 200);
const std::vector<std::uint8_t> kCnfBytes = fill("BOOT2 SYNTHETIC SYSTEM\n", 57);

// External-oracle CRC32s (python zlib.crc32 over the identical fill()).
constexpr std::uint32_t kCrcWelcome = 0x4EF5A3C6;
constexpr std::uint32_t kCrcBoot = 0x636BB29C;
constexpr std::uint32_t kCrcModel = 0xE8D3716D;
constexpr std::uint32_t kCrcCnf = 0xD18C6C54;

// Fixture D (game-like skeleton) contents + its gate CRC. "SYNTHETIC-".
const std::vector<std::uint8_t> kDGate = fill("SYNTHETIC-D-GATE-NO-GAME-BYTES\n", 4096);
constexpr std::uint32_t kCrcDGate = 0x85B52F7A;
const std::vector<std::uint8_t> kD2048 = fill("D-CAR-Q-SYNTHETIC\n", 2048);
const std::vector<std::uint8_t> kDTvb = fill("D-SOUND-BGM-TVB-SYNTHETIC\n", 409600);
const std::vector<std::uint8_t> kDVag = fill("D-SOUND-2CH-L-VAG\n", 4096);
const std::vector<std::uint8_t> kDWelcome = fill("D-WELCOME-EXCLUDED\n", 57);

constexpr std::uint64_t kPlanDBytes = 444425;   // 12x2048 + 2000 + 409600 + 3x4096 + 57
constexpr std::size_t kPlanDItems = 17;       // 15 required + 2 optional (ACTION, SHOP)

// ------------------------------------------------------------ binary helpers

void put16(std::vector<std::uint8_t>& out, std::size_t at, std::uint16_t v) {
    out[at] = static_cast<std::uint8_t>(v & 0xFF);
    out[at + 1] = static_cast<std::uint8_t>(v >> 8);
}
void put16be(std::vector<std::uint8_t>& out, std::size_t at, std::uint16_t v) {
    out[at] = static_cast<std::uint8_t>(v >> 8);
    out[at + 1] = static_cast<std::uint8_t>(v & 0xFF);
}
void put32(std::vector<std::uint8_t>& out, std::size_t at, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) out[at + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>((v >> (8 * i)) & 0xFF);
}
void put32be(std::vector<std::uint8_t>& out, std::size_t at, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) out[at + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>((v >> (8 * (3 - i))) & 0xFF);
}

struct RecordSpec {
    std::string name;  // WITH its ";1" version for files (as mastered)
    bool isDir = false;
    std::uint32_t lba = 0;
    std::uint32_t size = 0;
};

std::vector<std::uint8_t> makeDirectoryExtent(const std::vector<RecordSpec>& records) {
    std::vector<std::uint8_t> extent;
    auto append = [&](std::string_view rawName, std::uint32_t lba, std::uint32_t size, std::uint8_t flags) {
        std::size_t len = 33 + rawName.size();
        if (len % 2 != 0) ++len;
        const std::size_t start = extent.size();
        extent.resize(start + len, 0);
        extent[start] = static_cast<std::uint8_t>(len);
        put32(extent, start + 2, lba);       // extent LBA, little-endian
        put32be(extent, start + 6, lba);     // then big-endian
        put32(extent, start + 10, size);     // data length, little-endian
        put32be(extent, start + 14, size);    // then big-endian
        extent[start + 25] = flags;
        put16(extent, start + 28, 1);        // volume sequence number
        put16be(extent, start + 30, 1);
        extent[start + 32] = static_cast<std::uint8_t>(rawName.size());
        std::memcpy(extent.data() + start + 33, rawName.data(), rawName.size());
    };
    append(std::string_view("\0", 1), 0, 0, 0x02);  // '.' self record
    append(std::string_view("\1", 1), 0, 0, 0x02);  // '..' parent record
    for (const RecordSpec& r : records)
        append(r.name, r.lba, r.size, r.isDir ? 0x02 : 0x00);
    extent.resize((extent.size() + 2047) / 2048 * 2048, 0);  // never spans a sector
    return extent;
}

std::vector<std::uint8_t> makePvd(std::uint32_t rootLba, std::uint32_t rootSize,
                                  std::string_view volumeId, std::uint32_t totalSectors) {
    std::vector<std::uint8_t> pvd(2048, 0);
    pvd[0] = 1;                              // primary volume descriptor
    std::memcpy(pvd.data() + 1, "CD001", 5);  // at +1
    pvd[6] = 1;                              // version 1
    for (std::size_t i = 8; i < 40; ++i) pvd[i] = ' ';
    for (std::size_t i = 40; i < 72; ++i) pvd[i] = ' ';
    std::memcpy(pvd.data() + 40, volumeId.data(), std::min<std::size_t>(volumeId.size(), 32));
    put32(pvd, 80, totalSectors);
    put32be(pvd, 84, totalSectors);
    constexpr std::size_t r = 156;  // the root directory record
    pvd[r + 0] = 34;
    put32(pvd, r + 2, rootLba);
    put32be(pvd, r + 6, rootLba);
    put32(pvd, r + 10, rootSize);
    put32be(pvd, r + 14, rootSize);
    pvd[r + 25] = 0x02;  // directory flag
    put16(pvd, r + 28, 1);
    put16be(pvd, r + 30, 1);
    pvd[r + 32] = 1;
    pvd[r + 33] = 0;
    return pvd;
}

std::vector<std::uint8_t> makeTerminator() {
    std::vector<std::uint8_t> out(2048, 0);
    out[0] = 255;
    std::memcpy(out.data() + 1, "CD001", 5);
    out[6] = 1;
    return out;
}

class IsoBuilder {
public:
    void setSector(std::uint32_t lba, const std::vector<std::uint8_t>& sector) {
        if (sector.size() != 2048) return;
        const std::size_t need = (static_cast<std::size_t>(lba) + 1) * 2048;
        if (m_sectors.size() < need) m_sectors.resize(need, 0);
        std::memcpy(m_sectors.data() + static_cast<std::size_t>(lba) * 2048, sector.data(), 2048);
    }
    void setRange(std::uint32_t firstLba, const std::vector<std::uint8_t>& bytes) {
        const std::uint32_t sectors = static_cast<std::uint32_t>((bytes.size() + 2047) / 2048);
        std::vector<std::uint8_t> sector(2048, 0);
        for (std::uint32_t i = 0; i < sectors; ++i) {
            if (!bytes.empty()) {
                sector.assign(2048, 0);
                const std::size_t src = static_cast<std::size_t>(i) * 2048;
                const std::size_t n = std::min<std::size_t>(2048, bytes.size() - src);
                if (n != 0) std::memcpy(sector.data(), bytes.data() + src, n);
            }
            setSector(firstLba + i, sector);
        }
    }
    std::uint32_t totalSectors() const { return static_cast<std::uint32_t>(m_sectors.size() / 2048); }
    std::vector<std::uint8_t> buildCooked() const { return m_sectors; }

    // Raw 2352-byte sectors: 12 sync bytes, 4 header (mode byte last),
    // 8 subheader (Mode 2 only), 2048 user bytes, zero padding.
    // Mode 2 data at +24 (the PS2 CD form), Mode 1 at +16.
    std::vector<std::uint8_t> buildRaw2352(std::uint8_t mode) const {
        const std::size_t dataOffset = mode == 2 ? 24 : 16;
        std::vector<std::uint8_t> out;
        out.reserve(static_cast<std::size_t>(totalSectors()) * 2352);
        static const std::uint8_t kSync[12] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                               0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
        std::vector<std::uint8_t> sector(2352, 0);
        for (std::uint32_t s = 0; s < totalSectors(); ++s) {
            sector.assign(2352, 0);
            std::memcpy(sector.data(), kSync, 12);
            sector[15] = mode;
            std::memcpy(sector.data() + dataOffset,
                        m_sectors.data() + static_cast<std::size_t>(s) * 2048, 2048);
            out.insert(out.end(), sector.begin(), sector.end());
        }
        return out;
    }

private:
    std::vector<std::uint8_t> m_sectors;
};

// ---------------------------------------------------------- fixture A

struct FixtureA {
    IsoBuilder builder;
    // Assembly pass-throughs used by the test bodies below.
    std::vector<std::uint8_t> buildCooked() const { return builder.buildCooked(); }
    std::vector<std::uint8_t> buildRaw2352(std::uint8_t mode) const { return builder.buildRaw2352(mode); }
    FixtureA() {
        // LBAs: root dir 18, DATA dir 19, WELCOME 20-21, SLUS 22-24,
        // MODEL 25-28, PAGE 29, GREETER 30, greeter 31, EMPTY 32, CNF 33.
        builder.setSector(16, makePvd(18, 2048, "RT-NATIVE-TEST", 34));
        builder.setSector(17, makeTerminator());
        builder.setSector(18, makeDirectoryExtent(std::vector<RecordSpec>{
                                 {"WELCOME.TXT;1", false, 20, 2880},
                                 {"SLUS_203.98;1", false, 22, 6144},
                                 {"DATA", true, 19, 2048},
                                 {"EMPTY.DAT;1", false, 32, 0},
                                 {"SYSTEM.CNF;1", false, 33, 57},
                             }));
        builder.setSector(19, makeDirectoryExtent(std::vector<RecordSpec>{
                                 {"GREETER.TXT;1", false, 30, 100},
                                 {"greeter.txt;1", false, 31, 200},
                                 {"MODEL.BIN;1", false, 25, 8192},
                                 {"PAGE.GSL;1", false, 29, 2048},
                             }));
        builder.setRange(20, kWelcomeBytes);
        builder.setRange(22, kBootBytes);
        builder.setRange(25, kModelBytes);
        builder.setRange(29, kPageBytes);
        builder.setRange(30, kGreeterUpperBytes);
        builder.setRange(31, kGreeterLowerBytes);
        builder.setRange(33, kCnfBytes);
    }
};

// ---------------------------------------------------------- fixture D / E
// A game-like folder skeleton driven by the real manifest: one stub file
// per manifest rule, an unmanifested WELCOME.TXT, and no CAR5. `lowercase`
// builds the same disc as mastered with lowercase identifiers (the PS2
// dump variant this reader must fold).

IsoBuilder buildGameLikeFixture(bool lowercase) {
    IsoBuilder b;
    const auto rel = [&](const std::vector<RecordSpec>& records) {
        std::vector<RecordSpec> out;
        out.reserve(records.size());
        for (const RecordSpec& r : records) {
            RecordSpec s = r;
            if (lowercase) std::transform(s.name.begin(), s.name.end(), s.name.begin(),
                                          [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            out.push_back(s);
        }
        return out;
    };
    b.setSector(16, makePvd(18, 2048, lowercase ? "rt-native-dump" : "RT-NATIVE-DUMP", 251));
    b.setSector(17, makeTerminator());
    b.setSector(18, makeDirectoryExtent(rel(std::vector<RecordSpec>{
                        {"SLUS_203.98;1", false, 19, 4096},
                        {"WELCOME.TXT;1", false, 249, 57},  // on disc, outside the manifest
                        {"SYSTEM.CNF;1", false, 250, 57},
                        {"SYS", true, 21, 2048},
                        {"FLD", true, 22, 2048},
                        {"COURSE", true, 23, 2048},
                        {"CAR0", true, 24, 2048},
                        {"CAR1", true, 25, 2048},
                        {"CAR2", true, 26, 2048},
                        {"CAR3", true, 27, 2048},
                        {"CAR4", true, 28, 2048},
                        {"CARS", true, 29, 2048},
                        {"ITEM", true, 30, 2048},
                        {"SOUND", true, 31, 2048},
                        {"ACTION", true, 32, 2048},
                        {"SHOP", true, 33, 2048},
                    })));
    b.setSector(21, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"SAMPLE.GSL;1", false, 34, 2048}})));
    b.setSector(22, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"123.BIN;1", false, 35, 2048}})));
    b.setSector(23, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"C07.BIN;1", false, 36, 2048}})));
    b.setSector(24, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"Q18.BIN;1", false, 37, 2048}})));
    b.setSector(25, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"Q48.BIN;1", false, 38, 2048}})));
    b.setSector(26, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"Q78.BIN;1", false, 39, 2048}})));
    b.setSector(27, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"Q108.BIN;1", false, 40, 2048}})));
    b.setSector(28, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"Q138.BIN;1", false, 41, 2048}})));
    b.setSector(29, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"TIRE.BIN;1", false, 42, 2048}})));
    b.setSector(30, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"METER3.GSL;1", false, 43, 2048}})));
    b.setSector(31, makeDirectoryExtent(rel(std::vector<RecordSpec>{
                        {"BGM_03.TSQ;1", false, 44, 2000},
                        {"BGM.TVB;1", false, 45, 409600},
                        {"2CH_L.VAG;1", false, 245, 4096},
                    })));
    b.setSector(32, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"A09.BIN;1", false, 247, 2048}})));
    b.setSector(33, makeDirectoryExtent(rel(std::vector<RecordSpec>{{"T04.BIN;1", false, 248, 2048}})));
    b.setRange(19, kDGate);
    const auto pattern2048 = [](const char* p) { return fill(p, 2048); };
    b.setRange(34, pattern2048("D-SYS-SAMPLE-GSL-SYNTHETIC\n"));
    b.setRange(35, pattern2048("D-FLD-123-SYNTHETIC\n"));
    b.setRange(36, pattern2048("D-COURSE-C07-SYNTHETIC\n"));
    b.setRange(37, kD2048);  // CAR0/Q18.BIN
    b.setRange(38, kD2048);  // CAR1/Q48.BIN
    b.setRange(39, kD2048);  // CAR2/Q78.BIN
    b.setRange(40, kD2048);  // CAR3/Q108.BIN
    b.setRange(41, kD2048);  // CAR4/Q138.BIN
    b.setRange(42, pattern2048("D-CARS-TIRE-SYNTHETIC\n"));
    b.setRange(43, pattern2048("D-ITEM-METER3-SYNTHETIC\n"));
    b.setRange(44, fill("D-SOUND-BGM-03-TSQ\n", 2000));
    b.setRange(45, kDTvb);
    b.setRange(245, kDVag);
    b.setRange(247, pattern2048("D-ACTION-A09-SYNTHETIC\n"));
    b.setRange(248, pattern2048("D-SHOP-T04-SYNTHETIC\n"));
    b.setRange(249, kDWelcome);
    b.setRange(250, fill("D-SYSTEM-CNF-SYNTHETIC\n", 57));
    return b;
}

// ---------------------------------------------------------- fixture F

struct FixtureF {
    IsoBuilder builder;
    std::vector<std::uint8_t> buildCooked() const { return builder.buildCooked(); }
    std::vector<std::uint8_t> buildRaw2352(std::uint8_t mode) const { return builder.buildRaw2352(mode); }
    FixtureF() {
        builder.setSector(16, makePvd(18, 2048, "OTHERGAME", 21));
        builder.setSector(17, makeTerminator());
        builder.setSector(18, makeDirectoryExtent(std::vector<RecordSpec>{
                                 {"WELCOME.TXT;1", false, 20, 2880},
                             }));
        builder.setRange(20, kWelcomeBytes);
    }
};

// ---------------------------------------------------------- temp helpers

std::filesystem::path g_tempDir;

std::filesystem::path writeTempFile(const std::string& name, const std::vector<std::uint8_t>& bytes) {
    std::filesystem::create_directories(g_tempDir);
    const std::filesystem::path out = g_tempDir / name;
    std::FILE* f = std::fopen(out.string().c_str(), "wb");
    CHECK(f != nullptr);
    if (f) {
        if (!bytes.empty()) CHECK(std::fwrite(bytes.data(), 1, bytes.size(), f) == bytes.size());
        std::fclose(f);
    }
    return out;
}

std::vector<std::uint8_t> readAllFileBytes(const std::filesystem::path& p) {
    std::ifstream in(p, std::ios::binary);
    return std::vector<std::uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

std::string readAllFileText(const std::filesystem::path& p) {
    std::ifstream in(p);
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> out;
    std::size_t start = 0;
    while (start < text.size()) {
        const std::size_t end = text.find('\n', start);
        if (end == std::string::npos) {
            out.push_back(text.substr(start));
            break;
        }
        out.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    return out;
}

std::filesystem::path makeEmptyExtractDir(const std::string& name) {
    const auto dir = g_tempDir / name;
    std::error_code ec;
    std::filesystem::remove_all(dir, ec);
    std::filesystem::create_directories(dir, ec);
    return dir;
}

struct ExpectedEntry {
    const char* name;
    std::uint32_t lba;
    std::uint64_t bytes;
    bool isDir;
};
bool matchesEntry(const IsoEntry& e, const ExpectedEntry& x) {
    return e.name == x.name && e.lba == x.lba && e.bytes == x.bytes && e.isDir == x.isDir;
}

// =========================================================== tests

void testCrc32AgainstExternalOracle() {
    Crc32 crc;
    crc.update(kWelcomeBytes.data(), kWelcomeBytes.size());
    CHECK_EQ(crc.value(), kCrcWelcome);

    // Streamed (split) updates converge to the same value.
    Crc32 split;
    const std::size_t half = kBootBytes.size() / 2;
    split.update(kBootBytes.data(), 1);
    split.update(kBootBytes.data() + 1, half - 1);
    split.update(kBootBytes.data() + half, kBootBytes.size() - half);
    CHECK_EQ(split.value(), kCrcBoot);

    Crc32 empty;  // CRC32 of no bytes is 0
    CHECK_EQ(empty.value(), 0x00000000u);

    static const std::uint8_t kProbe[] = "123456789";  // the canonical check value
    Crc32 canonical;
    canonical.update(kProbe, 9);
    CHECK_EQ(canonical.value(), 0xCBF43926u);

    Crc32 d;
    d.update(kDGate.data(), kDGate.size());
    CHECK_EQ(d.value(), kCrcDGate);
}

void testOpenPvdAndWalkCooked() {
    FixtureA a;
    const auto image = writeTempFile("fixture-a.iso", a.buildCooked());
    IsoImage iso;
    auto volume = iso.open(image);
    CHECK(volume.has_value());
    CHECK(volume->volumeId == "RT-NATIVE-TEST");
    CHECK(volume->stride == IsoStride::Cooked2048);
    CHECK_EQ(volume->volumeSpaceSectors, 34u);
    CHECK_EQ(volume->root.lba, 18u);
    CHECK_EQ(volume->root.bytes, 2048u);
    CHECK(volume->root.isDir);
    CHECK(iso.isOpen());

    auto entries = iso.walk();
    CHECK(entries.has_value());
    const ExpectedEntry expected[] = {
        {"WELCOME.TXT", 20, 2880, false},      // root: record-extent order
        {"SLUS_203.98", 22, 6144, false},       // ";1" stripped by the walk
        {"DATA", 19, 2048, true},
        {"EMPTY.DAT", 32, 0, false},
        {"SYSTEM.CNF", 33, 57, false},
        {"DATA/GREETER.TXT", 30, 100, false},  // subdirectory after the root's records
        {"DATA/greeter.txt", 31, 200, false},
        {"DATA/MODEL.BIN", 25, 8192, false},
        {"DATA/PAGE.GSL", 29, 2048, false},
    };
    CHECK_EQ(entries->size(), static_cast<std::size_t>(sizeof(expected) / sizeof(expected[0])));
    for (std::size_t i = 0; i < entries->size(); ++i) CHECK(matchesEntry((*entries)[i], expected[i]));
}

void testLocateExactAndFolded() {
    FixtureA a;
    const auto image = writeTempFile("fixture-a.iso", a.buildCooked());
    IsoImage iso;
    CHECK(iso.open(image).has_value());

    auto exact = iso.locate("DATA/MODEL.BIN");
    CHECK(exact.has_value());
    CHECK_EQ(exact->lba, 25u);
    CHECK_EQ(exact->bytes, 8192u);

    auto folded = iso.locate("data/model.bin");
    CHECK(folded.has_value());
    CHECK_EQ(folded->lba, 25u);
    CHECK_EQ(folded->bytes, 8192u);

    // Exact-case preference: GREETER.TXT and greeter.txt both exist; the
    // exact spelling wins for both queries, and a case-mixed query with no
    // exact match falls back to the first fold match in walk order.
    auto upper = iso.locate("DATA/GREETER.TXT");
    CHECK(upper.has_value());
    CHECK_EQ(upper->lba, 30u);
    CHECK_EQ(upper->bytes, 100u);
    auto lowerEntry = iso.locate("data/greeter.txt");
    CHECK(lowerEntry.has_value());
    CHECK_EQ(lowerEntry->lba, 31u);
    CHECK_EQ(lowerEntry->bytes, 200u);
    auto mixed = iso.locate("Data/Greeter.txt");
    CHECK(mixed.has_value());
    CHECK_EQ(mixed->lba, 30u);
    CHECK_EQ(mixed->bytes, 100u);

    auto missing = iso.locate("NO/SUCH.FILE");
    CHECK(!missing.has_value());
    CHECK_EQ(missing.error(), IsoError::NotThisGame);

    auto zero = iso.locate("EMPTY.DAT");
    CHECK(zero.has_value());
    CHECK_EQ(zero->bytes, 0u);
}

void testReadFileByteExactAndProgress() {
    FixtureA a;
    const auto image = writeTempFile("fixture-a.iso", a.buildCooked());
    IsoImage iso;
    CHECK(iso.open(image).has_value());

    auto welcome = iso.locate("WELCOME.TXT");
    CHECK(welcome.has_value());
    std::vector<std::uint8_t> got;
    CHECK(iso.readFile(*welcome, [&](const std::uint8_t* data, std::size_t n) {
        got.insert(got.end(), data, data + n);
        return true;
    }));
    CHECK(got == kWelcomeBytes);
    Crc32 welcomeCrc;
    welcomeCrc.update(got.data(), got.size());
    CHECK_EQ(welcomeCrc.value(), kCrcWelcome);

    // External oracle for the PAGE.GSL recipe too (zlib.crc32).
    Crc32 pageCrc;
    pageCrc.update(kPageBytes.data(), kPageBytes.size());
    CHECK_EQ(pageCrc.value(), 0xCA21E4EA);

    // Chunked progress is monotone and reaches the exact size.
    auto model = iso.locate("DATA/MODEL.BIN");
    CHECK(model.has_value());
    std::uint64_t last = 0;
    int calls = 0;
    std::vector<std::uint8_t> modelBytes;
    CHECK(iso.readFile(*model,
                      [&](const std::uint8_t* data, std::size_t n) {
                          modelBytes.insert(modelBytes.end(), data, data + n);
                          return true;
                      },
                      [&](std::uint64_t done, std::uint64_t total) {
                          CHECK(done >= last);
                          CHECK_EQ(total, model->bytes);
                          last = done;
                          ++calls;
                      },
                      1 /* chunkSectors: one callback per sector */));
    CHECK(modelBytes == kModelBytes);
    CHECK_EQ(last, 8192u);
    CHECK_EQ(calls, 4);
    Crc32 modelCrc;
    modelCrc.update(modelBytes.data(), modelBytes.size());
    CHECK_EQ(modelCrc.value(), kCrcModel);

    // An empty file streams nothing and reports success.
    auto empty = iso.locate("EMPTY.DAT");
    CHECK(empty.has_value());
    bool sinkCalled = false;
    CHECK(iso.readFile(*empty, [&](const std::uint8_t*, std::size_t) {
        sinkCalled = true;
        return true;
    }));
    CHECK(!sinkCalled);
}

void testRaw2352StrideDetection() {
    FixtureA a;
    const auto rawMode2 = writeTempFile("fixture-b-mode2.bin", a.buildRaw2352(2));
    IsoImage iso;
    auto volume = iso.open(rawMode2);
    CHECK(volume.has_value());
    CHECK(volume->stride == IsoStride::RawMode2);
    CHECK(volume->volumeId == "RT-NATIVE-TEST");
    auto model = iso.locate("data/model.bin");
    CHECK(model.has_value());
    std::vector<std::uint8_t> got;
    CHECK(iso.readFile(*model, [&](const std::uint8_t* data, std::size_t n) {
        got.insert(got.end(), data, data + n);
        return true;
    }));
    CHECK(got == kModelBytes);

    // Strict mode (loose raw mode off) refuses raw sectors.
    IsoImage strict;
    auto refused = strict.open(rawMode2, IsoOpenOptions{.allowRawBin = false, .allowCue = true});
    CHECK(!refused.has_value());
    CHECK_EQ(refused.error(), IsoError::NotISO);

    // Mode 1 raw .bin: data at +16, still byte-exact.
    const auto rawMode1 = writeTempFile("fixture-b-mode1.bin", a.buildRaw2352(1));
    IsoImage iso1;
    auto volume1 = iso1.open(rawMode1);
    CHECK(volume1.has_value());
    CHECK(volume1->stride == IsoStride::RawMode1);
    auto welcome1 = iso1.locate("WELCOME.TXT");
    CHECK(welcome1.has_value());
    std::vector<std::uint8_t> got1;
    CHECK(iso1.readFile(*welcome1, [&](const std::uint8_t* data, std::size_t n) {
        got1.insert(got1.end(), data, data + n);
        return true;
    }));
    CHECK(got1 == kWelcomeBytes);
}

void testCueSheetFollowsFileLine() {
    FixtureA a;
    writeTempFile("fixture-cue-track.bin", a.buildRaw2352(2));
    const std::filesystem::path cue = g_tempDir / "fixture-cue-track.cue";
    std::FILE* f = std::fopen(cue.string().c_str(), "wb");
    CHECK(f != nullptr);
    if (f) {
        std::fprintf(f, "FILE \"fixture-cue-track.bin\" BINARY\n  TRACK 01 MODE2/2352\n    INDEX 01 00:00:00\n");
        std::fclose(f);
    }

    IsoImage iso;
    auto volume = iso.open(cue);
    CHECK(volume.has_value());
    CHECK(volume->stride == IsoStride::RawMode2);

    // A cue with no FILE line reads as NotReadable.
    const auto badCue = writeTempFile("empty.cue", std::vector<std::uint8_t>(64, ' '));
    IsoImage bad;
    auto badVolume = bad.open(badCue);
    CHECK(!badVolume.has_value());
    CHECK_EQ(badVolume.error(), IsoError::NotReadable);
}

void testOpenErrorTypes() {
    IsoImage missing;
    auto missingResult = missing.open(g_tempDir / "does-not-exist-anywhere.iso");
    CHECK(!missingResult.has_value());
    CHECK_EQ(missingResult.error(), IsoError::NotReadable);

    const auto emptyFile = writeTempFile("empty.iso", {});
    IsoImage empty;
    auto emptyResult = empty.open(emptyFile);
    CHECK(!emptyResult.has_value());
    CHECK_EQ(emptyResult.error(), IsoError::NotReadable);

    const auto garbage = writeTempFile("garbage.bin", std::vector<std::uint8_t>(30 * 2048, 0xAA));
    IsoImage junk;
    auto junkResult = junk.open(garbage);
    CHECK(!junkResult.has_value());
    CHECK_EQ(junkResult.error(), IsoError::NotISO);

    // PVD bytes present at sector 16 but the root record is unusable
    // (root LBA 0): NotISO, not a successful open.
    auto bytes = std::vector<std::uint8_t>(20 * 2048, 0);
    const auto pvd = makePvd(0, 2048, "BAD-ROOT", 20);
    std::memcpy(bytes.data() + 16 * 2048, pvd.data(), 2048);
    const auto badRootFile = writeTempFile("bad-root.iso", bytes);
    IsoImage badRoot;
    auto badRootResult = badRoot.open(badRootFile);
    CHECK(!badRootResult.has_value());
    CHECK_EQ(badRootResult.error(), IsoError::NotISO);

    // CHD magic: refused up-front as NotReadable.
    std::vector<std::uint8_t> chdBytes(1u << 20, 0);
    std::memcpy(chdBytes.data(), "MComprHD", 8);
    const auto chdFile = writeTempFile("synthetic.chd", chdBytes);
    IsoImage chd;
    auto chdResult = chd.open(chdFile);
    CHECK(!chdResult.has_value());
    CHECK_EQ(chdResult.error(), IsoError::NotReadable);
}

void testGateChainPassFailAndTyping() {
    FixtureA a;
    const auto image = writeTempFile("fixture-a.iso", a.buildCooked());

    // Pass: the gate file's CRC matches the injected expectation.
    {
        IsoImage iso;
        CHECK(iso.open(image).has_value());
        GateConfig config;
        config.expectedBootCrc = kCrcBoot;  // injectable; production default pinned below
        CHECK_EQ(GateConfig{}.expectedBootCrc, kBootElfCrc32);
        std::vector<GateStage> stages;
        std::vector<std::uint64_t> hashDone;
        std::uint32_t actual = 0;
        auto gate = rt::native::assets::runGateChain(iso, config,
                                                     [&](GateStage stage, std::uint64_t done, std::uint64_t) {
                                                         if (stages.empty() || stages.back() != stage)
                                                             stages.push_back(stage);
                                                         if (stage == GateStage::Hashing) hashDone.push_back(done);
                                                     },
                                                     &actual);
        CHECK(gate.has_value());
        CHECK_EQ(actual, kCrcBoot);
        CHECK_EQ(gate->actualCrc, kCrcBoot);
        CHECK_EQ(gate->bootElf.lba, 22u);
        CHECK_EQ(gate->bootElf.bytes, 6144u);
        CHECK(stages.size() >= 2 && stages[0] == GateStage::Located && stages[1] == GateStage::Hashing);
        for (std::size_t i = 1; i < hashDone.size(); ++i) CHECK(hashDone[i] >= hashDone[i - 1]);
        CHECK_EQ(hashDone.back(), 6144u);
    }

    // Fail: wrong expected CRC -> CorruptFile, the actual still reported.
    {
        IsoImage iso;
        CHECK(iso.open(image).has_value());
        GateConfig wantOther;
        wantOther.expectedBootCrc = kCrcBoot ^ 0x1u;
        std::uint32_t actual = 0;
        auto gate = rt::native::assets::runGateChain(iso, wantOther, {}, &actual);
        CHECK(!gate.has_value());
        CHECK_EQ(gate.error(), IsoError::CorruptFile);
        CHECK_EQ(actual, kCrcBoot);
    }

    // Wrong game (no gate file at the expected path): NotThisGame.
    {
        FixtureF f;
        const auto other = writeTempFile("fixture-f.iso", f.buildCooked());
        IsoImage iso;
        CHECK(iso.open(other).has_value());
        auto gate = rt::native::assets::runGateChain(iso, GateConfig{}, {});
        CHECK(!gate.has_value());
        CHECK_EQ(gate.error(), IsoError::NotThisGame);
    }

    // Truncated image (the gate file's third sector is cut away): Io.
    {
        auto truncated = a.buildCooked();
        truncated.resize(23 * 2048 + 1000);  // SLUS_203.98 spans 22..24
        const auto cut = writeTempFile("truncated.iso", truncated);
        IsoImage iso;
        CHECK(iso.open(cut).has_value());
        auto gate = rt::native::assets::runGateChain(iso, GateConfig{}, {});
        CHECK(!gate.has_value());
        CHECK_EQ(gate.error(), IsoError::Io);
    }
}

void testPlanRequiredMissing() {
    FixtureA a;
    const auto image = writeTempFile("fixture-a.iso", a.buildCooked());
    IsoImage iso;
    CHECK(iso.open(image).has_value());
    auto entries = iso.walk();
    CHECK(entries.has_value());

    std::vector<std::string> missing;
    auto plan = rt::native::assets::buildExtractionPlan(*entries, &missing);
    CHECK(!plan.has_value());
    CHECK_EQ(plan.error(), IsoError::NotThisGame);
    bool sawSys = false;
    for (const std::string& m : missing) sawSys = sawSys || m == "SYS";
    CHECK(sawSys);
    CHECK_EQ(missing.size(), 13u);  // SYS, FLD, COURSE, CAR0..CAR4, CARS, ITEM, SOUND/*.TSQ, SOUND/*.TVB, SOUND/*.VAG
}

void testPlanOnGameLikeFixture() {
    const auto upperImage = writeTempFile("fixture-d.iso", buildGameLikeFixture(false).buildCooked());
    const auto lowerImage = writeTempFile("fixture-e.iso", buildGameLikeFixture(true).buildCooked());

    auto planFor = [&](const std::filesystem::path& path) {
        IsoImage iso;
        CHECK(iso.open(path).has_value());
        auto entries = iso.walk();
        CHECK(entries.has_value());
        std::vector<std::string> missing;
        auto plan = rt::native::assets::buildExtractionPlan(*entries, &missing);
        if (!plan.has_value()) {
            std::fprintf(stderr, "plan error for %s: %d missing=%zu:", path.c_str(),
                        static_cast<int>(plan.error()), missing.size());
            for (const std::string& m : missing) std::fprintf(stderr, " %s", m.c_str());
            std::fprintf(stderr, "\n");
        }
        CHECK(plan.has_value());
        return *plan;
    };
    const auto upper = planFor(upperImage);
    const auto lower = planFor(lowerImage);

    CHECK_EQ(upper.items.size(), kPlanDItems);
    CHECK_EQ(upper.totalBytes, kPlanDBytes);

    // Weighted heaviest-first order, deterministic ties by path: TVB (400 KB)
    // first, then the 4096 pair (SLUS_203.98 sorts before SOUND/2CH_L.VAG).
    CHECK(upper.items[0].path == "SOUND/BGM.TVB");
    CHECK_EQ(upper.items[0].bytes, 409600u);
    CHECK(upper.items[1].path == "SLUS_203.98");
    CHECK(upper.items[2].path == "SOUND/2CH_L.VAG");
    CHECK(upper.items.back().path == "SYSTEM.CNF");
    CHECK_EQ(upper.items.back().bytes, 57u);
    std::uint64_t sum = 0;
    for (std::size_t i = 0; i < upper.items.size(); ++i) {
        if (i + 1 < upper.items.size())
            CHECK(upper.items[i].bytes >= upper.items[i + 1].bytes);
        sum += upper.items[i].bytes;
    }
    CHECK_EQ(sum, upper.totalBytes);

    // WELCOME.TXT is on the disc but outside the manifest: excluded.
    for (const auto& item : upper.items) CHECK(item.path != "WELCOME.TXT");

    // Lowercase mastering canonicalizes to the very same plan paths, while
    // the on-disc spellings stay as mastered.
    CHECK_EQ(lower.items.size(), upper.items.size());
    for (std::size_t i = 0; i < upper.items.size(); ++i) {
        CHECK(lower.items[i].path == upper.items[i].path);
        CHECK_EQ(lower.items[i].bytes, upper.items[i].bytes);
    }
    CHECK(lower.items[0].imagePath == "sound/bgm.tvb");
    CHECK(lower.items[0].path == "SOUND/BGM.TVB");
    bool sawLowerGate = false;
    for (const auto& item : lower.items)
        sawLowerGate = sawLowerGate || item.path == "SLUS_203.98";
    CHECK(sawLowerGate);
}

void testThroughputEtaSyntheticThroughput() {
    ThroughputEta eta;
    eta.begin(100, 0.0);
    CHECK_EQ(eta.total(), 100u);

    eta.sample(0, 0.5);
    eta.sample(20, 2.5);
    CHECK(eta.secondsLeft(20, 2.5) < 0.0);  // still inside the 3 s warm-up

    eta.sample(40, 4.5);
    const double etaA = eta.secondsLeft(40, 4.5);  // rate 10/s over 4 s -> 6 s left
    CHECK(etaA > 5.9 && etaA < 6.1);

    eta.sample(60, 7.5);
    const double etaB = eta.secondsLeft(60, 7.5);
    CHECK(etaB > 0.0 && etaB < etaA);  // at a steady rate the estimate only falls

    // Window pruning + span rebuild: samples older than 40 s drop out (the
    // ones above collapse the span until a fresh pair builds up).  Re-begin
    // at the 1000-byte total this sub-case reasons about.
    eta.begin(1000, 47.0);  // warm: 3 s phase elapsed by t = 50
    eta.sample(500, 50.0);
    CHECK(eta.secondsLeft(500, 50.0) < 0.0);
    eta.sample(900, 51.0);
    const double etaC = eta.secondsLeft(900, 51.0);  // rate 400/s over 1 s
    CHECK(etaC > 0.24 && etaC < 0.26);
}

void testEtaDescribeStrings() {
    CHECK(ThroughputEta::describe(-1.0) == "");
    CHECK(ThroughputEta::describe(5.0) == "Almost done");
    CHECK(ThroughputEta::describe(30.0) == "About 30 seconds left");
    CHECK(ThroughputEta::describe(54.0) == "About 50 seconds left");
    CHECK(ThroughputEta::describe(60.0) == "About a minute left");
    CHECK(ThroughputEta::describe(183.0) == "About 3 minutes left");
}

void testManifestIsDataDriven() {
    const auto& manifest = rt::native::assets::nativeAssetManifest();
    CHECK(manifest.size() >= 18);
    int required = 0, optional = 0;
    bool sawSlus = false, sawCar5 = false, sawSoundTsq = false;
    for (const auto& spec : manifest) {
        CHECK(spec.pattern != nullptr && *spec.pattern != '\0');
        CHECK(spec.provenance != nullptr && *spec.provenance != '\0');
        if (spec.rule == AssetRule::Required)
            ++required;
        else
            ++optional;
        sawSlus = sawSlus || std::string_view(spec.pattern) == kBootElfName;
        sawCar5 = sawCar5 || std::string_view(spec.pattern) == "CAR5";
        sawSoundTsq = sawSoundTsq || std::string_view(spec.pattern) == "SOUND/*.TSQ";
    }
    CHECK(sawSlus);       // the gate
    CHECK(sawCar5);       // tolerated absent (the good dump has no CAR5)
    CHECK(sawSoundTsq);   // extension subset rule
    CHECK(required >= 13);
    CHECK(optional >= 3);
}

void testFlowFullRunSnapshot() {
    const auto image = writeTempFile("flow-d.iso", buildGameLikeFixture(false).buildCooked());
    const auto extractDir = makeEmptyExtractDir("flow-d-extract");

    GateConfig config;
    config.expectedBootCrc = kCrcDGate;  // injectable gate for the synthetic fixture
    FirstRunExtraction flow(config);
    CHECK(flow.run(image, extractDir));
    const ExtractionSnapshot done = flow.snapshot();
    CHECK(done.phase == ExtractionPhase::Ready);
    CHECK_EQ(done.stepCount, 3);
    CHECK_EQ(done.stepIndex, 2);
    CHECK(done.fraction > 0.999 && done.fraction <= 1.0);
    CHECK_EQ(done.bytesDone, kPlanDBytes);
    CHECK_EQ(done.bytesTotal, kPlanDBytes);
    CHECK_EQ(done.etaSeconds, -1.0);
}

void testFlowExtractedBytesSidecarAndMarker() {
    // Lowercase mastering end-to-end: canonical target paths, byte-exact
    // contents, the sidecar and the install marker.
    const auto image = writeTempFile("flow-e.iso", buildGameLikeFixture(true).buildCooked());
    const auto extractDir = makeEmptyExtractDir("flow-e-extract");

    GateConfig config;
    config.expectedBootCrc = kCrcDGate;
    FirstRunExtraction flow(config);
    CHECK(flow.run(image, extractDir));
    CHECK(flow.snapshot().phase == ExtractionPhase::Ready);

    CHECK(std::filesystem::is_regular_file(extractDir / "SYS" / "SAMPLE.GSL"));
    CHECK(std::filesystem::is_regular_file(extractDir / "SOUND" / "BGM.TVB"));
    CHECK(std::filesystem::is_regular_file(extractDir / "SOUND" / "BGM_03.TSQ"));
    CHECK(std::filesystem::is_regular_file(extractDir / "SOUND" / "2CH_L.VAG"));
    CHECK(std::filesystem::is_regular_file(extractDir / "SLUS_203.98"));
    CHECK(std::filesystem::is_regular_file(extractDir / "CAR0" / "Q18.BIN"));

    // Byte-exact content through the full record walk + raw stride path.
    CHECK(readAllFileBytes(extractDir / "SOUND" / "BGM.TVB") == kDTvb);
    CHECK(readAllFileBytes(extractDir / "SLUS_203.98") == kDGate);

    // The unmanifested root file is NOT extracted.
    CHECK(!std::filesystem::exists(extractDir / "WELCOME.TXT"));

    // Sidecar: one row per extracted file, walk order, sibling format.
    const std::string mapText = readAllFileText(extractDir / ".lbn_map.tsv");
    const std::vector<std::string> rows = splitLines(mapText);
    CHECK_EQ(rows.size(), kPlanDItems);
    CHECK(rows[0] == "SLUS_203.98\t19\t4096");
    bool sawTvb = false;
    for (const std::string& row : rows) sawTvb = sawTvb || row == "SOUND/BGM.TVB\t45\t409600";
    CHECK(sawTvb);

    // Marker: names, counts and the verified CRC only (no disc bytes).
    const std::string marker = readAllFileText(extractDir / ".native_assets.json");
    CHECK(marker.find("\"boot_crc32\": \"85B52F7A\"") != std::string::npos);
    CHECK(marker.find("\"files\": 17") != std::string::npos);
    CHECK(marker.find("\"volume_id\": \"rt-native-dump\"") != std::string::npos);

    // Every extracted file exists again at the same size: validation passes.
    const InstallStatus after = rt::native::assets::validateInstalledAssets(extractDir);
    CHECK(after.installed);
    CHECK_EQ(after.bytes, kPlanDBytes);
}

void testFlowFastPathAndValidateInstalled() {
    // An empty dir is not installed, and reports the full required set.
    const auto emptyDir = makeEmptyExtractDir("flow-empty");
    const InstallStatus empty = rt::native::assets::validateInstalledAssets(emptyDir);
    CHECK(!empty.installed);
    CHECK(!empty.missingRules.empty());
    CHECK_EQ(empty.files, 0u);

    const auto image = writeTempFile("flow-fast.iso", buildGameLikeFixture(false).buildCooked());
    const auto extractDir = makeEmptyExtractDir("flow-fast-extract");
    GateConfig config;
    config.expectedBootCrc = kCrcDGate;
    FirstRunExtraction flow(config);
    CHECK(flow.run(image, extractDir));

    const InstallStatus installed = rt::native::assets::validateInstalledAssets(extractDir);
    CHECK(installed.installed);
    CHECK_EQ(installed.files, kPlanDItems);
    CHECK_EQ(installed.bytes, kPlanDBytes);

    // A second run short-circuits on the existing extract: Ready without
    // ever opening the image (a nonexistent path stands in for the pick).
    FirstRunExtraction second(config);
    CHECK(second.run(g_tempDir / "no-such-image.bin", extractDir));
    CHECK(second.snapshot().phase == ExtractionPhase::Ready);
    CHECK(second.snapshot().currentItem == "Assets already installed");

    // Removing one extracted file flips validation back to not-installed.
    std::error_code ec;
    std::filesystem::remove(extractDir / "CAR0" / "Q18.BIN", ec);
    const InstallStatus damaged = rt::native::assets::validateInstalledAssets(extractDir);
    CHECK(!damaged.installed);
    bool sawMissingCar = false;
    for (const std::string& m : damaged.missingRules)
        sawMissingCar = sawMissingCar || m.find("CAR0/Q18.BIN") != std::string::npos;
    CHECK(sawMissingCar);
}

void testFlowCancellation() {
    const auto image = writeTempFile("flow-cancel.iso", buildGameLikeFixture(false).buildCooked());
    const auto extractDir = makeEmptyExtractDir("flow-cancel-extract");
    GateConfig config;
    config.expectedBootCrc = kCrcDGate;
    FirstRunExtraction flow(config);
    flow.requestStop();
    CHECK(!flow.run(image, extractDir));
    CHECK(flow.snapshot().phase == ExtractionPhase::Cancelled);

    // The stop flag is consumed: the same flow object completes a retry.
    CHECK(flow.run(image, extractDir));
    CHECK(flow.snapshot().phase == ExtractionPhase::Ready);
}

void testFlowTypedFailures() {
    // NotReadable: a path that does not exist.
    {
        const auto extractDir = makeEmptyExtractDir("flow-fail-readable");
        FirstRunExtraction flow;
        CHECK(!flow.run(g_tempDir / "missing-image.iso", extractDir));
        CHECK(flow.snapshot().phase == ExtractionPhase::Failed);
        CHECK_EQ(flow.snapshot().error, IsoError::NotReadable);
    }
    // NotISO: 20 sectors of junk.
    {
        writeTempFile("junk-image.bin", std::vector<std::uint8_t>(20 * 2048, 0x55));
        const auto extractDir = makeEmptyExtractDir("flow-fail-notiso");
        FirstRunExtraction flow;
        CHECK(!flow.run(g_tempDir / "junk-image.bin", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::NotISO);
        CHECK(flow.snapshot().message.find("not an ISO9660") != std::string::npos);
    }
    // CHD magic: NotReadable with the CHD-specific message.
    {
        std::vector<std::uint8_t> chd(1u << 20, 0);
        std::memcpy(chd.data(), "MComprHD", 8);
        writeTempFile("junk-image.chd", chd);
        const auto extractDir = makeEmptyExtractDir("flow-fail-chd");
        FirstRunExtraction flow;
        CHECK(!flow.run(g_tempDir / "junk-image.chd", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::NotReadable);
        CHECK(flow.snapshot().message.find("CHD") != std::string::npos);
    }
    // NotThisGame: no gate file at the expected path.
    {
        FixtureF f;
        writeTempFile("fixture-f.iso", f.buildCooked());
        const auto extractDir = makeEmptyExtractDir("flow-fail-wronggame");
        FirstRunExtraction flow;
        CHECK(!flow.run(g_tempDir / "fixture-f.iso", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::NotThisGame);
        CHECK(flow.snapshot().message.find(kBootElfName) != std::string::npos);
    }
    // CorruptFile: gate present, wrong build CRC (injected expectation).
    {
        writeTempFile("flow-corrupt.iso", buildGameLikeFixture(false).buildCooked());
        const auto extractDir = makeEmptyExtractDir("flow-fail-corrupt");
        GateConfig config;
        config.expectedBootCrc = kCrcDGate ^ 0xFFu;
        FirstRunExtraction flow(config);
        CHECK(!flow.run(g_tempDir / "flow-corrupt.iso", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::CorruptFile);
        CHECK(flow.snapshot().message.find("85B52F7A") != std::string::npos);  // the actual
        CHECK(flow.snapshot().message.find("85B52F85") != std::string::npos);  // the expected
    }
    // Io: truncated mid-file during extraction.
    {
        auto truncated = buildGameLikeFixture(false).buildCooked();
        truncated.resize(100 * 2048);  // mid BGM.TVB (sectors 45..244)
        writeTempFile("flow-truncated.iso", truncated);
        const auto extractDir = makeEmptyExtractDir("flow-fail-io");
        GateConfig config;
        config.expectedBootCrc = kCrcDGate;
        FirstRunExtraction flow(config);
        CHECK(!flow.run(g_tempDir / "flow-truncated.iso", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::Io);
    }
    // NotThisGame from the plan: fixture A passes an injected gate but has
    // none of the game folders.
    {
        FixtureA a;
        writeTempFile("fixture-a.iso", a.buildCooked());
        const auto extractDir = makeEmptyExtractDir("flow-fail-plan");
        GateConfig config;
        config.expectedBootCrc = kCrcBoot;
        FirstRunExtraction flow(config);
        CHECK(!flow.run(g_tempDir / "fixture-a.iso", extractDir));
        CHECK_EQ(flow.snapshot().error, IsoError::NotThisGame);
        CHECK(flow.snapshot().message.find("SYS") != std::string::npos);
    }
}

}  // namespace

int main() {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    g_tempDir = std::filesystem::temp_directory_path() / ("rt-iso-extract-test-" + std::to_string(stamp));
    std::error_code ec;
    std::filesystem::remove_all(g_tempDir, ec);
    std::filesystem::create_directories(g_tempDir, ec);

    testCrc32AgainstExternalOracle();
    testOpenPvdAndWalkCooked();
    testLocateExactAndFolded();
    testReadFileByteExactAndProgress();
    testRaw2352StrideDetection();
    testCueSheetFollowsFileLine();
    testOpenErrorTypes();
    testGateChainPassFailAndTyping();
    testPlanRequiredMissing();
    testPlanOnGameLikeFixture();
    testThroughputEtaSyntheticThroughput();
    testEtaDescribeStrings();
    testManifestIsDataDriven();
    testFlowFullRunSnapshot();
    testFlowExtractedBytesSidecarAndMarker();
    testFlowFastPathAndValidateInstalled();
    testFlowCancellation();
    testFlowTypedFailures();

    std::filesystem::remove_all(g_tempDir, ec);
    if (g_fail != 0) {
        std::fprintf(stderr, "test_iso_extract: %d FAILURES\n", g_fail);
        return 1;
    }
    std::printf("test_iso_extract: all checks passed\n");
    return 0;
}
