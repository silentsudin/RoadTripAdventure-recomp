// W12 host services, wave 3 (libdma). Mirrors the oracle's ps2_stubs DMA
// family (Kernel/Stubs/DMA.cpp + Helpers/Support.h submitDmaSend/
// submitDmaSync) for the 14 dma entries in config/native_host_stubs.txt.
// Register file: the oracle keeps DMAC regs in PS2Memory's IO map; here the
// kRam slots at the MMIO addresses ARE the register file, so guest-side
// reads stay coherent with the stubs (guest writes to DMAC regs outside
// these stubs are still unwired — that's the MMIO-staging gap, W12 task).
// shortcut: chain/normal transfers are WALKED (tags parsed, links followed,
// dmac causes fired) but payloads are not delivered — no native GS/VIF0/
// VIF1 consumer exists yet; when one lands, consumeChannel() is the seam.
// Nothing here is compiled from ps2xRuntime (decomp hard rule).
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <unordered_map>
#include <vector>
#include "guest_kernel.h"
#include "guest_shim.h"
#include "guest_vif1.h"
#include "guest_gif.h"
#include "guest_vu1.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

constexpr uint32_t DMA_REG_CTRL = 0x1000E000u;   // SIF2 block (oracle DMA.cpp:21)
constexpr uint32_t DMA_REG_PCR = 0x1000E020u;
constexpr uint32_t DMA_REG_SQWC = 0x1000E030u;
constexpr uint32_t DMA_REG_RBSR = 0x1000E040u;
constexpr uint32_t DMA_REG_RBOR = 0x1000E050u;
constexpr uint32_t DMA_REG_STADR = 0x1000E060u;

struct SceDmaEnv {
    uint8_t sts = 0, std = 0, mfd = 0, rele = 0;
    uint32_t pcr = 0, sqwc = 0, rbor = 0, rbsr = 0;
};

constexpr uint8_t kStsTable[10] = {0u, 0u, 0u, 3u, 0u, 1u, 0u, 0u, 2u, 0u};
constexpr uint8_t kStdTable[10] = {0u, 1u, 2u, 0u, 0u, 0u, 3u, 0u, 0u, 0u};
constexpr uint8_t kMfdTable[10] = {0u, 2u, 3u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};

// Support.h kDmaChannelBases: MTO0,MTO1,GIF,VIF1,VIF0,SPU2,SPU2_1,IPU,SIF0,SIF1
constexpr uint32_t kDmaChannelBases[10] = {
    0x10008000u, 0x10009000u, 0x1000A000u, 0x1000B000u, 0x1000B400u,
    0x1000C000u, 0x1000C400u, 0x1000C800u, 0x1000D000u, 0x1000D400u};

SceDmaEnv g_dmaCurrentEnv;
std::unordered_map<uint32_t, uint32_t> g_dmaPendingPolls;

// Oracle getMemPtr: null outside physical RAM.
void* mem(uint32_t a) {
    const uint32_t p = a & 0x01FFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}

bool dmaTrace() {
    static const bool on = [] {
        const char* e = std::getenv("RT_DMA_TRACE");
        return e && *e && *e != '0';
    }();
    return on;
}

uint32_t dmaRead(uint32_t reg) {
    uint32_t v = 0;
    std::memcpy(&v, rtguest::kRam + (reg & 0x01FFFFFFu), 4);
    return v;
}
void dmaWrite(uint32_t reg, uint32_t v) {
    std::memcpy(rtguest::kRam + (reg & 0x01FFFFFFu), &v, 4);
}

uint32_t resolveDmaChannelBase(uint32_t chanArg) {  // Support.h:1298
    for (uint32_t b : kDmaChannelBases)
        if (b == chanArg) return chanArg;
    if (chanArg < 10u) return kDmaChannelBases[chanArg];
    return 0;
}

uint32_t toDmaPhys(uint32_t addr) {                 // Support.h:1240
    if ((addr & 0x80000000u) != 0) {
        const uint32_t lower = addr & 0x7FFFFFFFu;
        if (lower >= 0x70000000u && lower < 0x70004000u) return lower;  // scratchpad
    }
    return addr & 0x01FFFFFFu;
}

uint32_t normalizeQwcFromArg(uint32_t value) {      // Support.h:1254
    if (value == 0) return 0;
    if (value > 0xFFFFu) return std::min<uint32_t>((value + 15u) >> 4u, 0xFFFFu);
    return value & 0xFFFFu;
}

// Data plane: walk the submission per the oracle's chain semantics and hand the
// VIF1 channel's concatenated stream to the native interpreter.
void consumeChannel(uint32_t channelBase, uint32_t chcr, uint32_t madr,
                    uint32_t qwc, uint32_t tadr) {
    // W3 step 1 (docs/native-reference/w13-gs-sink-design.md): VIF1 channel payloads
    // are decoded by the native interpreter (guest_vif1.cpp); RT_VIF_HIST reporting
    // lives there. The GIF channel payload is GIF packets, decoded in guest_gif.cpp.
    static const bool vifHist = std::getenv("RT_VIF_HIST") != nullptr;
    // W3 step 2: the MSCAL/MSCALF seam runs the transliterated VU1 programs
    // (guest_vu1.cpp); step 3a: their XGKICK output and the GIF channel payload
    // decode in the GIF front end (guest_gif.cpp).
    static const bool streamsWired = (rtguest::vif1SetHooks(&rtguest::vu1OnMscal, nullptr),
                                      rtguest::vu1SetKick(&rtguest::gifSink), true);
    (void)streamsWired;
    static std::map<uint32_t, uint64_t> gifSegs; // GIF chain segs by tag upper bits[61:58]
    static uint64_t gsegs = 0, gqw = 0;
    uint64_t words = 0;
    uint32_t links = 0;
    if ((chcr & 0x4u) != 0u) {  // MODE=1 chain — mirrors ps2_memory.cpp:1565-1685.
        // Tag ids per the oracle: 0 REFE (payload at addr, chain ends), 1 CNT (payload
        // follows the tag, chain continues after it), 2 REF (payload follows, chain jumps
        // to addr), 3/4 CALL-class (payload at addr, continue after tag), 5 CALL (push
        // return addr), 6 RET (pop), 7 END. With CHCR.TTE the tag's upper 64 bits join
        // the VIF stream as two VIF codes ahead of the payload — the game carries its
        // per-object STCYCL/MSCAL there, so skipping them drifts the decode.
        // shortcut: RAM-only addresses (no scratchpad chains), guest RAM masks like the
        // rest of the shim; add SPR spans if a chain ever parks tags in scratchpad.
        const bool tagCodes = channelBase == 0x10009000u && (chcr & 0x40u) != 0u;
        const bool tie = (chcr & (1u << 7)) != 0u;
        uint32_t tagAddr = tadr, asp = 0, asr0 = 0, asr1 = 0;
        static std::vector<uint8_t> chainBuf;
        chainBuf.clear();
        bool endChain = false;
        for (; links < 4096u && !endChain && tagAddr != 0u; ++links) {
            const uint32_t physTag = tagAddr & 0x01FFFFFFu;
            if (physTag + 16u > RT_GUEST_RAM_SIZE) break;
            const std::uint64_t* tp = reinterpret_cast<const std::uint64_t*>(
                rtguest::kRam + physTag);
            const std::uint64_t tag = tp[0];
            const uint32_t tq = static_cast<uint32_t>(tag & 0xFFFFu);
            const uint32_t id = static_cast<uint32_t>((tag >> 28) & 0x7u);
            const bool irq = ((tag >> 31) & 1ull) != 0ull;
            const uint32_t addr = static_cast<uint32_t>((tag >> 32) & 0x7FFFFFFFu);
            words += 1u + tq;
            if (vifHist && channelBase == 0x1000A000u) {  // GIF tags: path/NPIX field
                ++gifSegs[static_cast<uint32_t>((tp[1] >> 58) & 0x3Fu)];
                if (gsegs++ % 500u == 0u) {
                    std::fprintf(stderr, "[gifhist] segs=%llu qw=%llu\n",
                                 (unsigned long long)gsegs, (unsigned long long)gqw);
                    for (auto& [k, c] : gifSegs)
                        std::fprintf(stderr, "  taghi=%#x segs=%llu\n", k,
                                     (unsigned long long)c);
                }
                gqw += tq;
            }
            uint32_t dataAddr = 0;
            bool hasPayload = tq > 0u;
            endChain = false;
            switch (id) {
            case 0: dataAddr = addr; tagAddr += 16u; endChain = true; break;
            case 1: dataAddr = tagAddr + 16u; tagAddr = dataAddr + tq * 16u; break;
            case 2: dataAddr = tagAddr + 16u; tagAddr = addr; break;
            case 3: case 4: dataAddr = addr; tagAddr += 16u; break;
            case 5: {
                dataAddr = tagAddr + 16u;
                const uint32_t retAddr = dataAddr + tq * 16u;
                if (asp == 0u) { asr0 = retAddr; asp = 1u; }
                else if (asp == 1u) { asr1 = retAddr; asp = 2u; }
                tagAddr = addr;
                break;
            }
            case 6:
                dataAddr = tagAddr + 16u;
                if (asp == 2u) { tagAddr = asr1; asp = 1u; }
                else if (asp == 1u) { tagAddr = asr0; asp = 0u; }
                else endChain = true;
                break;
            case 7: dataAddr = tagAddr + 16u; endChain = true; break;
            default: hasPayload = false; endChain = true; break;
            }
            if (tagCodes) {
                const std::uint8_t* up = rtguest::kRam + physTag + 8u;
                chainBuf.insert(chainBuf.end(), up, up + 8u);
            }
            if (hasPayload && (channelBase == 0x10009000u || channelBase == 0x1000A000u)) {
                const uint32_t phys = dataAddr & 0x01FFFFFFu;
                uint32_t len = tq * 16u;
                if (phys + len > RT_GUEST_RAM_SIZE) len = RT_GUEST_RAM_SIZE - phys;
                chainBuf.insert(chainBuf.end(), rtguest::kRam + phys,
                                rtguest::kRam + phys + len);
            }
            if (irq && tie) endChain = true;
        }
        if (!chainBuf.empty()) {
            const uint32_t len = static_cast<std::uint32_t>(chainBuf.size());
            if (channelBase == 0x1000A000u)
                rtguest::gifSink(chainBuf.data(), len);  // payload carries its GIF tags
            else
                rtguest::vif1Consume(chainBuf.data(), len);
        }
        // Oracle register write-back (ps2_memory.cpp:1687-1690) sans CHCR: the callers
        // own CHCR completion, and re-writing STR here would re-trap.
        dmaWrite(channelBase + 0x30u, tagAddr);
        dmaWrite(channelBase + 0x40u, asr0);
        dmaWrite(channelBase + 0x50u, asr1);
    } else {
        words = 1u + qwc;                                // normal: madr count (qwc words)
        if (channelBase == 0x10009000u)
            rtguest::vif1Consume(rtguest::kRam + (madr & 0x01FFFFFFu), qwc * 16u);
        else if (channelBase == 0x1000A000u)
            rtguest::gifSink(rtguest::kRam + (madr & 0x01FFFFFFu), qwc * 16u);
    }
    if (dmaTrace())
        std::fprintf(stderr, "[dma] ch=0x%x chcr=0x%x madr=0x%x qwc=0x%x tadr=0x%x "
                             "words=%llu links=%u (not delivered)\n",
                     channelBase, chcr, madr, qwc, tadr,
                     static_cast<unsigned long long>(words), links);
}

int32_t submitSend(uint32_t chanArg, uint32_t payloadArg, uint32_t countArg,
                   bool preferNormalCount) {  // Support.h:1338
    const uint32_t channelBase = resolveDmaChannelBase(chanArg);
    if (channelBase == 0) return -1;
    const uint32_t payloadPhys = toDmaPhys(payloadArg);
    uint32_t madr = 0, qwc = 0;
    const uint32_t tadr = payloadPhys;

    const uint32_t configuredChcr = dmaRead(channelBase + 0x00u);
    const uint32_t transferTagEnable = configuredChcr & 0x00000040u;
    uint32_t chcr = 0x00000181u | transferTagEnable;
    if (preferNormalCount) {
        qwc = normalizeQwcFromArg(countArg);
        madr = payloadPhys;
    } else {
        chcr = 0x00000185u | transferTagEnable;  // MODE=1 chain, DIR=1, TIE=1, STR=1
    }
    dmaWrite(channelBase + 0x20u, qwc & 0xFFFFu);
    dmaWrite(channelBase + 0x10u, madr);
    dmaWrite(channelBase + 0x30u, tadr);
    dmaWrite(channelBase + 0x00u, chcr);

    consumeChannel(channelBase, chcr, madr, qwc, tadr);
    dmaWrite(channelBase + 0x00u, chcr & ~0x101u);  // transfer complete: STR cleared, not busy

    // Oracle: processPendingTransfers records causes; absent that, libdma's
    // completion fallback maps the channel to its cause (Support.h:1383-1394).
    uint32_t cause = 0xFFFFFFFFu;
    if (channelBase == 0x10008000u) cause = 0u;
    else if (channelBase == 0x10009000u) cause = 1u;
    else if (channelBase == 0x1000A000u) cause = 2u;
    if (cause != 0xFFFFFFFFu) rtguest::kFireDmacCause(cause);

    g_dmaPendingPolls[channelBase] = 1;
    return 0;
}

int32_t submitSync(uint32_t chanArg, uint32_t mode) {  // Support.h:1440
    const uint32_t channelBase = resolveDmaChannelBase(chanArg);
    if (channelBase == 0) return -1;
    // Direct-MMIO kick (guest wrote CHCR.STR itself, e.g. sendTexturePackets
    // writing 0x105 into the kRam reg slot): our data plane is synchronous, so
    // complete it now — clear STR/busy and fire the channel cause.
    uint32_t chcr = dmaRead(channelBase + 0x00u);
    if ((chcr & 0x101u) == 0x101u && g_dmaPendingPolls.find(channelBase) ==
                                        g_dmaPendingPolls.end()) {
        dmaWrite(channelBase + 0x00u, chcr & ~0x101u);
        if (channelBase == 0x10008000u) rtguest::kFireDmacCause(0u);
        else if (channelBase == 0x10009000u) rtguest::kFireDmacCause(1u);
        else if (channelBase == 0x1000A000u) rtguest::kFireDmacCause(2u);
    }
    bool modelBusy = false;
    auto it = g_dmaPendingPolls.find(channelBase);
    if (it != g_dmaPendingPolls.end() && it->second > 0) {
        modelBusy = true;
        if (mode != 0) {
            --it->second;
            if (it->second == 0) g_dmaPendingPolls.erase(it);
        } else {
            g_dmaPendingPolls.erase(it);  // blocking: completes here (native = now)
        }
    }
    const bool hwBusy = (dmaRead(channelBase + 0x00u) & 0x100u) != 0;
    return ((modelBusy || hwBusy) && mode != 0) ? 1 : 0;
}

}  // namespace

// CHCR write trap for the typed MMIO staging slots (guest_shim.h MmioReg):
// a guest store to CHCR with STR set is a DMA kick — the oracle's recompiled
// sendTexturePackets (0x207b50) and vifKick (0x2271f0) hit PS2Memory's DMAC
// device with these stores; our transliterated bodies write the Ghidra-named
// reg globals, so the device effect belongs here (docs/native-reference/
// w12-kernel-design.md, MMIO staging). Payload args come from the channel's
// own staging regs, exactly as the kickers wrote them a moment earlier.
void eeDmaChcrTrap(uint32_t regAddr, uint32_t x) {
    constexpr uint32_t kVif1Chcr = 0x10009000u, kGifChcr = 0x1000A000u;
    if (regAddr != kVif1Chcr && regAddr != kGifChcr) return;
    const bool gif = regAddr == kGifChcr;
    const uint32_t madr = toDmaPhys(gif ? REG_DMAC_2_GIF_MADR : REG_DMAC_1_VIF1_MADR);
    const uint32_t qwc = (gif ? REG_DMAC_2_GIF_QWC : REG_DMAC_1_VIF1_QWC) & 0xFFFFu;
    const uint32_t tadr = toDmaPhys(gif ? REG_DMAC_2_GIF_TADR : REG_DMAC_1_VIF1_TADR);
    if ((x & 0x1u) != 0u) {  // STR: walk + complete (synchronous data plane)
        consumeChannel(regAddr, x, madr, qwc, tadr);
        g_dmaPendingPolls[regAddr] = 1;
        rtguest::kFireDmacCause(gif ? 2u : 1u);
    }
    dmaWrite(regAddr, x & ~0x101u);  // completion visible in the reg file
}

uint32_t sceDmaGetChan(uint32_t chanArg, ...) {
    return resolveDmaChannelBase(chanArg);
}
RT_STUB_FN(0x00270180, guest::sceDmaGetChan)

uint32_t sceDmaGetEnv(uint32_t envAddr, ...) {
    if (uint8_t* dst = (uint8_t*)mem(envAddr)) {
        std::memcpy(dst, &g_dmaCurrentEnv, sizeof(g_dmaCurrentEnv));
    }
    return envAddr;
}
RT_STUB_FN(0x00270470, guest::sceDmaGetEnv)

uint32_t sceDmaPutEnv(uint32_t envAddr, ...) {
    const uint8_t* src = (const uint8_t*)mem(envAddr);
    if (!src) return -1;
    SceDmaEnv env{};
    std::memcpy(&env, src, sizeof(env));
    if (env.sts >= 10) return -1;
    if (env.std >= 10) return -2;
    if (env.mfd >= 10) return -3;
    if (env.rele >= 7u) return -4;

    uint32_t ctrl = dmaRead(DMA_REG_CTRL);
    ctrl = (ctrl & 0xFFFFFFCFu) | (static_cast<uint32_t>(kStsTable[env.sts]) << 4);
    ctrl = (ctrl & 0xFFFFFF3Fu) | (static_cast<uint32_t>(kStdTable[env.std]) << 6);
    ctrl = (ctrl & 0xFFFFFFF3u) | (static_cast<uint32_t>(kMfdTable[env.mfd]) << 2);
    if (env.rele == 0u) {
        ctrl &= 0xFFFFFFFDu;
    } else {
        ctrl = ((ctrl | 0x2u) & 0xFFFFFCFFu) |
               ((static_cast<uint32_t>(env.rele - 1u) & 0x7u) << 8);
    }
    dmaWrite(DMA_REG_CTRL, ctrl);
    dmaWrite(DMA_REG_PCR, env.pcr);
    dmaWrite(DMA_REG_SQWC, env.sqwc);
    dmaWrite(DMA_REG_RBOR, env.rbor);
    dmaWrite(DMA_REG_RBSR, env.rbsr);
    g_dmaCurrentEnv = env;
    return 0;
}
RT_STUB_FN(0x00270298, guest::sceDmaPutEnv)

uint32_t sceDmaPutStallAddr(uint32_t newAddr, ...) {
    const uint32_t oldAddr = dmaRead(DMA_REG_STADR);
    if (newAddr != 0xFFFFFFFFu) dmaWrite(DMA_REG_STADR, newAddr);
    return oldAddr;
}
RT_STUB_FN(0x002704A8, guest::sceDmaPutStallAddr)

uint32_t sceDmaReset(uint32_t q0, ...) {
    // libdma reset leaves the controller runnable; DMAE must be re-enabled
    // or chain submissions are accepted but never execute (DMA.cpp:173).
    dmaWrite(DMA_REG_CTRL, 0u);
    dmaWrite(DMA_REG_PCR, 0u);
    dmaWrite(DMA_REG_SQWC, 0u);
    dmaWrite(DMA_REG_RBOR, 0u);
    dmaWrite(DMA_REG_RBSR, 0u);
    dmaWrite(DMA_REG_STADR, 0u);
    dmaWrite(DMA_REG_CTRL, 1u);
    g_dmaCurrentEnv = {};
    g_dmaPendingPolls.clear();
    return 0;
}
RT_STUB_FN(0x002701A8, guest::sceDmaReset)

uint32_t sceDmaSend(uint32_t chanArg, uint32_t payloadArg, ...) {
    return submitSend(chanArg, payloadArg, 0, false);  // chain mode: count unused
}
RT_STUB_FN(0x002704D0, guest::sceDmaSend)

uint32_t sceDmaSendI(uint32_t chanArg, uint32_t payloadArg, uint32_t countArg, ...) {
    return submitSend(chanArg, payloadArg, countArg, false);
}
RT_STUB_FN(0x00270690, guest::sceDmaSendI)

uint32_t sceDmaSendN(uint32_t chanArg, uint32_t payloadArg, uint32_t countArg, ...) {
    return submitSend(chanArg, payloadArg, countArg, true);
}
RT_STUB_FN(0x002705A8, guest::sceDmaSendN)

uint32_t sceDmaSync(uint32_t chanArg, uint32_t mode, uint32_t param_3, ...) {
    (void)param_3;
    return submitSync(chanArg, mode);
}
RT_STUB_FN(0x00270A20, guest::sceDmaSync)

}  // namespace guest

rtguest::MmioReg& rtguest::MmioReg::operator=(uint32_t x) {
    value = x;
    guest::eeDmaChcrTrap(addr, x);
    value &= ~0x101u;  // STR/tag-int auto-clear on completion: unblocks guest poll loops
    return *this;
}
