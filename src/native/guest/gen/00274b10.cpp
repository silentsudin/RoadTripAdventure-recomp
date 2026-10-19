// guest:0x00274b10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sqrtf(uint32_t q0, ...) {
  __ieee754_sqrtf();
  return 0;
}


RT_GUEST_FN(0x00274b10, guest::sqrtf)

}  // namespace guest
