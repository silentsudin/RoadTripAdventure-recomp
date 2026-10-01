#include "FpsOverlay.h"

#include "ps2_runtime.h"
#include "raylib.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>

namespace rt::debug
{
    void drawFpsOverlay(PS2Runtime &runtime)
    {
        static const bool enabled = [] {
            const char *env = std::getenv("RT_SHOW_FPS");
            return env && *env && *env != '0';
        }();
        if (!enabled)
            return;

        static uint64_t lastDispfb = 0;
        static uint32_t flips = 0;
        static double shownFps = 0.0;
        static auto windowStart = std::chrono::steady_clock::now();

        // Read without locking: a torn read only miscounts one flip.
        const uint64_t dispfb = runtime.memory().gs().dispfb1;
        if (dispfb != lastDispfb)
        {
            lastDispfb = dispfb;
            ++flips;
        }

        const auto now = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(now - windowStart).count();
        if (elapsed >= 2.0)
        {
            shownFps = flips / elapsed;
            std::fprintf(stderr, "[fps] game %.1f fps (host %d)\n", shownFps, GetFPS());
            flips = 0;
            windowStart = now;
        }
        DrawText(TextFormat("game %.1f fps", shownFps), 10, 10, 20, YELLOW);
    }
}
