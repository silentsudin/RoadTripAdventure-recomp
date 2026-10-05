#pragma once

#include <filesystem>

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
}
