// guest:0x00241ba0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t NoOpEffectB(uint32_t q0, ...) {
  return 0;
}


RT_GUEST_FN(0x00241ba0, guest::NoOpEffectB)

}  // namespace guest
