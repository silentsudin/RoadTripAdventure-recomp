// guest:00284ce0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _EnableIntc(uint32_t q0) {
  // W12: EnableIntc($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x14, q0);
}


RT_GUEST_FN(0x00284ce0, guest::_EnableIntc)

}  // namespace guest
