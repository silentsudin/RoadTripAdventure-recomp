// guest:0x0027af60 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

// pseudocode: ulong __muldi3(ulong, ulong)
uint64_t __muldi3(uint64_t param_1, uint64_t param_2, ...) {
  rtguest::FrameGuard rt_fg;
  ulong uVar1;
  
  uVar1 = (param_1 & 0xffffffff) * (param_2 & 0xffffffff);
  return uVar1 & 0xffffffff |
         (long)((int)(uVar1 >> 0x20) +
               (int)param_1 * (int)(param_2 >> 0x20) + (int)(param_1 >> 0x20) * (int)param_2) <<
         0x20;
}


RT_GUEST_FN(0x0027af60, guest::__muldi3)

}  // namespace guest
