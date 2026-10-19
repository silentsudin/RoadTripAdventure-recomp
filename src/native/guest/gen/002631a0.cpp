// guest:0x002631a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t targetCourseRaceTask(uint32_t param_1, uint32_t param_2, ...) {
  char cVar1;
uint32_t __s;
  int iVar2;
  uint uVar3;
  int iVar4;
  long lVar5;
  short sVar6;
  undefined4 uVar7;
  int iVar8;
uint32_t puVar9;
uint32_t pfVar10;
  int iVar11;
  long unaff_s5;
  float fVar12;
uint32_t auStack_60 = rtguest::allocFrame(16);
  
  iVar8 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 8))) == 0) {
    __s = (uint32_t)getExtraPacketArea(0x8cc0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    InitObjectType20((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
    gs_music_play_bgm(0xa100201);
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb0))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = 0;
    SetObjectField844C(iVar4,0);
    return 0;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  puVar9 = (uint32_t)(iVar4 + 0x8cb0);
  iVar11 = (int)param_2;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar9))) {
  case 0:
    ResetVehicleState(param_2,0x177ac50);
    StoreIndexedPosition((*(uint32_t*)rtguest::G(0x33319c)),(*(uint32_t*)rtguest::G(0x3331a0)),(*(uint32_t*)rtguest::G(0x3331a4)),0,0xffffffffffff8000);
    SetBannerConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + 1);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffeffff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 1;
    GetOrSetPresidentRaceResult(0);
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8)));
    goto LAB_002632d8;
  case 1:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8)));
LAB_002632d8:
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = iVar2 + 1;
    if (0x77 < iVar2 + 1) {
      SetObjectField844C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1);
      SetBannerConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 0x104;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) == '\0') {
        PlayBgmSlot1();
      }
    }
    if (700.0 < (*(uint32_t*)rtguest::G(0x177ace8))) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
      goto LAB_00263684;
    }
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 2;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = 0;
    break;
  case 2:
    lVar5 = GetSpeedFromF15Vector(0x177ad40);
    if (lVar5 != 0) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    }
    iVar2 = 0;
    pfVar10 = (uint32_t)0x002da090;
    do {
      vu0SubVector(auStack_60,0x2da0b0,0x177ace0);
      fVar12 = (float)vecDotProductVu0(auStack_60,auStack_60);
      if (SQRT(fVar12) < (*(float*)rtguest::G((uint32_t)pfVar10))) {
        unaff_s5 = SetIndexedValueAndSum
                             ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),
                              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2da0a0u + iVar2 * 2))));
        GetOrSetPresidentRaceResult(1);
        break;
      }
      iVar2 = iVar2 + 1;
      pfVar10 = pfVar10 + 1;
    } while (iVar2 < 4);
    if (iVar2 == 4) {
      unaff_s5 = SetIndexedValueAndSum
                           ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),0);
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    }
    if (cVar1 == '\x02') {
      lVar5 = UpdateMaxCounter846C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
      if (((lVar5 == 0) || (lVar5 = TestStateBitFc78(0x46), lVar5 == 0)) &&
         ((unaff_s5 < 200 || (lVar5 = TestStateBitFc78(0x46), lVar5 != 0)))) {
        GetOrSetPresidentRaceResult(1);
        if (unaff_s5 == 300) {
          GetOrSetPresidentRaceResult(3);
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        }
        else {
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        }
        goto LAB_00263500;
      }
      GetOrSetPresidentRaceResult(2);
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) + 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = iVar2;
      if (0x77 < iVar2) {
        uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = 0;
        SetObjectField845C(uVar7,0);
        SetRoadData2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1);
        goto LAB_0026350c;
      }
      SetObjectField845C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1);
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    }
    else {
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
LAB_00263500:
      SetObjectField845C(uVar7,1);
LAB_0026350c:
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 3;
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = uVar3 & 0xfffffeff;
    break;
  case 3:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) + 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = iVar2;
    if (0xb3 < iVar2) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 0x10000;
      SetObjectField845C(iVar4,0);
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffefb;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) < 2) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 4;
      }
      else {
        (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 7;
      }
    }
    break;
  case 4:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x18);
    sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x80) {
LAB_0026365c:
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = sVar6 + 1;
    }
    else {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 6;
    }
    break;
  case 5:
    break;
  case 6:
    uiFade(0xffffffff80000000);
    StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331a8)),(*(uint32_t*)rtguest::G(0x3331ac)),(*(uint32_t*)rtguest::G(0x3331b0)),0,0xffffffffffff8000);
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + '\x01';
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
    SetObjectField844C(uVar7,0);
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  case 7:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 8;
    PlayBgmSlot5();
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  case 8:
    uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x18);
    sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x80) goto LAB_0026365c;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0xffffffff;
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 1;
    break;
  default:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
  }
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
LAB_00263684:
  if ((uVar3 & 0x10000) == 0) {
    raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4)));
  }
  else {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4)));
  }
  if (iVar4 != 0) {
    SoundStopAllChannels();
    uiFade(0xffffffff80000000);
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
  }
}


RT_GUEST_FN(0x002631a0, guest::targetCourseRaceTask)

}  // namespace guest
