// guest:0x00284ea0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t ReferThreadStatus(uint32_t q0, ...) {
  syscall(0x30);
  return 0;
}


RT_GUEST_FN(0x00284ea0, guest::ReferThreadStatus)

}  // namespace guest
