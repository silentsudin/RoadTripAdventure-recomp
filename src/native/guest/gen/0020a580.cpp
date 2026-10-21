// guest:0x0020a580 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateMenuScreenTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  if (iVar2 == 0) {
    RT_CALLRA(0x20a5a8, guest::gs_tasks_spawn(param_1,0x228d10,0));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8)));
  }
  if (iVar2 < 0x20) {
    RT_CALLRA(0x20a5c0, guest::uiFade(iVar2 << 0x19));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  else {
    RT_CALLRA(0x20a5d0, guest::uiFade(0x40000000));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  if (cVar1 == '\0') {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) == '\0') {
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) == 1) {
      RT_CALLRA(0x20a60c, guest::gs_tasks_spawn(param_1,0x269f48,0));
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
      return 0;
    }
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
  }
  else {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
  (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
  gs_tasks_kill(param_1);
  return 0;
}


RT_GUEST_FN(0x0020a580, guest::UpdateMenuScreenTask)

}  // namespace guest
