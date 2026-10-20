// guest:0x0025be30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t textEntryExitFade(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  int iVar1;
  
  iVar1 = (int)param_1;
  RT_CALLRA(0x25be4c, guest::uiSetTexa((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) * -8 + 0x80));
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == 0x10) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 1;
    RT_CALLRA(0x25be70, guest::gs_tasks_kill(param_1));
  }
  RT_CALLRA(0x25be78, guest::drawTextEntryGrid(param_1));
  drawTextEntrySlots(param_1);
  return 0;
}


RT_GUEST_FN(0x0025be30, guest::textEntryExitFade)

}  // namespace guest
