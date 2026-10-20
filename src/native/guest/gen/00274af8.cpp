// guest:0x00274af8 translated from ELF: tail `j __ieee754_atan2f` (sd/ld ra
// cancel). Thunk forwards all registers: y/x arrive as f12/f14 float bits.
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t atan2f(uint32_t param_1, uint32_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  return __ieee754_atan2f(param_1, param_2);
}


RT_GUEST_FN(0x00274af8, guest::atan2f)

}  // namespace guest
