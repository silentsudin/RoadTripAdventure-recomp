// Authored memory-card save model contract tests. Synthetic 13,384-byte
// buffers only - no game data. Offsets per config/game_state.toml [progress]
// and docs/game-mechanics/menus-and-save.md "Save layout".
#include "../../src/native/gameplay/save.h"
#include "../../src/native/gameplay/save_slots.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace rt::native::gameplay;

static int g_fail = 0;
static int g_checks = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

void putU16(std::vector<std::uint8_t>& b, std::size_t off, std::uint16_t v) {
    b[off] = static_cast<std::uint8_t>(v);
    b[off + 1] = static_cast<std::uint8_t>(v >> 8);
}
void putU32(std::vector<std::uint8_t>& b, std::size_t off, std::uint32_t v) {
    putU16(b, off, static_cast<std::uint16_t>(v));
    putU16(b, off + 2, static_cast<std::uint16_t>(v >> 16));
}
void putU64(std::vector<std::uint8_t>& b, std::size_t off, std::uint64_t v) {
    putU32(b, off, static_cast<std::uint32_t>(v));
    putU32(b, off + 4, static_cast<std::uint32_t>(v >> 32));
}
void putStr(std::vector<std::uint8_t>& b, std::size_t off, std::size_t cap, const char* s) {
    const std::size_t n = std::strlen(s);
    CHECK(n <= cap);
    std::memcpy(b.data() + off, s, n);  // rest of the field stays as-filled
}

// Clean synthetic save: zero-based, known values at every documented offset.
std::vector<std::uint8_t> makeCleanBuffer() {
    std::vector<std::uint8_t> b(kSaveSize, 0);
    // parts @0x00D (six categories; +0x0E = engine index 7)
    const std::uint8_t parts[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    std::memcpy(b.data() + save_offset::kParts, parts, 6);
    // photos_taken @0x508: photo n taken = bit n-1; take photos 1 and 9
    putU16(b, save_offset::kPhotosTaken, 0x0101);       // bytes 0,1 = photo 1, photo 9
    // stamps_earned @0x518: two u64, stamps 1..100 at bits 0..99
    putU64(b, save_offset::kStampsEarned, 0x0123456789ABCDEFull);
    putU64(b, save_offset::kStampsEarned + 8, 0x0000007FFFFFFFFFull);  // bits 64..99 usable
    // town_entered @0x528 (guess): town 1 bit 0, town 3 bit 22
    putU32(b, save_offset::kTownEntered, 0x00400001u);
    // story_flags @0x538: low word = scene 6 (Q's Factory menu); WGP stage 1 = bit 0x65
    putU32(b, save_offset::kStoryFlags, 6u);
    b[save_offset::kStoryFlags + 12] = 0x20;  // bit 0x65: byte 12, bit 5
    // coins_missing @0x548: coin 1 missing (bit 0)
    b[save_offset::kCoinsMissing] = 0x01;
    // mail_inbox @0x5A8: slot 0 = mail id 3, read; slot 1 = mail id 5, unread
    b[save_offset::kMailInbox + 0] = 0x83;
    b[save_offset::kMailInbox + 1] = 0x05;
    putU32(b, save_offset::kMailCount, 2u);
    putStr(b, save_offset::kPlayerName, kPlayerNameSize, "PLAYER 1");
    putStr(b, save_offset::kCurrencyName, kCurrencyNameSize, "COIN");
    b[save_offset::kLicence] = 2;               // licence A
    putU32(b, save_offset::kMoney, 123456u);
    putU32(b, save_offset::kMileageM, 421995u);
    b[save_offset::kWgpTeammate0] = 1;
    b[save_offset::kWgpTeammate1] = 3;
    // unvisited_doors @0xBD8: the New-Game init masks (0x2277B0)
    const std::uint32_t doors[10] = {0u, 0x3DFFFu, 0x7FBFFu, 0x1FFFFFu, 0xFBFu,
                                     0xFu, 0xFFFFFu, 0x3FFFFu, 0x17Fu, 0x7FFFDFu};
    for (int i = 0; i < 10; ++i) putU32(b, save_offset::kUnvisitedDoors + 4 * i, doors[i]);
    // race_results @0xFF0: race 0 won (place-1 = 0), race 1 fifth, rest not raced
    for (int i = 0; i < 35; ++i) b[save_offset::kRaceResults + i] = 0xFF;
    b[save_offset::kRaceResults + 0] = 0;
    b[save_offset::kRaceResults + 1] = 4;
    // best_lap @0x1040 / best_total @0x1178: 78 x u32 frames, 0x7FFFFFFF = none
    for (int i = 0; i < 78; ++i) {
        putU32(b, save_offset::kBestLap + 4 * i, 0x7FFFFFFFu);
        putU32(b, save_offset::kBestTotal + 4 * i, 0x7FFFFFFFu);
    }
    putU32(b, save_offset::kBestLap + 4 * 1, 5432u);
    putU32(b, save_offset::kBestTotal + 4 * 1, 98765u);
    putU32(b, save_offset::kSkiJumpBest, 106u);          // Panther engine ~106 m
    putU16(b, save_offset::kTownsUnvisited, 0x3FEu);     // town 0 reached
    b[save_offset::kLocation] = 3;                       // Sandpolis save point
    b[save_offset::kRadioStation] = 1;                   // PEACH FM
    putU16(b, save_offset::kLastPhoto, 41u);
    // controller_remap @0x12E8: 10 u16 game-word-order masks (action_map order)
    const std::uint16_t remap[10] = {0x0040, 0x0080, 0x0020, 0x0010, 0x0008,
                                     0x0004, 0x0002, 0x0001, 0x0100, 0x1000};
    for (int i = 0; i < 10; ++i) putU16(b, save_offset::kControllerRemap + 2 * i, remap[i]);
    putU32(b, save_offset::kStampQueueCount, 2u);
    putU32(b, save_offset::kWgpStagesRun, 3u);
    return b;
}

// AUTHORED test fixture using the public PS2 memory-card format: 512-byte
// pages, two pages per cluster, a two-level FAT, and 512-byte directory
// entries.  It contains only synthetic progress bytes.
void putDirEntry(std::vector<std::uint8_t>& card, std::size_t off, std::uint16_t mode,
                 std::uint32_t length, std::uint32_t cluster, const char* name) {
    putU16(card, off, mode);
    putU32(card, off + 4, length);
    putU32(card, off + 0x10, cluster);
    std::memcpy(card.data() + off + 0x40, name, std::strlen(name));
}

std::vector<std::uint8_t> makeSyntheticP2(const std::vector<std::uint8_t>& save) {
    constexpr std::size_t kPage = 512;
    constexpr std::size_t kCluster = 1024;
    constexpr std::size_t kClusters = 128;
    constexpr std::size_t kAlloc = 10;
    std::vector<std::uint8_t> card(kClusters * kCluster, 0);
    std::memcpy(card.data(), "Sony PS2 Memory Card Format", 27);
    putU16(card, 0x28, kPage);
    putU16(card, 0x2A, 2);
    putU16(card, 0x2C, 16);
    putU32(card, 0x30, kClusters);
    putU32(card, 0x34, kAlloc);
    putU32(card, 0x3C, 0);  // root directory relative to alloc_offset
    putU32(card, 0x50, 1);  // indirect FAT cluster (absolute cluster number)

    // indirect FAT #1 -> FAT cluster #2; FAT values are relative allocations.
    putU32(card, kCluster, 2);
    for (std::uint32_t rel = 0; rel < 18; ++rel)
        putU32(card, 2 * kCluster + 4 * rel, rel == 17 ? 0xFFFFFFFFu : 0x80000000u | (rel + 1));

    const std::size_t root = kAlloc * kCluster;
    putDirEntry(card, root, 0x8020, 3, 0, ".");
    putDirEntry(card, root + kCluster, 0x8020, 3, 2, "BASLUS-20398");
    const std::size_t dir = (kAlloc + 2) * kCluster;
    putDirEntry(card, dir, 0x8020, 3, 0, ".");
    putDirEntry(card, dir + 2 * kPage, 0x8010, static_cast<std::uint32_t>(save.size()), 4,
                "BASLUS-20398");
    std::memcpy(card.data() + (kAlloc + 4) * kCluster, save.data(), save.size());
    return card;
}

bool contains(const std::vector<std::string>& v, const std::string& s) {
    for (const auto& x : v) if (x == s) return true;
    return false;
}

void testParseDocumentedFields() {
    const std::vector<std::uint8_t> b = makeCleanBuffer();
    SaveResult r = parseSave(b);
    CHECK(r.ok);
    CHECK(r.errors.empty());
    const SaveData& d = r.data;

    CHECK(d.parts[0] == 0x11 && d.parts[5] == 0x66);
    CHECK(d.photosTaken[0] == 0x01 && d.photosTaken[1] == 0x01 && d.photosTaken[2] == 0);
    CHECK(d.stampsEarned[0] == 0x0123456789ABCDEFull);
    CHECK(d.stampsEarned[1] == 0x0000007FFFFFFFFFull);
    CHECK(d.townEntered == 0x00400001u);
    CHECK(d.scene() == 6u);
    CHECK(d.storyFlags[12] == 0x20);  // WGP stage 1 flag (bit 0x65)
    CHECK(d.coinsMissing[0] == 0x01 && d.coinsMissing[15] == 0);
    CHECK(d.mailInbox[0] == 0x83 && d.mailInbox[1] == 0x05 && d.mailInbox[127] == 0);
    CHECK(d.mailCount == 2u);
    CHECK(d.playerName == "PLAYER 1");
    CHECK(d.currencyName == "COIN");
    CHECK(d.licence == 2);
    CHECK(d.money == 123456u);
    CHECK(d.mileageM == 421995u);
    CHECK(d.wgpTeammates[0] == 1 && d.wgpTeammates[1] == 3);
    CHECK(d.unvisitedDoors[0] == 0u && d.unvisitedDoors[1] == 0x3DFFFu &&
          d.unvisitedDoors[3] == 0x1FFFFFu && d.unvisitedDoors[9] == 0x7FFFDFu);
    CHECK(d.raceResults[0] == 0 && d.raceResults[1] == 4 && d.raceResults[2] == 0xFF);
    CHECK(d.bestLap[0] == 0x7FFFFFFFu && d.bestLap[1] == 5432u && d.bestLap[77] == 0x7FFFFFFFu);
    CHECK(d.bestTotal[1] == 98765u && d.bestTotal[0] == 0x7FFFFFFFu);
    CHECK(d.skiJumpBest == 106u);
    CHECK(d.townsUnvisited == 0x3FEu);
    CHECK(d.location == 3);
    CHECK(d.radioStation == 1);
    CHECK(d.lastPhoto == 41u);
    CHECK(d.controllerRemap[0] == 0x0040 && d.controllerRemap[7] == 0x0001 &&
          d.controllerRemap[9] == 0x1000);
    CHECK(d.stampQueueCount == 2u);
    CHECK(d.wgpStagesRun == 3u);
    CHECK(std::memcmp(d.raw.data(), b.data(), kSaveSize) == 0);  // input retained verbatim
}

void testRejectWrongSize() {
    std::vector<std::uint8_t> big(kSaveSize + 1, 0);
    SaveResult r1 = parseSave(big);
    CHECK(!r1.ok);
    CHECK(r1.errors.size() == 1);
    big.resize(kSaveSize - 1);
    SaveResult r2 = parseSave(big);
    CHECK(!r2.ok);
    CHECK(!r2.errors.empty());
    SaveResult r3 = parseSave({});
    CHECK(!r3.ok);
    CHECK(!r3.errors.empty());
    std::vector<std::uint8_t> exact(kSaveSize, 0);
    CHECK(parseSave(exact).ok);  // 13384 accepted, no magic required (none documented)
}

void testRoundTripExact() {
    // 0xA5 fill: no NULs anywhere, so even the string fields round-trip byte-exact.
    std::vector<std::uint8_t> b(kSaveSize, 0xA5);
    SaveResult r = parseSave(b);
    CHECK(r.ok);
    CHECK(r.data.playerName.size() == kPlayerNameSize);
    std::vector<std::uint8_t> out = serializeSave(r.data);
    CHECK(out.size() == kSaveSize);
    CHECK(out == b);
    // Clean buffer round-trips byte-exact too, and re-parsing yields equal fields.
    const std::vector<std::uint8_t> c = makeCleanBuffer();
    SaveResult rc = parseSave(c);
    std::vector<std::uint8_t> oc = serializeSave(rc.data);
    CHECK(oc == c);
    SaveResult rc2 = parseSave(oc);
    CHECK(rc2.ok);
    CHECK(rc2.data.money == rc.data.money && rc2.data.money == 123456u);
    CHECK(rc2.data.playerName == rc.data.playerName && rc.data.playerName == "PLAYER 1");
    CHECK(rc2.data.stampsEarned == rc.data.stampsEarned);
    CHECK(rc2.data.bestLap == rc.data.bestLap && rc2.data.bestTotal == rc.data.bestTotal);
    CHECK(rc2.data.controllerRemap == rc.data.controllerRemap);
}

void testUnknownRegionsPreserved() {
    std::vector<std::uint8_t> b(kSaveSize, 0xA5);
    SaveResult r = parseSave(b);
    CHECK(r.ok);
    // Mutate documented fields; serialize must touch only documented regions.
    r.data.money = 777u;
    r.data.playerName = "NATIVE";
    r.data.radioStation = 2;
    std::vector<std::uint8_t> out = serializeSave(r.data);
    CHECK(out.size() == kSaveSize);
    // Unknown bytes byte-identical (sampled across every unknown gap).
    const std::size_t unknownSamples[] = {0x0000, 0x000C, 0x0013, 0x0100, 0x052C, 0x0558,
                                          0x0652, 0x065C, 0x0675, 0x0C00, 0x1013, 0x12C0,
                                          0x12DE, 0x12FC, 0x3304, 0x3314, 0x3447};
    for (std::size_t off : unknownSamples) CHECK(out[off] == 0xA5);
    // Documented fields updated in place.
    CHECK(out[save_offset::kMoney] == 0x09 && out[save_offset::kMoney + 3] == 0x00);  // 777 LE
    CHECK(std::memcmp(out.data() + save_offset::kPlayerName, "NATIVE", 6) == 0);
    CHECK(out[save_offset::kPlayerName + 6] == 0);   // NUL-padded to the field
    CHECK(out[save_offset::kRadioStation] == 2);
    // Default-constructed SaveData: unknown regions serialize as zero.
    SaveData fresh;
    fresh.money = kMoneyNewGame;
    std::vector<std::uint8_t> fz = serializeSave(fresh);
    CHECK(fz.size() == kSaveSize);
    CHECK(fz[0x0000] == 0 && fz[0x3447] == 0);
    CHECK(fz[save_offset::kMoney] == 0xE8 && fz[save_offset::kMoney + 1] == 0x03);  // 1000 LE
}

void testUnknownFieldsReported() {
    SaveResult r = parseSave(makeCleanBuffer());
    CHECK(r.ok);
    // Computed complement ranges (documented regions excluded).
    CHECK(contains(r.unknownFields, "0x0000-0x000C"));  // slot header, not pinned
    CHECK(contains(r.unknownFields, "0x0013-0x0507"));
    CHECK(contains(r.unknownFields, "0x0652-0x0653"));  // currency/licence overlap padding
    CHECK(contains(r.unknownFields, "0x3304-0x330F"));  // queued-stamp entries
    CHECK(contains(r.unknownFields, "0x3314-0x3447"));  // WGP records + tail
    CHECK(!contains(r.unknownFields, "0x0654-0x0657")); // money IS documented
    // Explicit notes for known-but-unpinned pieces and the size-only validation.
    bool sawQueueNote = false, sawWgpNote = false, sawMagicNote = false;
    for (const auto& s : r.unknownFields) {
        if (s.find("queued-stamp") != std::string::npos) sawQueueNote = true;
        if (s.find("WGP per-stage") != std::string::npos) sawWgpNote = true;
        if (s.find("size only") != std::string::npos) sawMagicNote = true;
    }
    CHECK(sawQueueNote && sawWgpNote && sawMagicNote);
}

void testRegionTable() {
    const auto& regions = saveFieldRegions();
    CHECK(regions.size() == 27);
    std::size_t prevEnd = 0;
    for (const auto& r : regions) {
        CHECK(r.offset >= prevEnd);            // sorted, non-overlapping
        CHECK(r.size > 0 && r.offset + r.size <= kSaveSize);
        prevEnd = r.offset + r.size;
    }
    // Spot-check documented offsets/labels against game_state.toml [progress].
    CHECK(regions.front().offset == 0x000D && regions.front().size == 6);
    bool sawMoney = false, sawRemap = false;
    for (const auto& r : regions) {
        if (r.offset == 0x0654) { sawMoney = r.size == 4 && !std::strcmp(r.label, "money"); }
        if (r.offset == 0x12E8) { sawRemap = r.size == 20; }
    }
    CHECK(sawMoney && sawRemap);
}

void testMigrate() {
    SaveResult r = parseSave(makeCleanBuffer());
    CHECK(r.ok);
    // Already-clean save: no changes.
    SaveData clean = r.data;
    CHECK(migrateToNative(clean).empty());

    SaveData& s = r.data;
    s.licence = 9; s.radioStation = 5; s.location = 20;
    s.money = 1000000; s.mailCount = 200; s.stampQueueCount = 20;
    s.wgpStagesRun = 99; s.lastPhoto = 150; s.townsUnvisited = 0xFFFF;
    s.playerName = std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ123");  // 29 > 19
    s.currencyName = std::string("TOOLONGNAME0123456789");        // 23 > 18
    std::vector<std::string> changes = migrateToNative(s);
    CHECK(changes.size() == 11);
    CHECK(s.licence == 3);
    CHECK(s.radioStation == 2);
    CHECK(s.location == 9);
    CHECK(s.money == 999999u);          // SpendTimerBudget cap
    CHECK(s.mailCount == 128u);
    CHECK(s.stampQueueCount == 8u);
    CHECK(s.wgpStagesRun == 7u);
    CHECK(s.lastPhoto == 99u);
    CHECK(s.townsUnvisited == 0x3FFu);
    CHECK(s.playerName.size() == kPlayerNameSize);
    CHECK(s.playerName == "ABCDEFGHIJKLMNOPQRS");
    CHECK(s.currencyName.size() == kCurrencyNameSize);

    // Clamped values survive serialize -> parse; second migrate is a no-op.
    std::vector<std::uint8_t> out = serializeSave(s);
    SaveResult r2 = parseSave(out);
    CHECK(r2.ok);
    CHECK(r2.data.money == 999999u);
    CHECK(r2.data.licence == 3 && r2.data.radioStation == 2 && r2.data.location == 9);
    CHECK(r2.data.playerName == "ABCDEFGHIJKLMNOPQRS");
    CHECK(migrateToNative(r2.data).empty());
}

void testOriginalP2Migration() {
    const std::vector<std::uint8_t> original = makeCleanBuffer();
    const std::vector<std::uint8_t> p2 = makeSyntheticP2(original);
    const auto scanned = scanP2RoadTripSaves(p2);
    CHECK(scanned);
    CHECK(scanned->size() == 1);
    CHECK((*scanned)[0].directoryName == "BASLUS-20398");
    CHECK((*scanned)[0].fileName == "BASLUS-20398");
    CHECK((*scanned)[0].bytes == original);

    const auto imported = migrateFromP2Image(p2, 1);
    CHECK(imported);
    CHECK(imported->money == 123456u);
    CHECK(imported->playerName == "PLAYER 1");
    CHECK(std::memcmp(imported->raw.data(), original.data(), kSaveSize) == 0);

    const auto extracted = migrateFromSaveFile(std::span<const std::uint8_t>(original));
    CHECK(extracted);
    CHECK(extracted->raw == imported->raw);
}

void testOriginalP2MigrationErrors() {
    const auto save = makeCleanBuffer();
    auto p2 = makeSyntheticP2(save);
    CHECK(!migrateFromP2Image(p2, -1));
    CHECK(!migrateFromP2Image(p2, 2));
    p2[0] = 0;
    const auto badMagic = migrateFromP2Image(p2, 0);
    CHECK(!badMagic && badMagic.error().code == SaveErrorCode::NotP2Image);

    auto wrongTitle = makeSyntheticP2(save);
    std::memcpy(wrongTitle.data() + 10 * 1024 + 1024 + 0x40, "BADATA-00000", 12);
    const auto missing = migrateFromP2Image(wrongTitle, 0);
    CHECK(!missing && missing.error().code == SaveErrorCode::WrongTitle);

    std::vector<std::uint8_t> shortFile(kSaveSize - 1);
    const auto shortImport = migrateFromSaveFile(std::span<const std::uint8_t>(shortFile));
    CHECK(!shortImport && shortImport.error().code == SaveErrorCode::InvalidSize);
}

void testSaveActionDispatch() {
    // Options > Save rows: "save.save_state"/"save.load_state" through SlotStore
    // (the app dispatches the panel's lastAction; unknown ids fail cleanly).
    const std::filesystem::path dir = std::filesystem::temp_directory_path() /
        ("rt_save_action_" + std::to_string(::getpid()));
    std::filesystem::remove_all(dir);
    SlotStore store(dir);
    SaveData progress{};
    progress.money = 4242;
    std::string error;
    CHECK(runSaveAction("save.save_state", 1, store, progress, &error));
    CHECK(store.list()[1]);
    CHECK(!store.list()[0]);
    SaveData loaded{};
    CHECK(runSaveAction("save.load_state", 1, store, loaded, &error));
    CHECK(loaded.money == 4242u);
    CHECK(!runSaveAction("save.unknown", 0, store, progress, &error));
    CHECK(error == "unknown save action");
    // An empty slot is a failure, not a silent default save.
    SaveData empty{};
    CHECK(!runSaveAction("save.load_state", 0, store, empty, &error));
    CHECK(!error.empty());
    std::filesystem::remove_all(dir);
}

void testSaveImportFile() {
    // Issue #25: one file in, a SaveData out - an extracted BASLUS-20398 progress
    // file or a public-P2 image holding one. PCSX2's .psu/.ps2/.xps containers
    // are different formats and must be refused rather than guessed at.
    const std::filesystem::path dir = std::filesystem::temp_directory_path() /
        ("rt_save_import_" + std::to_string(::getpid()));
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
    const auto writeBytes = [](const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes) {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        return static_cast<bool>(out);
    };

    const std::vector<std::uint8_t> original = makeCleanBuffer();
    const std::filesystem::path imagePath = dir / "card.p2img";
    CHECK(writeBytes(imagePath, makeSyntheticP2(original)));
    const auto fromImage = importSaveFile(imagePath, 0);
    CHECK(fromImage);
    CHECK(fromImage && fromImage->money == 123456u);

    const std::filesystem::path filePath = dir / "BASLUS-20398";
    CHECK(writeBytes(filePath, original));
    const auto fromFile = importSaveFile(filePath, 1);
    CHECK(fromFile);
    CHECK(fromFile && fromImage && fromFile->raw == fromImage->raw);

    CHECK(!importSaveFile(imagePath, 2));  // slot out of range
    const auto missing = importSaveFile(dir / "missing.p2img", 0);
    CHECK(!missing && missing.error().code == SaveErrorCode::IoError);

    const std::filesystem::path junkPath = dir / "junk.bin";
    CHECK(writeBytes(junkPath, std::vector<std::uint8_t>(100, 0)));
    const auto junk = importSaveFile(junkPath, 0);
    CHECK(!junk && junk.error().code == SaveErrorCode::NotP2Image);

    const std::filesystem::path psuPath = dir / "card.psu";
    CHECK(writeBytes(psuPath, std::vector<std::uint8_t>{'P', 'S', '2', 'U', 0, 0, 0, 0}));
    CHECK(!importSaveFile(psuPath, 0));

    std::filesystem::remove_all(dir);
}

}  // namespace

int main() {
    testParseDocumentedFields();
    testRejectWrongSize();
    testRoundTripExact();
    testUnknownRegionsPreserved();
    testUnknownFieldsReported();
    testRegionTable();
    testMigrate();
    testOriginalP2Migration();
    testOriginalP2MigrationErrors();
    testSaveActionDispatch();
    testSaveImportFile();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::printf("save: %d checks passed\n", g_checks);
    return 0;
}
