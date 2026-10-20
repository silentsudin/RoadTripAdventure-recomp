// W3 step 3a: GIF front end (docs/native-reference/w13-gs-sink-design.md).
// Tag/loop/REGS semantics mirror the oracle gs_frontend.cpp (processGifPacket
// packed/reglist/image walk, PRE prim in the tag, kick on XYZ(F)2 with ADK
// clear); register-value decoding beyond PRIM/counts lands with the renderer
// feed. The oracle is the fidelity reference and is NOT linked here.
#include "guest_gif.h"

#include "guest_gs.h"
#include "guest_vif1.h"  // shares the RT_VIF_HIST census gate

#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

namespace rtguest {
namespace {

struct {
    uint64_t prim = 0;  // last PRIM setting (A+D tag or packed reg 0x0)
} st;

uint64_t g_tags[4] = {};  // tags by FLG: 0 packed, 1 reglist, 2 image, 3 bad
uint64_t g_verts = 0, g_kicks = 0, g_imageBytes = 0, g_pre = 0;
std::map<uint32_t, uint64_t> g_kickPrim;  // prim shape (bits 2:0) -> kicks
std::map<uint32_t, uint64_t> g_regs;      // REGS code -> data writes
std::map<uint32_t, uint64_t> g_direct;    // DIRECT (0xE) target GS reg addr
std::map<uint32_t, uint64_t> g_xyzw;      // probe: XYZF2 fog/flag word histogram

inline uint64_t le64(const uint8_t* p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return v;
}

void countReg(uint8_t reg, uint64_t lo, uint64_t hi, bool adk) {
    ++g_regs[reg];
    switch (reg) {
    case 0x0:
        st.prim = lo & 0x7FFu;
        break;
    case 0x1:  // RGBAQ
    case 0x2:  // STQ
    case 0x3:  // UV
        break;  // values decode with the renderer feed
    case 0x4:  // XYZF2
    case 0x5:  // XYZ2
        ++g_verts;
        if (reg == 0x4) ++g_xyzw[(hi >> 32) & 0xFFFFu];  // probe: fog/flag word
        if (!adk) {  // ADK clear: oracle vertexKick(!adk)
            ++g_kicks;
            ++g_kickPrim[static_cast<uint32_t>(st.prim & 7u)];
        }
        break;
    case 0xe:  // GIF DIRECT: GS reg addr in hi 7:0, value in lo (oracle :1171)
        ++g_direct[static_cast<uint32_t>(hi & 0xFFu)];
        break;
    default:
        break;  // TEX0/CLAMP/FOG/FRAME/... decode with the renderer feed
    }
}

}  // namespace

bool gifCensusOn() { return vif1HistOn(); }

uint32_t gifSinkConsume(const uint8_t* data, uint32_t sizeBytes) {
    uint32_t off = 0;
    while (off + 16u <= sizeBytes) {
        const uint32_t tagStart = off;
        const uint64_t lo = le64(data + off), hi = le64(data + off + 8);
        off += 16u;
        const uint32_t nloop = static_cast<uint32_t>(lo & 0x7FFFu);
        const uint32_t flg = static_cast<uint32_t>((lo >> 58) & 3u);
        uint32_t nreg = static_cast<uint32_t>((lo >> 60) & 0xFu);
        if (nreg == 0u) nreg = 16u;
        const uint64_t payload = flg == 0u ? static_cast<uint64_t>(nloop) * nreg * 16u
                           : flg == 1u ? ((static_cast<uint64_t>(nloop) * nreg * 8u + 15u) & ~15ull)
                           : flg == 2u ? static_cast<uint64_t>(nloop) * 16u : 0u;
        if (flg == 3u) { ++g_tags[flg]; return sizeBytes; }  // FLG 3 (reserved): the oracle drops the stream
        if (off + payload > sizeBytes) return tagStart;  // partial packet: carried, reparsed
        ++g_tags[flg];
        if (((lo >> 46) & 1u) != 0u) {  // PRE: prim carried in the tag
            ++g_pre;
            ++g_regs[0x0];
            st.prim = (lo >> 47) & 0x7FFu;
            gsWriteRegister(0x00, (lo >> 47) & 0x7FFu);
        }
        uint8_t regs[16];
        for (uint32_t i = 0; i < nreg; ++i)
            regs[i] = static_cast<uint8_t>((hi >> (i * 4)) & 0xFu);
        if (flg == 0u) {  // PACKED
            const uint64_t bytes = payload;
            for (uint64_t i = 0; i < bytes; i += 16u) {
                const uint64_t rlo = le64(data + off + i),
                               rhi = le64(data + off + i + 8);
                countReg(regs[(i / 16u) % nreg], rlo, rhi,
                         ((rhi >> 47) & 1u) != 0u);
                gsWritePacked(regs[(i / 16u) % nreg], rlo, rhi);
            }
            off += static_cast<uint32_t>(bytes);
        } else if (flg == 1u) {  // REGLIST
            const uint64_t n = static_cast<uint64_t>(nloop) * nreg;
            for (uint64_t i = 0; i < n; ++i) {
                const uint64_t v = le64(data + off + i * 8u);
                countReg(regs[i % nreg], v, 0u, ((v >> 47) & 1u) != 0u);
                gsWriteRegister(regs[i % nreg], v);
            }
            off += static_cast<uint32_t>(payload);  // odd n pads 8
        } else {  // IMAGE
            g_imageBytes += payload;
            gsImageTransfer(data + off, static_cast<uint32_t>(payload));
            off += static_cast<uint32_t>(payload);
        }
    }
    return off;
}

void gifSink(const uint8_t* data, uint32_t sizeBytes) { (void)gifSinkConsume(data, sizeBytes); }

// VIF1 DIRECT/DIRECTHL payload = GIF PATH2 stream (oracle
// ps2_vif1_interpreter.cpp:652-686 forwards it whole, hl flag bookkeeping only).
// Packets may split across DIRECT commands: keep the partial tail and reparse it
// with the next chunk (the oracle re-tags split IMAGE uploads; same VRAM result).
void gifPath2Direct(const uint8_t* data, uint32_t sizeBytes, bool) {
    static std::vector<uint8_t> carry;
    if (carry.empty()) {
        const uint32_t used = gifSinkConsume(data, sizeBytes);
        if (used < sizeBytes) carry.assign(data + used, data + sizeBytes);
        return;
    }
    carry.insert(carry.end(), data, data + sizeBytes);
    const uint32_t used = gifSinkConsume(carry.data(), static_cast<uint32_t>(carry.size()));
    carry.erase(carry.begin(), carry.begin() + used);
    if (carry.size() > 65536u) carry.clear();  // shortcut: desync guard; real packets are <2 KB
}

uint64_t gifTagCount(uint32_t flg) { return flg < 4u ? g_tags[flg] : 0u; }
uint64_t gifVertsCount() { return g_verts; }
uint64_t gifKicksCount() { return g_kicks; }
uint64_t gifImageBytesCount() { return g_imageBytes; }

void gifCensusDump() {
    std::fprintf(stderr,
                 "[gifcensus] tags P=%llu R=%llu I=%llu X=%llu pre=%llu "
                 "verts=%llu kicks=%llu imageB=%llu\n",
                 (unsigned long long)g_tags[0], (unsigned long long)g_tags[1],
                 (unsigned long long)g_tags[2], (unsigned long long)g_tags[3],
                 (unsigned long long)g_pre, (unsigned long long)g_verts,
                 (unsigned long long)g_kicks, (unsigned long long)g_imageBytes);
    for (auto& [r, c] : g_regs)
        std::fprintf(stderr, "  reg=%#x n=%llu\n", r, (unsigned long long)c);
    for (auto& [a, c] : g_direct)
        std::fprintf(stderr, "  gsreg=%#x n=%llu\n", a, (unsigned long long)c);
    uint32_t shown = 0;
    for (auto& [w, c] : g_xyzw)
        if (c > 2000u) { std::fprintf(stderr, "  xyzw=%#x n=%llu\n", w, (unsigned long long)c); if (++shown >= 20u) break; }
    for (auto& [p, c] : g_kickPrim)
        std::fprintf(stderr, "  kickprim=%u n=%llu\n", p, (unsigned long long)c);
}

}  // namespace rtguest
