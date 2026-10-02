#include "FpsOverlay.h"

#include "ps2_runtime.h"
#include "raylib.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace rt::debug
{
    namespace
    {
        std::atomic<uint32_t> g_flips{0};

        // The game flips at most once per vblank (16.7 ms). Sampling DISPFB1 every millisecond
        // on its own thread sees every flip; sampling it once per host frame missed flips
        // whenever the host and game rates were close (A->B->A between two samples).
        void startFlipSampler(PS2Runtime &runtime)
        {
            std::thread([&runtime] {
                uint64_t last = 0, previousSample = 0;
                for (;;)
                {
                    // A new value only counts once it has held for two samples: the register is
                    // written in halves (and read without locking), so transient values appear.
                    const uint64_t dispfb = runtime.memory().gs().dispfb1;
                    if (dispfb == previousSample && dispfb != last)
                    {
                        last = dispfb;
                        g_flips.fetch_add(1, std::memory_order_relaxed);
                    }
                    previousSample = dispfb;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }).detach();
        }
    }

    void drawFpsOverlay(PS2Runtime &runtime)
    {
        static const bool enabled = [] {
            const char *env = std::getenv("RT_SHOW_FPS");
            return env && *env && *env != '0';
        }();
        if (!enabled)
            return;

        static double shownFps = 0.0;
        static auto windowStart = std::chrono::steady_clock::now();
        static const bool started = (startFlipSampler(runtime), true);
        (void)started;

        const auto now = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(now - windowStart).count();
        if (elapsed >= 2.0)
        {
            shownFps = g_flips.exchange(0, std::memory_order_relaxed) / elapsed;
            std::fprintf(stderr, "[fps] game %.1f fps (host %d)\n", shownFps, GetFPS());
            windowStart = now;
        }
        DrawText(TextFormat("game %.1f fps", shownFps), 10, 10, 20, YELLOW);
    }
}
