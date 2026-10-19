// guest:0x0023bb00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ShowMessageAndWait(uint32_t param_1, ...) {
  undefined8 uVar1;
  long lVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    uVar1 = uiMenuCreate();
    uiMenuAddItem(uVar1,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))),0);
    PlayUiSound(0x1e);
    return 0;
  }
  lVar2 = uiMenuRun((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x28))));
  if (lVar2 != 0) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = 0;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0023bb00, guest::ShowMessageAndWait)

}  // namespace guest
