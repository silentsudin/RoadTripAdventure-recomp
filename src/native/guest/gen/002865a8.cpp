// guest:0x002865a8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint64_t sioPutChar(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg;
  do {
  } while ((REG_SIO_ISR & 0x8000) != 0);
  REG_SIO_TXFIFO = (char)param_1;
  return param_1;
}


RT_GUEST_FN(0x002865a8, guest::sioPutChar)

}  // namespace guest
