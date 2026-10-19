// guest:0x0020e1b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AbortCdOperation(uint32_t q0, ...) {
  (*(uint32_t*)rtguest::G(0x333370)) = 0;
  if (((*(uint32_t*)rtguest::G(0x3356c0)) & 0x8000) != 0) {
    sceCdBreak();
    return 0;
  }
}


RT_GUEST_FN(0x0020e1b8, guest::AbortCdOperation)

}  // namespace guest
