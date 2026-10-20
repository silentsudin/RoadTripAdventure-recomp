// guest:0x0022e7f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DrawScoreAndRemaining(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
uint32_t puVar1;
  undefined4 uVar2;
  
  RT_CALLRA(0x22e820, guest::DrawDialogTextLine(0x1b8,0x152,(*(int32_t*)rtguest::G(0x177fdb4)),0x177fd9f));
  uVar2 = 0;
  puVar1 = (uint32_t)((*(int32_t*)rtguest::G(0x177fdb4)) - param_1);
  if ((int)puVar1 < 0) {
    uVar2 = 0xffffffff;
  }
  else if (999999 < (int)puVar1) {
    uVar2 = 1;
    puVar1 = 0x000f423f;
  }
  RT_CALLRA(0x22e874, guest::DrawDialogTextLine(0x1b8,0x182,puVar1,0x177fd9f));
  RT_CALLRA(0x22e898, guest::uiDrawIcon20(0x204,((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)param_2)) >> 1) & 7) + 0x171,7));
  return uVar2;
}


RT_GUEST_FN(0x0022e7f0, guest::DrawScoreAndRemaining)

}  // namespace guest
