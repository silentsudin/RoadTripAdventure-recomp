// guest:0x00260638 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t goalAreaTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x30);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x28);
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
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
    __s = (uint32_t)RT_CALLRA(0x260668, guest::getExtraPacketArea(0x8cc0));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))) = __s;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(__s)), 0, 0x8cc0));
    RT_CALLRA(0x260684, guest::ResetMode16((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)))));
    RT_CALLRA(0x260690, guest::gs_music_play_bgm(0xa100201));
    RT_CALLRA(0x2606a4, guest::gs_tasks_spawn(param_1,0x2357c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)))));
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
    RT_CALLRA(0x260728, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x333168)),0x41c40000,0x43c20000,0,0));
    RT_CALLRA(0x260740, guest::StoreIndexedPosition((*(uint32_t*)rtguest::G(0x33316c)),uVar8,0x43c20000,1,0));
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 1;
  case 1:
    if ((((*(float*)rtguest::G(0x177ace0)) < 410.0) || (430.0 < (*(float*)rtguest::G(0x177ace0)))) || ((*(float*)rtguest::G(0x177ace8)) < 370.0)
       ) {
      RT_CALLRA(0x2607b0, guest::GetOrSetPresidentRaceResult(0));
      RT_CALLRA(0x2607c0, guest::SetRoadData5((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),0,1));
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff;
    }
    fVar3 = (*(float*)rtguest::G(0x177af58));
    fVar7 = (*(float*)rtguest::G(0x333170));
    if ((*(float*)rtguest::G(0x333170)) < (*(float*)rtguest::G(0x177ace8))) {
      (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 2;
      (*(uint16_t*)rtguest::G(0x177ade8)) = (*(uint16_t*)rtguest::G(0x177ade8)) | 0x800;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff;
      if (fVar7 < fVar3) {
        RT_CALLRA(0x26083c, guest::GetOrSetPresidentRaceResult(1));
        RT_CALLRA(0x26084c, guest::SetWheelSprite2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),0,1));
        fVar7 = (*(float*)rtguest::G(0x333174));
      }
      else {
        if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb8))) < 0x1e1) {
          RT_CALLRA(0x260870, guest::GetOrSetPresidentRaceResult(3));
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
        }
        else {
          RT_CALLRA(0x260880, guest::GetOrSetPresidentRaceResult(2));
          uVar8 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
        }
        RT_CALLRA(0x260890, guest::SetWheelSprite(uVar8,0,1));
        fVar7 = (*(float*)rtguest::G(0x333174));
      }
    }
    if (fVar7 < (*(float*)rtguest::G(0x177af58))) {
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
    RT_CALLRA(0x2608e8, guest::InitHudConfig2((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28))),uVar8));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x28)));
    break;
  case 2:
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffffeff | 8;
    (*(uint32_t*)rtguest::G((uint32_t)puVar6)) = 3;
    break;
  case 3:
    RT_CALLRA(0x26091c, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) << 0x18));
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
  RT_CALLRA(0x260960, guest::raceHudDraw(iVar4));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x8cb4))) != 0) {
    RT_CALLRA(0x260974, guest::SoundStopAllChannels());
    RT_CALLRA(0x26097c, guest::uiFade(0xffffffff80000000));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 8;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
    RT_CALLRA(0x2609a4, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x00260638, guest::goalAreaTask)

}  // namespace guest
