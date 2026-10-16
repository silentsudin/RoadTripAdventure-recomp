// Navigation path traversal tests.  Fixtures are deliberately small complete
// graphs so ordering decisions are visible without disc data.
#include "../../src/native/gameplay/nav_path.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

using namespace rt::native::assets;
using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

NavSegment segment(float ax, float az, float bx, float bz) { return {ax, az, bx, bz}; }

NavTransition transition(std::uint8_t a, std::uint8_t b, std::uint8_t a0,
                         std::uint8_t a1, std::uint8_t b0, std::uint8_t b1) {
    NavTransition t;
    t.segmentA = a;
    t.segmentB = b;
    t.sideACandidates[0] = a0;
    t.sideACandidates[1] = a1;
    t.sideBCandidates[0] = b0;
    t.sideBCandidates[1] = b1;
    return t;
}

void testStraightAheadAndDedupe() {
    // Replacing node 1 with branch node 2's turn (or appending segment 1
    // twice) must fail these observable centerline/order assertions.
    NavGraph graph;
    graph.segments = {
        segment(0, -1, 0, 1), segment(10, -1, 10, 1),
        segment(20, -1, 20, 1), segment(10, 9, 10, 11),
    };
    graph.transitions = {
        transition(0, 1, 1, 2, 1, 2),
        transition(1, 2, 1, 1, 1, 1),
        transition(1, 3, 2, 2, 2, 2),
    };
    const NavPath path = buildNavPath(graph);
    CHECK((path.nodeOrder == std::vector<int>{0, 1}));
    CHECK(path.centerline.size() == 3);
    CHECK(path.centerline[0].x == 0.f && path.centerline[1].x == 10.f &&
          path.centerline[2].x == 20.f);
    CHECK(std::fabs(path.totalLength() - 20.f) < 1e-4f);
    CHECK(!path.closed);
}

void testLoopClosesOnlyAtZero() {
    // Allowing a repeated nonzero node, or failing to retain the return to
    // zero as a closure, changes both the route order and closed result.
    NavGraph graph;
    graph.segments = {
        segment(0, -1, 0, 1), segment(10, -1, 10, 1),
        segment(20, -1, 20, 1), segment(30, -1, 30, 1),
    };
    graph.transitions = {
        transition(0, 1, 1, 1, 1, 1),
        transition(1, 2, 2, 2, 2, 2),
        transition(2, 3, 0, 0, 0, 0),
        transition(3, 3, 3, 3, 3, 3),
    };
    const NavPath path = buildNavPath(graph);
    CHECK((path.nodeOrder == std::vector<int>{0, 1, 2, 0}));
    CHECK(path.closed);
    CHECK(path.centerline.size() == 4);  // shared segments are emitted once
}

void testStopsOnInvalidCandidatesAndStart() {
    // Treating dangling bytes as valid indices or wrapping an invalid start
    // would produce a nonempty route here.
    NavGraph graph;
    graph.segments = {segment(0, -1, 0, 1), segment(10, -1, 10, 1)};
    graph.transitions = {transition(0, 1, 9, 9, 9, 9)};
    const NavPath dangling = buildNavPath(graph);
    CHECK((dangling.nodeOrder == std::vector<int>{0}));
    CHECK(!dangling.closed);
    CHECK(buildNavPath(graph, 1).nodeOrder.empty());
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testDiscPinsAndCorpus() {
    auto tables = NavRouteTables::open(discRoot() / "SLUS_203.98");
    if (!tables) {
        std::printf("nav_path: disc ELF unavailable — disc pins skipped\n");
        return;
    }
    auto peach = tables->staticGraph(0);
    CHECK(static_cast<bool>(peach));
    if (peach) {
        const NavPath path = buildNavPath(*peach);
        CHECK(!path.nodeOrder.empty());
        CHECK(path.totalLength() > 0.f);
        CHECK(path.closed);  // pinned from the user ELF; update only with evidence.
        for (const Pt& point : path.centerline)
            CHECK(point.x >= -1.f && point.x <= 1570.f && point.z >= -1.f && point.z <= 1570.f);
    }

    std::size_t graphs = 0, closed = 0;
    for (std::size_t i = 0; i < kNavStaticPairCount; ++i) {
        auto graph = tables->staticGraph(i);
        if (!graph) continue;
        const NavPath path = buildNavPath(*graph);
        ++graphs;
        if (path.closed) ++closed;
        const std::size_t visited = path.nodeOrder.size() - (path.closed ? 1 : 0);
        std::printf("nav_path disc: graph=%zu visited=%zu/%zu closed=%s\n", i,
                    visited, graph->transitions.size(), path.closed ? "yes" : "no");
    }
    std::printf("nav_path disc: closure=%zu/%zu (%.1f%%)\n", closed, graphs,
                graphs == 0 ? 0.0 : 100.0 * static_cast<double>(closed) / graphs);
}

} // namespace

int main() {
    testStraightAheadAndDedupe();
    testLoopClosesOnlyAtZero();
    testStopsOnInvalidCandidatesAndStart();
    testDiscPinsAndCorpus();
    if (g_fail) return 1;
    std::printf("nav_path: all checks passed\n");
    return 0;
}
