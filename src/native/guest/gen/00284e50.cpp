// guest:00284e50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t RotateThreadReadyQueue(uint32_t q0) {
  rtguest::FrameGuard rt_fg(0);
  // W12: RotateThreadReadyQueue($4 prio); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x2b, q0);
}


RT_GUEST_FN(0x00284e50, guest::RotateThreadReadyQueue)

}  // namespace guest
