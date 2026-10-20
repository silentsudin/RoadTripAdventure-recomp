// guest:0x002082d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fadeInTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x1c))) == 0) {
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) << 0x19);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x40) {
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  else {
    uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) << 0x1a);
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x20) {
      gs_tasks_kill(param_1);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002082d8, guest::fadeInTask)

}  // namespace guest
