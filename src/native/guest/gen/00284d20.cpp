// guest:0x00284d20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SetAlarm(uint32_t q0, ...) {
  syscall(0xfc);
  return 0;
}


RT_GUEST_FN(0x00284d20, guest::SetAlarm)

}  // namespace guest
