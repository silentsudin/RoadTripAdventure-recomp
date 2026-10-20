// guest:0x0023be88 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t dialogueCmdPlaySoundCue(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  iVar4 = (int)param_1;
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8)));
  if (iVar2 == 0) {
    uVar3 = (uint32_t)(*(int8_t*)rtguest::G((uint32_t)(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c)))));
    if (uVar3 == 0x10) {
      uVar3 = (uint32_t)(*(uint8_t*)rtguest::G((uint32_t)0x0177f778 + 1));
    }
    gs_tasks_spawn(param_1,0x2083c0,(*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(0x002f00ac + uVar3 * 0x10))));
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 8)));
  }
  if (iVar2 == 0x40) {
    iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x18)));
    bVar1 = (*(int8_t*)rtguest::G((uint32_t)(uint32_t)((*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar4 + 0x1c))) + 1)));
    (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 0x24))) = 1;
    (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + 4))) = (uint32_t)bVar1;
    gs_tasks_kill(param_1);
    return 0;
  }
  return 0;
}


RT_GUEST_FN(0x0023be88, guest::dialogueCmdPlaySoundCue)

}  // namespace guest
