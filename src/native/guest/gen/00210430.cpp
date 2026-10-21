// guest:0x00210430 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartTownModeTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  undefined8 uVar1;
  
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0x404;
  if (param_3 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0x00080404;
  }
  RT_CALLRA(0x21046c, guest::SelectScene3dHandler(0));
  RT_CALLRA(0x210478, guest::UpdateSceneLightsAndFog(0,(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x22)))));
  if (param_3 == 0) {
    RT_CALLRA(0x210488, guest::ApplySoundVolumes((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x14))),2));
  }
  uVar1 = 2;
  if (param_3 == 0) {
    uVar1 = 0;
  }
  RT_CALLRA(0x210498, guest::SpawnGameViewTasks(param_1,uVar1));
  uVar1 = RT_CALLRA(0x2104ac, guest::gs_tasks_spawn(param_1,0x242fe8,0));
  RT_CALLRA(0x2104b4, guest::SetTaskFlag(uVar1));
  if (param_3 == 0) {
    uVar1 = RT_CALLRA(0x2104dc, guest::gs_tasks_spawn(param_1,0x218458,0x20000));
    RT_CALLRA(0x2104e4, guest::SetTaskFlag(uVar1));
  }
  else {
    uVar1 = RT_CALLRA(0x2104fc, guest::gs_tasks_spawn(param_1,0x218458,(int)(*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) << 10 | 0x800000));
    RT_CALLRA(0x210504, guest::SetTaskFlag(uVar1));
  }
  RT_CALLRA(0x210518, guest::gs_tasks_spawn(param_1,0x20e708,0));
  uVar1 = RT_CALLRA(0x21052c, guest::gs_tasks_spawn(param_1,0x2179d8,1));
  RT_CALLRA(0x210534, guest::SetTaskFlag(uVar1));
  if (param_3 == 0) {
    uVar1 = RT_CALLRA(0x210550, guest::gs_tasks_spawn(param_1,0x2490a0,0));
    RT_CALLRA(0x210558, guest::SetTaskFlag(uVar1));
    uVar1 = RT_CALLRA(0x21056c, guest::gs_tasks_spawn(param_1,0x241ba8,0));
    RT_CALLRA(0x210574, guest::SetTaskFlag(uVar1));
    uVar1 = RT_CALLRA(0x210588, guest::gs_tasks_spawn(param_1,0x259138,0));
    RT_CALLRA(0x210590, guest::SetTaskFlag(uVar1));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0x27))) = 2;
  return 0;
}


RT_GUEST_FN(0x00210430, guest::StartTownModeTask)

}  // namespace guest
