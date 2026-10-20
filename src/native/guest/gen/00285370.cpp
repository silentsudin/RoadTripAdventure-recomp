// guest:0x00285370 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t isceSifSetDma(uint32_t q0, uint32_t q1) {
  rtguest::FrameGuard rt_fg(0);
  // W12: isceSifSetDma($4 dmat, $5 count); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffff89, q0, q1);
}


RT_GUEST_FN(0x00285370, guest::isceSifSetDma)

}  // namespace guest
