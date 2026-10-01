#pragma once

// Minimal ISO9660 reader for PS2 disc images (.cue/.bin MODE1/MODE2 2352, or .iso 2048).
// Mirrors scripts/iso9660.py.

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace rt
{
    struct IsoEntry
    {
        std::string path; // "SYS/FONT.GSL" (";1" stripped)
        uint32_t lba = 0;
        uint32_t size = 0;
        bool isDir = false;
    };

    class IsoReader
    {
    public:
        IsoReader() = default;
        ~IsoReader();
        IsoReader(const IsoReader &) = delete;
        IsoReader &operator=(const IsoReader &) = delete;

        // Accepts a .cue (follows its FILE line), raw .bin, or cooked .iso.
        bool open(const std::filesystem::path &imagePath, std::string &error);
        void close();

        const std::filesystem::path &dataFile() const { return m_dataFile; }
        const std::string &volumeId() const { return m_volumeId; }

        bool readSectors(uint32_t lba, uint32_t count, uint8_t *out);
        std::vector<IsoEntry> listDir(const IsoEntry &dir);
        std::vector<IsoEntry> walk(); // every file and directory, depth-first
        bool find(const std::string &path, IsoEntry &out);

        // Streams a file's bytes to `sink` in chunks; returns false on read error.
        bool readFile(const IsoEntry &entry, const std::function<bool(const uint8_t *, size_t)> &sink);

    private:
        std::FILE *m_file = nullptr;
        std::filesystem::path m_dataFile;
        uint32_t m_rawSectorSize = 2048;
        uint32_t m_dataOffset = 0;
        std::string m_volumeId;
        IsoEntry m_root;
    };
}
