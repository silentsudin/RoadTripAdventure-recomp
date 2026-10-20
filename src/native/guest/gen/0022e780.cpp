// guest:0x0022e780 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void DrawSlotScoreNumber(uint64_t param_1, uint32_t param_2, uint32_t param_3) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  DrawDialogTextLine(param_1,param_2 * 2 + -6,(*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_3 * 0xd12) * 4)),
                     0x0177fd9f + param_3 * 0xd12);
  return;
}


RT_GUEST_FN(0x0022e780, guest::DrawSlotScoreNumber)

}  // namespace guest
