// guest:0x002786a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdNcmdDiskReady(uint32_t q0, ...) {
  long lVar1;
  
  lVar1 = _sceCd_ncmd_prechk(2);
  if (lVar1 != 0) {
    lVar1 = sceSifCallRpc(0x2e80d0,0xe,0,0,0,0x2e6f80,4,0);
    if (lVar1 < 0) {
      SignalSema((*(uint32_t*)rtguest::G(0x2e6f28)));
    }
    else {
      SignalSema((*(uint32_t*)rtguest::G(0x2e6f28)));
    }
  }
  return 0;
}


RT_GUEST_FN(0x002786a8, guest::sceCdNcmdDiskReady)

}  // namespace guest
