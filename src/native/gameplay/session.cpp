#include "gameplay/session.h"

#include <algorithm>

namespace rt::native::gameplay {

namespace {

GameLoopParams makeRaceParams(int totalLaps) {
    GameLoopParams params;
    params.raceMode = true;
    params.totalLaps = std::max(totalLaps, 1);
    return params;
}

std::uint32_t rewardForRank(int rank) {
    // Simplified authored counterpart of AwardRaceResult (0x38950).
    if (rank == 0) return 1000;
    if (rank == 1) return 500;
    return 250;
}

// Peach Town square spawn, proven from the oracle golden frame
// (docs/native-reference/oracle-golden-frames.md:54: car at
// (506.283, 30.000, 442.230), clock 82974, just outside the factory).
// Heading 0 (+Z) is AUTHORED — the ledger records position, not yaw.
constexpr float kTownSpawnX = 506.283f;
constexpr float kTownSpawnYGame = 30.0f;  // square elevation, oracle y=30.000
constexpr float kTownSpawnZ = 442.230f;

}  // namespace

GameSession::GameSession(int totalLaps)
    : progress_(makeDefaultSave()), gameLoop_(makeRaceParams(totalLaps)) {
    // Begin every Adventure at the Peach Town square so facade reads,
    // explore camera and terrain streaming all show the town on boot.
    setExplorePose(kTownSpawnX, kTownSpawnZ, 0.f);
}

void GameSession::update(const MenuInput& input, const CarInput* driveInput) {
    switch (state_) {
    case SessionState::EXPLORE:
        if (input.confirm) startRace();
        // Town drive v1 (physics-wave slice, 2026-10-17): when the host is
        // driving via the test socket, free-drive the SAME validated car
        // physics + chase camera + world streaming as the race (GameLoop::
        // tickPlayer). The original town script (doors, traffic, pedestrians)
        // remains unmerged; the pose starts on the Peach Town square anchor
        // (kTownSpawn* above, oracle-proven). Null driveInput (every
        // non-lockstep run) keeps EXPLORE behaviour unchanged.
        if (driveInput) lastSnapshot_ = gameLoop_.tickPlayer(1.f / 60.f, input, *driveInput);
        break;
    case SessionState::RACE:
        raceTracker_.tick();
        lastSnapshot_ = driveInput ? gameLoop_.tickPlayer(1.f / 60.f, input, *driveInput)
                                   : gameLoop_.tick(1.f / 60.f, input);
        while (recordedLaps_ < lastSnapshot_.lap) {
            raceTracker_.completeLap();
            ++recordedLaps_;
        }
        if (lastSnapshot_.finished) {
            raceTracker_.finishRace();
            awardRaceResult();
            state_ = SessionState::RESULTS;
        }
        break;
    case SessionState::RESULTS:
        if (input.confirm) continueFromResults();
        break;
    }
}

void GameSession::startRace() {
    if (state_ != SessionState::EXPLORE) return;
    gameLoop_.reset();
    raceTracker_ = RaceTracker(0);
    raceTracker_.loadFromSave(progress_);
    recordedLaps_ = 0;
    lastSnapshot_ = FrameSnapshot();
    state_ = SessionState::RACE;
}

void GameSession::continueFromResults() {
    if (state_ != SessionState::RESULTS) return;
    state_ = SessionState::EXPLORE;
    // Back to the Peach Town square after a race (AUTHORED placement: the
    // oracle returns the car to town; the exact return yaw is unrecorded).
    setExplorePose(kTownSpawnX, kTownSpawnZ, 0.f);
}

void GameSession::setExplorePose(float x, float z, float headingRadians) {
    // The Peach Town square sits at game-unit y=30 (oracle ledger), lifted
    // through GameLoop::setCarPose so the chase camera rides above the plaza.
    gameLoop_.setCarPose(x, z, headingRadians, kTownSpawnYGame);
    // One idle tick rebuilds the facade-visible snapshot at the new pose
    // (setCarPose marks the camera uninitialised so this tick snaps to it).
    const CarInput idle{};
    lastSnapshot_ = gameLoop_.tickPlayer(1.f / 60.f, MenuInput{}, idle);
}

bool GameSession::save(MemoryCard& card, int slot) const {
    return card.saveSlot(slot, progress_);
}

bool GameSession::load(const MemoryCard& card, int slot) {
    return card.loadSlot(slot, progress_);
}

void GameSession::awardRaceResult() {
    const std::uint32_t reward = rewardForRank(lastSnapshot_.rank);
    progress_.money += std::min(reward, kMoneyCap - std::min(progress_.money, kMoneyCap));
    if (lastSnapshot_.rank >= 0 && lastSnapshot_.rank < static_cast<int>(kResultNotRaced))
        progress_.raceResults[0] = static_cast<std::uint8_t>(lastSnapshot_.rank);
    raceTracker_.writeToSave(progress_);
}

}  // namespace rt::native::gameplay
