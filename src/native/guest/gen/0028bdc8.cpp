// guest:0x0028bdc8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifFreeIopHeap(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  long lVar2;
  
  if ((*(uint32_t*)rtguest::G(0x2e999c)) < 0) {
    uVar1 = 0;
  }
  else {
    (*(uint32_t*)rtguest::G(0x1779cc0)) = param_1;
    lVar2 = sceSifCallRpc(0x1779c40,2,0,0x1779cc0,4,0x1779c80,4,0);
    uVar1 = (*(uint32_t*)rtguest::G(0x1779c80));
    if (lVar2 < 0) {
      uVar1 = 0xffffffff;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028bdc8, guest::sceSifFreeIopHeap)

}  // namespace guest
