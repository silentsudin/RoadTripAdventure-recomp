// guest:0x002832c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _fwalk(uint32_t param_1, uint32_t param_2, ...) {
  ulong uVar1;
  int iVar2;
  int iVar3;
uint32_t piVar4;
  ulong uVar5;
  
  piVar4 = (uint32_t)(param_1 + 0x1d8);
  uVar5 = 0;
  if (piVar4 != (uint32_t)0x0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1dc)));
    while( true ) {
      iVar3 = (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(2) * sizeof(int32_t)));
      while (iVar2 = iVar2 + -1, -1 < iVar2) {
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) != 0) {
          uVar1 = rtCall(param_2, iVar3);
          uVar5 = uVar5 | uVar1;
        }
        iVar3 = iVar3 + 0x58;
      }
      piVar4 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar4));
      if (piVar4 == (uint32_t)0x0) break;
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar4 + (uint32_t)(1) * sizeof(int32_t)));
    }
  }
}


RT_GUEST_FN(0x002832c0, guest::_fwalk)

}  // namespace guest
