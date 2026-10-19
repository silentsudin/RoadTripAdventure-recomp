// guest:0x00277bc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t copysign(uint32_t param_1, uint32_t param_2, ...) {
  return param_1 & 0xffffffff |
         ((long)param_1 >> 0x20 & 0x7fffffffU | param_2 >> 0x20 & 0xffffffff80000000U) << 0x20;
}


RT_GUEST_FN(0x00277bc0, guest::copysign)

}  // namespace guest
