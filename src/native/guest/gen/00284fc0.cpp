// guest:00284fc0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SignalSema(uint32_t q0) {
  rtguest::FrameGuard rt_fg(0);
  // W12: SignalSema($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x42, q0);
}


RT_GUEST_FN(0x00284fc0, guest::SignalSema)

}  // namespace guest
