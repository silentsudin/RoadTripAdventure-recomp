// guest:0x0022eac8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunFadeOutExitTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
    rtguest::gS[2] = 0x000000000177ac50ull;  // walk const
  }
  int iVar1;
  
  iVar1 = (int)param_1;
  uiFade((int)(*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) << 0x19);
  if ((*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) < 0x40) {
    (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) = (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 2))) + 1;
    return 0;
  }
  (*(uint8_t*)rtguest::G(0x177ae97)) = 0xff;
  (*(uint16_t*)rtguest::G(0x177ade8)) = 0;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x18)));
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 4))) = 0xffffffff;
  RT_CALLRA(0x22eb4c, guest::gs_tasks_kill(param_1));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x50))) = 0x80000000;
  uiFade(0xffffffff80000000);
  return 0;
}


RT_GUEST_FN(0x0022eac8, guest::RunFadeOutExitTask)

}  // namespace guest
