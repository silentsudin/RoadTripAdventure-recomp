// Pausing in the background (see Lifecycle.h).

#include "platform/Lifecycle.h"

#include "runtime/ps2_audio_suspend.h"
#include "runtime/ps2_test_harness.h"

#include <SDL3/SDL_events.h>

#include <atomic>
#include <cstdio>
#include <mutex>

namespace rt::lifecycle
{
    namespace
    {
        std::mutex g_mutex;
        std::atomic<bool> g_away{false};
        bool g_pausedByUs = false;

        void goAway()
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (g_away.exchange(true))
                return;
            g_pausedByUs = !ps2_test::paused(); // the menu may have paused it already
            if (g_pausedByUs)
                ps2_test::setPaused(true);
            ps2AudioOutSuspend(true);
            std::fprintf(stderr, "[lifecycle] background: game paused, audio off\n");
        }

        void comeBack()
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (!g_away.exchange(false))
                return;
            ps2AudioOutSuspend(false);
            if (g_pausedByUs)
                ps2_test::setPaused(false);
            g_pausedByUs = false;
            std::fprintf(stderr, "[lifecycle] foreground: resumed\n");
        }

        bool watch(void *, SDL_Event *e)
        {
            switch (e->type)
            {
            case SDL_EVENT_WILL_ENTER_BACKGROUND:
            case SDL_EVENT_DID_ENTER_BACKGROUND:
            case SDL_EVENT_WINDOW_MINIMIZED:
                goAway();
                break;
            case SDL_EVENT_DID_ENTER_FOREGROUND:
            case SDL_EVENT_WINDOW_RESTORED:
                comeBack();
                break;
            default:
                break;
            }
            return true;
        }
    }

    void install()
    {
#if defined(__ANDROID__)
        static bool installed = false;
        if (!installed)
            installed = SDL_AddEventWatch(watch, nullptr);
#endif
    }

    bool away() { return g_away.load(); }
}
