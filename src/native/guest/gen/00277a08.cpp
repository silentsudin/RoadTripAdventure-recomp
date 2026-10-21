// guest:0x00277a08 transliterated from Ghidra pseudocode — DO NOT hand-edit
#include "../guest_shim.h"
#include "guest_decls.inc"

namespace guest {

uint32_t floorf(uint32_t param_1, ...) {
  rtguest::FrameGuard rt_fg(0);
  uint uVar1;
  uint uVar2;
  
  uVar1 = ((param_1 & 0x7fffffffu) >> 0x17) - 0x7f;  // param_1 = f12 bits
  if ((int)uVar1 < 0x17) {
    if ((int)uVar1 < 0) {
      if (0.0f < rtguest::fbits(param_1) + 1e+30f) {
        if ((int)param_1 < 0) {
          if (0x7fffff < (param_1 & 0x7fffffffu)) {
            param_1 = rtguest::f2bits(-1.0f);
          }
        }
        else {
          param_1 = 0;
        }
      }
    }
    else {
      uVar2 = 0x7fffff >> (uVar1 & 0x1f);
      if ((((uint32_t)param_1 & uVar2) != 0) && (0.0f < rtguest::fbits(param_1) + 1e+30f)) {
        if ((int)param_1 < 0) {
          param_1 = param_1 + (0x800000u >> (uVar1 & 0x1f));
        }
        param_1 = param_1 & ~uVar2;
      }
    }
  }
  return param_1;
}


RT_GUEST_FN(0x00277a08, guest::floorf)

}  // namespace guest
