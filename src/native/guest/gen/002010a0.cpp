// guest:0x002010a0 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of uiKanaGlyphUV's switch: t1 = &u (set at 0x201008), t0 = &v; MIPS 0x2010a0..0x201168.
uint32_t switchD_0020101c_caseD_8176(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t t0, uint32_t t1, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint16_t*)rtguest::G((uint32_t)t1)) = 0;
  (*(int16_t*)rtguest::G((uint32_t)t0)) = (*(int16_t*)rtguest::G((uint32_t)t0)) + 0xb4;
  return 0;
}


RT_GUEST_FN(0x002010a0, guest::switchD_0020101c_caseD_8176)

}  // namespace guest
