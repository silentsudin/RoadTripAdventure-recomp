#include "assets/field_tile_index.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <string_view>
#include <vector>

namespace rt::native::assets {
namespace {

constexpr std::size_t kDescriptorBytes = 12;
constexpr std::uint32_t kSectorBytes = 2048;

std::uint32_t readU32LE(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

std::uint32_t swap2(std::uint32_t c) noexcept {
    return ((c >> 1) & 1u) | ((c & 1u) << 1);
}

std::uint32_t ceilSectors(std::uint64_t bytes) noexcept {
    return static_cast<std::uint32_t>((bytes + kSectorBytes - 1) / kSectorBytes);
}

// Maps an ELF virtual address to a file offset through PT_LOAD (type 1)
// program headers; sufficient for the statically-linked EE kernel-format
// ELF the game ships as SLUS_203.98.
bool elfFindFileOffset(const std::vector<std::uint8_t>& elf, std::uint32_t vaddr,
                      std::size_t span, std::size_t& offsetOut) {
    if (elf.size() < 52 || elf[0] != 0x7f || elf[1] != 'E' || elf[2] != 'L' || elf[3] != 'F' ||
        elf[4] != 1 /* ELFCLASS32 */) {
        return false;
    }
    const std::uint32_t ePhoff = readU32LE(elf, 28);
    const std::uint16_t ePhentsize =
        static_cast<std::uint16_t>(elf[42] | (elf[43] << 8));
    const std::uint16_t ePhnum =
        static_cast<std::uint16_t>(elf[44] | (elf[45] << 8));
    if (ePhentsize < 32) {
        return false;
    }
    for (std::uint16_t i = 0; i < ePhnum; ++i) {
        const std::size_t ph = ePhoff + static_cast<std::size_t>(i) * ePhentsize;
        if (ph + 32 > elf.size()) {
            return false;
        }
        const std::uint32_t pType = readU32LE(elf, ph);
        const std::uint32_t pOffset = readU32LE(elf, ph + 4);
        const std::uint32_t pVaddr = readU32LE(elf, ph + 8);
        const std::uint32_t pFilesz = readU32LE(elf, ph + 16);
        if (pType != 1 || vaddr < pVaddr) {
            continue;
        }
        const std::uint64_t inSeg = static_cast<std::uint64_t>(vaddr) - pVaddr;
        if (inSeg + span > pFilesz || pOffset + inSeg + span > elf.size()) {
            continue;
        }
        offsetOut = pOffset + static_cast<std::size_t>(inSeg);
        return true;
    }
    return false;
}

bool stripExtension(std::string_view name, std::string_view& stemOut) {
    const std::size_t dot = name.find_last_of('.');
    if (dot == std::string_view::npos) {
        return false;
    }
    stemOut = name.substr(0, dot);
    return true;
}

}  // namespace

int fieldTileIdFromName(std::string_view fileName) noexcept {
    std::string_view stem;
    if (!stripExtension(fileName, stem) || stem.size() != 3) {
        return -1;
    }
    for (const char c : stem) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return -1;
        }
    }
    const std::uint32_t a = static_cast<std::uint32_t>(stem[0] - '0');
    const std::uint32_t b = static_cast<std::uint32_t>(stem[1] - '0');
    const std::uint32_t c = static_cast<std::uint32_t>(stem[2] - '0');
    if (a > 3 || b > 3 || c > 3) {
        return -1;
    }
    return static_cast<int>((a << 4) | (b << 2) | swap2(c));
}

expected<FieldTileIndex, FieldTileError> buildFieldTileIndex(
    const std::filesystem::path& discRoot) {
    std::ifstream elfStream(discRoot / "SLUS_203.98", std::ios::binary);
    if (!elfStream) {
        return FieldTileError::Truncated;
    }
    const std::vector<std::uint8_t> elf((std::istreambuf_iterator<char>(elfStream)),
                                       std::istreambuf_iterator<char>());
    std::size_t tableOffset = 0;
    if (!elfFindFileOffset(elf, kFieldTileTableVaddr, kFieldTileCount * kDescriptorBytes,
                           tableOffset)) {
        return FieldTileError::BadHeader;
    }

    struct Descriptor {
        std::uint32_t sectorOffset = 0;
        std::uint32_t sectorCount = 0;
    };
    std::array<Descriptor, kFieldTileCount> descriptors{};
    for (std::size_t id = 0; id < kFieldTileCount; ++id) {
        const std::size_t at = tableOffset + id * kDescriptorBytes;
        if (readU32LE(elf, at) != kFieldTileDescriptorTag) {
            return FieldTileError::BadHeader;
        }
        descriptors[id].sectorOffset = readU32LE(elf, at + 4);
        descriptors[id].sectorCount = readU32LE(elf, at + 8);
    }

    // Sector-count -> candidate file stems from the FLD directory.
    std::map<std::uint32_t, std::vector<std::string>> bySectorCount;
    const std::filesystem::path fldDir = discRoot / "FLD";
    if (!std::filesystem::exists(fldDir)) {
        return FieldTileError::Truncated;
    }
    for (const auto& entry : std::filesystem::directory_iterator(fldDir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".BIN") {
            continue;
        }
        const std::string name = entry.path().filename().string();
        const std::uintmax_t size = entry.file_size();
        if (size > std::numeric_limits<std::uint32_t>::max()) {
            continue;
        }
        bySectorCount[ceilSectors(size)].push_back(name);
    }
    for (auto& [count, names] : bySectorCount) {
        std::sort(names.begin(), names.end());
    }

    // Optional exact-LBA sidecar shipped with PS2Recomp extracts:
    // "<dir>/<name>\t<lba>\t<sizeBytes>".  An LBA + sectorCount agreement with
    // the descriptor is definitive and preferred over size+formula matching
    // (see docs/native-reference/asset-map.md).
    std::map<std::uint32_t, std::pair<std::uint32_t, std::string>> lbnBySector;
    if (std::ifstream lbnStream(discRoot / ".lbn_map.tsv"); lbnStream) {
        std::string line;
        while (std::getline(lbnStream, line)) {
            const std::size_t t1 = line.find('\t');
            const std::size_t t2 = line.find('\t', t1 + 1);
            if (t1 == std::string::npos || t2 == std::string::npos) {
                continue;
            }
            const std::string_view path(line);
            constexpr std::string_view kFldPrefix = "FLD/";
            if (!path.starts_with(kFldPrefix)) {
                continue;
            }
            const std::size_t slash = line.find_last_of('/');
            const std::size_t stemStart =
                (slash == std::string::npos) ? 0 : slash + 1;
            const std::string stem = line.substr(stemStart, t1 - stemStart);
            const unsigned long lba =
                std::strtoul(line.substr(t1 + 1, t2 - t1 - 1).c_str(), nullptr, 10);
            const unsigned long bytes = std::strtoul(line.substr(t2 + 1).c_str(), nullptr, 10);
            lbnBySector[static_cast<std::uint32_t>(lba)] = {ceilSectors(bytes), stem};
        }
    }

    FieldTileIndex index{};
    std::vector<std::string> used;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> ranges;
    for (std::size_t id = 0; id < kFieldTileCount; ++id) {
        const Descriptor& desc = descriptors[id];
        std::string chosen;
        bool fallback = false;
        bool exact = false;
        if (auto mit = lbnBySector.find(desc.sectorOffset); mit != lbnBySector.end() &&
            mit->second.first == desc.sectorCount &&
            std::find(used.begin(), used.end(), mit->second.second) == used.end()) {
            chosen = mit->second.second;
            exact = true;
        }
        if (!exact) {
            auto it = bySectorCount.find(desc.sectorCount);
            if (it == bySectorCount.end()) {
                continue;  // Provenance gap; flagged below.
            }
            // Primary rule: the digit formula identifies exactly one name per
            // id.  Fallback: a single remaining name with matching sector
            // count.
            const std::vector<std::string>& candidates = it->second;
            int formulaMatches = 0;
            for (const std::string& name : candidates) {
                if (std::find(used.begin(), used.end(), name) != used.end()) {
                    continue;
                }
                if (fieldTileIdFromName(name) == static_cast<int>(id)) {
                    chosen = name;
                    ++formulaMatches;
                }
            }
            if (formulaMatches != 1) {
                chosen.clear();
                for (const std::string& name : candidates) {
                    if (std::find(used.begin(), used.end(), name) == used.end()) {
                        if (!chosen.empty()) {  // Ambiguous even by size.
                            chosen.clear();
                            break;
                        }
                        chosen = name;
                    }
                }
                fallback = !chosen.empty();
            }
        }
        if (chosen.empty()) {
            continue;
        }
        used.push_back(chosen);
        index[id].fileName = std::move(chosen);
        index[id].sectorOffset = desc.sectorOffset;
        index[id].sectorCount = desc.sectorCount;
        index[id].sizeOnlyFallback = fallback;
        index[id].fromLbnMap = exact;
        ranges.emplace_back(desc.sectorOffset, desc.sectorCount);
    }
    // Disc LBA ranges must be pairwise disjoint (proven invariant of the
    // real table; a collision means the extraction or table is corrupt).
    std::sort(ranges.begin(), ranges.end());
    for (std::size_t i = 1; i < ranges.size(); ++i) {
        if (ranges[i].first < ranges[i - 1].first + ranges[i - 1].second) {
            return FieldTileError::BadHeader;
        }
    }
    if (used.size() != kFieldTileCount) {
        return FieldTileError::MissingProvenance;
    }
    return index;
}

}  // namespace rt::native::assets
