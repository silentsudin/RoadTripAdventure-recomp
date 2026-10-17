#include "assets/script_image.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <set>

namespace rt::native::assets {
namespace {

std::uint32_t le32(const std::vector<std::uint8_t>& d, std::size_t o) {
    return static_cast<std::uint32_t>(d[o]) | (static_cast<std::uint32_t>(d[o + 1]) << 8) |
           (static_cast<std::uint32_t>(d[o + 2]) << 16) | (static_cast<std::uint32_t>(d[o + 3]) << 24);
}

// Safety bound only; the real maximum level-1 run is 42 words.
constexpr std::size_t kMaxLevel1Words = 4096;

}  // namespace

std::optional<ScriptImage> ScriptImage::fromBytes(std::vector<std::uint8_t> elf) {
    if (elf.size() < 52 || elf[0] != 0x7f || elf[1] != 'E' || elf[2] != 'L' || elf[3] != 'F' ||
        elf[4] != 1 /* ELFCLASS32 */) {
        return std::nullopt;
    }
    ScriptImage image;
    image.image_ = std::move(elf);
    // The directory must be addressable, or this is the wrong ELF.
    std::size_t offset = 0;
    if (!image.map(kScriptTownDirectoryVaddr, kScriptTownDirectoryWords * 4, offset)) {
        return std::nullopt;
    }
    // dataEnd = end of the file-backed PT_LOAD that holds the directory.
    const std::vector<std::uint8_t>& e = image.image_;
    const std::uint32_t phoff = le32(e, 28);
    const std::uint16_t phentsize = static_cast<std::uint16_t>(e[42] | (e[43] << 8));
    const std::uint16_t phnum = static_cast<std::uint16_t>(e[44] | (e[45] << 8));
    for (std::uint16_t i = 0; i < phnum; ++i) {
        const std::size_t ph = phoff + static_cast<std::size_t>(i) * phentsize;
        if (ph + 32 > e.size() || le32(e, ph) != 1) continue;
        const std::uint32_t vaddr = le32(e, ph + 8);
        const std::uint32_t filesz = le32(e, ph + 16);
        if (kScriptTownDirectoryVaddr >= vaddr &&
            kScriptTownDirectoryVaddr < static_cast<std::uint64_t>(vaddr) + filesz) {
            image.dataEnd_ = vaddr + filesz;
        }
    }
    if (image.dataEnd_ == 0) return std::nullopt;
    return image;
}

std::optional<ScriptImage> ScriptImage::open(const std::filesystem::path& elfPath) {
    std::ifstream in(elfPath, std::ios::binary);
    if (!in) return std::nullopt;
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());
    return fromBytes(std::move(bytes));
}

bool ScriptImage::map(std::uint32_t vaddr, std::size_t span, std::size_t& offsetOut) const {
    const std::vector<std::uint8_t>& e = image_;
    const std::uint32_t phoff = le32(e, 28);
    const std::uint16_t phentsize = static_cast<std::uint16_t>(e[42] | (e[43] << 8));
    const std::uint16_t phnum = static_cast<std::uint16_t>(e[44] | (e[45] << 8));
    if (phentsize < 32) return false;
    for (std::uint16_t i = 0; i < phnum; ++i) {
        const std::size_t ph = phoff + static_cast<std::size_t>(i) * phentsize;
        if (ph + 32 > e.size()) return false;
        if (le32(e, ph) != 1 /* PT_LOAD */) continue;
        const std::uint32_t pOffset = le32(e, ph + 4);
        const std::uint32_t pVaddr = le32(e, ph + 8);
        const std::uint32_t pFilesz = le32(e, ph + 16);
        if (vaddr < pVaddr) continue;
        const std::uint64_t inSeg = static_cast<std::uint64_t>(vaddr) - pVaddr;
        if (inSeg + span > pFilesz || static_cast<std::uint64_t>(pOffset) + inSeg + span > e.size()) {
            continue;
        }
        offsetOut = pOffset + static_cast<std::size_t>(inSeg);
        return true;
    }
    return false;
}

bool ScriptImage::readU8(std::uint32_t vaddr, std::uint8_t& out) const {
    std::size_t off = 0;
    if (!map(vaddr, 1, off)) return false;
    out = image_[off];
    return true;
}

bool ScriptImage::readU32(std::uint32_t vaddr, std::uint32_t& out) const {
    std::size_t off = 0;
    if (!map(vaddr, 4, off)) return false;
    out = le32(image_, off);
    return true;
}

std::uint32_t ScriptImage::townTable(std::size_t town) const {
    if (town == 0 || town >= kScriptTownDirectoryWords) return 0;
    std::uint32_t v = 0;
    return readU32(kScriptTownDirectoryVaddr + static_cast<std::uint32_t>(town) * 4, v) ? v : 0;
}

std::uint32_t ScriptImage::shopTable(std::size_t town, std::size_t shop) const {
    const std::uint32_t l1 = townTable(town);
    if (l1 == 0) return 0;
    for (std::size_t i = 0; i <= shop && i < kMaxLevel1Words; ++i) {
        std::uint32_t v = 0;
        if (!readU32(l1 + static_cast<std::uint32_t>(i) * 4, v) || v == 0) return 0;
        if (i == shop) return v;
    }
    return 0;
}

ScriptCensus ScriptImage::census() const {
    ScriptCensus c;
    std::set<std::uint32_t> l2;
    for (std::size_t town = 1; town < kScriptTownDirectoryWords; ++town) {
        const std::uint32_t l1 = townTable(town);
        c.townTable.push_back(l1);
        std::uint32_t slots = 0;
        if (l1 != 0) {
            ++c.towns;
            for (std::size_t i = 0; i < kMaxLevel1Words; ++i) {
                std::uint32_t v = 0;
                if (!readU32(l1 + static_cast<std::uint32_t>(i) * 4, v) || v == 0) break;
                l2.insert(v);
                ++slots;
            }
        }
        c.slotsPerTown.push_back(slots);
        c.level1Slots += slots;
    }
    const std::vector<std::uint32_t> starts(l2.begin(), l2.end());
    for (std::size_t i = 0; i < starts.size(); ++i) {
        ScriptTableInfo info;
        info.vaddr = starts[i];
        const bool hasNext = i + 1 < starts.size();
        info.nextTableVaddr = hasNext ? starts[i + 1] : 0;
        const std::uint32_t limit = hasNext ? starts[i + 1] : dataEnd_;
        std::uint32_t n = 0;
        std::uint32_t last = 0;
        for (;;) {
            const std::uint32_t at = info.vaddr + n * 4;
            if (at + 4 > limit) break;
            if (!readU32(at, last) || !isScriptPointer(last)) break;
            ++n;
        }
        info.length = n;
        const std::uint32_t endAt = info.vaddr + n * 4;
        if (!hasNext) {
            info.end = ScriptTableEnd::Last;
        } else if (endAt == limit) {
            info.end = ScriptTableEnd::NextTable;
        } else {
            std::uint32_t w = 0;
            info.end = (readU32(endAt, w) && w == 0) ? ScriptTableEnd::ZeroPad
                                                     : ScriptTableEnd::Foreign;
        }
        c.tables.push_back(info);
    }
    return c;
}

std::uint32_t ScriptImage::tableLength(const ScriptCensus& census, std::uint32_t vaddr) {
    const auto it = std::lower_bound(
        census.tables.begin(), census.tables.end(), vaddr,
        [](const ScriptTableInfo& t, std::uint32_t v) { return t.vaddr < v; });
    return (it != census.tables.end() && it->vaddr == vaddr) ? it->length : 0;
}

}  // namespace rt::native::assets
