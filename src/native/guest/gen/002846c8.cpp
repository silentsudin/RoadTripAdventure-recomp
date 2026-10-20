// guest:0x002846c8 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: int isinf(undefined8)
// W12: int32_t (bare `int` is invisible to refresh_decls sig tracking; same width)
int32_t isinf(uint64_t param_1, ...) {
  rtguest::FrameGuard rt_fg(16);
  uint uVar1;
  
  uVar1 = 0x7ff00000 -
          ((uint32_t)(param_1 >> 0x20) & 0x7fffffff | ((uint32_t)param_1 | -(uint32_t)param_1) >> 0x1f);
  return ((int)(uVar1 | -uVar1) >> 0x1f) + 1;
}


RT_GUEST_FN(0x002846c8, guest::isinf)

}  // namespace guest
