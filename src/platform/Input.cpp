#include "Input.h"

#include "platform/Paths.h"
#include "raylib.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ps2_stubs
{
    // Defined in the runtime (Kernel/Stubs/Pad.cpp); mutex-protected, read by scePadRead.
    void setPadOverrideState(uint16_t buttons, uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry);
}

namespace rt::input
{
    namespace
    {
        // DualShock 2 digital buttons (active-low bits in the pad data).
        struct ButtonDef
        {
            const char *name;
            uint16_t mask;
            const char *defaults;
        };
        constexpr ButtonDef kButtons[] = {
            {"up", 0x0010, "KEY_UP PAD_LEFT_FACE_UP"},
            {"down", 0x0040, "KEY_DOWN PAD_LEFT_FACE_DOWN"},
            {"left", 0x0080, "KEY_LEFT PAD_LEFT_FACE_LEFT"},
            {"right", 0x0020, "KEY_RIGHT PAD_LEFT_FACE_RIGHT"},
            {"cross", 0x4000, "KEY_X KEY_SPACE PAD_RIGHT_FACE_DOWN"},
            {"circle", 0x2000, "KEY_C PAD_RIGHT_FACE_RIGHT"},
            {"square", 0x8000, "KEY_Z PAD_RIGHT_FACE_LEFT"},
            {"triangle", 0x1000, "KEY_V PAD_RIGHT_FACE_UP"},
            {"l1", 0x0400, "KEY_Q PAD_LEFT_TRIGGER_1"},
            {"r1", 0x0800, "KEY_E PAD_RIGHT_TRIGGER_1"},
            {"l2", 0x0100, "KEY_ONE PAD_LEFT_TRIGGER_2"},
            {"r2", 0x0200, "KEY_THREE PAD_RIGHT_TRIGGER_2"},
            {"start", 0x0008, "KEY_ENTER PAD_MIDDLE_RIGHT"},
            {"select", 0x0001, "KEY_TAB KEY_BACKSPACE PAD_MIDDLE_LEFT"},
            {"l3", 0x0002, "KEY_F PAD_LEFT_THUMB"},
            {"r3", 0x0004, "KEY_G PAD_RIGHT_THUMB"},
        };

        // Keys that push a stick fully in one direction (the gamepad sticks always work too).
        struct StickDef
        {
            const char *name;
            int axis;      // 0 = left X, 1 = left Y, 2 = right X, 3 = right Y
            float sign;
            const char *defaults;
        };
        constexpr StickDef kStickKeys[] = {
            {"lstick_up", 1, -1.0f, "KEY_W"},
            {"lstick_down", 1, 1.0f, "KEY_S"},
            {"lstick_left", 0, -1.0f, "KEY_A"},
            {"lstick_right", 0, 1.0f, "KEY_D"},
            {"rstick_up", 3, -1.0f, "KEY_I"},
            {"rstick_down", 3, 1.0f, "KEY_K"},
            {"rstick_left", 2, -1.0f, "KEY_J"},
            {"rstick_right", 2, 1.0f, "KEY_L"},
        };

        const std::map<std::string, int> &keyNames()
        {
            static const std::map<std::string, int> names = [] {
                std::map<std::string, int> m{
                    {"KEY_UP", KEY_UP}, {"KEY_DOWN", KEY_DOWN}, {"KEY_LEFT", KEY_LEFT}, {"KEY_RIGHT", KEY_RIGHT},
                    {"KEY_SPACE", KEY_SPACE}, {"KEY_ENTER", KEY_ENTER}, {"KEY_TAB", KEY_TAB},
                    {"KEY_BACKSPACE", KEY_BACKSPACE}, {"KEY_ESCAPE", KEY_ESCAPE},
                    {"KEY_LEFT_SHIFT", KEY_LEFT_SHIFT}, {"KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT},
                    {"KEY_LEFT_CONTROL", KEY_LEFT_CONTROL}, {"KEY_RIGHT_CONTROL", KEY_RIGHT_CONTROL},
                    {"KEY_LEFT_ALT", KEY_LEFT_ALT}, {"KEY_RIGHT_ALT", KEY_RIGHT_ALT},
                    {"KEY_ZERO", KEY_ZERO}, {"KEY_ONE", KEY_ONE}, {"KEY_TWO", KEY_TWO}, {"KEY_THREE", KEY_THREE},
                    {"KEY_FOUR", KEY_FOUR}, {"KEY_FIVE", KEY_FIVE}, {"KEY_SIX", KEY_SIX}, {"KEY_SEVEN", KEY_SEVEN},
                    {"KEY_EIGHT", KEY_EIGHT}, {"KEY_NINE", KEY_NINE},
                    {"KEY_COMMA", KEY_COMMA}, {"KEY_PERIOD", KEY_PERIOD}, {"KEY_SLASH", KEY_SLASH},
                    {"KEY_SEMICOLON", KEY_SEMICOLON}, {"KEY_APOSTROPHE", KEY_APOSTROPHE},
                };
                for (char c = 'A'; c <= 'Z'; ++c)
                    m[std::string("KEY_") + c] = KEY_A + (c - 'A');
                return m;
            }();
            return names;
        }

        const std::map<std::string, int> &padNames()
        {
            static const std::map<std::string, int> names{
                {"PAD_LEFT_FACE_UP", GAMEPAD_BUTTON_LEFT_FACE_UP}, {"PAD_LEFT_FACE_DOWN", GAMEPAD_BUTTON_LEFT_FACE_DOWN},
                {"PAD_LEFT_FACE_LEFT", GAMEPAD_BUTTON_LEFT_FACE_LEFT}, {"PAD_LEFT_FACE_RIGHT", GAMEPAD_BUTTON_LEFT_FACE_RIGHT},
                {"PAD_RIGHT_FACE_UP", GAMEPAD_BUTTON_RIGHT_FACE_UP}, {"PAD_RIGHT_FACE_DOWN", GAMEPAD_BUTTON_RIGHT_FACE_DOWN},
                {"PAD_RIGHT_FACE_LEFT", GAMEPAD_BUTTON_RIGHT_FACE_LEFT}, {"PAD_RIGHT_FACE_RIGHT", GAMEPAD_BUTTON_RIGHT_FACE_RIGHT},
                {"PAD_LEFT_TRIGGER_1", GAMEPAD_BUTTON_LEFT_TRIGGER_1}, {"PAD_LEFT_TRIGGER_2", GAMEPAD_BUTTON_LEFT_TRIGGER_2},
                {"PAD_RIGHT_TRIGGER_1", GAMEPAD_BUTTON_RIGHT_TRIGGER_1}, {"PAD_RIGHT_TRIGGER_2", GAMEPAD_BUTTON_RIGHT_TRIGGER_2},
                {"PAD_MIDDLE_LEFT", GAMEPAD_BUTTON_MIDDLE_LEFT}, {"PAD_MIDDLE_RIGHT", GAMEPAD_BUTTON_MIDDLE_RIGHT},
                {"PAD_LEFT_THUMB", GAMEPAD_BUTTON_LEFT_THUMB}, {"PAD_RIGHT_THUMB", GAMEPAD_BUTTON_RIGHT_THUMB},
            };
            return names;
        }

        struct Binding
        {
            std::vector<int> keys;
            std::vector<int> padButtons;
        };

        std::map<std::string, Binding> g_bindings;
        float g_deadzone = 0.2f;

        Binding parseBinding(const std::string &list, const std::string &where)
        {
            Binding b;
            std::istringstream words(list);
            for (std::string w; words >> w;)
            {
                if (auto k = keyNames().find(w); k != keyNames().end())
                    b.keys.push_back(k->second);
                else if (auto p = padNames().find(w); p != padNames().end())
                    b.padButtons.push_back(p->second);
                else
                    std::fprintf(stderr, "[input] %s: unknown input name '%s'\n", where.c_str(), w.c_str());
            }
            return b;
        }

        std::string defaultConfig()
        {
            std::ostringstream s;
            s << "# Road Trip input mapping. Each line: <ps2 input> = \"<names separated by spaces>\".\n"
                 "# Key names are raylib's (KEY_A .. KEY_Z, KEY_UP, KEY_SPACE, KEY_ENTER, ...), gamepad buttons\n"
                 "# are PAD_* (PAD_RIGHT_FACE_DOWN = A on Xbox / Cross on PlayStation). Gamepad sticks and\n"
                 "# analog triggers always work. Delete this file to get the defaults back.\n\n";
            for (const auto &b : kButtons)
                s << b.name << " = \"" << b.defaults << "\"\n";
            s << "\n# Keys that push a stick fully in one direction.\n";
            for (const auto &k : kStickKeys)
                s << k.name << " = \"" << k.defaults << "\"\n";
            s << "\n# Stick deadzone, 0..1.\ndeadzone = 0.2\n";
            return s.str();
        }

        void load()
        {
            const auto path = paths::dataRoot() / "input.toml";
            std::map<std::string, std::string> values;
            for (const auto &b : kButtons)
                values[b.name] = b.defaults;
            for (const auto &k : kStickKeys)
                values[k.name] = k.defaults;

            std::ifstream in(path);
            if (!in)
            {
                std::error_code ec;
                std::filesystem::create_directories(path.parent_path(), ec);
                std::ofstream(path) << defaultConfig();
            }
            else
            {
                for (std::string line; std::getline(in, line);)
                {
                    const auto hash = line.find('#');
                    if (hash != std::string::npos)
                        line.erase(hash);
                    const auto eq = line.find('=');
                    if (eq == std::string::npos)
                        continue;
                    auto trim = [](std::string s) {
                        const auto b = s.find_first_not_of(" \t\"");
                        const auto e = s.find_last_not_of(" \t\"\r");
                        return b == std::string::npos ? std::string() : s.substr(b, e - b + 1);
                    };
                    const std::string key = trim(line.substr(0, eq));
                    const std::string value = trim(line.substr(eq + 1));
                    if (key == "deadzone")
                        g_deadzone = std::clamp(std::strtof(value.c_str(), nullptr), 0.0f, 0.95f);
                    else if (values.count(key))
                        values[key] = value;
                    else if (!key.empty())
                        std::fprintf(stderr, "[input] %s: unknown setting '%s'\n", path.c_str(), key.c_str());
                }
            }
            for (const auto &[name, list] : values)
                g_bindings[name] = parseBinding(list, name);
        }

        bool active(const Binding &b)
        {
            for (int k : b.keys)
                if (IsKeyDown(k))
                    return true;
            for (int pad = 0; pad < 4; ++pad)
                if (IsGamepadAvailable(pad))
                    for (int button : b.padButtons)
                        if (IsGamepadButtonDown(pad, button))
                            return true;
            return false;
        }

        // RT_INPUT_SCRIPT="<seconds>:<button>[:<hold seconds>],..." presses buttons at fixed times
        // after startup (debugging without a person at the keyboard), e.g. "40:start,45:cross:3".
        struct ScriptedPress
        {
            double at, hold;
            uint16_t mask;
        };
        std::vector<ScriptedPress> g_script;
        std::chrono::steady_clock::time_point g_start;

        void loadScript()
        {
            const char *env = std::getenv("RT_INPUT_SCRIPT");
            if (!env)
                return;
            std::istringstream items(env);
            for (std::string item; std::getline(items, item, ',');)
            {
                std::istringstream parts(item);
                std::string at, name, hold;
                std::getline(parts, at, ':');
                std::getline(parts, name, ':');
                std::getline(parts, hold, ':');
                for (const auto &b : kButtons)
                    if (name == b.name)
                        g_script.push_back({std::atof(at.c_str()), hold.empty() ? 0.25 : std::atof(hold.c_str()), b.mask});
            }
        }

        uint8_t toStickByte(float v)
        {
            v = std::clamp(v, -1.0f, 1.0f);
            return static_cast<uint8_t>(std::lround(127.5f + v * 127.5f));
        }
    }

    void initialize()
    {
        SetExitKey(KEY_NULL); // Escape is a game key now; Cmd+Q or the close button quit.
        load();
        loadScript();
        g_start = std::chrono::steady_clock::now();
    }

    void update()
    {
        uint16_t buttons = 0xFFFF; // active-low
        for (const auto &b : kButtons)
            if (active(g_bindings[b.name]))
                buttons &= static_cast<uint16_t>(~b.mask);

        if (!g_script.empty())
        {
            const double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - g_start).count();
            for (const auto &p : g_script)
                if (t >= p.at && t < p.at + p.hold)
                    buttons &= static_cast<uint16_t>(~p.mask);
        }

        float axes[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        for (int pad = 0; pad < 4; ++pad)
        {
            if (!IsGamepadAvailable(pad))
                continue;
            const int ids[4] = {GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_RIGHT_Y};
            for (int a = 0; a < 4; ++a)
            {
                const float v = GetGamepadAxisMovement(pad, ids[a]);
                if (std::fabs(v) > g_deadzone && std::fabs(v) > std::fabs(axes[a]))
                    axes[a] = (v - std::copysign(g_deadzone, v)) / (1.0f - g_deadzone);
            }
            // Analog triggers (-1 released .. 1 pressed) also press L2/R2.
            if (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f)
                buttons &= static_cast<uint16_t>(~0x0100u);
            if (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f)
                buttons &= static_cast<uint16_t>(~0x0200u);
        }
        for (const auto &k : kStickKeys)
            if (active(g_bindings[k.name]))
                axes[k.axis] = k.sign;

        ps2_stubs::setPadOverrideState(buttons, toStickByte(axes[0]), toStickByte(axes[1]), toStickByte(axes[2]),
                                       toStickByte(axes[3]));
    }
}
