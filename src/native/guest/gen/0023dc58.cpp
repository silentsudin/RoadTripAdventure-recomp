// guest:0x0023dc58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpendTimerBudget(uint32_t param_1, uint32_t param_2, ...) {
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
      PlayUiSound(0x305);
      uVar1 = 0;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0023dc58, guest::SpendTimerBudget)

}  // namespace guest
