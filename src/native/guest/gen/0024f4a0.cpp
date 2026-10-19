// guest:0x0024f4a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t IntToPackedBCD(uint32_t param_1, ...) {
  uint uVar1;
  uint uVar2;
  
  uVar2 = 0;
  uVar1 = 0;
  for (; param_1 != 0; param_1 = param_1 / 10) {
    uVar2 = uVar2 | param_1 % 10 << ((uVar1 & 7) << 2);
    uVar1 = uVar1 + 1;
  }
}


RT_GUEST_FN(0x0024f4a0, guest::IntToPackedBCD)

}  // namespace guest
