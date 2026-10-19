// guest:0x00279e08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t mceIntrReadFixAlign(uint32_t param_1, ...) {
uint32_t piVar1;
  int iVar2;
  int iVar3;
uint32_t puVar4;
  
  piVar1 = (uint32_t)(param_1 | 0x20000000);
  if ((*(int32_t*)rtguest::G((uint32_t)piVar1)) == 0) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(1) * sizeof(int32_t)));
  }
  else {
    puVar4 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(2) * sizeof(int32_t)));
    iVar2 = 0;
    if (0 < (*(int32_t*)rtguest::G((uint32_t)piVar1))) {
      do {
        iVar3 = iVar2 + 0x10;
        iVar2 = iVar2 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar1 + iVar3)));
        puVar4 = puVar4 + 1;
      } while (iVar2 < (*(int32_t*)rtguest::G((uint32_t)piVar1)));
    }
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(1) * sizeof(int32_t)));
  }
  if (iVar2 != 0) {
    puVar4 = (uint32_t)(*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(3) * sizeof(int32_t)));
    iVar3 = 0;
    if (0 < iVar2) {
      do {
        iVar2 = iVar3 + 0x50;
        iVar3 = iVar3 + 1;
        (*(uint8_t*)rtguest::G((uint32_t)puVar4)) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)piVar1 + iVar2)));
        puVar4 = puVar4 + 1;
      } while (iVar3 < (*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(1) * sizeof(int32_t))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x00279e08, guest::mceIntrReadFixAlign)

}  // namespace guest
