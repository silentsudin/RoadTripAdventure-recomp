// Driving controls and feel (see Driving.h).

#include "game/Driving.h"

#include "game/DriveFeel.h"
#include "platform/input/Types.h"
#include "settings/Settings.h"

#include "ps2_runtime.h"
#include "ps2_runtime_macros.h"
#include "runtime/ps2_test_harness.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>

namespace rt::game
{
    namespace
    {
        // [controls] in config/game_state.toml.
        constexpr uint32_t kActionMap = 0x0177F760u + 0x12E8u; // u16 masks, + port * 0x3448
        constexpr uint32_t kPortStride = 0x3448u;
        // Field offsets per action (Horn & Lights has two: +0xC horn on the press, +0x10 lights held).
        constexpr uint32_t kActionField[kDriveActions] = {0x0, 0x2, 0x4, 0x6, 0x8, 0xA, 0xC, 0xE, 0x12};
        constexpr uint32_t kLightsField = 0x10;
        constexpr uint32_t kAllowedTable = 0x0029C538u; // 11 x {u32 mask, x, y, ...} (20 bytes)
        constexpr int kAllowedCount = 11;
        constexpr uint32_t kVibrationSwitch = 0x0033590Eu;
        constexpr uint32_t kPlayerControl = 0x0021A820u, kRumbleStart = 0x0020ACB8u;
        // Control word bits (0x21A820's result).
        constexpr uint32_t kGasBit = 1, kBrakeBit = 2, kReverseBit = 4;
        constexpr int kStillFrames = 20;     // a third of a second stopped on the brake ...
        constexpr uint32_t kStillSpeed = 150; // ... below this speed: reverse
        // The car object (a2 of 0x21A820, a1 of 0x21A310): 0x177AC50 + k * 0x270.
        constexpr uint32_t kCarPort = 0x198, kCarRpm = 0x1D0, kCarSpeed = 0x1D8, kCarBrakeRamp = 0x1FE, kCarGear = 0x1FF;
        // Seven ground contact words (0x2000 road, 0x2353 grass at Peach Raceway, -1 in the air) and
        // a skid level (0..180, while sliding through turns; inferred).
        constexpr uint32_t kCarContacts = 0x19C, kCarSkid = 0x1C4;

        constexpr const char *kNames[kDriveActions] = {"Gas", "Brake", "Reverse", "Jet", "Wing up", "Wing down",
                                                       "Horn & lights", "View", "Navigator"};
        constexpr const char *kKeys[kDriveActions] = {"gas", "brake", "reverse", "jet", "wing_up", "wing_down",
                                                      "horn", "view", "navigator"};
        using B = rt::input::Ps2Button;
        constexpr B kClassic[kDriveActions] = {B::Cross, B::Square, B::Circle, B::R2, B::L1, B::L2, B::R1, B::Triangle, B::Select};
        constexpr B kModern[kDriveActions] = {B::R2, B::L2, B::Square, B::Cross, B::R1, B::L1, B::Circle, B::Triangle, B::Select};

        // The game's words are the DS2 data bytes the other way round from SCE's (ps2Mask).
        uint16_t gameMask(int ps2Button)
        {
            const uint16_t m = rt::input::ps2Mask(static_cast<B>(ps2Button));
            return static_cast<uint16_t>((m >> 8) | (m << 8));
        }

        int buttonOfMask(uint16_t mask)
        {
            for (int b = 0; b < rt::input::kPs2ButtonCount; ++b)
                if (gameMask(b) == mask)
                    return b;
            return -1;
        }

        uint16_t read16(const uint8_t *ram, uint32_t a)
        {
            uint16_t v;
            std::memcpy(&v, ram + (a & PS2_RAM_MASK), 2);
            return v;
        }
        void write16(uint8_t *ram, uint32_t a, uint16_t v) { std::memcpy(ram + (a & PS2_RAM_MASK), &v, 2); }
        uint32_t read32(const uint8_t *ram, uint32_t a)
        {
            uint32_t v;
            std::memcpy(&v, ram + (a & PS2_RAM_MASK), 4);
            return v;
        }

        uint32_t field(int port, DriveAction a) { return kActionMap + port * kPortStride + kActionField[static_cast<int>(a)]; }

        bool allowed(const uint8_t *ram, int ps2Button)
        {
            if (ps2Button < 0)
                return false;
            const uint16_t m = gameMask(ps2Button);
            for (int i = 0; i < kAllowedCount; ++i)
                if (read32(ram, kAllowedTable + i * 20) == m)
                    return true;
            return false;
        }

        // The setup is in memory once every action has one allowed button.
        bool mapValid(const uint8_t *ram, int port)
        {
            for (int a = 0; a < kDriveActions; ++a)
                if (!allowed(ram, buttonOfMask(read16(ram, field(port, static_cast<DriveAction>(a))))))
                    return false;
            return true;
        }

        // Writes a layout into a port's setup where it differs; false if there is no setup yet.
        bool writeLayout(uint8_t *ram, int port, const int (&layout)[kDriveActions])
        {
            if (!ram || port < 0 || port > 1 || !mapValid(ram, port))
                return false;
            for (int a = 0; a < kDriveActions; ++a)
            {
                const uint16_t m = gameMask(layout[a]);
                if (read16(ram, field(port, static_cast<DriveAction>(a))) != m)
                    write16(ram, field(port, static_cast<DriveAction>(a)), m);
            }
            write16(ram, kActionMap + port * kPortStride + kLightsField, gameMask(layout[static_cast<int>(DriveAction::HornLights)]));
            return true;
        }

        std::atomic<int> g_portFamily[2] = {2, 2};

        // The scheme in use: the setting, or RT_CONTROL_SCHEME (-1: the game's own, untouched).
        int currentScheme()
        {
            static const int forced = [] {
                const char *e = std::getenv("RT_CONTROL_SCHEME");
                if (!e || !*e)
                    return -1;
                return std::strcmp(e, "modern") == 0 ? 0 : std::strcmp(e, "classic") == 0 ? 1 : 2;
            }();
            if (forced >= 0)
                return forced;
            return controlSchemeActive() ? static_cast<int>(rt::settings::current().controlScheme) : -1;
        }

        // The scheme a port plays with: Modern needs a gamepad.
        int portScheme(int port)
        {
            const int scheme = currentScheme();
            return scheme == 0 && g_portFamily[port] == 0 ? 1 : scheme;
        }

        // The wing set's dialogue (the only text naming driving buttons): "Furthermore, press the L1
        // button\nto get more flying distance,\nand press the L2 button to reduce it.\n..." ending
        // in its control codes 0C 06 00, 186 bytes of text.
        constexpr uint32_t kWingText = 0x00326EADu;
        constexpr size_t kWingTextLen = 186;

        // A game button's name on a family's pad (as the family's layout puts it, Mapping.h).
        const char *buttonName(int ps2Button, int family)
        {
            static const char *ps[] = {"Up", "Down", "Left", "Right", "Cross", "Circle", "Square", "Triangle",
                                       "L1", "R1", "L2", "R2", "START", "SELECT", "L3", "R3"};
            static const char *xbox[] = {"Up", "Down", "Left", "Right", "A", "Y", "X", "B",
                                         "LB", "RB", "LT", "RT", "Menu", "View", "LS", "RS"};
            static const char *nin[] = {"Up", "Down", "Left", "Right", "A", "X", "Y", "B",
                                        "L", "R", "ZL", "ZR", "+", "-", "LS", "RS"};
            if (ps2Button < 0 || ps2Button >= rt::input::kPs2ButtonCount)
                return "?";
            return family == 2 ? xbox[ps2Button] : family == 3 ? nin[ps2Button] : ps[ps2Button];
        }

        void patchWingText(uint8_t *ram, int up, int down, int family)
        {
            static int lastKey = -1;
            static std::string body;
            char *text = reinterpret_cast<char *>(ram + kWingText);
            const int key = (up << 8) | (down << 4) | family;
            if (key != lastKey)
            {
                body = std::string("Furthermore, press the ") + buttonName(up, family) +
                       " button\nto get more flying distance,\nand press the " + buttonName(down, family) +
                       " button to reduce it.\nBe sure to use\nthe wing set underwater or you won't\n"
                       "be able to change the direction!";
                if (body.size() > kWingTextLen)
                    body.replace(0, std::strlen("Furthermore, press"), "Also, press");
                lastKey = key;
            }
            // Only over the game's own text or ours (a save state may bring either back).
            if (body.size() > kWingTextLen || std::memcmp(text, body.data(), body.size()) == 0)
                return;
            if (std::strncmp(text, "Furthermore, press the ", 23) != 0 && std::strncmp(text, "Also, press the ", 16) != 0)
                return;
            std::memset(text, 0, kWingTextLen + 3);
            std::memcpy(text, body.data(), body.size());
            text[body.size()] = 0x0C;
            text[body.size() + 1] = 0x06;
        }

        // Modern: braking at a standstill for a moment reverses, until the brake is let go.
        int g_still[2] = {};
        bool g_reversing[2] = {};

        // ------------------------------------------------------------ analogue
        std::mutex g_mutex;
        AnalogInput g_analog[2];
        bool g_testAnalog[2] = {};        // the test socket's values replace the input layer's
        std::atomic<int> g_gasButton[2] = {-1, -1}, g_brakeButton[2] = {-1, -1};
        feel::Throttle g_throttle[2];

        // ------------------------------------------------------------ vibration
        feel::Synth g_synth[2];
        RumbleOut g_rumble[2];
        uint64_t g_driveVblank[2] = {};
        bool g_trace = false;

        PS2Runtime::RecompiledFunction g_playerControl = nullptr, g_rumbleStart = nullptr;

        // 0x21A820(a0 task, a1 sys, a2 car) -> v0 the car's control word for this frame.
        void playerControl(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
        {
            const uint32_t sys = GPR_U32(ctx, 5), car = GPR_U32(ctx, 6), ra = GPR_U32(ctx, 31);
            const int port = read16(rdram, car + kCarPort) & 1;
            // The control scheme's layout, so the game reads (and saves) it.
            const int scheme = portScheme(port);
            if (scheme >= 0)
            {
                int layout[kDriveActions];
                schemeLayout(scheme, layout);
                writeLayout(rdram, port, layout);
            }
            if (port == 0 && scheme >= 0)
                patchWingText(rdram, buttonOfMask(read16(rdram, field(0, DriveAction::WingUp))),
                              buttonOfMask(read16(rdram, field(0, DriveAction::WingDown))), g_portFamily[0] == 0 ? 1 : g_portFamily[0].load());
            g_playerControl(rdram, ctx, runtime);
            if (ctx->pc != ra) // paused inside a call (the horn's sound request); left as it is
                return;
            uint32_t word = GPR_U32(ctx, 2);
            g_gasButton[port] = buttonOfMask(read16(rdram, field(port, DriveAction::Gas)));
            g_brakeButton[port] = buttonOfMask(read16(rdram, field(port, DriveAction::Brake)));

            const uint32_t speed = read32(rdram, car + kCarSpeed);
            AnalogInput in;
            const bool analog = analogTriggersActive();
            if (analog)
            {
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    in = g_analog[port];
                }
                if (in.gas > feel::kTriggerRest && !(word & kReverseBit))
                {
                    // A button on Gas as well, or the trigger all the way down: full gas.
                    const bool full = in.gasDigital || in.gas >= feel::kTriggerFull;
                    if (full)
                        g_throttle[port].full(speed);
                    const bool on = full || g_throttle[port].step(in.gas, speed);
                    word = on ? (word | kGasBit) : (word & ~kGasBit);
                }
                else if (word & kGasBit)
                    g_throttle[port].full(speed); // the game's own full gas: learn the top speed
                if (in.brake > feel::kTriggerRest)
                {
                    word |= kBrakeBit;
                    // The ramp goes up by one in 0x21A310 after this: hold it at the limit.
                    const int limit = feel::brakeRampLimit(in.brake);
                    uint8_t &ramp = rdram[(car + kCarBrakeRamp) & PS2_RAM_MASK];
                    if (ramp >= limit)
                        ramp = static_cast<uint8_t>(limit - 1);
                }
            }
            // Modern and Custom: hold the brake at a standstill to reverse (gas pulsed by how far it is
            // pressed); it belongs to the brake, wherever that is.
            if (scheme == 0 || scheme == 2)
            {
                const bool brake = word & kBrakeBit, gas = word & kGasBit;
                if (g_reversing[port] && (!brake || gas))
                    g_reversing[port] = false;
                else if (!g_reversing[port])
                {
                    g_still[port] = brake && !gas && speed < kStillSpeed ? g_still[port] + 1 : 0;
                    g_reversing[port] = g_still[port] >= kStillFrames;
                }
                if (g_reversing[port])
                {
                    const bool push = !analog || in.brake <= feel::kTriggerRest || in.brakeDigital ||
                                      in.brake >= feel::kTriggerFull || g_throttle[port].pwm.step(feel::gasDuty(in.brake));
                    word = (word & ~(kBrakeBit | kGasBit)) | kReverseBit | (push ? kGasBit : 0);
                }
            }
            else
                g_reversing[port] = false, g_still[port] = 0;
            SET_GPR_U32(ctx, 2, word);

            // Vibration from the car.
            feel::CarSample s;
            s.rpm = read16(rdram, car + kCarRpm);
            s.speed = speed;
            s.gear = rdram[(car + kCarGear) & PS2_RAM_MASK];
            s.brakeRamp = rdram[(car + kCarBrakeRamp) & PS2_RAM_MASK];
            s.gas = word & kGasBit;
            s.brake = word & kBrakeBit;
            s.reverse = word & kReverseBit;
            s.airborne = true;
            for (uint32_t o = 0; o < 7; ++o)
                s.airborne &= read32(rdram, car + kCarContacts + 4 * o) == 0xFFFFFFFFu;
            s.skid = rdram[(car + kCarSkid) & PS2_RAM_MASK] / 180.0f;
            std::lock_guard<std::mutex> lock(g_mutex);
            const feel::Motors m = g_synth[port].step(s);
            // The game's own gate: the pad's mode byte and the vibration switch.
            if (rdram[(sys + 0xC + port) & PS2_RAM_MASK] & rdram[kVibrationSwitch])
                g_rumble[port] = {m.low, m.high, m.left, m.right};
            else
                g_rumble[port] = {};
            g_driveVblank[port] = ps2_test::currentVblank();
        }

        // 0x20ACB8(a0 sys, a1 port, a2 pattern, a3 strength): the game starts a vibration script.
        void rumbleStart(uint8_t *rdram, R5900Context *ctx, PS2Runtime *runtime)
        {
            const uint32_t sys = GPR_U32(ctx, 4), port = GPR_U32(ctx, 5) & 1;
            const int pattern = GPR_S32(ctx, 6), strength = GPR_S32(ctx, 7);
            if (g_trace)
                std::fprintf(stderr, "[rumble] vblank %llu port %u pattern %d strength %d from 0x%X\n",
                             static_cast<unsigned long long>(ps2_test::currentVblank()), port, pattern, strength,
                             GPR_U32(ctx, 31) - 8);
            // The game's own gate: the pad's mode byte and its switch.
            if (rdram[(sys + 0xC + port) & PS2_RAM_MASK] & rdram[kVibrationSwitch])
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                g_synth[port].event(pattern, strength);
            }
            g_rumbleStart(rdram, ctx, runtime);
        }

        float jsonFloat(const std::string &line, const char *key, float fallback)
        {
            const std::string v = ps2_test::jsonField(line, key);
            return v.empty() ? fallback : std::strtof(v.c_str(), nullptr);
        }
    }

    const char *driveActionName(DriveAction a) { return kNames[static_cast<int>(a)]; }
    const char *driveActionKey(DriveAction a) { return kKeys[static_cast<int>(a)]; }

    int actionButton(PS2Runtime &runtime, int port, DriveAction a)
    {
        const uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram || port < 0 || port > 1 || !mapValid(ram, port))
            return -1;
        return buttonOfMask(read16(ram, field(port, a)));
    }

    bool actionButtonAllowed(PS2Runtime &runtime, int ps2Button)
    {
        const uint8_t *ram = runtime.memory().getRDRAM();
        return ram && allowed(ram, ps2Button);
    }

    bool setActionButton(PS2Runtime &runtime, int port, DriveAction a, int ps2Button)
    {
        uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram || port < 0 || port > 1 || !mapValid(ram, port) || !allowed(ram, ps2Button))
            return false;
        const uint16_t mine = read16(ram, field(port, a)), want = gameMask(ps2Button);
        for (int o = 0; o < kDriveActions; ++o)
            if (o != static_cast<int>(a) && read16(ram, field(port, static_cast<DriveAction>(o))) == want)
                write16(ram, field(port, static_cast<DriveAction>(o)), mine);
        write16(ram, field(port, a), want);
        // Horn & Lights is one action with two fields.
        write16(ram, kActionMap + port * kPortStride + kLightsField, read16(ram, field(port, DriveAction::HornLights)));
        return true;
    }

    bool controlSchemeActive()
    {
        static const bool deterministic = [] {
            const char *t = std::getenv("RT_TIME");
            const char *rec = std::getenv("RT_MOVIE_RECORD");
            return (t && std::strcmp(t, "virtual") == 0) || (rec && *rec);
        }();
        return !deterministic && !ps2_test::inputScripted();
    }

    void schemeLayout(int scheme, int (&buttons)[kDriveActions])
    {
        for (int a = 0; a < kDriveActions; ++a)
            buttons[a] = static_cast<int>(scheme == 0 ? kModern[a] : kClassic[a]);
        if (scheme != 2)
            return;
        // Custom: the saved names, if all nine are there and different.
        std::istringstream in(rt::settings::current().customControls);
        int got[kDriveActions], n = 0;
        for (std::string w; n < kDriveActions && in >> w;)
        {
            const auto b = rt::input::ps2FromName(w);
            if (!b)
                return;
            got[n++] = static_cast<int>(*b);
        }
        if (n != kDriveActions)
            return;
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                if (got[i] == got[j])
                    return;
        std::memcpy(buttons, got, sizeof(got));
    }

    bool applyControlScheme(PS2Runtime &runtime)
    {
        uint8_t *ram = runtime.memory().getRDRAM();
        bool ok = false;
        for (int port = 0; port < 2; ++port)
        {
            const int scheme = portScheme(port);
            if (scheme < 0)
                continue;
            int layout[kDriveActions];
            schemeLayout(scheme, layout);
            ok |= writeLayout(ram, port, layout);
        }
        return ok;
    }

    std::string layoutText(PS2Runtime &runtime)
    {
        std::string out;
        for (int a = 0; a < kDriveActions; ++a)
        {
            const int b = actionButton(runtime, 0, static_cast<DriveAction>(a));
            if (b < 0)
                return {};
            out += (a ? " " : "") + std::string(rt::input::ps2Name(static_cast<B>(b)));
        }
        return out;
    }

    void setPortFamily(int port, int family)
    {
        if (port >= 0 && port < 2)
            g_portFamily[port] = family;
    }

    void setAnalogInput(int port, const AnalogInput &in)
    {
        if (port < 0 || port > 1)
            return;
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_testAnalog[port])
            g_analog[port] = in;
    }

    bool analogTriggersActive()
    {
        static const int forced = [] {
            const char *e = std::getenv("RT_ANALOG_TRIGGERS");
            return e && *e ? (*e == '1' ? 1 : 0) : -1;
        }();
        if (forced >= 0)
            return forced == 1;
        // Movies, scripts and lockstep tests replay buttons only: analogue values would make them
        // play differently.
        return controlSchemeActive() && rt::settings::current().analogTriggers;
    }

    int gasButton(int port) { return port >= 0 && port < 2 ? g_gasButton[port].load() : -1; }
    int brakeButton(int port) { return port >= 0 && port < 2 ? g_brakeButton[port].load() : -1; }

    bool dynamicRumble(int port, RumbleOut &out)
    {
        if (port < 0 || port > 1)
            return false;
        std::lock_guard<std::mutex> lock(g_mutex);
        const uint64_t now = ps2_test::currentVblank();
        if (now > g_driveVblank[port] + 6 || g_driveVblank[port] == 0)
            return false;
        out = g_rumble[port];
        return true;
    }

    void installDrivingHooks(PS2Runtime &runtime)
    {
        const char *trace = std::getenv("RT_RUMBLE_TRACE");
        g_trace = trace && *trace == '1';
        g_playerControl = runtime.lookupFunction(kPlayerControl);
        g_rumbleStart = runtime.lookupFunction(kRumbleStart);
        if (g_playerControl)
            runtime.replaceFunction(kPlayerControl, playerControl);
        else
            std::cerr << "[roadtrip] player controller 0x21A820 not found: no analogue triggers or dynamic vibration\n";
        if (g_rumbleStart)
            runtime.replaceFunction(kRumbleStart, rumbleStart);
        else
            std::cerr << "[roadtrip] rumble start 0x20ACB8 not found: dynamic vibration without the game's knocks\n";
    }

    std::string drivingCommand(PS2Runtime &runtime, const std::string &line)
    {
        const std::string portText = ps2_test::jsonField(line, "port");
        const int port = portText.empty() ? 0 : std::atoi(portText.c_str()) & 1;
        bool ok = true;
        const std::string action = ps2_test::jsonField(line, "action");
        if (!action.empty())
        {
            const auto button = rt::input::ps2FromName(ps2_test::jsonField(line, "button"));
            ok = false;
            for (int a = 0; a < kDriveActions; ++a)
                if (action == kKeys[a] && button)
                    ok = setActionButton(runtime, port, static_cast<DriveAction>(a), static_cast<int>(*button));
        }
        const std::string scheme = ps2_test::jsonField(line, "scheme");
        if (!scheme.empty())
        {
            const int i = scheme == "modern" ? 0 : scheme == "classic" ? 1 : 2;
            int layout[kDriveActions];
            schemeLayout(i, layout);
            ok = writeLayout(runtime.memory().getRDRAM(), port, layout);
        }
        const std::string period = ps2_test::jsonField(line, "pwm_period");
        if (!period.empty())
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (feel::Throttle &t : g_throttle)
                t.pwm = feel::Pwm{0, std::max(1, std::atoi(period.c_str()))};
        }
        const std::string analog = ps2_test::jsonField(line, "analog");
        if (!analog.empty())
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_testAnalog[port] = analog == "1";
            g_analog[port] = {jsonFloat(line, "gas", 0), jsonFloat(line, "brake", 0), false, false};
        }
        std::ostringstream o;
        o << "{\"ok\":" << (ok ? "true" : "false") << ",\"analog_active\":" << (analogTriggersActive() ? "true" : "false")
          << ",\"scheme\":" << currentScheme()
          << ",\"ports\":[";
        for (int p = 0; p < 2; ++p)
        {
            o << (p ? "," : "") << "{\"actions\":{";
            for (int a = 0; a < kDriveActions; ++a)
            {
                const int b = actionButton(runtime, p, static_cast<DriveAction>(a));
                o << (a ? "," : "") << "\"" << kKeys[a] << "\":\"" << (b >= 0 ? rt::input::ps2Name(static_cast<B>(b)) : "") << "\"";
            }
            AnalogInput in;
            RumbleOut r;
            bool driving;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                in = g_analog[p];
            }
            driving = dynamicRumble(p, r);
            o << "},\"gas\":" << in.gas << ",\"brake\":" << in.brake << ",\"driving\":" << (driving ? "true" : "false")
              << ",\"reversing\":" << (g_reversing[p] ? "true" : "false") << ",\"rumble\":[" << r.low << "," << r.high << "," << r.left << "," << r.right << "]}";
        }
        o << "]}";
        return o.str();
    }
}
