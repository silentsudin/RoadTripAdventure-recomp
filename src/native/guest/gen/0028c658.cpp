// guest:0x0028c658 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSearchModuleByAddress(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = _lf_bind();
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = lfVersionCheck();
    if (lVar2 == 0) {
      (*(uint32_t*)rtguest::G(0x1779e00)) = param_1;
      lVar2 = sceSifCallRpc(0x177a000,10,0,0x1779e00,4,0x1779e00,4,0);
      uVar1 = (*(uint32_t*)rtguest::G(0x1779e00));
      if (lVar2 < 0) {
        uVar1 = 0xfffeffff;
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
  return uVar1;
}


RT_GUEST_FN(0x0028c658, guest::sceSifSearchModuleByAddress)

}  // namespace guest
