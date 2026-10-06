// The game's own Options handed to the app (see GameOptions.h).

#include "game/GameOptions.h"

#include "settings/Settings.h"

#include "ps2_runtime.h"

#include <atomic>

namespace rt::game
{
    namespace
    {
        std::atomic<bool> g_requested{false};
        constexpr uint32_t kVibration = 0x0033590E;
    }

    void requestOptions() { g_requested = true; }

    bool takeOptionsRequest() { return g_requested.exchange(false); }

    void applyGameOptions(PS2Runtime &runtime)
    {
        uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram)
            return;
        // On: any non-zero value rumbles our pads (the boot value 2, "ANALOG mode button", too),
        // so it is left alone unless it is off.
        uint8_t &v = ram[kVibration];
        if (rt::settings::current().vibration ? v == 0 : v != 0)
            v = rt::settings::current().vibration ? 4 : 0;
    }
}
