// guest:0x00284dc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t StartThread(uint32_t q0, ...) {
  syscall(0x22);
  return 0;
}


RT_GUEST_FN(0x00284dc0, guest::StartThread)

}  // namespace guest
