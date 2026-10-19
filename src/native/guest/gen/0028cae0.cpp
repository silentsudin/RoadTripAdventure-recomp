// guest:0x0028cae0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifGetIopAddr(uint32_t param_1, uint32_t param_2, uint32_t param_3, ...) {
  undefined4 uVar1;
  long lVar2;
  
  lVar2 = _lf_bind();
  if (lVar2 < 0) {
    return 0xffff0000;
  }
  if (param_3 < 3) {
    (*(uint32_t*)rtguest::G(0x1779e04)) = (uint32_t)param_3;
    (*(uint32_t*)rtguest::G(0x1779e00)) = param_1;
    lVar2 = sceSifCallRpc(0x177a000,3,0,0x1779e00,0x20,0x1779e00,0x20,0);
    if (lVar2 < 0) {
      return 0xfffeffff;
    }
    if (param_3 == 0) {
      (*(uint8_t*)rtguest::G((uint32_t)(uint32_t)param_2)) = (uint32_t)(*(uint32_t*)rtguest::G(0x1779e00));
    }
    else if (param_3 == 1) {
      (*(uint16_t*)rtguest::G((uint32_t)(uint32_t)param_2)) = (uint32_t)(*(uint32_t*)rtguest::G(0x1779e00));
    }
    else {
      if (param_3 != 2) goto LAB_0028cb9c;
      (*(uint32_t*)rtguest::G((uint32_t)param_2)) = (*(uint32_t*)rtguest::G(0x1779e00));
    }
    uVar1 = 0;
  }
  else {
LAB_0028cb9c:
    uVar1 = 0xfffefffe;
  }
}


RT_GUEST_FN(0x0028cae0, guest::sceSifGetIopAddr)

}  // namespace guest
