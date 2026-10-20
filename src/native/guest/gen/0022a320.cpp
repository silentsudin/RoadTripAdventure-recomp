// guest:0x0022a320 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RunCardOpSequenceTask(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
  }
  char cVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = (int)param_1;
  if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 8))) == 0) {
    gs_tasks_spawn(param_1,0x2282c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))));
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  else {
    cVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24)));
  }
  if (cVar1 == '\0') {
    return 0;
  }
  if ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) != '\0') {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    if ((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) == 1) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 1;
    }
    else {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
      (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = 2;
    }
    gs_tasks_kill(param_1);
    return 0;
  }
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4)));
  if (iVar2 != 2) {
    iVar3 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x18)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x24))) = 1;
    (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 4))) = iVar2;
    gs_tasks_kill(param_1);
    return 0;
  }
  (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x25))) = 1;
  gs_tasks_spawn(param_1,0x228510,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar3 + 0x1c))));
  return 0;
}


RT_GUEST_FN(0x0022a320, guest::RunCardOpSequenceTask)

}  // namespace guest
