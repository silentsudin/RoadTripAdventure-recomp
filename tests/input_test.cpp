// The input layer without the game: mapping, player assignment, config migration, and a virtual
// SDL3 controller for hotplug, buttons and rumble.

#include "platform/input/Config.h"
#include "platform/input/Devices.h"
#include "platform/input/Mapping.h"
#include "platform/input/Players.h"
#include "platform/input/SystemVibrator.h"

#include <SDL3/SDL.h>

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

using namespace rt::input;

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

    bool pressed(const PadOutput &o, Ps2Button b) { return (o.buttons & ps2Mask(b)) == 0; }

    void mapping()
    {
        StickSettings s;
        float x = 0.1f, y = 0.0f;
        applyDeadzone(x, y, s);
        CHECK(x == 0 && y == 0);
        x = 1.0f, y = 0.0f;
        applyDeadzone(x, y, s);
        CHECK(std::fabs(x - 1.0f) < 1e-5f);
        x = 0.55f, y = 0.0f;
        applyDeadzone(x, y, s);
        CHECK(x > 0.45f && x < 0.55f); // scaled between the inner and outer deadzones

        CHECK(stickByte(0) == 128 && stickByte(-1) == 0 && stickByte(1) == 255);

        const GamepadProfile p = defaultGamepadProfile();
        AxisLatch latch;
        GamepadSnapshot pad;
        pad.buttons[static_cast<int>(PadButton::South)] = true;
        PadOutput o = mapGamepad(pad, p, latch);
        CHECK(pressed(o, Ps2Button::Cross) && !pressed(o, Ps2Button::Circle));

        // Family layouts: confirm (✕) and back (△) where each pad's players expect them.
        auto press = [&](const GamepadProfile &prof, PadButton b) {
            GamepadSnapshot snap;
            snap.buttons[static_cast<int>(b)] = true;
            AxisLatch l;
            return mapGamepad(snap, prof, l);
        };
        CHECK(padFamily("ps5") == "ps" && padFamily("switchpro") == "nintendo" && padFamily("standard") == "xbox");
        const GamepadProfile xbox = familyProfile(p, "xbox"), nin = familyProfile(p, "nintendo");
        CHECK(pressed(press(xbox, PadButton::South), Ps2Button::Cross));    // A
        CHECK(pressed(press(xbox, PadButton::East), Ps2Button::Triangle));  // B = back
        CHECK(pressed(press(xbox, PadButton::North), Ps2Button::Circle));   // Y
        CHECK(pressed(press(xbox, PadButton::West), Ps2Button::Square));    // X
        CHECK(pressed(press(nin, PadButton::East), Ps2Button::Cross));      // A
        CHECK(pressed(press(nin, PadButton::South), Ps2Button::Triangle));  // B = back
        CHECK(pressed(press(nin, PadButton::North), Ps2Button::Circle));    // X
        CHECK(pressed(press(familyProfile(p, "ps"), PadButton::North), Ps2Button::Triangle)); // unchanged

        // Trigger hysteresis: presses past 0.35, lets go below 0.25.
        auto r2 = [&](float v) {
            GamepadSnapshot t;
            t.axes[static_cast<int>(PadAxis::RightTrigger)] = v;
            return pressed(mapGamepad(t, p, latch), Ps2Button::R2);
        };
        CHECK(!r2(0.3f));
        CHECK(r2(0.4f));
        CHECK(r2(0.3f));
        CHECK(!r2(0.2f));

        PadOutput a, b;
        a.buttons &= static_cast<uint16_t>(~ps2Mask(Ps2Button::Start));
        a.lx = 0.2f;
        b.lx = -0.8f;
        const PadOutput m = merge(a, b);
        CHECK(pressed(m, Ps2Button::Start) && m.lx == -0.8f);
    }

    void players()
    {
        Players pl;
        auto slots = pl.assign({kKeyboardId, "padA", "padB", "padC"});
        CHECK(slots[0] == (std::vector<std::string>{kKeyboardId, "padA"}));
        CHECK(slots[1] == std::vector<std::string>{"padB"});
        CHECK(Players::pluggedIn(slots, 1));
        // Player 2's pad (and the spare padC) go away: port 2 is empty, i.e. unplugged.
        slots = pl.assign({kKeyboardId, "padA"});
        CHECK(slots[1].empty() && !Players::pluggedIn(slots, 1) && Players::pluggedIn(slots, 0));
        // padB comes back: its old slot.
        slots = pl.assign({kKeyboardId, "padA", "padB"});
        CHECK(slots[1] == std::vector<std::string>{"padB"});
        // Manual choice wins.
        pl.assignManually("padA", 1);
        slots = pl.assign({kKeyboardId, "padA", "padB"});
        CHECK(slots[1].size() == 2 && slots[0] == std::vector<std::string>{kKeyboardId});
    }

    void config()
    {
        const auto dir = std::filesystem::temp_directory_path() / "rt_input_test";
        std::filesystem::remove_all(dir);
        std::filesystem::create_directories(dir);
        const auto path = dir / "input.toml";
        std::ofstream(path) << "cross = \"KEY_X KEY_SPACE PAD_RIGHT_FACE_DOWN\"\n"
                               "l2 = \"KEY_ONE PAD_LEFT_TRIGGER_2\"\n"
                               "lstick_up = \"KEY_W\"\n"
                               "deadzone = 0.3\n";
        const Config c = loadConfig(path);
        const int cross = static_cast<int>(Ps2Button::Cross);
        CHECK(c.keyButtons[cross] == (std::vector<std::string>{"X", "Space"}));
        CHECK(c.gamepad.buttons[cross].size() == 1 && padSourceName(c.gamepad.buttons[cross][0]) == "south");
        CHECK(padSourceName(c.gamepad.buttons[static_cast<int>(Ps2Button::L2)][0]) == "lefttrigger");
        CHECK(c.keyStickDirs[static_cast<int>(StickDir::LUp)] == std::vector<std::string>{"W"});
        CHECK(std::fabs(c.gamepad.left.inner - 0.3f) < 1e-5f);
        CHECK(std::filesystem::exists(std::filesystem::path(path).concat(".v1.bak")));

        // Version 2 round trip, with a per-controller override and a manual player.
        Config v2 = c;
        v2.rumble = 0.5f;
        v2.players["padX"] = 1;
        GamepadProfile own = v2.gamepad;
        own.buttons[cross] = {*padSourceFromName("east")};
        own.rumble = 0.25f;
        v2.gamepads["padX"] = own;
        saveConfig(path, v2);
        const Config back = loadConfig(path);
        CHECK(std::fabs(back.rumble - 0.5f) < 1e-5f);
        CHECK(back.players.at("padX") == 1);
        CHECK(padSourceName(back.profileFor("padX").buttons[cross][0]) == "east");
        CHECK(std::fabs(back.profileFor("padX").rumble - 0.25f) < 1e-5f);
        CHECK(padSourceName(back.profileFor("other").buttons[cross][0]) == "south");
        std::filesystem::remove_all(dir);
    }

    struct RumbleLog
    {
        Uint16 low = 0, high = 0;
        int calls = 0;
    };

    bool onRumble(void *user, Uint16 low, Uint16 high)
    {
        auto *log = static_cast<RumbleLog *>(user);
        log->low = low;
        log->high = high;
        ++log->calls;
        return true;
    }

    void virtualController()
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        Devices devices;
        CHECK(devices.initialize(false));
        devices.update();
        const size_t before = devices.list().size();

        RumbleLog log;
        SDL_VirtualJoystickDesc desc;
        SDL_INIT_INTERFACE(&desc);
        desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
        desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
        desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
        desc.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1u;
        desc.button_mask = (1u << SDL_GAMEPAD_BUTTON_COUNT) - 1u;
        desc.name = "Virtual DualSense";
        desc.Rumble = onRumble;
        desc.userdata = &log;
        const SDL_JoystickID id = SDL_AttachVirtualJoystick(&desc);
        CHECK(id != 0);
        SDL_UpdateJoysticks();
        SDL_PumpEvents();
        CHECK(devices.update());
        CHECK(devices.list().size() == before + 1);
        const Device &d = devices.list().back();
        CHECK(d.name == "Virtual DualSense");

        SDL_Joystick *joy = SDL_GetJoystickFromID(id);
        SDL_SetJoystickVirtualButton(joy, SDL_GAMEPAD_BUTTON_SOUTH, true);
        SDL_SetJoystickVirtualAxis(joy, SDL_GAMEPAD_AXIS_LEFTX, 32767);
        SDL_UpdateJoysticks();
        Device &dev = devices.list().back();
        const PadOutput o = mapGamepad(Devices::snapshot(dev), defaultGamepadProfile(), dev.latch);
        CHECK(pressed(o, Ps2Button::Cross));
        CHECK(stickByte(o.lx) == 255);

        Devices::rumble(dev, 1.0f, 0.5f, 250);
        CHECK(log.calls >= 1 && log.low == 65535 && log.high > 30000 && log.high < 35000);

        SDL_DetachVirtualJoystick(id);
        SDL_PumpEvents();
        CHECK(devices.update());
        CHECK(devices.list().size() == before);
        devices.shutdown();
        SDL_Quit();
    }
    // The device's vibrator for pads without motors: the mix and how often it is called.
    void systemVibrator()
    {
        using namespace rt::input::systemvibrator::detail;
        CHECK(mixAmplitude(0, 0, 0, 0) == 0);
        CHECK(mixAmplitude(1, 1, 1, 1) == 248 || mixAmplitude(1, 1, 1, 1) == 255);
        CHECK(mixAmplitude(0.5f, 0, 0, 0) > mixAmplitude(0.25f, 0, 0, 0));
        CHECK(mixAmplitude(0.2f, 0, 0, 0) > 51);                           // the curve lifts low levels
        CHECK(mixAmplitude(0, 0.5f, 0, 0) < mixAmplitude(0.5f, 0, 0, 0));  // the small motor weighs less
        CHECK(mixAmplitude(0.3f, 0.3f, 0, 0) > mixAmplitude(0.3f, 0, 0, 0));
        CHECK(mixAmplitude(0, 0, 0, 0.5f) > 0);                            // the trigger motors count
        CHECK(mixAmplitude(0.3f, 0, 0, 0) % 8 == 0);

        // Cruising: the caller sends 250 ms pulses every 100 ms (input frames every 16 ms,
        // tick() between), the level flickering between 120 and 200 every 50 ms.
        auto cruise = [](bool flicker) {
            Limiter l;
            int sends = 0;
            int64_t until = 0;
            for (int64_t t = 0; t < 2000; t += 16)
            {
                if (t % 96 == 0)
                    until = t + 250;
                const int amp = flicker ? ((t / 48) % 2 ? 200 : 120) : 96;
                sends += l.decide(amp, t, until).send;
            }
            return sends;
        };
        const int steady = cruise(false), flickering = cruise(true);
        CHECK(steady >= 9 && steady <= 12);       // ~every 200 ms
        CHECK(flickering <= 16);                  // <= 8 a second
        std::printf("system vibrator: %d sends in 2 s steady, %d flickering\n", steady, flickering);

        Limiter l;
        auto d = l.decide(96, 0, 250);
        CHECK(d.send && d.amplitude == 96 && d.durationMs == 300); // a start at once (50 ms past the pulse)
        CHECK(!l.decide(200, 20, 250).send);                       // a rise waits 40 ms
        d = l.decide(200, 40, 250);
        CHECK(d.send && d.amplitude == 200 && d.durationMs == 260);
        CHECK(!l.decide(120, 60, 250).send);                       // a fall must last 150 ms
        CHECK(!l.decide(120, 200, 250).send);
        d = l.decide(120, 210, 250);
        CHECK(d.send && d.amplitude == 120);
        CHECK(!l.decide(128, 220, 250).send);                      // a small change: 300 ms
        CHECK(!l.decide(128, 240, 600).send);                      // no renewal 30 ms after a send
        d = l.decide(128, 520, 600);
        CHECK(d.send && d.amplitude == 128);
        d = l.decide(0, 530, 600);
        CHECK(d.send && d.amplitude == 0 && l.sentAmp == 0);       // a stop at once
        CHECK(!l.decide(0, 540, 600).send);                        // once
        CHECK(!l.decide(96, 700, 600).send);                       // nothing past the pulse's end
    }
}

int main()
{
    mapping();
    players();
    config();
    virtualController();
    systemVibrator();
    if (g_failures)
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
    else
        std::printf("input tests passed\n");
    return g_failures ? 1 : 0;
}
