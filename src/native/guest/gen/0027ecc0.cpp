// guest:0x0027ecc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __sigtramp_r(uint32_t param_1, uint32_t param_2, ...) {
uint32_t pcVar1;
  undefined4 uVar2;
  long lVar3;
uint32_t puVar4;
  int iVar5;
  
  uVar2 = 0xffffffff;
  if (param_2 < 0x20) {
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4)));
    if (iVar5 == 0) {
      lVar3 = _init_signal_r(param_1);  // 27ecf0: $4 intact since 27ece0 entry
      if (lVar3 != 0) {
        return 0xffffffff;
      }
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1d4)));
    }
    puVar4 = (uint32_t)((int)param_2 * 4 + iVar5);
    pcVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar4));
    uVar2 = 1;
    if (((pcVar1 != (uint32_t)0x0) && (uVar2 = 2, pcVar1 != (uint32_t)0xffffffff)) &&
       (uVar2 = 3, pcVar1 != (uint32_t)0x1)) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 0;
      rtCall(pcVar1, param_2);
      uVar2 = 0;
    }
  }
  return uVar2;
}


RT_GUEST_FN(0x0027ecc0, guest::__sigtramp_r)

}  // namespace guest
