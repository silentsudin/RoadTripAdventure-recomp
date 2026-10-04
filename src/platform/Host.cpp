#include "platform/Host.h"

#include "ps2_runtime.h"
#include "raylib.h"

#include <SDL3/SDL.h>

#include <array>

namespace rt::host
{
    namespace
    {
        PS2Runtime *g_runtime = nullptr;
        std::array<bool, SDL_SCANCODE_COUNT> g_down{}, g_was{};
        std::array<double, SDL_SCANCODE_COUNT> g_nextRepeat{};
        std::array<bool, SDL_SCANCODE_COUNT> g_repeat{};
    }

    void setRuntime(PS2Runtime *runtime) { g_runtime = runtime; }

    void beginFrame()
    {
        g_was = g_down;
        int count = 0;
        const bool *keys = SDL_GetKeyboardState(&count);
        const double t = now();
        for (int k = 0; k < SDL_SCANCODE_COUNT; ++k)
        {
            g_down[k] = keys && k < count && keys[k];
            // Repeats: 400 ms after the press, then every 50 ms while held.
            g_repeat[k] = false;
            if (g_down[k] && !g_was[k])
                g_nextRepeat[k] = t + 0.4;
            else if (g_down[k] && t >= g_nextRepeat[k])
            {
                g_repeat[k] = true;
                g_nextRepeat[k] = t + 0.05;
            }
        }
    }

    double now() { return static_cast<double>(SDL_GetTicksNS()) * 1e-9; }

    bool keyPressed(int k) { return k > 0 && k < SDL_SCANCODE_COUNT && g_down[k] && !g_was[k]; }

    bool keyPressedRepeat(int k) { return k > 0 && k < SDL_SCANCODE_COUNT && g_repeat[k]; }

    SDL_Window *window()
    {
        if (g_runtime && g_runtime->presenter())
            if (void *w = g_runtime->presenter()->sdlWindow())
                return static_cast<SDL_Window *>(w);
        // raylib on its SDL platform: the window handle is the SDL_Window.
        return IsWindowReady() ? static_cast<SDL_Window *>(GetWindowHandle()) : nullptr;
    }

    bool windowFocused()
    {
        SDL_Window *w = window();
        return w && (SDL_GetWindowFlags(w) & SDL_WINDOW_INPUT_FOCUS);
    }

    void windowSize(int &width, int &height)
    {
        width = height = 0;
        if (SDL_Window *w = window())
            SDL_GetWindowSize(w, &width, &height);
    }

    void displayUsableSize(int &width, int &height)
    {
        width = 1920;
        height = 1080;
        SDL_Window *w = window();
        SDL_Rect r;
        if (w && SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(w), &r))
        {
            width = r.w;
            height = r.h;
        }
    }

    int displayRefreshRate()
    {
        SDL_Window *w = window();
        const SDL_DisplayMode *mode = w ? SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(w)) : nullptr;
        return mode ? static_cast<int>(mode->refresh_rate + 0.5f) : 0;
    }

    bool screenshot(const std::string &pngPath)
    {
        return g_runtime && g_runtime->presenter() && g_runtime->presenter()->captureWindow(pngPath);
    }

    const char *presenterName()
    {
        return g_runtime && g_runtime->presenter() ? g_runtime->presenter()->name() : "none";
    }
}
