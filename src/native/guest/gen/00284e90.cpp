// guest:00284e90 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t GetThreadId() {
  // W12: GetThreadId(); the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x2f);
}


RT_GUEST_FN(0x00284e90, guest::GetThreadId)

}  // namespace guest
