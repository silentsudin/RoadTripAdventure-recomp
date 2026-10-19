// guest:0x002083c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AdvanceTimedTask(uint32_t param_1, ...) {
  int iVar1;
  int iVar2;
  
  iVar2 = (int)param_1;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
  if (iVar1 == 0) {
    PlayUiSound((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x1c))));
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 8)));
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x2c))) = (*(uint8_t*)rtguest::G(0x208428));
  }
  if (iVar1 != 0x40) {
    return 0;
  }
  gs_tasks_kill(param_1);
}


RT_GUEST_FN(0x002083c0, guest::AdvanceTimedTask)

}  // namespace guest
