// guest:0x00201118 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of uiKanaGlyphUV's switch: t1 = &u, t0 = &v; MIPS 0x201118..0x20112c (v += 0x60, not 0xb4).
uint32_t switchD_0020101c_caseD_818b(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t t0, uint32_t t1, ...) {
  rtguest::FrameGuard rt_fg(0);
  (*(uint16_t*)rtguest::G((uint32_t)t1)) = 0xc0;
  (*(int16_t*)rtguest::G((uint32_t)t0)) = (*(int16_t*)rtguest::G((uint32_t)t0)) + 0x60;
  return 0;
}


RT_GUEST_FN(0x00201118, guest::switchD_0020101c_caseD_818b)

}  // namespace guest
