#pragma once

// Host input -> the game's two DualShock 2 ports (src/platform/input/). Call update() once per
// host frame on the render thread: it tracks the keyboard and every SDL3 controller (hotplug,
// stable ids), works out who plays as player 1 and 2, maps each player's devices through
// input.toml in the data directory (written with defaults on first run), hands each port's state
// to the runtime's pad emulation, and plays the game's vibration on that player's controllers.
// The game thread never touches host input, so there is no data race and short taps are not missed.

#include <string>
#include <vector>

namespace rt::input
{
    // Loads (or creates) input.toml, starts controller support, stops Escape from closing the window.
    void initialize();
    void update();
    // While another window (Options) is open: the game reads neutral pads, motors stop.
    void blockGameInput(bool blocked);
    // Stops vibration and closes the controllers (before the window closes).
    void shutdown();

    // Menu actions this frame from any controller or the keyboard (directions repeat while held).
    // Cross/South or Enter confirms; Triangle/North, Circle/East or Escape/Backspace go back;
    // Square/West or R is the page's extra action; Guide, Back+Start held together for 0.3 s,
    // Escape or F3 open/close the menu.
    struct MenuInput
    {
        bool up = false, down = false, left = false, right = false;
        bool confirm = false, back = false, extra = false;
        bool toggleMenu = false;
    };
    MenuInput menuInput();

    struct DeviceStatus
    {
        std::string id, name, type; // type: "keyboard", "ps5", "xboxone", "switchpro", ...
        int batteryPercent = -1;
        int player = -1;            // 0 / 1, or -1 when not playing
    };
    std::vector<DeviceStatus> devices();
    int playerOf(const std::string &id);
    // True if any controller is connected (for button prompts and hints).
    bool anyGamepad();
}
