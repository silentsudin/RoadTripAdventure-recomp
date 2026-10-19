// guest:0x0027ed58 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t raise(uint32_t param_1, ...) {
  _raise_r((*(uint32_t*)rtguest::G(0x002e8cc0)),param_1);
  return 0;
}


RT_GUEST_FN(0x0027ed58, guest::raise)

}  // namespace guest
