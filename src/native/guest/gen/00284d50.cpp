// guest:00284d50 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iDisableIntc(uint32_t q0) {
  rtguest::FrameGuard rt_fg(0);
  // W12: iDisableIntc($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffe5, q0);
}


RT_GUEST_FN(0x00284d50, guest::_iDisableIntc)

}  // namespace guest
