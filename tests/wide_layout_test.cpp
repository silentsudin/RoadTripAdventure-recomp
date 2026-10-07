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

    WideLayout::PrimState state(uint32_t tbp = 0)
    {
        WideLayout::PrimState s;
        s.tbp[0] = s.tbp[1] = tbp;
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

    void run(WideLayout &w, uint32_t path, std::vector<uint8_t> &p, uint32_t tbp = 0)
    {
        w.transformPacket(path, p.data(), static_cast<uint32_t>(p.size()), state(tbp));
    }

    float centredX(float x) { return 320 + (x - 320) * 0.75f; }

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


    void messageWindow()
    {
        // A town conversation's window (Cloud Hill, Dust; RT_WIDE_DEBUG): one packet of sprites,
        // the frame in three tiles across the screen, the name plate on its top right, the text
        // in rows on it. With HUD at the edges, the window is one element: it stays whole and
        // centred (each tile by its own third tore it apart, and the text with it).
        for (HudPlacement placement : {HudPlacement::Edges, HudPlacement::Centred})
        {
            WideLayout w;
            w.configure(16.0f / 9.0f, placement);
            drivingFrame(w);
            const std::vector<std::pair<float, float>> window = {
                {32, 164}, {72, 192},   {32, 192}, {72, 216},   // left tiles
                {72, 164}, {568, 192},  {72, 192}, {568, 216},  // middle
                {568, 164}, {608, 192}, {568, 192}, {608, 216}, // right
                {304, 208}, {336, 220},                         // the "more" arrow
                {70, 173}, {85, 185},   {84, 173}, {99, 185},   // text, first row
                {216, 173}, {231, 185}, {371, 197}, {385, 209}, // ... further on
                {510, 158}, {518, 172}, {518, 158}, {574, 172}, {574, 158}, {598, 172}, // name plate
            };
            auto p = packet(kTexSprite, window);
            run(w, PATH2, p);
            bool ok = true;
            for (size_t i = 0; i < window.size(); ++i)
                ok = ok && near(xAt(p, i), centredX(window[i].first));
            CHECK(ok);
        }
    }

    void separateElements()
    {
        // Elements in one packet that don't touch keep their own anchors.
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Edges);
        drivingFrame(w);
        auto p = packet(kTexSprite, {{20, 10}, {120, 38}, {560, 147}, {620, 196}, {232, 100}, {408, 140}});
        run(w, PATH2, p);
        CHECK(near(xAt(p, 0), 15) && near(xAt(p, 1), 90));    // left
        CHECK(near(xAt(p, 2), 580) && near(xAt(p, 3), 625));  // right
        CHECK(near(xAt(p, 4), 254) && near(xAt(p, 5), 386));  // centre
        // Text touching a left-hand element goes with it, though it reaches the middle third.
        auto q = packet(kTexSprite, {{20, 10}, {120, 38}, {118, 12}, {260, 30}});
        run(w, PATH2, q);
        CHECK(near(xAt(q, 0), 15) && near(xAt(q, 1), 90) && near(xAt(q, 2), 88.5f) && near(xAt(q, 3), 195));
    }

    void newScreenAt4By3()
    {
        // The Takara logo (3D) gives way to the title: a full-screen picture is the first draw.
        // The title's first frame is already 2D-backed (it was shown stretched for 10 frames).
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Centred);
        for (int f = 0; f < 5; ++f)
        {
            drivingFrame(w);
            w.framePresented();
        }
        CHECK(!w.lastFrameWas2D());
        w.frameStart();
        auto title = packet(kTexSprite, {{0, 0}, {640, 224}});
        run(w, 2, title, 7566); // PATH3, a texture beyond the frame buffers
        CHECK(w.lastFrameWas2D());
        w.framePresented();
        CHECK(w.lastFrameWas2D());
        // A post-pass first (the frame buffer as its texture) is no such proof: it only votes.
        WideLayout v;
        v.configure(16.0f / 9.0f, HudPlacement::Centred);
        drivingFrame(v);
        v.frameStart();
        auto post = packet(kTexSprite, {{0, 0}, {639, 224}});
        run(v, PATH2, post, 0);
        CHECK(!v.lastFrameWas2D());
        // "Now loading": a full-screen black fill, then the text. One such frame keeps the
        // verdict (a stray 2D-first frame while driving); the second one is 2D-backed.
        WideLayout l;
        l.configure(16.0f / 9.0f, HudPlacement::Centred);
        drivingFrame(l);
        for (int f = 0; f < 2; ++f)
        {
            CHECK(!l.lastFrameWas2D());
            l.frameStart();
            auto fill = packet(kSprite, {{0, -16}, {640, 240}});
            run(l, PATH2, fill);
        }
        CHECK(l.lastFrameWas2D());
        // And back: the first 3D frame after a 2D screen is shown wide at once.
        w.frameStart();
        auto world = packet(kTriStrip, {{-200, 50}, {300, 60}, {900, 200}});
        run(w, PATH1, world);
        CHECK(!w.lastFrameWas2D());
    }

    void fadeKeepsVerdict()
    {
        // Going into a building or a load: frames start with a blended black fill over the
        // last picture (no clear), then "NOW LOADING". They show the picture before them, so
        // they keep its verdict: a town picture is not squeezed to 4:3 for the fade, and the
        // text on it keeps its shape (narrowed as HUD).
        constexpr uint64_t kBlendSprite = kSprite | (1 << 6);
        WideLayout w;
        w.configure(16.0f / 9.0f, HudPlacement::Edges);
        drivingFrame(w);
        for (int f = 0; f < 12; ++f)
        {
            w.frameStart();
            auto fade = packet(kBlendSprite, {{0, -16}, {640, 240}});
            run(w, PATH2, fade);
            CHECK(near(xAt(fade, 0), 0) && near(xAt(fade, 1), 640)); // full-screen: stretches
            auto text = packet(kTexSprite, {{400, 180}, {600, 200}});
            run(w, PATH2, text, 7566);
            CHECK(near(xAt(text, 0), 460) && near(xAt(text, 1), 610)); // right third: towards the right edge
            w.framePresented();
            CHECK(!w.lastFrameWas2D());
        }
        // The building's inside (a full-screen picture) is 2D-backed at once; a fade there
        // keeps it 4:3, and leaves its text alone.
        w.frameStart();
        auto inside = packet(kTexSprite, {{0, 0}, {640, 224}});
        run(w, PATH2, inside, 7566);
        CHECK(w.lastFrameWas2D());
        for (int f = 0; f < 12; ++f)
        {
            w.frameStart();
            auto fade = packet(kBlendSprite, {{0, -16}, {640, 240}});
            run(w, PATH2, fade);
            auto text = packet(kTexSprite, {{400, 180}, {600, 200}});
            run(w, PATH2, text, 7566);
            CHECK(near(xAt(text, 0), 400) && near(xAt(text, 1), 600));
            CHECK(w.lastFrameWas2D());
        }
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
    messageWindow();
    separateElements();
    newScreenAt4By3();
    fadeKeepsVerdict();
    off();
    if (g_failures)
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    else
        std::printf("wide layout tests passed\n");
    return g_failures ? 1 : 0;
}
