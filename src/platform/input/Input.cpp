#include "platform/Input.h"

#include "State.h"
#include "game/Driving.h"
#include "platform/Host.h"
#include "settings/Settings.h"
#include "platform/Paths.h"
#include "raylib.h"
#include "runtime/ps2_test_harness.h"

#include <SDL3/SDL.h>

#if defined(PS2X_ENABLE_DEBUG_UI)
#include "imgui.h"
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>

namespace rt::input
{
    namespace
    {
        using Clock = std::chrono::steady_clock;

        struct Motors
        {
            float low = 0, high = 0, left = 0, right = 0;
            Clock::time_point sent{};
        };

        Config g_config;
        Devices g_devices;
        Players g_players;
        Players::Slots g_slots;
        KeyboardProfile g_keyboard;
        Motors g_motors[kPlayers];
        bool g_ready = false;
        bool g_blocked = false; // another window (Options) has the input

        std::vector<int> scancodes(const std::vector<std::string> &names, const char *what)
        {
            std::vector<int> out;
            for (const std::string &n : names)
            {
                const SDL_Scancode code = SDL_GetScancodeFromName(n.c_str());
                if (code == SDL_SCANCODE_UNKNOWN)
                    std::fprintf(stderr, "[input] keyboard.%s: unknown key '%s'\n", what, n.c_str());
                else
                    out.push_back(code);
            }
            return out;
        }

        void resolveConfig()
        {
            for (int b = 0; b < kPs2ButtonCount; ++b)
                g_keyboard.buttons[b] = scancodes(g_config.keyButtons[b], ps2Name(static_cast<Ps2Button>(b)));
            for (int d = 0; d < kStickDirCount; ++d)
                g_keyboard.stickDirs[d] = scancodes(g_config.keyStickDirs[d], stickDirName(static_cast<StickDir>(d)));
            g_players.setManual(g_config.players);
        }

        void reassign()
        {
            std::vector<std::string> connected{kKeyboardId};
            for (const Device &d : g_devices.list())
                connected.push_back(d.id);
            g_slots = g_players.assign(connected);
            for (const Device &d : g_devices.list())
                Devices::showPlayer(d, playerOf(d.id));
        }

        bool keyboardBlocked()
        {
#if defined(PS2X_ENABLE_DEBUG_UI)
            if (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard)
                return true;
#endif
            return false;
        }

        // A motor speed in 1/64 steps, so a slowly changing level is not resent every frame.
        float quantize(float v) { return std::round(std::clamp(v, 0.0f, 1.0f) * 64.0f) / 64.0f; }

        // Vibration on that player's controllers: the game's own (polled from the pad emulation;
        // a real DualShock 2's large motor does not turn below about 0x40, the small one is on or
        // off), or, with dynamic vibration while driving, the car's (game/Driving.h): graded
        // speeds on both motors and the trigger motors.
        void applyRumble(bool paused)
        {
            const auto now = Clock::now();
            const rt::settings::Settings &st = rt::settings::current();
            for (int p = 0; p < kPlayers; ++p)
            {
                const ps2_test::ActuatorState a = ps2_test::actuatorState(p);
                float low = (!paused && a.large >= 0x40) ? a.large / 255.0f : 0.0f;
                float high = (!paused && a.small) ? 1.0f : 0.0f;
                float left = 0, right = 0;
                rt::game::RumbleOut dyn;
                if (!paused && st.vibration && st.dynamicVibration && rt::game::dynamicRumble(p, dyn))
                {
                    // The game's own motors still count (anything its scripts play that the car
                    // model misses), softened: its small motor only knows full speed.
                    low = std::max(dyn.low, low * 0.6f);
                    high = std::max(dyn.high, high * 0.3f);
                    left = dyn.left;
                    right = dyn.right;
                }
                low = quantize(low), high = quantize(high), left = quantize(left), right = quantize(right);
                Motors &m = g_motors[p];
                const bool on = low > 0 || high > 0 || left > 0 || right > 0;
                const bool changed = low != m.low || high != m.high || left != m.left || right != m.right;
                // Pulses last 250 ms and are renewed while the motors run, so a stalled game does
                // not leave a pad buzzing; changes go out at most every 15 ms.
                const auto since = now - m.sent;
                if (changed ? since < std::chrono::milliseconds(15) && on
                            : (!on || since < std::chrono::milliseconds(100)))
                    continue;
                for (const std::string &id : g_slots[p])
                    for (const Device &d : g_devices.list())
                        if (d.id == id)
                        {
                            const float scale = g_config.rumble * g_config.profileFor(d.id).rumble;
                            Devices::rumble(d, low * scale, high * scale, on ? 250 : 0);
                            if (left > 0 || right > 0 || m.left > 0 || m.right > 0)
                                Devices::rumbleTriggers(d, left * scale, right * scale, on ? 250 : 0);
                        }
                m.low = low;
                m.high = high;
                m.left = left;
                m.right = right;
                m.sent = now;
            }
        }

        // How far a control is pushed, 0..1 (a button: 0 or 1).
        float controlValue(const GamepadSnapshot &s, const PadSource &src)
        {
            switch (src.kind)
            {
            case PadSource::Kind::Button: return s.buttons[src.index] ? 1.0f : 0.0f;
            case PadSource::Kind::AxisPlus: return std::max(0.0f, s.axes[src.index]);
            case PadSource::Kind::AxisMinus: return std::max(0.0f, -s.axes[src.index]);
            }
            return 0;
        }

        // The analogue part of one device's Gas / Brake controls (axes on the game's Gas / Brake
        // buttons), and whether a button presses them.
        void addAnalog(rt::game::AnalogInput &in, const GamepadSnapshot &s, const GamepadProfile &profile, int gas, int brake)
        {
            auto add = [&](int button, float &value, bool &digital) {
                if (button < 0)
                    return;
                for (const PadSource &src : profile.buttons[button])
                {
                    if (src.kind == PadSource::Kind::Button)
                        digital |= s.buttons[src.index];
                    else
                        value = std::max(value, controlValue(s, src));
                }
            };
            add(gas, in.gas, in.gasDigital);
            add(brake, in.brake, in.brakeDigital);
        }
    }

    int playerOf(const std::string &id)
    {
        for (int p = 0; p < kPlayers; ++p)
            for (const std::string &s : g_slots[p])
                if (s == id)
                    return p;
        return -1;
    }

    void initialize()
    {
        SetExitKey(KEY_NULL); // Escape is a game key now; Cmd+Q or the close button quit.
        g_config = loadConfig(paths::dataRoot() / "input.toml");
        const auto db = paths::dataRoot() / "gamecontrollerdb.txt";
        std::error_code ec;
        g_ready = g_devices.initialize(g_config.backgroundInput);
        if (g_ready && std::filesystem::exists(db, ec))
            std::fprintf(stderr, "[input] %d controller mappings from %s\n",
                         SDL_AddGamepadMappingsFromFile(db.string().c_str()), db.string().c_str());
        resolveConfig();
        reassign();
    }

    void blockGameInput(bool blocked) { g_blocked = blocked; }

    void shutdown()
    {
        g_devices.shutdown();
        g_ready = false;
    }

    void update()
    {
        host::beginFrame(); // key edges for this frame (menu, F-keys)
        if (g_devices.update())
            reassign();
        // The window in the background (unless background input is on) or the menu open: the game
        // reads neutral pads (still plugged in) and the motors stop.
        const bool focused = (host::windowFocused() || g_config.backgroundInput) && !g_blocked;

        const bool *keys = SDL_GetKeyboardState(nullptr);
        const bool useKeys = focused && keys && !keyboardBlocked();
        const bool analog = rt::settings::current().analogTriggers;
        for (int p = 0; p < kPlayers; ++p)
        {
            PadOutput out;
            rt::game::AnalogInput an;
            const int gas = rt::game::gasButton(p), brake = rt::game::brakeButton(p);
            for (const std::string &id : g_slots[p])
            {
                if (id == kKeyboardId)
                {
                    if (useKeys)
                    {
                        out = merge(out, mapKeyboard([&](int code) { return keys[code]; }, g_keyboard));
                        for (int code : gas >= 0 ? g_keyboard.buttons[gas] : std::vector<int>{})
                            an.gasDigital |= keys[code];
                        for (int code : brake >= 0 ? g_keyboard.buttons[brake] : std::vector<int>{})
                            an.brakeDigital |= keys[code];
                    }
                    continue;
                }
                for (Device &d : g_devices.list())
                    if (d.id == id && focused)
                    {
                        GamepadSnapshot snap = Devices::snapshot(d);
                        // Back+Start opens the recomp menu: Start does not reach the game meanwhile.
                        if (snap.buttons[static_cast<int>(PadButton::Back)])
                            snap.buttons[static_cast<int>(PadButton::Start)] = false;
                        // A controller's own bindings, else the shared ones in its family's layout.
                        const GamepadProfile profile = g_config.gamepads.count(d.id)
                                                           ? g_config.gamepads.at(d.id)
                                                           : familyProfile(g_config.gamepad, padFamily(d.type));
                        out = merge(out, mapGamepad(snap, profile, d.latch));
                        addAnalog(an, snap, profile, gas, brake);
                    }
            }
            if (!analog)
                an = {};
            rt::game::setAnalogInput(p, an);
            int family = 0;
            for (const std::string &id : g_slots[p])
                for (const Device &d : g_devices.list())
                    if (family == 0 && d.id == id)
                    {
                        const std::string f = padFamily(d.type);
                        family = f == "ps" ? 1 : f == "nintendo" ? 3 : 2;
                    }
            rt::game::setPortFamily(p, family);
            // The runtime hands this to the game at the next guest vblank (unless a movie, script or
            // test client drives the pads; see ps2_test_harness.h).
            ps2_test::setLiveInput(p, {out.buttons, stickByte(out.lx), stickByte(out.ly), stickByte(out.rx),
                                       stickByte(out.ry)});
            ps2_test::setLiveConnected(p, Players::pluggedIn(g_slots, p));
        }
        applyRumble(!focused);

        // Recording (RT_MOVIE_RECORD): F5 marks a moment, F6 a golden frame, F7 the end of a section.
        if (host::keyPressed(SDL_SCANCODE_F5))
            ps2_test::addMarker("mark");
        if (host::keyPressed(SDL_SCANCODE_F6))
            ps2_test::addMarker("golden");
        if (host::keyPressed(SDL_SCANCODE_F7))
            ps2_test::addMarker("section_end");
    }

    namespace detail
    {
        Config &config() { return g_config; }
        Devices &devices() { return g_devices; }
        Players &players() { return g_players; }
        const Players::Slots &slots() { return g_slots; }

        void applyConfig()
        {
            resolveConfig();
            reassign();
        }

        void save() { saveConfig(paths::dataRoot() / "input.toml", g_config); }

        void testRumble(const std::string &id)
        {
            for (const Device &d : g_devices.list())
                if (d.id == id)
                    Devices::rumble(d, 0.8f * g_config.rumble, 0.8f * g_config.rumble, 400);
        }
    }

    MenuInput menuInput()
    {
        enum Action { Up, Down, Left, Right, Confirm, Back, Extra, Toggle, Count };
        bool held[Count] = {};
        bool chord = false;
        for (const Device &d : g_devices.list())
        {
            const GamepadSnapshot s = Devices::snapshot(d);
            auto b = [&](PadButton x) { return s.buttons[static_cast<int>(x)]; };
            const float lx = s.axes[static_cast<int>(PadAxis::LeftX)], ly = s.axes[static_cast<int>(PadAxis::LeftY)];
            held[Up] |= b(PadButton::DpadUp) || ly < -0.6f;
            held[Down] |= b(PadButton::DpadDown) || ly > 0.6f;
            held[Left] |= b(PadButton::DpadLeft) || lx < -0.6f;
            held[Right] |= b(PadButton::DpadRight) || lx > 0.6f;
            // Confirm and back where the pad's players expect them: Nintendo A (right) and B (bottom),
            // Xbox A (bottom) and B (right), PlayStation ✕ (bottom) and △ or ○.
            const std::string family = padFamily(d.type);
            if (family == "nintendo")
            {
                held[Confirm] |= b(PadButton::East);
                held[Back] |= b(PadButton::South);
            }
            else
            {
                held[Confirm] |= b(PadButton::South);
                held[Back] |= b(PadButton::East) || (family == "ps" && b(PadButton::North));
            }
            held[Extra] |= b(PadButton::West);
            held[Toggle] |= b(PadButton::Guide);
            chord |= b(PadButton::Back) && b(PadButton::Start);
        }
        // Back+Start counts once held for 0.3 s, so a quick Select+Start in the game is left alone.
        static double chordSince = -1;
        const double t = host::now();
        if (!chord)
            chordSince = -1;
        else if (chordSince < 0)
            chordSince = t;
        held[Toggle] |= chord && t - chordSince >= 0.3;
        // Edges, with auto-repeat for the directions (after 350 ms, every 90 ms).
        static bool was[Count] = {};
        static double since[Count] = {};
        const double now = host::now();
        bool fire[Count] = {};
        for (int a = 0; a < Count; ++a)
        {
            if (held[a] && !was[a])
            {
                fire[a] = true;
                since[a] = now + 0.35;
            }
            else if (held[a] && a <= Right && now >= since[a])
            {
                fire[a] = true;
                since[a] = now + 0.09;
            }
            was[a] = held[a];
        }
        MenuInput m;
        m.up = fire[Up] || host::keyPressed(SDL_SCANCODE_UP) || host::keyPressedRepeat(SDL_SCANCODE_UP);
        m.down = fire[Down] || host::keyPressed(SDL_SCANCODE_DOWN) || host::keyPressedRepeat(SDL_SCANCODE_DOWN);
        m.left = fire[Left] || host::keyPressed(SDL_SCANCODE_LEFT) || host::keyPressedRepeat(SDL_SCANCODE_LEFT);
        m.right = fire[Right] || host::keyPressed(SDL_SCANCODE_RIGHT) || host::keyPressedRepeat(SDL_SCANCODE_RIGHT);
        m.confirm = fire[Confirm] || host::keyPressed(SDL_SCANCODE_RETURN) || host::keyPressed(SDL_SCANCODE_SPACE);
        m.back = fire[Back] || host::keyPressed(SDL_SCANCODE_BACKSPACE);
        m.extra = fire[Extra] || host::keyPressed(SDL_SCANCODE_R);
        static const bool debug = [] { const char *e = std::getenv("RT_INPUT_DEBUG"); return e && *e == '1'; }();
        static double lastBeat = 0;
        if (debug && now - lastBeat > 1.0)
        {
            lastBeat = now;
            for (const Device &d : g_devices.list())
            {
                SDL_Joystick *j = SDL_GetGamepadJoystick(d.pad);
                std::fprintf(stderr, "[input] beat %s: pad down %d a %d | joy b12 %d b0 %d\n", d.name.c_str(),
                             int(SDL_GetGamepadButton(d.pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)),
                             int(SDL_GetGamepadButton(d.pad, SDL_GAMEPAD_BUTTON_SOUTH)),
                             j ? int(SDL_GetJoystickButton(j, 12)) : -1, j ? int(SDL_GetJoystickButton(j, 0)) : -1);
            }
        }
        if (debug)
            for (int a = 0; a < Count; ++a)
                if (held[a] != was[a] || fire[a])
                    std::fprintf(stderr, "[input] menu action %d held %d fire %d\n", a, int(held[a]), int(fire[a]));
        // Android's Back button / gesture arrives as AC_BACK (SDL_HINT_ANDROID_TRAP_BACK_BUTTON).
        m.toggleMenu = fire[Toggle] || host::keyPressed(SDL_SCANCODE_ESCAPE) || host::keyPressed(SDL_SCANCODE_F3) ||
                       host::keyPressed(SDL_SCANCODE_AC_BACK);
        return m;
    }

    bool anyGamepad() { return !g_devices.list().empty(); }

    std::vector<DeviceStatus> devices()
    {
        std::vector<DeviceStatus> out{{kKeyboardId, "Keyboard", "keyboard", -1, playerOf(kKeyboardId)}};
        for (const Device &d : g_devices.list())
            out.push_back({d.id, d.name, d.type, d.batteryPercent, playerOf(d.id)});
        return out;
    }
}
