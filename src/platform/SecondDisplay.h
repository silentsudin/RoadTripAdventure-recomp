#pragma once

// A second display's surface and touches (Android: the activity's Presentation on another screen,
// such as the AYN Thor's lower one; RoadTripActivity.java). Elsewhere there is none.

#include <cstdint>
#include <vector>

namespace rt::seconddisplay
{
    // The newest surface (an acquired ANativeWindow*, or nullptr when it went away) if it changed
    // since the last call. The caller owns the reference it gets.
    bool takeWindow(void *&window);

    struct Touch
    {
        enum Kind : uint8_t { Down, Move, Up } kind;
        int pointer;
        float x, y; // surface pixels
    };
    // Touches since the last call, oldest first.
    std::vector<Touch> takeTouches();

    // Shows or hides the app's window on the second display (any thread; no-op without one).
    void setEnabled(bool on);
}
