// guest:0x00274b10 translated from ELF: tail `j __ieee754_sqrtf`. Thunk
// forwards f12 (float bits).
#include "../guest_shim.h"
#include "guest_decls.inc"
#include "w13_float_args.h"

namespace guest {

uint32_t sqrtf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  { uint32_t rt_sv = rtguest::frameTop();  // PROLOGUE-SAVES
    *(uint32_t*)rtguest::G(rt_sv+0x0) = rt_fg.rtRA;
    *(uint32_t*)rtguest::G(rt_sv+0x4) = 0x00000000u;
  }
  return __ieee754_sqrtf(param_1);
}


RT_GUEST_FN(0x00274b10, guest::sqrtf)

}  // namespace guest
