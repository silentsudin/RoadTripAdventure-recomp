// guest:0x002174e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateOptionsMenu(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::gS[0] = 0x0000000001727b90ull;  // walk const
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 8))) == 0) {
    RT_CALLRA(0x217510, guest::InitPauseMenuOptions(param_1,param_2,0x1727b90));
    (*(int32_t*)rtguest::G(0x1727b94)) = 5;
  }
  if ((*(uint32_t*)rtguest::G(0x1727b90)) != (*(int32_t*)rtguest::G(0x1727b94))) {
    (*(uint32_t*)rtguest::G(0x1727b90)) = (*(int32_t*)rtguest::G(0x1727b94));
    (*(uint32_t*)rtguest::G(0x1727b98)) = 0;
    (uint32_t)(uintptr_t)(::memset((char *)rtguest::G((uint32_t)(0x0172fd34)), 0, 500));
  }
  RT_CALLRA(0x217558, guest::gs_options_settings_page(param_1,param_2,0x1727b90,0x172fcc0));
  if ((*(int32_t*)rtguest::G(0x1727b94)) != 5) {
    (*(uint32_t*)rtguest::G(0x1727b9c)) = 1;
  }
  (*(uint32_t*)rtguest::G(0x1727b98)) = (*(uint32_t*)rtguest::G(0x1727b98)) + 1;
  if ((*(uint32_t*)rtguest::G(0x1727b9c)) != 0) {
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0xffffffff;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x002174e0, guest::UpdateOptionsMenu)

}  // namespace guest
