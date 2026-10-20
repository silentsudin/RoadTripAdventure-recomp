// guest:00284ba0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SetGsCrt(uint32_t q0, uint32_t q1, uint32_t q2, uint32_t q3, uint32_t q4) {
  rtguest::FrameGuard rt_fg;
  // W12: GsSetCrt($4 ram, $5 voff, $6 reverse, $7 vmode, $8 vwidth-ish shadow); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(2, q0, q1, q2, q3, q4);
}


RT_GUEST_FN(0x00284ba0, guest::SetGsCrt)

}  // namespace guest
