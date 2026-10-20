// guest:002853d0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t Deci2Call(uint32_t q0, uint32_t q1) {
  rtguest::FrameGuard rt_fg;
  // W12: Deci2Call($4 code, $5 args*); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x7c, q0, q1);
}


RT_GUEST_FN(0x002853d0, guest::Deci2Call)

}  // namespace guest
