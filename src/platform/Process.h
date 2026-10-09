#pragma once

// Running a child process (Windows; the POSIX builds spawn with posix_spawn in GameBuilder.cpp).
// Kept apart so windows.h never meets raylib.h (their CloseWindow clash).

#include <filesystem>
#include <string>
#include <vector>

namespace rt::process
{
    // Runs argv[0] with stdout and stderr appended to `log` and `cwd` as its working directory
    // (the current one if empty); waits and returns the exit code, or -1 if it couldn't start.
    int run(const std::vector<std::string> &argv, const std::filesystem::path &log,
            const std::filesystem::path &cwd = {});

    // Runs argv and returns its trimmed output; empty on a failure.
    std::string capture(const std::vector<std::string> &argv);
}
