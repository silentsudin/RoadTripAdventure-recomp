#pragma once

// Which devices play as player 1 and player 2. Pure: devices are their stable ids ("keyboard" is
// the keyboard). Rules:
//  - a device the player put in a slot (config or the Controllers window) stays there;
//  - otherwise the keyboard and the first gamepad play as player 1, the next gamepad as player 2,
//    and further gamepads wait unassigned;
//  - a gamepad that reconnects gets its old slot back;
//  - player 1 is always plugged in (keyboard at least); player 2 only with a device, like an empty
//    second port on a PS2.

#include "Types.h"

#include <array>
#include <map>
#include <string>
#include <vector>

namespace rt::input
{
    inline constexpr const char *kKeyboardId = "keyboard";

    class Players
    {
    public:
        using Slots = std::array<std::vector<std::string>, kPlayers>;

        // Devices the player assigned (player index per id; -1 = keep off both).
        void setManual(const std::map<std::string, int> &manual) { m_manual = manual; }
        const std::map<std::string, int> &manual() const { return m_manual; }
        void assignManually(const std::string &id, int player) { m_manual[id] = player; }

        // `connected`: device ids in the order they were connected (keyboard included).
        Slots assign(const std::vector<std::string> &connected);
        static bool pluggedIn(const Slots &slots, int player) { return player == 0 || !slots[player].empty(); }

    private:
        std::map<std::string, int> m_manual;
        std::map<std::string, int> m_remembered; // automatic choices, for reconnects
    };
}
