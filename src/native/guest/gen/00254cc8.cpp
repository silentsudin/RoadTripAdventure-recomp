// guest:0x00254cc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetTuningParamFromCarSlot(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar3 = 0;
  bVar1 = (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 0x24a)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)((int)param_2 + 600)));
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 4) != 0) {
    UpdateAiCourseSteering(param_2);
    uVar3 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar2 + (uint32_t)bVar1 * 0xc + 0x74)));
  }
  return uVar3;
}


RT_GUEST_FN(0x00254cc8, guest::GetTuningParamFromCarSlot)

}  // namespace guest
