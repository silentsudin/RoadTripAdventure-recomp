// guest:0x00260638 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t goalAreaTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  uint uVar2;
  float fVar3;
uint32_t __s;
  int iVar4;
  int iVar5;
uint32_t puVar6;
  float fVar7;
  undefined4 uVar8;
  
  iVar5 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x8cc0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    ResetMode16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))));
    gs_music_play_bgm(0xa100201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) + 0x8cb0))) = 0;
    (*(uint8_t*)rtguest::G(0x177b05a)) = 1;
    return 0;
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
  puVar6 = (uint32_t)(iVar1 + 0x8cb0);
  iVar4 = iVar1;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar6))) {
  case 0:
    uVar8 = 0x41c40000;
    StoreIndexedPosition((*(uint32_t*)rtguest::G(0x333168)),0x41c40000,0x43c20000,0,0);
    StoreIndexedPosition((*(uint32_t*)rtguest::G(0x33316c)),uVar8,0x43c20000,1,0);
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 1;
  case 1:
    if ((((*(uint32_t*)rtguest::G(0x177ace0)) < 410.0) || (430.0 < (*(uint32_t*)rtguest::G(0x177ace0)))) || ((*(uint32_t*)rtguest::G(0x177ace8)) < 370.0)
       ) {
      GetOrSetPresidentRaceResult(0);
      SetRoadData5((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),0,1);
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff;
    }
    fVar3 = (*(uint32_t*)rtguest::G(0x177af58));
    fVar7 = (*(uint32_t*)rtguest::G(0x333170));
    if ((*(uint32_t*)rtguest::G(0x333170)) < (*(uint32_t*)rtguest::G(0x177ace8))) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff;
      if (fVar7 < fVar3) {
        GetOrSetPresidentRaceResult(1);
        SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),0,1);
        fVar7 = (*(uint32_t*)rtguest::G(0x333174));
      }
      else {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) < 0x1e1) {
          GetOrSetPresidentRaceResult(3);
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
        }
        else {
          GetOrSetPresidentRaceResult(2);
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
        }
        SetWheelSprite(uVar8,0,1);
        fVar7 = (*(uint32_t*)rtguest::G(0x333174));
      }
    }
    if (fVar7 < (*(uint32_t*)rtguest::G(0x177af58))) {
      (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    else {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    if ((uVar2 & 4) == 0) {
LAB_002608e4:
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8)));
    }
    else {
      if ((uVar2 & 0x8000) == 0) {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) + 1;
        goto LAB_002608e4;
      }
      uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8)));
    }
    InitHudConfig2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),uVar8);
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 3;
    break;
  case 3:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) << 0x18);
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) < 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) + 1;
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    }
    else {
      iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 0xffffffff;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 1;
  }
  raceHudDraw(iVar4);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x00260638, guest::goalAreaTask)

}  // namespace guest
