#pragma once

// Shared vocabulary of the input layer: the DualShock 2 the game sees, and the controls of a
// modern gamepad (SDL3's gamepad model: buttons by position, so "south" is Cross on a
// PlayStation pad, A on Xbox and B on a Switch pad).

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace rt::input
{
    constexpr int kPlayers = 2;

    enum class Ps2Button : uint8_t
    {
        Up, Down, Left, Right, Cross, Circle, Square, Triangle, L1, R1, L2, R2, Start, Select, L3, R3, Count
    };
    constexpr int kPs2ButtonCount = static_cast<int>(Ps2Button::Count);

    // Active-low bit in the DualShock 2 button word.
    uint16_t ps2Mask(Ps2Button b);
    const char *ps2Name(Ps2Button b);
    std::optional<Ps2Button> ps2FromName(std::string_view name);

    // Stick directions a key or a gamepad control can push fully (keyboard play, remapping).
    enum class StickDir : uint8_t
    {
        LUp, LDown, LLeft, LRight, RUp, RDown, RLeft, RRight, Count
    };
    constexpr int kStickDirCount = static_cast<int>(StickDir::Count);
    const char *stickDirName(StickDir d); // "lstick_up", ...
    std::optional<StickDir> stickDirFromName(std::string_view name);

    // Gamepad buttons and axes, numbered as SDL3's SDL_GamepadButton / SDL_GamepadAxis (checked
    // with static_asserts where SDL is included).
    enum class PadButton : uint8_t
    {
        South, East, West, North, Back, Guide, Start, LStick, RStick, LShoulder, RShoulder,
        DpadUp, DpadDown, DpadLeft, DpadRight, Misc1, RPaddle1, LPaddle1, RPaddle2, LPaddle2, Touchpad,
        Misc2, Misc3, Misc4, Misc5, Misc6, Count
    };
    constexpr int kPadButtonCount = static_cast<int>(PadButton::Count);

    enum class PadAxis : uint8_t
    {
        LeftX, LeftY, RightX, RightY, LeftTrigger, RightTrigger, Count
    };
    constexpr int kPadAxisCount = static_cast<int>(PadAxis::Count);

    // One gamepad control that can drive a DS2 button: a button, or an axis pushed past the
    // trigger threshold one way ("lefttrigger", "leftx+", "lefty-", ...).
    struct PadSource
    {
        enum class Kind : uint8_t { Button, AxisPlus, AxisMinus } kind = Kind::Button;
        uint8_t index = 0;
        bool operator==(const PadSource &) const = default;
    };
    std::string padSourceName(PadSource s);
    std::optional<PadSource> padSourceFromName(std::string_view name);

    // What one player's pad reads: DS2 buttons (active-low) and sticks (-1..1, deadzones applied).
    struct PadOutput
    {
        uint16_t buttons = 0xFFFFu;
        float lx = 0, ly = 0, rx = 0, ry = 0;
    };

    // 0..255 with 128 at rest, as the DS2 reports its sticks.
    uint8_t stickByte(float v);
}
