// guest:0x00284dc0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t StartThread(uint32_t q0, uint32_t q1) {
  rtguest::FrameGuard rt_fg(0);
  // W12: StartThread($4 id, $5 arg); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x22, q0, q1);
}


RT_GUEST_FN(0x00284dc0, guest::StartThread)

}  // namespace guest
