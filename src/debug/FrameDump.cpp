#include "FrameDump.h"

#include "platform/Host.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace rt::debug
{
    void maybeDumpFrame()
    {
        static const char *dir = std::getenv("RT_FRAME_DUMP");
        if (!dir || !*dir)
            return;
        static const double interval = [] {
            const char *s = std::getenv("RT_FRAME_DUMP_SECONDS");
            return (s && std::atof(s) > 0) ? std::atof(s) : 2.0;
        }();
        static auto last = std::chrono::steady_clock::now();
        static int index = 0;
        const auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(now - last).count() < interval)
            return;
        last = now;

        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        char name[64];
        std::snprintf(name, sizeof(name), "/frame_%04d.png", index++);
        // Saved by the presenter as it presents this frame (with any open menu).
        rt::host::screenshot(std::string(dir) + name);
    }
}
