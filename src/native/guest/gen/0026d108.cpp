// guest:0x0026d108 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t HouseEntryInitTask(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x26d12c, guest::MatchSlotPairState(param_3,param_4));
  (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar1 * 0x1c;
  if (lVar1 == 0) {
    (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (int)param_3 * 4))) + (int)param_4 * 0x10;
  }
  gs_tasks_spawn(param_1,0x20dc50,0x70301);
  gs_tasks_spawn(param_1,0x22c518,1);
  return 0;
}


RT_GUEST_FN(0x0026d108, guest::HouseEntryInitTask)

}  // namespace guest
