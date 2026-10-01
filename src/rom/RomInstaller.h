#pragma once

// First-run installer: copies every file from the user's disc image into
// the app data directory so the runtime can serve cdrom0: from the host filesystem.

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "IsoReader.h"
#include "platform/TaskProgress.h"

namespace rt
{
    // Identity of the supported build (SLUS-20398, v1.02). See config/rom.json.
    inline constexpr const char *kBootElfName = "SLUS_203.98";
    inline constexpr const char *kVolumeId = "ROADTRIP";
    inline constexpr const char *kBootElfSha1 = "2431de1ec3edd0df4be37ba564658d70c4049089";
    inline constexpr uint32_t kBootElfSize = 1280576;

    enum class RomCheck
    {
        Ok,
        WrongGame,     // not a Road Trip disc
        UnknownBuild,  // Road Trip disc but boot ELF hash differs (other region/revision)
        Unreadable,
    };

    // Opens the image and checks the boot ELF against the known build.
    RomCheck checkRom(const std::filesystem::path &image, std::string &detail);

    // Original disc location of every extracted file (paths::lbnMap()). The game reads
    // sectors by LBN from a baked-in table, so the runtime needs these to find data.
    std::vector<IsoEntry> readLbnMap();

    // True if discDir() holds a complete extraction.
    bool isInstalled();

    // Extracts the full disc tree into paths::discDir(). Blocking; safe to run on a worker thread.
    // Does not mark `progress` finished on success, so further steps can follow.
    bool installFromRom(const std::filesystem::path &image, TaskProgress &progress);
}
