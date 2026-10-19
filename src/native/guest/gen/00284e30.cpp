// guest:0x00284e30 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ChangeThreadPriority(uint32_t q0, ...) {
  syscall(0x29);
  return 0;
}


RT_GUEST_FN(0x00284e30, guest::ChangeThreadPriority)

}  // namespace guest
