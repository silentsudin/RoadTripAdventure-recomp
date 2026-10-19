// guest:0x00284d00 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _EnableDmac(uint32_t q0, ...) {
  syscall(0x16);
  return 0;
}


RT_GUEST_FN(0x00284d00, guest::_EnableDmac)

}  // namespace guest
