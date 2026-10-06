#pragma once

#include <filesystem>
#include <string>

namespace rt::paths
{
    // ~/Library/Application Support/RoadTripRecomp (or $RT_DATA_DIR if set).
    std::filesystem::path dataRoot();
    // Extracted disc tree; acts as cdrom0: for the runtime.
    std::filesystem::path discDir();
    // Memory card root (mc0:).
    std::filesystem::path savesDir();
    // Marker written after a successful extraction.
    std::filesystem::path installManifest();
    // Where the game library is built from the user's disc on first launch.
    std::filesystem::path gameDir();
    // RoadTrip.app/Contents/Resources (recompiler, SDK headers, config). $RT_RESOURCES_DIR overrides.
    std::filesystem::path bundleResources();
    // Tab-separated "path<TAB>lbn<TAB>size" for every file on the disc.
    std::filesystem::path lbnMap();
    // Where shipped executables live (Android: the native library directory, which may run them).
    std::filesystem::path toolDir();
    // Texture dumps and packs (textures/dumps, textures/packs). On Android the app's external files
    // (Android/data/<package>/files), which a computer reaches over USB, unlike the private data.
    std::filesystem::path texturesDir();
    // A path as the player would find it (Android: from the shared storage's root).
    std::string displayPath(const std::filesystem::path &path);
}
