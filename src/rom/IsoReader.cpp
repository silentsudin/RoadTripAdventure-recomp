#include "IsoReader.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>

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
        if (std::memcmp(head, kSync, sizeof(kSync)) == 0)
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

    bool IsoReader::readSectors(uint32_t lba, uint32_t count, uint8_t *out)
    {
        if (!m_file)
            return false;
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
}
