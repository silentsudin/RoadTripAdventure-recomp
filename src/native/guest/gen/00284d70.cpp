// guest:00284d70 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _iDisableDmac(uint32_t q0) {
  // W12: iDisableDmac($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0xffffffffffffffe3, q0);
}


RT_GUEST_FN(0x00284d70, guest::_iDisableDmac)

}  // namespace guest
