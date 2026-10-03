#pragma once

// The Controllers window (ImGui): devices and which player they play as, rebinding, deadzones,
// vibration. F2, the Guide button, or Back+Start held for 2 s open and close it; the game reads
// neutral pads while it is open. Render thread only.

namespace rt::input
{
    // Every frame (handles the open/close keys and rebinding), before drawing.
    void updateControllersWindow();
    bool controllersWindowOpen();
    // Inside an ImGui frame.
    void drawControllersWindow();
}
