// guest:00285be8 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t setSyscall(uint32_t q0, uint32_t q1) {
  rtguest::FrameGuard rt_fg;
  // W12: SetSyscall($4 num, $5 handler); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x74, q0, q1);
}


RT_GUEST_FN(0x00285be8, guest::setSyscall)

}  // namespace guest
