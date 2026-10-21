// guest:0x0027a468 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int64_t sceMcGetDir(uint32_t param_1, uint32_t param_2, int64_t param_3, uint32_t param_4, int64_t param_5, uint64_t param_6, ...) {
  rtguest::FrameGuard rt_fg(160);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 6, rt_sv, 0x70, 8);
    rtguest::rtSpillS(rt_fg, 5, rt_sv, 0x60, 8);
    rtguest::rtSpillS(rt_fg, 4, rt_sv, 0x50, 8);
    rtguest::rtSpillS(rt_fg, 3, rt_sv, 0x40, 8);
    rtguest::rtSpillS(rt_fg, 2, rt_sv, 0x30, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
    rtguest::setS32(2, (uint32_t)param_6);  // walk: = param_6
    rtguest::setS32(3, (uint32_t)param_2);  // walk: = param_2
    rtguest::setS32(4, (uint32_t)param_4);  // walk: = param_4
    rtguest::gS[5] = 0x00000000002f0000ull;  // walk const
    rtguest::setS32(6, (uint32_t)param_1);  // walk: = param_1
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x27a4ac, guest::PollSema((*(uint32_t*)rtguest::G(0x2e89bc))));
  if (lVar1 < 0) {
    lVar1 = -200;
  }
  else if ((*(uint32_t*)rtguest::G(0x1774b64)) == 0) {
    RT_CALLRA(0x27a4d4, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -100;
  }
  else if ((param_3 == 0) || ((*(int8_t*)rtguest::G((uint32_t)(uint32_t)param_3)) == '\0')) {
    RT_CALLRA(0x27a4f8, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    lVar1 = -0xd2;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1774c00)) = (uint32_t)param_6;
    (*(uint32_t*)rtguest::G(0x1774bf0)) = param_1;
    (*(uint32_t*)rtguest::G(0x1774bf4)) = param_2;
    (*(uint32_t*)rtguest::G(0x1774bf8)) = param_4;
    (*(uint32_t*)rtguest::G(0x1774bfc)) = (int)param_5;
    (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01774c04)), (char *)rtguest::G((uint32_t)((uint32_t)param_3)), 0x3ff));
    (*(uint8_t*)rtguest::G(0x1775003)) = 0;
    if (-1 < param_5) {
      sceSifWriteBackDCache(param_6,(int)param_5 << 6);
    }
    lVar1 = RT_CALLRA(0x27a568, guest::sceSifCallRpc(0x1774b40,0xd,1,0x1774bf0,0x414,0x1776100,4,0));
    if (lVar1 == 0) {
      (*(uint32_t*)rtguest::G(0x2e89b8)) = 0xd;
    }
    else {
      RT_CALLRA(0x27a588, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e89bc))));
    }
  }
  return lVar1;
}


RT_GUEST_FN(0x0027a468, guest::sceMcGetDir)

}  // namespace guest
