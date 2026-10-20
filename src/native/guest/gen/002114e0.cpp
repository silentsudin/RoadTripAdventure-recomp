// guest:0x002114e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t UpdateTimeOfDayCounter(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
uint32_t piVar2;
  
  piVar2 = (uint32_t)(param_1 + 0x14);
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  (*(int32_t*)rtguest::G((uint32_t)piVar2)) = iVar1 + 1;
  if (0x34bc0 < iVar1) {
    (*(int32_t*)rtguest::G((uint32_t)piVar2)) = iVar1 + -0x34bbf;
  }
  if (((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(param_1 + 0x28))) & 0x1000) != 0) {
    (*(int32_t*)rtguest::G(0x335730)) = (*(int32_t*)rtguest::G(0x335730)) + 1;
  }
  return 0;
}


RT_GUEST_FN(0x002114e0, guest::UpdateTimeOfDayCounter)

}  // namespace guest
