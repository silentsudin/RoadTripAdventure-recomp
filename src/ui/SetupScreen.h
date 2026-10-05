#pragma once

// The first-run setup screen (extracting the disc, translating and compiling the game) in Road
// Trip's style (src/ui/Theme): the step, a progress bar, the percentage and the time left, from
// this device's measured speed. On failure, what went wrong and the end of the build log until a
// button, key or tap closes it. Drawn inside an ImGui frame over the presenter (Android, where the
// setup has no window of its own).

#include "platform/TaskProgress.h"

#include <string>

namespace rt::ui
{
    // One frame of the screen. `now` in seconds. Returns true once a failure has been acknowledged.
    bool drawSetupScreen(const TaskProgress &progress, double now);

    // Before the first setup: what the app needs (the player's own disc image) and a prompt to
    // choose it. `note`: why the last choice didn't work, or empty. Returns true when confirmed.
    bool drawWelcomeScreen(const std::string &note);
}
