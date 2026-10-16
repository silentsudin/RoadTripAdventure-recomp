// Authored mini-game scoring predicates. No game data.
#pragma once

#include "gameplay/geometry.h"
#include "gameplay/rules_tables.h"

#include <array>
#include <cstdint>
#include <vector>

namespace rt::native::gameplay {

enum class MinigameResult {
    NONE,
    GOAL,
    MISS,
};

[[nodiscard]] MinigameResult balloonPopResult(float playerY, float threshold);
[[nodiscard]] MinigameResult goalZoneResult(Vec2 p, float playerY, float speed,
                                             const std::vector<Vec2>& quad,
                                             const ArrivalCheck& arrival);

class MinigameScorer {
public:
    void add(MinigameResult result);
    [[nodiscard]] int score() const { return score_; }

private:
    int score_ = 0;
};

// The generic predicates/scorer above are retained compatibility helpers, not
// ELF outcome codes. The APIs below model independently verified SLUS rules.
// Outcome -1 means still playing; 0..3 are the raw 0x23D8C8 result channel.

// CONFIRMED 0x2527E4/0x252830: signed-byte records relative to par.
inline constexpr std::uint32_t kGolfBestHoleSaveOffset = 0x12D0;
inline constexpr std::uint32_t kGolfBestTotalSaveOffset = 0x12D9;
struct GolfRecords {
    // Supply existing save values; no unverified new-game sentinel is assumed.
    std::array<std::int8_t, kGolfHoles> bestHole;
    std::int8_t bestTotal;
};
struct GolfRoundScore {
    int total = 0;
    std::array<bool, kGolfHoles> newHoleRecord{};
    bool newTotalRecord = false;
};
[[nodiscard]] int golfHoleScore(unsigned strokesBeforeCup, int par);
[[nodiscard]] int golfRunningTotal(int previous, int relativeScore);
[[nodiscard]] int golfRoundOutcome(int total);
// Caller supplies each hole in play order. Invalid indices are an authored
// guard (false/no mutation), not a claimed ELF behavior. Completion is hole 8.
[[nodiscard]] bool recordGolfHole(GolfRecords& records, GolfRoundScore& round,
                                  unsigned hole, unsigned strokesBeforeCup);

// CONFIRMED 0x25AA58..0x25AB20: five u32 best-frame records, strict improvement.
inline constexpr std::uint32_t kGateBestSaveOffset = 0x12B4;
inline constexpr std::uint32_t kGateScoreCap = 360000;
inline constexpr std::uint32_t kGatePenaltyFrames = 300;
// Zero time is an unscored run. The score helper returns 0 in that case;
// recordGateCourse preserves the previous best (matching the ELF early exit).
[[nodiscard]] std::uint32_t gateCourseScore(std::uint32_t frames, unsigned penalties);
[[nodiscard]] bool recordGateCourse(std::uint32_t& best, std::uint32_t frames,
                                    unsigned penalties);

// CONFIRMED result branches: 0x25E880..0x25E8BC (progress pair 0x37),
// 0x25F8C4..0x25F8F4 (0x0E), 0x25FD1C..0x25FD4C (0x39).
// These booleans select repeat-clear tiers, NOT entry/unlock permission.
[[nodiscard]] int goalPlaneOutcome(bool previouslyCleared, std::uint32_t frames);
[[nodiscard]] int finishZoneOutcome(bool previouslyCleared, std::uint32_t frames);
[[nodiscard]] int halfPlaneOutcome(bool previouslyCleared, std::uint32_t frames);
// Only the 1P finish / late-finish branches, not the full 2P state machines.
[[nodiscard]] int duelCourseOutcome(std::uint32_t frames);       // 0x25F000
[[nodiscard]] int dualViewLateOutcome(std::uint32_t frames);     // 0x2601CC
[[nodiscard]] int rouletteBalanceOutcome(std::uint32_t start, std::uint32_t end); // 0x262458
[[nodiscard]] bool collectionComplete(unsigned collected);      // 0x25DDF8
[[nodiscard]] bool minigameTimerActive(std::uint32_t flags);     // 0x25E95C

// CONFIRMED actual sensors, separate from the legacy configurable helpers.
[[nodiscard]] MinigameResult goalPlaneSensor(Vec2 p, float playerY); // 0x25E724..0x25E954
[[nodiscard]] MinigameResult balloonCourseResult(float playerZ, bool collision); // 0x25E458
[[nodiscard]] int goalAreaOutcome(Vec2 p, float opponentZ, std::uint32_t frames); // 0x260754

}  // namespace rt::native::gameplay
