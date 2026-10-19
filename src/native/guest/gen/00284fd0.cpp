// guest:0x00284fd0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t iSignalSema(uint32_t q0, ...) {
  syscall(0xffffffffffffffbd);
  return 0;
}


RT_GUEST_FN(0x00284fd0, guest::iSignalSema)

}  // namespace guest
