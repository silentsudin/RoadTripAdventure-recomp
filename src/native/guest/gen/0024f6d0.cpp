// guest:0x0024f6d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

void uiArenaSetColour(uint32_t param_1, uint64_t param_2) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x20;
  iVar1 = (int)param_1 + (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) + 0x10;
  if ((*(int32_t*)rtguest::G((uint32_t)param_1)) < iVar2) {
    (*(int32_t*)rtguest::G((uint32_t)param_1)) = 0;
    iVar1 = 0;
  }
  else {
    (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t))) = iVar2;
  }
  if (iVar1 != 0) {
    gifAdPair(iVar1,param_2,1);
    return;
  }
  return;
}


RT_GUEST_FN(0x0024f6d0, guest::uiArenaSetColour)

}  // namespace guest
