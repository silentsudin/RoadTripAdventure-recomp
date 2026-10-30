// Authored Adventure session flow replacing the original mode-dispatch glue:
// explore -> race -> results -> explore. It owns simulation and persistent
// progress but leaves memory-card I/O to explicit save/load calls.
#pragma once

#include "gameplay/game_loop.h"
#include "gameplay/memcard.h"
#include "gameplay/save.h"

namespace rt::native::gameplay {

enum class SessionState { EXPLORE, RACE, RESULTS };

// Peach Town square spawn, proven from the oracle golden frame
// (docs/native-reference/oracle-golden-frames.md:54: car at
// (506.283, 30.000, 442.230), clock 82974, just outside the factory).
// Heading 0 (+Z) is AUTHORED — the ledger records position, not yaw.
inline constexpr float kTownSpawnX = 506.283f;
inline constexpr float kTownSpawnYGame = 30.0f;  // square elevation, oracle y=30.000
inline constexpr float kTownSpawnZ = 442.230f;

class GameSession {
public:
    // A single lap keeps the authored host loop concise while GameLoop remains
    // configurable for callers that want longer races. `farPlane` is the
    // scene's far clip: the app raises it with the draw-distance row so a
    // wider terrain neighbourhood is not cut at its far corner.
    explicit GameSession(int totalLaps = 1, float farPlane = 500.f);

    [[nodiscard]] SessionState state() const { return state_; }
    void update(const MenuInput& input, const CarInput* driveInput = nullptr);

    void startRace();
    void continueFromResults();

    // Anchors the authoritative car pose and refreshes the facade-visible
    // snapshot with one idle tick. Defaults to the Peach Town square
    // elevation (kTownSpawnYGame, oracle-proven); pass an explicit
    // yGameUnits for other tiles (0 = datum; GAP: per-town elevations
    // unmerged). Simulation thread only (mirrors GameLoop::setCarPose).
    void setExplorePose(float x, float z, float headingRadians,
                       float yGameUnits = kTownSpawnYGame);

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

    // Right-stick analog camera (Options > Controllers); (0,0) = original view.
    void setCameraLook(float stickX, float stickY) { gameLoop_.setCameraLook(stickX, stickY); }

  private:
    RaceTracker raceTracker_{0};
    FrameSnapshot lastSnapshot_;
    SessionState state_ = SessionState::EXPLORE;
    int recordedLaps_ = 0;

    void awardRaceResult();
};

}  // namespace rt::native::gameplay
