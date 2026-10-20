// guest:002852c0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t GsGetIMR() {
  rtguest::FrameGuard rt_fg(0);
  // W12: GsGetIMR() -> IMR; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x70);
}


RT_GUEST_FN(0x002852c0, guest::GsGetIMR)

}  // namespace guest
