// guest:0x002886b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t get_iob(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    rtguest::rtSpillS(rt_fg, 1, rt_sv, 0x10, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_1);  // walk: = param_1
  }
  int iVar1;
  
  RT_CALLRA(0x2886c4, guest::fsFdSemInit());
  RT_CALLRA(0x2886cc, guest::WaitSema((*(uint32_t*)rtguest::G(0x2e9990))));
  if (param_1 < 0x20) {
    iVar1 = (int)param_1 * 0x10 + 0x1779980;
    RT_CALLRA(0x2886e0, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
  }
  else {
    RT_CALLRA(0x2886fc, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e9990))));
    iVar1 = 0;
  }
  return iVar1;
}


RT_GUEST_FN(0x002886b0, guest::get_iob)

}  // namespace guest
