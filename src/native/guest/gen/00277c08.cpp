// guest:0x00277c08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t copysignf(uint32_t param_1, uint32_t param_2, ...) {
  return param_1 & 0x7fffffff | param_2 & 0x80000000;
}


RT_GUEST_FN(0x00277c08, guest::copysignf)

}  // namespace guest
