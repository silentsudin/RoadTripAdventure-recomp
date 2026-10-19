// guest:00284d40 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iEnableIntc(uint32_t q0) {
  // W12: iEnableIntc($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffe6, q0);
}


RT_GUEST_FN(0x00284d40, guest::_iEnableIntc)

}  // namespace guest
