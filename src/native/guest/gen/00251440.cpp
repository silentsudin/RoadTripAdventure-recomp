// guest:0x00251440 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// Pseudocode passes the destination explicitly; guest_decls only models its three-argument form.
void sceVu0ScaleVectorXYZ(float, uint32_t, uint32_t, uint32_t);

uint32_t UpdateBallPhysics(uint32_t param_1, uint32_t param_2, ...) {
uint32_t auStack_120 = rtguest::allocFrame(4);
    uint32_t fStack_11c = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_114 = rtguest::allocFrame(4);
    uint32_t uStack_110 = rtguest::allocFrame(8);
    uint32_t uStack_108 = rtguest::allocFrame(8);
    uint32_t fStack_100 = rtguest::allocFrame(4);
    uint32_t fStack_fc = rtguest::allocFrame(4);
    uint32_t fStack_f8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_f0 = rtguest::allocFrame(64);
uint32_t afStack_b0 = rtguest::allocFrame(8);
    uint32_t fStack_a8 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
uint32_t auStack_a0 = rtguest::allocFrame(16);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
uint32_t auStack_80 = rtguest::allocFrame(4);
    uint32_t uStack_7c = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_74 = rtguest::allocFrame(4);
uint32_t auStack_70 = rtguest::allocFrame(4);
uint32_t apuStack_6c = rtguest::allocFrame(12);
uint32_t pfVar1;
uint32_t puVar2;
  ulong uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  undefined4 uVar10;
  
  iVar4 = param_1 + 0x60;
  iVar5 = param_1 + 0x40;
  vu0AddVector(iVar4,iVar4,param_1 + 0x70);
  vu0CopyVector(auStack_120,iVar4);
  (*(uint32_t*)rtguest::G((uint32_t)apuStack_6c + (uint32_t)(0) * 4)) = (uint32_t)0x0;
  uVar3 = rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_120, auStack_70, apuStack_6c, 2, param_2);
  if ((*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 100))) < (*(float*)rtguest::G((uint32_t)fStack_114))) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) = 1;
      PlayUiSound(0x31c);
    }
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) = 0;
    PlayUiSound(0x31c);
  }
  fVar9 = (*(float*)rtguest::G(0x332fe4));
  if (uVar3 == 0xffffffffffffffff) {
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = -(*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78)));
    goto LAB_00251920;
  }
  iVar4 = param_1 + 0x70;
  if ((*(float*)rtguest::G((uint32_t)fStack_11c)) + (*(float*)rtguest::G(0x332fe4)) < (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 100)))) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) + 1;
    fVar9 = (*(float*)rtguest::G(0x332fec));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) == 0) {
      fVar7 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) - (*(float*)rtguest::G(0x332fe8));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) * (*(float*)rtguest::G(0x2ac2f0));
      fVar9 = (*(float*)rtguest::G(0x2ac2f0));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) = fVar7;
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) * fVar9;
    }
    else {
      fVar8 = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) - (*(float*)rtguest::G(0x332ff0));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) * (*(float*)rtguest::G(0x2ac2f0)) * (*(float*)rtguest::G(0x332fec));
      fVar7 = (*(float*)rtguest::G(0x2ac2f0));
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) = fVar8;
      (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) * fVar7 * fVar9;
    }
    goto LAB_00251920;
  }
  fVar7 = (float)vecDotProductVu0(iVar4,iVar4);
  fVar7 = SQRT(fVar7);
  uVar10 = 0;
  if (fVar7 == 0.0) goto LAB_00251920;
  iVar6 = param_1 + 0x60;
  vu0SubVector(fStack_100,auStack_120,iVar6);
  (*(uint64_t*)rtguest::G((uint32_t)uStack_110)) = (*(uint64_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)apuStack_6c))));
  (*(uint64_t*)rtguest::G((uint32_t)uStack_108)) = (*(uint64_t*)rtguest::G((*(uint32_t*)rtguest::G((uint32_t)apuStack_6c)) + sizeof(uint64_t)));
  vecNormalizeVu0(uStack_110,uStack_110);
  fVar8 = (float)vecDotProductVu0(uStack_110,fStack_100);
  sceVu0ScaleVectorXYZ(fVar8 + fVar9,fStack_100,uStack_110);
  fVar9 = (float)vecDotProductVu0(fStack_100,fStack_100);
  if (fVar7 < SQRT(fVar9)) {
    sceVu0ScaleVectorXYZ(fVar7,fStack_100,uStack_110,fStack_100);
  }
  vu0AddVector(iVar6,iVar6,fStack_100);
  puVar2 = (*(uint32_t*)rtguest::G((uint32_t)apuStack_6c + (uint32_t)(0) * 4));
  sceVu0ScaleVectorXYZ(rtguest::fbits(0xbf800000),auStack_a0,iVar4);
  if (puVar2 == (uint32_t)0x0) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = CONCAT44(0x3f800000,uVar10);
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = CONCAT44(uVar10,uVar10);
  }
  else {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint64_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint64_t)));
  }
  vecNormalizeVu0(uStack_90,uStack_90);
  fVar9 = (float)vecDotProductVu0(uStack_90,auStack_a0);
  sceVu0ScaleVectorXYZ(fVar9 + fVar9,fStack_100,uStack_90);
  vu0SubVector(fStack_100,fStack_100,auStack_a0);
  fVar9 = (*(float*)rtguest::G(0x332ff4));
  iVar4 = (int)(uVar3 & 0xf);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x90))) == 0) {
    iVar4 = iVar4 * 8;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = (*(float*)rtguest::G((uint32_t)fStack_100)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2f8 + iVar4)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = (*(float*)rtguest::G((uint32_t)fStack_f8)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2f8 + iVar4)));
    (*(float*)rtguest::G((uint32_t)fStack_fc)) = (*(float*)rtguest::G((uint32_t)fStack_fc)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2fc + iVar4)));
  }
  else {
    iVar4 = iVar4 * 8;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = (*(float*)rtguest::G((uint32_t)fStack_100)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2f8 + iVar4))) * (*(float*)rtguest::G(0x332ff4));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = (*(float*)rtguest::G((uint32_t)fStack_f8)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2f8 + iVar4))) * fVar9;
    (*(float*)rtguest::G((uint32_t)fStack_fc)) = (*(float*)rtguest::G((uint32_t)fStack_fc)) * (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002ac2fc + iVar4))) * fVar9;
  }
  (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x74))) = (*(float*)rtguest::G((uint32_t)fStack_fc));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x8c))) == 0) {
LAB_0025186c:
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98)));
  }
  else {
    if ((uVar3 & 0xf) != 5) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x70))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78))) = 0;
      goto LAB_0025186c;
    }
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98)));
  }
  if (iVar4 < 0x10) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) = 0;
  }
  else {
    vu0SubVector(auStack_80,iVar6,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x10))) * 0x40 + 0x2ac0d0);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_7c)) = 0;
    (*(float*)rtguest::G((uint32_t)fStack_74)) = (float)vecDotProductVu0(auStack_80,auStack_80);
    (*(float*)rtguest::G((uint32_t)fStack_74)) = SQRT((*(float*)rtguest::G((uint32_t)fStack_74)));
    if ((*(float*)rtguest::G((uint32_t)fStack_74)) < (*(float*)rtguest::G(0x332ff8))) {
      PlayUiSound(0x31b);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) = 0;
    }
    else if ((*(float*)rtguest::G((uint32_t)fStack_74)) < (*(float*)rtguest::G(0x332ffc))) {
      PlayUiSound(0x31a);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x98))) = 0;
    }
  }
LAB_00251920:
  vu0ScaleVector(0,afStack_b0,afStack_b0);
  pfVar1 = (uint32_t)(param_1 + 0x70);
  for ((*(float*)rtguest::G((uint32_t)fStack_a8)) = -(*(float*)rtguest::G((uint32_t)pfVar1)) / (*(float*)rtguest::G(0x333000)); (*(float*)rtguest::G((uint32_t)fStack_a8)) < (*(float*)rtguest::G(0x333004));
      (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)fStack_a8)) + (*(float*)rtguest::G(0x333008))) {
  }
  if ((*(float*)rtguest::G(0x33300c)) < (*(float*)rtguest::G((uint32_t)fStack_a8))) {
    do {
      (*(float*)rtguest::G((uint32_t)fStack_a8)) = (*(float*)rtguest::G((uint32_t)fStack_a8)) - (*(float*)rtguest::G(0x333010));
    } while ((*(float*)rtguest::G(0x33300c)) < (*(float*)rtguest::G((uint32_t)fStack_a8)));
    (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78)));
  }
  else {
    (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x78)));
  }
  for ((*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) / (*(float*)rtguest::G(0x333014)); (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) < (*(float*)rtguest::G(0x333018));
      (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) + (*(float*)rtguest::G(0x33301c))) {
  }
  for (; (*(float*)rtguest::G(0x333020)) < (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)); (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_b0 + (uint32_t)(0) * 4)) - (*(float*)rtguest::G(0x333024))) {
  }
  iVar4 = param_1 + 0x50;
  vu0UnitMatrix(auStack_f0);
  vu0RotMatrix(auStack_f0,auStack_f0,afStack_b0);
  vu0ApplyMatrix(iVar4,auStack_f0,iVar4);
  vu0ApplyMatrix(iVar5,auStack_f0,iVar5);
  vecNormalizeVu0(iVar4,iVar4);
  vecNormalizeVu0(iVar5,iVar5);
  fVar9 = (float)vecDotProductVu0(pfVar1,pfVar1);
  iVar4 = 0x78;
  if (SQRT(fVar9) < (*(float*)rtguest::G(0x333028))) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94))) == 0) {
      vu0ScaleVector(0,pfVar1,pfVar1);
      return 1;
    }
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94))) + -1;
  }
  (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x94))) = iVar4;
  return 0;
}


RT_GUEST_FN(0x00251440, guest::UpdateBallPhysics)

}  // namespace guest
