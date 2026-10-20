// guest:00284cb0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t AddDmacHandler(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3) {
  rtguest::FrameGuard rt_fg(0);
  // W12: AddDmacHandler($4 cause, $5 handler, $6 next, $7 argument); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x12, q0, q1, q2, q3);
}


RT_GUEST_FN(0x00284cb0, guest::AddDmacHandler)

}  // namespace guest
