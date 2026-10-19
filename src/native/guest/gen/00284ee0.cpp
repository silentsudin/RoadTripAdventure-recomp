// guest:0x00284ee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iWakeupThread(uint32_t q0, ...) {
  syscall(0xffffffffffffffcc);
  return 0;
}


RT_GUEST_FN(0x00284ee0, guest::_iWakeupThread)

}  // namespace guest
