// guest:0x00207b20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t CountSetBits(uint32_t param_1, ...) {
  int iVar1;
  
  iVar1 = 0;
  for (; param_1 != 0; param_1 = param_1 >> 1) {
    iVar1 = iVar1 + (param_1 & 1);
  }
  return iVar1;
}


RT_GUEST_FN(0x00207b20, guest::CountSetBits)

}  // namespace guest
