// guest:0x00285340 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sceSifDmaStat(uint32_t q0, ...) {
  syscall(0x76);
  return 0;
}


RT_GUEST_FN(0x00285340, guest::sceSifDmaStat)

}  // namespace guest
