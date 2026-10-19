// guest:0x00241838 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t NoOpEffect(uint32_t q0, ...) {
  return 0;
}


RT_GUEST_FN(0x00241838, guest::NoOpEffect)

}  // namespace guest
