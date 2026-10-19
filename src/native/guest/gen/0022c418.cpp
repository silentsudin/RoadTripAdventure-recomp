// guest:0x0022c418 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunFadeOutWaitTask(uint32_t param_1, uint32_t param_2, ...) {
  int iVar1;
  
  uiFade(0x8000000);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
  if ((0x3f < (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8)))) && ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0')) {
    PlayBgmSlot2();
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
