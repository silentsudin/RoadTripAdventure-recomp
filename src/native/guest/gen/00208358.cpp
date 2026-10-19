// guest:0x00208358 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t flashTask(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)));
  uiFade(iVar1 << 0x1a | iVar1 << 3 | iVar1 << 0xb | iVar1 << 0x13);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8))) == 0x20) {
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00208358, guest::flashTask)

}  // namespace guest
