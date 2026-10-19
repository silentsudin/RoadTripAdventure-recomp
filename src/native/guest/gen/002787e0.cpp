// guest:0x002787e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceCdSyncS(uint32_t param_1, ...) {
  long lVar1;
  undefined8 uVar2;
  
  if (param_1 == 0) {
    if (0 < (*(uint32_t*)rtguest::G(0x002e6f10))) {
      scePrintf(0x331c88);
    }
    while( true ) {
      lVar1 = sceSifCheckStatRpc(0x2e8948);
      uVar2 = 0;
      if (lVar1 == 0) break;
      sceCdDelayThread(0x3c);
    }
  }
  else {
    uVar2 = sceSifCheckStatRpc(0x2e8948);
  }
  return uVar2;
}


RT_GUEST_FN(0x002787e0, guest::sceCdSyncS)

}  // namespace guest
