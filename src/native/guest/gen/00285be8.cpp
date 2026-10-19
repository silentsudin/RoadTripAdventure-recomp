// guest:0x00285be8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t setSyscall(uint32_t q0, ...) {
  syscall(0x74);
  return 0;
}


RT_GUEST_FN(0x00285be8, guest::setSyscall)

}  // namespace guest
