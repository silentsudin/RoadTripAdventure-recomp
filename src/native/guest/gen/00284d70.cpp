// guest:00284d70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iDisableDmac(uint32_t q0, ...) {
  syscall(0xffffffffffffffe3);
  return 0;
}


RT_GUEST_FN(0x00284d70, guest::_iDisableDmac)

}  // namespace guest
