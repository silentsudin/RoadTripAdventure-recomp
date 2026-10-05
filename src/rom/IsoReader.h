#pragma once

// Minimal ISO9660 reader for PS2 disc images (.cue/.bin MODE1/MODE2 2352, .iso 2048, or .chd:
// MAME's compressed hunks of data, CD or DVD, through libchdr). Mirrors scripts/iso9660.py.

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

        // Accepts a .cue (follows its FILE line), raw .bin, cooked .iso or a .chd (by its signature).
        bool open(const std::filesystem::path &imagePath, std::string &error);

        enum class Format
        {
            Raw,    // 2352-byte sectors (.bin)
            Cooked, // 2048-byte sectors (.iso)
            Chd,
        };
        Format format() const { return m_chd ? Format::Chd : m_rawSectorSize == 2048 ? Format::Cooked : Format::Raw; }

        // SHA-1 (hex) of the image as a whole: the file for .bin and .iso; for a CHD its
        // uncompressed data, every hunk decoded (so damage anywhere shows, and libchdr checks each
        // hunk's CRC). `progress(done, total)` is called as it goes; returns false on a read error.
        bool hashImage(const std::function<void(uint64_t, uint64_t)> &progress, std::string &hex);
        // A CHD's own record of that hash (chdman writes it), or "" for other formats.
        std::string chdDataSha1() const;
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
        bool readChdUnit(uint32_t unit, uint8_t *out);

        std::FILE *m_file = nullptr;
        void *m_chd = nullptr; // chd_file
        std::vector<uint8_t> m_hunk;
        int64_t m_hunkIndex = -1;
        uint32_t m_unitBytes = 0, m_unitsPerHunk = 0;
        std::filesystem::path m_dataFile;
        uint32_t m_rawSectorSize = 2048;
        uint32_t m_dataOffset = 0;
        std::string m_volumeId;
        IsoEntry m_root;
    };
}
