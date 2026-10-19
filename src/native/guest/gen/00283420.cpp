// guest:0x00283420 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t localeconv(uint32_t q0, ...) {
  _localeconv_r((*(uint32_t*)rtguest::G(0x002e8cc0)));
  return 0;
}


RT_GUEST_FN(0x00283420, guest::localeconv)

}  // namespace guest
