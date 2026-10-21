// guest:0x002335a8 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of the switchD_002334bc dispatcher: a2 = destination block. Ghidra folded the 0x2ec298 template
// (read-only data, MIPS 0x2335b8..0x23360c copies 0x2c bytes) into constants; word +0x28 is stored 0 then 2.0f
// (MIPS 0x233840 sw / 0x233844 swc1) and +0x24 = 0x1e (delay slot 0x23384c). param_3+5 is 8-byte scaled: +0x28.
uint32_t switchD_002334bc_caseD_fffffffe(uint32_t a0, uint32_t a1, uint32_t a2, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint32_t param_3 = a2;
  (*(uint64_t*)rtguest::G((uint32_t)param_3)) = 0x14000000003;
  (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(uint64_t))) = 0xe0;
  (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(uint64_t))) = 0x29fb8800000000;
  (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(3) * sizeof(uint64_t))) = 1;
  (*(uint64_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(4) * sizeof(uint64_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(param_3 + 5 * 8))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)(param_3 + 5 * 8))) = 0x40000000;
  (*(uint32_t*)rtguest::G((uint32_t)((int)param_3 + 0x24))) = 0x1e;
  return 0;
}


RT_GUEST_FN(0x002335a8, guest::switchD_002334bc_caseD_fffffffe)

}  // namespace guest
