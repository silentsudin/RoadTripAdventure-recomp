// guest:0x002853a0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSetReg(uint32_t q0, ...) {
  syscall(0x79);
  return 0;
}


RT_GUEST_FN(0x002853a0, guest::sceSifSetReg)

}  // namespace guest
