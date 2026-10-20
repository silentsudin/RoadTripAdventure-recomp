// guest:00285c40 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t getSyscall(uint32_t q0) {
  rtguest::FrameGuard rt_fg(64);
  // W12: GetEntryAddress($4 num) -> table word; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x5b, q0);
}


RT_GUEST_FN(0x00285c40, guest::getSyscall)

}  // namespace guest
