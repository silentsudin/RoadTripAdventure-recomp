// guest:0x0028c990 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _sceSifLoadElfPart(uint32_t param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4, ...) {
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = _lf_bind();
  uVar1 = 0xffff0000;
  if (-1 < lVar2) {
    lVar2 = lfVersionCheck();
    if (lVar2 == 0) {
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)((uint32_t)0x01779e08)), (char *)rtguest::G((uint32_t)(param_1)), 0xfc));
      (*(uint32_t*)rtguest::G(0x1779f03)) = 0;
      (uint32_t)(uintptr_t)(::strncpy((char *)rtguest::G((uint32_t)(0x01779f04)), (char *)rtguest::G((uint32_t)(param_2)), 0xfc));
      (*(uint32_t*)rtguest::G(0x1779fff)) = 0;
      lVar2 = sceSifCallRpc(0x177a000,param_4,0,0x1779e00,0x200,0x1779e00,0x10,0);
      if (lVar2 < 0) {
        uVar1 = 0xfffeffff;
      }
      else if ((*(uint32_t*)rtguest::G(0x1779e00)) == 0) {
        uVar1 = 0xfffefffd;
      }
      else {
        (*(int32_t*)rtguest::G((uint32_t)param_3)) = (*(uint32_t*)rtguest::G(0x1779e00));
        uVar1 = 0;
        (*(int32_t*)rtguest::G((uint32_t)param_3 + (uint32_t)(1) * sizeof(int32_t))) = (*(uint32_t*)rtguest::G(0x1779e04));
      }
    }
    else {
      uVar1 = 0xfffefffc;
    }
  }
}


RT_GUEST_FN(0x0028c990, guest::_sceSifLoadElfPart)

}  // namespace guest
