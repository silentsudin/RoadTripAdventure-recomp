// Deterministic kinematic NavDrive follower tests. Disc checks use only the
// user's own ELF and are skipped (never failed) when it is unavailable.
#include "../../src/native/assets/nav_routes.h"
#include "../../src/native/gameplay/nav_drive.h"
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

bool near(float a, float b, float epsilon = 1e-4f) { return std::fabs(a - b) <= epsilon; }

void testStraightSegmentExactDistance() {
    // Open single straight segment along +X, length 10.
    const NavPath path{{{0.f, 0.f}, {10.f, 0.f}}, {}, false};
    NavDrive drive(path);
    CHECK(!drive.closed());
    CHECK(drive.segmentCount() == 1);
    CHECK(near(drive.totalLength(), 10.f));
    CHECK(near(drive.pose().position.x, 0.f));
    CHECK(near(drive.pose().position.z, 0.f));

    const NavPose a = drive.step(2.5f);
    CHECK(near(a.arcLength, 2.5f));
    CHECK(near(a.position.x, 2.5f));
    CHECK(near(a.position.z, 0.f));
    CHECK(near(a.heading, static_cast<float>(M_PI) * .5f));
    CHECK(a.lap == 0);

    const NavPose b = drive.step(2.5f);
    CHECK(near(b.arcLength, 5.f));
    CHECK(near(b.position.x, 5.f));
    CHECK(near(b.position.z, 0.f));

    // Open paths clamp at the final point instead of wrapping.
    const NavPose end = drive.step(100.f);
    CHECK(near(end.arcLength, 10.f));
    CHECK(near(end.position.x, 10.f));
    CHECK(near(end.position.z, 0.f));
    CHECK(end.lap == 0);
}

void testSquareLapPerimeterAndCornerMidpoints() {
    // Closed unit-square-ish loop; perimeter 3 + 3 + 3 + 3 = 12.
    const NavPath path{{{0.f, 0.f}, {3.f, 0.f}, {3.f, 3.f}, {0.f, 3.f}}, {}, true};
    NavDrive drive(path);
    CHECK(drive.closed());
    CHECK(drive.segmentCount() == 4);
    CHECK(near(drive.totalLength(), 12.f));

    // Leg midpoints at arc 1.5, 4.5, 7.5, 10.5.
    const NavPose m0 = drive.step(1.5f);
    CHECK(near(m0.position.x, 1.5f));
    CHECK(near(m0.position.z, 0.f));
    CHECK(near(m0.heading, static_cast<float>(M_PI) * .5f));  // +X

    const NavPose m1 = drive.step(3.f);
    CHECK(near(m1.position.x, 3.f));
    CHECK(near(m1.position.z, 1.5f));
    CHECK(near(m1.heading, 0.f));  // +Z

    const NavPose m2 = drive.step(3.f);
    CHECK(near(m2.position.x, 1.5f));
    CHECK(near(m2.position.z, 3.f));
    CHECK(near(m2.heading, -static_cast<float>(M_PI) * .5f));  // -X

    const NavPose m3 = drive.step(3.f);
    CHECK(near(m3.position.x, 0.f));
    CHECK(near(m3.position.z, 1.5f));
    CHECK(near(std::fabs(m3.heading), static_cast<float>(M_PI)));  // -Z
}

void testLapWrap() {
    const NavPath path{{{0.f, 0.f}, {3.f, 0.f}, {3.f, 3.f}, {0.f, 3.f}}, {}, true};
    NavDrive drive(path);
    // Four 3-unit steps = exactly one perimeter.
    for (int i = 0; i < 3; ++i) {
        const NavPose pose = drive.step(3.f);
        CHECK(pose.lap == 0);
    }
    const NavPose wrap = drive.step(3.f);
    CHECK(wrap.lap == 1);
    CHECK(near(wrap.arcLength, 0.f));
    CHECK(near(wrap.position.x, path.centerline.front().x));
    CHECK(near(wrap.position.z, path.centerline.front().z));
    CHECK(near(wrap.heading, static_cast<float>(M_PI) * .5f));  // back on leg 0

    for (int i = 0; i < 4; ++i) (void)drive.step(3.f);
    CHECK(drive.pose().lap == 2);

    drive.reset();
    CHECK(drive.pose().lap == 0);
    CHECK(near(drive.pose().arcLength, 0.f));
}

void testDegenerateInputs() {
    // Empty path: valid object, no movement, start pose at origin.
    NavDrive empty{NavPath{}};
    CHECK(empty.empty());
    CHECK(near(empty.totalLength(), 0.f));
    const NavPose p = empty.step(5.f);
    CHECK(near(p.position.x, 0.f));
    CHECK(near(p.position.z, 0.f));
    CHECK(p.lap == 0);

    // Single point: no segments, pose at the point, stable under stepping.
    const NavDrive point{NavPath{{{4.f, 7.f}}, {}, true}};
    CHECK(point.empty());
    CHECK(near(point.pose().position.x, 4.f));
    CHECK(near(point.pose().position.z, 7.f));

    // Zero-length edges are ignored; only the two real legs remain.
    const NavPath doubled{{{0.f, 0.f}, {0.f, 0.f}, {5.f, 0.f}, {5.f, 0.f}}, {}, false};
    NavDrive drive(doubled);
    CHECK(drive.segmentCount() == 1);
    CHECK(near(drive.totalLength(), 5.f));

    // Non-positive / non-finite speed is a deterministic no-op.
    const NavPose before = drive.pose();
    (void)drive.step(0.f);
    (void)drive.step(-3.f);
    (void)drive.step(std::nanf(""));
    const NavPose after = drive.pose();
    CHECK(near(before.arcLength, after.arcLength));
    CHECK(near(before.position.x, after.position.x));
}

std::filesystem::path discRoot() {
    if (const char* value = std::getenv("RT_DISC")) return value;
    if (const char* home = std::getenv("HOME"))
        return std::filesystem::path(home) / "Library/Application Support/RoadTripRecomp/disc";
    return {};
}

void testDiscPeachTownDrive() {
    auto tables = NavRouteTables::open(discRoot() / "SLUS_203.98");
    if (!tables) {
        std::printf("nav_drive: disc ELF unavailable — disc checks skipped\n");
        return;
    }
    auto peach = tables->staticGraph(0);
    CHECK(static_cast<bool>(peach));
    if (!peach) return;
    const NavPath path = buildNavPath(*peach);
    CHECK(path.closed);
    if (!path.closed) return;

    NavDrive drive(path);
    // Independently count the nonzero edges a closed centerline should feed the
    // follower (consecutive pairs plus the closing edge); zero-length edges are
    // dropped by design.
    std::size_t nonzeroEdges = 0;
    for (std::size_t i = 1; i < path.centerline.size(); ++i) {
        const float dx = path.centerline[i].x - path.centerline[i - 1].x;
        const float dz = path.centerline[i].z - path.centerline[i - 1].z;
        if (std::sqrt(dx * dx + dz * dz) > 0.f) ++nonzeroEdges;
    }
    {
        const float dx = path.centerline.front().x - path.centerline.back().x;
        const float dz = path.centerline.front().z - path.centerline.back().z;
        if (std::sqrt(dx * dx + dz * dz) > 0.f) ++nonzeroEdges;
    }
    CHECK(drive.segmentCount() == nonzeroEdges);
    CHECK(std::round(static_cast<double>(drive.totalLength()) * 1000.0) == 1652558.0);

    // Walk a full lap plus a bit; the closed cursor must wrap exactly once and
    // return to the pinned start without drift beyond one step.
    const float stepUnits = 10.f;
    int steps = 0;
    while (drive.pose().lap < 1 && steps < 1000) { (void)drive.step(stepUnits); ++steps; }
    CHECK(drive.pose().lap == 1);
    const NavPose afterLap = drive.pose();
    CHECK(afterLap.arcLength < stepUnits);
    CHECK(near(afterLap.position.x, path.centerline.front().x, 10.f));
    CHECK(near(afterLap.position.z, path.centerline.front().z, 10.f));
    std::printf("nav_drive disc: Peach Town length=%.3f points=%zu segments=%zu lap-steps=%d\n",
                drive.totalLength(), path.centerline.size(), drive.segmentCount(), steps);
}

} // namespace

int main() {
    testStraightSegmentExactDistance();
    testSquareLapPerimeterAndCornerMidpoints();
    testLapWrap();
    testDegenerateInputs();
    testDiscPeachTownDrive();
    if (g_fail) return 1;
    std::printf("nav_drive: all checks passed\n");
    return 0;
}
