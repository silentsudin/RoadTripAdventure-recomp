// guest:0x00274b10 translated from ELF: tail `j __ieee754_sqrtf`. Thunk
// forwards f12 (float bits).
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t sqrtf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  return __ieee754_sqrtf(param_1);
}


RT_GUEST_FN(0x00274b10, guest::sqrtf)

}  // namespace guest
