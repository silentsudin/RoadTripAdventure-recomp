// guest:0x00260dc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t rouletteSceneTask(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  int iVar2;
  ulong uVar3;
  short sVar4;
  ushort uVar5;
uint32_t __s;
  uint uVar6;
uint32_t piVar7;
uint32_t puVar8;
  int iVar9;
uint32_t puVar10;
  int iVar11;
  float fVar12;
  float fVar13;
uint32_t auStack_50 = rtguest::allocFrame(16);
  
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x10d20);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x10d20));
  }
  puVar10 = (uint32_t)param_2;
  uVar6 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t)));
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x20)));
  puVar8 = (uint32_t)(iVar1 + 0x8cb0);
  iVar11 = iVar1 + 0x8cf0;
  if ((uVar6 & 0x8000) != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc8))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc8))) = 1;
      SetBaseValue(iVar1,1);
      uVar6 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t)));
    }
    goto LAB_00261338;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc8))) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cc8))) = 0;
    SetBaseValue(iVar1,0);
  }
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar8))) {
  case 0:
    gs_music_play_bgm(0xa100201);
    PlayBgmSlot1();
    initRouletteMachineState(iVar11);
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 1;
  case 1:
    uiFade(0xffffffff80000000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) & 0x7ffffffb;
    SetGamePausedState(param_2,1);
    gs_tasks_spawn(param_1,0x261ac8,iVar11);
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 2;
switchD_00260e9c_caseD_2:
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) == '\0') {
      return 0;
    }
    uiFade(0xffffffff80000000);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) != -1) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) & 0xfffeffff;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 3;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
      SetGamePausedState(param_2,0);
      InitCameraTransforms(param_2,0x177ac50);
      StoreIndexedPosition(0x43c80000,0x40a80000,0x43c80000,0,0);
      ResetMode18(iVar1);
      sVar4 = __umoddi3((*(uint64_t*)rtguest::G((uint32_t)puVar10)),0x14);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbe))) = ((uint32_t)((*(uint64_t*)rtguest::G((uint32_t)puVar10)) >> 5) & 0xf) * 0xc + 0x300;
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = sVar4 * 0xccc;
      return 0;
    }
    auditRouletteOutcome(iVar11);
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  case 2:
    goto switchD_00260e9c_caseD_2;
  case 3:
    sVar4 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) + 1;
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar4;
    if (0x20 < sVar4) {
      uVar3 = (*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t)));
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (uint32_t)uVar3 | 4;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 4;
    }
    break;
  case 4:
    (*(uint64_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(1) * 8)) = 0x43c80000;
    (*(uint64_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(0) * 8)) = 0x43c80000;
    vu0SubVector(auStack_50,0x177ace0,auStack_50);
    (*(uint64_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(0) * 8)) = (*(uint64_t*)rtguest::G((uint32_t)auStack_50 + (uint32_t)(0) * 8)) & 0xffffffff;
    fVar12 = (float)vecDotProductVu0(auStack_50,auStack_50);
    if ((*(uint32_t*)rtguest::G(0x33318c)) < SQRT(fVar12)) {
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(puVar10 + 5))) = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t))) | 0x80010000;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 5;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
    }
    break;
  case 5:
    uVar5 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbe))) - 3;
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbe))) = uVar5;
    fVar12 = (*(uint32_t*)rtguest::G(0x177ace0));
    if ((int)((uint32_t)uVar5 << 0x10) < 0) {
      fVar13 = (*(uint32_t*)rtguest::G(0x177ace8)) - 400.0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 6;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbe))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
      fVar12 = (float)atan2f(fVar12 - 400.0,fVar13);
      setRouletteReelResult
                (iVar11,(short)((((int)(short)(int)((fVar12 / (*(uint32_t*)rtguest::G(0x333190))) * 32768.0) -
                                 (uint32_t)(*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc)))) + 0x666 & 0xffff) / 0xccc));
    }
    roulettePlaceBallOnWheel(puVar8);
    break;
  case 6:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 7;
  case 7:
    piVar7 = (uint32_t)(iVar1 + 0x8cb4);
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
    (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2 + 1;
    if (0x7f < iVar2) {
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 8;
LAB_00261214:
      piVar7 = (uint32_t)(iVar1 + 0x8cb4);
      uiFade((*(int32_t*)rtguest::G((uint32_t)piVar7)) << 0x19);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)piVar7));
      (*(int32_t*)rtguest::G((uint32_t)piVar7)) = iVar2 + 1;
      if (0x3f < iVar2) {
        SetGamePausedState(param_2,1);
        gs_tasks_spawn(param_1,0x261de0,iVar11);
        (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 9;
switchD_00260e9c_caseD_9:
        if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) != '\0') {
          iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4)));
          if (iVar9 == 1) {
            SetWheelSprite(iVar1,0,1);
          }
          else {
            if (iVar9 < 2) {
              if (iVar9 == 0) {
                SetWheelSprite2(iVar1,0,1);
                goto LAB_00261308;
              }
            }
            else if (iVar9 == 2) {
              SetWheelSprite2(iVar1,0,0);
              SetWheelSprite(iVar1,0,0);
              goto LAB_00261308;
            }
            uiFade(0xffffffff80000000);
            (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 1;
          }
        }
LAB_00261308:
        raceHudDraw(iVar1);
        return 0;
      }
    }
    break;
  case 8:
    goto LAB_00261214;
  case 9:
    goto switchD_00260e9c_caseD_9;
  default:
    uiFade(0xffffffff80000000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = 1;
  }
  rouletteTablePoseUpdate(puVar8);
  uVar6 = (uint32_t)(*(uint64_t*)rtguest::G((uint32_t)puVar10 + (uint32_t)(5) * sizeof(uint64_t)));
LAB_00261338:
  if ((uVar6 & 0x10000) == 0) {
    raceHudDraw(iVar1);
  }
  return 0;
}


RT_GUEST_FN(0x00260dc0, guest::rouletteSceneTask)

}  // namespace guest
