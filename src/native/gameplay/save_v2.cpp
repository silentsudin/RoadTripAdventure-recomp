#include "gameplay/save_v2.h"

#include <cstring>

namespace rt::native::gameplay::save_v2 {

namespace {

constexpr std::uint32_t kProgressBase = 0x0177F760;  // gs_progress_ram_address (save-layout.md)

struct WordStore {
    std::uint32_t addr;  // EE address written by InitSaveSlotDefaults 0x2277B0
    std::uint32_t value;
};

// u32 stores from 0x2277B0 (decomp build/decomp/fn/002277b0.c); offset = addr - kProgressBase.
constexpr WordStore kWordStores[] = {
    {0x0177FCA8, 0xFFFFFFFFu}, {0x0177FCAC, 0xFFFFFFFFu},  // coins-missing bitmap, 100 bits set
    {0x0177FCB0, 0xFFFFFFFFu}, {0x0177FCB4, 0x0000000Fu},
    {0x0177FDB4, 1000u},                                   // money, InitProgressRamDefaults 0x229FD8
    {0x0177FDD0, 0x00000100u},
    {0x0178033C, 0x0003DFFFu}, {0x01780340, 0x0007FBFFu},  // unvisited-door masks idx1..9
    {0x01780344, 0x001FFFFFu}, {0x01780348, 0x00000FBFu},
    {0x0178034C, 0x0000000Fu}, {0x01780350, 0x000FFFFFu},
    {0x01780354, 0x0003FFFFu}, {0x01780358, 0x0000017Fu},
    {0x0178035C, 0x007FFFDFu},
    {0x01780360, 0xF}, {0x01780364, 0xF}, {0x01780368, 0xF}, {0x0178036C, 0x1F},
    {0x01780370, 7},   {0x01780374, 0x3F}, {0x01780378, 3},  {0x0178037C, 1},
    {0x01780380, 1},   {0x01780384, 0xF},  {0x01780388, 1},  {0x0178038C, 1},
    {0x017803A4, 1},   {0x017803B8, 1},    {0x017803CC, 1},  {0x017803E0, 1},
    {0x017803F4, 1},   {0x01780408, 1},    {0x0178041C, 1},  {0x01780430, 1},
    {0x01780444, 1},   {0x01780458, 1},    {0x0178046C, 1},  {0x01780480, 1},
    {0x01780494, 1},   {0x017804A8, 1},
};

void put16(Image& img, std::size_t off, std::uint16_t v) {
    img[off] = static_cast<std::uint8_t>(v);
    img[off + 1] = static_cast<std::uint8_t>(v >> 8);
}
void put32(Image& img, std::size_t off, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) img[off + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(v >> (8 * i));
}

Image buildNewGame() {
    Image img{};
    for (const auto& s : kWordStores) put32(img, s.addr - kProgressBase, s.value);

    constexpr char kName[] = "UnnamedRacer";  // 0x62C, strncpy at 0x2277B0
    std::memcpy(&img[save_offset::kPlayerName], kName, sizeof(kName));

    for (std::size_t i = 0; i < 78; ++i) img[save_offset::kRaceResults + i] = 0xFF;  // 3 x 26 byte stores
    for (std::size_t i = 0; i < kBestTimeEntries; ++i) {
        put32(img, save_offset::kBestLap + 4 * i, kBestTimeNone);
        put32(img, save_offset::kBestTotal + 4 * i, kBestTimeNone);
    }
    for (std::size_t k = 0; k < 10; ++k) {  // 0xA50 + 0x20k pairs
        put32(img, 0xA50 + 0x20 * k, kBestTimeNone);
        put32(img, 0xA54 + 0x20 * k, kBestTimeNone);
    }
    for (std::size_t k = 0; k < 8; ++k) {  // 0xB78 + 0xCk triples
        put32(img, 0xB78 + 0xC * k, kBestTimeNone);
        put32(img, 0xB7C + 0xC * k, kBestTimeNone);
        put16(img, 0xB82 + 0xC * k, 0x00FF);
    }
    for (std::size_t i = 0; i < 6; ++i) put32(img, 0x12B0 + 4 * i, kBestTimeNone);
    for (std::size_t i = 0; i < 10; ++i) img[0x12D0 + i] = 0x7F;
    put16(img, save_offset::kTownsUnvisited, kTownsAllUnvisited);
    img[save_offset::kRadioStation] = 2;
    img[0x12DE] = 0x5A;
    constexpr std::uint16_t kRemap[kControllerActionCount] = {0x40, 0x80, 0x20, 2, 4, 1, 8, 0x10, 8, 0x100};
    for (std::size_t i = 0; i < kControllerActionCount; ++i) put16(img, save_offset::kControllerRemap + 2 * i, kRemap[i]);
    return img;
}

std::uint32_t rd32(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint32_t>(b[o]) | (static_cast<std::uint32_t>(b[o + 1]) << 8) |
           (static_cast<std::uint32_t>(b[o + 2]) << 16) | (static_cast<std::uint32_t>(b[o + 3]) << 24);
}
std::uint16_t rd16(std::span<const std::uint8_t> b, std::size_t o) {
    return static_cast<std::uint16_t>(b[o] | (b[o + 1] << 8));
}
void wr32(std::vector<std::uint8_t>& b, std::size_t o, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) b[o + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(v >> (8 * i));
}
void wr16(std::vector<std::uint8_t>& b, std::size_t o, std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1] = static_cast<std::uint8_t>(v >> 8);
}

std::uint32_t fileCrc(std::span<const std::uint8_t> bytes) {
    std::vector<std::uint8_t> tmp(bytes.begin(), bytes.end());
    wr32(tmp, kHdrFileCrc, 0);
    return crc32(tmp);
}

std::size_t totalSize(std::size_t tlvSize) {
    return kHeaderSize + kRecordCount * kRecordEntrySize + kSaveSize + tlvSize;
}

struct Parsed {
    Status status = Status::Ok;
    std::uint32_t tlvSize = 0;
};

Parsed parseHeader(std::span<const std::uint8_t> b) {
    Parsed p;
    if (b.size() < kHeaderSize) { p.status = Status::Truncated; return p; }
    if (std::memcmp(b.data(), "RTN2", 4) != 0) { p.status = Status::BadMagic; return p; }
    if (rd16(b, 4) != kVersion) { p.status = Status::BadVersion; return p; }
    if (rd16(b, 6) != kRecordCount || rd32(b, 8) != kSaveSize) { p.status = Status::BadLayout; return p; }
    p.tlvSize = rd32(b, kHdrTlvSize);
    if (p.tlvSize > b.size()) { p.status = Status::BadLayout; return p; }
    const std::size_t want = totalSize(p.tlvSize);
    if (b.size() < want) p.status = Status::Truncated;
    else if (b.size() > want) p.status = Status::BadLayout;
    return p;
}

}  // namespace

const std::array<RecordSpec, kRecordCount>& recordSpecs() {
    static const std::array<RecordSpec, kRecordCount> kSpecs = {{
        {0, 0x0000, 0x0508, "core+parts"},
        {1, 0x0508, 0x00A0, "photos/stamps/story/coins"},
        {2, 0x05A8, 0x0084, "mail"},
        {3, 0x062C, 0x0048, "profile (name/licence/money/mileage)"},
        {4, 0x0674, 0x03DC, "wgp+misc A"},
        {5, 0x0A50, 0x0188, "wgp+misc B"},
        {6, 0x0BD8, 0x0418, "doors+masks"},
        {7, 0x0FF0, 0x0050, "race results"},
        {8, 0x1040, 0x0138, "best lap"},
        {9, 0x1178, 0x0138, "best total"},
        {10, 0x12B0, 0x0038, "ski/towns/location/radio"},
        {11, 0x12E8, 0x0014, "controller remap"},
        {12, 0x12FC, 0x2004, "tail A"},
        {13, 0x3300, 0x0148, "stamp queue / wgp records"},
    }};
    return kSpecs;
}

const Image& newGameImage() {
    static const Image kImg = buildNewGame();
    return kImg;
}

std::uint32_t crc32(std::span<const std::uint8_t> bytes) {
    static const auto kTable = [] {
        std::array<std::uint32_t, 256> t{};
        for (std::uint32_t i = 0; i < 256; ++i) {
            std::uint32_t c = i;
            for (int k = 0; k < 8; ++k) c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            t[i] = c;
        }
        return t;
    }();
    std::uint32_t c = 0xFFFFFFFFu;
    for (std::uint8_t v : bytes) c = kTable[(c ^ v) & 0xFFu] ^ (c >> 8);
    return ~c;
}

std::vector<std::uint8_t> serializeV2(const Container& c) {
    std::vector<std::uint8_t> out(totalSize(c.tlv.size()), 0);
    std::memcpy(out.data(), "RTN2", 4);
    wr16(out, 4, kVersion);
    wr16(out, 6, static_cast<std::uint16_t>(kRecordCount));
    wr32(out, 8, static_cast<std::uint32_t>(kSaveSize));
    wr32(out, kHdrTlvSize, static_cast<std::uint32_t>(c.tlv.size()));
    wr32(out, kHdrTlvCrc, crc32(c.tlv));
    wr32(out, 20, c.saveSeq);
    const std::size_t imgOff = kHeaderSize + kRecordCount * kRecordEntrySize;
    std::memcpy(out.data() + imgOff, c.image.data(), kSaveSize);
    std::size_t e = kHeaderSize;
    for (const auto& r : recordSpecs()) {
        wr16(out, e, r.id);
        wr16(out, e + 2, 0);
        wr32(out, e + 4, crc32(std::span<const std::uint8_t>(c.image.data() + r.offset, r.size)));
        e += kRecordEntrySize;
    }
    if (!c.tlv.empty()) std::memcpy(out.data() + imgOff + kSaveSize, c.tlv.data(), c.tlv.size());
    wr32(out, kHdrFileCrc, crc32(out));
    return out;
}

bool verifyV2(std::span<const std::uint8_t> bytes) {
    const Parsed p = parseHeader(bytes);
    if (p.status != Status::Ok) return false;
    if (fileCrc(bytes) != rd32(bytes, kHdrFileCrc)) return false;
    const std::size_t imgOff = kHeaderSize + kRecordCount * kRecordEntrySize;
    std::size_t e = kHeaderSize;
    for (const auto& r : recordSpecs()) {
        if (rd16(bytes, e) != r.id || rd16(bytes, e + 2) != 0) return false;
        if (crc32(bytes.subspan(imgOff + r.offset, r.size)) != rd32(bytes, e + 4)) return false;
        e += kRecordEntrySize;
    }
    const auto tlv = bytes.subspan(imgOff + kSaveSize, p.tlvSize);
    return crc32(tlv) == rd32(bytes, kHdrTlvCrc) && tlvWellFormed(tlv);
}

Container loadV2(std::span<const std::uint8_t> bytes, LoadReport& report) {
    report = LoadReport{};
    Container c;  // new-game defaults
    const Parsed p = parseHeader(bytes);
    report.status = p.status;
    if (p.status != Status::Ok) return c;

    const std::size_t imgOff = kHeaderSize + kRecordCount * kRecordEntrySize;
    report.fileCrcOk = fileCrc(bytes) == rd32(bytes, kHdrFileCrc);
    bool clean = report.fileCrcOk;
    c.saveSeq = rd32(bytes, 20);
    std::size_t e = kHeaderSize;
    for (const auto& r : recordSpecs()) {
        const auto slice = bytes.subspan(imgOff + r.offset, r.size);
        const bool idOk = rd16(bytes, e) == r.id && rd16(bytes, e + 2) == 0;
        if (idOk && crc32(slice) == rd32(bytes, e + 4)) {
            std::memcpy(c.image.data() + r.offset, slice.data(), r.size);
        } else {  // image already holds the default for this record
            report.resetRecords.push_back(r.id);
            clean = false;
        }
        e += kRecordEntrySize;
    }
    const auto tlv = bytes.subspan(imgOff + kSaveSize, p.tlvSize);
    if (crc32(tlv) == rd32(bytes, kHdrTlvCrc) && tlvWellFormed(tlv)) {
        c.tlv.assign(tlv.begin(), tlv.end());
    } else {
        report.tlvDropped = true;
        clean = false;
    }
    report.status = clean ? Status::Ok : Status::Recovered;
    return c;
}

Format detectFormat(std::span<const std::uint8_t> bytes) {
    if (bytes.size() >= 4 && std::memcmp(bytes.data(), "RTN2", 4) == 0) return Format::V2;
    if (bytes.size() == kSaveSize) return Format::V1Raw;
    return Format::Unknown;
}

std::optional<std::vector<std::uint8_t>> migrateV1ToV2(std::span<const std::uint8_t> v1) {
    if (v1.size() != kSaveSize) return std::nullopt;
    Container c;
    std::memcpy(c.image.data(), v1.data(), kSaveSize);
    c.saveSeq = 1;
    return serializeV2(c);
}

Container loadAny(std::span<const std::uint8_t> bytes, LoadReport& report) {
    if (detectFormat(bytes) == Format::V1Raw) {
        report = LoadReport{};
        report.status = Status::Ok;
        report.fileCrcOk = true;
        Container c;
        std::memcpy(c.image.data(), bytes.data(), kSaveSize);
        return c;
    }
    return loadV2(bytes, report);
}

SaveData toSaveData(const Container& c, std::vector<std::string>* normalizations) {
    SaveResult r = parseSave(c.image);
    SaveData d = r.data;  // parseSave only fails on size; image is always kSaveSize
    auto notes = migrateToNative(d);
    if (normalizations) *normalizations = std::move(notes);
    return d;
}

bool tlvWellFormed(std::span<const std::uint8_t> tlv) {
    std::size_t o = 0;
    while (o < tlv.size()) {
        if (tlv.size() - o < 4) return false;
        const std::size_t len = rd16(tlv, o + 2);
        if (tlv.size() - o - 4 < len) return false;
        o += 4 + len;
    }
    return true;
}

void tlvSet(Container& c, std::uint16_t tag, std::span<const std::uint8_t> value) {
    std::vector<std::uint8_t> out;
    std::size_t o = 0;
    while (o + 4 <= c.tlv.size()) {  // c.tlv is well-formed by construction
        const std::size_t len = c.tlv[o + 2] | (c.tlv[o + 3] << 8);
        if (rd16(c.tlv, o) != tag) out.insert(out.end(), c.tlv.begin() + static_cast<std::ptrdiff_t>(o),
                                              c.tlv.begin() + static_cast<std::ptrdiff_t>(o + 4 + len));
        o += 4 + len;
    }
    const std::size_t at = out.size();
    out.resize(at + 4 + value.size());
    wr16(out, at, tag);
    wr16(out, at + 2, static_cast<std::uint16_t>(value.size()));
    if (!value.empty()) std::memcpy(out.data() + at + 4, value.data(), value.size());
    c.tlv = std::move(out);
}

std::optional<std::vector<std::uint8_t>> tlvFind(const Container& c, std::uint16_t tag) {
    std::size_t o = 0;
    while (o + 4 <= c.tlv.size()) {
        const std::size_t len = c.tlv[o + 2] | (c.tlv[o + 3] << 8);
        if (rd16(c.tlv, o) == tag)
            return std::vector<std::uint8_t>(c.tlv.begin() + static_cast<std::ptrdiff_t>(o + 4),
                                             c.tlv.begin() + static_cast<std::ptrdiff_t>(o + 4 + len));
        o += 4 + len;
    }
    return std::nullopt;
}

}  // namespace rt::native::gameplay::save_v2
