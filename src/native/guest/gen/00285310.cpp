// guest:0x00285310 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t SetVSyncFlag(uint32_t q0, uint32_t q1) {
  rtguest::FrameGuard rt_fg;
  // W12: SetVSyncFlag($4 flag, $5 tick); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x73, q0, q1);
}


RT_GUEST_FN(0x00285310, guest::SetVSyncFlag)

}  // namespace guest
