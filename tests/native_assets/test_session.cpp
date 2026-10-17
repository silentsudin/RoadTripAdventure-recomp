// Pins the authored Adventure session loop. Synthetic simulation and temporary
// save files only; no game data.
#include "../../src/native/gameplay/session.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

using namespace rt::native::gameplay;

static int g_fail = 0;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

namespace {

class TempDirectory {
public:
    TempDirectory() {
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        path = std::filesystem::temp_directory_path() / ("rt-native-session-" + std::to_string(stamp));
        std::filesystem::create_directories(path);
    }
    ~TempDirectory() { std::error_code ec; std::filesystem::remove_all(path, ec); }

    std::filesystem::path path;
};

void testAdventureLoopAwardsFinishAndReturnsToExplore() {
    GameSession session(1);
    CHECK(session.state() == SessionState::EXPLORE);
    CHECK(session.progress().money == kMoneyNewGame);

    session.startRace();
    CHECK(session.state() == SessionState::RACE);
    CHECK(session.lastSnapshot().frame == 0);

    for (int i = 0; i < 130 && session.state() == SessionState::RACE; ++i)
        session.update({});

    CHECK(session.state() == SessionState::RESULTS);
    CHECK(session.lastSnapshot().finished);
    const std::uint32_t reward = session.lastSnapshot().rank == 0 ? 1000u :
                                 session.lastSnapshot().rank == 1 ? 500u : 250u;
    CHECK(session.progress().money == kMoneyNewGame + reward);
    CHECK(session.progress().raceResults[0] == static_cast<std::uint8_t>(session.lastSnapshot().rank));
    CHECK(session.progress().bestLap[0] != kBestTimeNone);
    CHECK(session.progress().bestTotal[0] != kBestTimeNone);

    session.continueFromResults();
    CHECK(session.state() == SessionState::EXPLORE);
    // Post-race return lands back on the Peach Town square anchor.
    CHECK(session.lastSnapshot().carPosition.x == 506.283f);
    CHECK(session.lastSnapshot().carPosition.z == 442.230f);
}

void testConfirmDrivesExploreAndResultsTransitions() {
    GameSession session(1);
    session.update({.confirm = true});
    CHECK(session.state() == SessionState::RACE);

    for (int i = 0; i < 130 && session.state() == SessionState::RACE; ++i)
        session.update({});
    CHECK(session.state() == SessionState::RESULTS);

    session.update({.confirm = true});
    CHECK(session.state() == SessionState::EXPLORE);
}

void testProgressSaveLoadRoundTrip() {
    TempDirectory temp;
    MemoryCard card(temp.path);
    GameSession saved(1);
    saved.startRace();
    for (int i = 0; i < 130 && saved.state() == SessionState::RACE; ++i)
        saved.update({});
    CHECK(saved.state() == SessionState::RESULTS);
    CHECK(saved.save(card, 0));

    GameSession loaded(1);
    CHECK(loaded.load(card, 0));
    CHECK(loaded.progress().money == saved.progress().money);
}

void testTownDriveMovesCarWithGas() {
    // Town drive v1 (session EXPLORE free-drive): the socket-driven pad
    // reaches the same validated car physics + chase camera as the race.
    GameSession session(1);
    const auto p0 = session.lastSnapshot().carPosition;

    // Peach Town square anchor (oracle-golden-frames.md:54): every Adventure
    // starts at the factory-side square, and facade reads see it immediately.
    CHECK(p0.x == 506.283f && p0.z == 442.230f);

    // Null driveInput (every non-lockstep run) keeps EXPLORE frozen — the
    // no-assets default-launch determinism contract.
    for (int i = 0; i < 60; ++i) session.update({});
    const auto pIdle = session.lastSnapshot().carPosition;
    CHECK(pIdle.x == p0.x && pIdle.z == p0.z);
    CHECK(session.state() == SessionState::EXPLORE);

    CarInput gas{};
    gas.gas = true;
    for (int i = 0; i < 120; ++i) session.update({}, &gas);
    const auto s1 = session.lastSnapshot();
    CHECK(s1.speed > 0.f);
    const float dx = s1.carPosition.x - p0.x, dz = s1.carPosition.z - p0.z;
    CHECK(dx * dx + dz * dz > 2500.f);  // > 50 game units over 2 s of gas
    CHECK(session.state() == SessionState::EXPLORE);  // driving never leaves town

    // Coasting stops the car dead and position holds exactly. A zero
    // CarInput (socket attached, no buttons) keeps the sim running; null
    // would mean harness-detached = frozen.
    // kCoast=4 per physics tick (car_physics.cpp:12) from ~9600 needs
    // >1000 frames, so bound the wait generously.
    CarInput idle{};
    for (int i = 0; i < 3000 && session.lastSnapshot().speed != 0.f; ++i) session.update({}, &idle);
    CHECK(session.lastSnapshot().speed == 0.f);
    const auto pStop = session.lastSnapshot().carPosition;
    for (int i = 0; i < 30; ++i) session.update({}, &idle);
    CHECK(session.lastSnapshot().carPosition.x == pStop.x);
    CHECK(session.lastSnapshot().carPosition.z == pStop.z);

    // Steering changes heading while rolling.
    CarInput drive{};
    drive.gas = true;
    drive.steer = 1;
    const float h0 = session.lastSnapshot().heading;
    for (int i = 0; i < 60; ++i) session.update({}, &drive);
    CHECK(session.lastSnapshot().heading != h0);

    // Determinism: identical input replay -> bit-identical pose.
    const auto replay = [] {
        GameSession s(1);
        CarInput in{};
        in.gas = true;
        in.steer = 1;
        std::uint32_t hash = 2166136261u;
        const auto mix = [&hash](std::uint8_t b) { hash = (hash ^ b) * 16777619u; };
        for (int i = 0; i < 90; ++i) s.update({}, &in);
        const auto& p = s.lastSnapshot();
        const float f[4] = {p.carPosition.x, p.carPosition.z, p.heading, p.speed};
        const auto* b = reinterpret_cast<const std::uint8_t*>(&f);
        for (int i = 0; i < 16; ++i) mix(b[i]);
        return hash;
    };
    CHECK(replay() == replay());
}

void testExplorePoseReanchorsOffTown() {
    // Facade warp contract (main.cpp warp lambda): datum anchors for
    // non-town tiles, default-elevation town anchor for tile 43.
    GameSession s(1);
    s.setExplorePose(0.f, 0.f, 0.f, 0.f);
    CHECK(s.lastSnapshot().carPosition.x == 0.f);
    CHECK(s.lastSnapshot().carPosition.z == 0.f);
    s.setExplorePose(kTownSpawnX, kTownSpawnZ, 0.f);
    CHECK(s.lastSnapshot().carPosition.x == kTownSpawnX);
    CHECK(s.lastSnapshot().carPosition.z == kTownSpawnZ);
    // The town default lifts the eye above the plaza (y=30 game units).
    CHECK(s.lastSnapshot().cameraEye.y > 5.f);
}

}  // namespace

int main() {
    testAdventureLoopAwardsFinishAndReturnsToExplore();
    testConfirmDrivesExploreAndResultsTransitions();
    testProgressSaveLoadRoundTrip();
    testTownDriveMovesCarWithGas();
    testExplorePoseReanchorsOffTown();
    if (g_fail != 0) return EXIT_FAILURE;
    std::puts("session: all checks passed");
    return EXIT_SUCCESS;
}
