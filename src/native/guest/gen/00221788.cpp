// guest:0x00221788 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawCar(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t param_5, ...) {
uint32_t auStack_220 = rtguest::allocFrame(52);
    uint32_t fStack_1ec = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_1e0 = rtguest::allocFrame(48);
    uint32_t uStack_1b0 = rtguest::allocFrame(4);
    uint32_t uStack_1ac = rtguest::allocFrame(4);
    uint32_t uStack_1a8 = rtguest::allocFrame(4);
    uint32_t uStack_1a4 = rtguest::allocFrame(4);
    uint32_t uStack_1a0 = rtguest::allocFrame(4);
    uint32_t uStack_19c = rtguest::allocFrame(4);
    uint32_t uStack_198 = rtguest::allocFrame(4);
    uint32_t uStack_194 = rtguest::allocFrame(4);
    uint32_t uStack_190 = rtguest::allocFrame(4);
    uint32_t uStack_18c = rtguest::allocFrame(4);
    uint32_t uStack_188 = rtguest::allocFrame(4);
    uint32_t uStack_184 = rtguest::allocFrame(4);
    uint32_t uStack_180 = rtguest::allocFrame(4);
    uint32_t uStack_17c = rtguest::allocFrame(4);
    uint32_t uStack_178 = rtguest::allocFrame(4);
    uint32_t uStack_174 = rtguest::allocFrame(4);
uint32_t auStack_170 = rtguest::allocFrame(48);
    uint32_t fStack_140 = rtguest::allocFrame(4);
    uint32_t uStack_13c = rtguest::allocFrame(4);
    uint32_t uStack_138 = rtguest::allocFrame(4);
    uint32_t uStack_134 = rtguest::allocFrame(4);
uint32_t auStack_130 = rtguest::allocFrame(64);
uint32_t auStack_f0 = rtguest::allocFrame(64);
uint32_t auStack_b0 = rtguest::allocFrame(48);
uint32_t auStack_80 = rtguest::allocFrame(16);
  ushort uVar1;
uint32_t puVar2;
  int iVar3;
  ulong uVar4;
  uint uVar5;
uint32_t piVar6;
  int iVar7;
uint32_t puVar8;
  uint uVar9;
  long lVar10;
  int iVar11;
  int iVar12;
  float fVar13;
  float fVar14;
  
  iVar12 = 0;
  puVar2 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)(param_4) * 4));
  uVar5 = (*(uint32_t*)rtguest::G((uint32_t)puVar2));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_18c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 >> 0xe & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_188)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 >> 0x12 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_1a0)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 & 0xf) * 4)));
  iVar3 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(1) * sizeof(uint32_t))) * 0x44;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_19c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 >> 2 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_198)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 >> 6 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_190)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x29d150 + (uVar5 >> 10 & 0x3c))));
  (*(uint32_t*)rtguest::G((uint32_t)uStack_194)) = 0;
  (*(uint32_t*)rtguest::G((uint32_t)uStack_184)) = 0;
  vifUploadColours(uStack_1a0,uStack_190);
  iVar11 = (int)param_3;
  vu0MulMatrix(auStack_220,param_3,iVar11 + 0x40);
  vu0CopyMatrix(auStack_1e0,auStack_220);
  ScaleCarLightMatrix((*(float*)rtguest::G((uint32_t)fStack_1ec)) - param_2,0x177f620 + param_5 * 0x40);
  vifObjectMatrix(auStack_220);
  vifBatchRegs((long)(param_4 * 0x44 + 0x243e) | 0x1d308000U |
               (long)(param_4 * 0x44 + 0x247e) << 0x25 | 0x20000005c0000000,7,0,0xf);
  if (((*(float*)rtguest::G(0x332cc0)) <= param_1) || (5 < (*(uint32_t*)rtguest::G(0x3356e8)))) {
    iVar12 = 1;
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f314 + param_4 * 0x18))) + 0x10);
  }
  else {
    vifCallModel((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)0x177f310 + (uint32_t)(param_4 * 6) * 4)) + 0x10);
    (*(uint32_t*)rtguest::G(0x3356e8)) = (*(uint32_t*)rtguest::G(0x3356e8)) + 1;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t))) & 0x1000) == 0) {
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f318 + param_4 * 0x18))) + 0x10);
  }
  else {
    vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177f31c + param_4 * 0x18))) + 0x10);
  }
  uVar9 = 0;
  uVar5 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t)));
  piVar6 = (uint32_t)(0x0177f320 + param_4 * 0x18);
  do {
    if ((uVar5 & 0x2000 << (uVar9 & 0x1f)) != 0) {
      vifBatchRegs(0x20137c45dd309bc2,7,(long)(int)(uVar9 + 10) << 0x38 | 0x137c45dd409bc2,7);
      vifCallModel((*(int32_t*)rtguest::G((uint32_t)piVar6)) + 0x10);
      uVar5 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t)));
    }
    uVar9 = uVar9 + 1;
    piVar6 = piVar6 + 0x4;  // T* element step
  } while ((int)uVar9 < 2);
  lVar10 = 0;
  do {
    uVar9 = (uint32_t)lVar10;
    if (((int)uVar5 >> (uVar9 & 0x1f) & 1U) == 0) {
LAB_00221c74:
      lVar10 = (long)(int)(uVar9 + 1);
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)uStack_180)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)uStack_174)) = 0x3f800000;
      switch(lVar10) {
      case 0:
      case 1:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed728 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed72c + iVar3)));
        break;
      case 2:
      case 3:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed730 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed734 + iVar3)));
        break;
      case 4:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed738 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed73c + iVar3)));
        break;
      case 5:
      case 9:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed740 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed744 + iVar3)));
        break;
      case 6:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed748 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed74c + iVar3)));
        break;
      case 8:
        (*(uint32_t*)rtguest::G((uint32_t)uStack_17c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed750 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_178)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed754 + iVar3)));
      }
      vu0ApplyMatrix(uStack_1b0,auStack_220,uStack_180);
      vifObjectMatrix(auStack_1e0);
      vifBatchRegs(0x20137c45dd309bc2,7,lVar10 << 0x38 | 0x137c45dd409bc2,7);
      if (lVar10 != 6) {
        vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eac8 + uVar9 * 4))) + 0x10);
        goto LAB_00221c74;
      }
      if (iVar12 == 0) {
        iVar7 = 0;
        fVar13 = (*(uint32_t*)rtguest::G(0x332cc4));
        vifCallModel((*(uint32_t*)rtguest::G(0x177eae0)) + 0x10);
        fVar14 = (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x88)));
        vu0UnitMatrix(auStack_170);
        vu0RotMatrixZ(-fVar14,auStack_170,auStack_170);
        (*(uint32_t*)rtguest::G((uint32_t)uStack_13c)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed748 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_138)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed74c + iVar3)));
        (*(float*)rtguest::G((uint32_t)fStack_140)) = (*(uint32_t*)rtguest::G(0x332cc8));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_134)) = 0x3f800000;
        do {
          iVar7 = iVar7 + 1;
          vu0MulMatrix(auStack_130,auStack_220,auStack_170);
          vifObjectMatrix(auStack_130);
          vifCallModel((*(uint32_t*)rtguest::G(0x177eae4)) + 0x10);
          (*(float*)rtguest::G((uint32_t)fStack_140)) = (*(float*)rtguest::G((uint32_t)fStack_140)) + fVar13;
        } while (iVar7 < 4);
        lVar10 = 7;
      }
      else {
        vifCallModel((*(uint32_t*)rtguest::G(0x177eaf4)) + 0x10);
        lVar10 = 7;
      }
    }
    if (8 < lVar10) {
      uVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t)));
      if ((uVar1 & 0x200) != 0) {
        (*(uint32_t*)rtguest::G((uint32_t)uStack_1a8)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed744 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_1ac)) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002ed740 + iVar3)));
        (*(uint32_t*)rtguest::G((uint32_t)uStack_1a4)) = 0x3f800000;
        (*(uint32_t*)rtguest::G((uint32_t)uStack_1b0)) = 0;
        vu0ApplyMatrix(uStack_1b0,auStack_220,uStack_1b0);
        vifObjectMatrix(auStack_1e0);
        iVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 7))) * 10;
        vifBatchRegs((long)(iVar3 + 0x1be4) | 0x19308000U | (long)(iVar3 + 0x1bec) << 0x25 |
                     0x2010000540000000,7,0,0xf);
        vifCallModel((*(uint32_t*)rtguest::G(0x177eaec)) + 0x10);
        uVar1 = (uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t)));
      }
      if ((uVar1 & 0x400) == 0) {
        if (param_1 == 0.0) {
          drawCarWheelsNear(param_3,iVar11 + 0x80,puVar2);
        }
        else if (param_1 < (*(uint32_t*)rtguest::G(0x332ccc))) {
          drawCarWheelsMid(param_3,iVar11 + 0x80);
        }
        else {
          vifObjectMatrix(param_3);
          uVar4 = (uint32_t)(int)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 6))) * 2 + 0x1c44);
          vifBatchRegs(uVar4 | 0x20138c85dd308000,7,
                       uVar4 | 0x1d408000 | (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 5))) << 0x38 |
                       0x138c85c0000000,7);
          vifCallModel((*(uint32_t*)rtguest::G(0x177eb04)) + 0x10);
        }
      }
      else {
        vifObjectMatrix();
        uVar5 = 0;
        vifCallModel((*(uint32_t*)rtguest::G(0x177eaf0)) + 0x10);
        vifFixedRegs(0x20138c85dd309c44,7,
                     (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar2 + 5))) << 0x38 | 0x138c85dd409c44,7);
        vu0UnitMatrix(auStack_b0);
        vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x80))),auStack_b0,auStack_b0);
        vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x84))),auStack_b0,auStack_b0);
        puVar8 = 0x0029cb20;
        do {
          vu0CopyVector(auStack_80,puVar8);
          vu0MulMatrix(auStack_f0,param_3,auStack_b0);
          vifObjectMatrix(auStack_f0);
          vifCallModel((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177eb08 + (uVar5 & 1) * 4))) + 0x10);
          if (uVar5 == 1) {
            vu0UnitMatrix(auStack_b0);
            vu0RotMatrixX((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x88))),auStack_b0,auStack_b0);
          }
          uVar5 = uVar5 + 1;
          puVar8 = puVar8 + 0x10;
        } while ((int)uVar5 < 4);
      }
      return iVar12;
    }
    uVar5 = (uint32_t)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)puVar2 + (uint32_t)(2) * sizeof(uint32_t)));
  } while( true );
}


RT_GUEST_FN(0x00221788, guest::drawCar)

}  // namespace guest
