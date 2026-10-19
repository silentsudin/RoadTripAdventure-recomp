// guest:0x002852e0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GsPutIMR(uint32_t q0, ...) {
  syscall(0x71);
  return 0;
}


RT_GUEST_FN(0x002852e0, guest::GsPutIMR)

}  // namespace guest
