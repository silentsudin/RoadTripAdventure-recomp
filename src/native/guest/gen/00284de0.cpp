// guest:0x00284de0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ExitDeleteThread(uint32_t q0, ...) {
  syscall(0x24);
  return 0;
}


RT_GUEST_FN(0x00284de0, guest::ExitDeleteThread)

}  // namespace guest
