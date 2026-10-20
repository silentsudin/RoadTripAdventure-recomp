// guest:0x002631a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t targetCourseRaceTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(96);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x50) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x54) = 0x00000000u;
  }
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
    __s = (uint32_t)RT_CALLRA(0x2631d8, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x2631f4, guest::InitObjectType20((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
    RT_CALLRA(0x263200, guest::gs_music_play_bgm(0xa100201));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb0))) = 0;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = 0;
    RT_CALLRA(0x26321c, guest::SetObjectField844C(iVar4,0));
    return 0;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
  puVar9 = (uint32_t)(iVar4 + 0x8cb0);
  iVar11 = (int)param_2;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar9))) {
  case 0:
    RT_CALLRA(0x263270, guest::ResetVehicleState(param_2,0x177ac50));
    RT_CALLRA(0x263288, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x33319c)),(*(uint32_t*)rtguest::G(0x3331a0)),(*(uint32_t*)rtguest::G(0x3331a4)),0,0xffffffffffff8000));
    RT_CALLRA(0x263298, guest::SetBannerConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + 1));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffeffff;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 1;
    RT_CALLRA(0x2632bc, guest::GetOrSetPresidentRaceResult(0));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8)));
    goto LAB_002632d8;
  case 1:
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8)));
LAB_002632d8:
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = iVar2 + 1;
    if (0x77 < iVar2 + 1) {
      RT_CALLRA(0x2632ec, guest::SetObjectField844C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1));
      RT_CALLRA(0x2632f8, guest::SetBannerConfig((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 0x104;
      if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) == '\0') {
        RT_CALLRA(0x263318, guest::PlayBgmSlot1());
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
    lVar5 = RT_CALLRA(0x263394, guest::GetSpeedFromF15Vector(0x177ad40));
    if (lVar5 != 0) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
      goto LAB_00263684;
    }
    iVar2 = 0;
    pfVar10 = (uint32_t)0x002da090;
    do {
      RT_CALLRA(0x2633c0, guest::vu0SubVector(auStack_60,0x2da0b0,0x177ace0));
      fVar12 = rtguest::fbits(RT_CALLRA(0x2633cc, guest::vecDotProductVu0(auStack_60,auStack_60)));
      if (SQRT(fVar12) < (*(float*)rtguest::G((uint32_t)pfVar10))) {
        unaff_s5 = RT_CALLRA(0x263370, guest::SetIndexedValueAndSum((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),
                              (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)((int)0x2da0a0u + iVar2 * 2)))));
        RT_CALLRA(0x26337c, guest::GetOrSetPresidentRaceResult(1));
        break;
      }
      iVar2 = iVar2 + 1;
      pfVar10 = pfVar10 + 0x4;  // T* element step
    } while (iVar2 < 4);
    if (iVar2 == 4) {
      unaff_s5 = RT_CALLRA(0x263434, guest::SetIndexedValueAndSum((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))),0));
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    }
    else {
      cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26)));
    }
    if (cVar1 == '\x02') {
      lVar5 = RT_CALLRA(0x263450, guest::UpdateMaxCounter846C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
      if (((lVar5 == 0) || (lVar5 = TestStateBitFc78(0x46), lVar5 == 0)) &&
         ((unaff_s5 < 200 || (lVar5 = TestStateBitFc78(0x46), lVar5 != 0)))) {
        RT_CALLRA(0x263488, guest::GetOrSetPresidentRaceResult(1));
        if (unaff_s5 == 300) {
          RT_CALLRA(0x2634e0, guest::GetOrSetPresidentRaceResult(3));
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        }
        else {
          uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        }
        goto LAB_00263500;
      }
      RT_CALLRA(0x2634f4, guest::GetOrSetPresidentRaceResult(2));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) + 1;
      (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = iVar2;
      if (0x77 < iVar2) {
        uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb8))) = 0;
        RT_CALLRA(0x2634a8, guest::SetObjectField845C(uVar7,0));
        RT_CALLRA(0x2634d0, guest::SetRoadData2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),0,1));
        goto LAB_0026350c;
      }
      RT_CALLRA(0x2634c0, guest::SetObjectField845C((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28))),1));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    }
    else {
      uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
LAB_00263500:
      RT_CALLRA(0x263500, guest::SetObjectField845C(uVar7,1));
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
      RT_CALLRA(0x263550, guest::SetObjectField845C(iVar4,0));
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
    RT_CALLRA(0x263594, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x18));
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
    RT_CALLRA(0x2635c0, guest::uiFade(0xffffffff80000000));
    RT_CALLRA(0x2635d8, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3331a8)),(*(uint32_t*)rtguest::G(0x3331ac)),(*(uint32_t*)rtguest::G(0x3331b0)),0,0xffffffffffff8000));
    uVar7 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)));
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x26))) + '\x01';
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0;
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) & 0xf7ff;
    RT_CALLRA(0x263608, guest::SetObjectField844C(uVar7,0));
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    goto LAB_00263684;
  case 7:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 8;
    RT_CALLRA(0x263630, guest::PlayBgmSlot5());
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    goto LAB_00263684;
  case 8:
    RT_CALLRA(0x263644, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) << 0x18));
    sVar6 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2)));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 2))) < 0x80) goto LAB_0026365c;
    (*(uint32_t*)rtguest::G((uint32_t)puVar9)) = 0xffffffff;
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4))) = 1;
    break;
  default:
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
    goto LAB_00263684;
  }
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28)));
LAB_00263684:
  if ((uVar3 & 0x10000) == 0) {
    RT_CALLRA(0x263694, guest::raceHudDraw((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x28)))));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4)));
  }
  else {
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x8cb4)));
  }
  if (iVar4 != 0) {
    RT_CALLRA(0x2636a8, guest::SoundStopAllChannels());
    RT_CALLRA(0x2636b0, guest::uiFade(0xffffffff80000000));
    iVar8 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar11 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar8 + 4))) = 0xffffffff;
    RT_CALLRA(0x2636d8, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x002631a0, guest::targetCourseRaceTask)

}  // namespace guest
