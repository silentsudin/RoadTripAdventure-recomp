// guest:0x0028bd48 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifAllocSysMemory(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  rtguest::FrameGuard rt_fg(48);
  undefined4 uVar1;
  long lVar2;
  
  uVar1 = 0;
  if (-1 < (*(uint32_t*)rtguest::G(0x2e999c))) {
    (*(uint32_t*)rtguest::G(0x1779cc0)) = param_2;
    (*(uint32_t*)rtguest::G(0x1779cc4)) = param_1;
    (*(uint32_t*)rtguest::G(0x1779cc8)) = param_3;
    lVar2 = sceSifCallRpc(0x1779c40,4,0,0x1779cc0,0xc,0x1779c80,4,0);
    uVar1 = (*(uint32_t*)rtguest::G(0x1779c80));
    if (lVar2 < 0) {
      uVar1 = 0;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028bd48, guest::sceSifAllocSysMemory)

}  // namespace guest
