// guest:0x00211808 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartTownExplorationTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x18) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x1c) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
  }
  int iVar1;
  long lVar2;
  int iVar3;
  
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) == '\0') {
    RT_CALLRA(0x2118a4, guest::gs_tasks_spawn(param_1,0x20dc50,(uint32_t)(*(uint32_t*)rtguest::G(0x177fdb2)) | 0x30000));
    RT_CALLRA(0x2118b8, guest::gs_tasks_spawn(param_1,0x22d1e0,0));
  }
  else {
    lVar2 = RT_CALLRA(0x211830, guest::MatchSlotPairState((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21)))));
    (*(int32_t*)rtguest::G(0x177ea5c)) = (int)0x177f760 + (int)lVar2 * 0x1c;
    if (lVar2 == 0) {
      (*(int32_t*)rtguest::G(0x177ea5c)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(0x2c22c8 + (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x23))) * 4))) +
                     (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(param_2 + 0x21))) * 0x10;
    }
    RT_CALLRA(0x2118e0, guest::gs_tasks_spawn(param_1,0x20dc50,0x30000));
    RT_CALLRA(0x2118f4, guest::gs_tasks_spawn(param_1,0x22c518,0));
  }
  iVar1 = (*(uint32_t*)rtguest::G(0x177fdb8)) / 6000;
  iVar3 = (int)param_1;
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 4;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x2105b8;
  (*(int16_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 2))) = (short)iVar1;
  return 0;
}


RT_GUEST_FN(0x00211808, guest::StartTownExplorationTask)

}  // namespace guest
