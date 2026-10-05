#pragma once

// The first-run setup screens in Road Trip's style (src/ui/Theme): welcome, a warning when the disc
// image doesn't verify, and the progress of checking, extracting, translating and compiling (step,
// bar, percentage, time left from this device's measured speed; on failure, what went wrong and
// "Try again"). Setup runs before the input layer, so there are no button glyphs: buttons are
// tapped, or focused with the D-pad, stick or arrow keys and pressed with A, Start or Enter. Drawn
// inside an ImGui frame over the presenter (Android, where setup has no window of its own).

#include "platform/TaskProgress.h"

#include <string>

namespace rt::ui
{
    // One frame of the setup progress, or, once `progress` has failed, what went wrong with
    // "Try again" and "Close". `now` in seconds.
    enum class SetupAction
    {
        None,
        Retry,
        Close,
    };
    SetupAction drawSetupScreen(const TaskProgress &progress, double now);

    // Before the first setup: what the app needs (the player's own disc image) and a button to
    // choose it. `note`: why the last choice didn't work (no file, not Road Trip), or empty.
    enum class WelcomeAction
    {
        None,
        Choose,
        UseAnyway,
    };
    WelcomeAction drawWelcomeScreen(const std::string &note);

    // The chosen image didn't verify against the good dump: choose another or use it anyway.
    enum class ImageWarning
    {
        Mismatch,   // a .bin/.iso that isn't the good dump
        Damaged,    // a CHD whose data no longer matches its own record
        Unverified, // an intact CHD made another way (most likely fine)
    };
    WelcomeAction drawImageWarning(ImageWarning kind);

    // Over the last setup frame: black at `amount` (0..1), fading into the game.
    void drawFadeOut(float amount);
}
