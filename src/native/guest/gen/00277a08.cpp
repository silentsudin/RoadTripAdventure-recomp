// guest:0x00277a08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t floorf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint uVar1;
  uint uVar2;
  
  uVar1 = ((uint32_t)ABS(param_1) >> 0x17) - 0x7f;
  if ((int)uVar1 < 0x17) {
    if ((int)uVar1 < 0) {
      if (0.0 < param_1 + 1e+30) {
        if ((int)param_1 < 0) {
          if (0x7fffff < (uint32_t)ABS(param_1)) {
            param_1 = -1.0;
          }
        }
        else {
          param_1 = 0.0;
        }
      }
    }
    else {
      uVar2 = 0x7fffff >> (uVar1 & 0x1f);
      if ((((uint32_t)param_1 & uVar2) != 0) && (0.0 < param_1 + 1e+30)) {
        if ((int)param_1 < 0) {
          param_1 = (float)((int)param_1 + (0x800000 >> (uVar1 & 0x1f)));
        }
        param_1 = (float)((uint32_t)param_1 & ~uVar2);
      }
    }
  }
  return param_1;
}


RT_GUEST_FN(0x00277a08, guest::floorf)

}  // namespace guest
