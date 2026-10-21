// guest:0x002234c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t drawModelParts(uint32_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
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
