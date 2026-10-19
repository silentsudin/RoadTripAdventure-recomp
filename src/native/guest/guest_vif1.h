// W3 gs-sink step 1: native VIF1 interpreter (docs/native-reference/w13-gs-sink-design.md).
// Semantics mirrored from the oracle's working VIF1 worker (ps2xRuntime/src/lib/
// ps2_vif1_interpreter.cpp, cited inline; that file is comparison ground truth only —
// nothing of it links here, decomp hard rule).
#pragma once
#include <cstdint>

namespace rtguest {

// VIF1 control registers as the interpreter maintains them (oracle regs).
struct Vif1Regs {
    std::uint32_t cycle = 0x00010000;   // STCYCL: cl=bits7:0, wl=bits15:8 (0 -> 1)
    std::uint16_t ofst = 0;             // OFFSET
    std::uint16_t base = 0;             // BASE
    std::uint16_t tops = 0;             // pending TOP (double-buffered)
    std::uint16_t itops = 0;            // pending ITOP
    std::uint16_t top = 0;              // latched at MSCAL/MSCNT
    std::uint16_t itop = 0;
    std::uint16_t mark = 0;             // MARK (+ stat.MRK)
    std::uint32_t mask = 0;             // STMASK field selects
    std::uint32_t row[4] = {};          // STROW
    std::uint32_t col[4] = {};          // STCOL
    std::uint32_t mode = 0;             // STMOD
    std::uint32_t code = 0;             // last VIFcode (VIFn_CODE)
    std::uint32_t num = 0;              // last command's NUM field (VIF1_NUM)
    std::uint32_t stat = 0;             // bit6 MRK, bit7 DBF, bit11 INT
};

inline Vif1Regs& vif1() { static Vif1Regs r; return r; }
inline std::uint8_t* vu1Code() { static std::uint8_t m[16u * 1024u]; return m; }  // oracle PS2_VU1_CODE_SIZE
inline std::uint8_t* vu1Data() { static std::uint8_t m[16u * 1024u]; return m; }  // oracle PS2_VU1_DATA_SIZE

// W3 step-2 seams: MSCAL/MSCALF hand the entry PC + latched TOP/ITOP to the ported
// VU1 entry programs; DIRECT hands its GIF payload block. Null = record-only.
using Vif1MscalFn = void (*)(std::uint32_t pc, std::uint32_t top, std::uint32_t itop);
using Vif1DirectFn = void (*)(const std::uint8_t* data, std::uint32_t bytes, bool directHl);
void vif1SetHooks(Vif1MscalFn mscal, Vif1DirectFn direct);

// Consume one VIF1 payload segment (chain-tag payload or normal madr block).
void vif1Consume(std::uint32_t segAddr, std::uint32_t qwcWords);

// RT_VIF_HIST counters (fed by vif1Consume; printed by the DMA diag).
bool vif1HistOn();
void vif1HistDump();
std::uint64_t vif1HistSegs();
std::uint64_t vif1HistMscals();
std::uint64_t vif1HistUnpackWrites();
std::uint64_t vif1HistDirectQw();
void vif1MaybeDumpCode();

}  // namespace rtguest
