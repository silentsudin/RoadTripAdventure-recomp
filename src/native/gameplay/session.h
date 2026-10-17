// Authored Adventure session flow replacing the original mode-dispatch glue:
// explore -> race -> results -> explore. It owns simulation and persistent
// progress but leaves memory-card I/O to explicit save/load calls.
#pragma once

#include "gameplay/game_loop.h"
#include "gameplay/memcard.h"
#include "gameplay/save.h"

namespace rt::native::gameplay {

enum class SessionState { EXPLORE, RACE, RESULTS };

class GameSession {
public:
    // A single lap keeps the authored host loop concise while GameLoop remains
    // configurable for callers that want longer races.
    explicit GameSession(int totalLaps = 1);

    [[nodiscard]] SessionState state() const { return state_; }
    void update(const MenuInput& input, const CarInput* driveInput = nullptr);

    void startRace();
    void continueFromResults();

    // Anchors the authoritative car pose and refreshes the facade-visible
    // snapshot with one idle tick. Used to place the explore car on the
    // Peach Town square (kTownSpawn* in session.cpp, oracle-proven).
    // Simulation thread only (mirrors GameLoop::setCarPose).
    void setExplorePose(float x, float z, float headingRadians);

    [[nodiscard]] const SaveData& progress() const { return progress_; }
    [[nodiscard]] const FrameSnapshot& lastSnapshot() const { return lastSnapshot_; }
    [[nodiscard]] const World& world() const { return gameLoop_.world(); }
    [[nodiscard]] int totalLaps() const { return gameLoop_.params().totalLaps; }

    bool save(MemoryCard& card, int slot) const;
    bool load(const MemoryCard& card, int slot);

private:
    SaveData progress_;
    GameLoop gameLoop_;

  public:
    // Host projection policy (widescreen, output-modes.md): widened 3D with
    // fovY pinned.  Re-applied every frame after update() because startRace()
    // rebuilds loop params from makeRaceParams().
    void setProjectionAspect(float aspect) { gameLoop_.setProjectionAspect(aspect); }

  private:
    RaceTracker raceTracker_{0};
    FrameSnapshot lastSnapshot_;
    SessionState state_ = SessionState::EXPLORE;
    int recordedLaps_ = 0;

    void awardRaceResult();
};

}  // namespace rt::native::gameplay
