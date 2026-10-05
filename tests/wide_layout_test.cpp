// Widescreen 2D placement (ps2x::gs::WideLayout) on hand-made GIF packets, without the game.

#include "runtime/gs/gs_wide_layout.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace ps2x::gs;

namespace
{
    int g_failures = 0;
    void check(bool ok, const char *what, int line)
    {
        if (!ok)
        {
            std::fprintf(stderr, "FAIL line %d: %s\n", line, what);
            ++g_failures;
        }
    }
#define CHECK(x) check((x), #x, __LINE__)

    constexpr uint32_t kOfx = (2048 - 320) * 16, kOfy = (2048 - 112) * 16;
    constexpr uint32_t PATH1 = 0, PATH2 = 1;

    WideLayout::PrimState state()
    {
        WideLayout::PrimState s;
        s.ofx[0] = s.ofx[1] = kOfx;
        s.ofy[0] = s.ofy[1] = kOfy;
        return s;
    }

    // One PACKED GIF tag: PRE with `prim`, XYZ2 for each (x, y) screen point.
    std::vector<uint8_t> packet(uint64_t prim, const std::vector<std::pair<float, float>> &points)
    {
        std::vector<uint8_t> p(16 + points.size() * 16);
        const uint64_t lo = points.size() | (1ull << 15) | (1ull << 46) | (prim << 47) | (1ull << 60);
        const uint64_t hi = 0x5; // XYZ2
        std::memcpy(p.data(), &lo, 8);
        std::memcpy(p.data() + 8, &hi, 8);
        for (size_t i = 0; i < points.size(); ++i)
        {
            const uint64_t x = static_cast<uint64_t>(std::lround(points[i].first * 16 + kOfx));
            const uint64_t y = static_cast<uint64_t>(std::lround(points[i].second * 16 + kOfy));
            const uint64_t q = x | (y << 32);
            std::memcpy(p.data() + 16 + i * 16, &q, 8);
        }
        return p;
    }

    float xAt(const std::vector<uint8_t> &p, size_t i)
    {
        uint16_t x;
        std::memcpy(&x, p.data() + 16 + i * 16, 2);
        return (static_cast<float>(x) - kOfx) / 16.0f;
    }

    constexpr uint64_t kSprite = 6, kTexSprite = 6 | (1 << 4), kTriStrip = 4;
    bool near(float a, float b) { return std::fabs(a - b) < 0.1f; }

    void run(WideLayout &w, uint32_t path, std::vector<uint8_t> &p)
    {
        w.transformPacket(path, p.data(), static_cast<uint32_t>(p.size()), state());
    }

    void drivingFrame(WideLayout &w)
    {
        w.frameStart(); // the game's clear
        auto world = packet(kTriStrip, {{-200, 50}, {300, 60}, {900, 200}});
        run(w, PATH1, world);
        CHECK(near(xAt(world, 0), -200) && near(xAt(world, 2), 900)); // 3D untouched
    }

    void centred()
    {
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Centred); // k = 0.75
        CHECK(w.active());
        drivingFrame(w);
        auto time = packet(kTexSprite, {{20, 10}, {120, 38}});
        run(w, PATH2, time);
        CHECK(near(xAt(time, 0), 95) && near(xAt(time, 1), 170)); // 320 + (x - 320) * 0.75
        auto fade = packet(kSprite, {{0, -16}, {640, 240}});
        run(w, PATH2, fade);
        CHECK(near(xAt(fade, 0), 0) && near(xAt(fade, 1), 640)); // full-screen: stretches
        CHECK(!w.lastFrameWas2D());
    }

    void edges()
    {
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Edges);
        drivingFrame(w);
        auto left = packet(kTexSprite, {{20, 10}, {120, 38}});
        run(w, PATH2, left);
        CHECK(near(xAt(left, 0), 15) && near(xAt(left, 1), 90)); // x * 0.75
        auto right = packet(kTexSprite, {{560, 147}, {620, 196}});
        run(w, PATH2, right);
        CHECK(near(xAt(right, 0), 580) && near(xAt(right, 1), 625)); // 640 - (640 - x) * 0.75
        // The race minimap is drawn in 3D right after the speedometer: placed by its own position.
        auto minimap = packet(kTriStrip, {{49, 66}, {112, 70}, {80, 136}});
        run(w, PATH1, minimap);
        CHECK(near(xAt(minimap, 0), 36.75f) && near(xAt(minimap, 1), 84));
        // The town minimap is far wider than its (scissored) frame: it goes with the frame.
        auto frame = packet(kTexSprite, {{30, 152}, {174, 212}});
        run(w, PATH2, frame);
        auto town = packet(kTriStrip, {{-424, 48}, {576, 60}, {100, 424}});
        run(w, PATH1, town);
        CHECK(near(xAt(town, 0), -318) && near(xAt(town, 1), 432)); // left anchor: x * 0.75
        auto middle = packet(kTexSprite, {{232, 100}, {408, 140}});
        run(w, PATH2, middle);
        CHECK(near(xAt(middle, 0), 254) && near(xAt(middle, 1), 386)); // centred
    }

    void screen2D()
    {
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Edges);
        for (int f = 0; f < 10; ++f)
        {
            CHECK(!w.lastFrameWas2D()); // one 2D-looking frame is not enough
            w.frameStart();
            auto background = packet(kTexSprite, {{0, 0}, {640, 224}});
            run(w, PATH2, background); // first draw is 2D: a 2D-backed screen
            auto text = packet(kTexSprite, {{20, 10}, {120, 38}});
            run(w, PATH2, text);
            CHECK(near(xAt(text, 0), 20) && near(xAt(text, 1), 120)); // untouched: shown 4:3 instead
        }
        w.frameStart();
        CHECK(w.lastFrameWas2D());
    }

    void title()
    {
        // The title has no clear: a full-screen textured picture each frame. Shown 4:3.
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Edges);
        CHECK(!w.lastFrameWas2D());
        for (int f = 0; f < 10; ++f)
        {
            auto background = packet(kTexSprite, {{0, 0}, {640, 224}});
            run(w, PATH2, background);
            w.framePresented();
        }
        CHECK(w.lastFrameWas2D());
        for (int f = 0; f < 3; ++f) // a race starts: its clears make the frames driving ones
        {
            drivingFrame(w);
            w.framePresented();
        }
        CHECK(!w.lastFrameWas2D());
    }

    void off()
    {
        WideLayout w;
        w.configure(4.0f / 3.0f, HudPlacement::Edges);
        CHECK(!w.active());
        drivingFrame(w);
        auto time = packet(kTexSprite, {{20, 10}, {120, 38}});
        run(w, PATH2, time);
        CHECK(near(xAt(time, 0), 20) && near(xAt(time, 1), 120));
    }
}

int main()
{
    centred();
    edges();
    screen2D();
    title();
    off();
    if (g_failures)
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    else
        std::printf("wide layout tests passed\n");
    return g_failures ? 1 : 0;
}
