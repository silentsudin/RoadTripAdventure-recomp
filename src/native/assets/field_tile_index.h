#pragma once

// Streaming id -> FLD filename resolution derived at runtime from the user's
// own data (never from committed game bytes): the ELF's ROM descriptor table
// (proven vaddr 0x0029BB50) cross-checked against physical file sizes, with
// the digit formula id = (A<<4)|(B<<2)|swap2(C) as tiebreaker.
// See docs/native-reference/field-topology-status.md
// "Tile identity & streaming (digit order RESOLVED)".

#include "field_converter.h"  // expected + FieldTileError

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace rt::native::assets {

inline constexpr std::size_t kFieldTileCount = 64;
inline constexpr std::uint32_t kFieldTileTableVaddr = 0x0029BB50;
inline constexpr std::uint32_t kFieldTileDescriptorTag = 0x00333388;

struct FieldTileIndexEntry {
    std::string fileName;              // e.g. "132.BIN", empty when unresolved
    std::uint32_t sectorOffset = 0;   // disc sector offset from the ROM table
    std::uint32_t sectorCount = 0;    // length in 2048-byte sectors
    bool sizeOnlyFallback = false;    // assigned by unique size, not formula
};

// 64 entries indexed by streaming id (the low 8 bits of the spawn tag
// id|0x390000 passed to loadFieldTile).
using FieldTileIndex = std::array<FieldTileIndexEntry, kFieldTileCount>;

// Digit formula: filename ABC.BIN -> streaming id.  swap2 swaps the two bits
// of C.  Returns -1 unless every digit is in range.
int fieldTileIdFromName(std::string_view fileName) noexcept;

// Resolves all 64 ids.  Errors: Truncated (ELF/table unreadable), BadHeader
// (ELF magic, segment lookup, or descriptor tag mismatch), MissingProvenance
// (any id left unmatched after formula + unique-size resolution).
expected<FieldTileIndex, FieldTileError> buildFieldTileIndex(
    const std::filesystem::path& discRoot);

}  // namespace rt::native::assets
