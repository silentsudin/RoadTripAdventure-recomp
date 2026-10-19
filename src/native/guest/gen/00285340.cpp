// guest:00285340 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t sceSifDmaStat(uint32_t q0) {
  // W12: sceSifDmaStat($4 trid) -> -1 (transfers immediate); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x76, q0);
}


RT_GUEST_FN(0x00285340, guest::sceSifDmaStat)

}  // namespace guest
