// guest:0x0022aeb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t shopScene(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(272);
uint32_t auStack_110 = rtguest::allocFrame(64);
    uint32_t uStack_d0 = rtguest::allocFrame(4);
    uint32_t uStack_cc = rtguest::allocFrame(4);
    rtguest::allocFrame(8);  // guest frame gap
uint32_t auStack_c0 = rtguest::allocFrame(16);
uint32_t auStack_b0 = rtguest::allocFrame(16);
uint32_t auStack_a0 = rtguest::allocFrame(48);
    uint32_t uStack_70 = rtguest::allocFrame(4);
    rtguest::allocFrame(4);  // guest frame gap
    uint32_t uStack_68 = rtguest::allocFrame(4);
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  short sVar4;
  int iVar5;
  long lVar6;
  undefined8 uVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  undefined4 uVar11;
  
  (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(uStack_d0)), 0, 8));
  UpdateSceneLightsAndFog(1,0);
  vu0UnitMatrix(auStack_110);
  SetViewDistanceGlobals(0x45000000,0x45000000);
  buildCamera(0x177e6d0,0x29f190,auStack_110,auStack_110);
  updateLightDirection(0x177e850);
  iVar9 = (int)param_1;
  iVar10 = (int)param_2;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x2ebbe0u)), 0x37);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) = 0x1785ff0;
    vu0UnitMatrix(0x17524b0);
    vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f32c)),0x17524b0,0x17524b0);
    vu0CopyVectorXYZ(0x17524e0,0x29f320);
    (*(uint32_t*)rtguest::G(0x1752538)) = 0;
    (*(uint32_t*)rtguest::G(0x1752534)) = 0;
    (*(uint32_t*)rtguest::G(0x175253c)) = (*(uint32_t*)rtguest::G(0x333680));
    (*(uint32_t*)rtguest::G(0x1752530)) = 0;
    vu0UnitMatrix(0x17524f0);
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) & 2) == 0) {
      vu0UnitMatrix(0x1752540);
      vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f33c)),0x1752540,0x1752540);
      vu0CopyVectorXYZ(0x1752570,0x29f330);
      (*(uint32_t*)rtguest::G(0x17525c8)) = 0;
      (*(uint32_t*)rtguest::G(0x17525c0)) = 0;
      (*(uint32_t*)rtguest::G(0x17525c4)) = 0;
      vu0UnitMatrix(0x1752580);
    }
    else {
      vu0UnitMatrix(0x1752540);
      vu0RotMatrixY((*(uint32_t*)rtguest::G(0x29f35c)),0x1752540,0x1752540);
      vu0CopyVectorXYZ(0x1752570,0x29f350);
      (*(uint32_t*)rtguest::G(0x17525c8)) = 0;
      (*(uint32_t*)rtguest::G(0x17525c0)) = 0;
      (*(uint32_t*)rtguest::G(0x17525c4)) = 0;
      vu0UnitMatrix(0x1752580);
    }
    gifBeginSpriteBatch(0x1785ff0,0x20045dce69329f2e);
    (*(uint32_t*)rtguest::G(0x1786040)) = 0x5003f8003ff;
    (*(uint32_t*)rtguest::G(0x1786000)) = 0x5380b;
    gifSetSpriteCount(0x1786300,1);
    uiSpriteWriteFull(0x1786310,0,0,0,0,0x280,0xe0);
    (*(uint32_t*)rtguest::G(0x1786354)) = 0x200;
    vu0StoreMatrixQuads(0x177f620);
    uVar7 = GetSceneFogParamsPtr();
    vu0LoadVf15Quad(uVar7);
    vu0LoadStateAndConstants(0x177e6d0);
    eeQuadBuild(0x1786050,0x29f1b0);
    (*(uint32_t*)rtguest::G(0x1786118)) = 0x1000;
    (*(uint32_t*)rtguest::G(0x1786088)) = 0x1000;
    (*(uint32_t*)rtguest::G(0x17860b8)) = 0x1000;
    (*(uint32_t*)rtguest::G(0x17860e8)) = 0x1000;
    gifAdPair(0x1786210,0x5fd409fe40a,8);
    gifAdPair(0x1786120,0x4fd009fe40a,8);
    PlayUiSound(0x32);
    if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) < '\n') && ((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) == '\0')) {
      (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x00335720)), 0xff, 8));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = 0;
      gs_music_play_bgm(0xa0b0101);
      goto LAB_0022b238;
    }
    lVar6 = ClassifyObjectContext(param_2);
    if (lVar6 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = 0xe;
      if (((*(uint64_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x20))) & 0xff00ff00) == 0x8000900) {
        gs_music_play_bgm(0xa0b0104);
      }
      else {
        gs_music_play_bgm(0xa0b0102);
      }
    }
    else {
      gs_music_play_bgm(0xa0b0103);
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = 0xe;
      lVar6 = ClassifyObjectContext(param_2);
      if (lVar6 == 2) {
        (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x00335720)), 0xff, 8));
      }
      else if (lVar6 < 3) {
        if (lVar6 == 1) {
LAB_0022b238:
          gs_tasks_spawn(param_1,0x20dc50,0x380000);
        }
      }
      else if (lVar6 == 3) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) = 0;
      }
      else if (lVar6 == 4) goto LAB_0022b238;
    }
    PlayBgmSlot1();
    (*(uint32_t*)rtguest::G(0x177ae8c)) = 0x40000;
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 8) != 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) = (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 0xfff7 | 4;
    }
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  }
  else {
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
  if ((cVar8 == '\0') && ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0x41)) {
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) & 2) == 0) {
      gs_tasks_spawn(param_1,0x23b168,
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) * 4 +
                      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)))))));
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 4;
    }
    cVar8 = '\x01';
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
  }
  if (cVar8 == '\x02') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0xc))) == '\a') {
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) == '\r') {
        iVar5 = (int)((int)(char)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x3e))) + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x3e))) >> 7))
                >> 1;
        lVar6 = (long)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2));
        if ((long)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) < 0) {
          lVar6 = (long)((*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) + 0xff);
        }
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) + (char)iVar5;
        (*(uint32_t*)rtguest::G(0x175253c)) = CONCAT22(((short)((uint32_t)lVar6 >> 8) + (short)iVar5) * 0x100,
                                (uint32_t)(*(uint32_t*)rtguest::G(0x175253c)));
        vu0CopyVector(auStack_c0,0x17524e0);
        vu0UnitMatrix(0x17524b0);
        vu0RotMatrixY((float)(int)(*(uint16_t*)rtguest::G((uint32_t)0x0175253c + 2)) * 3.0517578e-05 * (*(uint32_t*)rtguest::G(0x332e00)),0x17524b0,
                      0x17524b0);
        vu0CopyVectorXYZ(0x17524e0,auStack_c0);
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
      }
      else if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) == '\x05') {
        iVar5 = (int)((int)(char)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x3e))) + (uint32_t)((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x3e))) >> 7))
                >> 1;
        lVar6 = (long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2));
        if ((long)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) < 0) {
          lVar6 = (long)((*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) + 0xff);
        }
        (*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) = ((short)((uint32_t)lVar6 >> 8) + (short)iVar5) * 0x100;
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26))) + (char)iVar5;
        vu0CopyVector(auStack_b0,0x1752600);
        vu0UnitMatrix(0x17525d0);
        vu0RotMatrixY((float)(int)(*(uint16_t*)rtguest::G((uint32_t)0x0175265c + 2)) * 3.0517578e-05 * (*(uint32_t*)rtguest::G(0x332e04)),0x17525d0,
                      0x17525d0);
        vu0CopyVectorXYZ(0x1752600,auStack_b0);
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
      }
      else {
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
      }
    }
    else {
      cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
    }
    if (cVar8 != '\0') {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
      if (uVar2 == 0xffffffff) {
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 1;
        gs_tasks_spawn(param_1,0x23b168,
                       (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) * 4 +
                        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x002a7ed0 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) * 4)))))));
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
        goto LAB_0022b864;
      }
      if ((uVar2 & 0x8000) != 0) {
        copyCarModel(uVar2 & 0xff,0);
        cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
        goto LAB_0022b864;
      }
    }
    goto switchD_0022b654_caseD_7;
  }
  if (cVar8 == '\x03') {
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) != '\0') {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 1;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 1;
      lVar6 = TestStateBitFc78(100);
      if (lVar6 == 0) {
        gs_stamps_award_function(100);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) = 8;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) = 8;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x14))) = 54000;
        iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 5;
        gs_tasks_kill(param_1);
        return 0;
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) = 9;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x21))) = 8;
      gs_tasks_spawn(param_1,0x23b168,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x002a7ef4)) + 0x20))));
      cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    }
    goto LAB_0022b864;
  }
  if (cVar8 != '\x01') {
    if ((cVar8 != '\x04') || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) == '\0')) goto LAB_0022b864;
    PlayBgmSlot5();
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xc))) = 0x22c418;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) = 0;
    ReleaseStampFlag(param_2);
    goto switchD_0022b654_caseD_7;
  }
  lVar6 = ClassifyObjectContext(param_2);
  if (lVar6 == 0) {
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
  }
  else {
    DrawScoreCounter();
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24)));
  }
  if (cVar8 == '\0') {
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    goto LAB_0022b864;
  }
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
  sVar4 = (short)(uVar2 >> 8);
  switch(uVar2 & 0xff) {
  case 4:
    ReleaseStampFlag(param_2);
  case 0xb:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
    gs_tasks_kill(param_1);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    break;
  case 5:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 10;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 4;
  case 7:
  case 0xd:
  case 0xe:
switchD_0022b654_caseD_7:
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    break;
  case 6:
    PlayBgmSlot5();
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xc))) = 0x22c418;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    goto switchD_0022b654_caseD_7;
  case 8:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar4 + 1;
    gs_tasks_spawn(param_1,0x256958);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    break;
  case 9:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 2;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar4 + 1;
    gs_tasks_spawn(param_1,0x22bb88);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    break;
  case 0xc:
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 4;
  case 10:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = uVar2;
    goto switchD_0022b654_caseD_7;
  case 0xf:
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xf;
    PlayBgmSlot5();
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x23))) == '\b') {
      gs_tasks_spawn(param_1,0x26cb60,0);
    }
    else {
      gs_tasks_spawn(param_1,0x26d2c0,0);
    }
    gs_tasks_spawn(param_1,0x2082d8,0);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    break;
  default:
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
  }
LAB_0022b864:
  if ((cVar8 != '\x03') || ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) < 0x41)) {
    uVar11 = (*(uint32_t*)rtguest::G(0x332e08));
    AdvanceAnimationPhaseState(param_1);
    uVar3 = (*(uint32_t*)rtguest::G(0x332e0c));
    UpdateAnimationPhaseMachine(param_1,uStack_d0);
    vifBeginPacket(1);
    cVar8 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x26)));
    vu0UnitMatrix(auStack_a0);
    vu0RotMatrixY(((float)(uint32_t)(cVar8 + 0x80) * 0.0078125 - 1.0) * (*(uint32_t*)rtguest::G(0x332e10)),auStack_a0,
                  auStack_a0);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = uVar11;
    vu0MulMatrix(auStack_a0,0x177e6d0,auStack_a0);
    vu0LoadStateAndConstants(auStack_a0);
    eeQuadBuild(iVar1 + 0x150,0x29f230);
    vu0UnitMatrix(auStack_a0);
    (*(uint32_t*)rtguest::G((uint32_t)uStack_68)) = uVar3;
    (*(uint32_t*)rtguest::G((uint32_t)uStack_70)) = uVar11;
    vu0MulMatrix(auStack_a0,0x177e6d0,auStack_a0);
    vu0LoadStateAndConstants(auStack_a0);
    eeQuadBuild(iVar1 + 0x240,0x29f1f0);
    (*(uint32_t*)rtguest::G(0x1752524)) = (*(uint32_t*)rtguest::G(0x332e14));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G(0x177ea50)) + 8))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x1752524)) = 0;
    }
    (*(uint32_t*)rtguest::G(0x1752644)) = (*(uint32_t*)rtguest::G(0x332e18));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(uint32_t*)rtguest::G((uint32_t)0x177ea50 + (uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)))) * 4)) + 8))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x1752644)) = 0;
    }
    (*(uint32_t*)rtguest::G(0x17525b4)) = (*(uint32_t*)rtguest::G(0x332e1c));
    if (((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G(0x177ea5c)) + 8))) & 0x400) == 0) {
      (*(uint32_t*)rtguest::G(0x17525b4)) = 0;
    }
    if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) & 1) == 0) {
      vifDirect(iVar1,0x37);
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x50))) = 0x80606000;
    }
    vifUploadAt4(0x177e6d0);
    DrawCarShowroomRig(0x17524b0,0);
    DrawCarShowroomRig(0x1752540,3);
    if ((uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) - 3U) < 5) {
      DrawCarShowroomRig(0x17525d0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))));
    }
    ResetShowroomLighting();
    DrawCarVariant0(0x17524b0,0);
    DrawCarVariant0(0x1752540,3);
    if ((uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) - 3U) < 5) {
      DrawCarVariant0(0x17525d0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))));
    }
    UploadCarColorPalette();
    drawCarExtraParts(0x1752540,3,(*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18))) != 0,0);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18))) != 0) {
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x18))) + -1;
    }
    drawCarExtraParts(0x17524b0,0,(*(uint32_t*)rtguest::G((uint32_t)uStack_d0)),0);
    if ((uint32_t)((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) - 3U) < 5) {
      drawCarExtraParts(0x17525d0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))),(*(uint32_t*)rtguest::G((uint32_t)uStack_cc)),0);
    }
    uVar7 = vifEndPacket(1);
    vifKick(uVar7);
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) == '\v') {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar10 + 0x28))) | 0x400000;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0xc))) = 0x22c418;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) = 0;
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x1c))) & 2) == 0) {
        ReleaseStampFlag(param_2);
      }
      PlayBgmSlot5();
    }
  }
  return 0;
}


RT_GUEST_FN(0x0022aeb0, guest::shopScene)

}  // namespace guest
