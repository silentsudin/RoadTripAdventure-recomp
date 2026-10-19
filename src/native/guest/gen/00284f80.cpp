// guest:0x00284f80 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t EndOfHeap(uint32_t q0, ...) {
  syscall(0x3e);
  return 0;
}


RT_GUEST_FN(0x00284f80, guest::EndOfHeap)

}  // namespace guest
