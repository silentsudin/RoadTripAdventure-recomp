// guest:0x0021c6a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t DecomposeWorldPosition(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (*(int32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(int32_t)));
  (*(uint32_t*)rtguest::G((uint32_t)param_2)) = uVar3 & 0xe000000;
  iVar1 = (int)(uVar3 & 0x2000000) >> 1;
  (*(int32_t*)rtguest::G((uint32_t)param_1)) = ((*(int32_t*)rtguest::G((uint32_t)param_3)) - iVar1 & 0xe000000U) + iVar1;
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(2) * sizeof(int32_t)));
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_3));
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0x1ffffff;
  }
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xffffff;
  }
  uVar3 = iVar1 >> 0x19 & 1;
  return (iVar1 >> 0x19 & 6U) * 8 + ((iVar2 >> 0x18) - uVar3 & 0xe) + uVar3;
}


RT_GUEST_FN(0x0021c6a0, guest::DecomposeWorldPosition)

}  // namespace guest
