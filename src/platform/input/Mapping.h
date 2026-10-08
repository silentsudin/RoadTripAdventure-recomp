#pragma once

// From a device's raw state to what the game's pad reads. Pure functions: no SDL, no globals.

#include "Types.h"

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace rt::input
{
    // One gamepad's controls at an instant (axes: sticks -1..1, y down; triggers 0..1).
    struct GamepadSnapshot
    {
        std::array<bool, kPadButtonCount> buttons{};
        std::array<float, kPadAxisCount> axes{};
    };

    struct StickSettings
    {
        float inner = 0.15f; // radial deadzone: below this the stick reads centred
        float outer = 0.95f; // at and beyond this it reads fully pushed
    };

    // How a gamepad drives the DS2. Sticks always map left->left, right->right.
    struct GamepadProfile
    {
        std::array<std::vector<PadSource>, kPs2ButtonCount> buttons;
        std::array<std::vector<PadSource>, kStickDirCount> stickDirs; // controls that push a stick fully
        StickSettings left, right;
        float triggerOn = 0.35f;  // an axis used as a button presses past this ...
        float triggerOff = 0.25f; // ... and lets go below this (no chatter around one threshold)
        float rumble = 1.0f;      // vibration strength for this pad, 0..1
    };

    struct KeyboardProfile
    {
        std::array<std::vector<int>, kPs2ButtonCount> buttons;   // SDL scancodes
        std::array<std::vector<int>, kStickDirCount> stickDirs;
    };

    GamepadProfile defaultGamepadProfile(); // keyboard defaults are scancode names: see Config

    // The face-button layout family of a controller (SDL type name): "ps" for PlayStation pads,
    // "nintendo" for Nintendo ones, "xbox" for everything else (Xbox pads, and the "standard" pads
    // SDL can't name, such as the AYN Thor's, which are labelled the Xbox way).
    std::string padFamily(const std::string &type);
    // `base` (bindings by PlayStation position: the game's ✕ at the bottom, △ at the top) laid out
    // for the family's own conventions, so confirm and back are where its players expect them:
    // Xbox A (bottom) ✕ and B (right) △, ○ moving to Y (top); Nintendo A (right) ✕ and B (bottom)
    // △, ○ moving to X (top). PlayStation pads keep `base`.
    GamepadProfile familyProfile(const GamepadProfile &base, const std::string &family);

    // Axes used as buttons remember whether they are pressed (hysteresis), per device.
    struct AxisLatch
    {
        std::array<bool, kPadAxisCount> plus{}, minus{};
    };

    // A pushed stick (x, y) after the radial deadzone: 0 inside `inner`, scaled to reach 1 at `outer`.
    void applyDeadzone(float &x, float &y, const StickSettings &s);

    PadOutput mapGamepad(const GamepadSnapshot &pad, const GamepadProfile &profile, AxisLatch &latch);
    PadOutput mapKeyboard(const std::function<bool(int scancode)> &down, const KeyboardProfile &profile);

    // Several devices on one player: a button is pressed if any device presses it; each stick axis
    // takes the device pushing it furthest.
    PadOutput merge(const PadOutput &a, const PadOutput &b);
}
