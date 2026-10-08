#include "Devices.h"
#include "SystemVibrator.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdio>
#include <map>

namespace rt::input
{
    static_assert(kPadButtonCount == SDL_GAMEPAD_BUTTON_COUNT, "PadButton follows SDL_GamepadButton");
    static_assert(static_cast<int>(PadButton::South) == SDL_GAMEPAD_BUTTON_SOUTH);
    static_assert(static_cast<int>(PadButton::Touchpad) == SDL_GAMEPAD_BUTTON_TOUCHPAD);
    static_assert(kPadAxisCount == SDL_GAMEPAD_AXIS_COUNT, "PadAxis follows SDL_GamepadAxis");
    static_assert(static_cast<int>(PadAxis::RightTrigger) == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    namespace
    {
        const char *typeName(SDL_GamepadType t)
        {
            switch (t)
            {
            case SDL_GAMEPAD_TYPE_XBOX360: return "xbox360";
            case SDL_GAMEPAD_TYPE_XBOXONE: return "xboxone";
            case SDL_GAMEPAD_TYPE_PS3: return "ps3";
            case SDL_GAMEPAD_TYPE_PS4: return "ps4";
            case SDL_GAMEPAD_TYPE_PS5: return "ps5";
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO: return "switchpro";
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
            case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return "joycon";
            case SDL_GAMEPAD_TYPE_STANDARD: return "standard";
            default: return "unknown";
            }
        }

        float axis(SDL_Gamepad *pad, SDL_GamepadAxis a)
        {
            const float v = SDL_GetGamepadAxis(pad, a) / 32767.0f;
            return std::clamp(v, -1.0f, 1.0f);
        }
    }

    bool Devices::onEvent(void *self, SDL_Event *event)
    {
        if (event->type == SDL_EVENT_GAMEPAD_ADDED || event->type == SDL_EVENT_GAMEPAD_REMOVED)
            static_cast<Devices *>(self)->m_dirty.store(true);
        return true;
    }

    bool Devices::initialize(bool backgroundInput)
    {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, backgroundInput ? "1" : "0");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");
        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC))
        {
            std::fprintf(stderr, "[input] no controller support: %s\n", SDL_GetError());
            return false;
        }
        SDL_AddEventWatch(&Devices::onEvent, this);
        m_ready = true;
        return true;
    }

    void Devices::shutdown()
    {
        if (!m_ready)
            return;
        systemvibrator::stop();
        for (Device &d : m_devices)
        {
            SDL_RumbleGamepad(d.pad, 0, 0, 0);
            SDL_CloseGamepad(d.pad);
        }
        m_devices.clear();
        SDL_RemoveEventWatch(&Devices::onEvent, this);
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);
        m_ready = false;
    }

    bool Devices::update()
    {
        if (!m_ready)
            return false;
        // Also poll now and then: some platforms report hotplug only through the joystick layer.
        systemvibrator::tick();
        static int frames = 0;
        if (!m_dirty.exchange(false) && ++frames % 120 != 0)
        {
            for (Device &d : m_devices)
            {
                int percent = -1;
                if (SDL_GetGamepadPowerInfo(d.pad, &percent) == SDL_POWERSTATE_ON_BATTERY)
                    d.batteryPercent = percent;
            }
            return false;
        }
        int count = 0;
        SDL_JoystickID *ids = SDL_GetGamepads(&count);
        bool changed = false;
        // Drop controllers that went away.
        for (auto it = m_devices.begin(); it != m_devices.end();)
        {
            const bool present = std::find(ids, ids + count, it->instance) != ids + count;
            if (present && SDL_GamepadConnected(it->pad))
            {
                ++it;
                continue;
            }
            std::fprintf(stderr, "[input] disconnected: %s\n", it->name.c_str());
            SDL_CloseGamepad(it->pad);
            it = m_devices.erase(it);
            changed = true;
        }
        // Open new ones (in SDL's order, which is connection order).
        for (int i = 0; i < count; ++i)
        {
            const SDL_JoystickID instance = ids[i];
            if (std::any_of(m_devices.begin(), m_devices.end(), [&](const Device &d) { return d.instance == instance; }))
                continue;
            SDL_Gamepad *pad = SDL_OpenGamepad(instance);
            if (!pad)
                continue;
            Device d;
            d.instance = instance;
            d.pad = pad;
            d.name = SDL_GetGamepadName(pad) ? SDL_GetGamepadName(pad) : "Controller";
            d.type = typeName(SDL_GetGamepadType(pad));
            char guid[64];
            SDL_GUIDToString(SDL_GetGamepadGUIDForID(instance), guid, sizeof(guid));
            const char *serial = SDL_GetGamepadSerial(pad);
            if (serial && *serial)
                d.id = std::string(guid) + ":" + serial;
            else
            {
                // Identical controllers without serials: number them by position.
                int same = 0;
                for (const Device &o : m_devices)
                    same += o.id.rfind(guid, 0) == 0;
                d.id = std::string(guid) + "#" + std::to_string(same + 1);
            }
            // A pad without motors (the AYN Thor's built-in one) vibrates the device instead.
            const bool motors = SDL_GetBooleanProperty(SDL_GetGamepadProperties(pad), SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false);
            d.systemVibrator = (!motors || systemvibrator::forced()) && systemvibrator::available();
            std::fprintf(stderr, "[input] connected: %s (%s, %s)%s\n", d.name.c_str(), d.type.c_str(), d.id.c_str(),
                         d.systemVibrator ? ", vibrates the device" : motors ? "" : ", no vibration");
            if (const char *dbg = std::getenv("RT_INPUT_DEBUG"); dbg && *dbg == '1')
                if (char *mapping = SDL_GetGamepadMapping(d.pad))
                {
                    std::fprintf(stderr, "[input]   mapping: %s\n", mapping);
                    SDL_free(mapping);
                }
            m_devices.push_back(std::move(d));
            changed = true;
        }
        SDL_free(ids);
        return changed;
    }

    GamepadSnapshot Devices::snapshot(const Device &d)
    {
        GamepadSnapshot s;
        for (int b = 0; b < kPadButtonCount; ++b)
            s.buttons[b] = SDL_GetGamepadButton(d.pad, static_cast<SDL_GamepadButton>(b));
        for (int a = 0; a < kPadAxisCount; ++a)
            s.axes[a] = axis(d.pad, static_cast<SDL_GamepadAxis>(a));
        return s;
    }

    void Devices::rumble(const Device &d, float low, float high, uint32_t ms)
    {
        if (d.systemVibrator)
            return systemvibrator::motors(low, high, ms);
        auto u16 = [](float v) { return static_cast<Uint16>(std::clamp(v, 0.0f, 1.0f) * 65535.0f); };
        SDL_RumbleGamepad(d.pad, u16(low), u16(high), ms);
    }

    bool Devices::rumbleTriggers(const Device &d, float left, float right, uint32_t ms)
    {
        if (d.systemVibrator)
        {
            systemvibrator::triggers(left, right, ms);
            return true;
        }
        auto u16 = [](float v) { return static_cast<Uint16>(std::clamp(v, 0.0f, 1.0f) * 65535.0f); };
        return SDL_RumbleGamepadTriggers(d.pad, u16(left), u16(right), ms);
    }

    void Devices::showPlayer(const Device &d, int player)
    {
        SDL_SetGamepadPlayerIndex(d.pad, player);
        // Light bar: player 1 blue, player 2 red, unassigned dim white.
        if (player == 0)
            SDL_SetGamepadLED(d.pad, 0, 64, 255);
        else if (player == 1)
            SDL_SetGamepadLED(d.pad, 255, 32, 32);
        else
            SDL_SetGamepadLED(d.pad, 24, 24, 24);
    }
}
