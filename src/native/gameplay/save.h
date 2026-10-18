#pragma once
// Authored memory-card save model for Road Trip Adventure (SLUS-20398).
//
// The persistent state is the 13,384-byte (0x3448) "Adventure progress" block:
// the BASLUS-20398 memory-card file is this image verbatim, and it lives in EE
// RAM at gs_progress_ram_address 0x2C22C8 [brief] / 0x177F760 [config] (the two
// are the same buffer [inferred]; see docs/game-mechanics/menus-and-save.md
// "Save layout"). Field offsets come from config/game_state.toml [progress] /
// [stamps] / [world_grand_prix] / [houses] / [controls] and the save-layout
// section of the doc. All multi-byte fields are little-endian (EE R5900).
//
// Cross-checks anchoring the offsets (config bases minus 0x177F760):
//   DAT_0177FD8C ("PLAYER 1"/"PLAYER 2" strings) == +0x62C == player_name
//   DAT_0177FDB4 (SpendTimerBudget 0x23DC58, cap 999999) == +0x654 == money
//   action_map 0x1780A48 (10 u16 driving actions) == +0x12E8 == controller_remap
//
// There is NO in-block magic or version word documented (file identity is the
// BASLUS-20398 directory entry + icon.sys, outside the block), so parseSave
// validates size only; everything not pinned by the config/doc is preserved
// byte-identical and reported in SaveResult::unknownFields.
//
// Doc conflict resolved here: config lists currency_name at 0x63F length 21,
// but licence (u8 @0x651, confirmed) lands inside that span. We model
// currency_name with capacity 18 (0x63F..0x650) and report 0x652..0x653 as
// unknown padding.

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace rt::native::gameplay {

inline constexpr std::size_t kSaveSize = 13384;  // 0x3448: progress block == per-slot save image

// Documented field offsets (absolute within the block).
namespace save_offset {
inline constexpr std::size_t kParts           = 0x000D;  // 6 bytes, equipped part per category (+0x0E engine)
inline constexpr std::size_t kPhotosTaken     = 0x0508;  // 16-byte bitmap, bit n-1 = photo n
inline constexpr std::size_t kStampsEarned    = 0x0518;  // 2 x u64, 100 stamps at bits 0..99
inline constexpr std::size_t kTownEntered     = 0x0528;  // u32 story-flag word (confidence: guess)
inline constexpr std::size_t kStoryFlags      = 0x0538;  // 128-bit flag set; low u32 = scene; WGP stage n = bit 0x65+n-1
inline constexpr std::size_t kCoinsMissing    = 0x0548;  // 16-byte bitmap; found = 100 - popcount
inline constexpr std::size_t kMailInbox       = 0x05A8;  // 128 bytes: mail id | 0x80 once read (also president body Q149)
inline constexpr std::size_t kMailCount       = 0x0628;  // u32, max 128
inline constexpr std::size_t kPlayerName      = 0x062C;  // 19 bytes ("PLAYER 1" at New Game)
inline constexpr std::size_t kCurrencyName    = 0x063F;  // 18 bytes modeled (see header note; config says 21)
inline constexpr std::size_t kLicence         = 0x0651;  // u8: 0 none, 1 B, 2 A, 3 Super A
inline constexpr std::size_t kMoney           = 0x0654;  // u32, 1000 at New Game, 999999 cap
inline constexpr std::size_t kMileageM        = 0x0658;  // u32 metres (summary shows /1609 miles)
inline constexpr std::size_t kWgpTeammate0    = 0x0674;  // u8, 0 = none
inline constexpr std::size_t kWgpTeammate1    = 0x0676;  // u8, 0 = none
inline constexpr std::size_t kUnvisitedDoors  = 0x0BD8;  // 10 x u32: bit n set = door n not visited
inline constexpr std::size_t kRaceResults     = 0x0FF0;  // 35 bytes: place-1 per race entry, 0xFF = not raced
inline constexpr std::size_t kBestLap         = 0x1040;  // 78 x u32 frames, 0x7FFFFFFF = none
inline constexpr std::size_t kBestTotal       = 0x1178;  // 78 x u32 frames, 0x7FFFFFFF = none
inline constexpr std::size_t kSkiJumpBest     = 0x12C8;  // u32 metres (White Mountain ski jump)
inline constexpr std::size_t kTownsUnvisited  = 0x12DA;  // u16, bit n = town n never reached (0x3FF at New Game)
inline constexpr std::size_t kLocation        = 0x12DC;  // u8 save point: 1 Peach Town .. 9 My City
inline constexpr std::size_t kRadioStation    = 0x12DD;  // u8: 0 off, 1 PEACH FM, 2 E-RADIO
inline constexpr std::size_t kLastPhoto       = 0x12E2;  // u16 index (n-1) of last photo taken
inline constexpr std::size_t kControllerRemap = 0x12E8;  // 10 x u16 game-word-order masks (action_map)
inline constexpr std::size_t kStampQueueCount = 0x3300;  // u32 queued stamps (entries themselves not pinned)
inline constexpr std::size_t kWgpStagesRun    = 0x3310;  // u32 WGP stages run (records themselves not pinned)
}  // namespace save_offset

inline constexpr std::size_t kPartsCount          = 6;
inline constexpr std::size_t kPlayerNameSize      = 19;
inline constexpr std::size_t kCurrencyNameSize    = 18;  // see header note (config: 21, overlaps licence)
inline constexpr std::size_t kLocationCount       = 10;  // unvisited_doors u32s / towns (0..9)
inline constexpr std::size_t kRaceResultCount     = 78;  // InitSaveSlotDefaults 0x2277b0: 0xFF at 0xFF0 + i + 26*j (i<26, j<3) -> 78-byte blob
inline constexpr std::size_t kBestTimeEntries     = 78;  // 312 bytes / u32
inline constexpr std::size_t kControllerActionCount = 10;  // gas brake reverse jet wingUp wingDown horn view lights navigator
inline constexpr std::uint32_t kMoneyCap          = 999999;  // SpendTimerBudget 0x23DC58
inline constexpr std::uint32_t kBestTimeNone      = 0x7FFFFFFF;
inline constexpr std::uint8_t kResultNotRaced     = 0xFF;
inline constexpr std::uint32_t kMoneyNewGame      = 1000;
inline constexpr std::uint16_t kTownsAllUnvisited = 0x3FF;

// Migration errors are deliberately separate from SaveResult: parseSave is a
// size-only, lossless record parser, while the import boundary must identify a
// PS2 container and the ELF-confirmed BASLUS-20398 pathname.
enum class SaveErrorCode : std::uint8_t {
    InvalidSlot,
    InvalidSize,
    NotP2Image,
    WrongTitle,
    MalformedP2,
    IoError,
};

struct SaveError {
    SaveErrorCode code;
    std::string message;
};

// Local C++20 expected-like value.  This header cannot use std::expected
// (C++23), and the assets::expected template intentionally stays in assets.
template <class Value, class Error>
class expected {
public:
    expected(Value value) : storage_(std::move(value)) {}
    expected(Error error) : storage_(std::move(error)) {}
    bool has_value() const noexcept { return std::holds_alternative<Value>(storage_); }
    explicit operator bool() const noexcept { return has_value(); }
    Value& operator*() { return std::get<Value>(storage_); }
    const Value& operator*() const { return std::get<Value>(storage_); }
    Value* operator->() { return &std::get<Value>(storage_); }
    const Value* operator->() const { return &std::get<Value>(storage_); }
    const Error& error() const { return std::get<Error>(storage_); }
private:
    std::variant<Value, Error> storage_;
};

// One documented field region: [offset, offset+size) with a config/doc label.
struct SaveFieldRegion {
    std::size_t offset;
    std::size_t size;
    const char* label;
};

// Sorted, non-overlapping table of every documented region (drives parse /
// serialize / the unknown-range report).
const std::vector<SaveFieldRegion>& saveFieldRegions();

// Authored save model: the documented persistent fields, plus the full parsed
// image so undocumented bytes round-trip untouched.
struct SaveData {
    // --- documented fields (offsets in comments; see save_offset) ---
    std::array<std::uint8_t, kPartsCount> parts{};              // 0x00D
    std::array<std::uint8_t, 16> photosTaken{};                 // 0x508
    std::array<std::uint64_t, 2> stampsEarned{};                // 0x518
    std::uint32_t townEntered = 0;                              // 0x528 (guess)
    std::array<std::uint8_t, 16> storyFlags{};                  // 0x538 (128-bit)
    std::array<std::uint8_t, 16> coinsMissing{};                // 0x548
    std::array<std::uint8_t, 128> mailInbox{};                  // 0x5A8
    std::uint32_t mailCount = 0;                                // 0x628
    std::string playerName;                                     // 0x62C (<= 19 bytes, NUL-truncated)
    std::string currencyName;                                   // 0x63F (<= 18 bytes, NUL-truncated)
    std::uint8_t licence = 0;                                   // 0x651
    std::uint32_t money = 0;                                    // 0x654
    std::uint32_t mileageM = 0;                                 // 0x658
    std::array<std::uint8_t, 2> wgpTeammates{};                 // 0x674 / 0x676
    std::array<std::uint32_t, kLocationCount> unvisitedDoors{}; // 0xBD8
    std::array<std::uint8_t, kRaceResultCount> raceResults{};   // 0xFF0 (78-byte 3x26 grid; InitSaveSlotDefaults 0x2277b0)
    std::array<std::uint32_t, kBestTimeEntries> bestLap{};      // 0x1040
    std::array<std::uint32_t, kBestTimeEntries> bestTotal{};    // 0x1178
    std::uint32_t skiJumpBest = 0;                              // 0x12C8
    std::uint16_t townsUnvisited = 0;                           // 0x12DA
    std::uint8_t location = 0;                                  // 0x12DC
    std::uint8_t radioStation = 0;                              // 0x12DD
    std::uint16_t lastPhoto = 0;                                // 0x12E2
    std::array<std::uint16_t, kControllerActionCount> controllerRemap{};  // 0x12E8
    std::uint32_t stampQueueCount = 0;                          // 0x3300
    std::uint32_t wgpStagesRun = 0;                             // 0x3310

    // Full image as parsed (zeros for a default-constructed SaveData).
    // serializeSave() starts from this and overlays the documented fields, so
    // unknown regions are preserved byte-identical ("unchanged"), and a fresh
    // SaveData serializes with unknown regions zeroed ("zero").
    std::array<std::uint8_t, kSaveSize> raw{};

    // scene: low u32 word of the 128-bit story-flag set at 0x538 (6 = Q's
    // Factory menu, 2 = driving in town, 10 = racing, 3 = race over).
    std::uint32_t scene() const;
    void setScene(std::uint32_t v);
};

struct SaveResult {
    bool ok = false;
    SaveData data;
    std::vector<std::string> errors;         // why parse failed (empty when ok)
    std::vector<std::string> unknownFields;  // "0xXXXX-0xYYYY" ranges not covered by the
                                             // documented regions, plus "note:" entries for
                                             // known-but-unpinned sub-regions
};

// Parses a 13,384-byte progress-block image (memory-card file or RAM slice).
// Validation is size-only: no in-block magic/version is documented (see header).
// The input is never modified.
SaveResult parseSave(std::span<const std::uint8_t> bytes);

// Reproduces the 13,384-byte block: the retained image with every documented
// field overlaid from the typed members.
std::vector<std::uint8_t> serializeSave(const SaveData& save);

// Normalizes documented fields into native-valid ranges (clamps licence /
// radio / location / money cap 999999 / mail count / stamp queue / WGP stages /
// last photo, masks towns_unvisited to 0x3FF, truncates over-long names).
// Returns one string per normalization applied (empty = already clean).
std::vector<std::string> migrateToNative(SaveData& save);

// AUTHORED public-P2 container model. The game-specific directory and data
// filenames below are CONFIRMED ELF strings: /BASLUS-20398/BASLUS-20398.
// This does not validate NAND spare-area ECC: the game's own record I/O has no
// checksum/CRC (see save-layout.md), and ECC needs a real-card fixture before
// it can be validated without inventing acceptance rules.
struct P2SaveFile {
    std::string directoryName;
    std::string fileName;
    std::vector<std::uint8_t> bytes;
};

expected<std::vector<P2SaveFile>, SaveError>
scanP2RoadTripSaves(std::span<const std::uint8_t> p2Image);

// slot is the native host slot (0 or 1). It validates the target mapping; the
// returned SaveData is ready for the caller's existing native store.
expected<SaveData, SaveError>
migrateFromP2Image(std::span<const std::uint8_t> p2Image, int slot);

// Extracted BASLUS-20398 payload only. The span overload has no enclosing
// filename to verify, so callers must use a file selected by scanP2RoadTripSaves
// or use the path overload, which rejects a non-BASLUS-20398 basename.
expected<SaveData, SaveError>
migrateFromSaveFile(std::span<const std::uint8_t> extractedFile);
expected<SaveData, SaveError>
migrateFromSaveFile(const std::filesystem::path& extractedFile);

}  // namespace rt::native::gameplay
