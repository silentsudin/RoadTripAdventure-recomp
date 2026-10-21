// guest:0x0022c418 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunFadeOutWaitTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  RT_CALLRA(0x22c430, guest::uiFade(0x8000000));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
  if ((0x3f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)))) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
    RT_CALLRA(0x22c464, guest::PlayBgmSlot2());
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 5;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0022c418, guest::RunFadeOutWaitTask)

}  // namespace guest
