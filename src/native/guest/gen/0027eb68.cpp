// guest:0x0027eb68 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _signal_r(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined4 uVar1;
  int iVar2;
  long lVar3;
uint32_t puVar4;
  
  if (param_2 < 0x20) {
    iVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x75) * sizeof(uint32_t)));
    if (iVar2 == 0) {
      lVar3 = _init_signal_r();
      if (lVar3 != 0) {
        return 0xffffffff;
      }
      iVar2 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x75) * sizeof(uint32_t)));
    }
    puVar4 = (uint32_t)((int)param_2 * 4 + iVar2);
    uVar1 = (*(uint32_t*)rtguest::G((uint32_t)puVar4));
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = param_3;
  }
  else {
    uVar1 = 0xffffffff;
    (*(uint32_t*)rtguest::G((uint32_t)param_1)) = 0x16;
  }
  return uVar1;
}


RT_GUEST_FN(0x0027eb68, guest::_signal_r)

}  // namespace guest
