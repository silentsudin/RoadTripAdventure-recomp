// guest:0x00284fb0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t DeleteSema(uint32_t q0) {
  // W12: DeleteSema($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x41, q0);
}


RT_GUEST_FN(0x00284fb0, guest::DeleteSema)

}  // namespace guest
