// guest:0x0023dc58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpendTimerBudget(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_2 * 0xd12) * 4)) - param_1;
  uVar1 = 0xffffffff;
  if (param_1 <= (int)(*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_2 * 0xd12) * 4))) {
    (*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_2 * 0xd12) * 4)) = iVar2;
    if (999999 < iVar2) {
      (*(uint32_t*)rtguest::G((uint32_t)0x177fdb4 + (uint32_t)(param_2 * 0xd12) * 4)) = 0x000f423f;
    }
    uVar1 = 0;
    if (param_1 != 0) {
      RT_CALLRA(0x23dca8, guest::PlayUiSound(0x305));
      uVar1 = 0;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0023dc58, guest::SpendTimerBudget)

}  // namespace guest
