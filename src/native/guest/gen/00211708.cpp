// guest:0x00211708 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SpawnTownDialogueTasks(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
  }
  long lVar1;
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) == '\0') {
    gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2)) | 0x30000);
    gs_tasks_spawn(param_1,0x22d1e0,0);
    return 0;
  }
  lVar1 = RT_CALLRA(0x21172c, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21)))));
  (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar1 * 0x1c;
  if (lVar1 == 0) {
    (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) * 4))) +
                   (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) * 0x10;
  }
  gs_tasks_spawn(param_1,0x20dc50,0x30000);
  gs_tasks_spawn(param_1,0x22c518,0);
  return 0;
}


RT_GUEST_FN(0x00211708, guest::SpawnTownDialogueTasks)

}  // namespace guest
