// guest:0x00271918 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void textGridScrollUp(uint64_t param_1, uint32_t param_2) {
  rtguest::FrameGuard rt_fg(80);
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  
  uVar1 = (*(uint32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * 4));
  uVar2 = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)param_1));
  if (param_2 < uVar1) {
    iVar3 = uVar1 - param_2;
    textGridMoveRect(param_1,0,0,0,param_2,uVar2,iVar3);
    textGridFillRectBlank(param_1,0,iVar3,uVar2,param_2);
    return;
  }
  textGridFillRectBlank(param_1,0,0,uVar2);
  return;
}


RT_GUEST_FN(0x00271918, guest::textGridScrollUp)

}  // namespace guest
