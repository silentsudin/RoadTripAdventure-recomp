#pragma once

class PS2Runtime;

namespace rt::debug
{
    // Call once per host frame on the render thread. With RT_SHOW_FPS=1, counts the game's
    // buffer flips (DISPFB changes) and shows game FPS on screen and in the log every 2 seconds.
    void drawFpsOverlay(PS2Runtime &runtime);
    // The [fps] log line every 2 s (game fps, vblank rate, worker load); drawFpsOverlay calls it.
    void logFps(PS2Runtime &runtime);
    // RT_SHOW_FPS=1 with RT_HEADLESS=1: there are no host frames, so log from a timer thread.
    void startFpsLogIfHeadless(PS2Runtime &runtime);
}
