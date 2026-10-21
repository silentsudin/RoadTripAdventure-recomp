// guest:0x00234524 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// 0x234524 is a nop (MIPS) that falls straight into ResetStateBlock at 0x234528; Ghidra inlined its body here.
uint32_t FUN_00234524(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  return ResetStateBlock(param_1);
}


RT_GUEST_FN(0x00234524, guest::FUN_00234524)

}  // namespace guest
