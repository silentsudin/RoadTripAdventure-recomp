// guest:0x0021fa38 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t cellVisible(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x8, 8);
    rtguest::rtSpillF(rt_fg, 5, rt_sv, 0x20);
    rtguest::rtSpillF(rt_fg, 4, rt_sv, 0x18);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x10) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x14) = 0x00000000u;
    rtguest::setS32(1, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x21fa60, guest::TestSegmentInBoxWindow(param_3 + 0x100,param_3 + 0x120));
  if (lVar1 == 0) {
    lVar1 = RT_CALLRA(0x21fa7c, guest::TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x110,param_3 + 0x130));
    if (lVar1 != 0) {
      return true;
    }
    lVar1 = RT_CALLRA(0x21fa9c, guest::TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x140,param_3 + 0x150));
    if (lVar1 == 0) {
      lVar1 = RT_CALLRA(0x21fac0, guest::TestSegmentInBoxWindow(param_1,param_2,param_3 + 0x140,param_3 + 0x160));
      return lVar1 != 0;
    }
  }
  return true;
}


RT_GUEST_FN(0x0021fa38, guest::cellVisible)

}  // namespace guest
