// guest:0x00284cf0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _DisableIntc(uint32_t q0, ...) {
  syscall(0x15);
  return 0;
}


RT_GUEST_FN(0x00284cf0, guest::_DisableIntc)

}  // namespace guest
