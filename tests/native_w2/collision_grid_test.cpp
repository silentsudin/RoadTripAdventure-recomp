// W3-03: Town nav-graph occupancy collision contract.  The test only reads
// the user's disc at run time; its absence is an intentional success/skip.
#include "../../src/native/assets/nav_routes.h"
#include "../../src/native/gameplay/collision_grid.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

namespace {

int gFailures = 0;
#define CHECK(condition) do { \
    if (!(condition)) { \
        std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        ++gFailures; \
    } \
} while (false)

std::filesystem::path findDisc() {
    if (const char* elf = std::getenv("RT_ELF"); elf && *elf && std::filesystem::exists(elf)) return elf;
    const std::filesystem::path canonical =
        std::filesystem::path(std::getenv("HOME") ? std::getenv("HOME") : "") /
        "Library/Application Support/RoadTripRecomp/disc/SLUS_203.98";
    if (std::filesystem::exists(canonical)) return canonical;
    for (const auto& root : {std::filesystem::path("build"), std::filesystem::path("scratch")}) {
        if (!std::filesystem::exists(root)) continue;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (entry.path().filename() == "SLUS_203.98" && entry.path().parent_path().filename() == "disc") {
                return entry.path();
            }
        }
    }
    return {};
}

std::uint64_t replayHash(const rt::native::gameplay::collision::OccupancyGrid& grid,
                         rt::native::gameplay::collision::Point start) {
    using namespace rt::native::gameplay::collision;
    Point p = start;
    std::uint64_t hash = 1469598103934665603ull;
    for (int frame = 0; frame < 240; ++frame) {
        const float dx = static_cast<float>((frame * 17) % 23 - 11) * grid.cellUnits() / 16.f;
        const float dz = static_cast<float>((frame * 29) % 19 - 9) * grid.cellUnits() / 16.f;
        const CollisionResult r = grid.resolve(p, Point{p.x + dx, p.z + dz}, Point{dx, dz}, 0.3f);
        p = r.position;
        CHECK(!grid.blocked(p));
        const std::int32_t x = static_cast<std::int32_t>(p.x * 256.f);
        const std::int32_t z = static_cast<std::int32_t>(p.z * 256.f);
        hash ^= static_cast<std::uint32_t>(x);
        hash *= 1099511628211ull;
        hash ^= static_cast<std::uint32_t>(z);
        hash *= 1099511628211ull;
    }
    return hash;
}

}  // namespace

int main() {
    const std::filesystem::path disc = findDisc();
    if (disc.empty()) {
        std::puts("SCORE collision_grid=SKIP reason=NO_ELF");
        return 0;
    }

    auto tables = rt::native::assets::NavRouteTables::open(disc);
    CHECK(tables.has_value());
    if (!tables) return 1;
    auto graph = tables->townGraph(1);
    CHECK(graph.has_value());
    if (!graph) return 1;
    CHECK(graph->segments.size() == 84);  // ROM pair span: transition - segment = 84 * 24.

    using namespace rt::native::gameplay::collision;
    const OccupancyGrid grid = OccupancyGrid::fromTownGraph(*graph);
    CHECK(grid.width() == graph->segments.size());
    CHECK(grid.height() == graph->segments.size());

    Point start{graph->segments.front().ax, graph->segments.front().az};
    CHECK(!grid.blocked(start));
    const CollisionResult hit = grid.resolve(start, Point{-grid.cellUnits(), -grid.cellUnits()}, Point{-4.f, -4.f}, 0.3f);
    CHECK(hit.contact);
    CHECK(!grid.blocked(hit.position));
    CHECK(hit.velocity.x == -1.2f && hit.velocity.z == -1.2f);

    const std::uint64_t hashA = replayHash(grid, start);
    const std::uint64_t hashB = replayHash(grid, start);
    CHECK(hashA == hashB);

    // Fixed physics work is deliberately eight substeps per simulation frame;
    // render-frame aggregation must not leave a blocked occupancy cell.
    Point substep = start;
    const Point delta{grid.cellUnits() * 2.f, grid.cellUnits() * 2.f};
    for (int i = 0; i < 8; ++i) {
        const CollisionResult r = grid.resolve(
            substep, Point{substep.x + delta.x / 8.f, substep.z + delta.z / 8.f}, delta, 0.3f);
        substep = r.position;
    }
    CHECK(!grid.blocked(substep));
    CHECK(grid.cellX(substep) >= 0 && grid.cellZ(substep) >= 0);

    if (gFailures != 0) return 1;
    std::printf("SCORE collision_grid=PASS segments=%zu hash=%016llx substeps=8\n", graph->segments.size(),
                static_cast<unsigned long long>(hashA));
    return 0;
}
