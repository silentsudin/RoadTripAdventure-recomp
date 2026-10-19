// guest:0x00284fc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SignalSema(uint32_t q0, ...) {
  syscall(0x42);
  return 0;
}


RT_GUEST_FN(0x00284fc0, guest::SignalSema)

}  // namespace guest
