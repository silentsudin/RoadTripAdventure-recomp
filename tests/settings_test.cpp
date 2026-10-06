// settings.toml round trips and option gating per device, without the game.

#include "settings/Capabilities.h"
#include "settings/Settings.h"

#include <cstdio>
#include <filesystem>
#include <fstream>

using namespace rt::settings;

namespace
{
    int g_failures = 0;
    void check(bool ok, const char *what, int line)
    {
        if (!ok)
        {
            std::fprintf(stderr, "FAIL line %d: %s\n", line, what);
            ++g_failures;
        }
    }
#define CHECK(x) check((x), #x, __LINE__)

    void roundTrip()
    {
        const auto dir = std::filesystem::temp_directory_path() / "rt_settings_test";
        std::filesystem::remove_all(dir);
        const auto path = dir / "settings.toml";

        const Settings defaults = load(path); // missing file: defaults
        CHECK(defaults.superSampling == 8 && defaults.aspect == Aspect::R4_3 && defaults.refreshRate == 60);
        CHECK(defaults.hud == HudMode::FourThree && defaults.upscaler == Upscaler::None);

        Settings s;
        s.windowMode = WindowMode::Borderless;
        s.aspect = Aspect::R21_9;
        s.hud = HudMode::Edges;
        s.refreshRate = 240;
        s.superSampling = 16;
        s.sharpTextures = true;
        s.progressiveFields = false;
        s.upscaler = Upscaler::MetalFxTemporal;
        s.texturePack = "hd";
        s.anisotropy = 4;
        s.menuHintShown = true;
        save(path, s);
        const Settings b = load(path);
        CHECK(b.windowMode == WindowMode::Borderless && b.aspect == Aspect::R21_9 && b.hud == HudMode::Edges);
        CHECK(b.refreshRate == 240 && b.superSampling == 16 && b.sharpTextures && !b.progressiveFields);
        CHECK(b.upscaler == Upscaler::MetalFxTemporal && b.texturePack == "hd" && b.anisotropy == 4 && b.menuHintShown);

        // Hand-edited values snap to what exists; unknown names keep the default.
        std::ofstream(path) << "[display]\nrefresh_rate = 130\naspect = \"5:4\"\nhud = \"stretched\"\n[quality]\nsupersampling = 5\n";
        const Settings c = load(path);
        CHECK(c.refreshRate == 120 && c.aspect == Aspect::R4_3 && c.superSampling == 4);
        CHECK(c.hud == HudMode::FourThree); // there is no stretched HUD
        std::filesystem::remove_all(dir);
    }

    void gating()
    {
        Capabilities mac;
        mac.os = Os::MacOS;
        mac.gpuVendor = 0x106B;
        mac.displayRefresh = 120;
        CHECK(availability(mac, Upscaler::None).ok);
        CHECK(!availability(mac, Upscaler::Dlss).ok && availability(mac, Upscaler::Dlss).reason.find("NVIDIA") != std::string::npos);
        CHECK(availability(mac, Upscaler::Xess).reason.find("macOS") != std::string::npos);
        CHECK(availability(mac, Upscaler::SnapdragonGsr2).reason.find("Android") != std::string::npos);
        CHECK(refreshAvailability(mac, 60).ok);
        CHECK(!availability(mac, Upscaler::Fsr1).ok && !availability(mac, AntiAliasing::Fxaa).ok); // raylib presenter
        mac.postProcess = true;
        // No generated frames (the hardware GS): only 60.
        CHECK(!refreshAvailability(mac, 120).ok);
        mac.frameGeneration = true; // paraLLEl-GS
        mac.temporalInputs = true;
        CHECK(availability(mac, Upscaler::Fsr1).ok && availability(mac, AntiAliasing::Fxaa).ok); // Vulkan presenter
        CHECK(availability(mac, Upscaler::MetalFxSpatial).ok);
        CHECK(refreshAvailability(mac, 240).reason.find("120 Hz") != std::string::npos);
        CHECK(refreshAvailability(mac, 120).ok);
        CHECK(!refreshAvailability(mac, 90).ok);

        Capabilities thor;
        thor.os = Os::Android;
        thor.gpuVendor = 0x5143; // Qualcomm Adreno
        CHECK(availability(thor, Upscaler::MetalFxSpatial).reason.find("macOS") != std::string::npos);
        CHECK(availability(thor, Upscaler::ArmNss).reason.find("Mali") != std::string::npos);
        CHECK(availability(thor, Upscaler::Dlss).reason.find("NVIDIA") != std::string::npos);
        // The hardware GS: post-processing, but no motion vectors yet (no TAA).
        thor.postProcess = true;
        CHECK(availability(thor, AntiAliasing::Fxaa).ok);
        CHECK(availability(thor, AntiAliasing::Taa).reason.find("motion") != std::string::npos);

        Capabilities rtx;
        rtx.os = Os::Windows;
        rtx.gpuVendor = 0x10DE;
        // Possible on this machine, not built yet: greyed out as "later", not as impossible.
        CHECK(availability(rtx, Upscaler::Dlss).reason.find("later") != std::string::npos);
    }
}

int main()
{
    roundTrip();
    gating();
    if (g_failures)
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    else
        std::printf("settings tests passed\n");
    return g_failures ? 1 : 0;
}
