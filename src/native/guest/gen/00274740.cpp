// guest:0x00274740 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t scePadGetModVersion(uint32_t q0, ...) {
  undefined4 uVar1;
  long lVar2;
  
  (*(uint32_t*)rtguest::G(0x1773500)) = 0x12;
  lVar2 = sceSifCallRpc(0x17731c0,1,0,0x1773500,0x80,0x1773500,0x80,0);
  uVar1 = (*(uint32_t*)rtguest::G(0x177350c));
  if (lVar2 < 0) {
    uVar1 = 0;
  }
}


RT_GUEST_FN(0x00274740, guest::scePadGetModVersion)

}  // namespace guest
