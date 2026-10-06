#include "Apply.h"

#include "Capabilities.h"
#include "Settings.h"
#include "platform/Host.h"
#include "platform/Paths.h"
#include "platform/SecondDisplay.h"

#include <SDL3/SDL.h>
#include "ps2_runtime.h"
#include "runtime/gs/gs_pgs_backend.h"
#include "runtime/ps2_audio_suspend.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <cstring>
#include <string>

namespace rt::settings
{
    namespace
    {
        std::optional<std::string> g_packOverride; // the test socket's texture_pack command
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
            capabilities().maxSuperSampling = static_cast<int>(info.maxSuperSampling);
            capabilities().gpuGs = true;
        }
    }

    ps2x::gs::PgsControl *gsControl() { return g_gs; }

    void overrideTexturePack(const std::string &dir)
    {
        g_packOverride = dir;
        applyGraphics();
    }

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
#if defined(__ANDROID__)
        // Always the whole screen (immersive: no status or navigation bar).
        SDL_SetWindowFullscreen(w, true);
        return;
#endif
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
        // Texture dumps go to textures/dumps; packs are folders under textures/packs.
        // RT_TEXTURE_DUMP, RT_TEXTURE_PACK (directories) and RT_ANISOTROPY override the settings.
        const std::filesystem::path textures = rt::paths::dataRoot() / "textures";
        const char *dumpEnv = std::getenv("RT_TEXTURE_DUMP");
        const char *packEnv = g_packOverride ? g_packOverride->c_str() : std::getenv("RT_TEXTURE_PACK");
        const char *anisoEnv = std::getenv("RT_ANISOTROPY");
        const int aniso = anisoEnv ? std::atoi(anisoEnv) : s.anisotropy;
        g_gs->setAnisotropy(static_cast<uint32_t>(anisotropyAvailability(aniso).ok ? std::clamp(aniso, 1, 16) : 1));
        g_gs->setTextures(dumpEnv ? dumpEnv : s.dumpTextures ? (textures / "dumps").string() : std::string(),
                          packEnv ? packEnv : s.texturePack.empty() ? std::string() : (textures / "packs" / s.texturePack).string());
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
        // Progressive fields: the game hook drops the half-line field offset, the scanout stops
        // deinterlacing. RT_PROGRESSIVE_FIELDS=0|1 overrides the setting (the regression suite
        // keeps the original fields).
        const char *pf = std::getenv("RT_PROGRESSIVE_FIELDS");
        rt->gs().setProgressiveFields(pf ? std::strcmp(pf, "0") != 0 : s.progressiveFields);
        if (ps2x::HostPresenter *p = rt->presenter())
        {
            p->setDisplayAspect(aspect);
            capabilities().postProcess = p->supportsPostProcess();
            ps2x::HostPresenter::PostProcess post;
            post.aa = s.aa == AntiAliasing::Fxaa   ? ps2x::HostPresenter::PostProcess::AntiAliasing::Fxaa
                      : s.aa == AntiAliasing::Smaa ? ps2x::HostPresenter::PostProcess::AntiAliasing::Smaa
                      : s.aa == AntiAliasing::Taa  ? ps2x::HostPresenter::PostProcess::AntiAliasing::Taa
                                                   : ps2x::HostPresenter::PostProcess::AntiAliasing::None;
            post.scaling = s.upscaler == Upscaler::Fsr1             ? ps2x::HostPresenter::PostProcess::Scaling::Fsr1
                           : s.upscaler == Upscaler::MetalFxSpatial ? ps2x::HostPresenter::PostProcess::Scaling::MetalFxSpatial
                           : s.upscaler == Upscaler::MetalFxTemporal ? ps2x::HostPresenter::PostProcess::Scaling::MetalFxTemporal
                           : s.upscaler == Upscaler::SnapdragonGsr1  ? ps2x::HostPresenter::PostProcess::Scaling::SnapdragonGsr1
                           : s.upscaler == Upscaler::SnapdragonGsr2  ? ps2x::HostPresenter::PostProcess::Scaling::SnapdragonGsr2
                                                                    : ps2x::HostPresenter::PostProcess::Scaling::Bilinear;
            post.sharpness = s.sharpness;
            p->setPostProcess(post);
            ps2x::HostPresenter::FrameGeneration fg;
            if (refreshAvailability(capabilities(), s.refreshRate).ok)
                fg.factor = static_cast<uint32_t>(s.refreshRate / 60);
            const char *warp = std::getenv("RT_FRAME_GEN");
            fg.rerender = s.frameMode == FrameMode::Rerender || !(warp && *warp == '1');
            fg.extrapolate = s.frameMode == FrameMode::Extrapolate;
            p->setFrameGeneration(fg);
        }
    }

    void applyAll()
    {
        applyWindow();
        applyGraphics();
        applyAspect();
        rt::seconddisplay::setEnabled(current().secondScreen);
        ps2AudioOutSetMix(current().volume, current().mono);
    }
}
