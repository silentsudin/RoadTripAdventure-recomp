// guest:0x0022c5f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t FinishTaskOnInputIdle(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
    gs_tasks_kill();
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0022c5f0, guest::FinishTaskOnInputIdle)

}  // namespace guest
