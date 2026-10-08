#include "Paths.h"

#include <cstdint>
#include <cstdlib>
#include <system_error>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__ANDROID__)
#include <SDL3/SDL_system.h>
#include <dlfcn.h>
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
#elif defined(__ANDROID__)
        const char *internal = SDL_GetAndroidInternalStoragePath(); // the app's files/ directory
        return internal ? std::filesystem::path(internal) : std::filesystem::path(".");
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

    std::filesystem::path texturesDir()
    {
#if defined(__ANDROID__)
        if (!std::getenv("RT_DATA_DIR"))
            if (const char *external = SDL_GetAndroidExternalStoragePath(); external && *external)
            {
                const std::filesystem::path dir = std::filesystem::path(external) / "textures";
                // Once: what an earlier version kept in the private data moves here.
                std::error_code ec;
                const std::filesystem::path old = dataRoot() / "textures";
                if (std::filesystem::exists(old, ec) && !std::filesystem::exists(dir, ec))
                {
                    std::filesystem::create_directories(dir.parent_path(), ec);
                    std::filesystem::rename(old, dir, ec);
                    if (ec)
                    {
                        std::filesystem::copy(old, dir, std::filesystem::copy_options::recursive, ec);
                        if (!ec)
                            std::filesystem::remove_all(old, ec);
                    }
                }
                return dir;
            }
#endif
        return dataRoot() / "textures";
    }

    std::string displayPath(const std::filesystem::path &path)
    {
        std::string s = path.string();
#if defined(__ANDROID__)
        for (const char *root : {"/storage/emulated/0/", "/sdcard/"})
            if (s.rfind(root, 0) == 0)
                return s.substr(std::char_traits<char>::length(root));
#endif
        return s;
    }

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
#elif defined(__ANDROID__)
        return dataRoot() / "res"; // extracted from the APK's assets
#endif
        return "Resources";
    }

    std::filesystem::path toolDir()
    {
#if defined(__ANDROID__)
        // The APK's native library directory (extracted, executable): libmain.so and the compiler.
        Dl_info info{};
        if (dladdr(reinterpret_cast<void *>(&toolDir), &info) && info.dli_fname)
            return std::filesystem::path(info.dli_fname).parent_path();
#endif
        return bundleResources();
    }
}
