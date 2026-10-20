// guest:00284d60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iEnableDmac(uint32_t q0) {
  rtguest::FrameGuard rt_fg;
  // W12: iEnableDmac($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffe4, q0);
}


RT_GUEST_FN(0x00284d60, guest::_iEnableDmac)

}  // namespace guest
