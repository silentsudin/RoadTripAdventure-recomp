// guest:0x00274af8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t atan2f(uint32_t q0, ...) {
  __ieee754_atan2f();
  return 0;
}


RT_GUEST_FN(0x00274af8, guest::atan2f)

}  // namespace guest
