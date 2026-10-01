#include "Paths.h"

#include <cstdint>
#include <cstdlib>
#include <system_error>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace rt::paths
{
    std::filesystem::path dataRoot()
    {
        if (const char *overrideDir = std::getenv("RT_DATA_DIR"); overrideDir && *overrideDir)
            return overrideDir;
#if defined(__APPLE__)
        const char *home = std::getenv("HOME");
        return std::filesystem::path(home ? home : ".") / "Library" / "Application Support" / "RoadTripRecomp";
#else
        const char *xdg = std::getenv("XDG_DATA_HOME");
        if (xdg && *xdg)
            return std::filesystem::path(xdg) / "RoadTripRecomp";
        const char *home = std::getenv("HOME");
        return std::filesystem::path(home ? home : ".") / ".local" / "share" / "RoadTripRecomp";
#endif
    }

    std::filesystem::path discDir() { return dataRoot() / "disc"; }
    std::filesystem::path savesDir() { return dataRoot() / "saves"; }
    std::filesystem::path installManifest() { return discDir() / ".installed.json"; }
    std::filesystem::path lbnMap() { return discDir() / ".lbn_map.tsv"; }
    std::filesystem::path gameDir() { return dataRoot() / "game"; }

    std::filesystem::path bundleResources()
    {
        if (const char *overrideDir = std::getenv("RT_RESOURCES_DIR"); overrideDir && *overrideDir)
            return overrideDir;
#if defined(__APPLE__)
        char buf[4096];
        uint32_t size = sizeof(buf);
        if (_NSGetExecutablePath(buf, &size) == 0)
        {
            std::error_code ec;
            const std::filesystem::path exe = std::filesystem::canonical(buf, ec);
            if (!ec)
                return exe.parent_path().parent_path() / "Resources"; // Contents/MacOS/.. -> Contents/Resources
        }
#endif
        return "Resources";
    }
}
