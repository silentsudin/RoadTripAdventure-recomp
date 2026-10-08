// The device's vibrator for pads without motors (see SystemVibrator.h).

#include "SystemVibrator.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#if defined(__ANDROID__)
#include <SDL3/SDL_system.h>
#include <jni.h>
#endif

namespace rt::input::systemvibrator
{
    namespace detail
    {
        int mixAmplitude(float low, float high, float left, float right)
        {
            // The large motor is the body of the feel (engine, the ground); the small one's buzz
            // and the trigger motors add to it a little. A linear actuator feels weak at low
            // amplitudes, hence the curve.
            low = std::clamp(low, 0.0f, 1.0f);
            high = std::clamp(high, 0.0f, 1.0f) * 0.75f;
            const float tr = std::clamp(std::max(left, right), 0.0f, 1.0f) * 0.6f;
            const float top = std::max({low, high, tr});
            const float x = std::clamp(top + 0.25f * (low + high + tr - top), 0.0f, 1.0f);
            if (x < 0.02f)
                return 0;
            const int a = static_cast<int>(std::lround(std::pow(x, 0.6f) * 255.0f));
            return std::clamp(a & ~7, 8, 255); // 32 steps, so a slowly changing level isn't resent
        }

        Limiter::Decision Limiter::decide(int amp, int64_t now, int64_t until)
        {
            Decision d;
            if (amp <= 0)
            {
                if (sentAmp != 0)
                    d.send = true; // stop at once
                sentAmp = 0;
                differentSince = -1;
                return d;
            }
            if (until <= now)
                return d;
            // How long the level has differed from what the vibrator plays (a level that comes
            // back before it counts, as the dynamic mix's flicker between two levels does, sends
            // nothing).
            if (amp == sentAmp)
                differentSince = -1;
            else if (differentSince < 0)
                differentSince = now;
            const int64_t differing = amp == sentAmp ? 0 : now - differentSince;
            bool go = false;
            if (sentAmp == 0)
                go = true; // a start at once
            else if (amp >= sentAmp + kBigStep)
                go = now - sentAt >= kRiseGapMs; // a knock: soon
            else if (amp <= sentAmp - kBigStep)
                go = differing >= kFallHoldMs; // a fall must last
            else if (amp != sentAmp)
                go = differing >= kDriftHoldMs; // a small change, slower still
            // Renewed every 180 ms while the caller keeps extending its pulse, or whenever the
            // last one-shot is about to end (each lasts 50 ms past the caller's end, so a renewal
            // always comes before it runs out, and a stalled caller still stops soon).
            if (!go && until > sentUntil - kOverhangMs &&
                (now - sentAt >= kRenewEveryMs || sentUntil - now < kRenewLeadMs))
                go = true;
            if (!go)
                return d;
            d.send = true;
            d.amplitude = amp;
            d.durationMs = static_cast<int>(until + kOverhangMs - now);
            sentAmp = amp;
            sentAt = now;
            sentUntil = until + kOverhangMs;
            differentSince = -1;
            return d;
        }
    }

#if defined(__ANDROID__)
    namespace
    {
        using Clock = std::chrono::steady_clock;

        std::mutex g_mutex;
        int g_mode = -1; // RT_SYSTEM_VIBRATOR: -1 unset, 0 off, 1 pads without motors, 2 every pad
        int g_available = -1;
        float g_low = 0, g_high = 0, g_left = 0, g_right = 0;
        int64_t g_motorsUntil = 0, g_triggersUntil = 0; // ms on the steady clock
        detail::Limiter g_limiter;
        bool g_trace = false;

        int64_t nowMs()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
        }

        int mode()
        {
            if (g_mode == -1)
            {
                const char *v = std::getenv("RT_SYSTEM_VIBRATOR");
                g_mode = v && *v == '0' ? 0 : v && *v == '1' ? 2 : 1;
                const char *t = std::getenv("RT_RUMBLE_TRACE");
                g_trace = t && *t == '1';
            }
            return g_mode;
        }

        // Calls RoadTripActivity.<name>(args); returns its int result (or 0).
        int callActivity(const char *name, const char *sig, int a = 0, int b = 0)
        {
            auto *env = static_cast<JNIEnv *>(SDL_GetAndroidJNIEnv());
            auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
            if (!env || !activity)
                return 0;
            int result = 0;
            jclass cls = env->GetObjectClass(activity);
            if (jmethodID m = env->GetMethodID(cls, name, sig))
            {
                if (sig[1] == ')') // "()I"
                    result = env->CallIntMethod(activity, m);
                else
                    env->CallVoidMethod(activity, m, static_cast<jint>(a), static_cast<jint>(b));
            }
            if (env->ExceptionCheck())
                env->ExceptionClear();
            env->DeleteLocalRef(cls);
            env->DeleteLocalRef(activity);
            return result;
        }

        // Sends the current mix when the limiter says so.
        void update()
        {
            const int64_t now = nowMs();
            const bool motors = now < g_motorsUntil, trig = now < g_triggersUntil;
            const int amp = detail::mixAmplitude(motors ? g_low : 0, motors ? g_high : 0, trig ? g_left : 0,
                                                 trig ? g_right : 0);
            const int64_t until = std::max({now, motors ? g_motorsUntil : now, trig ? g_triggersUntil : now});
            const detail::Limiter::Decision d = g_limiter.decide(amp, now, until);
            if (!d.send)
                return;
            callActivity("systemVibrate", "(II)V", d.amplitude, d.durationMs);
            if (g_trace)
                std::fprintf(stderr, "[rumble] system vibrator %d for %d ms\n", d.amplitude, d.durationMs);
        }
    }

    bool available()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (mode() == 0)
            return false;
        if (g_available < 0)
        {
            g_available = callActivity("systemVibratorKind", "()I") > 0 ? 1 : 0;
            std::fprintf(stderr, "[input] system vibrator: %s\n", g_available ? "yes" : "none");
        }
        return g_available == 1;
    }

    bool forced() { return mode() == 2; }

    void motors(float low, float high, uint32_t ms)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_low = std::clamp(low, 0.0f, 1.0f);
        g_high = std::clamp(high, 0.0f, 1.0f);
        g_motorsUntil = nowMs() + ms;
        update();
    }

    void triggers(float left, float right, uint32_t ms)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_left = std::clamp(left, 0.0f, 1.0f);
        g_right = std::clamp(right, 0.0f, 1.0f);
        g_triggersUntil = nowMs() + ms;
        update();
    }

    void tick()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const int64_t now = nowMs();
        if (g_available == 1 && (g_limiter.sentAmp != 0 || g_motorsUntil > now || g_triggersUntil > now))
            update();
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_motorsUntil = g_triggersUntil = 0;
        if (g_available == 1 && g_limiter.sentAmp != 0)
            callActivity("systemVibrate", "(II)V", 0, 0);
        g_limiter = {};
    }
#else
    bool available() { return false; }
    bool forced() { return false; }
    void motors(float, float, uint32_t) {}
    void triggers(float, float, uint32_t) {}
    void tick() {}
    void stop() {}
#endif
}
