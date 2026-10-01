#include "FrameDump.h"

#include "raylib.h"
#include "rlgl.h"

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
        rlDrawRenderBatchActive(); // flush batched draws so the read sees this frame
        const int w = GetRenderWidth(), h = GetRenderHeight();
        unsigned char *pixels = rlReadScreenPixels(w, h);
        Image img{pixels, w, h, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
        char name[64];
        std::snprintf(name, sizeof(name), "/frame_%04d.png", index++);
        ExportImage(img, (std::string(dir) + name).c_str());
        RL_FREE(pixels);
    }
}
