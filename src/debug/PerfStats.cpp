// Performance numbers for the overlay (see PerfStats.h).

#include "debug/PerfStats.h"

#include "ps2_runtime.h"
#include "runtime/ps2_host_presenter.h"

#include <sys/resource.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

namespace rt::debug
{
    namespace
    {
        using Clock = std::chrono::steady_clock;
        PerfStats g_stats;

        double cpuSeconds()
        {
            rusage u{};
            getrusage(RUSAGE_SELF, &u);
            return u.ru_utime.tv_sec + u.ru_stime.tv_sec + (u.ru_utime.tv_usec + u.ru_stime.tv_usec) / 1e6;
        }

#if defined(__ANDROID__)
        long readNumber(const char *path)
        {
            std::ifstream in(path);
            long v = -1;
            in >> v; // "4 %" reads 4
            return in ? v : -1;
        }
#endif
    }

    void samplePerf(PS2Runtime &runtime)
    {
        static auto windowStart = Clock::now(), lastFlipAt = windowStart;
        static uint64_t lastFlips = runtime.memory().displayFlips(), windowFlips = lastFlips;
        static uint64_t lastVu1 = runtime.memory().gifVif1BusyNanos(), lastGs = runtime.memory().gsThreadBusyNanos();
        static double lastCpu = cpuSeconds(), worst = 0.0;

        const auto now = Clock::now();
        const uint64_t flips = runtime.memory().displayFlips();
        if (flips != lastFlips)
        {
            worst = std::max(worst, std::chrono::duration<double, std::milli>(now - lastFlipAt).count());
            lastFlipAt = now;
            lastFlips = flips;
        }
        const double elapsed = std::chrono::duration<double>(now - windowStart).count();
        if (elapsed < 0.5)
            return;

        PerfStats s;
        s.valid = true;
        s.fps = (flips - windowFlips) / elapsed;
        // Frame generation adds pictures between the game's (Options > Frame rate).
        const ps2x::HostPresenter *presenter = runtime.presenter();
        // Frame skip (GS::setFrameSkip): skipped frames show nothing new, and generated frames pause
        // while the game is behind.
        static uint64_t lastSkipped = runtime.gsUnsynced().framesSkipped();
        const uint64_t skipped = runtime.gsUnsynced().framesSkipped();
        const uint32_t skipLevel = runtime.gsUnsynced().frameSkipLevel();
        const double drawn = std::max(0.0, s.fps - (skipped - lastSkipped) / elapsed);
        s.shownFps = drawn * (presenter && skipLevel == 0u ? std::max<uint32_t>(presenter->frameGenerationFactor(), 1u) : 1u);
        s.worstFrameMs = worst;
        const uint64_t vu1 = runtime.memory().gifVif1BusyNanos(), gs = runtime.memory().gsThreadBusyNanos();
        s.vu1Busy = (vu1 - lastVu1) / elapsed / 1e7;
        s.gsBusy = (gs - lastGs) / elapsed / 1e7;
        const double cpu = cpuSeconds();
        s.appCpu = (cpu - lastCpu) / elapsed * 100.0;
#if defined(__ANDROID__)
        s.gpuBusy = static_cast<int>(readNumber("/sys/class/kgsl/kgsl-3d0/gpu_busy_percentage"));
        const long hz = readNumber("/sys/class/kgsl/kgsl-3d0/gpuclk");
        s.gpuMHz = hz > 0 ? static_cast<int>(hz / 1000000) : -1;
        const long tenths = readNumber("/sys/class/power_supply/battery/temp");
        s.batteryC = tenths > 0 ? tenths / 10.0 : -1.0;
#endif
#if defined(__ANDROID__)
        // GPU headroom for frame generation: with the GPU at 85% or more for two windows the
        // generated frames pause (the Thor's display fell behind near 100%: garbage strips on the
        // lower screen in towns and Q's Factory at 120 Hz). They come back when the scene gets
        // lighter: the GPU 12 points under its load in the first 2 s of the pause, for 2 s (leaving
        // Q's Factory, a loading screen). Not on a timer: a retry in the same heavy scene garbled
        // the lower screen again; and not by predicting their cost (1.6x the busy time at 60 kept
        // town and the field at 60 for good).
        {
            static int high = 0, sampled = 0, lighter = 0, baseline = 0;
            static double pausedSince = -1.0;
            const double now = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) / 1000.0;
            const bool fg = presenter && presenter->frameGenerationFactor() > 1u;
            if (!fg || s.gpuBusy < 0)
                high = 0, pausedSince = -1.0;
            else if (pausedSince < 0.0)
            {
                high = s.gpuBusy >= 85 ? high + 1 : 0;
                if (high >= 2)
                    pausedSince = now, high = 0, sampled = 0, lighter = 0, baseline = 0;
            }
            else if (sampled < 5) // 2 s at 60 (after the window the pause began in): this scene's load
            {
                if (sampled++ > 0)
                    baseline = std::max(baseline, s.gpuBusy);
            }
            else
            {
                lighter = s.gpuBusy <= baseline - 12 ? lighter + 1 : 0;
                if (lighter >= 4)
                    pausedSince = -1.0;
            }
            runtime.gsUnsynced().setGpuOverload(pausedSince >= 0.0);
            s.gpuOverload = pausedSince >= 0.0;
            if (s.gpuOverload)
                s.shownFps = s.fps;
        }
#endif
        // RT_PERF_LOG=1: every window to the log (measurements over a whole run).
        static const bool log = [] { const char *e = std::getenv("RT_PERF_LOG"); return e && *e == '1'; }();
        if (log)
        {
            // The CPU clusters' clocks now (Android: little, mid, prime on the Thor), so a slow
            // window can be told from one the governor clocked down.
            char clocks[64] = "";
#if defined(__ANDROID__)
            int at = 0;
            for (int c : {0, 3, 7})
            {
                char path[80];
                std::snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", c);
                const long khz = readNumber(path);
                if (khz > 0 && at < static_cast<int>(sizeof(clocks)) - 12)
                    at += std::snprintf(clocks + at, sizeof(clocks) - at, "%s%.2f", at ? "/" : ", clocks ", khz / 1e6);
            }
            if (at)
                std::snprintf(clocks + at, sizeof(clocks) - at, " GHz");
#endif
            std::fprintf(stderr, "[perf] %.1f fps (shown %.0f), worst %.1f ms, VU1 %.0f%%, GS %.0f%%, CPU %.0f%%, GPU %d%% at %d MHz, skip level %u (%llu skipped)%s\n",
                         s.fps, s.shownFps, s.worstFrameMs, s.vu1Busy, s.gsBusy, s.appCpu, s.gpuBusy, s.gpuMHz, skipLevel,
                         static_cast<unsigned long long>(skipped - lastSkipped), clocks);
        }
        lastSkipped = skipped;
        g_stats = s;
        windowStart = now;
        windowFlips = flips;
        lastVu1 = vu1;
        lastGs = gs;
        lastCpu = cpu;
        worst = 0.0;
    }

    const PerfStats &perfStats() { return g_stats; }
}
