#pragma once

// Controller settings for the in-game menu (src/ui/PauseMenu): which player each device plays
// as, rebinding the DS2 buttons, stick deadzones and vibration. Changes are saved to input.toml
// straight away. Render thread only.

#include <optional>
#include <string>
#include <vector>

namespace rt::input
{
    // Who a device plays as: Auto (the default rules), player 1, player 2, or not playing.
    enum class PlayerChoice { Auto, Player1, Player2, Off };
    PlayerChoice playerChoice(const std::string &id);
    void setPlayerChoice(const std::string &id, PlayerChoice choice);

    // The controls that press DS2 button `button` (Ps2Button) on this device, for display.
    std::string bindingText(const std::string &id, int button);
    // The same as glyphs to draw (Theme control()): family "keyboard" / "ps" / "xbox" / "nintendo",
    // kind 0 button, 1 / 2 axis + / -, index (SDL gamepad button / axis), key (a keyboard key).
    struct ControlGlyph
    {
        std::string family;
        int kind = 0, index = 0;
        std::string key;
    };
    std::vector<ControlGlyph> bindingGlyphs(const std::string &id, int button);
    // The label family of a device ("keyboard", "ps", "xbox", "nintendo").
    std::string familyOf(const std::string &id);
    // A short taste of a vibration style on player 1's controllers: 0 Dynamic, 1 Classic.
    void sampleVibration(int style); // 0 Dynamic, 1 Classic, 2 a short "ready" pulse

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

    // The Driving controls page: the next control used on any of a player's devices (keyboard
    // included), caught rather than bound (Escape, Guide or 6 s cancel). While it waits,
    // pollRebind() serves it as well.
    void startCapture(int player);
    bool capturing();
    float captureSecondsLeft();
    // While waiting: how far the pad's back button (B, ○ on PlayStation) has been held towards
    // cancelling, 0..1 (a tap of it is a control like any other), or -1 when it isn't held.
    float captureCancelHold();
    struct CapturedControl
    {
        std::string device;  // the device it came from
        int ps2Button = -1;  // the DS2 button it presses now (-1: none)
        std::string key;     // a keyboard key's name, or
        int padKind = 0, padIndex = 0; // a gamepad control (PadSource)
    };
    // The control caught, once (nothing while waiting or after a cancel).
    std::optional<CapturedControl> takeCapture();
    // Makes the caught control press DS2 button `button` on its device (swapping as rebinding does).
    void bindCaptured(const CapturedControl &c, int button);
    // The device a player's controls are shown for: its first controller, else the keyboard.
    std::string primaryDevice(int player);

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
