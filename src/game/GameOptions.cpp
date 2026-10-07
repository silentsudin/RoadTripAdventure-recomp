// The game's own Options handed to the app (see GameOptions.h).

#include "game/GameOptions.h"

#include "game/GameStats.h"
#include "settings/Settings.h"

#include "runtime/ps2_test_harness.h"

#include "ps2_runtime.h"

#include <atomic>
#include <cstdlib>
#include <cstring>

namespace rt::game
{
    namespace
    {
        std::atomic<int> g_requested{0};
        constexpr uint32_t kVibration = 0x0033590E;
        // [progress] radio_station: the progress block (0x177F760) + 0x12DD.
        constexpr uint32_t kRadioStation = 0x0177F760u + 0x12DDu;
        // [tasks]: the task pool (0x30 tasks of 0x30 bytes: +0 active, +0xC function) and the
        // town's task (0x2105B8).
        constexpr uint32_t kTaskPool = 0x0177A0D0u, kTownTask = 0x002105B8u;
        std::atomic<int> g_radioRequest{-1};

        bool townRunning(const uint8_t *ram)
        {
            for (uint32_t i = 0; i < 0x30; ++i)
            {
                const uint8_t *t = ram + kTaskPool + i * 0x30;
                uint32_t fn;
                std::memcpy(&fn, t + 0xC, 4);
                if (t[0] && fn == kTownTask)
                    return true;
            }
            return false;
        }
    }

    void requestOptions(OptionsRequest where) { g_requested = static_cast<int>(where); }

    OptionsRequest takeOptionsRequest() { return static_cast<OptionsRequest>(g_requested.exchange(0)); }

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

    const char *radioStationName(int station)
    {
        switch (station)
        {
        case kRadioOff: return "Off";
        case kRadioPeachFm: return "PEACH FM";
        case kRadioERadio: return "E-RADIO";
        default: return "?";
        }
    }

    bool radioAvailable(PS2Runtime &runtime)
    {
        const Stats st = readStats(runtime);
        return st.adventure && !st.demo;
    }

    int radioStation(PS2Runtime &runtime)
    {
        const uint8_t *ram = runtime.memory().getRDRAM();
        return ram ? ram[kRadioStation] : kRadioOff;
    }

    bool setRadioStation(PS2Runtime &runtime, int station)
    {
        uint8_t *ram = runtime.memory().getRDRAM();
        if (!ram || station < 0 || station >= kRadioStations || !radioAvailable(runtime))
            return false;
        ram[kRadioStation] = static_cast<uint8_t>(station);
        // The stream is switched by the town's task (it plays only there); elsewhere the byte is
        // what the game starts from.
        if (townRunning(ram))
            g_radioRequest = station;
        return true;
    }

    int takeRadioRequest() { return g_radioRequest.exchange(-1); }

    std::string radioCommand(PS2Runtime &runtime, const std::string &line)
    {
        bool ok = true;
        const std::string want = ps2_test::jsonField(line, "station");
        if (!want.empty())
            ok = setRadioStation(runtime, std::atoi(want.c_str()));
        const int st = radioStation(runtime);
        return std::string("{\"ok\":") + (ok ? "true" : "false") + ",\"available\":" +
               (radioAvailable(runtime) ? "true" : "false") + ",\"station\":" + std::to_string(st) +
               ",\"name\":\"" + radioStationName(st) + "\"}";
    }
}
