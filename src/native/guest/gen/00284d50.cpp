// guest:0x00284d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iDisableIntc(uint32_t q0, ...) {
  syscall(0xffffffffffffffe5);
  return 0;
}


RT_GUEST_FN(0x00284d50, guest::_iDisableIntc)

}  // namespace guest
