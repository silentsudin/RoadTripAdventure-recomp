// guest:0x0021f450 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t BuildCameraViewMatrices(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, uint32_t param_6, uint32_t param_7, ...) {
uint32_t auStack_d0 = rtguest::allocFrame(16);
uint32_t auStack_c0 = rtguest::allocFrame(16);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(16);
    uint32_t fStack_90 = rtguest::allocFrame(4);
    uint32_t fStack_8c = rtguest::allocFrame(4);
    uint32_t fStack_88 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_80 = rtguest::allocFrame(4);
    uint32_t fStack_7c = rtguest::allocFrame(4);
    uint32_t fStack_78 = rtguest::allocFrame(4);
    uint32_t uStack_74 = rtguest::allocFrame(4);
uint32_t pfVar1;
  float fVar2;
  int iVar3;
uint32_t pfVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  float fVar8;
  
  vu0UnitMatrix(auStack_c0);
  vecCrossProductVu0(auStack_d0,param_6,param_5);
  vecNormalizeVu0(auStack_c0,auStack_d0);
  vecNormalizeVu0(auStack_a0,param_5);
  vecCrossProductVu0(auStack_b0,auStack_a0,auStack_c0);
  float fParam1;
  __builtin_memcpy(&fParam1, &param_1, sizeof(fParam1));
  fVar2 = (*(float*)rtguest::G(0x332c80));
  if (param_7 != 0) {
    iVar6 = 0;
    (*(float*)rtguest::G((uint32_t)fStack_8c)) = (*(float*)rtguest::G((uint32_t)fStack_8c)) + (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(1) * sizeof(float)));
    uVar7 = 0x3f800000;
    iVar5 = 6;
    fVar8 = (*(float*)rtguest::G(0x332c7c));
    mat4InvertAffine(param_3,auStack_c0);
    pfVar4 = (uint32_t)0x0029cf40;
    (*(float*)rtguest::G((uint32_t)fStack_90)) = (*(float*)rtguest::G((uint32_t)fStack_90)) + (*(float*)rtguest::G((uint32_t)param_4));
    (*(float*)rtguest::G((uint32_t)fStack_88)) = (*(float*)rtguest::G((uint32_t)fStack_88)) + (*(float*)rtguest::G((uint32_t)param_4 + (uint32_t)(2) * sizeof(float)));
    do {
      if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
        (*(float*)rtguest::G((uint32_t)fStack_80)) = ((*(float*)rtguest::G((uint32_t)pfVar4)) * (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(2) * sizeof(float)))) / fParam1;
      }
      else {
        (*(float*)rtguest::G((uint32_t)fStack_80)) = ((*(float*)rtguest::G((uint32_t)pfVar4)) * (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(2) * sizeof(float)))) / (fParam1 * fVar8);
      }
      (*(float*)rtguest::G((uint32_t)fStack_78)) = (*(float*)rtguest::G((uint32_t)pfVar4 + (uint32_t)(2) * sizeof(float)));
      pfVar1 = pfVar4 + 1;
      pfVar4 = pfVar4 + 4;
      iVar3 = (int)param_7 + iVar6;
      iVar6 = iVar6 + 0x10;
      iVar5 = iVar5 + -1;
      (*(float*)rtguest::G((uint32_t)fStack_7c)) = ((*(float*)rtguest::G((uint32_t)pfVar1)) * (*(float*)rtguest::G((uint32_t)fStack_78))) / (param_1 * fVar2);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_74)) = uVar7;
      vu0ApplyMatrix(iVar3,auStack_c0,fStack_80);
    } while (-1 < iVar5);
    vu0CopyVector((int)param_7 + 0x70,fStack_90);
  }
  mat4InvertAffine(param_2,auStack_c0);
  iVar5 = (int)param_2;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4)));
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14)));
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24)));
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x34))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x34)));
  if (param_7 != 0) {
    iVar5 = (int)param_3;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x34))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x34)));
  }
  return 0;
}


RT_GUEST_FN(0x0021f450, guest::BuildCameraViewMatrices)

}  // namespace guest
