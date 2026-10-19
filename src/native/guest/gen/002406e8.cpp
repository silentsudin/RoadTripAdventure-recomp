// guest:0x002406e8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t EmptyStub(uint32_t q0, ...) {
  return 0;
}


RT_GUEST_FN(0x002406e8, guest::EmptyStub)

}  // namespace guest
