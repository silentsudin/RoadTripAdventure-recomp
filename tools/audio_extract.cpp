// rt_audio_extract <disc dir> <output dir>
//
// The disc's VAG streams (PlayStation ADPCM, "VAGp" header) as WAV files; <name>_L / <name>_R
// pairs become one stereo file. Road Trip's SOUND folder has three of them (about an hour each, at
// 12 kHz). The sequenced music (TSQ sequences, TVB sample banks) is not decoded here.

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    uint32_t be32(const uint8_t *p) { return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3]; }

    // PlayStation ADPCM: 16-byte blocks of 28 samples (shift and filter, flags, 14 data bytes).
    std::vector<int16_t> decodeAdpcm(const uint8_t *d, size_t size)
    {
        static const int f0[5] = {0, 60, 115, 98, 122}, f1[5] = {0, 0, -52, -55, -60};
        std::vector<int16_t> out;
        out.reserve(size / 16 * 28);
        int s1 = 0, s2 = 0;
        for (size_t b = 0; b + 16 <= size; b += 16)
        {
            const int shift = d[b] & 0x0F, filter = std::min(d[b] >> 4, 4);
            const uint8_t flags = d[b + 1];
            if (flags == 7) // end of data
                break;
            for (int i = 0; i < 28; ++i)
            {
                const int nibble = (d[b + 2 + i / 2] >> ((i & 1) * 4)) & 0xF;
                int s = int(int16_t(uint16_t(nibble << 12))) >> shift;
                s += (s1 * f0[filter] + s2 * f1[filter] + 32) >> 6;
                s = std::clamp(s, -32768, 32767);
                s2 = s1;
                s1 = s;
                out.push_back(int16_t(s));
            }
        }
        return out;
    }

    bool readVag(const fs::path &path, std::vector<int16_t> &samples, uint32_t &rate)
    {
        std::ifstream in(path, std::ios::binary);
        std::vector<uint8_t> d((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        if (d.size() < 48 || std::string(d.begin(), d.begin() + 4) != "VAGp")
            return false;
        rate = be32(&d[16]);
        const size_t size = std::min<size_t>(be32(&d[12]), d.size() - 48);
        samples = decodeAdpcm(d.data() + 48, size);
        return true;
    }

    void writeWav(const fs::path &path, const std::vector<int16_t> &interleaved, uint32_t rate, uint16_t channels)
    {
        std::ofstream out(path, std::ios::binary);
        auto u32 = [&](uint32_t v) { out.write(reinterpret_cast<const char *>(&v), 4); };
        auto u16 = [&](uint16_t v) { out.write(reinterpret_cast<const char *>(&v), 2); };
        const uint32_t bytes = uint32_t(interleaved.size() * 2);
        out.write("RIFF", 4);
        u32(36 + bytes);
        out.write("WAVEfmt ", 8);
        u32(16);
        u16(1);
        u16(channels);
        u32(rate);
        u32(rate * channels * 2);
        u16(channels * 2);
        u16(16);
        out.write("data", 4);
        u32(bytes);
        out.write(reinterpret_cast<const char *>(interleaved.data()), bytes);
    }
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::fprintf(stderr, "usage: %s <disc dir> <output dir>\n", argv[0]);
        return 2;
    }
    const fs::path disc = argv[1], outDir = argv[2];
    std::error_code ec;
    fs::create_directories(outDir, ec);
    std::map<std::string, std::array<fs::path, 2>> pairs; // stem without _L/_R -> left, right
    std::vector<fs::path> singles;
    for (const auto &e : fs::recursive_directory_iterator(disc, fs::directory_options::follow_directory_symlink, ec))
    {
        if (!e.is_regular_file())
            continue;
        std::string ext = e.path().extension().string();
        for (auto &c : ext)
            c = char(std::toupper(static_cast<unsigned char>(c)));
        if (ext != ".VAG")
            continue;
        const std::string stem = e.path().stem().string();
        if (stem.size() > 2 && (stem.substr(stem.size() - 2) == "_L" || stem.substr(stem.size() - 2) == "_R"))
            pairs[(e.path().parent_path() / stem.substr(0, stem.size() - 2)).string()][stem.back() == 'R'] = e.path();
        else
            singles.push_back(e.path());
    }
    int written = 0;
    for (auto &[base, lr] : pairs)
    {
        std::vector<int16_t> l, r;
        uint32_t rl = 0, rr = 0;
        const bool haveL = !lr[0].empty() && readVag(lr[0], l, rl), haveR = !lr[1].empty() && readVag(lr[1], r, rr);
        const fs::path name = outDir / (fs::path(base).filename().string() + ".wav");
        if (haveL && haveR && rl == rr)
        {
            std::vector<int16_t> st(std::max(l.size(), r.size()) * 2);
            for (size_t i = 0; i < st.size() / 2; ++i)
            {
                st[2 * i] = i < l.size() ? l[i] : 0;
                st[2 * i + 1] = i < r.size() ? r[i] : 0;
            }
            writeWav(name, st, rl, 2);
            std::printf("%s: stereo, %u Hz, %.0f s\n", name.filename().string().c_str(), rl, double(st.size() / 2) / rl);
            ++written;
        }
        else
            for (int c = 0; c < 2; ++c)
                if (!lr[c].empty())
                    singles.push_back(lr[c]);
    }
    for (const auto &p : singles)
    {
        std::vector<int16_t> s;
        uint32_t rate = 0;
        if (!readVag(p, s, rate))
            continue;
        const fs::path name = outDir / (p.stem().string() + ".wav");
        writeWav(name, s, rate, 1);
        std::printf("%s: mono, %u Hz, %.0f s\n", name.filename().string().c_str(), rate, double(s.size()) / rate);
        ++written;
    }
    std::printf("%d WAV files\n", written);
    return 0;
}
