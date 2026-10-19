// guest:0x00251b60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateGolfMiniGame(uint32_t param_1, uint32_t param_2, ...) {
uint32_t aiStack_150 = rtguest::allocFrame(48);
uint32_t aiStack_120 = rtguest::allocFrame(48);
uint32_t auStack_f0 = rtguest::allocFrame(4);
    uint32_t uStack_ec = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_e4 = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
uint32_t auStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_c4 = rtguest::allocFrame(4);
    rtguest::allocFrame(16);  // guest frame gap
uint32_t auStack_b0 = rtguest::allocFrame(4);
    uint32_t uStack_ac = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t fStack_a4 = rtguest::allocFrame(4);
uint32_t auStack_a0 = rtguest::allocFrame(16);
    uint32_t uStack_90 = rtguest::allocFrame(8);
    uint32_t uStack_88 = rtguest::allocFrame(8);
uint32_t auStack_80 = rtguest::allocFrame(4);
uint32_t apuStack_7c = rtguest::allocFrame(12);
  int iVar1;
  float fVar2;
uint32_t pvVar3;
  undefined4 uVar4;
  long lVar5;
  uint uVar6;
  int iVar7;
uint32_t piVar8;
uint32_t puVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    SetGamePausedState(param_2,1);
    uiFade(0xffffffff80000000);
    pvVar3 = (uint32_t)getExtraPacketArea(0x10e30);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = pvVar3;
    pvVar3 = (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(pvVar3)), 0, 0x10e30));
    return pvVar3;
  }
  iVar11 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 1) {
    (*(uint64_t*)rtguest::G(0x177ea30)) = 0x200565c59940ab26;
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  }
  else {
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)));
  puVar9 = (uint32_t)(iVar1 + 0x8cb0);
  iVar13 = iVar1 + 0x8d50;
  if ((uVar6 & 0x8000) != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = 1;
      SetBaseValue(iVar1,1);
      uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    }
    goto LAB_00252488;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = 0;
    SetBaseValue(iVar1,0);
  }
  fVar2 = (*(uint32_t*)rtguest::G(0x333030));
  fVar15 = (*(uint32_t*)rtguest::G(0x33302c));
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar9))) {
  case 0:
    fVar16 = 0.0;
    iVar12 = 0x2ac0e0;
    iVar11 = 0;
    do {
      (*(int32_t*)rtguest::G((uint32_t)aiStack_150 + (uint32_t)(iVar11) * 4)) = (int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 2)));
      vu0SubVector(auStack_f0,0x002ac0c0 + iVar11 * 0x40,iVar11 * 0x40 + 0x2ac0d0);
      (*(uint32_t*)rtguest::G((uint32_t)uStack_ec)) = 0;
      (*(float*)rtguest::G((uint32_t)fStack_e4)) = rtguest::fbits(vecDotProductVu0(auStack_f0,auStack_f0));
      (*(float*)rtguest::G((uint32_t)fStack_e4)) = SQRT((*(float*)rtguest::G((uint32_t)fStack_e4)));
      fVar14 = 0.0;
      iVar10 = iVar11 + 1;
      if (fVar16 < (*(float*)rtguest::G((uint32_t)fStack_e4)) - fVar15) {
        fVar14 = ((*(float*)rtguest::G((uint32_t)fStack_e4)) - fVar15) / fVar2;
      }
      (*(int32_t*)rtguest::G((uint32_t)aiStack_120 + (uint32_t)(iVar11) * 4)) = (int)fVar14;
      iVar12 = iVar12 + 0x40;
      iVar11 = iVar10;
    } while (iVar10 < 9);
    InitGolfCourseTable(iVar13,aiStack_150,aiStack_120);
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 2;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
  case 2:
    SetGamePausedState(param_2,1);
    SetGolfCurrentHole(iVar13,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))));
    gs_tasks_spawn(param_1,0x252880,iVar13);
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
  case 3:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) == '\0') {
      return (uint32_t)0x0;
    }
    uiFade(0xffffffff80000000);
    pvVar3 = (uint32_t)0xa;
    goto LAB_00252384;
  case 10:
    SetGamePausedState(param_2,0);
    InitMiniGameBallState(puVar9,param_2);
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffefb | 0x10000;
    gs_music_play_bgm(0xa0d0201);
    vu0SubVector(auStack_d0,iVar1 + 0x8d10,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))) * 0x40 + 0x2ac0d0);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_cc)) = 0;
    (*(float*)rtguest::G((uint32_t)fStack_c4)) = rtguest::fbits(vecDotProductVu0(auStack_d0,auStack_d0));
    (*(float*)rtguest::G((uint32_t)fStack_c4)) = SQRT((*(float*)rtguest::G((uint32_t)fStack_c4)));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_c4)) - (*(uint32_t*)rtguest::G(0x333034))) {
      fVar15 = ((*(float*)rtguest::G((uint32_t)fStack_c4)) - (*(uint32_t*)rtguest::G(0x333034))) / (*(uint32_t*)rtguest::G(0x333038));
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
    }
    else {
      fVar15 = 0.0;
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8ce8))) = fVar15;
    InitMode17(iVar1,iVar13,(int)fVar15,(*(uint32_t*)rtguest::G((uint32_t)0x2ac0e2 + (uint32_t)(iVar13 * 0x20) * 4)));
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0xb;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
    goto LAB_00251efc;
  case 0xb:
LAB_00251efc:
    piVar8 = (uint32_t)(iVar1 + 0x8cb4);
    uiFade(0xffffffff80000000);
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
    (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
    if (7 < iVar13) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x1e;
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
LAB_00251f38:
      piVar8 = (uint32_t)(iVar1 + 0x8cb4);
      uiFade((0x40 - (*(int32_t*)rtguest::G((uint32_t)piVar8))) * 0x2000000);
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
      if (0x3f < iVar13) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffeffff | 0x104;
        SetTrackMeta(iVar1,1);
        PlayBgmSlot1();
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x1f;
        (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
LAB_00251fa8:
        piVar8 = (uint32_t)(iVar1 + 0x8cb4);
        iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
        (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
        if (0x1f < iVar13) {
          (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x21;
          (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
LAB_00251fd4:
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 4;
          (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
          (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x20;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
LAB_0025200c:
          lVar5 = CalculateBallLaunchImpulse(puVar9,param_2);
          if (lVar5 != 0) {
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffffb;
            (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x22;
            (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
          }
        }
      }
    }
    break;
  case 0x1e:
    goto LAB_00251f38;
  case 0x1f:
    goto LAB_00251fa8;
  case 0x20:
    goto LAB_0025200c;
  case 0x21:
    goto LAB_00251fd4;
  case 0x22:
    if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) < 0x20) &&
       (iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) + 1, (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = iVar13, 0x1f < iVar13)) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 0x80000000;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    }
    ApplyCupAttraction(puVar9,param_2);
    lVar5 = UpdateBallPhysics(puVar9,param_2);
    if (lVar5 == 0) {
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
    }
    else {
      if ((*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8ce8))) == 0.0) {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0x7ffffeff;
        if ((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))) + 5) * sizeof(uint32_t))) == 0) {
          PlayUiSound(0x319);
        }
        SetTrackMeta(iVar1,0);
        SetTrackMeta4(iVar1,1);
        SetShadowConfig(iVar1,1);
        uVar4 = 0x24;
      }
      else {
        uVar4 = 0x23;
      }
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = uVar4;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
    }
    vu0SubVector(auStack_b0,iVar1 + 0x8d10,iVar13 * 0x40 + 0x2ac0d0);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_ac)) = 0;
    (*(float*)rtguest::G((uint32_t)fStack_a4)) = rtguest::fbits(vecDotProductVu0(auStack_b0,auStack_b0));
    (*(float*)rtguest::G((uint32_t)fStack_a4)) = SQRT((*(float*)rtguest::G((uint32_t)fStack_a4)));
    if (0.0 < (*(float*)rtguest::G((uint32_t)fStack_a4)) - (*(uint32_t*)rtguest::G(0x33303c))) {
      fVar15 = ((*(float*)rtguest::G((uint32_t)fStack_a4)) - (*(uint32_t*)rtguest::G(0x33303c))) / (*(uint32_t*)rtguest::G(0x333040));
    }
    else {
      fVar15 = 0.0;
    }
    (*(float*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8ce8))) = fVar15;
    SetTrackMeta2(iVar1,(int)fVar15);
    break;
  case 0x23:
    piVar8 = (uint32_t)(iVar1 + 0x8cb4);
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
    (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
    if (0x40 < iVar13) {
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
      iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
      iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8 + (uint32_t)(iVar12 + 4) * sizeof(int32_t)));
      if (iVar13 < 0x62) {
        (*(int32_t*)rtguest::G((uint32_t)piVar8 + (uint32_t)(iVar12 + 4) * sizeof(int32_t))) = iVar13 + 1;
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0)));
      }
      SetTrackMeta3(iVar1,(*(int32_t*)rtguest::G((uint32_t)piVar8 + (uint32_t)(iVar12 + 4) * sizeof(int32_t))));
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x21;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0x7fffffff;
    }
    break;
  case 0x24:
    piVar8 = (uint32_t)(iVar1 + 0x8cb4);
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
    (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
    if (0xbf < iVar13) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 0x10000;
      PlayBgmSlot5();
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x25;
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
      goto LAB_002522bc;
    }
    break;
  case 0x25:
LAB_002522bc:
    piVar8 = (uint32_t)(iVar1 + 0x8cb4);
    uiFade((*(int32_t*)rtguest::G((uint32_t)piVar8)) << 0x18);
    iVar13 = (*(int32_t*)rtguest::G((uint32_t)piVar8));
    (*(int32_t*)rtguest::G((uint32_t)piVar8)) = iVar13 + 1;
    if (0x7f < iVar13) {
      PlayBgmSlot2();
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x28;
      (*(int32_t*)rtguest::G((uint32_t)piVar8)) = 0;
    }
    break;
  case 0x28:
    gs_music_play_bgm(0xa0b0102);
    PlayBgmSlot1();
    SetGamePausedState(param_2,1);
    RecordGolfHoleScore(iVar13,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)(iVar1 + 0x8cb4)) + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))) + 4) * 4)));
    gs_tasks_spawn(param_1,0x252cb0,iVar13);
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0x29;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
  case 0x29:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) == '\0') {
      return (uint32_t)0x0;
    }
    uiFade(0xffffffff80000000);
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))) + 1;
    if (iVar7 < 9) {
      pvVar3 = (uint32_t)0x2;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))) = iVar7;
    }
    else {
      pvVar3 = (uint32_t)0xffffffff;
    }
LAB_00252384:
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = pvVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
    return pvVar3;
  case 0xffffffff:
  case 1:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 0xc:
  case 0xd:
  case 0xe:
  case 0xf:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x26:
  case 0x27:
  default:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 1;
  }
  BuildTransformMatrix(0x177eb10,iVar1 + 0x8cf0,iVar1 + 0x8d00);
  vu0CopyVectorXYZ(0x177eb40,iVar1 + 0x8d10);
  vu0CopyVector(auStack_a0,iVar1 + 0x8d10);
  (*(uint32_t*)rtguest::G((uint32_t)apuStack_7c + (uint32_t)(0) * 4)) = (uint32_t)0x0;
  lVar5 = rtCall((*(uint32_t*)rtguest::G(0x335710)), auStack_a0, auStack_80, apuStack_7c, 2, param_2);
  if (lVar5 != -1) {
    (*(uint64_t*)rtguest::G((uint32_t)uStack_90)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apuStack_7c + (uint32_t)(0) * 4))));
    (*(uint64_t*)rtguest::G((uint32_t)uStack_88)) = (*(uint32_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)apuStack_7c + (uint32_t)(0) * 4)) + (uint32_t)(1) * 4));
    vecNormalizeVu0(uStack_90,uStack_90);
    BuildYawTransform(0x177eb50,uStack_90);
    vu0CopyVectorXYZ(0x177eb80,auStack_a0);
    (*(uint32_t*)rtguest::G(0x177eb84)) = (*(uint32_t*)rtguest::G(0x177eb84)) + (*(uint32_t*)rtguest::G(0x333044));
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d30))) != 0) {
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d30))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d30))) + -1;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d34))) == 0) {
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d30))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d30))) = 10;
      PlayUiSound();
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d34))) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8d34))) = 0;
    }
    uVar6 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  }
LAB_00252488:
  if ((uVar6 & 0x10000) == 0) {
    gs_map_draw_functions_scissored(param_2,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc0))));
    raceMapMarkers(param_2);
    raceMapPickups(param_2,1);
    raceHudDraw(iVar1);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8)));
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8)));
  }
  pvVar3 = (uint32_t)0x0;
  if (iVar1 != 0) {
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
    pvVar3 = (uint32_t)gs_tasks_kill(param_1);
  }
  return pvVar3;
}


RT_GUEST_FN(0x00251b60, guest::UpdateGolfMiniGame)

}  // namespace guest
