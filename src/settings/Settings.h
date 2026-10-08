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
    enum class FrameMode { Interpolate, Extrapolate, Rerender };
    enum class AntiAliasing { None, Fxaa, Smaa, Taa };
    enum class PerfOverlay { Off, Fps, Detailed };
    // Where the driving actions are (the game's own button setup, game/Driving.h): Modern (right
    // trigger gas, left trigger brake, ...; a keyboard-only player keeps Classic), Classic (the
    // game's own layout) or Custom (the player's, from the Driving controls page).
    enum class ControlScheme { Modern, Classic, Custom };
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
        int refreshRate = 60;          // 60, 120, 240: 1, 2 or 4 frames per game frame (generated)
        FrameMode frameMode = FrameMode::Rerender; // re-rendered frames: no added latency
        bool frameSkip = true;         // behind real time: skip drawing frames (Auto) rather than slow the game (Off)
        bool vsync = true;
        // Quality
        int superSampling = 8;         // samples per pixel, 1..16 (8 with progressive fields: 1280x896)
        bool sharpTextures = false;    // always sample the top texture level
        bool progressiveFields = true; // no half-line offset on alternate fields, no deinterlacing
        int anisotropy = 16;           // texture-pack images: 1 (trilinear), 2, 4, 8, 16
        AntiAliasing aa = AntiAliasing::None;
        Upscaler upscaler = Upscaler::None;
        float sharpness = 0.5f;
        // Textures
        std::string texturePack;       // "" = none; folder name under textures/packs
        bool dumpTextures = false;
        // General
        bool menuHintShown = false;
        bool modernHintShown = false; // the first drive with Modern controls showed where gas and brake are  // the first-launch "how to open the menu" hint was shown
        PerfOverlay perfOverlay = PerfOverlay::Off; // frame rate (and load, temperature) along the bottom
        bool secondScreen = true;     // a second display (the AYN Thor's lower screen) shows the map and stats
        // Sound and vibration (the game's own Options, which it never saved)
        float volume = 1.0f;          // master volume, 0..1
        bool mono = false;            // speaker: stereo / mono
        bool vibration = true;        // the game's vibration switch
        bool dynamicVibration = true; // on: rumble from the car (engine, road, brakes, knocks); off: the game's own
        // Controls
        ControlScheme controlScheme = ControlScheme::Modern;
        std::string customControls;   // Custom: the 9 actions' DS2 buttons ("r2 l2 square ..."), Driving.h order
        bool analogTriggers = true;   // gas and brake on analogue triggers respond to how far they are pressed
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
