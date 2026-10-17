// Reader for the ELF-resident town/shop event-script image
// (docs/native-reference/script-vm.md).  The scripts and their dialogue text
// are not disc files: they live in the ELF .data segment behind a three-level
// pointer walk, and this module only *locates* them.  No script/text bytes
// are embedded in this repository; callers read them from the user's own
// SLUS_203.98 through PT_LOAD translation.
//
//   level 0  town directory   0x2A7ED0   22 words: [0] = 0, [1..21] = per-town
//                                         level-1 table vaddrs
//   level 1  per-town table   word[shop] = level-2 table vaddr, terminated by
//                                         the first zero word (the walk used
//                                         by FUN_0026BE50 @0x26BF00..)
//   level 2  shop table       word[0] = name string, word[1] = entry script,
//                                         word[2..] = branch targets
//                                         (dialogueTask 0x23B168: cursor =
//                                         table[1]; branch = table[idx])
//   level 3  script bytes     control prefix (ops 1..0x1D) then text
//
// Level-2 tables have no stored length.  Observed layout (all 307): they are
// packed back to back with the per-town level-1 tables in 0x2A4450..0x2A7EC8;
// a table's length is the run of leading words that are script-region
// pointers (>= kScriptRegionLo, inside the file-backed data segment), clipped
// at the next level-2 table start.  Verified against the census in the test.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

namespace rt::native::assets {

// Spawners 0x2105B8 / 0x22AEB0 (decomp 002105b8.c:373, 0022aeb0.c:130):
//   gs_tasks_spawn(ctx, 0x23B168,
//       *(u32*)(shop21*4 + *(u32*)(0x2A7ED0 + (s8)shop23*4)));
inline constexpr std::uint32_t kScriptTownDirectoryVaddr = 0x002A7ED0;
inline constexpr std::size_t kScriptTownDirectoryWords = 22;  // word 0 is 0
inline constexpr std::size_t kScriptTownCount = 21;
inline constexpr std::uint32_t kDialogueTaskVaddr = 0x0023B168;
inline constexpr std::uint32_t kInterpretScriptCommandsVaddr = 0x0023AD00;
// First script/text pointer observed is 0x2F1300 and the first non-script
// pointer seen inside a table run is the level-1 region (< 0x2A8000), so any
// bound in [0x2A8000, 0x2F1300] yields identical lengths (checked).
inline constexpr std::uint32_t kScriptRegionLo = 0x002F0000;

enum class ScriptTableEnd : std::uint8_t {
    NextTable,  // run ends exactly where the next level-2 table starts
    ZeroPad,    // run ends at a zero alignment word
    Foreign,    // run ends at a word that is not a script pointer (level-1 data)
    Last,       // last table in the region, ended by data-end
};

struct ScriptTableInfo {
    std::uint32_t vaddr = 0;
    std::uint32_t length = 0;  // words (name + entry + branch targets)
    ScriptTableEnd end = ScriptTableEnd::Last;
    std::uint32_t nextTableVaddr = 0;  // 0 when last
};

struct ScriptCensus {
    std::size_t towns = 0;                       // non-null town directory words
    std::vector<std::uint32_t> townTable;        // [1..21] level-1 vaddrs
    std::vector<std::uint32_t> slotsPerTown;     // index 0 = town 1
    std::size_t level1Slots = 0;                 // sum of slotsPerTown
    std::vector<ScriptTableInfo> tables;         // distinct level-2, sorted by vaddr
};

class ScriptImage {
public:
    static std::optional<ScriptImage> open(const std::filesystem::path& elfPath);
    static std::optional<ScriptImage> fromBytes(std::vector<std::uint8_t> elf);

    bool readU8(std::uint32_t vaddr, std::uint8_t& out) const;
    bool readU32(std::uint32_t vaddr, std::uint32_t& out) const;

    // End (exclusive vaddr) of the file-backed PT_LOAD holding the directory.
    std::uint32_t dataEnd() const { return dataEnd_; }
    bool isScriptPointer(std::uint32_t vaddr) const {
        return vaddr >= kScriptRegionLo && vaddr < dataEnd_;
    }

    // Level-1 table vaddr of `town` (1..21); 0 when out of range.
    std::uint32_t townTable(std::size_t town) const;
    // Level-2 vaddr of (town, shop); 0 when absent (the game's walk stops at
    // the first zero word, and so does this).
    std::uint32_t shopTable(std::size_t town, std::size_t shop) const;

    // Full census (distinct level-2 tables with lengths).
    ScriptCensus census() const;

    // Length (words) of the level-2 table starting at `vaddr`, using `census`.
    static std::uint32_t tableLength(const ScriptCensus& census, std::uint32_t vaddr);

private:
    ScriptImage() = default;
    bool map(std::uint32_t vaddr, std::size_t span, std::size_t& offsetOut) const;

    std::vector<std::uint8_t> image_;
    std::uint32_t dataEnd_ = 0;
};

}  // namespace rt::native::assets
