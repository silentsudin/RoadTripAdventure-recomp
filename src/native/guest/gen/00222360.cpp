// guest:0x00222360 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCarExtraParts(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
uint32_t piVar1;
  int iVar2;
  int iVar3;
uint32_t auStack_70 = rtguest::allocFrame(64);
  
  vu0MulMatrix(auStack_70,param_1,(int)param_1 + 0x40);
  vifObjectMatrix(auStack_70);
  vifBatchRegs((long)(param_2 * 0x44 + 0x243e) | 0x1d308000U |
               (long)(param_2 * 0x44 + 0x247e) << 0x25 | 0x20000005c0000000,7,0,0xf);
  piVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_2 * 6 + param_4) * 4));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar1));
  if ((*(int32_t*)rtguest::G((uint32_t)piVar1 + (uint32_t)(2) * sizeof(int32_t))) != 0) {
    if (iVar2 < 0) {
      iVar2 = iVar2 + 0xf;
    }
    if ((param_3 & 1) != 0) {
      vifCallModel(piVar1 + (iVar2 >> 4) * 4);
    }
  }
  iVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_2 * 6) * 4));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) != 0) {
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0xf;
    }
    if ((param_3 & 2) != 0) {
      vifCallModel(iVar2 + (iVar3 >> 4) * 0x10);
    }
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + param_2 * 0x18)));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xc))) != 0) {
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0xf;
    }
    if ((param_3 & 2) != 0) {
      vifCallModel(iVar2 + (iVar3 >> 4) * 0x10);
    }
  }
  return 0;
}


RT_GUEST_FN(0x00222360, guest::drawCarExtraParts)

}  // namespace guest
