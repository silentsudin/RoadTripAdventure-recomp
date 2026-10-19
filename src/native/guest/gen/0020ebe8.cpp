// guest:0x0020ebe8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnGameViewTasks(uint32_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  
  if (param_2 != 1) {
    uVar1 = gs_tasks_spawn(param_1,0x241490,0);
    SetTaskFlag(uVar1);
    if (param_2 == 2) {
      uVar1 = gs_tasks_spawn(param_1,0x21f098,0);
      SetTaskFlag(uVar1);
    }
    else {
      uVar1 = gs_tasks_spawn(param_1,0x21e690,0);
      SetTaskFlag(uVar1);
    }
    uVar1 = gs_tasks_spawn(param_1,0x221300,0);
    SetTaskFlag(uVar1);
    return 0;
  }
  uVar1 = gs_tasks_spawn(param_1,0x241490,0);
  SetTaskFlag(uVar1);
  uVar1 = gs_tasks_spawn(param_1,0x21e690,2);
  SetTaskFlag(uVar1);
  uVar1 = gs_tasks_spawn(param_1,0x21e690,3);
  SetTaskFlag(uVar1);
  uVar1 = gs_tasks_spawn(param_1,0x221300,1);
  SetTaskFlag(uVar1);
  uVar1 = gs_tasks_spawn(param_1,0x221300,2);
  SetTaskFlag(uVar1);
  return 0;
}


RT_GUEST_FN(0x0020ebe8, guest::SpawnGameViewTasks)

}  // namespace guest
