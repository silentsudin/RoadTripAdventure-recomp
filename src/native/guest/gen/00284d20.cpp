// guest:00284d20 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SetAlarm(uint32_t q0, uint32_t q1, uint32_t q2) {
  rtguest::FrameGuard rt_fg;
  // W12: SetAlarm($4 ticks[64us], $5 handler, $6 argument); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xfc, q0, q1, q2);
}


RT_GUEST_FN(0x00284d20, guest::SetAlarm)

}  // namespace guest
