// guest:0x00284fa0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t CreateSema(uint32_t q0) {
  // W12: CreateSema($4 ee_sema_t*); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x40, q0);
}


RT_GUEST_FN(0x00284fa0, guest::CreateSema)

}  // namespace guest
