// guest:0x002082d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fadeInTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  iVar1 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1c))) == 0) {
    RT_CALLRA(0x2082f4, guest::uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) << 0x19));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x40) {
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  else {
    RT_CALLRA(0x208320, guest::uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) << 0x1a));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x20) {
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002082d8, guest::fadeInTask)

}  // namespace guest
