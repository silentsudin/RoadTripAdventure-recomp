// guest:0x00285c40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t getSyscall(uint32_t q0, ...) {
  syscall(0x5b);
  return 0;
}


RT_GUEST_FN(0x00285c40, guest::getSyscall)

}  // namespace guest
