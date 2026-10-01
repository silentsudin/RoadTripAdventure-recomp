#pragma once

// Native dialogs. Kept in a separate translation unit so Cocoa headers never meet raylib.h.

#include <filesystem>
#include <optional>
#include <string>

namespace rt::dialogs
{
    // Prompts for a disc image (.cue/.bin/.iso). Empty if cancelled or unsupported.
    std::optional<std::filesystem::path> pickRomImage();

    // Shows a modal message. Returns true if the user pressed the primary button.
    bool message(const std::string &title, const std::string &text, bool withCancel = false);
}
