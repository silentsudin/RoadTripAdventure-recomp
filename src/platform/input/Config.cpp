#include "Config.h"

#include <toml.hpp>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace rt::input
{
    namespace
    {
        // Keyboard defaults (SDL scancode names), as the version 1 defaults.
        constexpr const char *kKeyDefaults[kPs2ButtonCount] = {
            "Up", "Down", "Left", "Right", "X Space", "C", "Z", "V", "Q", "E", "1", "3",
            "Return", "Tab Backspace", "F", "G",
        };
        constexpr const char *kKeyStickDefaults[kStickDirCount] = {"W", "S", "A", "D", "I", "K", "J", "L"};

        std::vector<std::string> words(const std::string &list)
        {
            std::vector<std::string> out;
            std::istringstream in(list);
            for (std::string w; in >> w;)
                out.push_back(w);
            return out;
        }

        // Version 1 names (raylib) -> version 2 names (SDL scancodes / positional gamepad controls).
        std::string migrateName(const std::string &v1)
        {
            static const std::map<std::string, std::string> names = {
                {"KEY_UP", "Up"}, {"KEY_DOWN", "Down"}, {"KEY_LEFT", "Left"}, {"KEY_RIGHT", "Right"},
                {"KEY_SPACE", "Space"}, {"KEY_ENTER", "Return"}, {"KEY_TAB", "Tab"}, {"KEY_BACKSPACE", "Backspace"},
                {"KEY_ESCAPE", "Escape"}, {"KEY_LEFT_SHIFT", "Left Shift"}, {"KEY_RIGHT_SHIFT", "Right Shift"},
                {"KEY_LEFT_CONTROL", "Left Ctrl"}, {"KEY_RIGHT_CONTROL", "Right Ctrl"}, {"KEY_LEFT_ALT", "Left Alt"},
                {"KEY_RIGHT_ALT", "Right Alt"}, {"KEY_COMMA", ","}, {"KEY_PERIOD", "."}, {"KEY_SLASH", "/"},
                {"KEY_SEMICOLON", ";"}, {"KEY_APOSTROPHE", "'"}, {"KEY_ZERO", "0"}, {"KEY_ONE", "1"}, {"KEY_TWO", "2"},
                {"KEY_THREE", "3"}, {"KEY_FOUR", "4"}, {"KEY_FIVE", "5"}, {"KEY_SIX", "6"}, {"KEY_SEVEN", "7"},
                {"KEY_EIGHT", "8"}, {"KEY_NINE", "9"},
                {"PAD_LEFT_FACE_UP", "dpup"}, {"PAD_LEFT_FACE_DOWN", "dpdown"}, {"PAD_LEFT_FACE_LEFT", "dpleft"},
                {"PAD_LEFT_FACE_RIGHT", "dpright"}, {"PAD_RIGHT_FACE_DOWN", "south"}, {"PAD_RIGHT_FACE_RIGHT", "east"},
                {"PAD_RIGHT_FACE_LEFT", "west"}, {"PAD_RIGHT_FACE_UP", "north"}, {"PAD_LEFT_TRIGGER_1", "lshoulder"},
                {"PAD_RIGHT_TRIGGER_1", "rshoulder"}, {"PAD_LEFT_TRIGGER_2", "lefttrigger"},
                {"PAD_RIGHT_TRIGGER_2", "righttrigger"}, {"PAD_MIDDLE_LEFT", "back"}, {"PAD_MIDDLE_RIGHT", "start"},
                {"PAD_LEFT_THUMB", "lstick"}, {"PAD_RIGHT_THUMB", "rstick"},
            };
            if (auto it = names.find(v1); it != names.end())
                return it->second;
            if (v1.size() == 5 && v1.rfind("KEY_", 0) == 0)
                return v1.substr(4); // KEY_A .. KEY_Z
            return {};
        }

        std::vector<std::string> stringList(const toml::value &v)
        {
            std::vector<std::string> out;
            if (v.is_string())
                out = words(v.as_string());
            else if (v.is_array())
                for (const auto &item : v.as_array())
                    if (item.is_string())
                        out.push_back(item.as_string());
            return out;
        }

        float number(const toml::value &table, const char *key, float fallback)
        {
            if (!table.is_table() || !table.contains(key))
                return fallback;
            const toml::value &v = table.at(key);
            if (v.is_floating())
                return static_cast<float>(v.as_floating());
            if (v.is_integer())
                return static_cast<float>(v.as_integer());
            return fallback;
        }

        // A [gamepad] table (or one controller's) on top of `base`.
        GamepadProfile readGamepad(const toml::value &t, GamepadProfile base, const std::string &where)
        {
            for (int b = 0; b < kPs2ButtonCount; ++b)
            {
                const char *name = ps2Name(static_cast<Ps2Button>(b));
                if (!t.contains(name))
                    continue;
                base.buttons[b].clear();
                for (const std::string &s : stringList(t.at(name)))
                {
                    if (auto src = padSourceFromName(s))
                        base.buttons[b].push_back(*src);
                    else
                        std::fprintf(stderr, "[input] %s.%s: unknown control '%s'\n", where.c_str(), name, s.c_str());
                }
            }
            for (int d = 0; d < kStickDirCount; ++d)
            {
                const char *name = stickDirName(static_cast<StickDir>(d));
                if (!t.contains(name))
                    continue;
                base.stickDirs[d].clear();
                for (const std::string &s : stringList(t.at(name)))
                    if (auto src = padSourceFromName(s))
                        base.stickDirs[d].push_back(*src);
            }
            base.left.inner = number(t, "left_deadzone", number(t, "deadzone", base.left.inner));
            base.right.inner = number(t, "right_deadzone", number(t, "deadzone", base.right.inner));
            base.left.outer = number(t, "left_outer", number(t, "outer", base.left.outer));
            base.right.outer = number(t, "right_outer", number(t, "outer", base.right.outer));
            base.triggerOn = number(t, "trigger_on", base.triggerOn);
            base.triggerOff = std::min(base.triggerOn, number(t, "trigger_off", base.triggerOff));
            base.rumble = std::clamp(number(t, "rumble", base.rumble), 0.0f, 1.0f);
            return base;
        }

        std::string quoted(const std::vector<std::string> &list)
        {
            std::string out = "[";
            for (size_t i = 0; i < list.size(); ++i)
                out += (i ? ", \"" : "\"") + list[i] + "\"";
            return out + "]";
        }

        std::vector<std::string> sourceNames(const std::vector<PadSource> &sources)
        {
            std::vector<std::string> out;
            for (const PadSource &s : sources)
                out.push_back(padSourceName(s));
            return out;
        }

        void writeGamepad(std::ostream &out, const GamepadProfile &p)
        {
            for (int b = 0; b < kPs2ButtonCount; ++b)
                out << ps2Name(static_cast<Ps2Button>(b)) << " = " << quoted(sourceNames(p.buttons[b])) << "\n";
            for (int d = 0; d < kStickDirCount; ++d)
                if (!p.stickDirs[d].empty())
                    out << stickDirName(static_cast<StickDir>(d)) << " = " << quoted(sourceNames(p.stickDirs[d])) << "\n";
            out << "left_deadzone = " << p.left.inner << "\nright_deadzone = " << p.right.inner
                << "\nleft_outer = " << p.left.outer << "\nright_outer = " << p.right.outer
                << "\ntrigger_on = " << p.triggerOn << "\ntrigger_off = " << p.triggerOff
                << "\nrumble = " << p.rumble << "\n";
        }

        Config fromVersion1(const toml::value &v1)
        {
            Config c = defaultConfig();
            for (int b = 0; b < kPs2ButtonCount; ++b)
            {
                const char *name = ps2Name(static_cast<Ps2Button>(b));
                if (!v1.contains(name))
                    continue;
                c.keyButtons[b].clear();
                c.gamepad.buttons[b].clear();
                for (const std::string &w : stringList(v1.at(name)))
                {
                    const std::string to = migrateName(w);
                    if (w.rfind("KEY_", 0) == 0 && !to.empty())
                        c.keyButtons[b].push_back(to);
                    else if (auto src = padSourceFromName(to))
                        c.gamepad.buttons[b].push_back(*src);
                }
                // Version 1 always let the analog triggers press L2/R2.
                if (b == static_cast<int>(Ps2Button::L2) || b == static_cast<int>(Ps2Button::R2))
                {
                    const PadSource trigger{PadSource::Kind::AxisPlus,
                                            static_cast<uint8_t>(b == static_cast<int>(Ps2Button::L2)
                                                                     ? PadAxis::LeftTrigger : PadAxis::RightTrigger)};
                    auto &list = c.gamepad.buttons[b];
                    if (std::find(list.begin(), list.end(), trigger) == list.end())
                        list.push_back(trigger);
                }
            }
            for (int d = 0; d < kStickDirCount; ++d)
            {
                const char *name = stickDirName(static_cast<StickDir>(d));
                if (!v1.contains(name))
                    continue;
                c.keyStickDirs[d].clear();
                for (const std::string &w : stringList(v1.at(name)))
                    if (const std::string to = migrateName(w); !to.empty())
                        c.keyStickDirs[d].push_back(to);
            }
            c.gamepad.left.inner = c.gamepad.right.inner = std::clamp(number(v1, "deadzone", 0.2f), 0.0f, 0.9f);
            return c;
        }
    }

    const GamepadProfile &Config::profileFor(const std::string &id) const
    {
        auto it = gamepads.find(id);
        return it != gamepads.end() ? it->second : gamepad;
    }

    Config defaultConfig()
    {
        Config c;
        for (int b = 0; b < kPs2ButtonCount; ++b)
            c.keyButtons[b] = words(kKeyDefaults[b]);
        for (int d = 0; d < kStickDirCount; ++d)
            c.keyStickDirs[d] = words(kKeyStickDefaults[d]);
        c.gamepad = defaultGamepadProfile();
        return c;
    }

    Config loadConfig(const std::filesystem::path &path)
    {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec))
        {
            Config c = defaultConfig();
            saveConfig(path, c);
            return c;
        }
        toml::value root;
        try
        {
            root = toml::parse(path);
        }
        catch (const std::exception &e)
        {
            std::fprintf(stderr, "[input] cannot read %s (using defaults): %s\n", path.string().c_str(), e.what());
            return defaultConfig();
        }
        if (!root.contains("version"))
        {
            Config c = fromVersion1(root);
            std::filesystem::copy_file(path, std::filesystem::path(path).concat(".v1.bak"),
                                       std::filesystem::copy_options::overwrite_existing, ec);
            saveConfig(path, c);
            std::fprintf(stderr, "[input] migrated %s to version 2 (old file kept as .v1.bak)\n", path.string().c_str());
            return c;
        }
        Config c = defaultConfig();
        if (root.contains("general"))
        {
            const toml::value &g = root.at("general");
            c.rumble = std::clamp(number(g, "rumble", c.rumble), 0.0f, 1.0f);
            if (g.contains("background_input") && g.at("background_input").is_boolean())
                c.backgroundInput = g.at("background_input").as_boolean();
        }
        if (root.contains("players") && root.at("players").is_table())
            for (const auto &[key, value] : root.at("players").as_table())
            {
                const int player = key == "p1" ? 0 : key == "p2" ? 1 : key == "off" ? -1 : -2;
                if (player != -2)
                    for (const std::string &id : stringList(value))
                        c.players[id] = player;
            }
        if (root.contains("keyboard") && root.at("keyboard").is_table())
        {
            const toml::value &k = root.at("keyboard");
            for (int b = 0; b < kPs2ButtonCount; ++b)
                if (k.contains(ps2Name(static_cast<Ps2Button>(b))))
                    c.keyButtons[b] = stringList(k.at(ps2Name(static_cast<Ps2Button>(b))));
            for (int d = 0; d < kStickDirCount; ++d)
                if (k.contains(stickDirName(static_cast<StickDir>(d))))
                    c.keyStickDirs[d] = stringList(k.at(stickDirName(static_cast<StickDir>(d))));
        }
        if (root.contains("gamepad") && root.at("gamepad").is_table())
        {
            const toml::value &g = root.at("gamepad");
            c.gamepad = readGamepad(g, c.gamepad, "gamepad");
            for (const auto &[key, value] : g.as_table())
                if (value.is_table())
                    c.gamepads[key] = readGamepad(value, c.gamepad, "gamepad." + key);
        }
        return c;
    }

    void saveConfig(const std::filesystem::path &path, const Config &c)
    {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        std::ostringstream out;
        out << "# Road Trip controls. Delete this file to get the defaults back.\n"
               "version = 2\n\n"
               "[general]\n"
               "rumble = " << c.rumble << "            # vibration strength, 0 (off) .. 1\n"
               "background_input = " << (c.backgroundInput ? "true" : "false")
            << "  # read controllers while the window is in the background\n\n"
               "# Who plays: device ids (\"keyboard\", or a controller's id from the Controllers window).\n"
               "# Unlisted devices are assigned automatically: keyboard and first controller = player 1,\n"
               "# second controller = player 2.\n"
               "[players]\n";
        std::vector<std::string> byPlayer[3];
        for (const auto &[id, player] : c.players)
            byPlayer[player < 0 ? 2 : player].push_back(id);
        out << "p1 = " << quoted(byPlayer[0]) << "\np2 = " << quoted(byPlayer[1]) << "\noff = " << quoted(byPlayer[2])
            << "\n\n# Keyboard: SDL key names (\"X\", \"Space\", \"Return\", \"Left Shift\", \"Up\", ...).\n[keyboard]\n";
        for (int b = 0; b < kPs2ButtonCount; ++b)
            out << ps2Name(static_cast<Ps2Button>(b)) << " = " << quoted(c.keyButtons[b]) << "\n";
        for (int d = 0; d < kStickDirCount; ++d)
            out << stickDirName(static_cast<StickDir>(d)) << " = " << quoted(c.keyStickDirs[d]) << "\n";
        out << "\n# Controllers, by position: south/east/west/north (Cross/Circle/Square/Triangle on a\n"
               "# PlayStation pad), dpup.., lshoulder/rshoulder, lefttrigger/righttrigger, lstick/rstick,\n"
               "# back, start, guide, touchpad, misc1, paddles, and stick directions leftx+/lefty-/...\n"
               "# A [gamepad.\"<id>\"] table overrides these for one controller.\n[gamepad]\n";
        writeGamepad(out, c.gamepad);
        for (const auto &[id, profile] : c.gamepads)
        {
            out << "\n[gamepad.\"" << id << "\"]\n";
            writeGamepad(out, profile);
        }
        std::ofstream(path) << out.str();
    }
}
