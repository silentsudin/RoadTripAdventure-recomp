// guest:0x0025d7c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t collectionNpcUpdate(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t uStack_5c = rtguest::allocFrame(4);
  short sVar1;
uint32_t pfVar2;
  int iVar3;
  ushort uVar4;
  char cVar5;
  ushort uVar6;
uint32_t pfVar7;
uint32_t puVar8;
  float fVar9;
  float fVar10;
uint32_t auStack_60 = rtguest::allocFrame(4);
uint32_t auStack_50 = rtguest::allocFrame(16);
uint32_t auStack_40 = rtguest::allocFrame(4);
uint32_t auStack_3c = rtguest::allocFrame(12);
  
  pfVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c)));
  uVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) - (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a)));
  uVar4 = -uVar6;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 8))) == '\0') {
    puVar8 = (uint32_t)param_2;
    if (((*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(5) * sizeof(uint64_t))) & 0x8000) == 0) {
      rtCall((*(uint32_t*)rtguest::G(0x335710)), pfVar2, auStack_40, auStack_3c, 0, param_2);
      (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(1) * sizeof(float))) = (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(1) * sizeof(float))) + 1.0;
      if (((uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) - (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a)))) + 0x3ff < 0x7ff) {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a)));
      }
      if (uVar6 <= uVar4) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) + -0x400;
      }
      if (uVar6 < uVar4) {
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9)));
      }
      else {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9))) + 0x400;
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 9)));
      }
      if (sVar1 == (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a)))) {
        if ((*(float*)rtguest::G((uint32_t)pfVar2)) < 720.0) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a))) = ((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar8)) & 0x7f) * 0x100 + 0x4000;
          fVar9 = (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float)));
        }
        else {
          fVar9 = (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float)));
        }
        if (fVar9 < 720.0) {
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a))) = (short)(((*(uint64_t*)rtguest::G((uint32_t)puVar8)) & 0x7f) << 8);
        }
        if (880.0 <= (*(float*)rtguest::G((uint32_t)pfVar2))) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a))) = ((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar8)) & 0x7f) * 0x100 + -0x4000;
        }
        if (880.0 <= fVar9) {
          (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a))) = ((uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar8)) & 0x7f) * 0x100 + -0x8000;
        }
      }
      else {
        if (uVar6 == uVar4) {
          (*(int16_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x2a))) = (short)(((*(uint64_t*)rtguest::G((uint32_t)puVar8)) & 0xff) << 8);
          fVar9 = (*(float*)rtguest::G((uint32_t)pfVar2));
        }
        else {
          fVar9 = (*(float*)rtguest::G((uint32_t)pfVar2));
        }
        if (fVar9 < 720.0) {
          (*(float*)rtguest::G((uint32_t)pfVar2)) = 720.0;
          fVar10 = (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float)));
          fVar9 = 720.0;
        }
        else {
          fVar10 = (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float)));
        }
        if (fVar10 < 720.0) {
          (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float))) = 720.0;
          fVar10 = 720.0;
        }
        if (880.0 <= fVar9) {
          (*(float*)rtguest::G((uint32_t)pfVar2)) = 880.0;
        }
        if (880.0 <= fVar10) {
          (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(2) * sizeof(float))) = 880.0;
        }
      }
      vu0SubVector(auStack_50,pfVar2,0x177ace0);
      fVar9 = (float)vecDotProductVu0(auStack_50,auStack_50);
      if (((SQRT(fVar9) < 2.5) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18))) + 0x24))) == '\0')) &&
         (((*(uint64_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(5) * sizeof(uint64_t))) & 4) != 0)) {
        PlayUiSound(0x302);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 8))) = 1;
        iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
        (*(uint32_t*)rtguest::G((uint32_t)0x177eb4c + (uint32_t)((uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) * 0x10) * 4)) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 1;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
      }
      TransformRectangle(auStack_60,(int)pfVar2 + 0x22);
      pfVar7 = pfVar2 + 4;
      fVar9 = (float)sinf((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)));
      (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(6) * sizeof(float))) = fVar9;
      fVar9 = (float)cosf((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)));
      (*(float*)rtguest::G((uint32_t)pfVar2 + (uint32_t)(4) * sizeof(float))) = -fVar9;
      vecNormalizeVu0(pfVar7,pfVar7);
      vu0ScaleVector((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4bd0 + ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) & 3) * 4))),pfVar7
                     ,pfVar7);
      vu0AddVector(pfVar2,pfVar2,pfVar7);
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 8)));
    }
    else {
      TransformRectangle(auStack_60,(int)pfVar2 + 0x22);
      cVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(pfVar2 + 8)));
    }
    if (cVar5 == '\0') {
      vu0UnitMatrix(0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) * 0x40);
      vu0RotMatrixY((*(uint32_t*)rtguest::G((uint32_t)uStack_5c)),0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) * 0x40,
                    0x0177eb10 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) * 0x40);
      vu0CopyVectorXYZ(0x0177eb40 + (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)pfVar2 + 0x21))) * 0x10,pfVar2);
    }
  }
  return 0;
}


RT_GUEST_FN(0x0025d7c8, guest::collectionNpcUpdate)

}  // namespace guest
