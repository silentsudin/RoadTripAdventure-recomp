// guest:0x0027a798 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcFormat(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(80);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::gS[1] = 0x00000000002f0000ull;  // walk const
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x27a7b8, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x27a7e0, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -100;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774bc4)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bc8)) = param_2;
    lVar1 = RT_CALLRA(0x27a81c, guest::sceSifCallRpc(0x1774b40,0x10,1,0x1774bc0,0x30,0x1776100,4,0));
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 0x10;
    }
    else {
      RT_CALLRA(0x27a83c, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a798, guest::sceMcFormat)

}  // namespace guest
