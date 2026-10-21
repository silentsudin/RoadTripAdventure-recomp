// guest:0x0027a860 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcDelete(uint32_t param_1, uint32_t param_2, int64_t param_3, ...) {
  rtguest::FrameGuard rt_fg(128);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::gS[2] = 0x00000000002f0000ull;  // walk const
    rtguest::setS32(4, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(5, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x27a890, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x27a8b8, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -100;
  }
  else if ((param_3 == 0) || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_3)) == '\0')) {
    RT_CALLRA(0x27a8dc, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -0xd2;
  }
  else {
    (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01774c04)), (char *)rtguest::G((uint32_t)((uint32_t)param_3)), 0x3ff));
    (*(uint8_t*)rtguest::G(0x1775003)) = 0;
    (*(uint32_t*)rtguest::G(0x1774bf8)) = 0;
    (*(uint32_t*)rtguest::G(0x1774bf0)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bf4)) = param_2;
    lVar1 = RT_CALLRA(0x27a938, guest::sceSifCallRpc(0x1774b40,0xf,1,0x1774bf0,0x414,0x1776100,4,0));
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 0xf;
    }
    else {
      RT_CALLRA(0x27a958, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a860, guest::sceMcDelete)

}  // namespace guest
