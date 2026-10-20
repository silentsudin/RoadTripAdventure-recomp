// guest:0x0025be30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEntryExitFade(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  
  iVar1 = (int)param_1;
  uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) * -8 + 0x80);
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x10) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
    gs_tasks_kill(param_1);
  }
  drawTextEntryGrid(param_1);
  drawTextEntrySlots(param_1);
  return 0;
}


RT_GUEST_FN(0x0025be30, guest::textEntryExitFade)

}  // namespace guest
