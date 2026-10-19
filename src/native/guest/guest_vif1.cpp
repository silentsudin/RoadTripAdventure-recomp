// W3 gs-sink step 1: native VIF1 interpreter. Semantics mirrored with citations
// from the oracle's VIF1 worker (ps2xRuntime/src/lib/ps2_vif1_interpreter.cpp:
// opcode field (word>>24)&0x7F with that file's VIFCmd enum; generic UNPACK loop
// at :759-990). Generic UNPACK path only — the oracle's fast path is an
// optimization, not a semantic. Nothing from ps2xRuntime links here (decomp rule).
#include "guest_vif1.h"
#include "guest_vu1.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

#include "guest_shim.h"

namespace rtguest {
namespace {

// Opcodes per the oracle's field layout (word bits 30:24), its enum values.
enum : std::uint32_t {
    kNop = 0x00, kStcycl = 0x01, kOffset = 0x02, kBase = 0x03, kItop = 0x04,
    kStmod = 0x05, kMskPath3 = 0x06, kMark = 0x07,
    kFlushe = 0x10, kFlush = 0x11, kFlusha = 0x13,
    kMscal = 0x14, kMscalf = 0x15, kMscnt = 0x17,
    kStmask = 0x20, kStrow = 0x30, kStcol = 0x31,
    kMpg = 0x4A, kDirect = 0x50, kDirectHl = 0x51,
};
constexpr std::uint32_t kVU1CodeSize = 16u * 1024u, kVU1DataSize = 16u * 1024u;

Vif1MscalFn g_mscal = nullptr;
Vif1DirectFn g_direct = nullptr;

// RT_VIF_HIST counters.
std::map<std::uint32_t, std::uint64_t> g_ops;
std::uint64_t g_segs = 0, g_mscals = 0, g_unpackWrites = 0, g_directQw = 0;
bool g_histInit = false, g_histOn = false;

bool histOn() {
    if (!g_histInit) { g_histInit = true; g_histOn = std::getenv("RT_VIF_HIST") != nullptr; }
    return g_histOn;
}

// MSCAL/MSCALF/MSCNT shared latch + DBF rotation (oracle :570-611).
void mscLatch() {
    Vif1Regs& r = vif1();
    r.top = r.tops & 0x3FFu;
    r.itop = r.itops & 0x3FFu;
    const bool dbf = (r.stat & (1u << 7)) != 0u;
    r.tops = dbf ? (r.base & 0x3FFu) : ((r.base + r.ofst) & 0x3FFu);
    r.stat ^= (1u << 7);
}

}  // namespace

void vif1SetHooks(Vif1MscalFn mscal, Vif1DirectFn direct) {
    g_mscal = mscal;
    g_direct = direct;
}

bool vif1HistOn() { return histOn(); }
std::uint64_t vif1HistSegs() { return g_segs; }
std::uint64_t vif1HistMscals() { return g_mscals; }
std::uint64_t vif1HistUnpackWrites() { return g_unpackWrites; }
std::uint64_t vif1HistDirectQw() { return g_directQw; }

// W3 step 2 aid: RT_VIF1_CODE_DUMP=<path> writes the 16KB VU1 micro image at exit.
void vif1MaybeDumpCode() {
    static bool done = false;
    const char* p = std::getenv("RT_VIF1_CODE_DUMP");
    if (!p || !*p || done) return;
    done = true;
    if (FILE* f = std::fopen(p, "wb")) { std::fwrite(vu1Code(), 1, 16u * 1024u, f); std::fclose(f); }
}

void vif1HistDump() {
    std::fprintf(stderr, "[vifhist] segs=%llu mscals=%llu unpackWrites=%llu directQw=%llu\n",
                 (unsigned long long)g_segs, (unsigned long long)g_mscals,
                 (unsigned long long)g_unpackWrites, (unsigned long long)g_directQw);
    std::fprintf(stderr, "[vu1] runs=%llu unported=%llu\n",
                 (unsigned long long)vu1Runs(), (unsigned long long)vu1Unported());
    for (auto& [k, c] : g_ops) {
        if (k > 0xFFu) std::fprintf(stderr, "  op=%#x mscal_pc=%#x count=%llu\n", k & 0xFFu,
                                    ((k >> 8) * 8u), (unsigned long long)c);
        else std::fprintf(stderr, "  op=%#x count=%llu\n", k, (unsigned long long)c);
    }
}

void vif1Consume(const std::uint8_t* data, std::uint32_t sizeBytes) {
    // One decoded unit, as the oracle's processVIF1Data(data, sizeBytes): the DMA
    // layer concatenates a chain's payload spans (and TTE tag-upper VIF codes).
    std::uint32_t pos = 0;
    Vif1Regs& r = vif1();
    const bool hist = histOn();
    if (++g_segs % 2000u == 0u && hist) vif1HistDump();

    while (pos + 4 <= sizeBytes) {
        std::uint32_t w;
        std::memcpy(&w, data + pos, 4);
        pos += 4;
        const std::uint32_t opcode = (w >> 24) & 0x7Fu;
        const std::uint16_t imm = static_cast<std::uint16_t>(w & 0xFFFFu);
        const std::uint8_t num = static_cast<std::uint8_t>((w >> 16) & 0xFFu);
        r.code = w;
        r.num = num;
        if (w & 0x80000000u) r.stat |= (1u << 11);  // irq bit -> VIF1_STAT.INT (oracle :515)
        if (hist && opcode != kMscal && opcode != kMscalf) ++g_ops[opcode];

        switch (opcode) {
        case kNop: case kFlushe: case kFlush: case kFlusha:
            continue;
        case kStcycl: r.cycle = imm; continue;                       // oracle :518
        case kOffset:                                                  // oracle :522-529
            r.ofst = imm & 0x3FFu;
            r.tops = r.base & 0x3FFu;
            r.stat &= ~(1u << 7);
            continue;
        case kBase:  r.base = imm & 0x3FFu; continue;                 // oracle :531
        case kItop:  r.itops = imm & 0x3FFu; continue;                // oracle :536
        case kStmod: r.mode = imm & 3u; continue;                     // oracle :540
        case kMskPath3: break;                                        // path-3 mask: no Path3 draws emitted yet
        case kMark:  r.mark = imm; r.stat |= (1u << 6); continue;     // oracle :550
        case kMscal: case kMscalf: {                                  // oracle :560-585
            const std::uint32_t pc = static_cast<std::uint32_t>(imm) * 8u;
            mscLatch();
            ++g_mscals;
            if (hist)
                g_ops[opcode | (static_cast<std::uint32_t>(imm & 0x3FFu) << 8)] += 1;
            if (g_mscal) g_mscal(pc, r.top, r.itop);
            continue;
        }
        case kMscnt: mscLatch(); continue;                            // oracle :587-600
        case kStmask: {
            if (pos + 4 > sizeBytes) return;
            std::memcpy(&r.mask, data + pos, 4);
            pos += 4;
            continue;                                                 // oracle :602-610
        }
        case kStrow: {
            if (pos + 16 > sizeBytes) return;
            std::memcpy(r.row, data + pos, 16);
            pos += 16;
            continue;                                                 // oracle :612-619
        }
        case kStcol: {
            if (pos + 16 > sizeBytes) return;
            std::memcpy(r.col, data + pos, 16);
            pos += 16;
            continue;                                                 // oracle :622-628
        }
        case kMpg: {                                                   // oracle :630-650
            const std::uint32_t dest = static_cast<std::uint32_t>(imm) * 8u;
            const std::uint32_t n = num == 0u ? 256u : num;
            std::uint32_t bytes = n * 8u;
            if (pos + bytes > sizeBytes) bytes = sizeBytes - pos;      // truncated segment: take what fits
            if (dest < kVU1CodeSize && bytes) {
                const std::uint32_t copyBytes =
                    (dest + bytes > kVU1CodeSize) ? kVU1CodeSize - dest : bytes;
                std::memcpy(vu1Code() + dest, data + pos, copyBytes);
            }
            pos += bytes;
            if (pos > sizeBytes) return;
            continue;
        }
        case kDirect: case kDirectHl: {                                // oracle :652-686
            std::uint32_t qw = imm == 0u ? 65536u : imm;
            const std::uint32_t avail = (sizeBytes - pos) / 16u;
            const bool truncated = qw > avail;
            if (qw > avail) qw = avail;
            if (qw) {
                g_directQw += qw;
                if (g_direct) g_direct(data + pos, qw * 16u, opcode == kDirectHl);
            }
            pos += qw * 16u;
            if (truncated) return;
            continue;
        }
        default: break;
        }

        if ((opcode & 0x60u) != 0x60u) continue;  // unknown control word: skip 4 (oracle :988)

        // UNPACK (oracle :688-986). vn = bits 3:2, vl = bit 1:0, mask bit 0x10.
        const std::uint32_t vn = (opcode >> 2) & 3u;
        const std::uint32_t vl = opcode & 3u;
        const bool maskEnable = (opcode & 0x10u) != 0u;
        const int components = static_cast<int>(vn) + 1;
        std::uint32_t bytesPerVector;
        switch (vl) {
        case 0: bytesPerVector = components * 4u; break;
        case 1: bytesPerVector = components * 2u; break;
        case 2: bytesPerVector = components * 1u; break;
        default: bytesPerVector = (vn == 3u) ? 2u : components * 2u; break;  // vl==3
        }
        // (oracle keeps bitsPerVector; ours is already bytes — no /8.)
        const std::uint32_t writes = num == 0u ? 256u : num;
        std::uint32_t cl = r.cycle & 0xFFu, wl = (r.cycle >> 8) & 0xFFu;
        if (!cl) cl = 1;
        if (!wl) wl = 1;
        std::uint32_t sourceVectors = writes;
        if (cl < wl) {
            const std::uint32_t full = writes / wl, rem = writes % wl;
            sourceVectors = full * cl + (rem > cl ? cl : rem);
        }
        std::uint32_t totalBytes = sourceVectors * bytesPerVector;
        totalBytes = (totalBytes + 3u) & ~3u;
        std::uint32_t vuAddr = imm & 0x3FFu;
        if (imm & 0x8000u) vuAddr = (vuAddr + (r.tops & 0x3FFu)) & 0x3FFu;  // W bit (oracle :736)
        const bool zeroExtend = (imm & 0x4000u) != 0u;                       // Z bit (oracle :738)
        if (totalBytes == 0 || pos + totalBytes > sizeBytes) return;         // truncated: stop segment

        std::uint32_t srcIndex = 0;
        for (std::uint32_t wi = 0; wi < writes; ++wi) {
            const std::uint32_t cyclePos = wi % wl;
            const bool srcOk = (cl >= wl) || (cyclePos < cl);
            const std::uint32_t destVec =
                (cl >= wl) ? ((vuAddr + (wi / wl) * cl + cyclePos) & 0x3FFu)
                           : ((vuAddr + wi) & 0x3FFu);
            const std::uint32_t destOff = destVec * 16u;
            if (destOff + 16u > kVU1DataSize) {
                if (srcOk && srcIndex < sourceVectors) ++srcIndex;
                continue;                                                   // oracle :789-795
            }
            std::uint32_t lanes[4] = {};
            std::memcpy(lanes, vu1Data() + destOff, 16);
            std::uint32_t dec[4] = {lanes[0], lanes[1], lanes[2], lanes[3]};
            bool decoded = false, handledFormat = true;
            const std::uint8_t* srcVec = nullptr;
            if (srcOk && srcIndex < sourceVectors) {
                srcVec = data + pos + srcIndex * bytesPerVector;
                ++srcIndex;
                decoded = true;
            } else {
                handledFormat = false;
            }
            auto e16 = [&](std::uint16_t v) { return zeroExtend ? std::uint32_t(v)
                                           : std::uint32_t(static_cast<std::int32_t>(static_cast<std::int16_t>(v))); };
            auto e8 = [&](std::uint8_t v) { return zeroExtend ? std::uint32_t(v)
                                          : std::uint32_t(static_cast<std::int32_t>(static_cast<std::int8_t>(v))); };
            if (decoded && vl == 0u) {
                if (components == 1) { std::memcpy(&dec[0], srcVec, 4); dec[1] = dec[2] = dec[3] = dec[0]; }
                else for (int c = 0; c < components && c < 4; ++c) std::memcpy(&dec[c], srcVec + c * 4u, 4);
            } else if (decoded && vl == 1u) {
                std::uint16_t raw;
                if (components == 1) { std::memcpy(&raw, srcVec, 2); dec[0] = e16(raw); dec[1] = dec[2] = dec[3] = dec[0]; }
                else for (int c = 0; c < components && c < 4; ++c) { std::memcpy(&raw, srcVec + c * 2u, 2); dec[c] = e16(raw); }
            } else if (decoded && vl == 2u) {
                if (components == 1) { dec[0] = e8(srcVec[0]); dec[1] = dec[2] = dec[3] = dec[0]; }
                else for (int c = 0; c < components && c < 4; ++c) dec[c] = e8(srcVec[c]);
            } else if (decoded && vl == 3u && vn == 3u) {                // V4-1080 (oracle :916-922)
                std::uint16_t packed;
                std::memcpy(&packed, srcVec, 2);
                dec[0] = packed & 0x1Fu;
                dec[1] = (packed >> 5) & 0x1Fu;
                dec[2] = (packed >> 10) & 0x1Fu;
                dec[3] = (packed >> 15) & 1u;
            } else if (decoded) {
                handledFormat = false;
            }
            if (!handledFormat && decoded && !maskEnable && (r.mode == 0u || r.mode == 3u)) {
                const std::uint32_t cp = bytesPerVector < 16u ? bytesPerVector : 16u;  // raw-copy fallback (oracle :931-937)
                std::memcpy(vu1Data() + destOff, srcVec, cp);
                ++g_unpackWrites;
                continue;
            }
            const bool canAdd = !(vl == 3u && vn == 3u);
            const std::uint32_t mode = r.mode & 3u;
            const std::uint32_t colIdx = cyclePos > 3u ? 3u : cyclePos;
            for (std::uint32_t field = 0; field < 4u; ++field) {        // oracle :940-981
                std::uint32_t maskSpec = 0u;
                if (maskEnable)
                    maskSpec = (r.mask >> (((colIdx * 4u) + field) * 2u)) & 3u;
                if (!decoded && maskSpec == 0u) maskSpec = 1u;          // fill writes take ROW
                std::uint32_t val = lanes[field];
                if (maskSpec == 0u) {
                    if (handledFormat) {
                        val = dec[field];
                        if (canAdd && (mode == 1u || mode == 2u)) {
                            val += r.row[field];
                            if (mode == 2u) r.row[field] = val;
                        }
                    }
                } else if (maskSpec == 1u) {
                    val = r.row[field];
                } else if (maskSpec == 2u) {
                    val = r.col[colIdx];
                } else {
                    continue;                                           // write-protect
                }
                lanes[field] = val;
            }
            std::memcpy(vu1Data() + destOff, lanes, 16);
            ++g_unpackWrites;
        }
        pos += totalBytes;
        if (pos > sizeBytes) return;
    }
}

}  // namespace rtguest
