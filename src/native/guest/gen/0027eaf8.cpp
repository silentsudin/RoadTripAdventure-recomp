// guest:0x0027eaf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _init_signal_r(uint32_t param_1, ...) {
  undefined4 uVar1;
uint32_t puVar2;
  long lVar3;
  int iVar4;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x1d4))) == 0) {
    lVar3 = _malloc_r(param_1,0x80);
    uVar1 = 0xffffffff;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x1d4))) = (int)lVar3;
    if (lVar3 != 0) {
      puVar2 = (uint32_t)((int)lVar3 + 0x7c);
      iVar4 = 0x1f;
      do {
        iVar4 = iVar4 + -1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar2)) = 0;
        puVar2 = puVar2 + -1;
      } while (-1 < iVar4);
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


RT_GUEST_FN(0x0027eaf8, guest::_init_signal_r)

}  // namespace guest
