// guest:0x00285bf8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t syscallMemInstall(uint32_t q0, uint32_t q1, uint32_t q2) {
  rtguest::FrameGuard rt_fg;
  // W12: Copy($4 dst, $5 src, $6 len); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x5a, q0, q1, q2);
}


RT_GUEST_FN(0x00285bf8, guest::syscallMemInstall)

}  // namespace guest
