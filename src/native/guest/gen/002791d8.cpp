// guest:0x002791d8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdBreak(uint32_t q0, ...) {
  rtguest::FrameGuard rt_fg(64);
  long lVar1;
  
  lVar1 = _sceCd_scmd_prechk(0x1e);
  if (lVar1 != 0) {
    (*(uint32_t*)rtguest::G(0x2e6f54)) = 8;
    lVar1 = sceSifCallRpc(0x2e8948,0x16,0,0,0,0x2e8100,4,0);
    if (lVar1 < 0) {
      SignalSema((*(int32_t*)rtguest::G(0x2e6f2c)));
      (*(uint32_t*)rtguest::G(0x2e6f54)) = 0;
    }
    else {
      (*(uint32_t*)rtguest::G(0x2e6f54)) = 0;
      SignalSema((*(int32_t*)rtguest::G(0x2e6f2c)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002791d8, guest::sceCdBreak)

}  // namespace guest
