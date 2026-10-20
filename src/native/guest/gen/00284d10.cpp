// guest:00284d10 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _DisableDmac(uint32_t q0) {
  rtguest::FrameGuard rt_fg;
  // W12: DisableDmac($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x17, q0);
}


RT_GUEST_FN(0x00284d10, guest::_DisableDmac)

}  // namespace guest
