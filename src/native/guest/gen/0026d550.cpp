// guest:0x0026d550 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t LoadRaceSceneEntities(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0xf))) == '\0') {
    gs_tasks_spawn(param_1,0x25bcf0,0);
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_1 + 0xc))) = 0x26d5e0;
    gs_tasks_spawn(param_1,0x20dc50,0x370001);
    gs_tasks_spawn(param_1,0x20dc50,0x300001);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0026d550, guest::LoadRaceSceneEntities)

}  // namespace guest
