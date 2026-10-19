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
#include <unordered_map>
#include "guest_kernel.h"
#include "guest_shim.h"
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

// Data-plane seam: walk the submission, count words, hand payloads to the
// (future) native GS/VIF consumer. Shortcut: walk-and-count only (see header).
void consumeChannel(uint32_t channelBase, uint32_t chcr, uint32_t madr,
                    uint32_t qwc, uint32_t tadr) {
    uint64_t words = 0;
    uint32_t links = 0;
    if ((chcr & 0x4u) != 0u) {  // MODE=1 chain
        uint32_t addr = tadr;
        while (addr && links < 4096u) {
            const uint64_t* tag = reinterpret_cast<const uint64_t*>(
                rtguest::kRam + (addr & 0x01FFFFFFu));
            const uint32_t tq = static_cast<uint32_t>(tag[0] & 0xFFFFu);
            const uint32_t id = static_cast<uint32_t>((tag[0] >> 28) & 0x7u);
            const uint32_t next = static_cast<uint32_t>((tag[0] >> 32) & 0x7FFFFFFFu);
            words += 1u + tq;
            ++links;
            if (id == 1u || id == 3u) addr = next;       // CALL / END use addr
            else if (id == 0u) addr = next;              // REFE: ref-end, link
            else break;                                  // BRTC/MSKNG: stop walk
            if (id == 3u) break;                         // END terminates
        }
    } else {
        words = 1u + qwc;                                // normal: madr count (qwc words)
        (void)madr;
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
