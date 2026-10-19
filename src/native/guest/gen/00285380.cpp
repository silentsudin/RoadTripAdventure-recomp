// guest:0x00285380 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifSetDChain(uint32_t q0, ...) {
  syscall(0x78);
  return 0;
}


RT_GUEST_FN(0x00285380, guest::sceSifSetDChain)

}  // namespace guest
