// guest:0x00287e18 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sifRpcCallRequest(uint32_t param_1, ...) {
  int iVar1;
uint32_t piVar2;
  undefined4 uVar3;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x34)));
  piVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x40)));
  if ((*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(3) * sizeof(int32_t))) == 0) {
    (*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(3) * sizeof(int32_t))) = iVar1;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t))) + 0x3c))) = iVar1;
  }
  (*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(4) * sizeof(int32_t))) = iVar1;
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x20))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x14)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1c))) = uVar3;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x20)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x2c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x2c)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x30))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x30)));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x34))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10)));
  if ((-1 < (*(int32_t*)rtguest::G((uint32_t)piVar2))) && ((*(int32_t*)rtguest::G((uint32_t)piVar2 + (uint32_t)(1) * sizeof(int32_t))) == 0)) {
    iWakeupThread((*(int32_t*)rtguest::G((uint32_t)piVar2)));  // 287e7c lw $4,0x0($6) feeds the jal
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00287e18, guest::sifRpcCallRequest)

}  // namespace guest
