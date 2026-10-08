#pragma once

// The device's own vibrator (Android), for controllers that have no motors of their own: the
// AYN Thor's built-in pad ("Odin Controller") exposes no vibrator, so SDL's gamepad rumble goes
// nowhere, while the body has a system vibrator with amplitude control.
//
// The pad's two motors and the trigger motors are mixed into one amplitude; a change goes out
// as a VibrationEffect one-shot lasting what the caller asked for (renewed by the caller), and
// 0 cancels. Elsewhere (and when there is no vibrator) every call does nothing.
// RT_SYSTEM_VIBRATOR=0 turns it off; =1 also uses it for pads that do have motors (testing).

#include <cstdint>

namespace rt::input::systemvibrator
{
    // Whether a pad without motors should vibrate the device instead (Android with a vibrator).
    bool available();
    // Forced for every pad (RT_SYSTEM_VIBRATOR=1).
    bool forced();
    // The latest motor speeds (0..1) from a pad with none of its own; they last `ms`.
    void motors(float low, float high, uint32_t ms);
    void triggers(float left, float right, uint32_t ms);
    // Once a frame: sends a change held back by the rate limit.
    void tick();
    // Stops at once (background, shutdown).
    void stop();

    // The platform-independent parts (input_test checks them).
    namespace detail
    {
        // One vibrator amplitude, 0 (still) or 8..255 in steps of 8, from the motor speeds (0..1).
        int mixAmplitude(float low, float high, float left, float right);

        // Which levels go out (vibrator calls are binder calls): a start or a stop at once, a
        // rise of kBigStep or more 40 ms after the last send, a fall only once it has lasted
        // 150 ms and a small change once it has lasted 300 ms (so the dynamic mix flickering
        // between two levels sends nothing), and a renewal every 180 ms while the caller extends
        // its pulse (its 250 ms pulses, renewed every 100 ms, go out about every 200 ms). Each
        // one-shot lasts 50 ms past the caller's end. Times in ms.
        struct Limiter
        {
            static constexpr int kBigStep = 16;
            static constexpr int64_t kRiseGapMs = 40, kFallHoldMs = 150, kDriftHoldMs = 300;
            static constexpr int64_t kRenewEveryMs = 180, kRenewLeadMs = 20, kOverhangMs = 50;

            struct Decision
            {
                bool send = false;  // call the vibrator: amplitude 0 = cancel
                int amplitude = 0;
                int durationMs = 0;
            };
            // amp: the level wanted now (mixAmplitude); until: when the caller's pulse ends.
            Decision decide(int amp, int64_t now, int64_t until);

            int sentAmp = 0; // what the vibrator plays (0 = still)
            int64_t sentAt = 0, sentUntil = 0, differentSince = -1;
        };
    }
}
