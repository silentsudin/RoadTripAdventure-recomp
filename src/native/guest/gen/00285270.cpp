// guest:00285270 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t sceSifStopDma(uint32_t q0) {
  // W12: sceSifStopDma($4 trid) -> 0; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x6b, q0);
}


RT_GUEST_FN(0x00285270, guest::sceSifStopDma)

}  // namespace guest
