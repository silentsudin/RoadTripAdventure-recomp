// W2-08 acceptance: save format v2 (integrity container around the 0x3448 block).
//   w2_save_roundtrip_test [--elf PATH] [--verbose]
// ELF: $RT_ELF, else --elf, else ~/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98.
// Without an ELF the synthetic suites run, "NO_ELF" is printed and the ELF pins are
// reported as 0/0.  Exit 0 only if every check passes.  No ROM bytes are embedded:
// only addresses, instruction words / pointers and a CRC-32 of one path literal.

#include "gameplay/save_v2.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

using namespace rt::native::gameplay;
namespace v2 = rt::native::gameplay::save_v2;

namespace {

bool gVerbose = false;

struct Fnv {
    std::uint64_t h = 0xcbf29ce484222325ULL;
    void bytes(const std::uint8_t* p, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) h = (h ^ p[i]) * 0x100000001b3ULL;
    }
    void u64(std::uint64_t v) {
        for (int i = 0; i < 8; ++i) h = (h ^ ((v >> (8 * i)) & 0xff)) * 0x100000001b3ULL;
    }
};

struct Rng {
    std::uint64_t s;
    explicit Rng(std::uint64_t seed) : s(seed ? seed : 1) {}
    std::uint64_t next() {
        s ^= s << 13;
        s ^= s >> 7;
        s ^= s << 17;
        return s;
    }
};

// ---------------------------------------------------------------- ELF reader
class Elf {
public:
    static std::optional<Elf> open(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) return std::nullopt;
        Elf e;
        e.bytes_.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        if (e.bytes_.size() < 0x34 || std::memcmp(e.bytes_.data(), "\x7f" "ELF", 4) != 0) return std::nullopt;
        const std::uint32_t phoff = e.u32At(0x1C);
        const std::uint16_t phnum = static_cast<std::uint16_t>(e.bytes_[0x2C] | (e.bytes_[0x2D] << 8));
        for (std::uint16_t i = 0; i < phnum; ++i) {
            const std::size_t p = phoff + 32u * i;
            if (p + 32 > e.bytes_.size()) return std::nullopt;
            if (e.u32At(p) != 1) continue;
            e.segs_.push_back({e.u32At(p + 8), e.u32At(p + 4), e.u32At(p + 16)});
        }
        return e;
    }
    bool readU32(std::uint32_t va, std::uint32_t& out) const {
        const std::size_t off = locate(va, 4);
        if (off == npos) return false;
        out = u32At(off);
        return true;
    }
    bool readBytes(std::uint32_t va, std::size_t n, std::vector<std::uint8_t>& out) const {
        const std::size_t off = locate(va, n);
        if (off == npos) return false;
        out.assign(bytes_.begin() + static_cast<std::ptrdiff_t>(off), bytes_.begin() + static_cast<std::ptrdiff_t>(off + n));
        return true;
    }
private:
    static constexpr std::size_t npos = ~std::size_t{0};
    struct Seg { std::uint32_t vaddr, offset, filesz; };
    std::size_t locate(std::uint32_t va, std::size_t n) const {
        for (const auto& s : segs_)
            if (va >= s.vaddr && std::uint64_t{va} + n <= std::uint64_t{s.vaddr} + s.filesz) {
                const std::size_t off = s.offset + (va - s.vaddr);
                return off + n <= bytes_.size() ? off : npos;
            }
        return npos;
    }
    std::uint32_t u32At(std::size_t o) const {
        return std::uint32_t{bytes_[o]} | (std::uint32_t{bytes_[o + 1]} << 8) | (std::uint32_t{bytes_[o + 2]} << 16) |
               (std::uint32_t{bytes_[o + 3]} << 24);
    }
    std::vector<std::uint8_t> bytes_;
    std::vector<Seg> segs_;
};

struct PinCount {
    int ok = 0, total = 0;
    void check(bool cond, const char* what, std::uint32_t va) {
        ++total;
        if (cond) ++ok;
        else if (gVerbose) std::cerr << "PIN FAIL " << what << " @0x" << std::hex << va << std::dec << "\n";
    }
};

PinCount runPins(const Elf& elf) {
    PinCount p;
    auto word = [&](std::uint32_t va, std::uint32_t want, const char* what, bool extra) {
        std::uint32_t w = 0;
        p.check(elf.readU32(va, w) && w == want && extra, what, va);
    };
    // addiu a2,zero,0x3448 (memset / sceMcRead / sceMcWrite sizes)
    word(0x2277C4, 0x24063448, "InitSaveSlotDefaults memset size", (0x24063448u & 0xFFFF) == kSaveSize);
    word(0x2285D4, 0x24063448, "MemcardReadSlot sceMcRead size", true);
    word(0x228A38, 0x24063448, "MemcardCreateSlotTask sceMcWrite size", true);
    // SpendTimerBudget cap: lui v1,0xf ; ori v1,v1,0x423f == 999999
    word(0x23DC88, 0x3C03000F, "money cap lui", true);
    word(0x23DC8C, 0x3463423F, "money cap ori", ((0xFu << 16) | 0x423Fu) == kMoneyCap);
    word(0x229FFC, 0x240303E8, "new-game money addiu 1000", 0x3E8 == kMoneyNewGame);
    word(0x2279FC, 0x241603FF, "towns_unvisited 0x3ff", 0x3FF == kTownsAllUnvisited);
    // unvisited-door masks (lui+ori pairs; ori words pinned)
    word(0x2279B0, 0x34A5DFFF, "door mask idx1 0x3dfff ori", true);
    word(0x2279B4, 0x34C6FBFF, "door mask idx2 0x7fbff ori", true);
    word(0x2279C8, 0x354AFFDF, "door mask idx9 0x7fffdf ori", true);
    word(0x2279E4, 0x24100FBF, "door mask idx4 0xfbf", true);
    word(0x2279E8, 0x2411017F, "door mask idx8 0x17f", true);
    word(0x2279D0, 0x240D005A, "default byte 0x5a @0x12DE", true);
    word(0x227804, 0x24030040, "remap[0]=0x40", true);
    word(0x227818, 0x240A0100, "remap[9]=0x100 (const)", true);
    // "/BASLUS-20398/BASLUS-20398": length 26, CRC-32 3a7ad250
    std::vector<std::uint8_t> s;
    const bool strOk = elf.readBytes(0x2EB5B0, 26, s) && v2::crc32(s) == 0x3A7AD250u;
    std::uint32_t term = 0;
    p.check(strOk && elf.readU32(0x2EB5B0 + 24, term) && (term >> 16) == 0, "save path literal crc32", 0x2EB5B0);
    return p;
}

// ---------------------------------------------------------------- synthetic saves
v2::Image makeSave(int which) {
    v2::Image img = v2::newGameImage();
    Rng r(0x1000 + static_cast<std::uint64_t>(which));
    auto put32 = [&](std::size_t off, std::uint32_t v) {
        for (int i = 0; i < 4; ++i) img[off + static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(v >> (8 * i));
    };
    switch (which) {
        case 0: break;  // new game
        case 1:         // mid-game: random body, sane scalar fields
            for (auto& b : img) b = static_cast<std::uint8_t>(r.next());
            put32(save_offset::kMoney, 54321);
            img[save_offset::kLicence] = 2;
            img[save_offset::kRadioStation] = 1;
            img[save_offset::kLocation] = 4;
            break;
        case 2:  // full stamps + photos
            std::memset(&img[save_offset::kStampsEarned], 0xFF, 12);
            img[save_offset::kStampsEarned + 12] = 0x0F;
            std::memset(&img[save_offset::kPhotosTaken], 0xFF, 16);
            break;
        case 3: put32(save_offset::kMoney, kMoneyCap); break;
        case 4:  // longest names
            std::memset(&img[save_offset::kPlayerName], 'A', kPlayerNameSize - 1);
            img[save_offset::kPlayerName + kPlayerNameSize - 1] = 0;
            std::memset(&img[save_offset::kCurrencyName], 'B', kCurrencyNameSize - 1);
            img[save_offset::kCurrencyName + kCurrencyNameSize - 1] = 0;
            break;
        default: img.fill(0xFF); break;  // 5: all 0xFF
    }
    return img;
}

// Pinned after first generation (FNV-1a 64 of the migrated v2 bytes for makeSave(0..5), no TLV).
constexpr std::uint64_t kGoldenV2Fnv[6] = {
    0x5f0dabd235415f80ULL, 0x5de06b6348a738a3ULL, 0xb89f0c0153934d84ULL,
    0x462a68f44ca5d3ecULL, 0x28b61baa4c3033b2ULL, 0xf4885cc59eeeb7bfULL,
};

std::uint64_t fnvOf(const std::vector<std::uint8_t>& v) {
    Fnv f;
    f.bytes(v.data(), v.size());
    return f.h;
}

struct Results {
    int rtOk = 0, rtTotal = 0;
    int migOk = 0, migTotal = 0;
    int fuzzRuns = 0, fuzzViolations = 0, fuzzRecovered = 0, fuzzUnrecoverable = 0, fuzzStrictMiss = 0;
    int truncOk = 0, truncTotal = 0;
    std::uint64_t hash = 0;
    std::uint64_t golden[6] = {};
};

bool consistent(const v2::Container& c) {
    const SaveData d = v2::toSaveData(c);
    if (d.money > kMoneyCap || d.licence > 3 || d.radioStation > 2 || d.location > 9 || d.mailCount > 128 ||
        d.townsUnvisited > kTownsAllUnvisited || d.lastPhoto > 99 || d.stampQueueCount > 8 || d.wgpStagesRun > 7)
        return false;
    if (!v2::tlvWellFormed(c.tlv)) return false;
    return v2::verifyV2(v2::serializeV2(c));  // re-serialized recovery state is itself valid
}

void runSuite(Results& R) {
    Fnv th;

    // ---- golden roundtrip (6 saves; odd ones carry TLV incl. an unknown tag)
    for (int i = 0; i < 6; ++i) {
        v2::Container c;
        c.image = makeSave(i);
        c.saveSeq = 7 + static_cast<std::uint32_t>(i);
        if (i & 1) {
            const std::uint8_t opt[4] = {90, 1, 0, 2};
            v2::tlvSet(c, 0x0001, opt);
            const std::uint8_t unk[3] = {0xDE, 0xAD, 0xBE};
            v2::tlvSet(c, 0x7EEF, unk);
        }
        const auto bytes = v2::serializeV2(c);
        v2::LoadReport rep;
        const v2::Container back = v2::loadV2(bytes, rep);
        const auto again = v2::serializeV2(back);
        const bool unkOk = !(i & 1) || (v2::tlvFind(back, 0x7EEF) && v2::tlvFind(back, 0x7EEF)->size() == 3);
        const bool ok = v2::verifyV2(bytes) && rep.status == v2::Status::Ok && back.image == c.image && again == bytes &&
                        back.saveSeq == c.saveSeq && unkOk && v2::detectFormat(bytes) == v2::Format::V2;
        ++R.rtTotal;
        R.rtOk += ok ? 1 : 0;
        th.u64(fnvOf(bytes));
    }

    // ---- v1 -> v2 migration golden
    for (int i = 0; i < 6; ++i) {
        const v2::Image img = makeSave(i);
        ++R.migTotal;
        const auto mig = v2::migrateV1ToV2(img);
        bool ok = mig && v2::detectFormat(img) == v2::Format::V1Raw && v2::verifyV2(*mig);
        if (ok) {
            v2::LoadReport rep;
            const v2::Container c = v2::loadV2(*mig, rep);
            v2::LoadReport rep2;
            const v2::Container viaAny = v2::loadAny(img, rep2);
            R.golden[i] = fnvOf(*mig);
            ok = rep.status == v2::Status::Ok && c.image == img && viaAny.image == img && c.saveSeq == 1 &&
                 R.golden[i] == kGoldenV2Fnv[i];
            th.u64(R.golden[i]);
        }
        R.migOk += ok ? 1 : 0;
    }
    {  // wrong-size v1 input is refused
        std::vector<std::uint8_t> bad(kSaveSize - 1, 0);
        ++R.migTotal;
        R.migOk += (!v2::migrateV1ToV2(bad) && v2::detectFormat(bad) == v2::Format::Unknown) ? 1 : 0;
    }
    {  // corrupt-record recovery: damage record 7 (race results) only; others survive, record 7 -> default
        v2::Container c;
        c.image = makeSave(1);
        auto bytes = v2::serializeV2(c);
        const auto& spec = v2::recordSpecs()[7];
        bytes[v2::kHeaderSize + v2::kRecordCount * v2::kRecordEntrySize + spec.offset + 3] ^= 0x55;
        v2::LoadReport rep;
        const v2::Container back = v2::loadV2(bytes, rep);
        bool ok = !v2::verifyV2(bytes) && rep.status == v2::Status::Recovered && rep.resetRecords.size() == 1 &&
                  rep.resetRecords[0] == spec.id;
        for (const auto& r : v2::recordSpecs()) {
            const bool same = std::equal(back.image.begin() + static_cast<std::ptrdiff_t>(r.offset),
                                         back.image.begin() + static_cast<std::ptrdiff_t>(r.offset + r.size),
                                         (r.id == spec.id ? v2::newGameImage() : c.image).begin() + static_cast<std::ptrdiff_t>(r.offset));
            ok = ok && same;
        }
        ++R.migTotal;
        R.migOk += ok ? 1 : 0;
    }

    // ---- corruption fuzz
    v2::Container gold;
    gold.image = makeSave(1);
    const std::uint8_t opt[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    v2::tlvSet(gold, 0x0001, opt);
    const auto goldBytes = v2::serializeV2(gold);
    const std::size_t imgOff = v2::kHeaderSize + v2::kRecordCount * v2::kRecordEntrySize;
    Rng rng(0xC0FFEE123ULL);
    for (int it = 0; it < 1000; ++it) {
        auto m = goldBytes;
        const int nflip = 1 + static_cast<int>(rng.next() % 4);
        std::vector<std::size_t> pos;
        while (static_cast<int>(pos.size()) < nflip) {
            const std::size_t p = (it % 10 == 0) ? imgOff + rng.next() % kSaveSize : rng.next() % m.size();
            if (std::find(pos.begin(), pos.end(), p) == pos.end()) pos.push_back(p);
        }
        for (std::size_t p : pos) m[p] ^= static_cast<std::uint8_t>(1 + rng.next() % 255);
        ++R.fuzzRuns;
        bool bad = false;
        if (v2::verifyV2(m)) { bad = true; ++R.fuzzStrictMiss; }
        v2::LoadReport rep;
        const v2::Container c = v2::loadV2(m, rep);
        if (!consistent(c)) bad = true;
        if (rep.status == v2::Status::Ok) bad = true;  // a damaged file may never report clean
        const bool headerFail = rep.status != v2::Status::Recovered;
        if (headerFail) {
            ++R.fuzzUnrecoverable;
            if (c.image != v2::newGameImage() || !c.tlv.empty()) bad = true;
        } else {
            ++R.fuzzRecovered;
            std::size_t e = v2::kHeaderSize;
            for (const auto& r : v2::recordSpecs()) {
                bool dataHit = false, entryHit = false;
                for (std::size_t p : pos) {
                    dataHit |= p >= imgOff + r.offset && p < imgOff + r.offset + r.size;
                    entryHit |= p >= e && p < e + v2::kRecordEntrySize;
                }
                const auto b = c.image.begin() + static_cast<std::ptrdiff_t>(r.offset);
                const bool isGold = std::equal(b, b + static_cast<std::ptrdiff_t>(r.size), gold.image.begin() + static_cast<std::ptrdiff_t>(r.offset));
                const bool isDef = std::equal(b, b + static_cast<std::ptrdiff_t>(r.size), v2::newGameImage().begin() + static_cast<std::ptrdiff_t>(r.offset));
                if (!dataHit && !entryHit && !isGold) bad = true;                // untouched record must survive
                if (dataHit && !entryHit && !isDef) bad = true;                  // damaged record must reset
                const bool listed = std::find(rep.resetRecords.begin(), rep.resetRecords.end(), r.id) != rep.resetRecords.end();
                if ((dataHit || entryHit) && !listed) bad = true;
                e += v2::kRecordEntrySize;
            }
        }
        if (bad) {
            ++R.fuzzViolations;
            if (gVerbose) std::cerr << "fuzz violation it=" << it << "\n";
        }
        th.u64(static_cast<std::uint64_t>(rep.status) | (std::uint64_t{rep.resetRecords.size()} << 8));
        th.bytes(c.image.data(), c.image.size());
    }

    // ---- truncation: every sampled prefix is rejected strictly and recovers to a consistent state
    for (std::size_t len = 0; len < goldBytes.size(); len += (len < 80 ? 1 : 97)) {
        const std::vector<std::uint8_t> t(goldBytes.begin(), goldBytes.begin() + static_cast<std::ptrdiff_t>(len));
        v2::LoadReport rep;
        const v2::Container c = v2::loadV2(t, rep);
        ++R.truncTotal;
        R.truncOk += (!v2::verifyV2(t) && rep.status != v2::Status::Ok && rep.status != v2::Status::Recovered && consistent(c)) ? 1 : 0;
    }
    R.hash = th.h;
}

}  // namespace

int main(int argc, char** argv) {
    std::string elfPath;
    if (const char* e = std::getenv("RT_ELF")) elfPath = e;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--elf" && i + 1 < argc) { if (elfPath.empty()) elfPath = argv[++i]; else ++i; }
        else if (a == "--verbose") gVerbose = true;
    }
    if (elfPath.empty()) {
        const char* home = std::getenv("HOME");
        if (home) elfPath = std::string(home) + "/Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    }

    Results a, b;
    runSuite(a);
    runSuite(b);
    const bool det = a.hash == b.hash && a.rtOk == b.rtOk && a.migOk == b.migOk && a.fuzzViolations == b.fuzzViolations;

    if (kGoldenV2Fnv[0] == 0 && gVerbose)
        for (int i = 0; i < 6; ++i) std::cerr << "golden[" << i << "]=0x" << std::hex << a.golden[i] << std::dec << "\n";

    PinCount pins;
    bool noElf = false;
    if (auto elf = Elf::open(elfPath)) pins = runPins(*elf);
    else noElf = true;
    if (noElf) std::cout << "NO_ELF\n";

    std::cout << "W2-08 DETAIL recovered=" << a.fuzzRecovered << " unrecoverable=" << a.fuzzUnrecoverable
              << " strict_miss=" << a.fuzzStrictMiss << " trunc=" << a.truncOk << "/" << a.truncTotal << "\n";
    std::cout << "W2-08 SCORE roundtrip=" << a.rtOk << "/" << a.rtTotal << " migration=" << a.migOk << "/" << a.migTotal
              << " fuzz_runs=" << a.fuzzRuns << " fuzz_recover_violations=" << a.fuzzViolations
              << " elf_pins=" << pins.ok << "/" << pins.total << " deterministic=" << (det ? 1 : 0) << "\n";

    const bool pass = a.rtOk == a.rtTotal && a.migOk == a.migTotal && a.fuzzRuns == 1000 && a.fuzzViolations == 0 &&
                      a.fuzzStrictMiss == 0 && a.truncOk == a.truncTotal && det && (noElf || pins.ok == pins.total);
    return pass ? 0 : 1;
}
