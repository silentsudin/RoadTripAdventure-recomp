// guest:00284d00 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _EnableDmac(uint32_t q0) {
  // W12: EnableDmac($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x16, q0);
}


RT_GUEST_FN(0x00284d00, guest::_EnableDmac)

}  // namespace guest
