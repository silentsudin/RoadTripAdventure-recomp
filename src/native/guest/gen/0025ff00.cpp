// guest:0x0025ff00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dualViewCourseTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  long lVar6;
uint32_t psVar7;
  int iVar8;
  undefined4 uVar9;
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    uVar5 = getExtraPacketArea(0x8cd0);
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = (int)uVar5;
    InitMode14(uVar5,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x1c))));
    gs_music_play_bgm(0xa100201);
    gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)((uint32_t)(iVar8 + 0x8cb0))), 0, 0x20));
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x8cb4))) = 0;
    if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x8cb6))) = 0xffff;
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x8cb6))) = 0;
    }
    (*(uint8_t*)rtguest::G(0x177b05a)) = 1;
    return 0;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  psVar7 = (uint32_t)(iVar3 + 0x8cb0);
  switch((*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4)))) {
  case 0:
    uVar9 = 0x43ce0000;
    StoreIndexedPosition(0x43ce0000,0x42540000,(*(uint32_t*)rtguest::G(0x33313c)),0,0xffffffffffffc000);
    StoreIndexedPosition(uVar9,0x42540000,(*(uint32_t*)rtguest::G(0x333140)),1,0xffffffffffffc000);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 1;
  case 1:
    if (((*(int8_t*)rtguest::G(0x177ae9a)) != 0x13) || ((*(uint32_t*)rtguest::G(0x333144)) <= (*(uint32_t*)rtguest::G(0x177ace8)))) {
      if (((*(int8_t*)rtguest::G(0x177ae9a)) == 0x14) && ((*(uint32_t*)rtguest::G(0x333148)) <= (*(uint32_t*)rtguest::G(0x177ace8)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_002600f0;
      }
      if (((*(int8_t*)rtguest::G(0x177ae9a)) == 0x40) && ((*(uint32_t*)rtguest::G(0x33314c)) <= (*(uint32_t*)rtguest::G(0x177ace8)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_002600f0;
      }
      if (((*(int8_t*)rtguest::G(0x177ae9a)) == 0x41) && ((*(uint32_t*)rtguest::G(0x177ace8)) < (*(uint32_t*)rtguest::G(0x333150)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_002600f0;
      }
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0)));
      if ((*(int8_t*)rtguest::G(0x177ae9a)) != 0x54) goto LAB_00260148;
      if ((uVar4 & 1) == 0) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_002600f0;
      }
LAB_00260154:
      if ((*(int8_t*)rtguest::G(0x177ae9a)) == 0x55) {
        if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) == 5) {
          SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1);
          GetOrSetPresidentRaceResult(1);
        }
        else {
          SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1);
          if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
            if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) < 0xbb9) {
              GetOrSetPresidentRaceResult(3,1);
            }
            else {
              GetOrSetPresidentRaceResult(2,1);
            }
          }
          else {
            SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1,1);
            GetOrSetPresidentRaceResult(2);
          }
        }
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 2;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 2;
        goto LAB_002601fc;
      }
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    }
    else {
      uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
LAB_002600f0:
      SetObjectState(uVar9,0,1);
      if (((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10))) & 2) == 0) {
        GetOrSetPresidentRaceResult(0);
      }
      else {
        GetOrSetPresidentRaceResult(1);
        SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1,1);
      }
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 2;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 2;
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0)));
LAB_00260148:
      if ((uVar4 & 1) != 0) goto LAB_00260154;
LAB_002601fc:
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    }
    if ((((bVar1 & 2) == 0) && (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) & 2) != 0)) && ((*(uint32_t*)rtguest::G(0x177b10a)) == 0x55)) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 5;
    }
    if ((*(int8_t*)rtguest::G(0x177ae9a)) == 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) | 1;
    }
    if ((*(int8_t*)rtguest::G(0x177ae9a)) < 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) & 0xfffffffe;
    }
    if ((*(uint32_t*)rtguest::G(0x177b10a)) == 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) | 2;
    }
    if ((*(uint32_t*)rtguest::G(0x177b10a)) < 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) & 0xfffffffd;
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    }
    else {
      bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 10)));
    }
    if ((bVar1 & 2) == 0) {
      if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 4) == 0) {
LAB_002602dc:
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      else {
        if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x8000) == 0) {
          (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))) + 1;
          goto LAB_002602dc;
        }
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
      }
      InitHudConfig2(uVar9,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cbc))));
      lVar6 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6)));
    }
    else {
      lVar6 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6)));
    }
    goto LAB_00260378;
  case 2:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 3;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    break;
  case 3:
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar7));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)psVar7))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)psVar7)) + -0x40) * 0x1000000);
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)psVar7));
    }
    if (sVar2 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)psVar7)) = sVar2 + 2;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = 1;
    }
    break;
  case 4:
  case 5:
    break;
  default:
    lVar6 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6)));
    goto LAB_00260378;
  }
  lVar6 = (long)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6)));
LAB_00260378:
  switch(lVar6) {
  case 0:
    StoreIndexedPosition(0x43ce0000,0x42540000,(*(uint32_t*)rtguest::G(0x333154)),1,0xffffffffffffc000);
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 1;
  case 1:
    if (((*(uint32_t*)rtguest::G(0x177b10a)) != 0x13) || ((*(uint32_t*)rtguest::G(0x333158)) <= (*(uint32_t*)rtguest::G(0x177af58)))) {
      if (((*(uint32_t*)rtguest::G(0x177b10a)) == 0x14) && ((*(uint32_t*)rtguest::G(0x33315c)) <= (*(uint32_t*)rtguest::G(0x177af58)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_00260478;
      }
      if (((*(uint32_t*)rtguest::G(0x177b10a)) == 0x40) && ((*(uint32_t*)rtguest::G(0x333160)) <= (*(uint32_t*)rtguest::G(0x177af58)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_00260478;
      }
      if (((*(uint32_t*)rtguest::G(0x177b10a)) == 0x41) && ((*(uint32_t*)rtguest::G(0x177af58)) < (*(uint32_t*)rtguest::G(0x333164)))) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_00260478;
      }
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0)));
      if ((*(uint32_t*)rtguest::G(0x177b10a)) != 0x54) goto LAB_002604ac;
      if ((uVar4 & 2) == 0) {
        uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        goto LAB_00260478;
      }
LAB_002604b8:
      if ((*(uint32_t*)rtguest::G(0x177b10a)) == 0x55) {
        SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1,1);
        SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1);
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 2;
        (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 2;
        GetOrSetPresidentRaceResult(1);
      }
    }
    else {
      uVar9 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
LAB_00260478:
      SetObjectState(uVar9,1,1);
      SetWheelSprite((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1);
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 2;
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb4))) = 2;
      GetOrSetPresidentRaceResult(2);
      uVar4 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0)));
LAB_002604ac:
      if ((uVar4 & 2) != 0) goto LAB_002604b8;
    }
    if ((*(uint32_t*)rtguest::G(0x177b10a)) == 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) | 2;
    }
    if ((*(uint32_t*)rtguest::G(0x177b10a)) < 0x14) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cc0))) & 0xfffffffd;
    }
    break;
  case 2:
    (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb6))) = 3;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G(0x177b058)) = (*(uint32_t*)rtguest::G(0x177b058)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    break;
  case 3:
    sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb2)));
    if (0x40 < (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb2)))) {
      uiFade(((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb2))) + -0x40) * 0x1000000);
      sVar2 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb2)));
    }
    if (sVar2 < 0xc0) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb2))) = sVar2 + 2;
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) = 1;
    }
  }
  raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x8cb8))) != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
  return 0;
}


RT_GUEST_FN(0x0025ff00, guest::dualViewCourseTask)

}  // namespace guest
