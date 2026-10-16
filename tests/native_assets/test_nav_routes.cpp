// Nav-route runtime reader tests: synthetic minimal ELF plus disc-gated
// pins against the user's own SLUS_203.98 (docs/native-reference/nav-tables.md).
// No game bytes are committed; the disc section only pins counts, vaddrs and
// a few endpoint values already published in the doc.
#include "../../src/native/assets/nav_routes.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>

using namespace rt::native::assets;

static int g_fail = 0;
static int g_pass = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } else { ++g_pass; } } while (0)

namespace {

void putU32(std::vector<std::uint8_t>& d, std::size_t offset, std::uint32_t v) {
    d[offset] = static_cast<std::uint8_t>(v);
    d[offset + 1] = static_cast<std::uint8_t>(v >> 8);
    d[offset + 2] = static_cast<std::uint8_t>(v >> 16);
    d[offset + 3] = static_cast<std::uint8_t>(v >> 24);
}

void putFloat(std::vector<std::uint8_t>& d, std::size_t offset, float v) {
    std::uint32_t bits;
    std::memcpy(&bits, &v, sizeof(bits));
    putU32(d, offset, bits);
}

std::filesystem::path writeFakeElf() {
    // One PT_LOAD covering both directory vaddrs; directory and one graph
    // live inside it.  Layout (vaddr - loadVaddr = file offset - 84).
    const std::uint32_t loadVaddr = 0x2bd000;
    const std::size_t payloadBytes = 0x2d9930 - loadVaddr;  // covers outer directory
    const std::size_t fileSize = 84 + payloadBytes;
    std::vector<std::uint8_t> elf(fileSize, 0);
    elf[0] = 0x7f; elf[1] = 'E'; elf[2] = 'L'; elf[3] = 'F'; elf[4] = 1; elf[5] = 1; elf[6] = 1;
    putU32(elf, 28, 52);                       // e_phoff
    elf[42] = 32; elf[43] = 0;                // e_phentsize
    elf[44] = 1; elf[45] = 0;                // e_phnum
    // PT_LOAD
    putU32(elf, 52, 1);                       // p_type
    putU32(elf, 56, 84);                      // p_offset
    putU32(elf, 60, loadVaddr);              // p_vaddr
    putU32(elf, 68, static_cast<std::uint32_t>(payloadBytes));  // p_filesz
    const auto at = [&](std::uint32_t vaddr) { return 84 + (vaddr - loadVaddr); };
    // Static directory: pair 0 -> 3-segment graph; pair 2 null.
    putU32(elf, at(kNavStaticDirectoryVaddr), 0x2bd800);
    putU32(elf, at(kNavStaticDirectoryVaddr) + 4, 0x2bd800 + 3 * 24);
    const std::uint32_t segs[3][4] = {{500, 505, 510, 515}, {520, 525, 530, 535}, {540, 545, 550, 555}};
    for (int i = 0; i < 3; ++i) {
        putFloat(elf, at(0x2bd800) + i * 24, static_cast<float>(segs[i][0]));
        putFloat(elf, at(0x2bd800) + i * 24 + 4, static_cast<float>(segs[i][1]));
        putFloat(elf, at(0x2bd800) + i * 24 + 8, static_cast<float>(segs[i][2]));
        putFloat(elf, at(0x2bd800) + i * 24 + 12, static_cast<float>(segs[i][3]));
        putU32(elf, at(0x2bd800) + i * 24 + 16, 0);
        putU32(elf, at(0x2bd800) + i * 24 + 20, 0);
        const std::size_t tr = at(0x2bd800 + 3 * 24) + i * 8;
        elf[tr] = static_cast<std::uint8_t>(i);        // segmentA < 3
        elf[tr + 1] = static_cast<std::uint8_t>((i + 1) % 3);
        elf[tr + 2] = static_cast<std::uint8_t>(i);
        elf[tr + 3] = static_cast<std::uint8_t>(i);
        elf[tr + 4] = static_cast<std::uint8_t>((i + 2) % 3);
        elf[tr + 5] = static_cast<std::uint8_t>(0);
        elf[tr + 6] = 0;
        elf[tr + 7] = 0;
    }
    // Outer directory: T0 -> pair array with one pair then a null terminator.
    putU32(elf, at(kNavOuterDirectoryVaddr), 0x2bd900);
    putU32(elf, at(0x2bd900), 0x2bd800);
    putU32(elf, at(0x2bd900) + 4, 0x2bd800 + 3 * 24);
    // pair 1 stays zero -> null terminator.
    // T=1 -> empty pair array (immediate terminator; zero-filled), T=9 ->
    // T0's array (the scene 0 -> 9 alias target); T=2 stays a null row.
    putU32(elf, at(kNavOuterDirectoryVaddr) + 4, 0x2bd940);
    putU32(elf, at(kNavOuterDirectoryVaddr) + 36, 0x2bd900);

    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "rt_native_nav_fake.elf";
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(elf.data()), static_cast<std::streamsize>(elf.size()));
    return path;
}

void testSynthetic() {
    const std::filesystem::path path = writeFakeElf();
    auto tables = NavRouteTables::open(path);
    CHECK(static_cast<bool>(tables));
    if (!tables) { std::filesystem::remove(path); return; }

    std::uint32_t seg = 0, trans = 0;
    CHECK((*tables).staticPair(0, seg, trans));
    CHECK(seg == 0x2bd800 && trans == 0x2bd800u + 72);
    CHECK(!(*tables).staticPair(kNavStaticPairCount, seg, trans));

    auto graph = (*tables).staticGraph(0);
    CHECK(static_cast<bool>(graph));
    if (graph) {
        CHECK(graph->segments.size() == 3 && graph->transitions.size() == 3);
        CHECK(graph->segments[0].ax == 500.f && graph->segments[2].bz == 555.f);
        CHECK(graph->transitions[1].segmentA == 1 && graph->transitions[1].segmentB == 2);
        CHECK(NavRouteTables::graphWithinTile(*graph));
    }
    // Null pair and the game's alias rules.
    CHECK(!(*tables).staticGraph(1) && (*tables).staticGraph(1).error() == NavRouteError::NullPair);
    CHECK(!(*tables).staticGraph(11) && (*tables).staticGraph(11).error() == (*tables).staticGraph(6).error());
    CHECK(!(*tables).staticGraph(16) && (*tables).staticGraph(16).error() == (*tables).staticGraph(15).error());

    auto course = (*tables).courseGraph(0, 0);
    CHECK(static_cast<bool>(course) && course->segments.size() == 3);
    CHECK(!(*tables).courseGraph(0, 1) && (*tables).courseGraph(0, 1).error() == NavRouteError::NullPair);
    CHECK(!(*tables).courseGraph(1, 0) && (*tables).courseGraph(1, 0).error() == NavRouteError::NullPair);

    // Town street mode: the loader's scene 0 -> 9 remap makes both ids serve
    // the same car0 pair; every failure mode is a clean NullPair.
    auto town0 = (*tables).townGraph(0);
    auto town9 = (*tables).townGraph(9);
    CHECK(static_cast<bool>(town0) && static_cast<bool>(town9));
    if (town0 && town9) {
        CHECK(town9->segmentVaddr == 0x2bd800u && town0->segmentVaddr == town9->segmentVaddr);
        CHECK(town0->segments.size() == 3 && town0->segments[0].ax == 500.f);
        CHECK(town9->segments[2].bz == 555.f);
        CHECK((*town0).transitionVaddr == (*town9).transitionVaddr);
    }
    CHECK((*tables).townGraph(1).error() == NavRouteError::NullPair);  // empty pair array
    CHECK((*tables).townGraph(2).error() == NavRouteError::NullPair);  // null outer row
    CHECK((*tables).townGraph(-1).error() == NavRouteError::NullPair);
    CHECK((*tables).townGraph(-2147483647 - 1).error() == NavRouteError::NullPair);
    CHECK((*tables).townGraph(static_cast<int>(kNavOuterEntryCount)).error() == NavRouteError::NullPair);

    // Out-of-range segment index trips OutOfRange: break transition 0 -> 200.
    std::ifstream in(path, std::ios::binary);
    std::vector<std::uint8_t> elf((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const std::size_t tr0 = 84 + (0x2bd800u + 72 - 0x2bd000u);
    elf[tr0] = 200;
    in.close();
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(elf.data()), static_cast<std::streamsize>(elf.size()));
    out.close();
    auto broken = NavRouteTables::open(path);
    CHECK(static_cast<bool>(broken));
    if (broken) {
        CHECK(!broken->staticGraph(0) && broken->staticGraph(0).error() == NavRouteError::OutOfRange);
    }
    std::filesystem::remove(path);
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return std::filesystem::path(value);
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testDiscPins() {
    const std::filesystem::path elfPath = discRoot() / "SLUS_203.98";
    auto tables = NavRouteTables::open(elfPath);
    if (!tables) {
        std::printf("nav_routes: disc ELF not available (%s) — disc pins skipped\n",
                    elfPath.string().c_str());
        return;
    }

    // Static directory pins (nav-tables.md).
    std::uint32_t seg = 0, trans = 0;
    CHECK((*tables).staticPair(0, seg, trans));
    CHECK(seg == 0x002ac620u && trans == 0x002ac968u);

    auto peach = (*tables).staticGraph(0);
    CHECK(static_cast<bool>(peach));
    if (peach) {
        CHECK(peach->segments.size() == 35);
        CHECK(peach->segments[0].ax == 590.f && peach->segments[0].az == 569.f);
        CHECK(peach->segments[0].bx == 590.f && peach->segments[0].bz == 571.f);
        const NavTransition& t0 = peach->transitions[0];
        CHECK(t0.segmentA == 0 && t0.segmentB == 1);
        CHECK(t0.sideACandidates[0] == 0x22 && t0.sideACandidates[1] == 0x22);
        CHECK(t0.sideBCandidates[0] == 1 && t0.sideBCandidates[1] == 1);
        CHECK(t0.followerOutput == 0 && t0.reserved == 0);
    }

    // Alias rules and null entries.
    auto g6 = (*tables).staticGraph(6);
    auto g11 = (*tables).staticGraph(11);
    CHECK(static_cast<bool>(g6) && static_cast<bool>(g11) &&
          g11->segmentVaddr == g6->segmentVaddr && g11->segmentVaddr == 0x002b0d60u);
    auto g15 = (*tables).staticGraph(15);
    auto g16 = (*tables).staticGraph(16);
    CHECK(static_cast<bool>(g15) && static_cast<bool>(g16) && g16->segmentVaddr == g15->segmentVaddr);
    CHECK((*tables).staticGraph(18).error() == NavRouteError::NullPair);
    CHECK((*tables).staticGraph(21).error() == NavRouteError::NullPair);
    CHECK((*tables).staticGraph(23).error() == NavRouteError::NullPair);

    // Corpus: 21 decodable static graphs, 2,334 total segments, every
    // endpoint inside the tile bounds.
    std::size_t graphs = 0, segments = 0;
    bool boundsHold = true;
    for (std::size_t i = 0; i < kNavStaticPairCount; ++i) {
        auto graph = (*tables).staticGraph(i);
        if (!graph) continue;
        ++graphs;
        segments += graph->segments.size();
        if (!NavRouteTables::graphWithinTile(*graph)) boundsHold = false;
    }
    CHECK(graphs == 21);
    CHECK(segments == 2334);
    CHECK(boundsHold);

    // Course/entity mode: outer directory counts and pins.
    CHECK((*tables).outerEntry(0) == 0x002d9558u);
    CHECK((*tables).outerEntry(5) == 0);
    std::size_t outerNonNull = 0, pairs = 0;
    for (std::size_t t = 0; t < kNavOuterEntryCount; ++t) {
        if ((*tables).outerEntry(t) == 0) continue;
        ++outerNonNull;
        for (std::size_t car = 0; car < kNavMaxPairsPerOuter; ++car) {
            auto graph = (*tables).courseGraph(t, car);
            if (!graph) break;  // null-terminated arrays
            ++pairs;
        }
    }
    CHECK(outerNonNull == 21);
    CHECK(pairs == 69);
    auto t0car0 = (*tables).courseGraph(0, 0);
    CHECK(static_cast<bool>(t0car0) && t0car0->segmentVaddr == 0x002d5750u &&
          t0car0->segments.size() == 30);
    // Non-null outer with empty array (T2).
    CHECK((*tables).courseGraph(2, 0).error() == NavRouteError::NullPair);

    // -- Town street mode pins (scene-tile-map.md).
    // Outer directory rows, indexed by 0x2be438 scene-table record.
    CHECK((*tables).outerEntry(1) == 0x002d9310u);  // Peach Town
    CHECK((*tables).outerEntry(2) == 0x002d9368u);  // Fuji City
    CHECK((*tables).outerEntry(3) == 0x002d93c0u);  // Sandpolis
    CHECK((*tables).outerEntry(4) == 0x002d9408u);  // Chestnut Canyon
    CHECK((*tables).outerEntry(9) == 0x002d9558u   // My City row == T0 row
          && (*tables).outerEntry(9) == (*tables).outerEntry(0));

    // T=1 Peach Town car0 pair {0x2c5ff0, 0x2c67d0}, 84 segments.
    auto peachTown = (*tables).townGraph(1);
    CHECK(static_cast<bool>(peachTown));
    if (peachTown) {
        CHECK(peachTown->segmentVaddr == 0x002c5ff0u &&
              peachTown->transitionVaddr == 0x002c67d0u);
        CHECK(peachTown->segments.size() == 84);
        // Documented bbox x=[418,552] z=[443,1084]: every decoded endpoint
        // must lie inside it, and the graph touches all four edges exactly.
        float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::lowest();
        for (const NavSegment& s : peachTown->segments) {
            minX = std::min(minX, std::min(s.ax, s.bx));
            maxX = std::max(maxX, std::max(s.ax, s.bx));
            minZ = std::min(minZ, std::min(s.az, s.bz));
            maxZ = std::max(maxZ, std::max(s.az, s.bz));
        }
        CHECK(minX >= 418.f && maxX <= 552.f && minZ >= 443.f && maxZ <= 1084.f);
        CHECK(minX == 418.f && maxX == 552.f && minZ == 443.f && maxZ == 1084.f);
        // First/last segment endpoints (disc-decoded; all coordinates in
        // this graph are integer-valued f32 and both aux floats are zero).
        CHECK(peachTown->segments.front().ax == 484.f && peachTown->segments.front().az == 486.f);
        CHECK(peachTown->segments.front().bx == 480.f && peachTown->segments.front().bz == 482.f);
        CHECK(peachTown->segments.back().ax == 483.f && peachTown->segments.back().az == 470.f);
        CHECK(peachTown->segments.back().bx == 479.f && peachTown->segments.back().bz == 470.f);
        bool auxAllZero = true;
        for (const NavSegment& s : peachTown->segments) {
            if (s.auxA != 0.f || s.auxB != 0.f) auxAllZero = false;
        }
        CHECK(auxAllZero);
        // traffic-ai.md scene-1 spawn record 0 = (459, 30, 786) sits inside
        // the street box on the (x,z) plane.
        CHECK(459.f >= minX && 459.f <= maxX && 786.f >= minZ && 786.f <= maxZ);
        // qwc sanity: 84*24 = 2016 B = 126 qwc of segments, 84*8 = 672 B =
        // 42 qwc of transitions — both 16-byte multiples.
        CHECK(peachTown->segments.size() * 24 % 16 == 0);
        CHECK(peachTown->segments.size() * 8 % 16 == 0);
    }

    // Scene 0 executes as 9 (FUN_0020e708): one shared 30-segment graph.
    auto town0 = (*tables).townGraph(0);
    auto town9 = (*tables).townGraph(9);
    CHECK(static_cast<bool>(town0) && static_cast<bool>(town9));
    if (town0 && town9) {
        CHECK(town0->segmentVaddr == town9->segmentVaddr &&
              town9->segmentVaddr == 0x002d5750u);
        CHECK(town0->segments.size() == 30);
    }

    // Null rows and empty car0 arrays are clean errors; the sets are the
    // observed SLUS truth (11 null rows, 2 non-null rows with empty arrays).
    const int kNullRowScenes[] = {5, 10, 11, 12, 14, 15, 16, 17, 19, 20, 21};
    for (int t : kNullRowScenes) {
        CHECK((*tables).outerEntry(static_cast<std::size_t>(t)) == 0);
        CHECK((*tables).townGraph(t).error() == NavRouteError::NullPair);
    }
    const int kEmptyCar0Scenes[] = {2, 8};  // Fuji City, Cloud Hill
    for (int t : kEmptyCar0Scenes) {
        CHECK((*tables).outerEntry(static_cast<std::size_t>(t)) != 0);
        CHECK((*tables).townGraph(t).error() == NavRouteError::NullPair);
    }
    CHECK((*tables).townGraph(-1).error() == NavRouteError::NullPair);
    CHECK((*tables).townGraph(static_cast<int>(kNavOuterEntryCount)).error() == NavRouteError::NullPair);

    // Census: 19 of the 32 scene ids decode a car0 street graph (11 null
    // rows + 2 empty arrays absent), all inside tile bounds.
    std::size_t townGraphs = 0;
    bool townBoundsHold = true;
    for (int t = 0; t < static_cast<int>(kNavOuterEntryCount); ++t) {
        auto graph = (*tables).townGraph(t);
        if (!graph) continue;
        ++townGraphs;
        if (!NavRouteTables::graphWithinTile(*graph)) townBoundsHold = false;
    }
    CHECK(townGraphs == 19);
    CHECK(townBoundsHold);

    std::printf("nav_routes disc: staticGraphs=%zu segments=%zu outerNonNull=%zu pairs=%zu\n",
                graphs, segments, outerNonNull, pairs);
    std::printf("nav_routes disc: town car0 graphs=%zu (alias 0->9 shared); peach segments=%zu\n",
                townGraphs, peachTown ? peachTown->segments.size() : 0);
    if (peachTown) {
        const NavSegment& first = peachTown->segments.front();
        const NavSegment& last = peachTown->segments.back();
        std::printf("nav_routes disc: Peach bbox x=[418,552] z=[443,1084]; "
                    "first=(%.0f,%.0f)->(%.0f,%.0f) "
                    "last=(%.0f,%.0f)->(%.0f,%.0f)\n",
                    first.ax, first.az, first.bx, first.bz,
                    last.ax, last.az, last.bx, last.bz);
    }
    std::printf("nav_routes disc: null town rows=5,10,11,12,14,15,16,17,19,20,21; "
                "empty car0 rows=2,8\n");
}

} // namespace

int main() {
    testSynthetic();
    testDiscPins();
    if (g_fail != 0) {
        std::fprintf(stderr, "nav_routes: %d checks failed\n", g_fail);
        return 1;
    }
    std::printf("nav_routes: %d checks passed\n", g_pass);
    return 0;
}
