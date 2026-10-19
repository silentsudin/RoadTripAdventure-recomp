// guest:0x0027af60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t __muldi3(uint32_t param_1, uint32_t param_2, ...) {
  ulong uVar1;
  
  uVar1 = (param_1 & 0xffffffff) * (param_2 & 0xffffffff);
  return uVar1 & 0xffffffff |
         (long)((int)(uVar1 >> 0x20) +
               (int)param_1 * (int)(param_2 >> 0x20) + (int)(param_1 >> 0x20) * (int)param_2) <<
         0x20;
}


RT_GUEST_FN(0x0027af60, guest::__muldi3)

}  // namespace guest
