#pragma once

// Builds the game on the user's machine: ps2_recomp turns the ELF from *their* disc into C++, the
// system clang compiles it into libroadtrip_game.dylib, and the app loads that at startup.
// This keeps all game-derived code out of the distributed app.

#include "platform/TaskProgress.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace rt::game
{
    // Absolute path to clang++ from the Xcode Command Line Tools, if installed.
    std::optional<std::filesystem::path> findCompiler();

    // Asks macOS to install the Command Line Tools (shows the system prompt).
    void requestCommandLineTools();

    // True if a game library built by this exact app version (same SDK/recompiler) exists.
    bool isBuilt();

    // Recompiles `elf` and builds the game library. Blocking; run on a worker thread.
    bool build(const std::filesystem::path &elf, TaskProgress &progress);

    // dlopen()s the game library and fills the runtime's function table.
    bool load(std::string &error);

    std::filesystem::path buildLog();

    // After load(): the recompiled VU1 entry point (PS2Runtime::Vu1NativeEntry) or nullptr.
    void *vu1NativeEntry(uint64_t &imageHash);
}
