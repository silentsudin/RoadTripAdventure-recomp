// guest:00284ed0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t WakeupThread(uint32_t q0) {
  rtguest::FrameGuard rt_fg;
  // W12: WakeupThread($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x33, q0);
}


RT_GUEST_FN(0x00284ed0, guest::WakeupThread)

}  // namespace guest
