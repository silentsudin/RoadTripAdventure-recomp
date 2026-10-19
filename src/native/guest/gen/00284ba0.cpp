// guest:0x00284ba0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetGsCrt(uint32_t q0, ...) {
  syscall(2);
  return 0;
}


RT_GUEST_FN(0x00284ba0, guest::SetGsCrt)

}  // namespace guest
