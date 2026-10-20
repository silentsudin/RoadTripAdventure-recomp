// guest:00285390 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t isceSifSetDChain(uint32_t q0) {
  rtguest::FrameGuard rt_fg;
  // W12: isceSifSetDChain($4 ptr) -> 0; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffff88, q0);
}


RT_GUEST_FN(0x00285390, guest::isceSifSetDChain)

}  // namespace guest
