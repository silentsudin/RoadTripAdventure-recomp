// guest:0x00285bf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t syscallMemInstall(uint32_t q0, ...) {
  // syscall(0x5a) body is kernel-side; args pass through in a0-a2 untouched.
  syscall(0x5a);
  return 0;
}


RT_GUEST_FN(0x00285bf8, guest::syscallMemInstall)

}  // namespace guest
