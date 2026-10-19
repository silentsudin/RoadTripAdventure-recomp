// guest:0x002839c0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _hi0bits(uint32_t param_1, ...) {
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if ((param_1 & 0xffff0000) == 0) {
    iVar2 = 0x10;
    param_1 = param_1 << 0x10;
  }
  if ((param_1 & 0xff000000) == 0) {
    iVar2 = iVar2 + 8;
    param_1 = param_1 << 8;
  }
  if ((param_1 & 0xf0000000) == 0) {
    iVar2 = iVar2 + 4;
    param_1 = param_1 << 4;
  }
  if ((param_1 & 0xc0000000) == 0) {
    iVar2 = iVar2 + 2;
    param_1 = param_1 << 2;
  }
  iVar1 = iVar2;
  if (-1 < (int)param_1) {
    iVar1 = 0x20;
    if ((param_1 & 0x40000000) != 0) {
      iVar1 = iVar2 + 1;
    }
  }
  return iVar1;
}


RT_GUEST_FN(0x002839c0, guest::_hi0bits)

}  // namespace guest
