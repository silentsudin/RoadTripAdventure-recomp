// guest:0x0021f698 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t buildCamera(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
uint32_t auStack_1d0 = rtguest::allocFrame(16);
uint32_t auStack_1c0 = rtguest::allocFrame(16);
uint32_t auStack_1b0 = rtguest::allocFrame(16);
uint32_t auStack_1a0 = rtguest::allocFrame(16);
uint32_t auStack_190 = rtguest::allocFrame(16);
uint32_t afStack_180 = rtguest::allocFrame(20);
    uint32_t fStack_16c = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
    uint32_t uStack_158 = rtguest::allocFrame(4);
    uint32_t uStack_154 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_148 = rtguest::allocFrame(4);
    uint32_t uStack_144 = rtguest::allocFrame(4);
uint32_t afStack_140 = rtguest::allocFrame(20);
    uint32_t fStack_12c = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_120 = rtguest::allocFrame(4);
    uint32_t uStack_11c = rtguest::allocFrame(4);
    uint32_t uStack_118 = rtguest::allocFrame(4);
    uint32_t uStack_114 = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
    uint32_t uStack_108 = rtguest::allocFrame(4);
    uint32_t uStack_104 = rtguest::allocFrame(4);
uint32_t auStack_100 = rtguest::allocFrame(64);
uint32_t auStack_c0 = rtguest::allocFrame(64);
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  float fVar6;
  
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x1c))) & 0x1000) == 0) {
    ComposeCameraMatrix(auStack_1d0);
  }
  else {
    vu0CopyVector(auStack_1d0);
    vu0CopyVector(auStack_1c0,param_3);
    iVar4 = (int)param_3;
    vu0CopyVector(auStack_1b0,iVar4 + 0x10);
    vu0CopyVector(auStack_1a0,iVar4 + 0x20);
    vu0CopyVector(auStack_190,iVar4 + 0x30);
  }
  iVar4 = (int)param_1;
  fVar6 = (*(float*)rtguest::G(0x332c84));
  BuildCameraViewMatrices
            ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10))),auStack_100,auStack_c0,auStack_1d0,auStack_1b0,
             auStack_1c0,iVar4 + 0x100);
  uVar3 = (*(uint32_t*)rtguest::G(0x3356e4));
  (*(float*)rtguest::G((uint32_t)fStack_12c)) = (*(float*)rtguest::G(0x332c88));
  (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  if ((*(uint32_t*)rtguest::G(0x333288)) == 0) {
    fVar6 = 1.0;
  }
  uVar5 = (*(uint32_t*)rtguest::G(0x3356e0));
  vu0UnitMatrix(afStack_140);
  (*(float*)rtguest::G((uint32_t)fStack_12c)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) * (*(float*)rtguest::G((uint32_t)fStack_12c));
  (*(float*)rtguest::G((uint32_t)afStack_140 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) * fVar6;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_118)) = (*(uint32_t*)rtguest::G(0x332c8c));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_108)) = (*(uint32_t*)rtguest::G(0x332c90));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_11c)) = uVar3;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_114)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_104)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_120)) = uVar5;
  vu0UnitMatrix(afStack_180);
  (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4)) * 0.001953125;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_158)) = (*(uint32_t*)rtguest::G(0x332c94));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_148)) = (*(uint32_t*)rtguest::G(0x332c98));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_154)) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_144)) = 0;
  (*(float*)rtguest::G((uint32_t)fStack_16c)) = (*(float*)rtguest::G((uint32_t)afStack_180 + (uint32_t)(0) * 4));
  vu0UnitMatrix((uint32_t)(iVar4 + 0x80));
  uVar2 = (*(uint32_t*)rtguest::G(0x332ca4));
  uVar1 = (*(uint32_t*)rtguest::G(0x332ca0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x94))) = (*(uint32_t*)rtguest::G(0x332c9c));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xa8))) = uVar1;
  (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x80))) = fVar6 * 512.0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb8))) = uVar2;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xbc))) = 0x3f800000;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb0))) = uVar5;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0xb4))) = uVar3;
  vu0MulMatrix(param_1,afStack_140,auStack_100);
  vu0MulMatrix(iVar4 + 0x40,afStack_180,auStack_100);
  BuildCameraViewMatrices(0,auStack_100,auStack_c0,auStack_1d0,auStack_190,auStack_1a0,0);
  vu0MulMatrix(iVar4 + 0xc0,afStack_180,auStack_100);
  vu0CopyVector(iVar4 + 0x180,auStack_1b0);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x10)));
  return 0;
}


RT_GUEST_FN(0x0021f698, guest::buildCamera)

}  // namespace guest
