// guest:0x00283a40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _lo0bits(uint32_t param_1, ...) {
  int iVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = (*(uint32_t*)rtguest::G((uint32_t)param_1));
  if ((uVar3 & 7) == 0) {
    iVar2 = 0;
    if ((uVar3 & 0xffff) == 0) {
      iVar2 = 0x10;
      uVar3 = uVar3 >> 0x10;
    }
    if ((uVar3 & 0xff) == 0) {
      iVar2 = iVar2 + 8;
      uVar3 = uVar3 >> 8;
    }
    if ((uVar3 & 0xf) == 0) {
      iVar2 = iVar2 + 4;
      uVar3 = uVar3 >> 4;
    }
    if ((uVar3 & 3) == 0) {
      iVar2 = iVar2 + 2;
      uVar3 = uVar3 >> 2;
    }
    if ((uVar3 & 1) == 0) {
      iVar1 = 0x20;
      if (uVar3 >> 1 != 0) {
        (*(uint32_t*)rtguest::G((uint32_t)param_1)) = uVar3 >> 1;
        iVar1 = iVar2 + 1;
      }
    }
    else {
      (*(uint32_t*)rtguest::G((uint32_t)param_1)) = uVar3;
      iVar1 = iVar2;
    }
  }
  else {
    iVar1 = 0;
    if ((uVar3 & 1) == 0) {
      if ((uVar3 & 2) == 0) {
        (*(uint32_t*)rtguest::G((uint32_t)param_1)) = uVar3 >> 2;
        return 2;
      }
      (*(uint32_t*)rtguest::G((uint32_t)param_1)) = uVar3 >> 1;
      return 1;
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x00283a40, guest::_lo0bits)

}  // namespace guest
