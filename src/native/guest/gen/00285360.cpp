// guest:0x00285360 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSetDma(uint32_t q0, ...) {
  syscall(0x77);
  return 0;
}


RT_GUEST_FN(0x00285360, guest::sceSifSetDma)

}  // namespace guest
