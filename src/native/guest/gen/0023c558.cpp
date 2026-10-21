// guest:0x0023c558 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of switchD_0023c440: a2 = index (stride 0x14 for the halfword at 0x2f03ec, MIPS 0x23c55c-0x23c568;
// the byte at 0x2f03f0 is indexed by the same *0x14 stride via 0x10($a2): 0x2f03e0+0x10 -> stride 0x14, not 5).
uint32_t switchD_0023c440_caseD_b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint32_t param_3 = a2;
  uint32_t param_4 = a3;
  uint16_t uVar1 = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x002f03ec + param_3 * 0x14)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 7))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(0x002f03f0 + param_3 * 0x14)));
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) & 0xfcdb | uVar1;
  return 0;
}


RT_GUEST_FN(0x0023c558, guest::switchD_0023c440_caseD_b)

}  // namespace guest
