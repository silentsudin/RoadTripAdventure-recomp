// guest:0x00284d10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _DisableDmac(uint32_t q0, ...) {
  syscall(0x17);
  return 0;
}


RT_GUEST_FN(0x00284d10, guest::_DisableDmac)

}  // namespace guest
