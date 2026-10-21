// guest:0x002791d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdBreak(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x30) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x34) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x20, 8);
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x2791e8, guest::_sceCd_scmd_prechk(0x1e));
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G(0x2e6f54)) = 8;
    lVar1 = RT_CALLRA(0x279234, guest::sceSifCallRpc(0x2e8948,0x16,0,0,0,0x2e8100,4,0));
    if (lVar1 < 0) {
      RT_CALLRA(0x279248, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
      (*(uint32_t*)rtguest::G(0x2e6f54)) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G(0x2e6f54)) = 0;
      RT_CALLRA(0x279270, guest::SignalSema((*(int32_t*)rtguest::G(0x2e6f2c))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002791d8, guest::sceCdBreak)

}  // namespace guest
