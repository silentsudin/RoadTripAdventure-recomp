// guest:0x0022ee00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t KillTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  iVar1 = (int)param_1;
  RT_CALLRA(0x22ee14, guest::uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) << 0x18));
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) < 0x80) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) + 1;
  }
  else {
    RT_CALLRA(0x22ee40, guest::gs_tasks_kill(param_1));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0xffffffff;
  }
  return 0;
}


RT_GUEST_FN(0x0022ee00, guest::KillTask)

}  // namespace guest
