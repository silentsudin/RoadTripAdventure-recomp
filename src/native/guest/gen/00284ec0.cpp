// guest:0x00284ec0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t SleepThread(uint32_t q0, ...) {
  syscall(0x32);
  return 0;
}


RT_GUEST_FN(0x00284ec0, guest::SleepThread)

}  // namespace guest
