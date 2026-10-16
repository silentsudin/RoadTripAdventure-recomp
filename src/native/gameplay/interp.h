// Render-frame interpolation for the fixed 60-Hz simulation.
//
// Ground truth: docs/native-reference/frame-timing.md PROVES the original
// logic is VBlank-locked fixed-step (verdict a) — one task-walker pass per
// sceGsSyncV wait, town clock/vehicle integration/minigame counters all
// advance by fixed integer amounts per pass. The native rebuild therefore
// KEEPS the 1/60 s logic quantum on the host and only interpolates rendered
// POSES between completed logic steps so >60-Hz displays show smooth motion
// ("Higher refresh visuals" in that document). This module never feeds back
// into simulation state; logic rates are untouched.
//
// Pure host C++, no clocks (callers supply elapsed time), no GPU, no game
// data. See docs/native-reference/frame-interpolation.md for the design.
#pragma once

#include "gameplay/game_loop.h"  // FrameSnapshot
#include "gameplay/vec.h"

#include <cstdint>

namespace rt::native::gameplay {

// ---------------------------------------------------------------- pose

// Exactly the renderable pose previewRenderScene consumes for placement
// (src/native/preview/preview.cpp): the player-car position/heading
// (FrameSnapshot::carPosition/heading drive the car transform) and the chase
// camera eye/target (FrameSnapshot::cameraEye/cameraTarget; the view matrix
// is derived from them). Camera pitch is embedded in eye/target and the
// preview draws a single car, so there is no separate per-AI traffic pose to
// snapshot today — extend this struct if traffic rendering lands.
// Lighting/fog and HUD stay fixed-step by the design contract (they belong
// to the simulation clock, not to a continuous pose).
struct RenderPose {
    Vec2 carPosition{};
    float heading = 0.f;  // radians, 0 = +Z (FrameSnapshot convention)
    Vec3 cameraEye{};
    Vec3 cameraTarget{};
};

// Extracts the renderable pose of a finished logic step. This is the
// step-boundary pose the deterministic capture path records.
[[nodiscard]] RenderPose renderPoseOf(const FrameSnapshot& snapshot);

// Writes a pose back into a snapshot copy. The column-major view matrix is
// rebuilt with lookAtMatrix(eye, target) exactly like GameLoop::makeSnapshot,
// so snapshotWithRenderPose(s, renderPoseOf(s)) is bit-identical to s — the
// boundary identity the determinism contract relies on.
[[nodiscard]] FrameSnapshot snapshotWithRenderPose(const FrameSnapshot& snapshot,
                                                   const RenderPose& pose);

// Shortest-arc angle interpolation: the difference b - a is wrapped to
// [-pi, pi] (camera.h wrapAngle) before scaling, so a heading crossing
// +/-pi never spins the long way round. The result is not re-wrapped, so
// t = 0 returns `a` bit-exactly (t = 0 consumers pass it straight to
// sin/cos, which are 2-pi periodic).
[[nodiscard]] float lerpAngleShortestArc(float a, float b, float t);

// Linear render-pose blend at fraction t in [0, 1]: positions and camera
// eye/target lerped per component, heading through the shortest arc.
[[nodiscard]] RenderPose lerpRenderPose(const RenderPose& prev, const RenderPose& cur,
                                        float t);

// ---------------------------------------------------------------- clock

// Fixed-timestep accumulator driving the 60-Hz host logic quantum:
//
//     accumulator += real elapsed;
//     while (accumulator >= step && steps < catchUpCap) { run one step; accumulator -= step; }
//     render with alpha = accumulator / step
//
// The catch-up cap bounds how many logic quanta a single host frame may
// drain (default 2, RT_APP_INTERP_CAP), so a long stall recovers gradually
// instead of teleporting the simulation; alpha clamps to [0, 1] so a
// capped frame can never extrapolate past the newest step. stepsTaken() is
// the N in "rt-app interp: steps=N" — the host loop guarantees it equals
// RT_APP_FRAMES, which is how the matrix proves the sim rate is unchanged.
class FixedStepClock {
public:
    // The logic quantum: one 60-Hz host step standing in for the original
    // VBlank-paced task pass (frame-timing.md verdict a).
    static constexpr double kStepSeconds = 1.0 / 60.0;
    static constexpr int kDefaultCatchUpCap = 2;

    explicit FixedStepClock(int catchUpCap = kDefaultCatchUpCap);

    void addElapsed(double seconds);  // accumulator += real elapsed host time

    [[nodiscard]] bool stepDue() const;        // at least one full step is banked
    [[nodiscard]] int stepsReady() const;      // banked steps, clamped to the catch-up cap
    void takeStep();                           // consume one banked step (accumulator -= step)
    [[nodiscard]] float alpha() const;         // leftover fraction of the next step, clamped to [0, 1]
    [[nodiscard]] std::uint64_t stepsTaken() const;
    [[nodiscard]] double accumulatorSeconds() const;
    [[nodiscard]] int catchUpCap() const;

private:
    double accumulatorSeconds_ = 0.0;
    std::uint64_t stepsTaken_ = 0;
    int catchUpCap_ = kDefaultCatchUpCap;
};

}  // namespace rt::native::gameplay
