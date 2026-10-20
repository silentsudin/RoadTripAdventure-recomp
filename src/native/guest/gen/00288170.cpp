// guest:0x00288170 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifRegisterRpc(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  
  DIntr();
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xf) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0xe) * sizeof(uint32_t))) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)param_1)) = param_2;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_7 + 8)));
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(uint32_t))) = param_3;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(uint32_t))) = param_4;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(4) * sizeof(uint32_t))) = param_5;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(5) * sizeof(uint32_t))) = param_6;
  (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(0x10) * sizeof(uint32_t))) = param_7;
  if (iVar2 == 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_7 + 8))) = param_1;
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) = param_1;
    }
    else {
      do {
        iVar2 = iVar1;
        iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38)));
      } while (iVar1 != 0);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x38))) = param_1;
    }
  }
  EIntr();
  return 0;
}


RT_GUEST_FN(0x00288170, guest::sceSifRegisterRpc)

}  // namespace guest
