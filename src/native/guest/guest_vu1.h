// W3 gs-sink step 2: Road Trip's VU1 vertex programs as native code
// (docs/native-reference/w13-gs-sink-design.md). Transliteration of the guest VU1
// micro image the game uploads (captured hash-verified, FNV-1a 0xe2868a5859386847;
// full disassembly build/scratch/w3-step2/entry{2,5}_*.dis). Each block cites its
// micro-op range. The oracle's own reference HLE and its interpreter are comparison
// ground truth only (vu1_replay --hle vs build/vu1hle-caps captures) — nothing of
// them links here, decomp hard rule.
#pragma once
#include <cstdint>

namespace rtguest {

// VU1 register state as the programs leave it: it persists across runs (SetupLit
// writes the matrices LitStrip reads; VF28.w carries each batch's last fog word
// into the next batch's STQ.w).
struct Vu1Regs {
    float vf[32][4] = {};
    std::int32_t vi[16] = {};   // 16-bit semantics: writes truncate to int16
    float acc[4] = {};
    float q = 0.f, p = 0.f, i = 0.f;
    std::uint32_t clip = 0;
};
Vu1Regs& vu1();

// XGKICK output sink (fed to the render chain in step 3; tests take the bytes).
using Vu1KickFn = void (*)(const std::uint8_t* packet, std::uint32_t bytes);
void vu1SetKick(Vu1KickFn fn);

// MSCAL/MSCALF seam (guest_vif1): runs the programs ported at the slot PCs
// (Init 0x00, SetupLit 0x10, LitStrip 0x20, OffsetStrip 0x50); other entry PCs
// are counted as unported until transliterated.
void vu1OnMscal(std::uint32_t pc, std::uint32_t top, std::uint32_t itop);
std::uint64_t vu1Runs();
std::uint64_t vu1Unported();

}  // namespace rtguest
