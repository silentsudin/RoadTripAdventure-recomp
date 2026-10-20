// guest:0x0022e8c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawAltScoreAndRemaining(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
uint32_t puVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  puVar1 = (uint32_t)((*(int32_t*)rtguest::G(0x177fdb4)) - param_1);
  if ((int)puVar1 < 0) {
    uVar2 = 0xffffffff;
  }
  else if (999999 < (int)puVar1) {
    uVar2 = 1;
    puVar1 = 0x000f423f;
  }
  RT_CALLRA(0x22e91c, guest::DrawDialogTextLine(0x1b8,0x4a,puVar1,0x177fd9f));
  RT_CALLRA(0x22e940, guest::uiDrawIcon20(0x204,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x39,7));
  return uVar2;
}


RT_GUEST_FN(0x0022e8c0, guest::DrawAltScoreAndRemaining)

}  // namespace guest
