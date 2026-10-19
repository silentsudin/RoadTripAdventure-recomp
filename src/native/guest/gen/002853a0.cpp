// guest:002853a0 transliterated from Ghidra pseudocode (kernel_stubs.json
// deferred set) — W12 port: wired to the host libkernel dispatcher.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

int32_t sceSifSetReg(uint32_t q0, uint32_t q1) {
  // W12: sceSifSetReg($4 reg, $5 val) -> prev; the raw li-v0 number travels to the host
  // dispatcher (guest_kernel.cpp), which models the kernel side.
  return syscall(0x79, q0, q1);
}


RT_GUEST_FN(0x002853a0, guest::sceSifSetReg)

}  // namespace guest
