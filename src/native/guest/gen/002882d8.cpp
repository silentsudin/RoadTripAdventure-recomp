// guest:0x002882d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRemoveRpcQueue(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  DIntr();
  iVar4 = (*(int32_t*)rtguest::G(0x1778868));
  if ((*(int32_t*)rtguest::G(0x1778868)) == param_1) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1778868)) + 0x14)));
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G(0x1778868));
    if ((*(int32_t*)rtguest::G(0x1778868)) != 0) {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1778868)) + 0x14)));
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x1778868)) + 0x14))) == param_1) {
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
      }
      else {
        do {
          iVar4 = iVar2;
          if (iVar4 == 0) goto LAB_00288348;
          iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14)));
        } while ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) != param_1);
        uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
      }
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x14))) = uVar3;
      iVar1 = (*(int32_t*)rtguest::G(0x1778868));
    }
  }
LAB_00288348:
  (*(int32_t*)rtguest::G(0x1778868)) = iVar1;
  EIntr();
  return iVar4;
}


RT_GUEST_FN(0x002882d8, guest::sceSifRemoveRpcQueue)

}  // namespace guest
