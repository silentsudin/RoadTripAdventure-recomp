// guest:0x00208358 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t flashTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)));
  RT_CALLRA(0x208384, guest::uiFade(iVar1 << 0x1a | iVar1 << 3 | iVar1 << 0xb | iVar1 << 0x13));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8))) == 0x20) {
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00208358, guest::flashTask)

}  // namespace guest
