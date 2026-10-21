// guest:0x00254d20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t CarShopVisitSceneTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x28, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x38) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x3c) = 0x00000000u;
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
  }
uint32_t __s;
  int iVar1;
  int iVar2;
  uint uVar3;
uint32_t puVar4;
uint32_t piVar5;
  int iVar6;
  int iVar7;
  
  iVar7 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x254d54, guest::getExtraPacketArea(0x8cd0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cd0));
  }
  iVar6 = (int)param_2;
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x20)));
  puVar4 = (uint32_t)(iVar2 + 0x8cb0);
  if ((uVar3 & 0x8000) != 0) {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) == 0) {
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 1;
      RT_CALLRA(0x254da8, guest::SetBaseValue(iVar2,1));
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    }
    goto LAB_00254fe0;
  }
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cbc))) = 0;
    RT_CALLRA(0x254dc8, guest::SetBaseValue(iVar2,0));
  }
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar4))) {
  case 0:
    RT_CALLRA(0x254dfc, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x254e04, guest::ResetMode1B(iVar2));
    RT_CALLRA(0x254e18, guest::gs_tasks_spawn(param_1,0x2357c0,iVar2));
    RT_CALLRA(0x254e24, guest::InitCameraTransforms(param_2,0x177ac50));
    RT_CALLRA(0x254e40, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x3330a0)),0x43c80000,(*(uint32_t*)rtguest::G(0x3330a4)),0,0x4000));
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 1;
  case 1:
    if ((*(float*)rtguest::G(0x177ace4)) < (*(float*)rtguest::G(0x3330a8))) {
      RT_CALLRA(0x254e70, guest::SetRoadData4(iVar2,0,1));
      RT_CALLRA(0x254e78, guest::GetOrSetPresidentRaceResult(1));
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 2;
    }
    else if (((((*(float*)rtguest::G(0x177ace4)) < 350.0) &&
              (iVar1 = ::memcmp((char *)rtguest::G((uint32_t)((uint32_t)(iVar2 + 0x8cc0))), (char *)rtguest::G((uint32_t)(0x177ace0)), 0x10), iVar1 == 0)) &&
             (CONCAT44((*(uint32_t*)rtguest::G(0x177ad44)),(*(uint32_t*)rtguest::G(0x177ad40))) == 0)) && ((*(uint32_t*)rtguest::G(0x177ad48)) == 0)) {
      RT_CALLRA(0x254ee8, guest::SetRoadData(iVar2,0,1));
      RT_CALLRA(0x254ef0, guest::GetOrSetPresidentRaceResult(2));
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 2;
    }
    RT_CALLRA(0x254f10, guest::vu0CopyVector(iVar2 + 0x8cc0,0x177ace0));
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    goto LAB_00254fe0;
  case 2:
    (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) & 0xfffffeff;
    (*(uint32_t*)rtguest::G(0x177ad48)) = 0;
    (*(uint32_t*)rtguest::G(0x177ad40)) = 0;
    (*(uint32_t*)rtguest::G(0x177ad44)) = 0;
    (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 3;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb4))) = 0;
switchD_00254df0_caseD_3:
    piVar5 = (uint32_t)(iVar2 + 0x8cb4);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
    (*(int32_t*)rtguest::G((uint32_t)piVar5)) = iVar1 + 1;
    if (0x7f < iVar1) {
      RT_CALLRA(0x254f70, guest::PlayBgmSlot5());
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 0x10000;
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = 0;
      (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 4;
LAB_00254f9c:
      piVar5 = (uint32_t)(iVar2 + 0x8cb4);
      RT_CALLRA(0x254fa0, guest::uiFade((*(int32_t*)rtguest::G((uint32_t)piVar5)) << 0x19));
      iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar5));
      (*(int32_t*)rtguest::G((uint32_t)piVar5)) = iVar1 + 1;
      if (0x3f < iVar1) {
        (*(uint32_t*)rtguest::G((uint32_t)puVar4)) = 0xffffffff;
switchD_00254df0_caseD_ffffffff:
        RT_CALLRA(0x254fc8, guest::uiFade(0xffffffff80000000));
        (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8))) = 1;
      }
    }
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
LAB_00254fe0:
    if ((uVar3 & 0x10000) == 0) {
      RT_CALLRA(0x254fec, guest::raceHudDraw(iVar2));
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8)));
    }
    else {
      iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x8cb8)));
    }
    if (iVar2 != 0) {
      iVar7 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x18)));
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) | 8;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar7 + 4))) = 0xffffffff;
      gs_tasks_kill(param_1);
      return 0;
    }
    return 0;
  case 3:
    goto switchD_00254df0_caseD_3;
  case 4:
    goto LAB_00254f9c;
  default:
    goto switchD_00254df0_caseD_ffffffff;
  }
}


RT_GUEST_FN(0x00254d20, guest::CarShopVisitSceneTask)

}  // namespace guest
