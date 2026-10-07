// Unit test of the pure driving feel (src/game/DriveFeel.h): analogue gas as pulses, the brake
// ramp limit, and the vibration made from the car.

#include "game/DriveFeel.h"

#include <algorithm>
#include <cmath>
#include <initializer_list>
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
    // Rough ground: rumbles while the game keeps saying so, stops soon after.
    Synth r;
    CarSample rough = fast;
    rough.gear = 4;
    r.step(rough);
    float roughLow = 0;
    for (int i = 0; i < 30; ++i)
    {
        if (i % 3 == 0)
            r.event(3, 255);
        roughLow = std::max(roughLow, r.step(rough).low);
    }
    CHECK(roughLow > 0.25f && roughLow < 0.8f);
    for (int i = 0; i < 15; ++i)
        m = r.step(rough);
    CHECK(m.low < 0.15f);

    if (g_failed)
        std::fprintf(stderr, "%d check(s) failed\n", g_failed);
    else
        std::printf("drive_feel_test: ok\n");
    return g_failed ? 1 : 0;
}
