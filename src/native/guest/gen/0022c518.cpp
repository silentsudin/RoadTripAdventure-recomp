// guest:0x0022c518 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartCourseEventTask(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    return 0;
  }
  iVar2 = (int)param_2;
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0xf))) != '\0') {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x28))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x28))) | 0x400000;
    uiFade(0x10000000);
    return 0;
  }
  lVar1 = RT_CALLRA(0x22c54c, guest::CheckUnlockCondition((*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x23))),(*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x21))),
                     (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x14)))));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))) | 2;
  }
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x2c))) = 0x22c630;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0xc))) = 0x22aeb0;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) = 0;
  shopScene(param_1,param_2);
  gs_tasks_spawn(param_1,0x208270,0);
  return 0;
}


RT_GUEST_FN(0x0022c518, guest::StartCourseEventTask)

}  // namespace guest
