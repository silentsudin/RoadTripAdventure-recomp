// guest:002853b0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t sceSifGetReg(uint32_t q0) {
  rtguest::FrameGuard rt_fg;
  // W12: sceSifGetReg($4 reg) -> val; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x7a, q0);
}


RT_GUEST_FN(0x002853b0, guest::sceSifGetReg)

}  // namespace guest
