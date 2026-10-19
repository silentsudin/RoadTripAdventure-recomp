// guest:0x00284cb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t AddDmacHandler(uint32_t q0, ...) {
  syscall(0x12);
  return 0;
}


RT_GUEST_FN(0x00284cb0, guest::AddDmacHandler)

}  // namespace guest
