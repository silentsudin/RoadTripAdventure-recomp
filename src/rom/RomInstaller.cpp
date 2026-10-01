#include "RomInstaller.h"

#include "IsoReader.h"
#include "Sha1.h"
#include "platform/Paths.h"

#include <fstream>
#include <system_error>

namespace rt
{
    namespace fs = std::filesystem;

    RomCheck checkRom(const fs::path &image, std::string &detail)
    {
        IsoReader iso;
        if (!iso.open(image, detail))
            return RomCheck::Unreadable;

        IsoEntry elf;
        if (!iso.find(kBootElfName, elf))
        {
            detail = "volume '" + iso.volumeId() + "' has no " + kBootElfName;
            return RomCheck::WrongGame;
        }

        Sha1 sha;
        if (!iso.readFile(elf, [&](const uint8_t *p, size_t n)
                          { sha.update(p, n); return true; }))
        {
            detail = "read error while hashing boot ELF";
            return RomCheck::Unreadable;
        }
        const std::string digest = sha.hexdigest();
        if (digest != kBootElfSha1)
        {
            detail = std::string(kBootElfName) + " sha1 " + digest + " (expected " + kBootElfSha1 + ")";
            return RomCheck::UnknownBuild;
        }
        detail = digest;
        return RomCheck::Ok;
    }

    bool isInstalled()
    {
        std::error_code ec;
        return fs::exists(paths::installManifest(), ec) && fs::exists(paths::lbnMap(), ec) &&
               fs::exists(paths::discDir() / kBootElfName, ec);
    }

    std::vector<IsoEntry> readLbnMap()
    {
        std::vector<IsoEntry> out;
        std::ifstream in(paths::lbnMap());
        std::string line;
        while (std::getline(in, line))
        {
            const size_t a = line.find('\t');
            const size_t b = a == std::string::npos ? a : line.find('\t', a + 1);
            if (b == std::string::npos)
                continue;
            IsoEntry e;
            e.path = line.substr(0, a);
            e.lba = static_cast<uint32_t>(std::stoul(line.substr(a + 1, b - a - 1)));
            e.size = static_cast<uint32_t>(std::stoul(line.substr(b + 1)));
            out.push_back(std::move(e));
        }
        return out;
    }

    bool installFromRom(const fs::path &image, TaskProgress &progress)
    {
        auto fail = [&](std::string message)
        { return progress.fail(std::move(message)); };
        progress.setPhase("Extracting game files from your disc image");

        IsoReader iso;
        std::string error;
        if (!iso.open(image, error))
            return fail(error);

        std::vector<IsoEntry> entries = iso.walk();
        uint64_t totalBytes = 0;
        uint32_t totalFiles = 0;
        for (const auto &e : entries)
        {
            if (!e.isDir)
            {
                totalBytes += e.size;
                ++totalFiles;
            }
        }
        progress.done = 0;
        progress.total = totalBytes;
        uint32_t filesDone = 0;

        // Extract into a staging dir, then swap it in so a partial install never looks complete.
        const fs::path finalDir = paths::discDir();
        const fs::path staging = paths::dataRoot() / "disc.partial";
        std::error_code ec;
        fs::remove_all(staging, ec);
        fs::create_directories(staging, ec);
        if (ec)
            return fail("cannot create " + staging.string() + ": " + ec.message());

        for (const auto &e : entries)
        {
            const fs::path dst = staging / fs::path(e.path);
            if (e.isDir)
            {
                fs::create_directories(dst, ec);
                continue;
            }
            fs::create_directories(dst.parent_path(), ec);
            std::ofstream out(dst, std::ios::binary | std::ios::trunc);
            if (!out)
                return fail("cannot write " + dst.string());
            const bool ok = iso.readFile(e, [&](const uint8_t *p, size_t n)
                                         {
                                             out.write(reinterpret_cast<const char *>(p), static_cast<std::streamsize>(n));
                                             progress.done += n;
                                             return static_cast<bool>(out); });
            if (!ok)
                return fail("failed extracting " + e.path);
            progress.setDetail(std::to_string(++filesDone) + " / " + std::to_string(totalFiles) + " files");
        }

        {
            std::ofstream manifest(staging / ".installed.json", std::ios::trunc);
            manifest << "{\n  \"source\": \"" << image.filename().string() << "\",\n"
                     << "  \"volume_id\": \"" << iso.volumeId() << "\",\n"
                     << "  \"boot_elf\": \"" << kBootElfName << "\",\n"
                     << "  \"files\": " << totalFiles << ",\n"
                     << "  \"bytes\": " << totalBytes << "\n}\n";
        }

        {
            std::ofstream map(staging / ".lbn_map.tsv", std::ios::trunc);
            for (const auto &e : entries)
            {
                if (!e.isDir)
                    map << e.path << '\t' << e.lba << '\t' << e.size << '\n';
            }
        }

        fs::remove_all(finalDir, ec);
        fs::rename(staging, finalDir, ec);
        if (ec)
            return fail("cannot move extracted files into place: " + ec.message());
        fs::create_directories(paths::savesDir(), ec);
        return true;
    }
}
