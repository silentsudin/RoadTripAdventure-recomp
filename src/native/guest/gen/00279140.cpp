// guest:0x00279140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdGetError(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x20) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x24) = 0x00000000u;
    rtguest::rtSpillS(rt_fg, 0, rt_sv, 0x10, 8);
  }
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = RT_CALLRA(0x27914c, guest::_sceCd_scmd_prechk(3));
  if (lVar2 == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    lVar2 = RT_CALLRA(0x27918c, guest::sceSifCallRpc(0x2e8948,4,0,0,0,0x2e8100,4,0));
    if (lVar2 < 0) {
      RT_CALLRA(0x2791a0, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e6f2c))));
      uVar1 = 0xffffffff;
    }
    else {
      uVar1 = 0;
      RT_CALLRA(0x2791bc, guest::SignalSema((*(uint32_t*)rtguest::G(0x2e6f2c))));
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x00279140, guest::sceCdGetError)

}  // namespace guest
