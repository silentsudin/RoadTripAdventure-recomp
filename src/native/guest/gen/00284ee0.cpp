// guest:00284ee0 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iWakeupThread(uint32_t q0) {
  // W12: iWakeupThread($4 id); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffcc, q0);
}


RT_GUEST_FN(0x00284ee0, guest::_iWakeupThread)

}  // namespace guest
