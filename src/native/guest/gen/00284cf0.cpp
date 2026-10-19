// guest:00284cf0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t _DisableIntc(uint32_t q0) {
  // W12: DisableIntc($4 cause); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x15, q0);
}


RT_GUEST_FN(0x00284cf0, guest::_DisableIntc)

}  // namespace guest
