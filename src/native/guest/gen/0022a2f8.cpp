// guest:0x0022a2f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CompleteTaskSuccess(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
  gs_tasks_kill(param_1); // arg-drop fix: j 0x204de8 @0x22a314 keeps $a0=node (capstone)
  return 0;
}


RT_GUEST_FN(0x0022a2f8, guest::CompleteTaskSuccess)

}  // namespace guest
