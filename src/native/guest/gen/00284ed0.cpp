// guest:00284ed0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t WakeupThread(uint32_t q0, ...) {
  syscall(0x33);
  return 0;
}


RT_GUEST_FN(0x00284ed0, guest::WakeupThread)

}  // namespace guest
