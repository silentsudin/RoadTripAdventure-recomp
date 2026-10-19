// guest:0x002052d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartMenuFlow(uint32_t param_1, uint32_t param_2, ...) {
  undefined8 uVar1;
  
  gs_tasks_spawn(param_1,0x20df98,0);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x205088; // &UpdateMenuFlowTask: sw of the code address, not a data read
  gs_tasks_spawn(param_1,0x20dc50,0x400000);
  uVar1 = gs_tasks_spawn(param_1,0x20a2b0,0);
  SetTaskFlag(uVar1);
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0;
  SoundInitVolumes();
  return 0;
}


RT_GUEST_FN(0x002052d0, guest::StartMenuFlow)

}  // namespace guest
