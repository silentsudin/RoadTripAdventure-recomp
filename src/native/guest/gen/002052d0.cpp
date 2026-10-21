// guest:0x002052d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartMenuFlow(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_2);  // walk: = param_2
  }
  undefined8 uVar1;
  
  RT_CALLRA(0x2052f0, guest::gs_tasks_spawn(param_1,0x20df98,0));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x205088; // &UpdateMenuFlowTask: sw of the code address, not a data read
  RT_CALLRA(0x205310, guest::gs_tasks_spawn(param_1,0x20dc50,0x400000));
  uVar1 = RT_CALLRA(0x205324, guest::gs_tasks_spawn(param_1,0x20a2b0,0));
  RT_CALLRA(0x20532c, guest::SetTaskFlag(uVar1));
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x28))) = 0;
  SoundInitVolumes();
  return 0;
}


RT_GUEST_FN(0x002052d0, guest::StartMenuFlow)

}  // namespace guest
