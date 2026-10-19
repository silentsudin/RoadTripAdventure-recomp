// guest:0x002853b0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifGetReg(uint32_t q0, ...) {
  syscall(0x7a);
  return 0;
}


RT_GUEST_FN(0x002853b0, guest::sceSifGetReg)

}  // namespace guest
