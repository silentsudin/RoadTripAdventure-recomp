#pragma once
// Save format v2: an integrity-checked native container around the unchanged
// 0x3448-byte Adventure progress block (see save.h / docs/native-reference/
// save-layout.md and the "v2" section of save-blocks.md).
//
//   v1 = the raw 13,384-byte block (what save.h parses; PS2 file has no
//        magic / version / checksum).
//   v2 = Header(32) | RecordTable(8 * N) | Image(0x3448) | TLV(tlvSize)
//
// Header (little-endian):
//   +0  u8[4] magic "RTN2"
//   +4  u16   version (2)
//   +6  u16   recordCount (kRecordCount)
//   +8  u32   imageSize (kSaveSize)
//   +12 u32   tlvSize
//   +16 u32   tlvCrc32       (CRC-32/IEEE of the TLV bytes)
//   +20 u32   saveSeq        (generation counter, 1 after migration)
//   +24 u32   reserved (0)
//   +28 u32   fileCrc32      (CRC-32/IEEE of the whole file, this field as 0)
// Record table entry: u16 id, u16 flags (0), u32 crc32 of the record's image slice.
// Records partition the image contiguously (recordSpecs()).  A record whose
// crc32 does not match is reset to that record's new-game default; other
// records survive.  Unknown TLV tags are preserved byte-for-byte.
//
// Defaults come from the new-game initializer InitSaveSlotDefaults 0x2277B0 /
// InitProgressRamDefaults 0x229FD8 (word stores mapped to image offsets as
// address - 0x177F760).  Image bytes that initializer leaves unmodelled here
// stay zero (see save-blocks.md "v2" for the list).

#include "gameplay/save.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace rt::native::gameplay::save_v2 {

inline constexpr std::size_t kHeaderSize = 32;
inline constexpr std::size_t kRecordEntrySize = 8;
inline constexpr std::uint16_t kVersion = 2;
inline constexpr std::size_t kRecordCount = 14;
inline constexpr std::size_t kHdrTlvSize = 12;
inline constexpr std::size_t kHdrTlvCrc = 16;  // tlv crc32 field
inline constexpr std::size_t kHdrFileCrc = 28;

using Image = std::array<std::uint8_t, kSaveSize>;

struct RecordSpec {
    std::uint16_t id;
    std::size_t offset;  // within the 0x3448 image
    std::size_t size;
    const char* name;
};

// Contiguous partition of [0, kSaveSize), kRecordCount entries.
const std::array<RecordSpec, kRecordCount>& recordSpecs();

// New-game image (authored from the ELF initializer; see header note).
const Image& newGameImage();

std::uint32_t crc32(std::span<const std::uint8_t> bytes);

struct Container {
    Image image = newGameImage();
    std::vector<std::uint8_t> tlv;  // sequence of {u16 tag, u16 len, bytes}
    std::uint32_t saveSeq = 1;
};

enum class Status : std::uint8_t {
    Ok,         // every checksum valid
    Recovered,  // header readable; some checksum failed, damaged records/TLV reset
    Truncated,  // shorter than the header demands
    BadMagic,
    BadVersion,
    BadLayout,  // recordCount / imageSize / total length disagree with the format
};

struct LoadReport {
    Status status = Status::BadMagic;
    bool fileCrcOk = false;
    bool tlvDropped = false;
    std::vector<std::uint16_t> resetRecords;  // ids reset to default
};

std::vector<std::uint8_t> serializeV2(const Container& c);

// Strict: true only if the container is structurally valid and fileCrc, every
// record crc and the TLV crc all match.
bool verifyV2(std::span<const std::uint8_t> bytes);

// Lenient: always returns a consistent Container (new-game defaults when the
// header itself is unusable).
Container loadV2(std::span<const std::uint8_t> bytes, LoadReport& report);

enum class Format : std::uint8_t { Unknown, V1Raw, V2 };
Format detectFormat(std::span<const std::uint8_t> bytes);

// v1 (raw 0x3448 block) -> v2.  Payload bytes preserved exactly; nullopt if the
// input is not exactly kSaveSize bytes.
std::optional<std::vector<std::uint8_t>> migrateV1ToV2(std::span<const std::uint8_t> v1);

// Loads either format (v1 raw is wrapped without modification).
Container loadAny(std::span<const std::uint8_t> bytes, LoadReport& report);

// Typed view with native-range normalization (save.h migrateToNative).
SaveData toSaveData(const Container& c, std::vector<std::string>* normalizations = nullptr);

// TLV helpers (set replaces an existing tag, appends otherwise).
bool tlvWellFormed(std::span<const std::uint8_t> tlv);
void tlvSet(Container& c, std::uint16_t tag, std::span<const std::uint8_t> value);
std::optional<std::vector<std::uint8_t>> tlvFind(const Container& c, std::uint16_t tag);

}  // namespace rt::native::gameplay::save_v2
