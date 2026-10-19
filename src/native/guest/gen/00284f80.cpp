// guest:00284f80 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t EndOfHeap(uint32_t q0) {
  // W12: EndOfHeap($4 inc -> heap limit); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x3e, q0);
}


RT_GUEST_FN(0x00284f80, guest::EndOfHeap)

}  // namespace guest
