// guest:0x002853d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t Deci2Call(uint32_t q0, ...) {
  syscall(0x7c);
  return 0;
}


RT_GUEST_FN(0x002853d0, guest::Deci2Call)

}  // namespace guest
