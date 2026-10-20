// guest:0x0028bcd8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifAllocIopHeap(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(48);
  long lVar1;
  
  if ((-1 < (*(uint32_t*)rtguest::G(0x2e999c))) &&
     ((*(uint32_t*)rtguest::G(0x1779cc0)) = param_1, lVar1 = sceSifCallRpc(0x1779c40,1,0,0x1779cc0,4,0x1779c80,4,0),
     -1 < lVar1)) {
    return (*(uint32_t*)rtguest::G(0x1779c80));
  }
  return 0;
}


RT_GUEST_FN(0x0028bcd8, guest::sceSifAllocIopHeap)

}  // namespace guest
