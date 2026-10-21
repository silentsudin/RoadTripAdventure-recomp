// guest:0x002583f0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetVolumeDgaRegs(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x0, 8);
    *(uint32_t*)rtguest::G(rt_sv+0x8) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0xc) = 0x00000000u;
    rtguest::setS32(0, (uint32_t)param_2);  // walk: = param_2
  }
  sceSdRemote(1,0x8010,0x981,param_1);
  sceSdRemote(1,0x8010,0xa81,param_2);
  return 0;
}


RT_GUEST_FN(0x002583f0, guest::SetVolumeDgaRegs)

}  // namespace guest
