// guest:0x00208270 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t fadeOutTask(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8)));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x1c))) == 0) {
    if (iVar1 < 0x40) {
      uiFade((0x40 - iVar1) * 0x2000000);
      return 0;
    }
    gs_tasks_kill();
    return 0;
  }
  if (iVar1 < 0x20) {
    uiFade((0x20 - iVar1) * 0x4000000);
    return 0;
  }
  gs_tasks_kill();
  return 0;
}


RT_GUEST_FN(0x00208270, guest::fadeOutTask)

}  // namespace guest
