// Pins the authored GameLoop simulation frame (gameMainLoop 0x2003B8 per-frame
// update). No game data.
#include "../../src/native/gameplay/game_loop.h"

#include <cmath>
#include <cstdio>
#include <cstring>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static bool same(const FrameSnapshot& a, const FrameSnapshot& b) {
    return a.carPosition.x == b.carPosition.x && a.carPosition.z == b.carPosition.z &&
           a.heading == b.heading && a.waypointIndex == b.waypointIndex && a.lapCount == b.lapCount &&
           a.frame == b.frame && a.clock == b.clock &&
           std::memcmp(&a.lighting, &b.lighting, sizeof a.lighting) == 0 && a.visibleCells.size() == b.visibleCells.size() &&
           std::memcmp(a.view, b.view, sizeof a.view) == 0 && std::memcmp(a.proj, b.proj, sizeof a.proj) == 0;
}

static void testResetAndFirstTick() {
    GameLoop g;
    g.reset();
    const Vec2 start = g.world().roadWaypoints[0];
    const MenuInput mi;
    const FrameSnapshot s = g.tick(1.f / 60.f, mi);
    CHECK(s.frame == 1);
    CHECK(std::isfinite(s.heading));
    const float moved = std::sqrt(lengthSq(s.carPosition - start));
    CHECK(moved > 0.f && moved < 20.f);
    CHECK(s.carPosition.x > start.x);  // starts toward +x (waypoint 1)
    CHECK(g.tasks().liveCount() == 1);
    const FrameSnapshot s2 = g.tick(1.f / 60.f, mi);
    CHECK(s2.frame == 2 && s2.carPosition.x > s.carPosition.x);

    g.reset();
    const FrameSnapshot r = g.tick(1.f / 60.f, mi);
    CHECK(same(r, s));  // reset reproduces the first frame
}

static void testLapCorridorCamera() {
    GameLoop g;
    const MenuInput mi;
    const float half = g.world().roadHalfWidth;
    float maxDev = 0.f, maxCam = 0.f;
    FrameSnapshot s;
    for (int i = 0; i < 130; ++i) {  // one lap completes ~frame 110
        s = g.tick(1.f / 60.f, mi);
        maxDev = std::fmax(maxDev, g.roadDistance(s.carPosition));
        const float dx = s.cameraEye.x - s.carPosition.x * g.params().renderScale;
        const float dz = s.cameraEye.z - s.carPosition.z * g.params().renderScale;
        const float dy = s.cameraEye.y;
        maxCam = std::fmax(maxCam, std::sqrt(dx * dx + dy * dy + dz * dz));
        CHECK(!s.visibleCells.empty());
        CHECK(s.carPosition.x >= 0.f && s.carPosition.x < kWorldSize && s.carPosition.z >= 0.f &&
              s.carPosition.z < kWorldSize);
    }
    std::printf("maxDev=%g laps=%d waypointsPassed=%d maxCam=%g\n", maxDev, s.lapCount, s.waypointsPassed, maxCam);
    CHECK(maxDev < half);
    CHECK(s.lapCount >= 1);
    CHECK(s.waypointsPassed >= 4);
    CHECK(maxCam < 25.f && maxCam > 1.f);
    // View matrix translation (column-major [12..14]) = -R * eye: camera origin maps to eye.
    // Transform the eye through view: must land at the origin.
    const float ex = s.cameraEye.x, ey = s.cameraEye.y, ez = s.cameraEye.z;
    for (int r = 0; r < 3; ++r) {
        const float o = s.view[r] * ex + s.view[4 + r] * ey + s.view[8 + r] * ez + s.view[12 + r];
        CHECK(std::fabs(o) < 1e-3f);
    }
}

static void testCull() {
    GameLoop g;
    const MenuInput mi;
    const FrameSnapshot s = g.tick(0.f, mi);
    const auto expect = g.world().visibleCells(s.carPosition, g.params().viewDistance);
    CHECK(s.visibleCells.size() == expect.size());
    CHECK(!s.visibleCells.empty() && s.visibleCells.size() <= static_cast<std::size_t>(kCellCount));
    // Car near (400,400): the (100,100) corner cell centre is ~424 away (in), (1500,1500) ~1555 (out).
    CHECK(s.visibleCells.size() < static_cast<std::size_t>(kCellCount));
    for (const Cell& c : s.visibleCells)
        CHECK(std::sqrt(lengthSq(c.center() - s.carPosition)) <= g.params().viewDistance + 1e-3f);
}

static void testLighting() {
    const MenuInput mi;
    {  // clock advances 1/tick from the MORNING default (82950 = 09:13,
       // the oracle Peach Town clock — oracle-golden-frames.md:54; default set
       // in game_loop.h) and sits in the full-day band (same weights as the
       // 06:40 day band below)
        GameLoop g;
        for (std::uint32_t i = 1; i <= 100; ++i) {
            const FrameSnapshot s = g.tick(0.f, mi);
            CHECK(s.clock == 82950u + i);
            CHECK(s.lighting.dayWeight == 1.1f && s.lighting.nightWeight == 0.f && s.lighting.duskWeight == 0.f);
        }
    }
    {  // wrap at kDayTicks
        GameLoopParams p;
        p.initialClock = kDayTicks - 2;
        GameLoop g(p);
        CHECK(g.tick(0.f, mi).clock == kDayTicks - 1);
        CHECK(g.tick(0.f, mi).clock == 0);
        CHECK(g.tick(0.f, mi).clock == 1);
        g.reset();
        CHECK(g.tick(0.f, mi).clock == kDayTicks - 1);
    }
    {  // day band
        GameLoopParams p;
        p.initialClock = 60000;
        GameLoop g(p);
        FrameSnapshot s;
        for (int i = 0; i < 50; ++i) s = g.tick(0.f, mi);
        CHECK(s.clock == 60050);
        CHECK(s.lighting.dayWeight == 1.1f && s.lighting.nightWeight == 0.f && s.lighting.duskWeight == 0.f);
        CHECK(s.lighting.fogFar == 800.f);
    }
    {  // reset restores the initial clock
        GameLoopParams p;
        p.initialClock = 60000;
        GameLoop g(p);
        g.tick(0.f, mi);
        g.reset();
        CHECK(g.tick(0.f, mi).clock == 60001);
    }
}

static void testDeterminism() {
    GameLoop a, b;
    const MenuInput mi;
    for (int i = 0; i < 60; ++i) CHECK(same(a.tick(1.f / 60.f, mi), b.tick(1.f / 60.f, mi)));
}

static void testPlayerInputDrivesCar() {
    // Removing player input from the GameLoop, or applying it after integration,
    // must make one of these observable car responses fail.
    const MenuInput mi;
    const CarInput gas{.steer = 0, .gas = true};
    GameLoop forward;
    const Vec2 start = forward.world().roadWaypoints[0];
    const FrameSnapshot moving = forward.tickPlayer(1.f / 60.f, mi, gas);
    CHECK(moving.speed > 0.f);
    CHECK(lengthSq(moving.carPosition - start) > 0.f);

    GameLoop turning;
    const float initialHeading = turning.tickPlayer(1.f / 60.f, mi, {}).heading;
    const FrameSnapshot turned = turning.tickPlayer(1.f / 60.f, mi, {.steer = 1, .gas = true});
    CHECK(turned.heading > initialHeading);

    GameLoop braking;
    FrameSnapshot accelerated;
    for (int i = 0; i < 10; ++i) accelerated = braking.tickPlayer(1.f / 60.f, mi, gas);
    const FrameSnapshot slowed = braking.tickPlayer(1.f / 60.f, mi, {.brake = true});
    CHECK(slowed.speed < accelerated.speed);
}

static void testRaceModeTracksLapsAndFinish() {
    const MenuInput mi;
    GameLoopParams raceParams;
    raceParams.raceMode = true;
    raceParams.totalLaps = 1;
    GameLoop race(raceParams);
    FrameSnapshot raceSnapshot;
    // +240: the countdown hold (see above) parks the car for 4 s of frames.
    for (int i = 0; i < 130 + 240; ++i) raceSnapshot = race.tick(1.f / 60.f, mi);
    CHECK(raceSnapshot.finished);
    CHECK(raceSnapshot.lap == 1);
    CHECK(raceSnapshot.elapsedFrames > 0);
    CHECK(raceSnapshot.rank >= 0 && raceSnapshot.rank <= 2);

    GameLoop freeDrive;
    FrameSnapshot freeDriveSnapshot;
    for (int i = 0; i < 130; ++i) freeDriveSnapshot = freeDrive.tick(1.f / 60.f, mi);
    CHECK(!freeDriveSnapshot.finished);

    // RunCountdownTimerTask (0x22DAE8) raises the race word's GO bit at tick
    // 0xF0 (SetVehicleMode 2->6); before that a held gas must not move the car.
    GameLoop countdown(raceParams);
    const CarInput gas{.steer = 0, .gas = true};
    FrameSnapshot held;
    for (int i = 0; i < 240; ++i) held = countdown.tickPlayer(1.f / 60.f, mi, gas);
    CHECK(held.speed == 0.f);
    const FrameSnapshot released = countdown.tickPlayer(1.f / 60.f, mi, gas);
    CHECK(released.speed > 0.f);
}

int main() {
    testResetAndFirstTick();
    testLapCorridorCamera();
    testCull();
    testLighting();
    testDeterminism();
    testPlayerInputDrivesCar();
    testRaceModeTracksLapsAndFinish();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::puts("game_loop tests passed");
    return 0;
}
