#include "gameplay/minigames.h"

#include <algorithm>

namespace rt::native::gameplay {

MinigameResult balloonPopResult(float playerY, float threshold) {
    return playerY >= threshold ? MinigameResult::GOAL : MinigameResult::NONE;
}

MinigameResult goalZoneResult(Vec2 p, float playerY, float speed,
                              const std::vector<Vec2>& quad,
                              const ArrivalCheck& arrival) {
    switch (checkGoalZone(p, playerY, speed, quad, arrival)) {
    case ZoneResult::ARRIVED:
        return MinigameResult::GOAL;
    case ZoneResult::FAILED:
        return MinigameResult::MISS;
    case ZoneResult::NONE:
        return MinigameResult::NONE;
    }
    return MinigameResult::NONE;
}

void MinigameScorer::add(MinigameResult result) {
    if (result == MinigameResult::GOAL) ++score_;
}

int golfHoleScore(unsigned strokesBeforeCup, int par) {
    // CONFIRMED addiu +1, signed slti 100, subu par: 0x252748..0x252790.
    return static_cast<int>(std::min(strokesBeforeCup, 98u)) + 1 - par;
}

int golfRunningTotal(int previous, int relativeScore) {
    // CONFIRMED upper clamp ONLY, 0x2527A4..0x2527BC. Wide authored
    // intermediate avoids C++ overflow; valid nine-hole scores fit an int.
    return static_cast<int>(std::min<std::int64_t>(
        static_cast<std::int64_t>(previous) + relativeScore, 99));
}

int golfRoundOutcome(int total) {
    return total <= 0 ? 2 : 1; // CONFIRMED blez 0x252850
}

bool recordGolfHole(GolfRecords& records, GolfRoundScore& round,
                    unsigned hole, unsigned strokesBeforeCup) {
    if (hole >= kGolfHoles) return false;
    const int score = golfHoleScore(strokesBeforeCup, kGolfPar[hole]);
    round.total = golfRunningTotal(round.total, score);
    if (score < records.bestHole[hole]) {
        records.bestHole[hole] = static_cast<std::int8_t>(score);
        round.newHoleRecord[hole] = true;
    }
    // CONFIRMED strict signed comparisons and hole==8, 0x2527C8..0x252840.
    if (hole == kGolfHoles - 1) {
        if (round.total < records.bestTotal) {
            records.bestTotal = static_cast<std::int8_t>(round.total);
            round.newTotalRecord = true;
        }
    }
    return true;
}

std::uint32_t gateCourseScore(std::uint32_t frames, unsigned penalties) {
    if (frames == 0) return 0; // CONFIRMED beq 0x25AAA8 skips score/best stores
    return std::min(std::min(frames, kGateScoreCap)
                    + std::min(penalties, 99u) * kGatePenaltyFrames, kGateScoreCap);
}

bool recordGateCourse(std::uint32_t& best, std::uint32_t frames, unsigned penalties) {
    if (frames == 0) return false;
    const auto score = gateCourseScore(frames, penalties);
    if (score >= best) return false;
    best = score; // CONFIRMED slt/store 0x25AAF4..0x25AB10
    return true;
}

int goalPlaneOutcome(bool previouslyCleared, std::uint32_t frames) {
    return !previouslyCleared ? 1 : (frames < 4201 ? 2 : 0);
}

int finishZoneOutcome(bool previouslyCleared, std::uint32_t frames) {
    return previouslyCleared && frames < 10801 ? 2 : 1;
}

int halfPlaneOutcome(bool previouslyCleared, std::uint32_t frames) {
    return previouslyCleared && frames < 7201 ? 2 : 1;
}

int duelCourseOutcome(std::uint32_t frames) {
    return frames < 7201 ? 2 : 1;
}

int dualViewLateOutcome(std::uint32_t frames) {
    return frames < 3001 ? 3 : 2;
}

int rouletteBalanceOutcome(std::uint32_t start, std::uint32_t end) {
    return end < start ? 1 : (end > start ? 2 : 3);
}

bool collectionComplete(unsigned collected) {
    return collected == 20;
}

bool minigameTimerActive(std::uint32_t flags) {
    return (flags & 0x8004) == 4;
}

MinigameResult goalPlaneSensor(Vec2 p, float playerY) {
    // CONFIRMED table 0x2C4C40, strict interior, c.le/c.lt Y band.
    if (playerY >= 169.f && playerY < 171.f
        && pointInConvexQuad(p, {311.f,286.5f}, {311.f,282.2f},
                             {306.5f,282.2f}, {306.5f,286.5f})) {
        return MinigameResult::GOAL;
    }
    // It is world X, not speed: lwc1 0x90(s0), c.le.S at 0x25E92C.
    if (playerY <= 100.f || p.x <= 110.f) return MinigameResult::MISS;
    return MinigameResult::NONE;
}

MinigameResult balloonCourseResult(float playerZ, bool collision) {
    // CONFIRMED 0x25E468 checks goal before child collision at task+4.
    if (playerZ >= 1350.f) return MinigameResult::GOAL;
    return collision ? MinigameResult::MISS : MinigameResult::NONE;
}

int goalAreaOutcome(Vec2 p, float opponentZ, std::uint32_t frames) {
    // CONFIRMED 0x260754..0x260884; constant 790.f at 0x333170.
    // Independent tests: reaching the goal can overwrite the earlier failure.
    if (p.z > 790.f) return opponentZ > 790.f ? 1 : (frames < 481 ? 3 : 2);
    if (p.x < 410.f || p.x > 430.f || p.z < 370.f) return 0;
    return -1;
}

}  // namespace rt::native::gameplay
