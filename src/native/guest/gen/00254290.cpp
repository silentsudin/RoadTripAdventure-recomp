// guest:0x00254290 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RotateObjectByWrappedYaw(uint32_t param_1, ...) {
    uint32_t uStack_28 = rtguest::allocFrame(4);
  int iVar1;
uint32_t auStack_70 = rtguest::allocFrame(64);
uint32_t afStack_30 = rtguest::allocFrame(8);
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(afStack_30)), 0, 0x10));
  iVar1 = (int)param_1;
  for ((*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x30))); (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) < (*(float*)rtguest::G(0x333078));
      (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) + (*(uint32_t*)rtguest::G(0x33307c))) {
  }
  if ((*(uint32_t*)rtguest::G(0x333080)) < (*(uint32_t*)rtguest::G((uint32_t)uStack_28))) {
    do {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) = (*(uint32_t*)rtguest::G((uint32_t)uStack_28)) - (*(uint32_t*)rtguest::G(0x333084));
    } while ((*(uint32_t*)rtguest::G(0x333080)) < (*(uint32_t*)rtguest::G((uint32_t)uStack_28)));
    (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38)));
  }
  else {
    (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x38)));
  }
  for (; (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) < (*(uint32_t*)rtguest::G(0x333088)); (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) + (*(uint32_t*)rtguest::G(0x33308c))) {
  }
  for (; (*(uint32_t*)rtguest::G(0x333090)) < (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)); (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_30 + (uint32_t)(0) * 4)) - (*(uint32_t*)rtguest::G(0x333094))) {
  }
  iVar1 = iVar1 + 0x10;
  vu0UnitMatrix(auStack_70);
  vu0RotMatrix(auStack_70,auStack_70,afStack_30);
  vu0ApplyMatrix(iVar1,auStack_70,iVar1);
  vu0ApplyMatrix(param_1,auStack_70,param_1);
  vecNormalizeVu0(iVar1,iVar1);
  vecNormalizeVu0(param_1,param_1);
  return 0;
}


RT_GUEST_FN(0x00254290, guest::RotateObjectByWrappedYaw)

}  // namespace guest
