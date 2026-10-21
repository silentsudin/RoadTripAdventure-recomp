// guest:0x00243a78 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ComputeScaledPartValue(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x18, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x20, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x28) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x2c) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(2, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(3, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = RT_CALLRA(0x243aa4, guest::ComputeSlotScore(param_3));
  iVar2 = RT_CALLRA(0x243ab0, guest::ComputeSlotScore(param_3 ^ 1));
  iVar3 = RT_CALLRA(0x243ac0, guest::DispatchByIndex15(param_1,param_2));
  if (param_4 == 0) {
    iVar1 = (iVar3 * (2000 - (iVar1 - iVar2))) / 2000;
  }
  else {
    iVar1 = (iVar3 * ((iVar1 - iVar2) + 3000)) / 4000;
  }
  return iVar1;
}


RT_GUEST_FN(0x00243a78, guest::ComputeScaledPartValue)

}  // namespace guest
