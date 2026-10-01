#pragma once

class PS2Runtime;

namespace rt::debug
{
    // Call once per host frame on the render thread. With RT_SHOW_FPS=1, counts the game's
    // buffer flips (DISPFB changes) and shows game FPS on screen and in the log every 2 seconds.
    void drawFpsOverlay(PS2Runtime &runtime);
}
