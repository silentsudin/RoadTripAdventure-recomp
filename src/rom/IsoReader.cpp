#include "IsoReader.h"
#include "Sha1.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>

#include <libchdr/chd.h>

#if defined(__ANDROID__)
#include <SDL3/SDL_iostream.h>
#include <unistd.h>
#endif

namespace rt
{
    namespace
    {
        constexpr uint32_t kSector = 2048;

        uint32_t le32(const uint8_t *p)
        {
            return uint32_t(p[0]) | (uint32_t(p[1]) << 8) | (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
        }

        std::string upper(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                           { return static_cast<char>(std::toupper(c)); });
            return s;
        }

        std::filesystem::path resolveCue(const std::filesystem::path &cue, std::string &error)
        {
            std::ifstream in(cue);
            std::stringstream ss;
            ss << in.rdbuf();
            std::smatch m;
            const std::string text = ss.str();
            static const std::regex quoted(R"re(FILE\s+"([^"]+)")re");
            static const std::regex bare(R"re(FILE\s+(\S+))re");
            if (std::regex_search(text, m, quoted) || std::regex_search(text, m, bare))
                return cue.parent_path() / m[1].str();
            error = "no FILE entry in cue sheet";
            return {};
        }
    }

    IsoReader::~IsoReader() { close(); }

    void IsoReader::close()
    {
        if (m_chd)
            chd_close(static_cast<chd_file *>(m_chd)); // leaves m_file open (chd_open_file)
        m_chd = nullptr;
        m_hunk.clear();
        m_hunkIndex = -1;
        if (m_file)
            std::fclose(m_file);
        m_file = nullptr;
    }

    bool IsoReader::open(const std::filesystem::path &imagePath, std::string &error)
    {
        close();
        std::filesystem::path data = imagePath;
        std::string ext = upper(imagePath.extension().string());
        if (ext == ".CUE")
        {
            data = resolveCue(imagePath, error);
            if (data.empty())
                return false;
        }

#if defined(__ANDROID__)
        // A document picked through the storage access framework: a content:// URI, opened by the
        // content resolver (SDL) and read through our own copy of its file descriptor.
        if (data.string().rfind("content://", 0) == 0)
        {
            if (SDL_IOStream *io = SDL_IOFromFile(data.string().c_str(), "rb"))
            {
                auto *fp = static_cast<std::FILE *>(
                    SDL_GetPointerProperty(SDL_GetIOProperties(io), SDL_PROP_IOSTREAM_STDIO_FILE_POINTER, nullptr));
                const int fd = fp ? dup(fileno(fp)) : -1;
                SDL_CloseIO(io);
                if (fd >= 0 && !(m_file = fdopen(fd, "rb")))
                    ::close(fd);
            }
        }
        else
#endif
            m_file = std::fopen(data.string().c_str(), "rb");
        if (!m_file)
        {
            error = "cannot open " + data.string();
            return false;
        }
        m_dataFile = data;

        static const uint8_t kSync[12] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
        uint8_t head[16] = {};
        std::fread(head, 1, sizeof(head), m_file);
        if (std::memcmp(head, "MComprHD", 8) == 0)
        {
            // A CHD: units of 2448 bytes (a CD frame: 2352 + 96 subcode) or 2048 (DVD), several per
            // hunk. Where the 2048 user bytes sit in a unit: wherever sector 16 has the volume descriptor.
            std::fseek(m_file, 0, SEEK_SET);
            chd_file *chd = nullptr;
            if (chd_open_file(m_file, CHD_OPEN_READ, nullptr, &chd) != CHDERR_NONE)
            {
                error = "cannot read this CHD file";
                close();
                return false;
            }
            m_chd = chd;
            const chd_header *h = chd_get_header(chd);
            m_unitBytes = h->unitbytes;
            m_unitsPerHunk = h->unitbytes ? h->hunkbytes / h->unitbytes : 0;
            m_hunk.resize(h->hunkbytes);
            m_rawSectorSize = m_unitBytes;
            m_dataOffset = 0;
            std::vector<uint8_t> unit(m_unitBytes);
            bool found = false;
            if (m_unitsPerHunk && m_unitBytes >= kSector && readChdUnit(16, unit.data()))
                for (uint32_t off : {0u, 16u, 24u})
                    if (off + kSector <= m_unitBytes && std::memcmp(unit.data() + off + 1, "CD001", 5) == 0)
                    {
                        m_dataOffset = off;
                        found = true;
                        break;
                    }
            if (!found)
            {
                error = "not an ISO9660 disc inside this CHD";
                close();
                return false;
            }
        }
        else if (std::memcmp(head, kSync, sizeof(kSync)) == 0)
        {
            m_rawSectorSize = 2352;
            uint8_t mode = 0;
            std::fseek(m_file, 16 * 2352 + 15, SEEK_SET);
            std::fread(&mode, 1, 1, m_file);
            m_dataOffset = mode == 2 ? 24 : 16;
        }
        else
        {
            m_rawSectorSize = kSector;
            m_dataOffset = 0;
        }

        uint8_t pvd[kSector];
        if (!readSectors(16, 1, pvd) || std::memcmp(pvd + 1, "CD001", 5) != 0)
        {
            error = "not an ISO9660 image (no CD001 descriptor at sector 16)";
            close();
            return false;
        }
        m_volumeId.assign(reinterpret_cast<const char *>(pvd + 40), 32);
        m_volumeId.erase(m_volumeId.find_last_not_of(' ') + 1);

        const uint8_t *root = pvd + 156;
        m_root = IsoEntry{"", le32(root + 2), le32(root + 10), true};
        return true;
    }

    bool IsoReader::readChdUnit(uint32_t unit, uint8_t *out)
    {
        const int64_t hunk = unit / m_unitsPerHunk;
        if (hunk != m_hunkIndex)
        {
            if (chd_read(static_cast<chd_file *>(m_chd), static_cast<uint32_t>(hunk), m_hunk.data()) != CHDERR_NONE)
            {
                m_hunkIndex = -1;
                return false;
            }
            m_hunkIndex = hunk;
        }
        std::memcpy(out, m_hunk.data() + static_cast<size_t>(unit % m_unitsPerHunk) * m_unitBytes, m_unitBytes);
        return true;
    }

    bool IsoReader::readSectors(uint32_t lba, uint32_t count, uint8_t *out)
    {
        if (!m_file)
            return false;
        if (m_chd)
        {
            std::vector<uint8_t> unit(m_unitBytes);
            for (uint32_t i = 0; i < count; ++i)
            {
                if (!readChdUnit(lba + i, unit.data()))
                    return false;
                std::memcpy(out + static_cast<size_t>(i) * kSector, unit.data() + m_dataOffset, kSector);
            }
            return true;
        }
        if (m_rawSectorSize == kSector)
        {
            if (std::fseek(m_file, static_cast<long>(lba) * kSector, SEEK_SET) != 0)
                return false;
            return std::fread(out, kSector, count, m_file) == count;
        }
        for (uint32_t i = 0; i < count; ++i)
        {
            const long pos = static_cast<long>(lba + i) * m_rawSectorSize + m_dataOffset;
            if (std::fseek(m_file, pos, SEEK_SET) != 0 || std::fread(out + i * kSector, kSector, 1, m_file) != 1)
                return false;
        }
        return true;
    }

    std::vector<IsoEntry> IsoReader::listDir(const IsoEntry &dir)
    {
        std::vector<IsoEntry> out;
        const uint32_t sectors = (dir.size + kSector - 1) / kSector;
        std::vector<uint8_t> data(static_cast<size_t>(sectors) * kSector);
        if (!readSectors(dir.lba, sectors, data.data()))
            return out;

        size_t i = 0;
        while (i < data.size())
        {
            const uint8_t len = data[i];
            if (len == 0)
            {
                i = (i / kSector + 1) * kSector; // records never span sectors
                continue;
            }
            const uint8_t *rec = data.data() + i;
            const uint8_t nameLen = rec[32];
            std::string name(reinterpret_cast<const char *>(rec + 33), nameLen);
            if (!(nameLen == 1 && (name[0] == '\0' || name[0] == '\1')))
            {
                if (auto semi = name.find(';'); semi != std::string::npos)
                    name.resize(semi);
                out.push_back(IsoEntry{dir.path.empty() ? name : dir.path + "/" + name,
                                       le32(rec + 2), le32(rec + 10), (rec[25] & 2) != 0});
            }
            i += len;
        }
        return out;
    }

    std::vector<IsoEntry> IsoReader::walk()
    {
        std::vector<IsoEntry> out;
        std::vector<IsoEntry> stack{m_root};
        while (!stack.empty())
        {
            IsoEntry d = stack.back();
            stack.pop_back();
            for (auto &e : listDir(d))
            {
                out.push_back(e);
                if (e.isDir)
                    stack.push_back(e);
            }
        }
        return out;
    }

    bool IsoReader::find(const std::string &path, IsoEntry &out)
    {
        const std::string want = upper(path);
        for (auto &e : walk())
        {
            if (upper(e.path) == want)
            {
                out = e;
                return true;
            }
        }
        return false;
    }

    bool IsoReader::readFile(const IsoEntry &entry, const std::function<bool(const uint8_t *, size_t)> &sink)
    {
        constexpr uint32_t kChunkSectors = 256; // 512 KiB
        std::vector<uint8_t> buf(kChunkSectors * kSector);
        uint32_t remaining = entry.size;
        uint32_t lba = entry.lba;
        while (remaining > 0)
        {
            const uint32_t sectors = std::min(kChunkSectors, (remaining + kSector - 1) / kSector);
            if (!readSectors(lba, sectors, buf.data()))
                return false;
            const size_t n = std::min<size_t>(remaining, static_cast<size_t>(sectors) * kSector);
            if (!sink(buf.data(), n))
                return false;
            remaining -= static_cast<uint32_t>(n);
            lba += sectors;
        }
        return true;
    }

    std::string IsoReader::chdDataSha1() const
    {
        if (!m_chd)
            return {};
        const chd_header *h = chd_get_header(static_cast<chd_file *>(m_chd));
        static const char *digits = "0123456789abcdef";
        std::string hex;
        for (uint8_t b : h->rawsha1)
        {
            hex += digits[b >> 4];
            hex += digits[b & 15];
        }
        return hex;
    }

    bool IsoReader::hashImage(const std::function<void(uint64_t, uint64_t)> &progress, std::string &hex)
    {
        if (!m_file)
            return false;
        Sha1 sha;
        if (m_chd)
        {
            chd_file *chd = static_cast<chd_file *>(m_chd);
            const chd_header *h = chd_get_header(chd);
            std::vector<uint8_t> hunk(h->hunkbytes);
            uint64_t left = h->logicalbytes;
            for (uint32_t i = 0; i < h->totalhunks && left > 0; ++i)
            {
                if (chd_read(chd, i, hunk.data()) != CHDERR_NONE)
                    return false;
                const size_t n = static_cast<size_t>(std::min<uint64_t>(left, h->hunkbytes));
                sha.update(hunk.data(), n);
                left -= n;
                if (progress && (i & 63u) == 0u)
                    progress(h->logicalbytes - left, h->logicalbytes);
            }
            m_hunkIndex = -1; // the shared buffer wasn't used, but keep the cache honest
        }
        else
        {
            std::fseek(m_file, 0, SEEK_END);
            const uint64_t total = static_cast<uint64_t>(std::ftell(m_file));
            std::fseek(m_file, 0, SEEK_SET);
            std::vector<uint8_t> buf(1u << 20);
            uint64_t done = 0;
            for (size_t n; (n = std::fread(buf.data(), 1, buf.size(), m_file)) > 0;)
            {
                sha.update(buf.data(), n);
                done += n;
                if (progress)
                    progress(done, total);
            }
            if (std::ferror(m_file) || done != total)
                return false;
        }
        hex = sha.hexdigest();
        return true;
    }
}
