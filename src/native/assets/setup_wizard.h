#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace rt::native::assets {

struct SetupManifestFile {
    std::string path;
    std::uintmax_t size = 0;
    std::uint32_t crc32 = 0;
};

struct SetupManifest { std::vector<SetupManifestFile> files; };

std::uint32_t crc32(const std::vector<unsigned char>& bytes);
// The app's data directory (RT_DATA_DIR, else the platform default): disc,
// saves, settings, texture packs.
std::filesystem::path dataRoot();

bool hasExpectedBootElfCrc(const std::filesystem::path& elf);
bool writeSetupManifest(const std::filesystem::path& path, const SetupManifest& manifest);
bool isSetupManifestValid(const std::filesystem::path& assetsRoot);

// Recomp setup-screen wording (SetupScreen.cpp:264-405), data only.
// Native has no game-compile step, so the only phase text it adds is "Preparing Road Trip".
enum class SetupWarning { Different, Damaged, Unverified };
struct SetupScreenView {
    enum class Kind { Welcome, Warning, Progress, Failed } kind = Kind::Welcome;
    std::string note;                       // Welcome: extra warning line
    SetupWarning warning = SetupWarning::Different;
    std::string heading;                    // Progress: empty = default
    int step = 1, steps = 1;
    std::string phase = "Preparing Road Trip";
    double fraction = -1.0;                 // < 0 = unmeasured
    double etaSeconds = -1.0;               // < 0 = unknown
    int elapsedSeconds = 0;                 // shown while unmeasured
    std::string detail;
    std::string error;                      // Failed: message without details
    std::vector<std::string> logTail;
    bool skippable = false; std::string skipLabel;
};
struct SetupScreenText {
    std::string heading, body, phaseLine, percent, eta, footer;
    std::vector<std::string> buttons;
    int defaultFocus = 0;
};
SetupScreenText describeSetupScreen(const SetupScreenView& view);

// Optional on-screen host. pump() is called on the calling thread while work runs (and repeatedly on the
// Failed screen); it draws the view and returns the user's action. Without a host the wizard only logs.
enum class SetupInput { None, Close, TryAgain, Continue };
struct SetupHost {
    std::function<SetupInput(const SetupScreenView&)> pump;
    bool closed = false;  // set when the user closed the app from setup
};

// Runs before the renderer's normal title/state selection. It never throws:
// an absent or invalid user-supplied disc leaves the app running in setup.
bool RunFirstRunSetupIfRequired(SetupHost* host = nullptr);

} // namespace rt::native::assets
