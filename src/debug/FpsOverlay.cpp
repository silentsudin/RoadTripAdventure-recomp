#include "FpsOverlay.h"

#include "ps2_runtime.h"
#include "platform/Host.h"
#include "raylib.h"

#include <string>
#include <thread>

#include <chrono>
#include <cstdio>
#include <cstdlib>

namespace rt::debug
{
    namespace
    {
        uint64_t s_hostFrames = 0, s_lastHostFrames = 0; // host frames drawn
        double s_shownFps = 0.0;
    }

    namespace
    {
        bool enabled()
        {
            static const bool on = [] {
                const char *env = std::getenv("RT_SHOW_FPS");
                return env && *env && *env != '0';
            }();
            return on;
        }
    }

    void drawFpsOverlay(PS2Runtime &runtime)
    {
        if (!enabled())
            return;
        logFps(runtime);
        // On screen with raylib (the Vulkan presenter has the stderr line only, for now).
        if (std::string(rt::host::presenterName()) == "raylib")
            DrawText(TextFormat("game %.1f fps", s_shownFps), 10, 10, 20, YELLOW);
    }

    void startFpsLogIfHeadless(PS2Runtime &runtime)
    {
        const char *headless = std::getenv("RT_HEADLESS");
        if (!enabled() || !headless || *headless != '1')
            return;
        // No host frames headless: log from a timer thread (game, vblank and worker numbers only).
        std::thread([&runtime] {
            while (!runtime.isStopRequested())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
                logFps(runtime);
            }
        }).detach();
    }

    void logFps(PS2Runtime &runtime)
    {
        ++s_hostFrames;
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
            const double hostFps = (s_hostFrames - s_lastHostFrames) / elapsed;
            s_lastHostFrames = s_hostFrames;
            std::fprintf(stderr, "[fps] game %.1f fps (host %.0f, vblank %.1f/s), vif1/vu1 thread %.0f%% busy, gs thread %.0f%% busy\n",
                         shownFps, hostFps, (tick - lastTick) / elapsed, (worker - lastWorker) / elapsed / 1e7,
                         (gs - lastGs) / elapsed / 1e7);
            lastTick = tick;
            lastWorker = worker;
            lastGs = gs;
            windowStart = now;
        }
        s_shownFps = shownFps;
    }
}
