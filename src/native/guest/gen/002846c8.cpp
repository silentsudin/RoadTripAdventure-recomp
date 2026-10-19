// guest:0x002846c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t isinf(uint32_t param_1, ...) {
  uint uVar1;
  
  uVar1 = 0x7ff00000 -
          ((uint32_t)((uint32_t)param_1 >> 0x20) & 0x7fffffff | ((uint32_t)param_1 | -(uint32_t)param_1) >> 0x1f);
  return ((int)(uVar1 | -uVar1) >> 0x1f) + 1;
}


RT_GUEST_FN(0x002846c8, guest::isinf)

}  // namespace guest
