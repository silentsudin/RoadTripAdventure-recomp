// Runtime reader for the ROM-resident AI navigation route graphs
// (docs/native-reference/nav-tables.md).  Opens the user's own SLUS_203.98
// ELF and decodes {segment, transition} graph pairs through PT_LOAD
// translation; no table bytes are embedded in this repository.
//
// Static scene mode: a directory of {segmentVaddr, transitionVaddr} pairs at
// kNavStaticDirectoryVaddr, indexed by scene/type byte with the game's alias
// rules (11 -> 6, 16 -> 15) and null entries at 18/21/23.
// Course/entity mode: outer directory at kNavOuterDirectoryVaddr (T =
// (taskTag >> 10) & 0x1f) selects a null-terminated pair array; pair `car`
// serves that course/entity (FUN_0025fbc0 semantics; the game's car-1 flag
// adjustment stays with the caller).
//
// Segment records are 24 bytes of tile-local f32: endpoint A (x,z), endpoint
// B (x,z), plus two auxiliary floats with open semantics.  Transition records
// are 8 bytes of graph topology: the two segment indices evaluated at the
// node, two side-A candidate transitions, two side-B candidates, a follower
// output byte, and a reserved byte.  Segment count is recoverable from the
// pair as (transitionVaddr - segmentVaddr) / 24.
#pragma once

#include "field_converter.h"  // expected<>

#include <cstdint>
#include <filesystem>
#include <vector>

namespace rt::native::assets {

// Semantic constants from static analysis (FUN_0024fcd8 / FUN_00250068 /
// FUN_0025fbc0); values, not payload.
inline constexpr std::uint32_t kNavStaticDirectoryVaddr = 0x002bd578;
inline constexpr std::size_t kNavStaticPairCount = 24;
inline constexpr std::uint32_t kNavOuterDirectoryVaddr = 0x002d98a8;
inline constexpr std::size_t kNavOuterEntryCount = 32;
inline constexpr std::size_t kNavMaxPairsPerOuter = 8;  // observed ceiling

enum class NavRouteError {
    Io,          // ELF missing or unreadable
    BadElf,      // not a 32-bit LE ELF or no PT_LOAD covers the request
    NullPair,    // directory entry is null (scene has no nav graph)
    BadStride,   // transitionVaddr - segmentVaddr not a positive multiple of 24
    OutOfRange,  // transition byte references a segment index >= count
};

struct NavSegment {
    float ax = 0.f, az = 0.f;  // endpoint A, tile-local units
    float bx = 0.f, bz = 0.f;  // endpoint B
    float auxA = 0.f, auxB = 0.f;  // read by the follower; semantics OPEN
};

struct NavTransition {
    std::uint8_t segmentA = 0;
    std::uint8_t segmentB = 0;
    std::uint8_t sideACandidates[2] = {0, 0};
    std::uint8_t sideBCandidates[2] = {0, 0};
    std::uint8_t followerOutput = 0;
    std::uint8_t reserved = 0;
};

struct NavGraph {
    std::uint32_t segmentVaddr = 0;
    std::uint32_t transitionVaddr = 0;
    std::vector<NavSegment> segments;
    std::vector<NavTransition> transitions;
};

class NavRouteTables {
public:
    static expected<NavRouteTables, NavRouteError> open(const std::filesystem::path& elfPath);

    // Raw static-directory pair; false when the index is out of range.
    bool staticPair(std::size_t index, std::uint32_t& segmentOut, std::uint32_t& transitionOut) const;

    // Static scene graph with the game's alias rules applied (11 -> 6,
    // 16 -> 15); null pairs yield NavRouteError::NullPair.
    expected<NavGraph, NavRouteError> staticGraph(std::size_t index) const;

    // Outer directory value for T (0 when absent).
    std::uint32_t outerEntry(std::size_t t) const;

    // Pair `car` of outer entry T (raw index; no car-1 adjustment).
    expected<NavGraph, NavRouteError> courseGraph(std::size_t t, std::size_t car) const;

    // True when every endpoint lies in [-1.0, 1600.0] tile units (the
    // corpus-wide observed bounds, -1.0 sentinel included).
    static bool graphWithinTile(const NavGraph& graph);

private:
    NavRouteTables() = default;
    bool map(std::uint32_t vaddr, std::size_t span, std::size_t& offsetOut) const;
    expected<NavGraph, NavRouteError> decodePair(std::uint32_t segmentVaddr,
                                                 std::uint32_t transitionVaddr) const;

    std::vector<std::uint8_t> image_;
};

} // namespace rt::native::assets
