// guest:0x00285270 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifStopDma(uint32_t q0, ...) {
  syscall(0x6b);
  return 0;
}


RT_GUEST_FN(0x00285270, guest::sceSifStopDma)

}  // namespace guest
