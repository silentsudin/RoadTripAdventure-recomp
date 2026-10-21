// guest:0x0025fbf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t halfPlaneCourseTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
uint32_t __s;
  long lVar4;
  int iVar5;
  int iVar6;
uint32_t puVar7;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    __s = (uint32_t)RT_CALLRA(0x25fc20, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x25fc3c, guest::ResetMode15((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)))));
    RT_CALLRA(0x25fc48, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x25fc5c, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)))));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))) + 0x8cb0))) = 0;
    return 0;
  }
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  puVar7 = (uint32_t)(iVar1 + 0x8cb0);
  iVar5 = iVar1;
  switch((*(uint32_t*)rtguest::G((uint32_t)puVar7))) {
  case 0:
    (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 1;
  case 1:
    if (0.0 <= ((*(float*)rtguest::G(0x2d9944)) - (*(float*)rtguest::G(0x2d993c))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2d9940))) -
               ((*(float*)rtguest::G(0x2d9940)) - (*(float*)rtguest::G(0x2d9938))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2d9944)))) {
      if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) < 0x1c21) && (lVar4 = TestStateBitFc78(0x39), lVar4 != 0)) {
        RT_CALLRA(0x25fd38, guest::GetOrSetPresidentRaceResult(2));
      }
      else {
        RT_CALLRA(0x25fd48, guest::GetOrSetPresidentRaceResult(1));
      }
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) = 6;
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      RT_CALLRA(0x25fd7c, guest::SetRoadData3((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))),0,1));
    }
    if (0.0 <= ((*(float*)rtguest::G(0x2d9934)) - (*(float*)rtguest::G(0x2d992c))) * ((*(float*)rtguest::G(0x177ace0)) - (*(float*)rtguest::G(0x2d9930))) -
               ((*(float*)rtguest::G(0x2d9930)) - (*(float*)rtguest::G(0x2d9928))) * ((*(float*)rtguest::G(0x177ace8)) - (*(float*)rtguest::G(0x2d9934)))) {
      RT_CALLRA(0x25fdd8, guest::GetOrSetPresidentRaceResult(0));
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    else {
      uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    if ((uVar2 & 4) == 0) {
LAB_0025fe1c:
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc)));
    }
    else {
      if ((uVar2 & 0x8000) == 0) {
        (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc))) + 1;
        goto LAB_0025fe1c;
      }
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cbc)));
    }
    RT_CALLRA(0x25fe20, guest::InitHudConfig2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28))),uVar3));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 3;
    break;
  case 3:
    RT_CALLRA(0x25fe54, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) << 0x18));
    if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) < 0x80) {
      (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 2))) + 1;
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
    }
    else {
      iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
      (*(uint32_t*)rtguest::G((uint32_t)puVar7)) = 0xffffffff;
    }
    break;
  case 0xffffffff:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) = 1;
  }
  RT_CALLRA(0x25fe98, guest::raceHudDraw(iVar5));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) != 0) {
    RT_CALLRA(0x25feac, guest::SoundStopAllChannels());
    RT_CALLRA(0x25feb4, guest::uiFade(0xffffffff80000000));
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0xffffffff;
    RT_CALLRA(0x25fedc, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x0025fbf8, guest::halfPlaneCourseTask)

}  // namespace guest
