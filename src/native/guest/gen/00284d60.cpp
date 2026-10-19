// guest:00284d60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iEnableDmac(uint32_t q0, ...) {
  syscall(0xffffffffffffffe4);
  return 0;
}


RT_GUEST_FN(0x00284d60, guest::_iEnableDmac)

}  // namespace guest
