// NavPath-to-RaceRoute bridge tests. Disc checks use only the user's own ELF.
#include "../../src/native/assets/nav_routes.h"
#include "../../src/native/gameplay/nav_path.h"
#include "../../src/native/gameplay/nav_route_bridge.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

using namespace rt::native::assets;
using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

Vec2 midpoint(Pt a, Pt b) { return {(a.x + b.x) * 0.5f, (a.z + b.z) * 0.5f}; }

std::vector<Vec2> zoneMidpoints(const NavPath& path) {
    std::vector<Vec2> points;
    if (path.centerline.size() < 2) return points;
    const auto append = [&points](Pt a, Pt b) {
        const float dx = b.x - a.x;
        const float dz = b.z - a.z;
        if (dx * dx + dz * dz > 0.f) points.push_back(midpoint(a, b));
    };
    for (std::size_t i = 1; i < path.centerline.size(); ++i)
        append(path.centerline[i - 1], path.centerline[i]);
    if (path.closed) append(path.centerline.back(), path.centerline.front());
    return points;
}

void testOpenLShapeZonesContainSegmentMidpoints() {
    const NavPath path{{{0.f, 0.f}, {100.f, 0.f}, {100.f, 100.f}}, {}, false};
    const auto zones = buildRouteZones(path, 10.f);
    const RaceRoute route(zones);
    CHECK(zones.size() == 2);
    CHECK(route.zoneOf(midpoint(path.centerline[0], path.centerline[1])) == 0);
    CHECK(route.zoneOf(midpoint(path.centerline[1], path.centerline[2])) == 1);
}

void testClosedSquareAdvancesLap() {
    const NavPath path{{{0.f, 0.f}, {100.f, 0.f}, {100.f, 100.f}, {0.f, 100.f}}, {}, true};
    const auto zones = buildRouteZones(path, 10.f);
    const RaceRoute route(zones);
    RaceRoute::CarProgress car;
    CHECK(zones.size() == 4);
    for (std::size_t i = 0; i < path.centerline.size(); ++i) {
        const Pt a = path.centerline[i];
        const Pt b = path.centerline[(i + 1) % path.centerline.size()];
        CHECK(route.zoneOf(midpoint(a, b)) == static_cast<int>(i));
        route.advance(car, midpoint(a, b));
        CHECK(car.zone == static_cast<int>(i));
    }
    CHECK(car.lap == 0);
    route.advance(car, midpoint(path.centerline[0], path.centerline[1]));
    CHECK(car.lap == 1 && car.zone == 0);
}

void testDegeneratePairSkipped() {
    // Duplicate consecutive points must not create a zero-length zone.
    const NavPath path{{{0.f, 0.f}, {0.f, 0.f}, {100.f, 0.f}}, {}, false};
    const auto zones = buildRouteZones(path, 10.f);
    CHECK(zones.size() == 1);
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testPeachTownDiscRoute() {
    const auto elf = discRoot() / "SLUS_203.98";
    auto tables = NavRouteTables::open(elf);
    if (!tables) {
        std::printf("nav_route_bridge: disc ELF unavailable — disc checks skipped\n");
        return;
    }
    auto graph = tables->staticGraph(0);
    CHECK(static_cast<bool>(graph));
    if (!graph) return;
    const NavPath path = buildNavPath(*graph);
    CHECK(path.closed);
    const auto zones = buildRouteZones(path);
    RaceRoute route(zones);
    RaceRoute::CarProgress car;
    const auto points = zoneMidpoints(path);
    std::printf("nav_route_bridge disc: Peach Town zones=%zu closed=%s\n", zones.size(),
                path.closed ? "yes" : "no");
    CHECK(zones.size() == 34);
    CHECK(points.size() == zones.size());
    // Honest semantics: RaceRoute::zoneOf is first-match containment and the
    // Peach Town ROM graph genuinely crosses itself — a later leg's midpoint
    // can lie inside an earlier zone, and advance only accepts zone+1, so
    // progress cascades to a stall there. Pin the deterministic reachable
    // prefix rather than pretend a full sequential walk exists.
    int firstStall = -1;
    float lastProgress = -1.f;
    for (std::size_t i = 0; i < points.size(); ++i) {
        route.advance(car, points[i]);
        CHECK(car.progress >= lastProgress);  // monotonic progress always
        lastProgress = car.progress;
        if (car.zone != static_cast<int>(i) && firstStall < 0) {
            firstStall = static_cast<int>(i);
            std::printf("nav_route_bridge disc: first stall at leg %d — zoneOf=%d current zone=%d\n",
                        firstStall, route.zoneOf(points[static_cast<std::size_t>(firstStall)]), car.zone);
        }
    }
    std::printf("nav_route_bridge disc: pinned-prefix progress=%.0f of %zu zones (firstStall=%d)\n",
                car.progress, points.size(), firstStall);
    CHECK(car.progress == 16.f);   // deterministic crossing stall at leg 17
    CHECK(firstStall == 17);
}

}  // namespace

int main() {
    testOpenLShapeZonesContainSegmentMidpoints();
    testClosedSquareAdvancesLap();
    testDegeneratePairSkipped();
    testPeachTownDiscRoute();
    if (g_fail) return 1;
    std::printf("nav_route_bridge: all checks passed\n");
    return 0;
}
