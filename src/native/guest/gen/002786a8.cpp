// guest:0x002786a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdNcmdDiskReady(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
  }
  long lVar1;
  
  lVar1 = RT_CALLRA(0x2786b4, guest::_sceCd_ncmd_prechk(2));
  if (lVar1 != 0) {
    lVar1 = RT_CALLRA(0x2786f4, guest::sceSifCallRpc(0x2e80d0,0xe,0,0,0,0x2e6f80,4,0));
    if (lVar1 < 0) {
      RT_CALLRA(0x278708, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e6f28))));
    }
    else {
      RT_CALLRA(0x278724, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e6f28))));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002786a8, guest::sceCdNcmdDiskReady)

}  // namespace guest
