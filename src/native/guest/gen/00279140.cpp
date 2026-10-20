// guest:0x00279140 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdGetError(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(48);
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = _sceCd_scmd_prechk(3);
  if (lVar2 == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    lVar2 = sceSifCallRpc(0x2e8948,4,0,0,0,0x2e8100,4,0);
    if (lVar2 < 0) {
      SignalSema((*(uint32_t*)rtguest::G(0x2e6f2c)));
      uVar1 = 0xffffffff;
    }
    else {
      uVar1 = 0;
      SignalSema((*(uint32_t*)rtguest::G(0x2e6f2c)));
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x00279140, guest::sceCdGetError)

}  // namespace guest
