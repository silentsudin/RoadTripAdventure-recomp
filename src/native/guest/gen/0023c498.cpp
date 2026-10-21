// guest:0x0023c498 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of switchD_0023c440: a2 = index (stride 0x1c, MIPS 0x23c49c-0x23c4a0), a3 = record. Table 0x2eca8a.
uint32_t switchD_0023c440_caseD_1(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint32_t param_3 = a2;
  uint32_t param_4 = a3;
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x002eca8a + param_3 * 0x1c))) != 0) {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) | 0x400;
    return 0;
  }
  (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(param_4 + 8))) & 0xfbff;
  return 0;
}


RT_GUEST_FN(0x0023c498, guest::switchD_0023c440_caseD_1)

}  // namespace guest
