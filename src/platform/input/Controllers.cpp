#include "platform/Controllers.h"
#include "platform/Input.h"

#include "State.h"
#include "platform/Host.h"

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

        // The Driving controls page's capture.
        int g_capturePlayer = -1;
        Clock::time_point g_captureStart;
        std::optional<CapturedControl> g_captured;
        std::vector<GamepadSnapshot> g_captureHeld; // per device in the player's slot, at the start
        // The pad's back button held while waiting: a tap binds it, holding it cancels.
        constexpr float kCancelHoldSeconds = 0.8f;
        std::string g_backDevice;
        Clock::time_point g_backSince;

        int g_swappedWith = -1;
        Clock::time_point g_swappedAt;

        void finished(int swapped)
        {
            g_swappedWith = swapped;
            g_swappedAt = Clock::now();
            g_rebinding = -1;
        }

        // The shared bindings in this controller's family layout (Mapping.h familyProfile).
        GamepadProfile sharedFor(const std::string &id)
        {
            for (const DeviceStatus &d : devices())
                if (d.id == id)
                    return familyProfile(detail::config().gamepad, padFamily(d.type));
            return detail::config().gamepad;
        }

        // A controller's own profile, made from the shared one (in its layout) on its first edit.
        GamepadProfile &ownProfile(const std::string &id)
        {
            Config &c = detail::config();
            if (!c.gamepads.count(id))
                c.gamepads[id] = sharedFor(id);
            return c.gamepads[id];
        }

        // The bindings a controller plays with: its own, else the shared ones in its layout.
        GamepadProfile sharedOrOwn(const std::string &id)
        {
            const Config &c = detail::config();
            return c.gamepads.count(id) ? c.gamepads.at(id) : sharedFor(id);
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
            case PadAxis::LeftTrigger: return padFamily(type) == "xbox" ? "LT" : type == "switchpro" ? "ZL" : "L2";
            case PadAxis::RightTrigger: return padFamily(type) == "xbox" ? "RT" : type == "switchpro" ? "ZR" : "R2";
            default: return padSourceName(s);
            }
        }
        // Labels by the pad's family (Mapping.h padFamily): "standard" pads such as the AYN Thor's
        // are labelled the Xbox way.
        const bool xbox = padFamily(type) == "xbox", nin = type == "switchpro" || type == "joycon";
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
            // What it plays with: its own bindings, else the shared ones in its family's layout.
            const GamepadProfile profile = sharedOrOwn(id); // (a temporary would die before the loop)
            for (const PadSource &s : profile.buttons[button])
                out += (out.empty() ? "" : ", ") + controlName(type, s);
        }
        return out.empty() ? "-" : out;
    }

    std::string familyOf(const std::string &id)
    {
        if (id == kKeyboardId)
            return "keyboard";
        const Device *d = device(id);
        return padFamily(d ? d->type : "");
    }

    std::vector<ControlGlyph> bindingGlyphs(const std::string &id, int button)
    {
        std::vector<ControlGlyph> out;
        if (button < 0 || button >= kPs2ButtonCount)
            return out;
        const std::string family = familyOf(id);
        if (id == kKeyboardId)
        {
            for (const std::string &k : detail::config().keyButtons[button])
                out.push_back({family, 0, 0, k});
            return out;
        }
        const GamepadProfile profile = sharedOrOwn(id);
        for (const PadSource &s : profile.buttons[button])
            out.push_back({family, static_cast<int>(s.kind), s.index, {}});
        return out;
    }

    void sampleVibration(int style)
    {
        for (const std::string &id : detail::slots()[0])
            for (const Device &d : detail::devices().list())
                if (d.id == id)
                {
                    const float k = detail::config().rumble * detail::config().profileFor(id).rumble;
                    if (style == 0) // Dynamic: a soft engine hum with the triggers
                    {
                        Devices::rumble(d, 0.15f * k, 0.35f * k, 350);
                        Devices::rumbleTriggers(d, 0.25f * k, 0.4f * k, 350);
                    }
                    else // Classic: the game's on/off buzz
                        Devices::rumble(d, 0.6f * k, 1.0f * k, 250);
                }
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

    void pollCapture();

    void pollRebind()
    {
        if (g_capturePlayer >= 0)
            pollCapture();
        if (g_rebinding < 0)
            return;
        if (host::keyPressed(SDL_SCANCODE_ESCAPE) || Clock::now() - g_rebindStart > std::chrono::seconds(6))
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
            c.gamepads[id].buttons = sharedFor(id).buttons;
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

namespace rt::input
{
    namespace
    {
        // The DS2 button a control presses on a device (first match), or -1.
        int buttonOfControl(const std::string &id, const CapturedControl &c)
        {
            const Config &cfg = detail::config();
            const GamepadProfile profile = id == kKeyboardId ? GamepadProfile{} : sharedOrOwn(id);
            for (int b = 0; b < kPs2ButtonCount; ++b)
            {
                if (id == kKeyboardId)
                {
                    for (const std::string &k : cfg.keyButtons[b])
                        if (k == c.key)
                            return b;
                }
                else
                    for (const PadSource &s : profile.buttons[b])
                        if (static_cast<int>(s.kind) == c.padKind && s.index == c.padIndex)
                            return b;
            }
            return -1;
        }
    }

    void startCapture(int player)
    {
        g_capturePlayer = player;
        g_captureStart = Clock::now();
        g_captured.reset();
        g_captureHeld.clear();
        g_backDevice.clear();
        for (const std::string &id : detail::slots()[player])
            if (const Device *d = device(id))
                g_captureHeld.push_back(Devices::snapshot(*d));
            else
                g_captureHeld.push_back({});
        int count = 0;
        const bool *keys = SDL_GetKeyboardState(&count);
        g_heldKeys.assign(keys, keys + count);
    }

    bool capturing() { return g_capturePlayer >= 0; }

    float captureCancelHold()
    {
        if (g_capturePlayer < 0 || g_backDevice.empty())
            return -1.0f;
        return std::min(1.0f, std::chrono::duration<float>(Clock::now() - g_backSince).count() / kCancelHoldSeconds);
    }

    float captureSecondsLeft()
    {
        if (g_capturePlayer < 0)
            return 0;
        return std::max(0.0f, 6.0f - std::chrono::duration<float>(Clock::now() - g_captureStart).count());
    }

    std::optional<CapturedControl> takeCapture()
    {
        std::optional<CapturedControl> c;
        c.swap(g_captured);
        return c;
    }

    void pollCapture()
    {
        const int player = g_capturePlayer;
        if (host::keyPressed(SDL_SCANCODE_ESCAPE) || Clock::now() - g_captureStart > std::chrono::seconds(6) ||
            player < 0 || player >= kPlayers)
        {
            g_capturePlayer = -1;
            return;
        }
        const auto &slot = detail::slots()[player];
        auto done = [&](CapturedControl c) {
            c.ps2Button = buttonOfControl(c.device, c);
            g_captured = c;
            g_capturePlayer = -1;
            g_backDevice.clear();
            if (const Device *d = device(c.device))
                Devices::rumble(*d, 0.35f, 0.35f, 100); // "got it", on the control's own pad
        };
        for (size_t i = 0; i < slot.size(); ++i)
        {
            const std::string &id = slot[i];
            if (id == kKeyboardId)
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
                        CapturedControl c;
                        c.device = id;
                        c.key = SDL_GetScancodeName(static_cast<SDL_Scancode>(code));
                        return done(c);
                    }
                }
                continue;
            }
            const Device *d = device(id);
            if (!d || i >= g_captureHeld.size())
                continue;
            const GamepadSnapshot s = Devices::snapshot(*d);
            if (s.buttons[static_cast<int>(PadButton::Guide)])
            {
                g_capturePlayer = -1;
                return;
            }
            GamepadSnapshot &held = g_captureHeld[i];
            // The back button: bound on release after a tap, cancels once held.
            const int back = static_cast<int>(padFamily(d->type) == "nintendo" ? PadButton::South : PadButton::East);
            if (g_backDevice == id)
            {
                if (!s.buttons[back])
                {
                    CapturedControl c;
                    c.device = id;
                    c.padKind = static_cast<int>(PadSource::Kind::Button);
                    c.padIndex = back;
                    return done(c);
                }
                if (captureCancelHold() >= 1.0f)
                {
                    g_capturePlayer = -1;
                    g_backDevice.clear();
                    held.buttons[back] = true; // it stays held into the menu: not a Back there
                    return;
                }
            }
            else if (s.buttons[back] && !held.buttons[back] && g_backDevice.empty())
            {
                g_backDevice = id;
                g_backSince = Clock::now();
            }
            for (int b = 0; b < kPadButtonCount; ++b)
                held.buttons[b] = held.buttons[b] && s.buttons[b];
            for (int a = 0; a < kPadAxisCount; ++a)
                if (std::fabs(s.axes[a]) < 0.25f)
                    held.axes[a] = 0;
            for (int b = 0; b < kPadButtonCount; ++b)
                if (s.buttons[b] && !held.buttons[b] && b != static_cast<int>(PadButton::Guide) && b != back)
                {
                    CapturedControl c;
                    c.device = id;
                    c.padKind = static_cast<int>(PadSource::Kind::Button);
                    c.padIndex = b;
                    return done(c);
                }
            for (int a = 0; a < kPadAxisCount; ++a)
                if (std::fabs(s.axes[a]) > 0.5f && std::fabs(held.axes[a]) < 0.25f)
                {
                    CapturedControl c;
                    c.device = id;
                    c.padKind = static_cast<int>(s.axes[a] > 0 ? PadSource::Kind::AxisPlus : PadSource::Kind::AxisMinus);
                    c.padIndex = a;
                    return done(c);
                }
        }
    }

    void bindCaptured(const CapturedControl &c, int button)
    {
        if (button < 0 || button >= kPs2ButtonCount)
            return;
        Config &cfg = detail::config();
        if (c.device == kKeyboardId)
        {
            finished(bindControl(cfg.keyButtons, button, c.key));
            detail::applyConfig();
        }
        else
        {
            const PadSource src{static_cast<PadSource::Kind>(c.padKind), static_cast<uint8_t>(c.padIndex)};
            finished(bindControl(ownProfile(c.device).buttons, button, src));
        }
        detail::save();
    }

    std::string primaryDevice(int player)
    {
        if (player < 0 || player >= kPlayers)
            return kKeyboardId;
        for (const std::string &id : detail::slots()[player])
            if (id != kKeyboardId)
                return id;
        return kKeyboardId;
    }
}
