// guest:0x0025e278 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t balloonCourseTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  short sVar1;
  int iVar2;
  undefined4 uVar3;
uint32_t __s;
  int iVar4;
  uint uVar5;
  undefined8 uVar6;
  long lVar7;
uint32_t puVar8;
  int iVar9;
  uint uVar10;
  long lVar11;
  int iVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  
  uVar3 = (*(uint32_t*)rtguest::G(0x333118));
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x90c0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x90c0));
    InitObjectType21((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))));
    gs_music_play_bgm(0xa100201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) + 0x90b4))) = 0;
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 1) {
      (*(int32_t*)rtguest::G(0x177f5c8)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(uint64_t*)rtguest::G(0x177ea48)) = (*(uint64_t*)rtguest::G(0x177ea30));
      (*(int32_t*)rtguest::G(0x177f598)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(int32_t*)rtguest::G(0x177f5b0)) = (*(uint32_t*)rtguest::G(0x177f580));
      (*(uint64_t*)rtguest::G(0x177ea38)) = (*(uint64_t*)rtguest::G(0x177ea30));
      (*(uint64_t*)rtguest::G(0x177ea40)) = (*(uint64_t*)rtguest::G(0x177ea30));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    }
    iVar12 = iVar2;
    switch((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4)))) {
    case 0:
      uVar10 = 0;
      iVar12 = 0x370;
      lVar11 = 0;
      uVar5 = 0;
      uVar14 = (*(uint32_t*)rtguest::G(0x333114));
      puVar8 = (uint32_t)(iVar2 + 0x8cb0);
      do {
        lVar7 = (*(int64_t*)rtguest::G((uint32_t)param_2)) + lVar11;
        lVar11 = lVar11 + 1;
        uVar6 = __muldi3((*(uint32_t*)rtguest::G((uint32_t)0x2c4be0 + (uint32_t)(uVar5) * 4)),lVar7);
        iVar4 = __umoddi3(uVar6,5);
        uVar13 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002c4bf0 + iVar4 * 4)));
        (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(puVar8 + 6))) = (char)uVar10;
        uVar10 = uVar10 + 1;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(2) * sizeof(uint32_t))) = (float)iVar12;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = uVar13;
        iVar12 = iVar12 + 0x13;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(1) * sizeof(uint32_t))) = uVar14;
        (*(uint32_t*)rtguest::G((uint32_t)puVar8 + (uint32_t)(4) * sizeof(uint32_t))) = uVar3;
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)puVar8 + 0x19))) = 0;
        gs_tasks_spawn(param_1,0x25df80,puVar8);
        uVar5 = uVar10 & 0xf;
        puVar8 = puVar8 + 0x20;  // T* element step
      } while ((int)uVar10 < 0x20);
      SelectMeshIntersectionRoutine(1);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 1;
    case 1:
      if ((*(uint32_t*)rtguest::G(0x33311c)) <= (*(uint32_t*)rtguest::G(0x177ace8))) {
        SetRoadData((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1);
        GetOrSetPresidentRaceResult(1);
        (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x21))) = 0xd;
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 2;
        rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x334e88u)));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
      }
      else {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) == 1) {
          PlayUiSound(0x14);
          SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1);
          GetOrSetPresidentRaceResult(0);
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 2;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) & 0xfffffffb;
          (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0;
          (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 0;
          rtguest::rtPrintf((char *)rtguest::G((uint32_t)(0x334e90u)));
        }
        raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))));
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
      }
      break;
    case 2:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) & 0xfffffeff;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 3;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = 0;
    case 3:
      sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
      if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) {
        uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) + -0x40) * 0x1000000);
        iVar12 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
        sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
      }
      if (sVar1 < 0xc0) {
        (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar1 + 1;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b4))) = 0xffffffff;
      }
      break;
    case 0xffffffff:
    default:
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b8))) = 1;
    }
    raceHudDraw(iVar12);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x90b8))) != 0) {
      SoundStopAllChannels();
      uiFade(0xffffffff80000000);
      iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 5))) | 8;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
    }
  }
  return 0;
}


RT_GUEST_FN(0x0025e278, guest::balloonCourseTask)

}  // namespace guest
