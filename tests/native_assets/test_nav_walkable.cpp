// Waypoint-style NavPath walk tests. Disc checks use only the user's own ELF.
#include "../../src/native/assets/nav_routes.h"
#include "../../src/native/gameplay/nav_path.h"
#include "../../src/native/gameplay/nav_walkable.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

using namespace rt::native::assets;
using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

bool near(float a, float b, float epsilon = 1e-4f) { return std::fabs(a - b) <= epsilon; }

void testOpenLWalksExactThreeFourLength() {
    const NavPath path{{{0.f, 0.f}, {3.f, 0.f}, {3.f, 4.f}}, {}, false};
    const WalkResult result = walkNavPath(path, 2.f);
    CHECK(near(result.totalLength, 7.f));
    CHECK(near(result.visitedLength, 7.f));
    CHECK(near(result.visitedFraction, 1.f));
    CHECK(!result.completedLap);
    CHECK(result.cursorPositions.size() == 5);
    CHECK(near(result.cursorPositions.back().x, 3.f));
    CHECK(near(result.cursorPositions.back().z, 4.f));
}

void testClosedSquareCompletesLapAndReturnsToStart() {
    const NavPath path{{{0.f, 0.f}, {3.f, 0.f}, {3.f, 3.f}, {0.f, 3.f}}, {}, true};
    const WalkResult result = walkNavPath(path, 2.f);
    CHECK(near(result.totalLength, 12.f));
    CHECK(near(result.visitedLength, 12.f));
    CHECK(near(result.visitedFraction, 1.f));
    CHECK(result.completedLap);
    CHECK(!result.cursorPositions.empty());
    CHECK(near(result.cursorPositions.back().x, path.centerline.front().x));
    CHECK(near(result.cursorPositions.back().z, path.centerline.front().z));
}

void testEmptyPathIsDegenerate() {
    const WalkResult result = walkNavPath({}, 1.f);
    CHECK(near(result.totalLength, 0.f));
    CHECK(near(result.visitedLength, 0.f));
    CHECK(near(result.visitedFraction, 0.f));
    CHECK(!result.completedLap);
    CHECK(result.cursorPositions.empty());
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testDiscFullLapAndCorpus() {
    auto tables = NavRouteTables::open(discRoot() / "SLUS_203.98");
    if (!tables) {
        std::printf("nav_walkable: disc ELF unavailable — disc checks skipped\n");
        return;
    }
    auto peach = tables->staticGraph(0);
    CHECK(static_cast<bool>(peach));
    if (peach) {
        const NavPath path = buildNavPath(*peach);
        const WalkResult result = walkNavPath(path, 10.f);
        CHECK(path.closed);
        CHECK(std::round(static_cast<double>(result.totalLength) * 1000.0) == 1652558.0);
        CHECK(near(result.visitedFraction, 1.f));
        CHECK(result.completedLap);
        CHECK(!result.cursorPositions.empty());
        CHECK(near(result.cursorPositions.back().x, path.centerline.front().x, 1e-3f));
        CHECK(near(result.cursorPositions.back().z, path.centerline.front().z, 1e-3f));
        const NodeCoverage coverage = nodeCoverage(path, *peach, result, 10.f);
        CHECK(coverage.totalNodes == path.nodeOrder.size() - 1);
        CHECK(near(coverage.fraction, 1.f));
        std::printf("nav_walkable disc: Peach Town length=%.3f node-coverage=%zu/%zu\n",
                    result.totalLength, coverage.visitedNodes, coverage.totalNodes);
    }

    std::size_t graphs = 0, closed = 0;
    for (std::size_t i = 0; i < kNavStaticPairCount; ++i) {
        auto graph = tables->staticGraph(i);
        if (!graph) continue;
        const NavPath path = buildNavPath(*graph);
        const WalkResult result = walkNavPath(path, 10.f);
        ++graphs;
        if (path.closed) ++closed;
        std::printf("nav_walkable disc: graph=%zu closed=%s length=%.3f walked=%.1f%%\n", i,
                    path.closed ? "yes" : "no", result.totalLength, 100.f * result.visitedFraction);
    }
    std::printf("nav_walkable disc: closure=%zu/%zu (%.1f%%)\n", closed, graphs,
                graphs == 0 ? 0.0 : 100.0 * static_cast<double>(closed) / graphs);
}

} // namespace

int main() {
    testOpenLWalksExactThreeFourLength();
    testClosedSquareCompletesLapAndReturnsToStart();
    testEmptyPathIsDegenerate();
    testDiscFullLapAndCorpus();
    if (g_fail) return 1;
    std::printf("nav_walkable: all checks passed\n");
    return 0;
}
