#include "platform/Controllers.h"
#include "platform/Input.h"

#include "State.h"
#include "raylib.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <array>
#include <optional>

namespace rt::input
{
    namespace
    {
        using Clock = std::chrono::steady_clock;

        std::string g_device;   // the device being rebound
        int g_rebinding = -1;   // Ps2Button being rebound, or -1
        Clock::time_point g_rebindStart;
        // What was already held when rebinding started (it must be let go before it counts).
        GamepadSnapshot g_heldPad;
        std::vector<bool> g_heldKeys;

        const Device *device(const std::string &id)
        {
            for (const Device &d : detail::devices().list())
                if (d.id == id)
                    return &d;
            return nullptr;
        }

        // Binds `got` to `button` in place of its first control (others stay). If another button
        // used `got`, it gets this button's old first control: the two swap. Returns that button.
        template <typename T, size_t N>
        int bindControl(std::array<std::vector<T>, N> &buttons, int button, const T &got)
        {
            std::vector<T> &mine = buttons[button];
            const std::optional<T> old = mine.empty() ? std::nullopt : std::optional<T>(mine.front());
            int swapped = -1;
            for (size_t b = 0; b < N; ++b)
            {
                if (static_cast<int>(b) == button)
                    continue;
                auto it = std::find(buttons[b].begin(), buttons[b].end(), got);
                if (it == buttons[b].end())
                    continue;
                swapped = static_cast<int>(b);
                if (old && std::find(buttons[b].begin(), buttons[b].end(), *old) == buttons[b].end())
                    *it = *old;
                else
                    buttons[b].erase(it);
            }
            mine.erase(std::remove(mine.begin(), mine.end(), got), mine.end());
            if (mine.empty())
                mine.push_back(got);
            else
                mine.front() = got;
            return swapped;
        }

        int g_swappedWith = -1;
        Clock::time_point g_swappedAt;

        void finished(int swapped)
        {
            g_swappedWith = swapped;
            g_swappedAt = Clock::now();
            g_rebinding = -1;
        }

        // A controller's own profile, made from the shared one on its first edit.
        GamepadProfile &ownProfile(const std::string &id)
        {
            Config &c = detail::config();
            if (!c.gamepads.count(id))
                c.gamepads[id] = c.gamepad;
            return c.gamepads[id];
        }
    }

    PlayerChoice playerChoice(const std::string &id)
    {
        const Config &c = detail::config();
        const auto it = c.players.find(id);
        if (it == c.players.end())
            return PlayerChoice::Auto;
        return it->second < 0 ? PlayerChoice::Off : it->second == 0 ? PlayerChoice::Player1 : PlayerChoice::Player2;
    }

    void setPlayerChoice(const std::string &id, PlayerChoice choice)
    {
        Config &c = detail::config();
        if (choice == PlayerChoice::Auto)
            c.players.erase(id);
        else
            c.players[id] = choice == PlayerChoice::Off ? -1 : choice == PlayerChoice::Player1 ? 0 : 1;
        detail::applyConfig();
        detail::save();
    }

    std::string controlName(const std::string &type, PadSource s)
    {
        if (s.kind != PadSource::Kind::Button)
        {
            switch (static_cast<PadAxis>(s.index))
            {
            case PadAxis::LeftTrigger: return type.rfind("xbox", 0) == 0 ? "LT" : type == "switchpro" ? "ZL" : "L2";
            case PadAxis::RightTrigger: return type.rfind("xbox", 0) == 0 ? "RT" : type == "switchpro" ? "ZR" : "R2";
            default: return padSourceName(s);
            }
        }
        const bool xbox = type.rfind("xbox", 0) == 0, nin = type == "switchpro" || type == "joycon";
        switch (static_cast<PadButton>(s.index))
        {
        case PadButton::South: return xbox ? "A" : nin ? "B" : "Cross";
        case PadButton::East: return xbox ? "B" : nin ? "A" : "Circle";
        case PadButton::West: return xbox ? "X" : nin ? "Y" : "Square";
        case PadButton::North: return xbox ? "Y" : nin ? "X" : "Triangle";
        case PadButton::Back: return xbox ? "View" : nin ? "-" : "Create";
        case PadButton::Start: return xbox ? "Menu" : nin ? "+" : "Options";
        case PadButton::LStick: return xbox || nin ? "Left stick press" : "L3";
        case PadButton::RStick: return xbox || nin ? "Right stick press" : "R3";
        case PadButton::LShoulder: return xbox ? "LB" : nin ? "L" : "L1";
        case PadButton::RShoulder: return xbox ? "RB" : nin ? "R" : "R1";
        case PadButton::DpadUp: return "D-pad up";
        case PadButton::DpadDown: return "D-pad down";
        case PadButton::DpadLeft: return "D-pad left";
        case PadButton::DpadRight: return "D-pad right";
        case PadButton::Touchpad: return "Touchpad";
        default: return padSourceName(s);
        }
    }

    std::string bindingText(const std::string &id, int button)
    {
        const Config &c = detail::config();
        std::string out;
        if (id == kKeyboardId)
            for (const std::string &k : c.keyButtons[button])
                out += (out.empty() ? "" : ", ") + k;
        else
        {
            const Device *d = device(id);
            const std::string type = d ? d->type : "";
            for (const PadSource &s : c.profileFor(id).buttons[button])
                out += (out.empty() ? "" : ", ") + controlName(type, s);
        }
        return out.empty() ? "-" : out;
    }

    void startRebind(const std::string &id, int button)
    {
        g_device = id;
        g_rebinding = button;
        g_rebindStart = Clock::now();
        g_heldPad = {};
        if (const Device *d = device(id))
            g_heldPad = Devices::snapshot(*d);
        int count = 0;
        const bool *keys = SDL_GetKeyboardState(&count);
        g_heldKeys.assign(keys, keys + count);
    }

    int rebinding() { return g_rebinding; }

    float rebindSecondsLeft()
    {
        if (g_rebinding < 0)
            return 0;
        return std::max(0.0f, 6.0f - std::chrono::duration<float>(Clock::now() - g_rebindStart).count());
    }

    int lastRebindSwappedWith(float withinSeconds)
    {
        if (g_swappedWith < 0 || std::chrono::duration<float>(Clock::now() - g_swappedAt).count() > withinSeconds)
            return -1;
        return g_swappedWith;
    }

    void pollRebind()
    {
        if (g_rebinding < 0)
            return;
        if (IsKeyPressed(KEY_ESCAPE) || Clock::now() - g_rebindStart > std::chrono::seconds(6))
        {
            g_rebinding = -1;
            return;
        }
        Config &c = detail::config();
        if (g_device == kKeyboardId)
        {
            int count = 0;
            const bool *keys = SDL_GetKeyboardState(&count);
            g_heldKeys.resize(count, false);
            for (int code = 0; keys && code < count; ++code)
            {
                if (!keys[code])
                    g_heldKeys[code] = false;
                if (keys[code] && !g_heldKeys[code] && code != SDL_SCANCODE_ESCAPE)
                {
                    finished(bindControl(c.keyButtons, g_rebinding, std::string(SDL_GetScancodeName(static_cast<SDL_Scancode>(code)))));
                    detail::applyConfig();
                    detail::save();
                    return;
                }
            }
            return;
        }
        const Device *d = device(g_device);
        if (!d)
        {
            g_rebinding = -1;
            return;
        }
        const GamepadSnapshot s = Devices::snapshot(*d);
        // Guide cancels on a pad (it is never bound).
        if (s.buttons[static_cast<int>(PadButton::Guide)])
        {
            g_rebinding = -1;
            return;
        }
        for (int b = 0; b < kPadButtonCount; ++b)
            g_heldPad.buttons[b] = g_heldPad.buttons[b] && s.buttons[b];
        for (int a = 0; a < kPadAxisCount; ++a)
            if (std::fabs(s.axes[a]) < 0.25f)
                g_heldPad.axes[a] = 0;
        std::optional<PadSource> got;
        for (int b = 0; b < kPadButtonCount && !got; ++b)
            if (s.buttons[b] && !g_heldPad.buttons[b] && b != static_cast<int>(PadButton::Guide))
                got = PadSource{PadSource::Kind::Button, static_cast<uint8_t>(b)};
        for (int a = 0; a < kPadAxisCount && !got; ++a)
            if (std::fabs(s.axes[a]) > 0.5f && std::fabs(g_heldPad.axes[a]) < 0.25f)
                got = PadSource{s.axes[a] > 0 ? PadSource::Kind::AxisPlus : PadSource::Kind::AxisMinus, static_cast<uint8_t>(a)};
        if (!got)
            return;
        finished(bindControl(ownProfile(d->id).buttons, g_rebinding, *got));
        Devices::rumble(*d, 0.4f, 0.4f, 120); // "got it"
        detail::save();
    }

    void resetBindings(const std::string &id)
    {
        Config &c = detail::config();
        const Config defaults = defaultConfig();
        if (id == kKeyboardId)
        {
            c.keyButtons = defaults.keyButtons;
            c.keyStickDirs = defaults.keyStickDirs;
        }
        else if (c.gamepads.count(id))
            c.gamepads[id].buttons = c.gamepad.buttons;
        detail::applyConfig();
        detail::save();
    }

    float stickDeadzone(const std::string &id, bool right)
    {
        const GamepadProfile &p = detail::config().profileFor(id);
        return right ? p.right.inner : p.left.inner;
    }

    void setStickDeadzone(const std::string &id, bool right, float value)
    {
        GamepadProfile &p = ownProfile(id);
        (right ? p.right : p.left).inner = std::clamp(value, 0.0f, 0.5f);
        detail::save();
    }

    float vibration(const std::string &id) { return detail::config().profileFor(id).rumble; }

    void setVibration(const std::string &id, float value)
    {
        ownProfile(id).rumble = std::clamp(value, 0.0f, 1.0f);
        detail::save();
    }

    float vibrationOverall() { return detail::config().rumble; }

    void setVibrationOverall(float value)
    {
        detail::config().rumble = std::clamp(value, 0.0f, 1.0f);
        detail::save();
    }

    void testVibration(const std::string &id) { detail::testRumble(id); }
}
