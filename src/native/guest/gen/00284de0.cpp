// guest:0x00284de0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t ExitDeleteThread() {
  rtguest::FrameGuard rt_fg(0);
  // W12: ExitDeleteThread(no args); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x24);
}


RT_GUEST_FN(0x00284de0, guest::ExitDeleteThread)

}  // namespace guest
