#include "Assets.h"

#include "platform/Paths.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace rt::android
{
    namespace
    {
        std::string trim(std::string s)
        {
            while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' '))
                s.pop_back();
            return s;
        }

        std::string readAsset(const char *name)
        {
            size_t size = 0;
            void *data = SDL_LoadFile(name, &size); // APK assets on Android
            if (!data)
                return {};
            std::string s(static_cast<const char *>(data), size);
            SDL_free(data);
            return s;
        }

        // ustar: 512-byte headers, name at 0 (100) + prefix at 345 (155), octal size at 124, type at 156.
        bool untar(SDL_IOStream *in, const fs::path &dst, std::string &error)
        {
            std::vector<char> block(512), data;
            for (;;)
            {
                if (SDL_ReadIO(in, block.data(), 512) != 512)
                    return true; // end of archive
                if (block[0] == 0)
                    return true;
                std::string name(block.data(), strnlen(block.data(), 100));
                const std::string prefix(block.data() + 345, strnlen(block.data() + 345, 155));
                if (!prefix.empty())
                    name = prefix + "/" + name;
                const uint64_t size = std::strtoull(std::string(block.data() + 124, 12).c_str(), nullptr, 8);
                const char type = block[156];
                const uint64_t padded = (size + 511) / 512 * 512;
                data.resize(padded);
                if (padded && SDL_ReadIO(in, data.data(), padded) != padded)
                {
                    error = "the build kit is truncated (" + name + ")";
                    return false;
                }
                if (type != '0' && type != 0)
                    continue; // directories are made as needed
                const fs::path out = dst / name;
                std::error_code ec;
                fs::create_directories(out.parent_path(), ec);
                std::ofstream f(out, std::ios::binary | std::ios::trunc);
                f.write(data.data(), std::streamsize(size));
                if (!f)
                {
                    error = "cannot write " + out.string();
                    return false;
                }
            }
        }
    }

    bool ensureBuildKit(std::string &error)
    {
        const std::string id = trim(readAsset("rt.id"));
        if (id.empty())
        {
            error = "the app is missing its build kit (assets/rt.id)";
            return false;
        }
        const fs::path res = paths::bundleResources();
        std::ifstream current(res / "sdk" / "build_id");
        std::string have;
        std::getline(current, have);
        if (trim(have) == id)
            return true;

        std::cout << "[setup] extracting the build kit " << id.substr(0, 12) << "\n";
        const fs::path staging = res.string() + ".new";
        std::error_code ec;
        fs::remove_all(staging, ec);
        fs::create_directories(staging, ec);
        SDL_IOStream *in = SDL_IOFromFile("rt.tar", "rb");
        if (!in)
        {
            error = "cannot open the build kit (assets/rt.tar)";
            return false;
        }
        const bool ok = untar(in, staging, error);
        SDL_CloseIO(in);
        if (!ok)
            return false;
        fs::remove_all(res, ec);
        fs::rename(staging, res, ec);
        if (ec)
        {
            error = "cannot install the build kit: " + ec.message();
            return false;
        }
        return true;
    }
}
