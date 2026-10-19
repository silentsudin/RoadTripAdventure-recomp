// W12 host services, wave 2 (libpad). Mirrors the oracle's ps2_stubs Pad
// family (Kernel/Stubs/Pad.cpp) for the 28 pad entries in
// config/native_host_stubs.txt — state machine, port/slot semantics, pad
// status block layout and InfoMode table are verbatim; input SOURCE differs:
// shortcut: buttons release / sticks centred (no host pad device wired to
// the kernel yet); W13 hooks rt::input so PadRead reports real controls.
// Nothing here is compiled from ps2xRuntime (decomp hard rule).
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "guest_shim.h"
#include "guest_decls.inc"

namespace guest {

#define RT_STUB_FN(addr, name)                                       \
    namespace { struct RtReg_##addr { RtReg_##addr() {               \
        rtguest::registerGuest(addr,                                 \
            reinterpret_cast<void*>(&name)); } } rt_reg_##addr##_; }

namespace {

constexpr uint8_t kPadModeDigital = 0x41;
constexpr uint8_t kPadModeDualShock = 0x73;
constexpr int32_t kPadTypeDigital = 4;
constexpr int32_t kPadTypeDualShock = 7;
constexpr uint32_t kPadStateExecCmd = 5;
constexpr uint32_t kPadStateStable = 6;

struct PadInputState {
    uint16_t buttons = 0xFFFF;  // active-low
    uint8_t rx = 0x80, ry = 0x80, lx = 0x80, ly = 0x80;
};
struct PadPortState {
    bool open = false;
    bool analogMode = false;
    bool pressureEnabled = false;
    uint16_t buttonMask = 0xFFFF;
    uint32_t dmaAddr = 0;
    uint32_t reqState = 0;
    uint32_t transientState = 0;
    uint8_t actAlign[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t smallMotor = 0, largeMotor = 0;
};
PadPortState g_padPorts[2];
uint32_t g_padFrameCount = 0;

// Oracle getMemPtr: null outside physical RAM (G() quarantines instead).
void* mem(uint32_t a) {
    const uint32_t p = a & 0x01FFFFFFu;
    return p < RT_GUEST_RAM_SIZE ? rtguest::kRam + p : nullptr;
}

bool padTrace() {
    static const bool on = [] {
        const char* e = std::getenv("RT_PAD_TRACE");
        return e && *e && *e != '0';
    }();
    return on;
}

PadPortState* padPort(uint32_t port, uint32_t slot) {
    return port < 2u && slot < 1u ? &g_padPorts[port] : nullptr;
}

uint8_t pressureValue(const PadInputState& s, const PadPortState& p, uint16_t mask) {
    if (!p.pressureEnabled || (p.buttonMask & mask) == 0u) return 0u;
    return (s.buttons & mask) == 0u ? 0xFFu : 0u;
}

// Pad status block (Pad.cpp fillPadStatus): buttons active-low, axes at
// byte 4..7, pressure bytes 8..19.
void fillPadStatus(uint8_t* data, const PadInputState& s, const PadPortState& p) {
    std::memset(data, 0, 32);
    data[1] = p.analogMode ? kPadModeDualShock : kPadModeDigital;
    data[2] = (uint8_t)(s.buttons & 0xFFu);
    data[3] = (uint8_t)((s.buttons >> 8) & 0xFFu);
    data[4] = s.rx; data[5] = s.ry; data[6] = s.lx; data[7] = s.ly;
    const uint16_t m[12] = {1u<<5, 1u<<7, 1u<<4, 1u<<6, 1u<<12, 1u<<13,
                            1u<<14, 1u<<15, 1u<<10, 1u<<8, 1u<<11, 1u<<9};
    for (int i = 0; i < 12; ++i) data[8 + i] = pressureValue(s, p, m[i]);
}

}  // namespace

uint32_t scePadEnd(uint32_t q0, ...) {
    for (auto& p : g_padPorts) p = PadPortState{};
    return 1;
}
RT_STUB_FN(0x00273778, guest::scePadEnd)

uint32_t scePadEnterPressMode(uint64_t port, uint32_t slot, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;
    p->pressureEnabled = true; p->reqState = 0; p->transientState = kPadStateExecCmd;
    return 1;
}
RT_STUB_FN(0x002744F0, guest::scePadEnterPressMode)

uint32_t scePadExitPressMode(uint64_t port, uint32_t slot, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;
    p->pressureEnabled = false; p->reqState = 0; p->transientState = kPadStateExecCmd;
    return 1;
}
RT_STUB_FN(0x00274548, guest::scePadExitPressMode)

uint32_t scePadGetButtonMask(uint32_t port, uint32_t slot, ...) {
    const PadPortState* p = padPort(port, slot);
    return p ? p->buttonMask : 0xFFFFu;
}
RT_STUB_FN(0x00274328, guest::scePadGetButtonMask)

uint32_t scePadGetDmaStr(uint32_t port, uint32_t slot, ...) {
    const PadPortState* p = padPort(port, slot);
    return p ? p->dmaAddr : 0u;  // oracle reads reg $6 for unopened ports (garbage under .inc's 2-arg decl)
}
RT_STUB_FN(0x00273A98, guest::scePadGetDmaStr)

uint32_t scePadGetFrameCount(uint64_t q0, uint32_t q1, ...) { return g_padFrameCount++; }
RT_STUB_FN(0x00273AF8, guest::scePadGetFrameCount)

uint32_t scePadGetModVersion(uint32_t q0, ...) { return 0x0200; }
RT_STUB_FN(0x00274740, guest::scePadGetModVersion)

uint32_t scePadGetPortMax(uint32_t q0, ...) { return 2; }
RT_STUB_FN(0x00274670, guest::scePadGetPortMax)

uint32_t scePadGetReqState(uint64_t port, uint32_t slot, ...) {
    const PadPortState* p = padPort(port, slot);
    return p ? p->reqState : 0u;
}
RT_STUB_FN(0x00273CE0, guest::scePadGetReqState)

uint32_t scePadGetSlotMax(uint32_t q0, ...) { return 1; }
RT_STUB_FN(0x002746D8, guest::scePadGetSlotMax)

uint32_t scePadGetState(uint32_t port, uint32_t slot, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;  // DISCONNECTED
    if (p->transientState != 0u) {
        const uint32_t s = p->transientState;  // one-shot: consumed on read
        p->transientState = 0;
        return s;
    }
    return kPadStateStable;
}
RT_STUB_FN(0x00273BC8, guest::scePadGetState)

uint32_t scePadInfoAct(uint32_t port, uint32_t slot, uint32_t act, uint32_t param_4, ...) {
    const PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;
    const int32_t a = (int32_t)act;
    if (a < 0) return 2;  // small + large motors
    return a < 2 ? 1u : 0u;
}
RT_STUB_FN(0x00273D68, guest::scePadInfoAct)

uint64_t scePadInfoComb(uint64_t q0, uint32_t q1, uint64_t q2, uint64_t q3, ...) {
    return 0;
}
RT_STUB_FN(0x00273E88, guest::scePadInfoComb)

uint32_t scePadInfoMode(uint32_t port, uint32_t slot, uint32_t infoMode, uint32_t index, ...) {
    const PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;
    const int32_t curId = p->analogMode ? kPadTypeDualShock : kPadTypeDigital;
    switch (infoMode) {
    case 1: case 2: return (uint32_t)curId;               // MODECURID / MODECUREXID
    case 3: return 0;                                     // MODECUROFFS
    case 4: return index == -1 ? 1u : (index == 0 ? (uint32_t)curId : 0u);  // MODETABLE
    default: return 0;
    }
}
RT_STUB_FN(0x00273FA8, guest::scePadInfoMode)

uint32_t scePadInfoPressMode(uint64_t port, uint32_t slot, ...) {
    const PadPortState* p = padPort(port, slot);
    return p && p->open ? 1u : 0u;
}
RT_STUB_FN(0x00274490, guest::scePadInfoPressMode)

uint32_t scePadInit(uint64_t mode, ...) {
    for (auto& p : g_padPorts) p = PadPortState{};
    return 1;
}
RT_STUB_FN(0x00273598, guest::scePadInit)

uint32_t scePadInit2(uint32_t q0, ...) {
    for (auto& p : g_padPorts) p = PadPortState{};
    return 1;
}
RT_STUB_FN(0x002736D8, guest::scePadInit2)

uint32_t scePadPortClose(uint32_t port, uint32_t slot, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p) return 0;
    p->open = false; p->pressureEnabled = false;
    p->reqState = 0; p->transientState = 0;
    return 1;
}
RT_STUB_FN(0x002739E0, guest::scePadPortClose)

uint32_t scePadPortOpen(uint64_t port, uint64_t slot, uint32_t dmaAddr, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p || (dmaAddr != 0u && !mem(dmaAddr))) return 0;
    p->open = true; p->analogMode = false; p->pressureEnabled = false;
    p->buttonMask = 0xFFFFu; p->dmaAddr = dmaAddr;
    p->reqState = 0; p->transientState = 0;
    for (auto& a : p->actAlign) a = 0xFF;
    p->smallMotor = p->largeMotor = 0;
    if (dmaAddr) std::memset(mem(dmaAddr), 0, 32);  // libpad clears the status block
    return 1;
}
RT_STUB_FN(0x002737F8, guest::scePadPortOpen)

uint32_t scePadRead(uint32_t port, uint32_t slot, uint32_t dataAddr, ...) {
    uint8_t* data = (uint8_t*)mem(dataAddr);
    if (!data) return 0;
    const PadPortState* p = padPort(port, slot);
    if (!p || !p->open) {
        data[0] = 0xFF;  // libpad's "missing pad" marker
        return 0;
    }
    PadInputState state;  // shortcut: neutral (see file header)
    fillPadStatus(data, state, *p);
    return 1;
}
RT_STUB_FN(0x00273B48, guest::scePadRead)

uint32_t scePadSetActAlign(uint64_t port, uint64_t slot, uint32_t alignAddr, ...) {
    const uint8_t* align = (const uint8_t*)mem(alignAddr);
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open || !align) return 0;
    std::memcpy(p->actAlign, align, sizeof p->actAlign);
    if (padTrace())
        std::fprintf(stderr, "[pad] port %u SetActAlign %02x %02x %02x %02x %02x %02x\n",
                     port, p->actAlign[0], p->actAlign[1], p->actAlign[2],
                     p->actAlign[3], p->actAlign[4], p->actAlign[5]);
    p->reqState = 0; p->transientState = kPadStateExecCmd;
    return 1;
}
RT_STUB_FN(0x00274250, guest::scePadSetActAlign)

uint32_t scePadSetActDirect(uint32_t port, uint32_t slot, uint32_t dataAddr, ...) {
    const uint8_t* data = (const uint8_t*)mem(dataAddr);
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open || !data) return 0;
    uint8_t small = 0, large = 0;
    for (int i = 0; i < 6; ++i) {  // align byte picks the motor (Pad.cpp)
        if (p->actAlign[i] == 0u) small = data[i] != 0u ? 1u : 0u;
        else if (p->actAlign[i] == 1u) large = data[i];
    }
    if (padTrace() && (small != p->smallMotor || large != p->largeMotor))
        std::fprintf(stderr, "[pad] port %u SetActDirect small %u large %u\n",
                     port, small, large);
    p->smallMotor = small; p->largeMotor = large;
    // W13: route to rt::input rumble here (oracle ps2_test::onActuator).
    return 1;
}
RT_STUB_FN(0x00274198, guest::scePadSetActDirect)

uint32_t scePadSetButtonInfo(uint64_t port, uint64_t slot, uint32_t mask, ...) {
    PadPortState* p = padPort(port, slot);
    if (p && p->open) {
        p->buttonMask = (uint16_t)mask;
        p->reqState = 0; p->transientState = kPadStateExecCmd;
    }
    return 1;
}
RT_STUB_FN(0x002743E0, guest::scePadSetButtonInfo)

uint32_t scePadSetMainMode(uint64_t port, uint64_t slot, uint32_t mode, uint32_t lock, ...) {
    PadPortState* p = padPort(port, slot);
    if (!p || !p->open) return 0;
    p->analogMode = mode != 0u;
    if (padTrace())
        std::fprintf(stderr, "[pad] port %u SetMainMode %u lock %u\n", port, mode, lock);
    p->reqState = 0; p->transientState = kPadStateExecCmd;
    return 1;
}
RT_STUB_FN(0x002740E0, guest::scePadSetMainMode)

uint32_t scePadSetReqState(uint32_t port, uint32_t slot, uint32_t req, ...) {
    PadPortState* p = padPort(port, slot);
    if (p && p->open) {
        p->reqState = req ? 1u : 0u;
        p->transientState = kPadStateExecCmd;
    }
    return 1;
}
RT_STUB_FN(0x00273C78, guest::scePadSetReqState)

uint32_t scePadSetVrefParam(uint64_t q0, uint64_t q1, uint32_t q2, ...) { return 1; }
RT_STUB_FN(0x002745A0, guest::scePadSetVrefParam)

uint32_t scePadSetWarningLevel(uint32_t q0, ...) { return 0; }
RT_STUB_FN(0x002747A8, guest::scePadSetWarningLevel)

uint32_t scePadStateIntToStr(uint32_t state, uint32_t strAddr, ...) {
    char* buf = (char*)mem(strAddr);
    if (!buf) return (uint32_t)-1;
    const char* text = state == 6u ? "STABLE" : state == 1u ? "FINDPAD"
                   : state == 5u ? "EXECCMD" : state == 0u ? "DISCONNECTED" : "UNKNOWN";
    std::strncpy(buf, text, 31);
    buf[31] = '\0';
    return 0;
}
RT_STUB_FN(0x00273C40, guest::scePadStateIntToStr)

}  // namespace guest
