// guest:0x00284f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t _iSuspendThread(uint32_t q0, ...) {
  syscall(0xffffffffffffffc8);
  return 0;
}


RT_GUEST_FN(0x00284f20, guest::_iSuspendThread)

}  // namespace guest
