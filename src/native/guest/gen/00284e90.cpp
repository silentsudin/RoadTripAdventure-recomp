// guest:0x00284e90 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t GetThreadId(uint32_t q0, ...) {
  syscall(0x2f);
  return 0;
}


RT_GUEST_FN(0x00284e90, guest::GetThreadId)

}  // namespace guest
