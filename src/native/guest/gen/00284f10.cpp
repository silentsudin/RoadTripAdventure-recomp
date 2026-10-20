// guest:00284f10 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SuspendThread(uint32_t q0) {
  rtguest::FrameGuard rt_fg(0);
  // W12: SuspendThread($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x37, q0);
}


RT_GUEST_FN(0x00284f10, guest::SuspendThread)

}  // namespace guest
