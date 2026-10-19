// guest:002000b8 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t Ps2ExitThreadId(uint32_t q0) {
  // W12: ExitThread() [pseudocode comment]; call sites carry a leftover $4
  // (abort 0027db20 passes 1) which the stub forwards and the kernel ignores.
  return syscall(4, q0);
}


RT_GUEST_FN(0x002000b8, guest::Ps2ExitThreadId)

}  // namespace guest
