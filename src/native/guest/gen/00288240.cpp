// guest:0x00288240 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRemoveRpc(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  DIntr();
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8)));
  if (iVar3 == param_1) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 8))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38)));
  }
  else if (iVar3 != 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38))) == param_1) {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
    }
    else {
      do {
        iVar3 = iVar1;
        if (iVar3 == 0) goto LAB_002882b0;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38)));
      } while ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38))) != param_1);
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x38)));
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x38))) = uVar2;
  }
LAB_002882b0:
  EIntr();
  return iVar3;
}


RT_GUEST_FN(0x00288240, guest::sceSifRemoveRpc)

}  // namespace guest
