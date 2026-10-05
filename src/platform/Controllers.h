#pragma once

// Controller settings for the in-game menu (src/ui/PauseMenu): which player each device plays
// as, rebinding the DS2 buttons, stick deadzones and vibration. Changes are saved to input.toml
// straight away. Render thread only.

#include <string>

namespace rt::input
{
    // Who a device plays as: Auto (the default rules), player 1, player 2, or not playing.
    enum class PlayerChoice { Auto, Player1, Player2, Off };
    PlayerChoice playerChoice(const std::string &id);
    void setPlayerChoice(const std::string &id, PlayerChoice choice);

    // The controls that press DS2 button `button` (Ps2Button) on this device, for display.
    std::string bindingText(const std::string &id, int button);
    // The next control used on that device replaces the button's first control (Escape, Guide or
    // 6 s cancel). A control already on another button swaps places with it.
    void startRebind(const std::string &id, int button);
    // The DS2 button being rebound, or -1, and how long until it gives up.
    int rebinding();
    float rebindSecondsLeft();
    // The button the last rebind swapped with (its old control moved there), if within `seconds`.
    int lastRebindSwappedWith(float seconds);
    // Every frame while rebinding (before reading menu input).
    void pollRebind();
    // Back to the default buttons (keyboard: the default keys; a controller: the shared profile).
    void resetBindings(const std::string &id);

    // A controller's own settings (0..0.5 deadzone, 0..1 vibration); edits give it its own profile.
    float stickDeadzone(const std::string &id, bool right);
    void setStickDeadzone(const std::string &id, bool right, float value);
    float vibration(const std::string &id);
    void setVibration(const std::string &id, float value);
    // Overall vibration strength (0 = off).
    float vibrationOverall();
    void setVibrationOverall(float value);
    void testVibration(const std::string &id);
}
