// guest:00284f20 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iSuspendThread(uint32_t q0) {
  rtguest::FrameGuard rt_fg(0);
  // W12: iSuspendThread($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffc8, q0);
}


RT_GUEST_FN(0x00284f20, guest::_iSuspendThread)

}  // namespace guest
