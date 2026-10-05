#include "Extract.h"

#include "raylib.h"
#include "runtime/gs/ps2_gs_memory.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace rt::textures
{
    namespace
    {
        enum Psm : uint32_t
        {
            CT32 = 0x00, CT24 = 0x01, CT16 = 0x02, CT16S = 0x0A,
            T8 = 0x13, T4 = 0x14, T8H = 0x1B, T4HL = 0x24, T4HH = 0x2C,
        };

        uint32_t bitsPerPixel(uint32_t psm)
        {
            switch (psm)
            {
            case CT32: return 32;
            case CT24: return 24;
            case CT16: case CT16S: return 16;
            case T8: case T8H: return 8;
            case T4: case T4HL: case T4HH: return 4;
            default: return 0;
            }
        }

        bool indexed(uint32_t psm) { return psm == T8 || psm == T8H || psm == T4 || psm == T4HL || psm == T4HH; }
        bool fourBit(uint32_t psm) { return psm == T4 || psm == T4HL || psm == T4HH; }

        uint64_t field(uint64_t v, unsigned lo, unsigned n) { return (v >> lo) & ((1ull << n) - 1); }

        // One host-to-local transfer and the file offset it came from.
        struct Upload
        {
            size_t offset;
            uint32_t dbp, dbw, psm, dx, dy, w, h;
        };

        // A TEX0 the file sets (texture base, format, palette).
        struct Tex0
        {
            uint32_t tbp, tbw, psm, cbp, cpsm, csm, csa;
            bool operator<(const Tex0 &o) const
            {
                return std::tie(tbp, tbw, psm, cbp, cpsm, csm, csa) < std::tie(o.tbp, o.tbw, o.psm, o.cbp, o.cpsm, o.csm, o.csa);
            }
        };

        struct GsImage
        {
            std::vector<uint8_t> vram = std::vector<uint8_t>(GSMem::MEMORY_SIZE);

            void write(const Upload &u, const uint8_t *data)
            {
                uint8_t *m = vram.data();
                const uint32_t n = u.w * u.h;
                for (uint32_t i = 0; i < n; ++i)
                {
                    const uint32_t x = u.dx + i % u.w, y = u.dy + i / u.w;
                    switch (u.psm)
                    {
                    case CT32: { uint32_t v; std::memcpy(&v, data + i * 4, 4); GSMem::WriteCT32(m, u.dbp, u.dbw, x, y, v); break; }
                    case CT24: { const uint8_t *p = data + i * 3; GSMem::WriteCT24(m, u.dbp, u.dbw, x, y, p[0] | (p[1] << 8) | (p[2] << 16)); break; }
                    case CT16: { uint16_t v; std::memcpy(&v, data + i * 2, 2); GSMem::WriteCT16(m, u.dbp, u.dbw, x, y, v); break; }
                    case CT16S: { uint16_t v; std::memcpy(&v, data + i * 2, 2); GSMem::WriteCT16S(m, u.dbp, u.dbw, x, y, v); break; }
                    case T8: GSMem::WriteP8(m, u.dbp, u.dbw, x, y, data[i]); break;
                    case T8H: GSMem::WriteP8H(m, u.dbp, u.dbw, x, y, data[i]); break;
                    case T4: GSMem::WriteP4(m, u.dbp, u.dbw, x, y, (data[i / 2] >> ((i & 1) * 4)) & 0xF); break;
                    case T4HL: GSMem::WriteP4HL(m, u.dbp, u.dbw, x, y, (data[i / 2] >> ((i & 1) * 4)) & 0xF); break;
                    case T4HH: GSMem::WriteP4HH(m, u.dbp, u.dbw, x, y, (data[i / 2] >> ((i & 1) * 4)) & 0xF); break;
                    default: return;
                    }
                }
            }

            uint32_t read(uint32_t psm, uint32_t bp, uint32_t bw, uint32_t x, uint32_t y)
            {
                uint8_t *m = vram.data();
                switch (psm)
                {
                case CT32: return GSMem::ReadCT32(m, bp, bw, x, y);
                case CT24: return GSMem::ReadCT24(m, bp, bw, x, y);
                case CT16: return GSMem::ReadCT16(m, bp, bw, x, y);
                case CT16S: return GSMem::ReadCT16S(m, bp, bw, x, y);
                case T8: return GSMem::ReadP8(m, bp, bw, x, y);
                case T8H: return GSMem::ReadP8H(m, bp, bw, x, y);
                case T4: return GSMem::ReadP4(m, bp, bw, x, y);
                case T4HL: return GSMem::ReadP4HL(m, bp, bw, x, y);
                case T4HH: return GSMem::ReadP4HH(m, bp, bw, x, y);
                default: return 0;
                }
            }
        };

        // paraLLEl-GS's colour expansion (shaders/utils.h).
        uint32_t pack(int r, int g, int b, int a)
        {
            auto c = [](int v) { return static_cast<uint32_t>(std::clamp(v, 0, 255)); };
            return c(r) | (c(g) << 8) | (c(b) << 16) | (c(a) << 24);
        }

        uint32_t rgba16(uint32_t c, const ExtractOptions &o)
        {
            const int r = (c & 0x1F) << 3, g = ((c >> 5) & 0x1F) << 3, b = ((c >> 10) & 0x1F) << 3;
            const bool zero = o.aem && (c & 0xFFFF) == 0;
            return pack(r, g, b, zero ? 0 : (((c >> 15) & 1) ? int(o.ta1) : int(o.ta0)));
        }

        uint32_t rgb24(uint32_t c, const ExtractOptions &o)
        {
            const bool zero = o.aem && (c & 0xFFFFFF) == 0;
            return (c & 0xFFFFFF) | (static_cast<uint32_t>(zero ? 0 : o.ta0) << 24);
        }

        // The GS CLUT buffer after loading (CSM1) from cbp: 512 halves (32-bit entries are split
        // into a low and a high bank, as the GS stores them).
        std::array<uint16_t, 512> loadClut(GsImage &gs, uint32_t texPsm, uint32_t cbp, uint32_t cpsm, uint32_t csa)
        {
            std::array<uint16_t, 512> clut{};
            const bool sixteen = cpsm == CT16 || cpsm == CT16S;
            const uint32_t entries = fourBit(texPsm) ? 16u : 256u;
            const uint32_t base = (csa & (sixteen ? 0x1Fu : 0x0Fu)) << 4;
            for (uint32_t e = 0; e < entries; ++e)
            {
                const uint32_t s = (e & ~0x18u) | ((e & 0x08u) << 1) | ((e & 0x10u) >> 1); // CSM1: bits 3 and 4 swapped
                const uint32_t raw = gs.read(cpsm, cbp, 1, s & 0x0F, s >> 4);
                const uint32_t dst = (base + e) & (sixteen ? 0x1FFu : 0xFFu);
                if (sixteen)
                    clut[dst] = static_cast<uint16_t>(raw);
                else
                {
                    clut[dst] = static_cast<uint16_t>(raw & 0xFFFF);
                    clut[dst + 256] = static_cast<uint16_t>(raw >> 16);
                }
            }
            return clut;
        }

        std::vector<uint8_t> decode(GsImage &gs, const Upload &u, const std::array<uint16_t, 512> *clut, uint32_t cpsm,
                                    uint32_t csa, const ExtractOptions &o)
        {
            std::vector<uint8_t> rgba(size_t(u.w) * u.h * 4);
            const bool sixteen = cpsm == CT16 || cpsm == CT16S;
            for (uint32_t y = 0; y < u.h; ++y)
                for (uint32_t x = 0; x < u.w; ++x)
                {
                    const uint32_t v = gs.read(u.psm, u.dbp, u.dbw, x, y);
                    uint32_t c = 0;
                    if (indexed(u.psm))
                    {
                        // upload.comp: offset * 16 + index, 8-bit indices clamped to the last bank.
                        uint32_t i = (csa * 16 + v) & 0x1FF;
                        if (!fourBit(u.psm))
                            i = std::min(i >> 4, sixteen ? 31u : 15u) * 16 + (i & 15);
                        if (sixteen)
                            c = rgba16((*clut)[i & 0x1FF], o);
                        else
                        {
                            i &= 0xFF;
                            c = (*clut)[i] | (static_cast<uint32_t>((*clut)[i + 256]) << 16);
                            if (cpsm == CT24)
                                c = rgb24(c, o);
                        }
                    }
                    else if (u.psm == CT32)
                        c = v;
                    else if (u.psm == CT24)
                        c = rgb24(v, o);
                    else
                        c = rgba16(v, o);
                    std::memcpy(&rgba[(size_t(y) * u.w + x) * 4], &c, 4);
                }
            return rgba;
        }

        uint64_t contentHash(uint32_t w, uint32_t h, const std::vector<uint8_t> &rgba)
        {
            // Must match gs_texture_tools.cpp: FNV-1a 64 over width, height and the pixels.
            uint64_t hash = 1469598103934665603ull;
            auto mix = [&](const uint8_t *p, size_t n) {
                for (size_t i = 0; i < n; ++i)
                {
                    hash ^= p[i];
                    hash *= 1099511628211ull;
                }
            };
            mix(reinterpret_cast<const uint8_t *>(&w), 4);
            mix(reinterpret_cast<const uint8_t *>(&h), 4);
            mix(rgba.data(), rgba.size());
            return hash;
        }

        // A palette-shaped upload (32/16-bit, 8x2 / 16x2 / 16x16 and the like).
        bool clutShaped(const Upload &u)
        {
            return (u.psm == CT32 || u.psm == CT16 || u.psm == CT16S) && u.w <= 16 && (u.h <= 2 || (u.w == 16 && u.h == 16) || (u.w == 8 && u.h == 8));
        }

        // Every upload in a file: an A+D GIF packet setting BITBLTBUF..TRXDIR (host to local),
        // followed by IMAGE packets holding the pixels. TEX0s set by A+D packets are collected too.
        struct Parsed
        {
            std::vector<std::pair<Upload, std::vector<uint8_t>>> uploads;
            std::set<Tex0> tex0s;
        };

        Parsed parse(const std::vector<uint8_t> &d)
        {
            Parsed out;
            auto q64 = [&](size_t at) { uint64_t v; std::memcpy(&v, d.data() + at, 8); return v; };
            for (size_t off = 0; off + 32 <= d.size(); off += 16)
            {
                const uint64_t tag = q64(off), regs = q64(off + 8);
                const uint32_t nloop = uint32_t(tag & 0x7FFF);
                if (field(tag, 58, 2) != 0 || field(tag, 60, 4) != 1 || (regs & 0xF) != 0xE || nloop == 0 || nloop > 64)
                    continue;
                if (off + 16 * (size_t(nloop) + 1) > d.size())
                    continue;
                uint64_t bitbltbuf = 0, trxpos = 0, trxreg = 0;
                bool haveBlt = false, havePos = false, haveReg = false, valid = true;
                std::vector<uint64_t> tex0s;
                int64_t dir = -1;
                for (uint32_t i = 0; i < nloop; ++i)
                {
                    const size_t at = off + 16 * (i + 1);
                    const uint64_t value = q64(at), addr = q64(at + 8) & 0xFF; // A+D: the register is the low byte
                    if (addr > 0x63)
                    {
                        valid = false;
                        break;
                    }
                    switch (addr)
                    {
                    case 0x50: bitbltbuf = value; haveBlt = true; break;
                    case 0x51: trxpos = value; havePos = true; break;
                    case 0x52: trxreg = value; haveReg = true; break;
                    case 0x53: dir = int64_t(value & 3); break;
                    case 0x06: case 0x07: tex0s.push_back(value); break;
                    default: break;
                    }
                }
                if (!valid)
                    continue;
                for (uint64_t t : tex0s)
                {
                    Tex0 x{uint32_t(field(t, 0, 14)), uint32_t(field(t, 14, 6)), uint32_t(field(t, 20, 6)), uint32_t(field(t, 37, 14)),
                           uint32_t(field(t, 51, 4)), uint32_t(field(t, 55, 1)), uint32_t(field(t, 56, 5))};
                    if (indexed(x.psm))
                        out.tex0s.insert(x);
                }
                if (!(haveBlt && havePos && haveReg && dir == 0))
                    continue;
                Upload u{off, uint32_t(field(bitbltbuf, 32, 14)), uint32_t(field(bitbltbuf, 48, 6)), uint32_t(field(bitbltbuf, 56, 6)),
                         uint32_t(field(trxpos, 32, 11)), uint32_t(field(trxpos, 48, 11)), uint32_t(field(trxreg, 0, 12)),
                         uint32_t(field(trxreg, 32, 12))};
                const uint32_t bpp = bitsPerPixel(u.psm);
                if (!bpp || !u.w || !u.h || !u.dbw)
                    continue;
                const size_t bytes = (size_t(u.w) * u.h * bpp + 7) / 8;
                // The pixels: IMAGE packets right after.
                std::vector<uint8_t> data;
                size_t at = off + 16 * (size_t(nloop) + 1);
                while (data.size() < bytes && at + 16 <= d.size())
                {
                    const uint64_t t = q64(at);
                    if (field(t, 58, 2) != 2)
                        break;
                    const size_t n = size_t(t & 0x7FFF) * 16;
                    at += 16;
                    if (at + n > d.size())
                        break;
                    data.insert(data.end(), d.begin() + at, d.begin() + at + n);
                    at += n;
                }
                if (data.size() < bytes)
                    continue;
                data.resize(bytes);
                out.uploads.push_back({u, std::move(data)});
                off = at - 16; // continue after the pixels
            }
            return out;
        }
    }

    ExtractStats extractDisc(const std::filesystem::path &discDir, const std::filesystem::path &outDir,
                             const ExtractOptions &options, const std::function<void(const std::string &)> &progress)
    {
        GSMem::InitLookupTables();
        SetTraceLogLevel(LOG_WARNING); // raylib logs every saved file
        ExtractStats stats;
        std::error_code ec;
        std::filesystem::create_directories(outDir, ec);
        std::unordered_set<uint64_t> done;
        for (const auto &e : std::filesystem::directory_iterator(outDir, ec))
        {
            const std::string n = e.path().filename().string();
            if (n.size() > 16)
                done.insert(std::strtoull(n.substr(0, 16).c_str(), nullptr, 16));
        }
        std::ofstream csv(outDir / "sources.csv", std::ios::app);
        if (csv.tellp() == 0)
            csv << "texture,file,offset,width,height,psm,clut_offset,cpsm,csa\n";

        std::vector<std::filesystem::path> files;
        for (const auto &e : std::filesystem::recursive_directory_iterator(discDir, std::filesystem::directory_options::follow_directory_symlink, ec))
            if (e.is_regular_file())
                files.push_back(e.path());
        std::sort(files.begin(), files.end());

        for (const auto &path : files)
        {
            std::ifstream in(path, std::ios::binary);
            std::vector<uint8_t> d((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            Parsed p = parse(d);
            if (p.uploads.empty())
                continue;
            ++stats.files;
            const std::string rel = std::filesystem::relative(path, discDir).generic_string();
            if (progress)
                progress(rel + ": " + std::to_string(p.uploads.size()) + " uploads");

            // Replay the file's uploads in order; decode each texture right after its palette
            // (the upload that follows it), so later uploads to the same addresses don't interfere.
            GsImage gs;
            auto emit = [&](const Upload &u, const std::array<uint16_t, 512> *clut, uint32_t cpsm, uint32_t csa, const Upload *clutUp,
                            bool variant) {
                std::vector<uint8_t> rgba = decode(gs, u, clut, cpsm, csa, options);
                ++stats.textures;
                const uint64_t key = contentHash(u.w, u.h, rgba);
                if (!done.insert(key).second)
                    return;
                for (size_t i = 3; i < rgba.size(); i += 4)
                    rgba[i] = static_cast<uint8_t>(std::min(255u, rgba[i] * 255u / 128u));
                char name[96];
                std::snprintf(name, sizeof(name), "%016llx_%ux%u_psm%02x.png", static_cast<unsigned long long>(key), u.w, u.h, u.psm);
                // Guessed palette banks (most are never used) go in a subfolder.
                const std::filesystem::path dir = variant ? outDir / "palette-variants" : outDir;
                if (variant)
                    std::filesystem::create_directories(dir, ec);
                Image image = {rgba.data(), int(u.w), int(u.h), 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
                if (ExportImage(image, (dir / name).string().c_str()))
                    ++stats.written;
                csv << (variant ? "palette-variants/" : "") << name << ',' << rel << ',' << u.offset << ',' << u.w << ',' << u.h << ',' << u.psm << ','
                    << (clutUp ? std::to_string(clutUp->offset) : std::string()) << ',' << cpsm << ',' << csa << '\n';
            };

            for (size_t i = 0; i < p.uploads.size(); ++i)
            {
                const auto &[u, data] = p.uploads[i];
                gs.write(u, data.data());
                ++stats.uploads;
                if (u.dx || u.dy)
                    continue;
                if (!indexed(u.psm))
                {
                    if (!clutShaped(u))
                        emit(u, nullptr, 0, 0, nullptr, false);
                    continue;
                }
                // Palettes: those TEX0s in the file name for this texture, else the palette uploaded
                // right after it.
                std::vector<std::tuple<uint32_t, uint32_t, uint32_t, bool>> palettes; // cbp, cpsm, csa, guessed bank
                const Upload *clutUp = nullptr;
                for (const Tex0 &t : p.tex0s)
                    if (t.tbp == u.dbp && t.psm == u.psm && t.csm == 0)
                        palettes.emplace_back(t.cbp, t.cpsm, t.csa, false);
                if (i + 1 < p.uploads.size() && clutShaped(p.uploads[i + 1].first))
                {
                    const auto &[c, cdata] = p.uploads[i + 1];
                    gs.write(c, cdata.data());
                    clutUp = &c;
                    if (palettes.empty())
                    {
                        const uint32_t banks = fourBit(u.psm) && options.allBanks && c.w == 16 && c.h == 16 ? 16 : 1;
                        // A 256-colour palette holds sixteen 16-colour banks, picked with CSA.
                        for (uint32_t b = 0; b < banks; ++b)
                            palettes.emplace_back(c.dbp, c.psm, b, b != 0);
                        if (fourBit(u.psm) && c.w == 16 && c.h == 2)
                            palettes.emplace_back(c.dbp + 1, c.psm, 0, true); // second 8x2 palette, next block
                    }
                }
                for (auto [cbp, cpsm, csa, guessed] : palettes)
                {
                    // 16-colour banks of a 256-colour palette: load it as 8-bit does, then offset.
                    const bool bankOf256 = fourBit(u.psm) && clutUp && clutUp->w == 16 && clutUp->h == 16 && cbp == clutUp->dbp;
                    auto clut = bankOf256 ? loadClut(gs, T8, cbp, cpsm, 0) : loadClut(gs, u.psm, cbp, cpsm, csa);
                    emit(u, &clut, cpsm, csa, clutUp, guessed);
                }
            }
        }
        return stats;
    }
}
