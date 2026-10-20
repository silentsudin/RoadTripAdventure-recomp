// guest:0x0024f728 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiArenaDataTag(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  int iVar1;
  int iVar2;
  
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  if (((*(int32_t*)rtguest::G((uint32_t)param_1)) - iVar1) / 0x60 < 1) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
  }
  else {
    if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar1 + 0x10) {
      (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
      iVar1 = 0;
    }
    else {
      (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar1 + 0x10;
      iVar1 = (int)param_1 + iVar1 + 0x10;
    }
    if (iVar1 != 0) {
      iVar2 = ((*(int32_t*)rtguest::G((uint32_t)param_1)) - (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)))) / 0x50;
      if (param_2 <= iVar2) {
        iVar2 = param_2;
      }
      gifSetSpriteCount(iVar1,iVar2);
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x0024f728, guest::uiArenaDataTag)

}  // namespace guest
