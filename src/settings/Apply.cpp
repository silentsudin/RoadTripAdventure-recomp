#include "Apply.h"

#include "Capabilities.h"
#include "Settings.h"
#include "platform/Host.h"

#include <SDL3/SDL.h>
#include "ps2_runtime.h"
#include "runtime/gs/gs_pgs_backend.h"

#include <cstdlib>
#include <string>

namespace rt::settings
{
    namespace
    {
        ps2x::gs::PgsControl *g_gs = nullptr;
        bool g_ssaaFromEnv = false; // RT_GS_SSAA was set by hand: keep it
    }

    void setGsControl(ps2x::gs::PgsControl *control)
    {
        g_gs = control;
        if (g_gs)
        {
            const ps2x::gs::PgsDeviceInfo info = g_gs->deviceInfo();
            setGpu(info.name, info.vendorId);
        }
    }

    ps2x::gs::PgsControl *gsControl() { return g_gs; }

    void exportGsEnvironment()
    {
        // The GS starts with the chosen supersampling (an explicit RT_GS_SSAA still wins).
        g_ssaaFromEnv = std::getenv("RT_GS_SSAA") != nullptr;
        if (!g_ssaaFromEnv)
            setenv("RT_GS_SSAA", std::to_string(current().superSampling).c_str(), 1);
    }

    void applyWindow()
    {
        SDL_Window *w = rt::host::window();
        if (!w)
            return;
        const Settings &s = current();
        capabilities().displayRefresh = rt::host::displayRefreshRate();
        const bool fullscreen = SDL_GetWindowFlags(w) & SDL_WINDOW_FULLSCREEN;
        if (s.windowMode == WindowMode::Windowed)
        {
            if (fullscreen)
                SDL_SetWindowFullscreen(w, false);
            int cw = 0, ch = 0;
            SDL_GetWindowSize(w, &cw, &ch);
            if (cw != s.windowWidth || ch != s.windowHeight)
                SDL_SetWindowSize(w, s.windowWidth, s.windowHeight);
            return;
        }
        // Borderless: the desktop's own mode (no mode switch). Fullscreen: exclusive, the desktop
        // resolution at the highest refresh rate it offers.
        const SDL_DisplayMode *mode = nullptr;
        if (s.windowMode == WindowMode::Fullscreen)
        {
            const SDL_DisplayID display = SDL_GetDisplayForWindow(w);
            if (const SDL_DisplayMode *desktop = SDL_GetDesktopDisplayMode(display))
            {
                static SDL_DisplayMode closest;
                if (SDL_GetClosestFullscreenDisplayMode(display, desktop->w, desktop->h, 0.0f, true, &closest))
                    mode = &closest;
            }
        }
        SDL_SetWindowFullscreenMode(w, mode);
        if (!fullscreen)
            SDL_SetWindowFullscreen(w, true);
    }

    void applyGraphics()
    {
        if (!g_gs)
            return;
        const Settings &s = current();
        if (!g_ssaaFromEnv)
            g_gs->setSuperSampling(static_cast<uint32_t>(s.superSampling));
        g_gs->setSharpTextures(s.sharpTextures);
    }

    void applyAspect()
    {
        // Widescreen: the GS narrows the HUD and tells 2D-backed screens apart, the game hook
        // widens the 3D camera, and the presenter shows the frame at this shape.
        PS2Runtime *rt = rt::host::runtime();
        if (!rt)
            return;
        const Settings &s = current();
        const float aspect = ratio(s.aspect);
        rt->gs().setWideLayout(aspect, s.hud == HudMode::Edges ? ps2x::gs::HudPlacement::Edges
                                                               : ps2x::gs::HudPlacement::Centred);
        if (ps2x::HostPresenter *p = rt->presenter())
        {
            p->setDisplayAspect(aspect);
            capabilities().postProcess = p->supportsPostProcess();
            ps2x::HostPresenter::PostProcess post;
            post.aa = s.aa == AntiAliasing::Fxaa   ? ps2x::HostPresenter::PostProcess::AntiAliasing::Fxaa
                      : s.aa == AntiAliasing::Smaa ? ps2x::HostPresenter::PostProcess::AntiAliasing::Smaa
                                                   : ps2x::HostPresenter::PostProcess::AntiAliasing::None;
            post.scaling = s.upscaler == Upscaler::Fsr1 ? ps2x::HostPresenter::PostProcess::Scaling::Fsr1
                                                        : ps2x::HostPresenter::PostProcess::Scaling::Bilinear;
            post.sharpness = s.sharpness;
            p->setPostProcess(post);
        }
    }

    void applyAll()
    {
        applyWindow();
        applyGraphics();
        applyAspect();
    }
}
