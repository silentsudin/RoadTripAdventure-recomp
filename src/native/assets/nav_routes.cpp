#include "assets/nav_routes.h"

#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>

namespace rt::native::assets {
namespace {

std::uint32_t readU32LE(const std::vector<std::uint8_t>& data, std::size_t offset) {
    return static_cast<std::uint32_t>(data[offset]) |
           (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(data[offset + 3]) << 24);
}

float readFloatLE(const std::vector<std::uint8_t>& data, std::size_t offset) {
    const std::uint32_t bits = readU32LE(data, offset);
    float value;
    static_assert(sizeof(float) == 4, "IEEE-754 single assumed");
    static_assert(std::numeric_limits<float>::is_iec559, "IEEE-754 single assumed");
    static_cast<void>(std::memcpy(&value, &bits, sizeof(value)));
    return value;
}

constexpr std::size_t kSegmentBytes = 24;
constexpr std::size_t kTransitionBytes = 8;

} // namespace

expected<NavRouteTables, NavRouteError> NavRouteTables::open(const std::filesystem::path& elfPath) {
    std::ifstream input(elfPath, std::ios::binary);
    if (!input) return NavRouteError::Io;
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                    std::istreambuf_iterator<char>());
    if (bytes.size() < 52 || bytes[0] != 0x7f || bytes[1] != 'E' || bytes[2] != 'L' ||
        bytes[3] != 'F' || bytes[4] != 1 /* ELFCLASS32 */) {
        return NavRouteError::BadElf;
    }
    NavRouteTables tables;
    tables.image_ = std::move(bytes);
    // Both directories must be addressable, or this is the wrong ELF.
    std::size_t offset = 0;
    if (!tables.map(kNavStaticDirectoryVaddr, kNavStaticPairCount * 8, offset) ||
        !tables.map(kNavOuterDirectoryVaddr, kNavOuterEntryCount * 4, offset)) {
        return NavRouteError::BadElf;
    }
    return tables;
}

bool NavRouteTables::map(std::uint32_t vaddr, std::size_t span, std::size_t& offsetOut) const {
    const std::vector<std::uint8_t>& elf = image_;
    const std::uint32_t ePhoff = readU32LE(elf, 28);
    const std::uint16_t ePhentsize = static_cast<std::uint16_t>(elf[42] | (elf[43] << 8));
    const std::uint16_t ePhnum = static_cast<std::uint16_t>(elf[44] | (elf[45] << 8));
    if (ePhentsize < 32) return false;
    for (std::uint16_t i = 0; i < ePhnum; ++i) {
        const std::size_t ph = ePhoff + static_cast<std::size_t>(i) * ePhentsize;
        if (ph + 32 > elf.size()) return false;
        if (readU32LE(elf, ph) != 1 /* PT_LOAD */) continue;
        const std::uint32_t pOffset = readU32LE(elf, ph + 4);
        const std::uint32_t pVaddr = readU32LE(elf, ph + 8);
        const std::uint32_t pFilesz = readU32LE(elf, ph + 16);
        if (vaddr < pVaddr) continue;
        const std::uint64_t inSegment = static_cast<std::uint64_t>(vaddr) - pVaddr;
        // Delta derived per phdr — never assume a fixed vaddr-file relation.
        if (inSegment + span > pFilesz ||
            static_cast<std::uint64_t>(pOffset) + inSegment + span > elf.size()) {
            continue;
        }
        offsetOut = pOffset + static_cast<std::size_t>(inSegment);
        return true;
    }
    return false;
}

bool NavRouteTables::staticPair(std::size_t index, std::uint32_t& segmentOut,
                               std::uint32_t& transitionOut) const {
    if (index >= kNavStaticPairCount) return false;
    std::size_t offset = 0;
    const std::size_t span = kNavStaticPairCount * 8;
    if (!map(kNavStaticDirectoryVaddr, span, offset)) return false;
    segmentOut = readU32LE(image_, offset + index * 8);
    transitionOut = readU32LE(image_, offset + index * 8 + 4);
    return true;
}

expected<NavGraph, NavRouteError> NavRouteTables::staticGraph(std::size_t index) const {
    if (index == 11) index = 6;   // FUN_0024fcd8 alias rules
    if (index == 16) index = 15;
    std::uint32_t segment = 0, transition = 0;
    if (!staticPair(index, segment, transition)) return NavRouteError::NullPair;
    if (segment == 0 || transition == 0) return NavRouteError::NullPair;
    return decodePair(segment, transition);
}

std::uint32_t NavRouteTables::outerEntry(std::size_t t) const {
    if (t >= kNavOuterEntryCount) return 0;
    std::size_t offset = 0;
    if (!map(kNavOuterDirectoryVaddr, kNavOuterEntryCount * 4, offset)) return 0;
    return readU32LE(image_, offset + t * 4);
}

expected<NavGraph, NavRouteError> NavRouteTables::courseGraph(std::size_t t, std::size_t car) const {
    const std::uint32_t arrayVaddr = outerEntry(t);
    if (arrayVaddr == 0) return NavRouteError::NullPair;
    if (car >= kNavMaxPairsPerOuter) return NavRouteError::NullPair;
    std::uint32_t segment = 0, transition = 0;
    // Pair arrays are null-terminated; read the requested slot directly.
    std::size_t offset = 0;
    if (!map(arrayVaddr, kNavMaxPairsPerOuter * 8, offset)) {
        // Short arrays still map partially; retry with the slot span.
        if (!map(arrayVaddr + static_cast<std::uint32_t>(car) * 8, 8, offset)) {
            return NavRouteError::BadElf;
        }
    }
    segment = readU32LE(image_, offset + car * 8);
    transition = readU32LE(image_, offset + car * 8 + 4);
    if (segment == 0 || transition == 0) return NavRouteError::NullPair;
    return decodePair(segment, transition);
}

expected<NavGraph, NavRouteError> NavRouteTables::townGraph(int sceneId) const {
    // FUN_0020e708 @0x20e750..0x20e764: the scene task executes scripted
    // scene id 0 as id 9 (My Garage loads as My City).  T=0 and T=9 share
    // the same outer row on the disc, so the remap is semantic belt and
    // braces; apply it to match the game regardless.
    if (sceneId == 0) sceneId = 9;
    if (sceneId < 0 || static_cast<unsigned>(sceneId) >= kNavOuterEntryCount) {
        return NavRouteError::NullPair;
    }
    return courseGraph(static_cast<std::size_t>(sceneId), 0);
}

expected<NavGraph, NavRouteError> NavRouteTables::decodePair(std::uint32_t segmentVaddr,
                                                             std::uint32_t transitionVaddr) const {
    if (transitionVaddr <= segmentVaddr ||
        (transitionVaddr - segmentVaddr) % kSegmentBytes != 0) {
        return NavRouteError::BadStride;
    }
    const std::size_t count = (transitionVaddr - segmentVaddr) / kSegmentBytes;
    std::size_t segmentOffset = 0, transitionOffset = 0;
    if (!map(segmentVaddr, count * kSegmentBytes, segmentOffset) ||
        !map(transitionVaddr, count * kTransitionBytes, transitionOffset)) {
        return NavRouteError::BadElf;
    }
    NavGraph graph;
    graph.segmentVaddr = segmentVaddr;
    graph.transitionVaddr = transitionVaddr;
    graph.segments.reserve(count);
    graph.transitions.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t s = segmentOffset + i * kSegmentBytes;
        graph.segments.push_back({readFloatLE(image_, s), readFloatLE(image_, s + 4),
                                 readFloatLE(image_, s + 8), readFloatLE(image_, s + 12),
                                 readFloatLE(image_, s + 16), readFloatLE(image_, s + 20)});
        const std::size_t t = transitionOffset + i * kTransitionBytes;
        NavTransition transition;
        transition.segmentA = image_[t];
        transition.segmentB = image_[t + 1];
        transition.sideACandidates[0] = image_[t + 2];
        transition.sideACandidates[1] = image_[t + 3];
        transition.sideBCandidates[0] = image_[t + 4];
        transition.sideBCandidates[1] = image_[t + 5];
        transition.followerOutput = image_[t + 6];
        transition.reserved = image_[t + 7];
        graph.transitions.push_back(transition);
    }
    for (const NavTransition& transition : graph.transitions) {
        if (transition.segmentA >= count || transition.segmentB >= count) {
            return NavRouteError::OutOfRange;
        }
    }
    return graph;
}

bool NavRouteTables::graphWithinTile(const NavGraph& graph) {
    // Corpus-wide observed bounds across all 21 static graphs (2,334
    // segments): endpoints sit in [-1.0, 1570.0] within the 1600-unit tile,
    // -1.0 acting as a sentinel.
    constexpr float kMin = -1.0f - 1e-3f;
    constexpr float kMax = 1600.0f + 1e-3f;
    for (const NavSegment& segment : graph.segments) {
        for (float v : {segment.ax, segment.az, segment.bx, segment.bz}) {
            if (!(v >= kMin && v <= kMax)) return false;
        }
    }
    return true;
}

} // namespace rt::native::assets
