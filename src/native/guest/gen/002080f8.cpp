// guest:0x002080f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t uiFade(uint64_t param_1, ...) {
  (*(uint64_t*)rtguest::G(0x29abb0)) = param_1;
  vifDirect(0x29ab70,6);
  return 0;
}


RT_GUEST_FN(0x002080f8, guest::uiFade)

}  // namespace guest
