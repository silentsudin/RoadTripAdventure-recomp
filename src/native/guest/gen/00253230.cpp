// guest:0x00253230 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetAiSteeringTarget(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  float fVar5;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 600))) + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x24a))) * 0xc;
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x78)));
  uVar3 = 0;
  if (iVar4 == 1) {
    if ((0x6000 < param_2) || (param_2 < -0x6000)) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x78))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = 2;
      goto LAB_0025339c;
    }
    if (param_2 < 0x81) {
      if (-0x81 < param_2) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = 1;
        goto LAB_0025339c;
      }
      uVar1 = 0x8001;
    }
    else {
      uVar1 = 0x2001;
    }
  }
  else {
    if (iVar4 < 2) {
      if (iVar4 == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = 2;
        fVar5 = (float)GetSpeedFromF15Vector(param_1 + 0xf0);
        if (fVar5 == 0.0) {
          uVar3 = 1;
          if ((param_2 < 0x4001) && (-0x4001 < param_2)) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x78))) = 1;
          }
          else {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x78))) = 2;
          }
        }
      }
      goto LAB_0025339c;
    }
    if (iVar4 != 2) goto LAB_0025339c;
    if (param_2 < 0x2001) {
      if (-0x2001 < param_2) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = 2;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x78))) = 0;
        goto LAB_0025339c;
      }
      uVar1 = 0x2005;
    }
    else {
      uVar1 = 0x8005;
    }
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = uVar1;
LAB_0025339c:
  param_1 = param_1 + 0xf0;
  fVar5 = (float)GetSpeedFromF15Vector(param_1);
  if ((int)fVar5 < 0) {
    fVar5 = (float)GetSpeedFromF15Vector(param_1);
    iVar4 = -(int)fVar5;
  }
  else {
    fVar5 = (float)GetSpeedFromF15Vector(param_1);
    iVar4 = (int)fVar5;
  }
  if ((param_3 < iVar4) && (0 < param_3)) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x74))) & 0xa000;
  }
  return uVar3;
}


RT_GUEST_FN(0x00253230, guest::SetAiSteeringTarget)

}  // namespace guest
