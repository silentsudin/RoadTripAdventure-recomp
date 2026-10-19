// guest:0x002000b8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t Ps2ExitThreadId(uint32_t q0, ...) {
  syscall(4);
  return 0;
}


RT_GUEST_FN(0x002000b8, guest::Ps2ExitThreadId)

}  // namespace guest
