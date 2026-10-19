// guest:00284da0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t CreateThread(uint32_t q0) {
  // W12: CreateThread($4 ee_thread_t*); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x20, q0);
}


RT_GUEST_FN(0x00284da0, guest::CreateThread)

}  // namespace guest
