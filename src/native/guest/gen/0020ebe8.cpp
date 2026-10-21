// guest:0x0020ebe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnGameViewTasks(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  undefined8 uVar1;
  
  if (param_2 != 1) {
    uVar1 = RT_CALLRA(0x20ec10, guest::gs_tasks_spawn(param_1,0x241490,0));
    SetTaskFlag(uVar1);
    if (param_2 == 2) {
      uVar1 = RT_CALLRA(0x20ec3c, guest::gs_tasks_spawn(param_1,0x21f098,0));
      SetTaskFlag(uVar1);
    }
    else {
      uVar1 = RT_CALLRA(0x20ec58, guest::gs_tasks_spawn(param_1,0x21e690,0));
      SetTaskFlag(uVar1);
    }
    uVar1 = RT_CALLRA(0x20ec74, guest::gs_tasks_spawn(param_1,0x221300,0));
    SetTaskFlag(uVar1);
    return 0;
  }
  uVar1 = RT_CALLRA(0x20eca4, guest::gs_tasks_spawn(param_1,0x241490,0));
  SetTaskFlag(uVar1);
  uVar1 = RT_CALLRA(0x20ecc0, guest::gs_tasks_spawn(param_1,0x21e690,2));
  SetTaskFlag(uVar1);
  uVar1 = RT_CALLRA(0x20ece0, guest::gs_tasks_spawn(param_1,0x21e690,3));
  SetTaskFlag(uVar1);
  uVar1 = RT_CALLRA(0x20ecf8, guest::gs_tasks_spawn(param_1,0x221300,1));
  SetTaskFlag(uVar1);
  uVar1 = RT_CALLRA(0x20ed10, guest::gs_tasks_spawn(param_1,0x221300,2));
  SetTaskFlag(uVar1);
  return 0;
}


RT_GUEST_FN(0x0020ebe8, guest::SpawnGameViewTasks)

}  // namespace guest
