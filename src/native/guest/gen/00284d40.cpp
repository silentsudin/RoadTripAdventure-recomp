// guest:00284d40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iEnableIntc(uint32_t q0, ...) {
  syscall(0xffffffffffffffe6);
  return 0;
}


RT_GUEST_FN(0x00284d40, guest::_iEnableIntc)

}  // namespace guest
