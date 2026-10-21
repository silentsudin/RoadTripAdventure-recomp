// guest:0x0027a2e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sceMcGetInfo(uint32_t param_1, uint32_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(2, (uint32_t)param_1);  // walk: = param_1
    rtguest::setS32(3, (uint32_t)param_3);  // walk: = param_3
    rtguest::setS32(4, (uint32_t)param_4);  // walk: = param_4
    rtguest::setS32(5, (uint32_t)param_5);  // walk: = param_5
    rtguest::gS[6] = 0x00000000002f0000ull;  // walk const
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x27a320, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x27a348, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -100;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774bdc)) = 0x01775040;
    (*(uint32_t*)rtguest::G(0x1774bd4)) = (uint32_t)(param_3 != 0);
    (*(uint32_t*)rtguest::G(0x1774bd0)) = (uint32_t)(param_4 != 0);
    (*(uint32_t*)rtguest::G(0x1774bcc)) = (uint32_t)(param_5 != 0);
    (*(uint32_t*)rtguest::G(0x1774b68)) = (uint32_t)param_3;
    (*(uint32_t*)rtguest::G(0x1774b6c)) = (uint32_t)param_4;
    (*(uint32_t*)rtguest::G(0x1774b70)) = (uint32_t)param_5;
    (*(uint32_t*)rtguest::G(0x1774bc4)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bc8)) = param_2;
    sceSifWriteBackDCache(0x1775040,0xc0);
    lVar1 = RT_CALLRA(0x27a40c, guest::sceSifCallRpc(0x1774b40,1,1,0x1774bc0,0x30,0x1776100,4,0x27a288));
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 1;
    }
    else {
      RT_CALLRA(0x27a42c, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a2e0, guest::sceMcGetInfo)

}  // namespace guest
