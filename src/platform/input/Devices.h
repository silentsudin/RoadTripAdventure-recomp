#pragma once

// Connected controllers, through SDL3's gamepad API (XInput, DirectInput, DualSense and
// DualShock 4 over HIDAPI, Switch Pro, 8BitDo, Android controllers, ... with SDL's mapping
// database). Render thread only. raylib's SDL backend drains the event queue each frame, so
// hotplug events only mark the list dirty (an event watch) and update() reconciles it.

#include "Mapping.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

struct SDL_Gamepad;
union SDL_Event;

namespace rt::input
{
    struct Device
    {
        uint32_t instance = 0;      // SDL_JoystickID
        SDL_Gamepad *pad = nullptr;
        std::string id;             // stable across reconnects: GUID + serial (or n-th of its kind)
        std::string name;
        std::string type;           // "ps5", "ps4", "xboxone", "switchpro", "standard", ...
        int batteryPercent = -1;    // -1 unknown / wired
        bool systemVibrator = false; // no motors of its own: rumble goes to the device's vibrator (Android)
        AxisLatch latch;
    };

    class Devices
    {
    public:
        bool initialize(bool backgroundInput);
        void shutdown();
        // Opens new controllers and drops removed ones; returns true if the list changed.
        bool update();
        std::vector<Device> &list() { return m_devices; }

        static GamepadSnapshot snapshot(const Device &d);
        // low/high: 0..1 (large, slow motor / small, fast motor); lasts `ms` unless renewed.
        static void rumble(const Device &d, float low, float high, uint32_t ms);
        // The trigger motors (Xbox One and later pads; false where the pad has none).
        static bool rumbleTriggers(const Device &d, float left, float right, uint32_t ms);
        // Player LEDs (DualSense, Switch Pro, Xbox 360) and the light bar (DualSense, DS4).
        static void showPlayer(const Device &d, int player);

    private:
        std::vector<Device> m_devices;
        std::atomic<bool> m_dirty{true};
        bool m_ready = false;
        static bool onEvent(void *self, SDL_Event *event);
    };
}
