// guest:0x0022dae8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCountdownTimerTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  short sVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = (int)param_1;
  sVar1 = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2)));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x1c)));
  if (sVar1 < 0x40) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0xfffeffff;
    RT_CALLRA(0x22db30, guest::uiFade((0x40 - sVar1) * 0x2000000));
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 2))) + 1;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) & 0x4000) != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) = 0;
    return 0;
  }
  iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8)));
  if (iVar4 == 0x3c) {
    RT_CALLRA(0x22db6c, guest::PlayUiSound(0x2d));
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8)));
  }
  else {
    if (iVar4 == 0xf0) {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 4;
    }
    else {
      if (iVar4 != 300) goto LAB_0022dbb4;
      RT_CALLRA(0x22db98, guest::PlayBgmSlot1());
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x100;
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar3;
    iVar4 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8)));
  }
LAB_0022dbb4:
  if (iVar4 < 300) {
    RT_CALLRA(0x22dbd0, guest::SetVehicleMode(uVar2,iVar4 / 0x3c + 2));
  }
  else {
    RT_CALLRA(0x22dbe0, guest::SetVehicleMode(uVar2,6));
    if ((*(int8_t*)rtguest::G(0x177adea)) == '\0') {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
      if (((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 8))) != 0x168) && ((uVar3 & 0x8000) == 0)) {
        return 0;
      }
    }
    else {
      uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28)));
    }
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = uVar3 | 0x114;
    RT_CALLRA(0x22dc2c, guest::SetVehicleMode(uVar2,0));
    iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 4))) = 0xffffffff;
    RT_CALLRA(0x22dc48, guest::gs_tasks_kill(param_1));
  }
  return 0;
}


RT_GUEST_FN(0x0022dae8, guest::RunCountdownTimerTask)

}  // namespace guest
