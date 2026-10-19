// guest:0x00284400 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: undefined8 _d2b(undefined8, long, int*, int*)
uint64_t _d2b(uint64_t param_1, uint64_t param_2, uint32_t param_3, uint32_t param_4, ...) {
    uint32_t uStack_50 = rtguest::allocFrame(4);
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  ulong uVar5;
  int iVar6;
uint32_t puVar7;
  ulong uVar8;
uint32_t auStack_4c = rtguest::allocFrame(12);
  
  uVar4 = _Balloc(param_1,1);
  uVar5 = param_2 >> 0x20 & 0x7fffffff;
  (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) = (uint32_t)(param_2 >> 0x20) & 0xfffff;
  uVar8 = uVar5 >> 0x14;
  iVar6 = (int)uVar4;
  puVar7 = (uint32_t)(iVar6 + 0x14);
  if (uVar8 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) | 0x100000;
  }
  if ((uint32_t)param_2 == 0) {
    iVar2 = 1;
    iVar3 = _lo0bits(auStack_4c);
    uVar1 = iVar3 + 0x20;
    (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10))) = 1;
  }
  else {
    (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) = (uint32_t)param_2;
    uVar1 = _lo0bits(uStack_50);
    if (uVar1 == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_50));
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_50)) | (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) << (-uVar1 & 0x1f);
      (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) = (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) >> (uVar1 & 0x1f);
    }
    iVar2 = 2;
    if ((*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4)) == 0) {
      iVar2 = 1;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18))) = (*(uint32_t*)rtguest::G((uint32_t)auStack_4c + (uint32_t)(0) * 4));
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x10))) = iVar2;
  }
  if (uVar8 == 0) {
    (*(int32_t*)rtguest::G((uint32_t)param_3)) = uVar1 - 0x432;
    iVar6 = _hi0bits((*(uint32_t*)rtguest::G((uint32_t)puVar7 + (uint32_t)(iVar2 + -1) * sizeof(uint32_t))));
    iVar6 = iVar2 * 0x20 - iVar6;
  }
  else {
    iVar6 = 0x35 - uVar1;
    (*(int32_t*)rtguest::G((uint32_t)param_3)) = ((uint32_t)uVar5 >> 0x14) + uVar1 + -0x433;
  }
  (*(int32_t*)rtguest::G((uint32_t)param_4)) = iVar6;
  return uVar4;
}


RT_GUEST_FN(0x00284400, guest::_d2b)

}  // namespace guest
