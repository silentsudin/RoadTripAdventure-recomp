// guest:0x0028c5b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSearchModuleByName(uint32_t param_1, ...) {
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = _lf_bind();
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = lfVersionCheck();
    if (lVar2 == 0) {
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)((uint32_t)0x01779e08)), (char *)rtguest::G((uint32_t)(param_1)), 0xfc));
      (*(uint8_t*)rtguest::G(0x1779f03)) = 0;
      lVar2 = sceSifCallRpc(0x177a000,9,0,0x1779e00,0x200,0x1779e00,4,0);
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


RT_GUEST_FN(0x0028c5b8, guest::sceSifSearchModuleByName)

}  // namespace guest
