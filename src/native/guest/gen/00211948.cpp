// guest:0x00211948 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FadeOutMenuTask(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = (int)param_1;
  uiFade((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) << 0x19);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x40) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x00211948, guest::FadeOutMenuTask)

}  // namespace guest
