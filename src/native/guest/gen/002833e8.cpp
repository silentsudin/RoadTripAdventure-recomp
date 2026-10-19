// guest:0x002833e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _localeconv_r(uint32_t q0, ...) {
  return 0x3323d8u;
}


RT_GUEST_FN(0x002833e8, guest::_localeconv_r)

}  // namespace guest
