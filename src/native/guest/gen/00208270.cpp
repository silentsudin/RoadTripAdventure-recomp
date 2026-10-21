// guest:0x00208270 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fadeOutTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) == 0) {
    if (iVar1 < 0x40) {
      uiFade((int)((uint32_t)(0x40 - iVar1) * 0x2000000u));  // pseudocode 0x208270: mul wraps mod 2^32 on the guest
      return 0;
    }
    gs_tasks_kill(param_1); // arg-drop fix: j 0x204de8 @0x2082a8/0x2082d0 keeps $a0=node (capstone)
    return 0;
  }
  if (iVar1 < 0x20) {
    uiFade((int)((uint32_t)(0x20 - iVar1) * 0x4000000u));  // pseudocode 0x208270: mul wraps mod 2^32 on the guest
    return 0;
  }
  gs_tasks_kill(param_1); // arg-drop fix: j 0x204de8 @0x2082a8/0x2082d0 keeps $a0=node (capstone)
  return 0;
}


RT_GUEST_FN(0x00208270, guest::fadeOutTask)

}  // namespace guest
