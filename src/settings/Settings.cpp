#include "Settings.h"

#include "platform/Paths.h"

#include <toml.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace rt::settings
{
    namespace
    {
        constexpr std::array<const char *, 3> kWindowModes = {"windowed", "borderless", "fullscreen"};
        constexpr std::array<const char *, 5> kAspects = {"4:3", "16:10", "16:9", "21:9", "32:9"};
        constexpr std::array<const char *, 2> kHud = {"4:3", "edges"};
        constexpr std::array<const char *, 3> kFrameModes = {"interpolate", "extrapolate", "rerender"};
        constexpr std::array<const char *, 4> kAa = {"none", "fxaa", "smaa", "taa"};
        constexpr std::array<const char *, 3> kPerfOverlay = {"off", "fps", "detailed"};
        constexpr std::array<const char *, static_cast<size_t>(Upscaler::Count)> kUpscalers = {
            "none", "fsr1", "metalfx_spatial", "metalfx_temporal", "arm_asr", "sgsr1", "sgsr2", "arm_nss",
            "fsr3", "fsr4", "xess", "dlss"};

        template <typename E, size_t N>
        E pick(const toml::value &t, const char *key, const std::array<const char *, N> &names, E fallback)
        {
            if (!t.contains(key) || !t.at(key).is_string())
                return fallback;
            const std::string v = t.at(key).as_string();
            for (size_t i = 0; i < N; ++i)
                if (v == names[i])
                    return static_cast<E>(i);
            std::fprintf(stderr, "[settings] %s: unknown value '%s'\n", key, v.c_str());
            return fallback;
        }

        int integer(const toml::value &t, const char *key, int fallback)
        {
            return t.contains(key) && t.at(key).is_integer() ? static_cast<int>(t.at(key).as_integer()) : fallback;
        }

        float number(const toml::value &t, const char *key, float fallback)
        {
            if (!t.contains(key))
                return fallback;
            const toml::value &v = t.at(key);
            return v.is_floating() ? static_cast<float>(v.as_floating())
                   : v.is_integer() ? static_cast<float>(v.as_integer()) : fallback;
        }

        bool flag(const toml::value &t, const char *key, bool fallback)
        {
            return t.contains(key) && t.at(key).is_boolean() ? t.at(key).as_boolean() : fallback;
        }

        const toml::value &table(const toml::value &root, const char *key)
        {
            static const toml::value empty = toml::table{};
            return root.contains(key) && root.at(key).is_table() ? root.at(key) : empty;
        }

        int nearestOf(int v, std::initializer_list<int> allowed)
        {
            return *std::min_element(allowed.begin(), allowed.end(),
                                     [&](int a, int b) { return std::abs(a - v) < std::abs(b - v); });
        }
    }

    const char *name(Aspect a) { return kAspects[static_cast<size_t>(a)]; }
    const char *name(HudMode h) { return kHud[static_cast<size_t>(h)]; }
    const char *name(AntiAliasing a) { return kAa[static_cast<size_t>(a)]; }
    const char *name(Upscaler u) { return kUpscalers[static_cast<size_t>(u)]; }

    float ratio(Aspect a)
    {
        switch (a)
        {
        case Aspect::R16_10: return 16.0f / 10.0f;
        case Aspect::R16_9: return 16.0f / 9.0f;
        case Aspect::R21_9: return 64.0f / 27.0f; // 21:9 as sold (2.37)
        case Aspect::R32_9: return 32.0f / 9.0f;
        default: return 4.0f / 3.0f;
        }
    }

    Settings load(const std::filesystem::path &path)
    {
        Settings s;
        std::error_code ec;
        if (!std::filesystem::exists(path, ec))
            return s;
        toml::value root;
        try
        {
            root = toml::parse(path);
        }
        catch (const std::exception &e)
        {
            std::fprintf(stderr, "[settings] cannot read %s (using defaults): %s\n", path.string().c_str(), e.what());
            return s;
        }
        const toml::value &d = table(root, "display");
        s.windowMode = pick(d, "window_mode", kWindowModes, s.windowMode);
        s.windowWidth = std::max(320, integer(d, "width", s.windowWidth));
        s.windowHeight = std::max(224, integer(d, "height", s.windowHeight));
        s.aspect = pick(d, "aspect", kAspects, s.aspect);
        s.hud = pick(d, "hud", kHud, s.hud);
        s.refreshRate = nearestOf(integer(d, "refresh_rate", s.refreshRate), {60, 120, 240});
        s.frameMode = pick(d, "frame_mode", kFrameModes, s.frameMode);
        s.vsync = flag(d, "vsync", s.vsync);
        const toml::value &q = table(root, "quality");
        s.superSampling = nearestOf(integer(q, "supersampling", s.superSampling), {1, 2, 4, 8, 16});
        s.sharpTextures = flag(q, "sharp_textures", s.sharpTextures);
        s.progressiveFields = flag(q, "progressive_fields", s.progressiveFields);
        s.aa = pick(q, "anti_aliasing", kAa, s.aa);
        s.upscaler = pick(q, "upscaler", kUpscalers, s.upscaler);
        s.sharpness = std::clamp(number(q, "sharpness", s.sharpness), 0.0f, 1.0f);
        const toml::value &t = table(root, "textures");
        if (t.contains("pack") && t.at("pack").is_string())
            s.texturePack = t.at("pack").as_string();
        s.dumpTextures = flag(t, "dump", s.dumpTextures);
        s.anisotropy = nearestOf(integer(t, "pack_anisotropy", s.anisotropy), {1, 2, 4, 8, 16});
        s.menuHintShown = flag(table(root, "general"), "menu_hint_shown", s.menuHintShown);
        s.perfOverlay = pick(table(root, "general"), "performance_overlay", kPerfOverlay, s.perfOverlay);
        s.secondScreen = flag(table(root, "general"), "second_screen", s.secondScreen);
        return s;
    }

    void save(const std::filesystem::path &path, const Settings &s)
    {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        std::ostringstream o;
        o << "# Road Trip recomp options (in-game menu: Guide, Back+Start, Esc or F3).\n\n"
          << "[display]\n"
          << "window_mode = \"" << kWindowModes[static_cast<size_t>(s.windowMode)] << "\"  # windowed, borderless, fullscreen\n"
          << "width = " << s.windowWidth << "\nheight = " << s.windowHeight << "\n"
          << "aspect = \"" << name(s.aspect) << "\"  # 4:3, 16:10, 16:9, 21:9, 32:9\n"
          << "hud = \"" << name(s.hud) << "\"  # 4:3 (centred) or edges (anchored to the window edges); the HUD is never stretched\n"
          << "refresh_rate = " << s.refreshRate << "  # 60, 120, 240 (frames between the game's 60 are generated)\n"
          << "frame_mode = \"" << kFrameModes[static_cast<size_t>(s.frameMode)] << "\"  # rerender (re-rendered from the game's geometry, no added latency); interpolate/extrapolate warp the picture (RT_FRAME_GEN=1)\n"
          << "vsync = " << (s.vsync ? "true" : "false") << "\n\n"
          << "[quality]\n"
          << "supersampling = " << s.superSampling << "  # samples per pixel: 1, 2, 4, 8, 16 (4+ doubles the resolution)\n"
          << "sharp_textures = " << (s.sharpTextures ? "true" : "false") << "\n"
          << "progressive_fields = " << (s.progressiveFields ? "true" : "false") << "  # false = the original interlaced fields\n"
          << "anti_aliasing = \"" << name(s.aa) << "\"  # none, fxaa, smaa, taa\n"
          << "upscaler = \"" << name(s.upscaler) << "\"\n"
          << "sharpness = " << s.sharpness << "\n\n"
          << "[textures]\n"
          << "pack = \"" << s.texturePack << "\"  # a folder in textures/packs; empty = the original textures\n"
          << "pack_anisotropy = " << s.anisotropy << "  # filtering of pack images: 1 (trilinear), 2, 4, 8, 16\n"
          << "dump = " << (s.dumpTextures ? "true" : "false") << "  # for pack makers: save every texture to textures/dumps\n\n"
          << "[general]\n"
          << "menu_hint_shown = " << (s.menuHintShown ? "true" : "false") << "\n"
          << "performance_overlay = \"" << kPerfOverlay[static_cast<size_t>(s.perfOverlay)]
          << "\"  # off, fps, detailed (frame rate, load and temperature along the bottom)\n"
          << "second_screen = " << (s.secondScreen ? "true" : "false")
          << "  # a second display (the AYN Thor's lower screen) shows the map and your stats\n";
        std::ofstream(path) << o.str();
    }

    Settings &current()
    {
        static Settings s = load(paths::dataRoot() / "settings.toml");
        return s;
    }

    void saveCurrent() { save(paths::dataRoot() / "settings.toml", current()); }
}
