// guest:0x0022c4a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunFadeHoldTask(uint32_t param_1, uint32_t param_2, ...) {
  if ((((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) == 0) || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) != '\0')) &&
     ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 8))) != 0x40)) {
    uiFade(0x8000000);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) | 0x400000;
    return 0;
  }
  gs_tasks_kill();
}


RT_GUEST_FN(0x0022c4a8, guest::RunFadeHoldTask)

}  // namespace guest
