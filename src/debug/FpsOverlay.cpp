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

        // Frames the game presented: vblanks at which the displayed buffer changed.
        static double shownFps = 0.0;
        static auto windowStart = std::chrono::steady_clock::now();
        static uint64_t lastFlips = runtime.memory().displayFlips();

        const auto now = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(now - windowStart).count();
        if (elapsed >= 2.0)
        {
            const uint64_t flips = runtime.memory().displayFlips();
            shownFps = (flips - lastFlips) / elapsed;
            lastFlips = flips;
            // How busy the GIF/VIF1 worker and the GS thread were: the headroom left at 60 fps.
            static uint64_t lastWorker = runtime.memory().gifVif1BusyNanos();
            static uint64_t lastGs = runtime.memory().gsThreadBusyNanos();
            const uint64_t worker = runtime.memory().gifVif1BusyNanos();
            const uint64_t gs = runtime.memory().gsThreadBusyNanos();
            static uint64_t lastTick = runtime.memory().gs().vsyncTick.load();
            const uint64_t tick = runtime.memory().gs().vsyncTick.load();
            std::fprintf(stderr, "[fps] game %.1f fps (host %d, vblank %.1f/s), vif1/vu1 thread %.0f%% busy, gs thread %.0f%% busy\n",
                         shownFps, GetFPS(), (tick - lastTick) / elapsed, (worker - lastWorker) / elapsed / 1e7,
                         (gs - lastGs) / elapsed / 1e7);
            lastTick = tick;
            lastWorker = worker;
            lastGs = gs;
            windowStart = now;
        }
        DrawText(TextFormat("game %.1f fps", shownFps), 10, 10, 20, YELLOW);
    }
}
