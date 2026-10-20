// guest:0x002234c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawModelParts(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
  }
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = (*(int32_t*)rtguest::G((uint32_t)param_1));
  if (iVar2 < 0) {
    iVar2 = iVar2 + 0xf;
  }
  iVar3 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(1) * sizeof(int32_t)));
  iVar1 = (*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(3) * sizeof(int32_t)));
  if (((*(int32_t*)rtguest::G((uint32_t)param_1 + (uint32_t)(2) * sizeof(int32_t))) != 0) && ((param_2 & 1) != 0)) {
    vifCallModel(param_1 + (uint32_t)((iVar2 >> 4) * 4) * sizeof(int32_t));
  }
  if (iVar1 != 0) {
    if (iVar3 < 0) {
      iVar3 = iVar3 + 0xf;
    }
    if ((param_2 & 2) != 0) {
      vifCallModel(param_1 + (uint32_t)((iVar3 >> 4) * 4) * sizeof(int32_t));
      return 0;
    }
  }
  return 0;
}


RT_GUEST_FN(0x002234c8, guest::drawModelParts)

}  // namespace guest
