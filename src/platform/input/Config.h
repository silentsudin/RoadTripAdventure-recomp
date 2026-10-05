#pragma once

// input.toml (in the data directory): rumble strength, which devices play as which player, the
// keyboard mapping (SDL scancode names) and the gamepad mapping (positional names: "south" is
// Cross on a PlayStation pad), with per-controller overrides. Version 1 files (raylib names,
// one deadzone) are migrated, keeping a copy as input.toml.v1.bak.

#include "Mapping.h"

#include <array>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace rt::input
{
    struct Config
    {
        float rumble = 1.0f;          // overall vibration strength, 0..1 (0 = off)
        bool backgroundInput = false; // read controllers while the window is in the background
        std::map<std::string, int> players; // device id -> player index (-1 = off) chosen by hand
        std::array<std::vector<std::string>, kPs2ButtonCount> keyButtons;
        std::array<std::vector<std::string>, kStickDirCount> keyStickDirs;
        GamepadProfile gamepad;                          // every controller ...
        std::map<std::string, GamepadProfile> gamepads;  // ... unless it has its own (by device id)

        const GamepadProfile &profileFor(const std::string &id) const;
    };

    Config defaultConfig();
    // Reads `path`; writes the defaults if it is missing, and migrates version 1 files.
    Config loadConfig(const std::filesystem::path &path);
    void saveConfig(const std::filesystem::path &path, const Config &config);
}
