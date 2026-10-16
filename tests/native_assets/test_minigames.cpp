// Authored mini-game scoring predicate contract tests. No game data.
#include "../../src/native/gameplay/minigames.h"

#include <cstdio>
#include <limits>
#include <vector>

using namespace rt::native::gameplay;

static int g_fail = 0;
static int g_checks = 0;
#define CHECK(c) do { ++g_checks; if (!(c)) { std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); ++g_fail; } } while (0)

static const std::vector<Vec2> kUnitSquare = {
    {0.f, 0.f}, {0.f, 1.f}, {1.f, 1.f}, {1.f, 0.f},
};

static void testBalloonPopThreshold() {
    CHECK(balloonPopResult(333.f, 333.f) == MinigameResult::GOAL);
    CHECK(balloonPopResult(332.999f, 333.f) == MinigameResult::NONE);
}

static void testGoalZoneResultMapping() {
    ArrivalCheck arrival;
    CHECK(goalZoneResult({0.5f, 0.5f}, 169.f, 111.f, kUnitSquare, arrival)
          == MinigameResult::GOAL);
    CHECK(goalZoneResult({0.5f, 0.5f}, 171.f, 110.f, kUnitSquare, arrival)
          == MinigameResult::MISS);
    CHECK(goalZoneResult({0.5f, 0.5f}, 171.f, 111.f, kUnitSquare, arrival)
          == MinigameResult::NONE);
}

static void testScorerAccumulatesGoalsOnly() {
    MinigameScorer scorer;
    scorer.add(MinigameResult::NONE);
    scorer.add(MinigameResult::GOAL);
    scorer.add(MinigameResult::MISS);
    scorer.add(MinigameResult::GOAL);
    CHECK(scorer.score() == 2);
}

static void testGolfRecords() {
    CHECK(golfHoleScore(0, 3) == -2);
    CHECK(golfHoleScore(2, 3) == 0);
    CHECK(golfHoleScore(98, 5) == 94);
    CHECK(golfHoleScore(99, 5) == 94);
    CHECK(golfHoleScore(std::numeric_limits<unsigned>::max(), 5) == 94);
    CHECK(golfRunningTotal(98, 4) == 99);
    CHECK(golfRunningTotal(-10, -2) == -12);
    CHECK(golfRoundOutcome(-1) == 2);
    CHECK(golfRoundOutcome(0) == 2);
    CHECK(golfRoundOutcome(1) == 1);
    GolfRecords records{{10,10,10,10,10,10,10,10,10}, 50};
    GolfRoundScore round;
    CHECK(recordGolfHole(records, round, 0, 2));
    CHECK(round.total == 0 && records.bestHole[0] == 0 && round.newHoleRecord[0]);
    CHECK(recordGolfHole(records, round, 0, 2));
    CHECK(records.bestHole[0] == 0 && round.newHoleRecord[0]); // flags are sticky
    GolfRoundScore tiedRound;
    CHECK(recordGolfHole(records, tiedRound, 0, 2));
    CHECK(!tiedRound.newHoleRecord[0]); // a tie alone does not set the flag
    CHECK(recordGolfHole(records, round, 8, 3));
    CHECK(round.total == -1 && records.bestTotal == -1 && round.newTotalRecord);
    CHECK(!recordGolfHole(records, round, 9, 0));
    CHECK(kGolfBestHoleSaveOffset == 0x12D0);
    CHECK(kGolfBestTotalSaveOffset == 0x12D9);
}

static void testGateScore() {
    CHECK(gateCourseScore(1, 1) == 301);
    CHECK(gateCourseScore(120, 2) == 720);
    CHECK(gateCourseScore(359999, 1) == 360000);
    CHECK(gateCourseScore(400000, 0) == 360000);
    CHECK(gateCourseScore(1, 100) == 29701);
    CHECK(gateCourseScore(0, 99) == 0); // zero time skips scoring in the ELF
    std::uint32_t best = 1000;
    CHECK(!recordGateCourse(best, 0, 0) && best == 1000);
    CHECK(recordGateCourse(best, 120, 2) && best == 720);
    CHECK(!recordGateCourse(best, 120, 2) && best == 720);
    CHECK(!recordGateCourse(best, 800, 0) && best == 720);
    CHECK(kGateBestSaveOffset == 0x12B4);
    CHECK(kGateBestSaveOffset + 5 * sizeof(std::uint32_t) == 0x12C8);
}

static void testConfirmedOutcomes() {
    CHECK(goalPlaneOutcome(false, 4200) == 1);
    CHECK(goalPlaneOutcome(false, 4201) == 1);
    CHECK(goalPlaneOutcome(true, 4200) == 2);
    CHECK(goalPlaneOutcome(true, 4201) == 0);
    CHECK(finishZoneOutcome(true, 10800) == 2);
    CHECK(finishZoneOutcome(true, 10801) == 1);
    CHECK(finishZoneOutcome(false, 0) == 1);
    CHECK(halfPlaneOutcome(true, 7200) == 2);
    CHECK(halfPlaneOutcome(true, 7201) == 1);
    CHECK(halfPlaneOutcome(false, 0) == 1);
    CHECK(duelCourseOutcome(7200) == 2);
    CHECK(duelCourseOutcome(7201) == 1);
    CHECK(dualViewLateOutcome(3000) == 3);
    CHECK(dualViewLateOutcome(3001) == 2);
    CHECK(rouletteBalanceOutcome(1000, 999) == 1);
    CHECK(rouletteBalanceOutcome(1000, 1001) == 2);
    CHECK(rouletteBalanceOutcome(1000, 1000) == 3);
    CHECK(collectionComplete(20));
    CHECK(!collectionComplete(19));
    CHECK(!collectionComplete(21)); // ELF tests equality, not >=
    CHECK(minigameTimerActive(4));
    CHECK(minigameTimerActive(0x104));
    CHECK(!minigameTimerActive(0));
    CHECK(!minigameTimerActive(0x8004));
}

static void testActualSensors() {
    CHECK(goalPlaneSensor({308.f, 284.f}, 169.f) == MinigameResult::GOAL);
    CHECK(goalPlaneSensor({308.f, 284.f}, 171.f) == MinigameResult::NONE);
    CHECK(goalPlaneSensor({311.f, 284.f}, 170.f) == MinigameResult::NONE);
    CHECK(goalPlaneSensor({308.f, 284.f}, 100.f) == MinigameResult::MISS);
    CHECK(goalPlaneSensor({110.f, 284.f}, 170.f) == MinigameResult::MISS);
    CHECK(balloonCourseResult(1349.f, true) == MinigameResult::MISS);
    CHECK(balloonCourseResult(1349.f, false) == MinigameResult::NONE);
    CHECK(balloonCourseResult(1350.f, true) == MinigameResult::GOAL);
    CHECK(goalAreaOutcome({410.f, 370.f}, 790.f, 0) == -1);
    CHECK(goalAreaOutcome({430.f, 790.f}, 790.f, 0) == -1);
    CHECK(goalAreaOutcome({409.f, 400.f}, 0.f, 0) == 0);
    CHECK(goalAreaOutcome({431.f, 400.f}, 0.f, 0) == 0);
    CHECK(goalAreaOutcome({420.f, 369.f}, 0.f, 0) == 0);
    CHECK(goalAreaOutcome({420.f, 791.f}, 790.f, 480) == 3);
    CHECK(goalAreaOutcome({420.f, 791.f}, 790.f, 481) == 2);
    CHECK(goalAreaOutcome({420.f, 791.f}, 791.f, 0) == 1);
    CHECK(goalAreaOutcome({409.f, 791.f}, 790.f, 0) == 3); // goal overwrites fail
}

int main() {
    testBalloonPopThreshold();
    testGoalZoneResultMapping();
    testScorerAccumulatesGoalsOnly();
    testGolfRecords();
    testGateScore();
    testConfirmedOutcomes();
    testActualSensors();
    if (g_fail) { std::fprintf(stderr, "%d failure(s)\n", g_fail); return 1; }
    std::printf("minigames: %d checks passed\n", g_checks);
    return 0;
}
