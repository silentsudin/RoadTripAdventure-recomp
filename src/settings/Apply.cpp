#include "Apply.h"

#include "Capabilities.h"
#include "Settings.h"
#include "raylib.h"
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
        if (!IsWindowReady())
            return;
        const Settings &s = current();
        capabilities().displayRefresh = GetMonitorRefreshRate(GetCurrentMonitor());
        const bool fullscreen = IsWindowFullscreen();
        const bool borderless = IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
        if (fullscreen && s.windowMode != WindowMode::Fullscreen)
            ToggleFullscreen();
        if (borderless && s.windowMode != WindowMode::Borderless)
            ToggleBorderlessWindowed();
        if (s.windowMode == WindowMode::Borderless && !IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE))
            ToggleBorderlessWindowed();
        else if (s.windowMode == WindowMode::Fullscreen && !IsWindowFullscreen())
            ToggleFullscreen();
        else if (s.windowMode == WindowMode::Windowed &&
                 (GetScreenWidth() != s.windowWidth || GetScreenHeight() != s.windowHeight))
            SetWindowSize(s.windowWidth, s.windowHeight);
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

    void applyAll()
    {
        applyWindow();
        applyGraphics();
    }
}
