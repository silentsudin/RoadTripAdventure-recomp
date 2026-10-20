// guest:0x00210430 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartTownModeTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  undefined8 uVar1;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0x404;
  if (param_3 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0x00080404;
  }
  SelectScene3dHandler(0);
  UpdateSceneLightsAndFog(0,(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22))));
  if (param_3 == 0) {
    ApplySoundVolumes((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))),2);
  }
  uVar1 = 2;
  if (param_3 == 0) {
    uVar1 = 0;
  }
  SpawnGameViewTasks(param_1,uVar1);
  uVar1 = gs_tasks_spawn(param_1,0x242fe8,0);
  SetTaskFlag(uVar1);
  if (param_3 == 0) {
    uVar1 = gs_tasks_spawn(param_1,0x218458,0x20000);
    SetTaskFlag(uVar1);
  }
  else {
    uVar1 = gs_tasks_spawn(param_1,0x218458,(int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) << 10 | 0x800000);
    SetTaskFlag(uVar1);
  }
  gs_tasks_spawn(param_1,0x20e708,0);
  uVar1 = gs_tasks_spawn(param_1,0x2179d8,1);
  SetTaskFlag(uVar1);
  if (param_3 == 0) {
    uVar1 = gs_tasks_spawn(param_1,0x2490a0,0);
    SetTaskFlag(uVar1);
    uVar1 = gs_tasks_spawn(param_1,0x241ba8,0);
    SetTaskFlag(uVar1);
    uVar1 = gs_tasks_spawn(param_1,0x259138,0);
    SetTaskFlag(uVar1);
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x27))) = 2;
  return 0;
}


RT_GUEST_FN(0x00210430, guest::StartTownModeTask)

}  // namespace guest
