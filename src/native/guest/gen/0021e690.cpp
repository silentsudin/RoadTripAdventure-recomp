// guest:0x0021e690 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCarCameraTask(uint32_t param_1, uint32_t param_2, ...) {
    uint32_t fStack_58 = rtguest::allocFrame(4);
  short sVar1;
  uint uVar2;
uint32_t pcVar3;
  int iVar4;
  int iVar5;
  long lVar6;
  undefined8 uVar7;
  int iVar8;
  uint uVar9;
  long lVar10;
  long lVar11;
  long lVar12;
  uint uVar13;
uint32_t puVar14;
  int iVar15;
  float fVar16;
  float fVar17;
  float fVar18;
uint32_t auStack_e0 = rtguest::allocFrame(48);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(64);
uint32_t afStack_60 = rtguest::allocFrame(8);
  
  iVar5 = (int)param_1;
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
  uVar13 = uVar2 & 1;
  iVar8 = uVar13 * 0x20;
  puVar14 = 0x0177e9f0 + iVar8;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    iVar8 = uVar13 * 0x10;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff30 + iVar8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar8))) = 0;
    SetViewDistanceGlobals(0x45000000,0x45000000);
    return 0;
  }
  iVar15 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 1) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 10))) == '\b') {
      InitializeTask(param_1,0x238b18,4);
      pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc)));
      goto LAB_0021e8cc;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x22))) == '\x1a') {
      LoadCameraPreset(puVar14,((int)(uVar2 & 2) >> 1) + 8);
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
    }
    else {
      uVar9 = uVar2 & 2;
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177add4 + (uint32_t)(uVar13 * 0x9c) * 4)) + 8))) & 0x400) != 0) {
        uVar9 = uVar2 & 2 | 4;
      }
      LoadCameraPreset(puVar14,uVar9);
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
    }
  }
  else {
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  }
  if ((uVar2 & 0x2000) != 0) {
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) == 2) {
    SetViewDistanceGlobals(0x45000000,0x44f90000);
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  }
  else if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) == 3) {
    SetViewDistanceGlobals(0x45000000,(*(uint32_t*)rtguest::G(0x332c4c)));
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  }
  else {
    SetViewDistanceGlobals(0x45000000,0x45000000);
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
  }
  if ((int)uVar2 < 0) {
    InitializeTask(param_1,0x21ed90,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))));
    pcVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc)));
LAB_0021e8cc:
    rtCall(pcVar3, param_1,param_2);
    return 0;
  }
  if ((((*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(uVar13 * 0x138) * 2)) & 0x200) != 0) && (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 10))) & 2) == 0)) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
    pcVar3 = (*(uint8_t*)rtguest::G(0x21e4d8));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = (*(uint8_t*)rtguest::G(0x21e4d8));
    goto LAB_0021e8cc;
  }
  if ((uVar2 & 0x400) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = (*(uint8_t*)rtguest::G(0x21ec58));
    RunCameraZoomTransition(param_1,param_2);
    return 0;
  }
  if (((uVar2 & 0x48008) == 0) &&
     (uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))),
     ((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((uVar9 & 1) * 2 + iVar15 + 0x30))) &
     (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x01780a56 + (uVar9 & 1) * 0x3448)))) != 0)) {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x22))) == '\x1a') {
      if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8))) == 0) {
        iVar4 = 1;
      }
      else {
        iVar4 = ((int)(uVar9 & 2) >> 1) + 8;
      }
      LoadCameraPreset(puVar14,iVar4);
    }
    else {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8))) ^ 1 | uVar9 & 2;
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177add4 + (uint32_t)(uVar13 * 0x9c) * 4)) + 8))) & 0x400) != 0) {
        uVar2 = uVar2 | 4;
      }
      LoadCameraPreset(puVar14,uVar2);
    }
    vu0CopyVector(afStack_60,0x0177ac60 + uVar13 * 0x4e);
    fVar18 = (float)(int)(short)(*(uint32_t*)rtguest::G((uint32_t)0x177ae24 + (uint32_t)(uVar13 * 0x138) * 4)) * (*(uint32_t*)rtguest::G(0x332c50)) * 3.0517578e-05;
    fVar16 = (float)sinf(fVar18);
    fVar18 = (float)cosf(fVar18);
    fVar17 = (*(float*)rtguest::G((uint32_t)fStack_58)) * fVar16;
    iVar4 = ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 1) * 0x10;
    (*(float*)rtguest::G((uint32_t)fStack_58)) = (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) * fVar16 + (*(float*)rtguest::G((uint32_t)fStack_58)) * fVar18;
    (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) = (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4)) * fVar18 - fVar17;
    uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28)));
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff30 + iVar4))) = (*(float*)rtguest::G((uint32_t)afStack_60 + (uint32_t)(0) * 4));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff34 + iVar4))) = 0;
    (*(float*)rtguest::G((uint32_t)(uint32_t)(0x0172ff38 + iVar4))) = (*(float*)rtguest::G((uint32_t)fStack_58));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x0172ff3c + iVar4))) = 0;
  }
  if (((uVar2 & 0x8000) == 0) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xb))) == '\0')) {
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar8)));
    if (sVar1 < -0x7f) {
      sVar1 = sVar1 + 0x80;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar8))) = sVar1;
    }
    if (sVar1 < 0x80) {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8)));
    }
    else {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea06 + iVar8))) = sVar1 + -0x80;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8)));
    }
    if (iVar4 != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar8))) = 0;
      goto LAB_0021eb8c;
    }
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 10))) == '\0') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x22))) != '\x1a') {
        iVar4 = (*(uint32_t*)rtguest::G((uint32_t)0x177add4 + (uint32_t)(uVar13 * 0x9c) * 4));
        goto LAB_0021eb08;
      }
      iVar5 = GetCarSlotEntry(8);
    }
    else {
      iVar4 = (*(uint32_t*)rtguest::G((uint32_t)0x177add4 + (uint32_t)(uVar13 * 0x9c) * 4));
LAB_0021eb08:
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c))) & 2;
      if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8))) & 0x400) != 0) {
        uVar2 = uVar2 | 4;
      }
      iVar5 = GetCarSlotEntry(uVar2);
    }
    sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x14)));
    lVar11 = (long)sVar1;
    if (((*(uint32_t*)rtguest::G((uint32_t)0x177ae63 + (uint32_t)(uVar13 * 0x270) * 4)) != '\0') &&
       (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177add4 + (uint32_t)(uVar13 * 0x9c) * 4)) + 8))) & 0x100) != 0)) {
      lVar11 = (long)(sVar1 + 0x500);
    }
    lVar10 = (long)((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea04 + iVar8))) + -0x80);
    lVar6 = (long)((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea04 + iVar8))) + 0x80);
    lVar12 = lVar11;
    if (lVar11 < lVar10) {
      lVar12 = lVar10;
    }
    if (lVar11 <= lVar6) {
      lVar6 = lVar12;
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea04 + iVar8))) = (short)lVar6;
LAB_0021eb74:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8)));
  }
  else {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xb))) == '\0') goto LAB_0021eb74;
    ApplyFreeCameraInput(param_2,puVar14,1);
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8)));
  }
  if (iVar5 == 0) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar8))) = -(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ae26 + uVar13 * 0x270)));
  }
  else {
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0a + iVar8))) = 0;
  }
LAB_0021eb8c:
  ComputeChaseCameraBasis(auStack_e0,auStack_a0,puVar14,0x177ac50 + uVar13 * 0x4e);
  FixedPosToFloatVec4(auStack_b0,0x0177acf0 + uVar13 * 0x9c);
  buildCamera(0x177e6d0 + uVar13 * 400,puVar14,auStack_e0,auStack_a0);
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x28))) & 0x8000) != 0) {
    return 0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x0177ea0c + iVar8))) != 0) {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0xb))) != '\0') {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar15 + 0x22))) == -1) {
    uVar7 = GetGridCellIndex(0x0177acf0 + uVar13 * 0x9c);
  }
  else {
    uVar7 = 2;
  }
  ResolveCameraTerrainCollision
            (0x177e6d0 + uVar13 * 400,puVar14,param_2,auStack_e0,auStack_a0,uVar7);
  return 0;
}


RT_GUEST_FN(0x0021e690, guest::RunCarCameraTask)

}  // namespace guest
