// guest:00284ec0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SleepThread() {
  rtguest::FrameGuard rt_fg;
  // W12: SleepThread(); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x32);
}


RT_GUEST_FN(0x00284ec0, guest::SleepThread)

}  // namespace guest
