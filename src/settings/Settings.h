#pragma once

// The recomp's own options (settings.toml in the data directory): display, image quality,
// upscaling, textures. Edited in the in-game menu (src/ui/PauseMenu). Environment
// variables still win for tests and debugging (RT_GS_SSAA, RT_GS_PROGRESSIVE, ...).

#include <filesystem>
#include <string>

namespace rt::settings
{
    enum class WindowMode { Windowed, Borderless, Fullscreen };
    enum class Aspect { R4_3, R16_10, R16_9, R21_9, R32_9 };
    // With a wide aspect only the 3D world widens: the game's 2D screens stay 4:3 and its HUD is
    // either kept in the centred 4:3 area or moved out to the window edges, never stretched.
    enum class HudMode { FourThree, Edges };
    enum class FrameMode { Interpolate, Extrapolate };
    enum class AntiAliasing { None, Fxaa, Smaa, Taa };
    enum class Upscaler
    {
        None, Fsr1, MetalFxSpatial, MetalFxTemporal, ArmAsr, SnapdragonGsr1, SnapdragonGsr2, ArmNss,
        Fsr3, Fsr4, Xess, Dlss, Count
    };

    struct Settings
    {
        // Display
        WindowMode windowMode = WindowMode::Windowed;
        int windowWidth = 1280, windowHeight = 896;
        Aspect aspect = Aspect::R4_3;
        HudMode hud = HudMode::FourThree;
        int refreshRate = 60;          // 60, 72, 90, 100, 120, 144
        FrameMode frameMode = FrameMode::Interpolate;
        bool vsync = true;
        // Quality
        int superSampling = 4;         // samples per pixel, 1..16; 4+ doubles the scanout resolution
        bool sharpTextures = false;    // always sample the top texture level
        int anisotropy = 1;            // 1 (off), 2, 4, 8, 16
        AntiAliasing aa = AntiAliasing::None;
        Upscaler upscaler = Upscaler::None;
        float sharpness = 0.5f;
        // Textures
        std::string texturePack;       // "" = none; folder name under textures/packs
        bool dumpTextures = false;
        // General
        bool menuHintShown = false;  // the first-launch "how to open the menu" hint was shown
    };

    const char *name(Aspect a);
    const char *name(HudMode h);
    const char *name(AntiAliasing a);
    const char *name(Upscaler u);
    float ratio(Aspect a);

    Settings load(const std::filesystem::path &path);
    void save(const std::filesystem::path &path, const Settings &s);
    // The settings in use (loaded at startup from the data directory).
    Settings &current();
    void saveCurrent();
}
