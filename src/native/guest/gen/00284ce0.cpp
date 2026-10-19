// guest:0x00284ce0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _EnableIntc(uint32_t q0, ...) {
  syscall(0x14);
  return 0;
}


RT_GUEST_FN(0x00284ce0, guest::_EnableIntc)

}  // namespace guest
