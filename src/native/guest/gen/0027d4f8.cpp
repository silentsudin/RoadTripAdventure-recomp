// guest:0x0027d4f8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t switchD_0027d18c_executeCfaInsnAbort(uint32_t q0, ...) {
  abort();
  return 0;
}


RT_GUEST_FN(0x0027d4f8, guest::switchD_0027d18c_executeCfaInsnAbort)

}  // namespace guest
