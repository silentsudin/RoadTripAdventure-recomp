// guest:0x0027c9a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __deregister_frame_info(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(32);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x10, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
    rtguest::gS[2] = 0x00000000002f0000ull;  // walk const
  }
uint32_t __ptr;
  int iVar1;
uint32_t piVar2;
  
  RT_CALLRA(0x27c9c8, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e99ac))));
  piVar2 = 0x01776160;
  iVar1 = (*(uint32_t*)rtguest::G(0x1776160));
  while( true ) {
    if (iVar1 == 0) {
      RT_CALLRA(0x27ca14, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e99ac))));
      iVar1 = abort();
      return iVar1;
    }
    if ((*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 8))) == param_1) break;
    piVar2 = (uint32_t)(iVar1 + 0x14);
    iVar1 = (*(int32_t*)rtguest::G((uint32_t)piVar2));
  }
  __ptr = (*(uint32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0xc)));
  (*(int32_t*)rtguest::G((uint32_t)piVar2)) = (*(int32_t*)rtguest::G((uint32_t)(uint32_t)(iVar1 + 0x14)));
  if ((__ptr != (uint32_t)0x0) && (__ptr != param_1)) {
    RT_CALLRA(0x27ca0c, guest::free(__ptr));
  }
  RT_CALLRA(0x27ca44, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e99ac))));
  return iVar1;
}


RT_GUEST_FN(0x0027c9a8, guest::__deregister_frame_info)

}  // namespace guest
