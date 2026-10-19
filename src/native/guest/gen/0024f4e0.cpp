// guest:0x0024f4e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountDecimalDigits(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = 1;
  while (param_1 = param_1 / 10, param_1 != 0) {
    iVar1 = iVar1 + 1;
  }
}


RT_GUEST_FN(0x0024f4e0, guest::CountDecimalDigits)

}  // namespace guest
