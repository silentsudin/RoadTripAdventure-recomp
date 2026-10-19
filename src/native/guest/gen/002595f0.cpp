// guest:0x002595f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StreetSceneTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  byte bVar6;
  int iVar7;
uint32_t puVar8;
  int iVar9;
  undefined4 uVar10;
  
  iVar9 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 8))) == 0) {
    iVar3 = getExtraPacketArea(0x8cc0);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) = iVar3;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(iVar3 + 0x8cb0))), 0, 0x10));
    InitMode13((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) >> 1 & 1);
    gs_music_play_bgm(0xa0d0201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))) + 0x8cb0))) = 0;
    return 0;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
  puVar8 = (uint32_t)(iVar3 + 0x8cb0);
  iVar7 = iVar3;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar8))) {
  case 0:
    uVar10 = 0x44340000;
    StoreIndexedPosition(0x44340000,0x40c00000,0x42c30000,0,0xffffffffffffc000);
    StoreIndexedPosition(uVar10,0x40c00000,0x42cd0000,1,0xffffffffffffc000);
    (*(uint8_t*)rtguest::G(0x177b05a)) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 1;
    break;
  case 1:
    break;
  case 2:
    GetOrSetPresidentRaceResult((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))));
    if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == 0) {
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    else {
      GetOrSetPresidentRaceResult((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) ^ 3);
      if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
        if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) & 1) != 0) {
          SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1);
        }
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) & 0x1000200) == 0x200) {
          SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1);
          goto LAB_00259934;
        }
        uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
      }
      else {
        SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) >> 1,1);
        if (((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27))) & ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) ^ 3)) == 0) {
          SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) & 1,1);
          uVar4 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) & 1;
          (*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(uVar4 * 0x138) * 2)) = (*(uint16_t*)rtguest::G((uint32_t)0x177ade8 + (uint32_t)(uVar4 * 0x138) * 2)) | 0x800;
LAB_00259934:
          uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
        }
        else {
          uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
        }
      }
    }
    iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar4 & 0xfffffeff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 3;
  case 3:
    raceHudDraw(iVar7);
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) + -0x40) * 0x1000000);
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2)));
    }
    if (sVar2 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 2))) = sVar2 + 1;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 0xffffffff;
    }
LAB_002599a8:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
    goto LAB_002599ac;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 1;
    goto LAB_002599a8;
  default:
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
    goto LAB_002599ac;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25))) == '\0') {
    bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    if ((bVar6 & 1) == 0) {
      CheckCarArrivedAtZone(param_1,0);
      bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    }
    if ((bVar6 & 2) == 0) {
      CheckCarArrivedAtZone(param_1,1);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x25)));
    }
    if (cVar1 != '\0') goto LAB_00259788;
    bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    if ((bVar6 & 2) != 0) goto LAB_00259790;
    if ((bVar5 & 1) != 0) goto LAB_00259788;
  }
  else {
LAB_00259788:
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 2;
    bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
LAB_00259790:
    if ((bVar5 & 1) == 0) {
      bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    }
    else {
      SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),0,1);
      bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
      bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
    }
  }
  if (((bVar6 & 2) != 0) && ((bVar5 & 2) != 0)) {
    SetRoadData4((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))),1,1);
    bVar5 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x27)));
  }
  if (bVar5 == 3) {
    (*(uint32_t*)rtguest::G((uint32_t)puVar8)) = 2;
    bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
  }
  else {
    bVar6 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
  }
  if (((bVar6 & 2) == 0) && ((*(uint32_t*)rtguest::G(0x177af50)) < 231.0)) {
    (*(uint32_t*)rtguest::G(0x177afb0)) = 0;
    (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
    (*(uint32_t*)rtguest::G(0x177afb8)) = 0;
  }
  raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x28))));
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)));
LAB_002599ac:
  if (iVar3 != 0) {
    uiFade(0xffffffff80000000);
    iVar9 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar9 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x002595f0, guest::StreetSceneTask)

}  // namespace guest
