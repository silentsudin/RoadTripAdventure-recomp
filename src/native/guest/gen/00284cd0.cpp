// guest:0x00284cd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RemoveDmacHandler(uint32_t q0, ...) {
  syscall(0x13);
  return 0;
}


RT_GUEST_FN(0x00284cd0, guest::RemoveDmacHandler)

}  // namespace guest
