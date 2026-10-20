// guest:0x0025eb70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t duelCourseTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
  }
  int iVar1;
  float fVar2;
  byte bVar3;
  undefined2 uVar4;
uint32_t __s;
  uint uVar5;
  int iVar6;
  short sVar7;
  long lVar8;
uint32_t puVar9;
uint32_t psVar10;
  int iVar11;
  int iVar12;
  
  iVar12 = (int)param_1;
  iVar11 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x25eb9c, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x25ebc4, guest::InitMode1D((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) >> 1 & 1));
    RT_CALLRA(0x25ebd0, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x25ebe4, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)))));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 0xffff;
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 0;
    }
  }
  else {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)));
  }
  psVar10 = (uint32_t)(iVar1 + 0x8cb0);
  switch((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4)))) {
  case 0:
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
      RT_CALLRA(0x25ec7c, guest::StoreIndexedPosition(0x43250000,0x43060000,0x42cd0000,0,0));
    }
    else {
      RT_CALLRA(0x25eca8, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x333128)),0x43060000,0x42cd0000,0,0));
    }
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 2;
    (*(int8_t*)rtguest::G(0x177ae9a)) = 0;
    (*(int16_t*)rtguest::G((uint32_t)psVar10)) = 0;
    break;
  case 1:
    RT_CALLRA(0x25ecd0, guest::UploadVifPacketA(0xffffffff80000000));
    RT_CALLRA(0x25ecdc, guest::InitCameraTransforms(param_2,0x177ac50));
    iVar6 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a)) * 0x10;
    RT_CALLRA(0x25ed30, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c4d40))),(*(float*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c4d44))) + 4.0,
               (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c4d48))),0,(short)(int)(*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4d4c + iVar6)))));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 6;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
    break;
  case 2:
    if ((*(int8_t*)rtguest::G(0x177ae9a)) < 0xe) {
      iVar6 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a)) * 0x10;
      if (0.0 <= ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c6c + iVar6))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c64 + iVar6)))) *
                 ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c68 + iVar6)))) -
                 ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c68 + iVar6))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c60 + iVar6)))) *
                 ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c6c + iVar6))))) {
        (*(int8_t*)rtguest::G(0x177ae9a)) = (*(int8_t*)rtguest::G(0x177ae9a)) + 1;
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a));
      }
      else {
        uVar5 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a));
      }
    }
    else {
      uVar5 = (uint32_t)(*(int8_t*)rtguest::G(0x177ae9a));
    }
    if (uVar5 == 0) {
LAB_0025ee44:
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    }
    else {
      if (((float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c5c + (uint32_t)(uVar5 * 4) * 4)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c54 + (uint32_t)(uVar5 * 4) * 4))) *
          ((*(uint32_t*)rtguest::G(0x177ace0)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c58 + (uint32_t)(uVar5 * 4) * 4))) -
          ((float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c58 + (uint32_t)(uVar5 * 4) * 4)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c50 + (uint32_t)(uVar5 * 4) * 4))) *
          ((*(uint32_t*)rtguest::G(0x177ace8)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c5c + (uint32_t)(uVar5 * 4) * 4))) < 0.0) {
        (*(int8_t*)rtguest::G(0x177ae9a)) = (char)uVar5 - 1;
        goto LAB_0025ee44;
      }
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    }
    if ((((bVar3 & 2) == 0) && ((*(int8_t*)rtguest::G(0x177ae9a)) == 0)) &&
       (0.0 <= ((*(uint32_t*)rtguest::G(0x2c4f1c)) - (*(uint32_t*)rtguest::G(0x2c4f14))) * ((*(uint32_t*)rtguest::G(0x177ace0)) - (*(uint32_t*)rtguest::G(0x2c4f18))) -
               ((*(uint32_t*)rtguest::G(0x2c4f18)) - (*(uint32_t*)rtguest::G(0x2c4f10))) * ((*(uint32_t*)rtguest::G(0x177ace8)) - (*(uint32_t*)rtguest::G(0x2c4f1c))))) {
      RT_CALLRA(0x25eeb8, guest::GetOrSetPresidentRaceResult(0));
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = uVar5 & 0xfffffffb;
    }
    if (((*(int8_t*)rtguest::G(0x177ae9a)) < 0xe) && ((*(uint32_t*)rtguest::G(0x177ace4)) <= (float)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2c4f00 + (uint32_t)((*(int8_t*)rtguest::G(0x177ae9a))) * 4)))) {
      RT_CALLRA(0x25ef1c, guest::GetOrSetPresidentRaceResult(0));
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      uVar4 = 5;
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
        uVar4 = 3;
      }
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = uVar4;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff;
    }
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    if ((*(int8_t*)rtguest::G(0x177ae9a)) == 0xe) {
      if ((bVar3 & 2) == 0) {
        (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 3;
        RT_CALLRA(0x25efb8, guest::GetOrSetPresidentRaceResult(1,0));
        RT_CALLRA(0x25eff4, guest::SetBumperConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),0,1));
        if (0x1c20 < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc)))) {
          bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
          goto LAB_0025f018;
        }
        RT_CALLRA(0x25efe4, guest::GetOrSetPresidentRaceResult(2));
      }
      else {
        RT_CALLRA(0x25ef7c, guest::SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),0,1));
        RT_CALLRA(0x25ef8c, guest::SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),1,1));
        (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
        (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 3;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 3;
        RT_CALLRA(0x25f00c, guest::GetOrSetPresidentRaceResult(2));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
      }
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    }
LAB_0025f018:
    if ((bVar3 & 2) == 0) {
      iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc)));
      if ((*(int8_t*)rtguest::G(0x177ae9a)) == 0) {
        if (0 < iVar6) {
          uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
          goto LAB_0025f04c;
        }
      }
      else {
        uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
LAB_0025f04c:
        if (((uVar5 & 4) != 0) && ((uVar5 & 0x8000) == 0)) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = iVar6 + 1;
          iVar6 = iVar6 + 1;
        }
      }
      RT_CALLRA(0x25f068, guest::InitHudConfig2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),iVar6));
      lVar8 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6)));
    }
    else {
      lVar8 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6)));
    }
    goto LAB_0025f1b8;
  case 3:
    RT_CALLRA(0x25f078, guest::PlayBgmSlot5());
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 4;
LAB_0025f1b0:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = uVar5;
    break;
  case 4:
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)psVar10))) {
      iVar6 = (*(int16_t*)rtguest::G((uint32_t)psVar10)) + -0x40;
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
        RT_CALLRA(0x25f0c8, guest::uiFade(iVar6 * 0x1000000));
        sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
      }
      else {
        RT_CALLRA(0x25f0b8, guest::UploadVifPacketA(iVar6 * 0x1000000));
        sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
      }
    }
    if (sVar7 < 0xc0) {
LAB_0025f14c:
      (*(int16_t*)rtguest::G((uint32_t)psVar10)) = sVar7 + 2;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 1;
    }
    break;
  case 5:
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)psVar10))) {
      iVar6 = (*(int16_t*)rtguest::G((uint32_t)psVar10)) + -0x40;
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
        RT_CALLRA(0x25f128, guest::uiFade(iVar6 * 0x1000000));
        sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
      }
      else {
        RT_CALLRA(0x25f118, guest::UploadVifPacketA(iVar6 * 0x1000000));
        sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
      }
    }
    if (sVar7 < 0xc0) goto LAB_0025f14c;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 1;
    break;
  case 6:
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10))) & 2) == 0) {
      RT_CALLRA(0x25f180, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)psVar10)) << 0x18));
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
    }
    else {
      RT_CALLRA(0x25f170, guest::UploadVifPacketA((int)(*(int16_t*)rtguest::G((uint32_t)psVar10)) << 0x18));
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)psVar10));
    }
    if (sVar7 < 1) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 2;
      uVar5 = uVar5 | 0x100;
      goto LAB_0025f1b0;
    }
    (*(int16_t*)rtguest::G((uint32_t)psVar10)) = (*(int16_t*)rtguest::G((uint32_t)psVar10)) + -2;
    break;
  case 7:
    break;
  default:
    lVar8 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6)));
    goto LAB_0025f1b8;
  }
  lVar8 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6)));
LAB_0025f1b8:
  switch(lVar8) {
  case 0:
    RT_CALLRA(0x25f1f8, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x33312c)),0x43060000,0x42cd0000,1,0));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 2;
    (*(uint32_t*)rtguest::G(0x177b10a)) = 0;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) = 0;
    break;
  case 1:
    RT_CALLRA(0x25f218, guest::UploadVifPacketB(0xffffffff80000000));
    RT_CALLRA(0x25f22c, guest::InitCameraTransforms(param_2));
    iVar6 = (uint32_t)(*(uint32_t*)rtguest::G(0x177b10a)) * 0x10;
    RT_CALLRA(0x25f280, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c4e20))),(*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4e24 + iVar6))) + 4.0,
               (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x2c4e28))),1,(short)(int)(*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4e2c + iVar6)))));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 6;
    (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) & 0xf7ff;
    break;
  case 2:
    if ((*(uint32_t*)rtguest::G(0x177b10a)) < 0xe) {
      iVar6 = (uint32_t)(*(uint32_t*)rtguest::G(0x177b10a)) * 0x10;
      if (0.0 <= ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c6c + iVar6))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c64 + iVar6)))) *
                 ((*(uint32_t*)rtguest::G(0x177af50)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c68 + iVar6)))) -
                 ((*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c68 + iVar6))) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c60 + iVar6)))) *
                 ((*(uint32_t*)rtguest::G(0x177af58)) - (*(float*)rtguest::G((uint32_t)(uint32_t)(0x002c4c6c + iVar6))))) {
        (*(uint32_t*)rtguest::G(0x177b10a)) = (*(uint32_t*)rtguest::G(0x177b10a)) + 1;
        uVar5 = (uint32_t)(*(uint32_t*)rtguest::G(0x177b10a));
      }
      else {
        uVar5 = (uint32_t)(*(uint32_t*)rtguest::G(0x177b10a));
      }
    }
    else {
      uVar5 = (uint32_t)(*(uint32_t*)rtguest::G(0x177b10a));
    }
    if ((uVar5 != 0) &&
       (((float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c5c + (uint32_t)(uVar5 * 4) * 4)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c54 + (uint32_t)(uVar5 * 4) * 4))) *
        ((*(uint32_t*)rtguest::G(0x177af50)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c58 + (uint32_t)(uVar5 * 4) * 4))) -
        ((float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c58 + (uint32_t)(uVar5 * 4) * 4)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c50 + (uint32_t)(uVar5 * 4) * 4))) *
        ((*(uint32_t*)rtguest::G(0x177af58)) - (float)(*(uint32_t*)rtguest::G((uint32_t)0x2c4c5c + (uint32_t)(uVar5 * 4) * 4))) < 0.0)) {
      (*(uint32_t*)rtguest::G(0x177b10a)) = (char)uVar5 - 1;
    }
    if (((*(uint32_t*)rtguest::G(0x177b10a)) < 0xe) && ((*(uint32_t*)rtguest::G(0x177af54)) <= (float)(uint32_t)(*(uint32_t*)rtguest::G((uint32_t)0x2c4f00 + (uint32_t)((*(uint32_t*)rtguest::G(0x177b10a))) * 4)))) {
      (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 5;
    }
    if ((*(uint32_t*)rtguest::G(0x177b10a)) == 0xe) {
      RT_CALLRA(0x25f418, guest::SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),1,1));
      RT_CALLRA(0x25f428, guest::SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28))),0,1));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 3;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
      (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 3;
      RT_CALLRA(0x25f464, guest::GetOrSetPresidentRaceResult(1));
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    }
    else {
      bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    }
    goto LAB_0025f558;
  case 3:
    uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 4;
LAB_0025f550:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = uVar5;
    break;
  case 4:
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)))) {
      RT_CALLRA(0x25f4a4, guest::UploadVifPacketB(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) + -0x40) * 0x1000000));
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)));
    }
    if (sVar7 < 0xc0) {
LAB_0025f508:
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) = sVar7 + 2;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 1;
    }
    break;
  case 5:
    sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)))) {
      RT_CALLRA(0x25f4e4, guest::UploadVifPacketB(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) + -0x40) * 0x1000000));
      sVar7 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2)));
    }
    if (sVar7 < 0xc0) goto LAB_0025f508;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 1;
    break;
  case 6:
    RT_CALLRA(0x25f51c, guest::UploadVifPacketB((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) << 0x18));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) < 1) {
      uVar5 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb6))) = 2;
      uVar5 = uVar5 | 0x100;
      goto LAB_0025f550;
    }
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb2))) + -2;
    break;
  case 7:
    break;
  default:
    bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
    goto LAB_0025f558;
  }
  bVar3 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 10)));
LAB_0025f558:
  fVar2 = (*(float*)rtguest::G(0x333130));
  if ((bVar3 & 2) != 0) {
    puVar9 = 0x0177acf8;
    iVar6 = 1;
    do {
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar9 + 0x1a2))) == '\0') && (fVar2 <= (float)(*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(-6) * sizeof(uint32_t))))) {
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(puVar9 + 0x4a))) = 0;
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = (*(uint32_t*)rtguest::G((uint32_t)puVar9)) - (int)((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(0x14) * sizeof(uint32_t))) << 5) / 0x19 & 0xfffffff;
        (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(-2) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(-2) * sizeof(uint32_t))) - (int)((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(0x12) * sizeof(uint32_t))) << 5) / 0x19 & 0xfffffff;
        (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(-1) * sizeof(uint32_t))) = (*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(-1) * sizeof(uint32_t))) - (int)((*(uint32_t*)rtguest::G((uint32_t)puVar9 + (uint32_t)(0x13) * sizeof(uint32_t))) << 5) / 0x19;
      }
      iVar6 = iVar6 + -1;
      puVar9 = puVar9 + 0x270;  // T* element step
    } while (-1 < iVar6);
  }
  RT_CALLRA(0x25f630, guest::raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x28)))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) != 0) {
    RT_CALLRA(0x25f644, guest::SoundStopAllChannels());
    RT_CALLRA(0x25f64c, guest::uiFade(0xffffffff80000000));
    iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar12 + 4))) = 0xffffffff;
    RT_CALLRA(0x25f674, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x0025eb70, guest::duelCourseTask)

}  // namespace guest
