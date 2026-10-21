// guest:0x00201130 hand-ported from Ghidra pseudocode (build/scratch/w13-orphan/REPORT.md) — table-called fragment
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Fragment of uiKanaGlyphUV's switch default arm: a2 = code (MIPS 0x201130 sltu on $a2), t1 = &u, t0 = &v.
// Pseudocode param_3 (ulong) is $a2 as the full register; MIPS 0x201144 masks the sum to 16 bits.
uint32_t switchD_0020101c_caseD_8143(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t t0, uint32_t t1, ...) {
  rtguest::FrameGuard rt_fg(0);
  int iVar1;
  uint uVar2;
  uint32_t param_3 = a2;
  uint32_t param_5 = t0;
  uint32_t param_6 = t1;

  iVar1 = (int)param_3;
  if (0x8753 < param_3) {
    (*(int16_t*)rtguest::G((uint32_t)param_6)) = ((short)param_3 + 0x78ac) * 0x14 + 0x78;
    (*(int16_t*)rtguest::G((uint32_t)param_5)) = (*(int16_t*)rtguest::G((uint32_t)param_5)) + 0xb4;
    return 0;
  }
  if (0x833f < param_3) {
    uVar2 = iVar1 + 0x7cc8U & 0xffff;
    (*(int16_t*)rtguest::G((uint32_t)param_6)) = (short)(uVar2 % 0x19) * 0x14;
    (*(int16_t*)rtguest::G((uint32_t)param_5)) = (*(int16_t*)rtguest::G((uint32_t)param_5)) + (short)(uVar2 / 0x19) * 0x14 + 0x3c;
    return 0;
  }
  if (0x829e < param_3) {
    uVar2 = iVar1 + 0x7d61U & 0xffff;
    (*(int16_t*)rtguest::G((uint32_t)param_6)) = (short)(uVar2 % 0x19) * 0x14;
    (*(int16_t*)rtguest::G((uint32_t)param_5)) = (*(int16_t*)rtguest::G((uint32_t)param_5)) + (short)(uVar2 / 0x19) * 0x14;
    return 0;
  }
  if (0x825f < param_3) {
    uVar2 = iVar1 + 0x7da4U & 0xffff;
    (*(int16_t*)rtguest::G((uint32_t)param_6)) = (short)(uVar2 % 0x19) * 0x14;
    (*(int16_t*)rtguest::G((uint32_t)param_5)) = (*(int16_t*)rtguest::G((uint32_t)param_5)) + (short)(uVar2 / 0x19) * 0x14 + 0x8c;
    return 0;
  }
  uVar2 = iVar1 + 0x7dc4U & 0xffff;
  (*(int16_t*)rtguest::G((uint32_t)param_6)) = (short)(uVar2 % 0x19) * 0x14;
  (*(int16_t*)rtguest::G((uint32_t)param_5)) = (*(int16_t*)rtguest::G((uint32_t)param_5)) + (short)(uVar2 / 0x19) * 0x14 + 0x78;
  return 0;
}


RT_GUEST_FN(0x00201130, guest::switchD_0020101c_caseD_8143)

}  // namespace guest
