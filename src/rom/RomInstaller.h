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

    // The good dump (Redump disc 27678: SLUS-20398 v1.02, one Mode 2 track of 262,046 sectors, whose
    // SHA-1 Redump lists as the .bin's below) in the forms players keep it, by SHA-1 of: the
    // .bin; an .iso made from it (its 2048-byte sectors); the data of a CHD made from the .bin
    // (`chdman createcd`: 2352 + 96 bytes of subcode per frame, padded to 4 frames), or from the
    // .iso (`chdman createdvd`: the .iso's bytes).
    inline constexpr const char *kImageSha1Bin = "236b902a72580f43a480f18d232723a8676b02e5";
    inline constexpr const char *kImageSha1Iso = "62a9b61ff1f7c4f5f88dbbc22682615707687cef";
    inline constexpr const char *kImageSha1ChdCd = "3db60fb071f32958817211a7d348e5aba827f2b1";

    enum class ImageCheck
    {
        Verified,   // the good dump
        Unverified, // a CHD whose data is intact (matches its own record) but isn't one of the above
        Mismatch,   // a .bin/.iso that isn't the good dump
        Damaged,    // a CHD whose data no longer matches its own record, or can't be decompressed
        Unreadable,
    };

    // Hashes the whole image (a few seconds for a .bin or .iso; a CHD is decompressed in full),
    // with `progress` showing it. `detail`: what was found, hashes included (for the log).
    ImageCheck verifyImage(const std::filesystem::path &image, TaskProgress &progress, std::string &detail);

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
