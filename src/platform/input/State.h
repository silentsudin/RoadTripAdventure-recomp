#pragma once

// The input layer's state, shared between Input.cpp (the per-frame loop) and the Controllers
// window. Render thread only.

#include "Config.h"
#include "Devices.h"
#include "Players.h"

namespace rt::input::detail
{
    Config &config();
    Devices &devices();
    Players &players();
    const Players::Slots &slots();
    // After the config or the player choices changed: re-resolve keys and re-assign players.
    void applyConfig();
    void save();
    // A short test vibration on one device.
    void testRumble(const std::string &id);
}
