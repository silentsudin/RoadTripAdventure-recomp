// guest:00284e50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t RotateThreadReadyQueue(uint32_t q0, ...) {
  syscall(0x2b);
  return 0;
}


RT_GUEST_FN(0x00284e50, guest::RotateThreadReadyQueue)

}  // namespace guest
