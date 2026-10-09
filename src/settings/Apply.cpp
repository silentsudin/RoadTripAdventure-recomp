#include "Apply.h"
#include "ps2x_compat.h"

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
#include <cstdio>
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
            ps2x::setEnv("RT_GS_SSAA", std::to_string(current().superSampling).c_str());
#if defined(__APPLE__)
        // Full screen without a Space of its own: switching is instant, and the Metal layer just resizes.
        SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0");
#endif
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
            {
                SDL_SetWindowFullscreen(w, false); // SDL puts the window back where it was
                SDL_SyncWindow(w);
            }
            // The size is set only when the setting changes (or the window has never had it):
            // any other option applies the window too, and must not undo a drag of its edges.
            static int appliedW = 0, appliedH = 0;
            if (appliedW != s.windowWidth || appliedH != s.windowHeight)
            {
                appliedW = s.windowWidth;
                appliedH = s.windowHeight;
                int cw = 0, ch = 0;
                SDL_GetWindowSize(w, &cw, &ch);
                if (cw != s.windowWidth || ch != s.windowHeight)
                {
                    SDL_SetWindowSize(w, s.windowWidth, s.windowHeight);
                    SDL_SetWindowPosition(w, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                }
            }
            return;
        }
        // Borderless: the desktop's own mode (SDL's desktop fullscreen, no mode switch).
        // Fullscreen: a display mode of its own, the desktop's resolution at the desktop's
        // refresh rate (never another resolution).
        const SDL_DisplayMode *mode = nullptr;
        if (s.windowMode == WindowMode::Fullscreen)
        {
            const SDL_DisplayID display = SDL_GetDisplayForWindow(w);
            if (const SDL_DisplayMode *desktop = SDL_GetDesktopDisplayMode(display))
            {
                static SDL_DisplayMode closest;
                if (SDL_GetClosestFullscreenDisplayMode(display, desktop->w, desktop->h, desktop->refresh_rate, true, &closest))
                    mode = &closest;
            }
        }
        SDL_SetWindowFullscreenMode(w, mode);
        if (!fullscreen)
            SDL_SetWindowFullscreen(w, true);
        if (std::getenv("RT_WINDOW_DEBUG"))
        {
            SDL_SyncWindow(w);
            int x = 0, y = 0, ww = 0, hh = 0;
            SDL_GetWindowPosition(w, &x, &y);
            SDL_GetWindowSize(w, &ww, &hh);
            SDL_Rect b{};
            SDL_GetDisplayBounds(SDL_GetDisplayForWindow(w), &b);
            std::fprintf(stderr, "[window] mode %d pos %d,%d size %dx%d flags 0x%llx display %d,%d %dx%d\n", int(s.windowMode), x, y, ww, hh,
                         (unsigned long long)SDL_GetWindowFlags(w), b.x, b.y, b.w, b.h);
        }
    }

    void toggleFullscreen()
    {
#if !defined(__ANDROID__)
        if (!rt::host::window())
            return;
        static WindowMode lastFullscreen = WindowMode::Borderless;
        Settings &s = current();
        if (s.windowMode != WindowMode::Windowed)
        {
            lastFullscreen = s.windowMode;
            s.windowMode = WindowMode::Windowed;
        }
        else
            s.windowMode = lastFullscreen;
        saveCurrent();
        applyWindow();
#endif
    }

    void serviceWindow(bool menuOpen)
    {
#if !defined(__ANDROID__)
        SDL_Window *w = rt::host::window();
        if (!w)
            return;
        // Alt+Enter and F11 (the game does not use them).
        const bool alt = rt::host::keyHeld(SDL_SCANCODE_LALT) || rt::host::keyHeld(SDL_SCANCODE_RALT);
        if (rt::host::keyPressed(SDL_SCANCODE_F11) || (alt && rt::host::keyPressed(SDL_SCANCODE_RETURN)))
            toggleFullscreen();
        // The pointer is hidden in the fullscreen modes while playing.
        static int cursorShown = -1;
        const int want = (menuOpen || current().windowMode == WindowMode::Windowed) ? 1 : 0;
        if (want != cursorShown)
        {
            cursorShown = want;
            want ? SDL_ShowCursor() : SDL_HideCursor();
        }
#else
        (void)menuOpen;
#endif
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
        const std::filesystem::path textures = rt::paths::texturesDir();
        const char *dumpEnv = std::getenv("RT_TEXTURE_DUMP");
        const char *anisoEnv = std::getenv("RT_ANISOTROPY");
        const int aniso = anisoEnv ? std::atoi(anisoEnv) : s.anisotropy;
        g_gs->setAnisotropy(static_cast<uint32_t>(anisotropyAvailability(aniso).ok ? std::clamp(aniso, 1, 16) : 1));
        g_gs->setTextures(dumpEnv ? dumpEnv : s.dumpTextures ? (textures / "dumps").string() : std::string(), texturePackDir());
    }

    std::string texturePackDir()
    {
        const char *packEnv = g_packOverride ? g_packOverride->c_str() : std::getenv("RT_TEXTURE_PACK");
        if (packEnv)
            return packEnv;
        const Settings &s = current();
        return s.texturePack.empty() ? std::string() : (rt::paths::texturesDir() / "packs" / s.texturePack).string();
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
        // Frame skip: RT_FRAME_SKIP=0|1 overrides the setting; guest time (RT_TIME=virtual: tests)
        // never falls behind the host, and keeps every frame.
        const char *fs = std::getenv("RT_FRAME_SKIP");
        const char *time = std::getenv("RT_TIME");
        const bool virtualTime = time && std::strcmp(time, "virtual") == 0;
        rt->gs().setFrameSkip(fs ? std::strcmp(fs, "0") != 0 : s.frameSkip && !virtualTime);
        if (ps2x::HostPresenter *p = rt->presenter())
        {
            p->setDisplayAspect(aspect);
            capabilities().postProcess = p->supportsPostProcess();
            capabilities().upscalerPlugins = p->availableUpscalerPlugins();
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
                           : s.upscaler == Upscaler::ArmAsr          ? ps2x::HostPresenter::PostProcess::Scaling::ArmAsr
                           : s.upscaler == Upscaler::Fsr3            ? ps2x::HostPresenter::PostProcess::Scaling::Fsr3
                           : s.upscaler == Upscaler::Dlss            ? ps2x::HostPresenter::PostProcess::Scaling::Dlss
                           : s.upscaler == Upscaler::Xess            ? ps2x::HostPresenter::PostProcess::Scaling::Xess
                                                                    : ps2x::HostPresenter::PostProcess::Scaling::Bilinear;
            post.sharpness = s.sharpness;
            p->setPostProcess(post);
            ps2x::HostPresenter::FrameGeneration fg;
            if (refreshAvailability(capabilities(), s.refreshRate).ok && frameGenerationWith(capabilities(), s.upscaler))
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
