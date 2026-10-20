// guest:0x0025aa58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t storeRacerResult(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  int iVar3;
uint32_t piVar4;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x80a8)));
  iVar2 = iVar1 * 4;
  if (360000 < param_2) {
    param_2 = 360000;
  }
  iVar3 = iVar2 + param_1;
  piVar4 = (uint32_t)(param_1 + 0xc + iVar2 + 0x80f0);
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + param_1 + 0xc + 0x80a0))) = param_2;
  if (param_2 != 0) {
    if (99 < param_3) {
      param_3 = 99;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80c0))) = param_3;
    param_2 = param_2 + param_3 * 300;
    iVar2 = 360000;
    if (param_2 < 0x57e41) {
      iVar2 = param_2;
    }
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x80d4))) = iVar2;
    if (iVar2 < (*(int32_t*)rtguest::G((uint32_t)piVar4))) {
      (*(int32_t*)rtguest::G((uint32_t)piVar4)) = iVar2;
      (*(uint32_t*)rtguest::G((uint32_t)0x1780a14 + (uint32_t)(iVar1) * 4)) = iVar2;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 33000))) = 1;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0025aa58, guest::storeRacerResult)

}  // namespace guest
