// guest:0x00208f50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t gs_options_sub_tasks_speaker(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  undefined8 uVar2;
  long lVar3;
uint32_t pbVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
    uVar2 = RT_CALLRA(0x208f74, guest::uiMenuCreate(param_1) /* $a0 = entry $a0 at the jal (MIPS: no $4 write/jal before 0x208f74); pseudocode dropped the live arg */);
    RT_CALLRA(0x208f8c, guest::uiMenuAddItem(uVar2,0x3332d0,0x10));
    RT_CALLRA(0x208fa0, guest::uiMenuAddItem(uVar2,0x3332d8,0x10));
    RT_CALLRA(0x208fac, guest::CenterRectangle(uVar2,8));
    iVar5 = (int)uVar2;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 1;
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xc))) = 2;
    cVar1 = RT_CALLRA(0x208fc0, guest::SoundRpcGetStatus());
    (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))) = cVar1;
    pbVar4 = (uint32_t)(iVar5 + cVar1 + 0x50);
    (*(int8_t*)rtguest::G((uint32_t)pbVar4)) = (*(int8_t*)rtguest::G((uint32_t)pbVar4)) | 2;
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 8))) == 0) {
      return 0;
    }
  }
  iVar5 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x28)));
  lVar3 = (int32_t)RT_CALLRA(0x208ffc, guest::uiMenuRun(iVar5,param_2));
  if (lVar3 != 0) {
    if (lVar3 == -2) {
      gs_tasks_kill(param_1);
      return 0;
    }
    if (lVar3 == -1) {
      RT_CALLRA(0x209060, guest::PlayUiSound(0x1b));
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x50))) & 0xfd;
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0x51))) & 0xfd;
      RT_CALLRA(0x209058, guest::gs_options_speaker_set((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd)))));
      RT_CALLRA(0x209088, guest::PlayUiSound(0x1f));
      pbVar4 = (uint32_t)(iVar5 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xd))) + 0x50);
      (*(int8_t*)rtguest::G((uint32_t)pbVar4)) = (*(int8_t*)rtguest::G((uint32_t)pbVar4)) | 2;
    }
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar5 + 0xe))) = 2;
    iVar6 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar6 + 4))) = 0;
  }
  return 0;
}


RT_GUEST_FN(0x00208f50, guest::gs_options_sub_tasks_speaker)

}  // namespace guest
