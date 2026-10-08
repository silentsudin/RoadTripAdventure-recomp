#pragma once

// What the performance overlay shows, measured over half-second windows: the game's frame rate and
// its longest frame, how busy the VU1 and GS threads were, the whole app's CPU use and, on Android,
// the GPU's load and clock and the battery temperature (the parts an app may read there).

class PS2Runtime;

namespace rt::debug
{
    struct PerfStats
    {
        bool valid = false;
        double fps = 0.0;          // frames the game showed per second
        double shownFps = 0.0;     // pictures on screen per second: the game's and the generated ones
        double worstFrameMs = 0.0; // the longest gap between them in the window
        double vu1Busy = -1.0;     // % of one core
        double gsBusy = -1.0;      // % of one core
        double appCpu = -1.0;      // % of one core, every thread of the app
        int gpuBusy = -1;          // % (Android)
        bool gpuOverload = false;  // generated frames paused for GPU headroom (Android)
        int gpuMHz = -1;           // (Android)
        double batteryC = -1.0;    // °C (Android)
    };

    // Once per host frame on the render thread.
    void samplePerf(PS2Runtime &runtime);
    const PerfStats &perfStats();
}
