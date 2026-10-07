// Unit test of the pure driving feel (src/game/DriveFeel.h): analogue gas as pulses, the brake
// ramp limit, and the vibration made from the car.

#include "game/DriveFeel.h"

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <utility>
#include <cstdio>

using namespace rt::game::feel;

static int g_failed = 0;
#define CHECK(c)                                                                 \
    do                                                                           \
    {                                                                            \
        if (!(c))                                                                \
        {                                                                        \
            std::fprintf(stderr, "%s:%d: CHECK(%s) failed\n", __FILE__, __LINE__, #c); \
            ++g_failed;                                                          \
        }                                                                        \
    } while (0)

int main()
{
    // Pulses: the duty is met over any window, and spread out (no run longer than needed).
    for (float duty : {0.0f, 0.1f, 0.25f, 0.5f, 0.7f, 0.9f, 1.0f})
    {
        Pwm pwm;
        int on = 0, run = 0, longestOff = 0;
        for (int i = 0; i < 600; ++i)
        {
            const bool b = pwm.step(duty);
            on += b;
            run = b ? 0 : run + 1;
            longestOff = std::max(longestOff, run);
        }
        CHECK(std::fabs(on / 600.0f - duty) < 0.01f);
        if (duty > 0)
            CHECK(longestOff <= static_cast<int>(std::ceil(1.0f / duty)));
    }
    // The throttle holds the speed asked for.
    {
        Throttle th;
        CHECK(th.step(0.5f, 5000));            // below half of 20000: pulses (first one on)
        int on = 0;
        for (int i = 0; i < 60; ++i)
            on += th.step(0.5f, 11000);
        CHECK(on == 0);                        // above: off
        th.full(24000);
        CHECK(th.top == 24000);
        on = 0;
        for (int i = 0; i < 60; ++i)
            on += th.step(0.5f, 11000);        // now below half of 24000
        CHECK(on >= 40);
    }
    CHECK(brakeRampLimit(0.0f) == 1);
    CHECK(brakeRampLimit(0.5f) == 16);
    CHECK(brakeRampLimit(1.0f) == 32);

    // Vibration: still and idle = quiet; flat out = engine and road; braking = left trigger.
    Synth s;
    CarSample idle;
    Motors m = s.step(idle);
    CHECK(m.low == 0 && m.high == 0 && m.left == 0 && m.right == 0);
    s.reset();
    CarSample fast;
    fast.rpm = 8000;
    fast.speed = 18000;
    fast.gear = 4;
    fast.gas = true;
    float low = 0, high = 0, right = 0;
    for (int i = 0; i < 60; ++i)
    {
        m = s.step(fast);
        low = std::max(low, m.low), high = std::max(high, m.high), right = std::max(right, m.right);
    }
    CHECK(low > 0.02f && low < 0.3f);
    CHECK(high > 0.05f && high < 0.4f);
    CHECK(right > 0.1f && m.left == 0);
    // A gear change thumps once and fades.
    fast.gear = 5;
    m = s.step(fast);
    CHECK(m.low >= 0.4f);
    for (int i = 0; i < 10; ++i)
        m = s.step(fast);
    CHECK(m.low < 0.3f);
    // Braking hard at speed.
    CarSample brake = fast;
    brake.gas = false;
    brake.brake = true;
    brake.brakeRamp = 32;
    float left = 0;
    for (int i = 0; i < 12; ++i)
        left = std::max(left, s.step(brake).left);
    CHECK(left > 0.3f);
    // A knock from the game: strong at once, gone within half a second.
    s.event(0, 255);
    m = s.step(idle);
    CHECK(m.low > 0.7f && m.high > 0.5f);
    for (int i = 0; i < 30; ++i)
        m = s.step(idle);
    CHECK(m.low < 0.01f);
    // A soft knock is softer; a collision by its strength.
    Synth t;
    t.event(0, 40);
    CHECK(t.step(idle).low < 0.6f);
    Synth c;
    c.event(6, 125);
    m = c.step(idle);
    CHECK(m.low > 0.5f && m.low < 0.8f);
    // Bumps are graded (the game's 11..41).
    Synth b1, b2;
    b1.event(1, 11);
    b2.event(1, 41);
    CHECK(b1.step(idle).low < b2.step(idle).low);
    // Wall scrapes (the game's 4 left, 3 right, 5 front): the trigger on that side, while the
    // game keeps saying so, gone soon after.
    for (int pattern : {4, 3, 5})
    {
        Synth w;
        CarSample cruise = fast;
        cruise.gear = 4;
        w.step(cruise);
        Motors peak;
        for (int i = 0; i < 30; ++i)
        {
            if (i % 3 == 0)
                w.event(pattern, 255);
            const Motors x = w.step(cruise);
            peak.left = std::max(peak.left, x.left), peak.right = std::max(peak.right, x.right);
            peak.high = std::max(peak.high, x.high);
        }
        CHECK(peak.high > 0.2f);
        if (pattern == 4)
            CHECK(peak.left > peak.right + 0.1f);
        else if (pattern == 3)
            CHECK(peak.right > peak.left + 0.1f);
        else
            CHECK(peak.left > 0.15f && peak.right > 0.15f);
        for (int i = 0; i < 15; ++i)
            m = w.step(cruise);
        CHECK(m.left < 0.3f && m.high < 0.3f); // the engine and the gas trigger stay
    }

    // Surfaces: the contact words seen on the courses, named by their material bits.
    CHECK(surfaceOf(0x2000) == Surface::Asphalt);   // Peach Raceway's road
    CHECK(surfaceOf(0x2353) == Surface::Grass);     // and its grass
    CHECK(surfaceOf(0x111) == Surface::Dirt);       // Temple Raceway's dirt
    CHECK(surfaceOf(0x2111) == Surface::Dirt);      // Lagoon Raceway's sand
    CHECK(surfaceOf(0x3444) == Surface::Snow);      // Snow Mountain
    CHECK(surfaceOf(0x12554) == Surface::Ice);      // its frozen lake
    CHECK(surfaceOf(0x500) == Surface::Boards);     // Temple's bridge, Night Glow's brick
    CHECK(surfaceOf(0x102651) == Surface::Water);   // Lagoon's sea
    CHECK(surfaceOf(0x100651) == Surface::Water);   // wading (the game's own word)
    CHECK(surfaceOf(0xFFFFFFFFu) == Surface::Other);
    // Each surface feels different at the same speed (mean motors over a second).
    {
        CarSample roll;
        roll.rpm = 5000, roll.speed = 12000, roll.gear = 3;
        float lows[8] = {}, highs[8] = {};
        const uint32_t words[] = {0x2000, 0x111, 0x2353, 0x3444, 0x12554, 0x500, 0x102651};
        for (int k = 0; k < 7; ++k)
        {
            Synth g;
            roll.ground = words[k];
            for (int i = 0; i < 60; ++i)
            {
                const Motors x = g.step(roll);
                lows[k] += x.low / 60, highs[k] += x.high / 60;
            }
        }
        CHECK(lows[6] > lows[1] && lows[1] > lows[0]); // water > dirt > asphalt
        CHECK(lows[4] < lows[0] + 0.02f);              // ice: hardly anything below
        CHECK(highs[1] > highs[2]);                    // dirt grittier than grass
        CHECK(lows[2] > lows[0]);                      // grass rougher than asphalt
    }

    // In the air: no ground; landing thumps by how fast the car came down.
    {
        auto land = [&](int frames, int perFrame) {
            Synth j;
            CarSample c = fast;
            c.ground = 0x2000;
            for (int i = 0; i < 10; ++i)
                j.step(c);
            CarSample a = c;
            Motors inAir;
            for (int i = 1; i <= frames; ++i)
            {
                a.fall[0] = a.fall[1] = a.fall[2] = i * perFrame;
                inAir = j.step(a);
            }
            const Motors hit = j.step(c);
            return std::make_pair(inAir, hit);
        };
        const auto [air, hop] = land(6, 97);
        const auto [air2, jump] = land(50, 97);
        CHECK(air.low < 0.05f);                 // no road rumble in the air
        CHECK(hop.low >= 0.3f && hop.low < 0.6f);
        CHECK(jump.low > 0.9f && jump.left > 0.4f);
        CHECK(landingStrength(600) <= 0.3f && landingStrength(8000) == 1.0f);
        // One point off the ground (a crest) is not a jump.
        Synth k;
        CarSample crest = fast;
        crest.ground = 0x2000;
        Motors last;
        for (int i = 1; i <= 12; ++i)
        {
            crest.fall[0] = i * 97;
            last = k.step(crest);
        }
        crest.fall[0] = 0;
        CHECK(k.step(crest).low < 0.2f);
    }

    if (g_failed)
        std::fprintf(stderr, "%d check(s) failed\n", g_failed);
    else
        std::printf("drive_feel_test: ok\n");
    return g_failed ? 1 : 0;
}
