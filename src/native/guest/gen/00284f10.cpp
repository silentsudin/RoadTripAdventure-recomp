// guest:0x00284f10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SuspendThread(uint32_t q0, ...) {
  syscall(0x37);
  return 0;
}


RT_GUEST_FN(0x00284f10, guest::SuspendThread)

}  // namespace guest
